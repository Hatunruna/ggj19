#include "Player.h"

#include <gf/Log.h>
#include <gf/RenderTarget.h>
#include <gf/Shapes.h>
#include <gf/VectorOps.h>

#include "Singletons.h"

namespace home {
    static constexpr float Velocity = 20.0f;
    static constexpr float Radius = 5.0f;
    Player::Player() 
    : m_position({0.0f, 0.0f})
    , m_positionClicked({0.0f, 0.0f}) {
      gMessageManager().registerHandler<CursorClickedPosition>(&Player::onMouseClicked, this);
    }

    void Player::render(gf::RenderTarget& target, const gf::RenderStates& states) {
      gf::CircleShape player;      
      player.setColor(gf::Color::Blue);
      player.setRadius(Radius);
      player.setPosition(m_position);
      player.setAnchor(gf::Anchor::Center);
      target.draw(player, states);
    }
    
    void Player::update(gf::Time time) {
      //gf::Log::debug("player position: %f, %f\n", m_position.x, m_position.y);
      if (gf::squareDistance(m_positionClicked, m_position) > Radius) {
        m_position += gf::normalize(m_positionClicked - m_position) * time.asSeconds() * Velocity;
      }
    }

    gf::MessageStatus Player::onMouseClicked(gf::Id id, gf::Message *msg) {
      assert(id == CursorClickedPosition::type);
      CursorClickedPosition *message = static_cast<CursorClickedPosition*>(msg);
      m_positionClicked = message->position;

      // gf::Log::debug("message position: %f, %f\n", message->position.x, message->position.y);
      return gf::MessageStatus::Keep;
    }   
}