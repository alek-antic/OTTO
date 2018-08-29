#include "eternal.hpp"

#include "core/globals.hpp"
#include "core/ui/vector_graphics.hpp"

#include "util/cache.hpp"
#include "util/iterator.hpp"
#include "util/utility.hpp"

#include "services/logger.hpp"

namespace otto::engines {

    using namespace ui;
    using namespace ui::vg;

    using Channel = Eternal::Channel;

    struct EternalScreen : EngineScreen<Eternal> {
        using EngineScreen<Eternal>::EngineScreen;

        void draw(Canvas& ctx) override;
        bool keypress(Key key) override;
        void rotary(RotaryEvent e) override;

        void draw_normal(Canvas & ctx);
        void draw_recording(Canvas& ctx);

        struct State {
            Point center = {127, 120};

            struct ChannelState {
                Channel* channel;
                float radius = 40;
                Colour color = Colours::Gray70;
                int length = 0;
                
            };
            std::array<ChannelState, 4> channels;
        } state;

        void refresh_state();

        void draw_channel(ui::vg::Canvas & ctx, State::ChannelState & chan);
    };

    Eternal::Eternal() : SequencerEngine("Eternal", props, std::make_unique<EternalScreen>(this)) {
        
    }

    audio::ProcessData<0> Eternal::process(audio::ProcessData<0> data) {
        if (recording) {
            for (auto& event : data.midi) {
                util::match(event,
                [&](midi::NoteOnEvent& ev) {
                    if(!_has_pressed_keys) {
                        util::fill(recording.value(), -1);
                        _has_pressed_keys = true;
                    }
                    for (auto& note : recording.value()) {
                        if (note != ev.key) continue;
                        note = -1;
                    }
                    if (util::all_of(recording.value(), [](char note) { return note < 0; })) {
                        recording = std::nullopt;
                    }
                },
                [](auto&&) {});
            }
        }
        if (_should_run) running = true;
        if (!running) return data;

        if (!_should_run && running) {
            _counter = _samples_per_beat;
            running = false;
            return data;
        }

        auto next_beat = _samples_per_beat - _counter;

        if (next_beat <= data.nframes) {
            for (auto& channel : props.channels) {
                if (channel.length > 0) {
                    channel._beat_counter++;
                    
                }
            }
        }

        _counter += data.nframes;
        _counter %= _samples_per_beat;
        return data;
    }

    void Eternal::Channel::delete_sequence() {
        sequence.clear();
    }

    void Eternal::Channel::add_note(char note) {
        sequence.push_back(note);
    }

    bool EternalScreen::keypress(ui::Key key) {
        switch (key) {
            case ui::Key::red_click: 
                // start recording
                break;
            case ui::Key::white_click:
                // delete sequence
                break;
            
            
        }
        return true;
    }
}