#include "Player.h"

#include <gf/Log.h>
#include <gf/RenderTarget.h>
#include <gf/Shapes.h>
#include <gf/Unused.h>
#include <gf/VectorOps.h>

#include "Singletons.h"

namespace home {
  static constexpr float Velocity = 200.0f;
  static constexpr float Radius = 20.0f;
  static constexpr gf::Vector2f TileSize = {128, 64};

  static constexpr float SFXVol = 75.0f;

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

  gf::Orientation getHarvestOrientation(float angle) {
    constexpr float Pi4 = gf::Pi / 4.0f;

    if (-4.0f * Pi4 <= angle && angle < -2.0f * Pi4) {
      return gf::Orientation::NorthWest;
    } else if (-2.0f * Pi4 <= angle && angle < 0.0f * Pi4) {
      return gf::Orientation::NorthEast;
    } else if (0.0f * Pi4 <= angle && angle < 2.0f * Pi4) {
      return gf::Orientation::SouthEast;
    }
    return gf::Orientation::SouthWest;
  }

  Player::Player()
  : gf::Entity(100)
  , m_position(TileSize * gf::Vector2f(45.0f, 42.0f))
  , m_positionClicked(TileSize * gf::Vector2f(45.0f, 42.0f))
  , m_jetSound(gResourceManager().getSound("sounds/jet_engine.ogg"))
  , m_wasJetSound(false)
  , m_deathSound(gResourceManager().getSound("sounds/death.ogg"))
  , m_wasDeathSound(false)
  , m_orientation(gf::Orientation::SouthEast)
  , m_moving(false)
  , m_overSupply(false)
  , m_shipFound(false)
  , m_dead(false)
  , m_moveAndPauseTexture(gResourceManager().getTexture("images/player/player_animations.png"))
  , m_deathTexture(gResourceManager().getTexture("images/player/player_death.png"))
  , m_currentAnimation(nullptr)
  , m_crosshairTexture(gResourceManager().getTexture("crosshair.png"))
  , m_gameOverImg(gResourceManager().getTexture("images/game_over.png"))
  , m_gameOverAlpha(0.0f)
  {
    gMessageManager().registerHandler<CursorClickedPosition>(&Player::onMouseClicked, this);
    gMessageManager().registerHandler<HarvestResource>(&Player::onHarvestResource, this);
    gMessageManager().registerHandler<GameOver>(&Player::onGameOver, this);

    m_jetSound.setLoop(true);
    m_jetSound.setVolume(SFXVol);

    m_deathSound.setVolume(SFXVol);

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

    loadAnimation(m_harvest_north_west, 15);
    loadAnimation(m_harvest_north_east, 16);
    loadAnimation(m_harvest_south_east, 17);
    loadAnimation(m_harvest_south_west, 18);

    loadAnimationDeath();
  }

  void Player::render(gf::RenderTarget& target, const gf::RenderStates& states) {
    gf::Sprite cross;
    cross.setTexture(m_crosshairTexture);
    cross.setPosition(m_positionClicked);
    cross.setScale({ 0.5f, 0.25f });
    cross.setAnchor(gf::Anchor::Center);
    target.draw(cross, states);

    gf::AnimatedSprite sprite;
    sprite.setAnimation(*m_currentAnimation);
    sprite.setScale(0.75f);
    sprite.setPosition(m_position);
    sprite.setAnchor(gf::Anchor::Center);
    target.draw(sprite, states);

    if (m_dead) {
      gf::Sprite goSprite;
      goSprite.setTexture(m_gameOverImg);
      goSprite.setColor({1.0f, 1.0f, 1.0f, m_gameOverAlpha});
      goSprite.setScale(0.55f);
      goSprite.setAnchor(gf::Anchor::Center);
      goSprite.setPosition(m_position);
      target.draw(goSprite, states);
    }
  }

