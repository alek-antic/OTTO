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
            Property<int> length = {this, "Length", 0, steppable::init(1), has_limits::init(0, max_length)};
            
            void delete_sequence();
            void add_note(char note);

            int _beat_counter = 0;
            std::vector<char> sequence;
        } channel;

        struct Props : Properties<> {
            Property<int> max_beat = {this, "Current Beat", 0, has_limits::init(0, max_length)};
        } props;

        Eternal();

        audio::ProcessData<0> process(audio::ProcessData<0>) override;

        std::optional<std::array<char, 6>> recording = std::nullopt;

        bool running = false;
    

    private:
        friend struct EternalScreen;

        int _samples_per_beat = 22050;
        int _counter = _samples_per_beat;
        bool _should_run = false;

        bool _has_pressed_keys = false;
    };
} // namespace otto::engines