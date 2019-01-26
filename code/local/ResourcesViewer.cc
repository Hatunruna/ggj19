#include "ResourcesViewer.h"

#include <gf/Log.h>
#include <gf/RenderTarget.h>
#include <gf/Shapes.h>

#include "Singletons.h"

namespace home {
  static constexpr float OxygenLoss = 4.0f;
  static constexpr gf::Vector2f OxygenSize = {102.0f, 10.0f};
  static constexpr gf::Vector2f OxygenPosition = {50.0f, 50.0f};
  ResourcesViewer::ResourcesViewer()
  : m_oxygen(100.0f) {
  }

  void ResourcesViewer::render(gf::RenderTarget& target, const gf::RenderStates& states) {
    gf::RectangleShape oxygenBackground, oxygen;
    /*
    player.setPosition(m_position);
    */

    oxygenBackground.setColor(gf::Color::Black);
    oxygenBackground.setAnchor(gf::Anchor::TopLeft);
    oxygenBackground.setSize(OxygenSize);
    oxygenBackground.setPosition(OxygenPosition);
    oxygen.setColor({1.0f - m_oxygen / 100, 0.0f, m_oxygen / 100, 1.0f});
    oxygen.setSize({m_oxygen, OxygenSize.y - 2.0f});
    oxygen.setPosition({OxygenPosition.x + 1.0f, OxygenPosition.y + 1.0f});
    target.draw(oxygenBackground, states);
    target.draw(oxygen, states);
  }

  void ResourcesViewer::update(gf::Time time) {
    if (m_oxygen > 0) {
      m_oxygen -= time.asSeconds() * OxygenLoss;
    }
  }
}
