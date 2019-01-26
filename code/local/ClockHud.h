#ifndef HOME_CLOCK_HUD_H
#define HOME_CLOCK_HUD_H

#include <gf/Entity.h>
#include <gf/Font.h>
#include <gf/Message.h>

namespace home {
  class ClockHud: public gf::Entity {
  public:
    ClockHud();

    virtual void render(gf::RenderTarget& target, const gf::RenderStates& states) override;

    gf::MessageStatus onClockInfo(gf::Id id, gf::Message *msg);

  private:
    gf::Font &m_font;
    float m_inGameTime;
    int m_numDays;
  };
}

#endif // HOME_CLOCK_HUD_H
