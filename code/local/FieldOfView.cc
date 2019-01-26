#include "FieldOfView.h"

#include <gf/Sprite.h>
#include <gf/RenderTarget.h>

#include "Singletons.h"

namespace home {
  static constexpr float TextureSize = 2048.0f;
  static constexpr float ViewSize = 100.0f; // MUST NOT BE HERE! Move to correct place

  FieldOfView::FieldOfView()
  : m_texture(gResourceManager().getTexture("fov.png")){
  }

  void FieldOfView::render(gf::RenderTarget& target, const gf::RenderStates& states) {
    constexpr float scale = ViewSize / TextureSize;

    gf::Sprite fov;
    fov.setTexture(m_texture);
    fov.setScale(scale);
    fov.setAnchor(gf::Anchor::Center);
    fov.setPosition({0.0f, 0.0f});
    target.draw(fov, states);
  }
}
