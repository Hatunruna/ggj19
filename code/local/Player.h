#ifndef HOME_PLAYER_H
#define HOME_PLAYER_H

#include <gf/Entity.h>
#include "Messages.h"

namespace home {
  class Player : public gf::Entity {
    public :
      Player();
      virtual void render(gf::RenderTarget& target, const gf::RenderStates& states) override;
      virtual void update(gf::Time time) override;
      gf::MessageStatus onMouseClicked(gf::Id id, gf::Message *msg);
    private :
      gf::Vector2f m_position;
      gf::Vector2f m_positionClicked;
  };
}

#endif // HOME_PLAYER_H