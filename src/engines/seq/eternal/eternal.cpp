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

    static constexpr int max_beats = 24;

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
    } state;

    void refresh_state();

    void draw(ui::vg::Canvas & ctx);
  };

  Eternal::Eternal() : SequencerEngine("Eternal", props, std::make_unique<EternalScreen>(this)) {
    static_cast<EternalScreen*>(&screen())->refresh_state();
  }

  audio::ProcessData<0> Eternal::process(audio::ProcessData<0> data) {
    for (auto& event : data.midi) {
      bool keyReleased = true;
      util::match(event,
                  [&] (midi::NoteOnEvent& ev) { // handle when a note is pressed
                    if (keyReleased) {
                      engine.channel.add_note(ev.key);
                      keyReleased = false;
                    }
                  },
                  [&] (midi::NoteOffEvent& ev){ // handle when a note is released
                    keyReleased = true;
                  },
                  [](auto&&){} //handle all other cases
                  );
    }

    // check if the engine should play the sounds in the data

    if (_should_run) running = true;
    if (!running) return data;

    if (!_should_run && running) {
      _counter = _samples_per_beat;
      running = false;
      return data;
    }

    // _should_run && running
    auto next_beat = _samples_per_beat - _counter;
    auto& channel = engine.channel;
    if (next_beat <= data.nframes) {
      if (engine.props.current_beat >= channel.length) {
        running = false;
      } else if (channel.length > 0) {
        auto note = engine.props.get_current_note();
        data.midi.push_back(midi::NoteOnEvent(note));
        data.midi.push_back(midi::NoteOffEvent(note));
      }
    }

    _counter += data.nframes;
    _counter %= _samples_per_beat;
    return data;
  }


  // SCREEN //

  void EternalScreen::rotary(ui::RotaryEvent ev) {

    switch(ev.rotary) {
      
    }
  }

  bool EternalScreen::keypress(ui::Key key) {
    switch (key) {
      case ui::Key::white_click:
        // delete sequence
        engine.channel.delete_sequence();
        break;
      
    }
    return true;
  }
}