#include "OxygenHud.h"

#include <gf/Coordinates.h>
#include <gf/Log.h>
#include <gf/RenderTarget.h>
#include <gf/Shapes.h>
#include <gf/Sprite.h>
#include <gf/StringUtils.h>
#include <gf/Text.h>

#include "Singletons.h"

namespace home {
  // Speed of oxygen loss
  static constexpr float OxygenLoss = 4.0f;
  // Max amount of oxygen
  static constexpr float MaxOxygen = 100.0f;
  OxygenHud::OxygenHud()
  : m_oxygen(MaxOxygen)
  , m_oxygenIcon(gResourceManager().getTexture("images/lungs.png")) {
  }

  void OxygenHud::render(gf::RenderTarget& target, const gf::RenderStates& states) {
    // Size of the oxygen bar
    static constexpr gf::Vector2f OxygenSize = {0.15f, 0.025f}; 
    // Position of the oxygen bar
    static constexpr gf::Vector2f OxygenPosition = {0.1f, 0.1f}; 
    // Offset of the icon (to the left)
    static constexpr float OffsetIcon = 0.01f;
    // Scale of the oxygen icon
    static constexpr float scale = 7500.0f;


    gf::Sprite oxygenIcon; // Icons
    gf::RectangleShape oxygenBackground, oxygen; // Oxygen bar
    gf::Coordinates coordinates(target);

    oxygenIcon.setTexture(m_oxygenIcon);
    //gf::Log::debug("relative size y %f\n", coordinates.getRelativeSize({1, 1}).y);
    oxygenIcon.setScale(coordinates.getRelativeSize({1, 1}).y / scale);
    oxygenIcon.setAnchor(gf::Anchor::CenterRight);
    oxygenIcon.setPosition(coordinates.getRelativeSize({OxygenPosition.x - OffsetIcon, OxygenPosition.y}));
    oxygenIcon.setColor({1.0f - m_oxygen / MaxOxygen, 0.0f, m_oxygen / MaxOxygen, 1.0f});  

    oxygenBackground.setColor(gf::Color::Black);
    oxygenBackground.setAnchor(gf::Anchor::TopLeft);
    oxygenBackground.setSize(coordinates.getRelativeSize(OxygenSize));
    oxygenBackground.setPosition(coordinates.getRelativeSize(OxygenPosition));
    oxygenBackground.setOutlineColor(gf::Color::Black);
    oxygenBackground.setOutlineThickness(1.0f);

    oxygen.setColor({1.0f - m_oxygen / MaxOxygen, 0.0f, m_oxygen / MaxOxygen, 1.0f});
    oxygen.setSize(coordinates.getRelativeSize({OxygenSize.x * m_oxygen / MaxOxygen, OxygenSize.y}));
    oxygen.setPosition(coordinates.getRelativeSize({OxygenPosition.x, OxygenPosition.y}));

    target.draw(oxygenIcon, states);
    target.draw(oxygenBackground, states);
    target.draw(oxygen, states);
  }

  void OxygenHud::update(gf::Time time) {
    if (m_oxygen > 0) {
      m_oxygen -= time.asSeconds() * OxygenLoss;
    }
  }
}
