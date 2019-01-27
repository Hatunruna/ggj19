#ifndef HOME_CRASHED_SHIP_H
#define HOME_CRASHED_SHIP_H

#include <gf/Entity.h>
#include <gf/Message.h>
#include <gf/Rect.h>

namespace home {
  class CrashedShip: public gf::Entity {
  public:
    CrashedShip();

    // virtual void render(gf::RenderTarget& target, const gf::RenderStates& states) override;
    virtual void update(gf::Time time) override;

    gf::MessageStatus onHeroPosition(gf::Id id, gf::Message *msg);

  private:
    gf::RectF m_hitbox;
    gf::Vector2f m_heroLocation;
    float m_oxygenLevel;
    float m_cristalQuantity;
    float m_metalQuantity;
  };
}

#endif // HOME_CRASHED_SHIP_H
