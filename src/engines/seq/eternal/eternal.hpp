#pragma once

#include "core/engines/engine.hpp"

#include "core/audio/faust.hpp"
#include "core/audio/voice_manager.hpp"

#include <array>
#include <vector>
#include <optional>

namespace otto::engines {

  using namespace core;
  using namespace core::engines;
  using namespace props;

  struct Eternal : SequencerEngine {
    static constexpr int max_length = 64;
    static constexpr int midi_max = 127;

    struct Channel : Properties<> {
      std::vector<char> sequence;

      Property<int> length = {this, "Length", 0, has_limits::init(0, max_length), 
                                    steppable::init(1)};
      Property<int> start = {this, "Start", 0, has_limits::init(0, max_length),
                                    steppable::init(1)};
      Property<int, wrap> sequence_time = {this, "Sequence Time", 2, has_limits::init(0,4), steppable::init(1)};

      struct Props : Properties<> {
        Property<int, wrap> current_beat = {this, "Current Beat", 0, has_limits::init(0, max_length)};
        Property<char, wrap> current_note = {this, "Current Note", -1, has_limits::init(-1, midi_max), steppable::init(1)};
        Property<int, wrap> note_time = {this, "Note Time", 2, has_limits::init(0, 4), steppable::init(1)};
      } props;

      void delete_sequence() {
        sequence.clear();
      }

      void add_note(char note) {
        sequence.push_back(note);
        props.current_beat;
      }

    } channel;



    Eternal();

    audio::ProcessData<0> process(audio::ProcessData<0>) override;

    bool running = false;
  

  private:
    friend struct EternalScreen;

    int _samples_per_beat = 22050;
    int _counter = _samples_per_beat;
    bool _should_run = false;

    bool _has_pressed_keys = false;
  };
} // namespace otto::engines