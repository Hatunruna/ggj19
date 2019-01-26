#include "Player.h"

#include <gf/Log.h>
#include <gf/RenderTarget.h>
#include <gf/Shapes.h>
#include <gf/VectorOps.h>

#include "Singletons.h"

namespace home {
  static constexpr float Velocity = 200.0f;
  static constexpr float Radius = 20.0f;
  static constexpr gf::Vector2f TileSize = {128, 64};

  gf::Orientation getOrientation(float angle) {
    constexpr float Pi8 = gf::Pi / 8.0f;

    if (-7.0f * Pi8 <= angle && angle < -5.0f * Pi8) {
      return gf::Orientation::NorthWest;
    } else if (-5.0f * Pi8 <= angle && angle < -3.0f * Pi8) {
      return gf::Orientation::North;
    } else if (-3.0f * Pi8 <= angle && angle < -1.0f * Pi8) {
      return gf::Orientation::NorthEast;
    } else if (-1.0f * Pi8 <= angle && angle < 1.0f * Pi8) {
      return gf::Orientation::East;
    } else if (1.0f * Pi8 <= angle && angle < 3.0f * Pi8) {
      return gf::Orientation::SouthEast;
    } else if (3.0f * Pi8 <= angle && angle < 5.0f * Pi8) {
      return gf::Orientation::South;
    } else if (5.0f * Pi8 <= angle && angle < 7.0f * Pi8) {
      return gf::Orientation::SouthWest;
    }
    return gf::Orientation::West;
  }

  Player::Player()
  : gf::Entity(100)
  , m_position(TileSize * gf::Vector2f(41.0f, 91.0f))
  , m_positionClicked(TileSize * gf::Vector2f(41.0f, 91.0f))
  , m_jetSound(gResourceManager().getSound("sounds/jet_engine.ogg"))
  , m_wasJetSound(false)
  , m_orientation(gf::Orientation::East)
  , m_moving(false)
  , m_moveAndPauseTexture(gResourceManager().getTexture("images/player/move_pause_animation.png"))
  , m_currentAnimation(nullptr)
   {
    gMessageManager().registerHandler<CursorClickedPosition>(&Player::onMouseClicked, this);

    m_jetSound.setLoop(true);
    m_jetSound.setVolume(100.0f);

    // load animations
    loadAnimation(m_move_west, 0);
    loadAnimation(m_move_north_west, 1);
    loadAnimation(m_move_north, 2);
    loadAnimation(m_move_north_east, 3);
    loadAnimation(m_move_east, 4);
    loadAnimation(m_move_south_east, 5);
    loadAnimation(m_move_south_west, 6);

    loadAnimation(m_pause_west, 7);
    loadAnimation(m_pause_north_west, 8);
    loadAnimation(m_pause_north, 9);
    loadAnimation(m_pause_north_east, 10);
    loadAnimation(m_pause_east, 11);
    loadAnimation(m_pause_south_east, 12);
    loadAnimation(m_pause_south_west, 13);

    loadAnimation(m_south, 14);
  }

  void Player::render(gf::RenderTarget& target, const gf::RenderStates& states) {
    gf::CircleShape player;
    player.setColor(gf::Color::Blue);
    player.setRadius(Radius);
    player.setPosition(m_position);
    player.setAnchor(gf::Anchor::Center);
    target.draw(player, states);

    gf::AnimatedSprite sprite;
    sprite.setAnimation(*m_currentAnimation);
    sprite.setScale(0.75f);
    sprite.setPosition(m_position);
    sprite.setAnchor(gf::Anchor::Center);
    target.draw(sprite, states);
  }

  void Player::update(gf::Time time) {
    //gf::Log::debug("player position: %f, %f\n", m_position.x, m_position.y);
    gf::Vector2f move = m_positionClicked - m_position;
    float length = gf::euclideanLength(move);

    if (length > time.asSeconds() * Velocity) {
      // Update player position according to where the mouse is clicked
      m_position += (move / length) * time.asSeconds() * Velocity;
      m_orientation = getOrientation(gf::angle(move));
      m_moving = true;
    } else {
      m_position += move;
      m_moving = false;
    }

    if (length > 0.0f && !m_wasJetSound) {
      m_wasJetSound = true;
      m_jetSound.play();
    } else if (length <= 0.0f && m_wasJetSound) {
      m_jetSound.stop();
      m_wasJetSound = false;
    }

    // Determine the animation
    switch (m_orientation) {
      case gf::Orientation::NorthWest:
        if (m_moving) {
          m_currentAnimation = &m_move_north_west;
        } else {
          m_currentAnimation = &m_pause_north_west;
        }
        break;
      case gf::Orientation::North:
        if (m_moving) {
          m_currentAnimation = &m_move_north;
        } else {
          m_currentAnimation = &m_pause_north;
        }
        break;
      case gf::Orientation::NorthEast:
        if (m_moving) {
          m_currentAnimation = &m_move_north_east;
        } else {
          m_currentAnimation = &m_pause_north_east;
        }
        break;
      case gf::Orientation::East:
        if (m_moving) {
          m_currentAnimation = &m_move_east;
        } else {
          m_currentAnimation = &m_pause_east;
        }
        break;
      case gf::Orientation::SouthEast:
        if (m_moving) {
          m_currentAnimation = &m_move_south_east;
        } else {
          m_currentAnimation = &m_pause_south_east;
        }
        break;
      case gf::Orientation::South:
        m_currentAnimation = &m_south;
        break;
      case gf::Orientation::SouthWest:
        if (m_moving) {
          m_currentAnimation = &m_move_south_west;
        } else {
          m_currentAnimation = &m_pause_south_west;
        }
        break;
      case gf::Orientation::West:
        if (m_moving) {
          m_currentAnimation = &m_move_west;
        } else {
          m_currentAnimation = &m_pause_west;
        }
        break;
      default:
        assert(false);
        break;
    }

    // Update sprite
    m_currentAnimation->update(time);

    HeroPosition message;
    message.position = m_position;
    gMessageManager().sendMessage(&message);
  }

  gf::MessageStatus Player::onMouseClicked(gf::Id id, gf::Message *msg) {
    assert(id == CursorClickedPosition::type);
    // store mouse coordinates
    m_positionClicked = static_cast<CursorClickedPosition*>(msg)->position;

    return gf::MessageStatus::Keep;
  }

  void Player::loadAnimation(gf::Animation &animation, int line) {
    static constexpr gf::Vector2f TextureSize = {5632.0f, 3840.0f};
    static constexpr gf::Vector2f FrameSize = {256.0f, 256.0f};
    static constexpr gf::Time frameTiming = gf::seconds(1.0f / 30.0f);

    for (int i = 0; i < 22; ++i) {
      gf::RectF frame({i * FrameSize.width / TextureSize.width, line * FrameSize.height / TextureSize.height}, FrameSize / TextureSize);

      animation.addFrame(m_moveAndPauseTexture, frame, frameTiming);
    }
  }
}
