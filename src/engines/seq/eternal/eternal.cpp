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
    static_cast<EternalScreen*>(&screen())->refresh_state();
  }

  audio::ProcessData<0> Eternal::process(audio::ProcessData<0> data) {

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