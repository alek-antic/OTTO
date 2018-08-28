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

    struct Endless : SequencerEngine {
        
        struct Channel : Properties<> {
            Property<int> length = {this, "Length", 0, steppable::init(1)};
            
            std::array<char, 6> notes = {{-1, -1, -1 ,-1 , -1, -1}};

            void delete_sequence();
            void add_note(char note);

            Channel(properties_base* prnt, int n) : Properties(prnt, std::to_string(n)){};

            int _beat_counter = 0;
            std::vector<char> sequence;
        } channel;

        struct Props : Properties<> {
            Property<int> channel = {this, "Channel", 0, has_limits::init(0,3)};
            Properties<> channels_props = {this, "Channels"};
            std::array<Channel, 4> channels = util::generate_array<4>([this](int) { return Channel(this, n); });
        } props;

        Endless();

        audio::ProcessData<0> process(audi::ProcessData<0>) override;

        Channel & current_channel() {
            return props.channels.at(props.channel);
        }

        std::optional<std::array<char, 6>> recording = std::nullopt;

        bool running = false;
    

    private:
        friend struct EndlessScreen;

        int _samples_per_beat = 22050;
        int _counter = _samples_per_beat;
        bool _should_run = false;

        bool _has_pressed_keys = false;
    }
} // namespace otto::engines