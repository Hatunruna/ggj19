#include "Player.h"

#include <gf/Log.h>
#include <gf/RenderTarget.h>
#include <gf/Shapes.h>
#include <gf/VectorOps.h>

#include "Singletons.h"

namespace home {
  static constexpr float Velocity = 200.0f;
  static constexpr float OxygenLoss = 0.5f;

  static constexpr float Radius = 20.0f;

  Player::Player()
  : m_position({0.0f, 0.0f})
  , m_positionClicked({0.0f, 0.0f})
  , m_oxygen(100.0f) {
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
    gf::Vector2f move = m_positionClicked - m_position;
    float length = gf::euclideanLength(move);

    if (length > time.asSeconds() * Velocity) {
      // Update player position according to where the mouse is clicked
      m_position += (move / length) * time.asSeconds() * Velocity;
    } else {
      m_position += move;
    }

    m_oxygen -= time.asSeconds() * OxygenLoss;
    if (m_oxygen > 0) {
      gf::Log::info("Oxygen: %f\n", m_oxygen);
    } else {
      gf::Log::info("Player is dead. RIP in peace.\n");
    }

    HeroPosition message;
    message.position = m_position;
    gMessageManager().sendMessage(&message);
  }

  gf::MessageStatus Player::onMouseClicked(gf::Id id, gf::Message *msg) {
    assert(id == CursorClickedPosition::type);
    // store mouse coordinates
    m_positionClicked = static_cast<CursorClickedPosition*>(msg)->position;

    // gf::Log::debug("message position: %f, %f\n", message->position.x, message->position.y);
    return gf::MessageStatus::Keep;
  }
}
