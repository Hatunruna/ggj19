#include "FieldOfView.h"

#include <gf/Shapes.h>
#include <gf/Sprite.h>
#include <gf/RenderTarget.h>

#include "Messages.h"
#include "Singletons.h"

namespace home {
  FieldOfView::FieldOfView()
  : m_texture(gResourceManager().getTexture("images/fov.png"))
  , m_position({0.0f, 0.0f}) {
    gMessageManager().registerHandler<HeroPosition>(&FieldOfView::onHeroPosition, this);
  }

  void FieldOfView::render(gf::RenderTarget& target, const gf::RenderStates& states) {
    gf::Sprite fov;
    fov.setTexture(m_texture);
    fov.setAnchor(gf::Anchor::Center);
    fov.setPosition(m_position);
    target.draw(fov, states);
  }

  gf::MessageStatus FieldOfView::onHeroPosition(gf::Id id, gf::Message *msg) {
    assert(id == HeroPosition::type);
    auto message = static_cast<HeroPosition*>(msg);

    m_position = message->position;

    return gf::MessageStatus::Keep;
  }
}
