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

    struct Sequence : Properties<> {
      std::vector<char> notes;

      bool add_note(char note) {
        if (notes.size() < max_length) {
          notes.push_back(note);
          return true;
        }
        return false;
      };

      void delta_note(int ind, char delta) {
        notes[ind] += delta;

        // wrap the note around the ends
        if (notes[ind] < 0) {
          notes[ind] = midi_max - notes[ind];
        } else if (notes[ind] > midi_max) {
          notes[ind] = notes[ind] - midi_max;
        }
      }

      void delete_note(int ind) {
        if (notes[ind]) {
          notes[ind] = -1;
        }
      }
      
      
    } sequence;

    struct Props : Properties<> {
      Property<int, wrap> current_beat = {this, "Current Beat", 0, has_limits::init(0, max_length),
                                          steppable::init(1)};
      
    } props;

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