  void Player::update(gf::Time time) {
    if (!m_dead) {
      gf::Vector2f move = m_positionClicked - m_position;
      float length = gf::euclideanLength(move);

      if (length > time.asSeconds() * Velocity) {
        // Update player position according to where the mouse is clicked
        //      m_position += (move / length) * time.asSeconds() * Velocity;
        if (m_overSupply) {
          m_orientation = getHarvestOrientation(gf::angle(move));
        } else {
          m_orientation = getOrientation(gf::angle(move));
        }

        m_velocity = (move / length) * Velocity;
        m_moving = true;
      } else {
        m_position += move;
        m_velocity = { 0.0f, 0.0f };
        m_moving = false;
      }

      if (length > 0.1f && !m_wasJetSound) {
        m_wasJetSound = true;
        m_jetSound.play();
      } else if (length <= 0.1f && m_wasJetSound) {
        m_jetSound.stop();
        m_wasJetSound = false;
      }

      // Determine the animation
      if (m_overSupply && !m_moving) {

        // gf::Log::debug("harvest\n");
        switch (m_orientation) {
          case gf::Orientation::NorthWest:
          m_currentAnimation = &m_harvest_north_west;
          break;
          case gf::Orientation::NorthEast:
          m_currentAnimation = &m_harvest_north_east;
          break;
          case gf::Orientation::SouthEast:
          m_currentAnimation = &m_harvest_south_east;
          break;
          case gf::Orientation::SouthWest:
          m_currentAnimation = &m_harvest_south_west;
          break;
          default:
          // assert(false);
          break;
        }
      }
      else {
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
      }
    }

    // Update sprite
    m_currentAnimation->update(time);
    if (!m_shipFound && m_position.x > 11020.0f && m_position.x < 11750.0f && m_position.y > 8480.0f && m_position.y < 9121.0f) {
      m_shipFound = true;
      MessageToDisplay msg;
      msg.message = "You found a spaceship!";
      msg.displayTime = 4.0f;
      gMessageManager().sendMessage(&msg);
    }
    if (m_position.x > 11310.0f && m_position.x < 11685.0f && m_position.y < 8865.0f && m_position.y > 8680.0f) {
      Victory msg;
      gMessageManager().sendMessage(&msg);
    }
    HeroPosition message;
    message.position = m_position;
    gMessageManager().sendMessage(&message);
    m_overSupply = false;

    if (m_dead) {
      m_gameOverAlpha = m_gameOverAlpha >= 1.0f ? 1.0f : m_gameOverAlpha + time.asSeconds();
    }
  }

  gf::MessageStatus Player::onMouseClicked(gf::Id id, gf::Message *msg) {
    assert(id == CursorClickedPosition::type);
    // store mouse coordinates
    m_positionClicked = static_cast<CursorClickedPosition*>(msg)->position;

    return gf::MessageStatus::Keep;
  }

  gf::MessageStatus Player::onHarvestResource(gf::Id id, gf::Message *msg) {
    assert(id == HarvestResource::type);
    gf::unused(id, msg);

    m_overSupply = true;

    return gf::MessageStatus::Keep;
  }

  gf::MessageStatus Player::onGameOver(gf::Id id, gf::Message *msg) {
    assert(id == GameOver::type);
    gf::unused(id, msg);

    if (!m_wasDeathSound) {
      m_deathSound.play();
      m_wasDeathSound = true;
    }

    m_currentAnimation = &m_death;
    m_dead = true;
    m_positionClicked = m_position;
    m_velocity = {0.0f, 0.0f};

    return gf::MessageStatus::Keep;
  }

  void Player::loadAnimation(gf::Animation &animation, int line) {
    static constexpr gf::Vector2f TextureSize = {5632.0f, 4864.0f};
    static constexpr gf::Vector2f FrameSize = {256.0f, 256.0f};
    static constexpr gf::Time frameTiming = gf::seconds(1.0f / 30.0f);

    for (int i = 0; i < 22; ++i) {
      gf::RectF frame({i * FrameSize.width / TextureSize.width, line * FrameSize.height / TextureSize.height}, FrameSize / TextureSize);

      animation.addFrame(m_moveAndPauseTexture, frame, frameTiming);
    }
  }

  void Player::loadAnimationDeath() {
    static constexpr gf::Vector2f TextureSize = {5632.0f, 1024.0f};
    static constexpr gf::Vector2f FrameSize = {256.0f, 256.0f};
    static constexpr gf::Time frameTiming = gf::seconds(1.0f / 30.0f);

    for (int i = 0; i < 4; ++i) {
      for (int j = 0; j < 22; ++j) {
        gf::RectF frame({j * FrameSize.width / TextureSize.width, i * FrameSize.height / TextureSize.height}, FrameSize / TextureSize);

        if (i == 3 && j == 18) {
          m_death.addFrame(m_deathTexture, frame, gf::seconds(60 * 60 * 24 * 10000));
        }
        else {
          m_death.addFrame(m_deathTexture, frame, frameTiming);
        }
      }
    }
  }
}
