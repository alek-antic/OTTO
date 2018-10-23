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

    struct State {
      Point center = {127, 120};

      struct ChannelState {
        Channel* channel;
        float radius = 40;
        Colour color = Colours::Gray70;
        int length = 0;
        
        int min_note_idx = 0;
        int max_note_idx = 0;
      };
    } state;

    void refresh_state();
  };

  Eternal::Eternal() : SequencerEngine("Eternal", props, std::make_unique<EternalScreen>(this)) {
    static_cast<EternalScreen*>(&screen())->refresh_state();
  }

  audio::ProcessData<0> Eternal::process(audio::ProcessData<0> data) {
    for (auto& event : data.midi) {
      util::match(event,
                  [&] (midi::NoteOnEvent& ev) { // handle when a note is pressed
                      engine.channel.add_note(ev.key);
                  },
                  [&] (midi::NoteOffEvent& ev){ // handle when a note is released
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
      if (engine.props.current_beat.get() >= channel.length.get()) {
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
      case Rotary::Green: //go back/forth through sequence
        engine.delta(ev.clicks);
      break;
    }
    refresh_state();
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

  void EternalScreen::refresh_state() {
    auto& props = engine.props;
    
    state.min_note_idx = std::max(0, props.current_beat.get() - max_beats / 2);
    state.max_note_idx = std::min(props.current_beat.get() + max_beats / 2, engine.channel.sequence.length);
  }

  void EternalScreen::draw(ui::vg::Canvas& ctx) {

  }
}