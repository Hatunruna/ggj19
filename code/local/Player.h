#ifndef HOME_PLAYER_H
#define HOME_PLAYER_H

#include <gf/AnimatedSprite.h>
#include <gf/Animation.h>
#include <gf/Orientation.h>
#include <gf/Entity.h>

#include <SFML/Audio.hpp>

#include "Messages.h"

namespace home {
  class Player : public gf::Entity {
    public :
      Player();

      void setPosition(gf::Vector2f position) {
        m_position = position;
      }

      gf::Vector2f getPosition() const {
        return m_position;
      }

      virtual void render(gf::RenderTarget& target, const gf::RenderStates& states) override;
      virtual void update(gf::Time time) override;
      // Mouse click handler
      gf::MessageStatus onMouseClicked(gf::Id id, gf::Message *msg);
      gf::MessageStatus onHarvestResource(gf::Id id, gf::Message *msg);

    private:
      void loadAnimation(gf::Animation &animation, int line);

    private :
      gf::Vector2f m_position; // Current position of the player
      gf::Vector2f m_positionClicked; // Where the mouse was last clicked

      sf::Sound m_jetSound;
      bool m_wasJetSound;

      gf::Orientation m_orientation;
      bool m_moving;
      bool m_overSupply;

      gf::Texture &m_moveAndPauseTexture;

      gf::Animation *m_currentAnimation;

      gf::Animation m_move_west;
      gf::Animation m_move_north_west;
      gf::Animation m_move_north;
      gf::Animation m_move_north_east;
      gf::Animation m_move_east;
      gf::Animation m_move_south_east;
      gf::Animation m_move_south_west;

      gf::Animation m_pause_west;
      gf::Animation m_pause_north_west;
      gf::Animation m_pause_north;
      gf::Animation m_pause_north_east;
      gf::Animation m_pause_east;
      gf::Animation m_pause_south_east;
      gf::Animation m_pause_south_west;

      gf::Animation m_south;

      gf::Animation m_harvest_north_west;
      gf::Animation m_harvest_north_east;
      gf::Animation m_harvest_south_east;
      gf::Animation m_harvest_south_west;
  };
}

#endif // HOME_PLAYER_H
