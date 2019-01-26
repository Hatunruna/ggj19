#include "FieldOfView.h"

#include <gf/Log.h>
#include <gf/Shapes.h>
#include <gf/Sprite.h>
#include <gf/RenderTarget.h>

#include "Messages.h"
#include "Singletons.h"

namespace home {
  static constexpr float TimeFactor = 1.0f / 60.0f; // 1 minute = 1 hour in game
  static constexpr float StartDusk = 20.0f; // Start of dusk - 8 pm
  static constexpr float StartDawn = 8.0f; // Start of dawn - 8 am
  static constexpr float PeriodTransitionDuration = 1.0f; // 1 hour in game

  FieldOfView::FieldOfView()
  : m_texture(gResourceManager().getTexture("images/fov.png"))
  , m_position({0.0f, 0.0f})
  , m_inGameTime(StartDawn)
  , m_alpha(gf::Color::Opaque(1.0f))
  , m_numberDays(1) {
    gMessageManager().registerHandler<HeroPosition>(&FieldOfView::onHeroPosition, this);
  }

  void FieldOfView::update(gf::Time time) {
    m_inGameTime += time.asSeconds() * TimeFactor;
  }

  void FieldOfView::render(gf::RenderTarget& target, const gf::RenderStates& states) {
    // Compute the right alpha
    if (m_inGameTime >= StartDusk && m_inGameTime <= StartDusk + PeriodTransitionDuration) {
      float elapsedTime = m_inGameTime - StartDusk;
      m_alpha = gf::Color::Opaque(elapsedTime / PeriodTransitionDuration);
    }
    else if (m_inGameTime >= StartDawn && m_inGameTime <= StartDawn + PeriodTransitionDuration) {
      float elapsedTime = m_inGameTime - StartDawn;
      m_alpha = gf::Color::Opaque(std::abs(elapsedTime / PeriodTransitionDuration - 1.0f));
    }
    else if (m_inGameTime >= 24.0f) {
      m_inGameTime -= 24.0f;
      m_numberDays += 1;
    }

    gf::Sprite fov;
    fov.setTexture(m_texture);
    fov.setAnchor(gf::Anchor::Center);
    fov.setPosition(m_position);
    fov.setColor(m_alpha);
    target.draw(fov, states);
  }

  gf::MessageStatus FieldOfView::onHeroPosition(gf::Id id, gf::Message *msg) {
    assert(id == HeroPosition::type);
    auto message = static_cast<HeroPosition*>(msg);

    m_position = message->position;

    return gf::MessageStatus::Keep;
  }
}
