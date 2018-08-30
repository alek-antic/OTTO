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
    struct Channel : Properties<> {
      Property<int> length = {this, "Length", 0, has_limits::init(0, max_length), steppable::init(1)};
      
      void delete_sequence() {
        sequence.clear();
      }

      void add_note(char note) {
        sequence.push_back(note);
        props.current_beat++;
      }

      std::vector<char> sequence;
    } channel;

    struct Props : Properties<> {
      Property<int> current_beat = {this, "Current Beat", 0, has_limits::init(0, max_length)};

      char get_current_note() {
        return channel.sequence[current_beat];
      }
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