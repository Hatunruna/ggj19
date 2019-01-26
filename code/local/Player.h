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
    // Mouse click handler
      gf::MessageStatus onMouseClicked(gf::Id id, gf::Message *msg);
    private :
      gf::Vector2f m_position; // Current position of the player
      gf::Vector2f m_positionClicked; // Where the mouse was last clicked
      float m_oxygen;
  };
}

#endif // HOME_PLAYER_H