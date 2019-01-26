#include "OxygenHud.h"

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
  , m_oxygenIcon(gResourceManager().getTexture("images/lungs.png"))
  , m_lowO2Sound(gResourceManager().getSound("sounds/breath_low_o2.ogg"))
  , m_lowO2Volume(0.0f)
  , m_lowO2SoundStarted(false) {
    m_lowO2Sound.setLoop(true);
    m_lowO2Sound.setVolume(m_lowO2Volume);
  }

  void OxygenHud::render(gf::RenderTarget& target, const gf::RenderStates& states) {
    // Size of the oxygen bar
    static constexpr gf::Vector2f OxygenSize = {150.0f, 15.0f}; 
    // Position of the oxygen bar
    static constexpr gf::Vector2f OxygenPosition = {50.0f, 50.0f}; 
    // Offset of the icon (to the left)
    static constexpr float OffsetIcon = 5.0f;
    // Scale of the oxygen icon
    static constexpr float scale = 0.07f;


    gf::Sprite oxygenIcon; // Icons
    gf::RectangleShape oxygenBackground, oxygen; // Oxygen bar

    oxygenIcon.setTexture(m_oxygenIcon);
    oxygenIcon.setScale(scale);
    oxygenIcon.setAnchor(gf::Anchor::CenterRight);
    oxygenIcon.setPosition({OxygenPosition.x - OffsetIcon, OxygenPosition.y + OxygenSize.y / 2.0f});
    oxygenIcon.setColor({1.0f - m_oxygen / MaxOxygen, 0.0f, m_oxygen / MaxOxygen, 1.0f});  

    oxygenBackground.setColor(gf::Color::Black);
    oxygenBackground.setAnchor(gf::Anchor::TopLeft);
    oxygenBackground.setSize(OxygenSize);
    oxygenBackground.setPosition(OxygenPosition);

    oxygen.setColor({1.0f - m_oxygen / MaxOxygen, 0.0f, m_oxygen / MaxOxygen, 1.0f});
    oxygen.setSize({m_oxygen / MaxOxygen * OxygenSize.x, OxygenSize.y - 2.0f});
    oxygen.setPosition({OxygenPosition.x + 1.0f, OxygenPosition.y + 1.0f});

    target.draw(oxygenIcon, states);
    target.draw(oxygenBackground, states);
    target.draw(oxygen, states);
  }

  void OxygenHud::update(gf::Time time) {
    // Low oxy limit for sound
    static constexpr float LowO2Limit = 30.0f;


    if (m_oxygen > 0) {
      m_oxygen -= time.asSeconds() * OxygenLoss;
    }

    if (m_oxygen <= LowO2Limit) {
      m_lowO2Volume = 100.0f - m_oxygen * 3.0f;
      m_lowO2Sound.setVolume(m_lowO2Volume);
      if (!m_lowO2SoundStarted) {
        m_lowO2SoundStarted = true;
        m_lowO2Sound.play();
      }
    }
    if (m_oxygen > LowO2Limit && m_lowO2SoundStarted) {
      m_lowO2SoundStarted = false;
      m_lowO2Sound.stop();
    }
  }
}
