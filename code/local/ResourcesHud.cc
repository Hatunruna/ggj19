#include "ResourcesHud.h"

#include <gf/Coordinates.h>
#include <gf/Log.h>
#include <gf/RenderTarget.h>
#include <gf/Shapes.h>
#include <gf/Sprite.h>
#include <gf/StringUtils.h>
#include <gf/Text.h>

#include "Singletons.h"

namespace home {

  static constexpr int MaxMinerals = 100;
  static constexpr float MaxEnergy = 100.0f;


  ResourcesHud::ResourcesHud()
  : m_minerals(0)
  , m_energy(0.0f)
  , m_messageDisplayed(false)
  , m_mineralsIcon(gResourceManager().getTexture("images/metal_icon.png"))
  , m_energyIcon(gResourceManager().getTexture("images/energy_icon.png"))
  , m_font(gResourceManager().getFont("fonts/dejavu_sans.ttf")) {
    gMessageManager().registerHandler<HarvestResource>(&ResourcesHud::onResourceHarvested, this);
  }

  void ResourcesHud::render(gf::RenderTarget& target, const gf::RenderStates& states) {
    // Size of the oxygen bar
    static constexpr gf::Vector2f EnergySize = {0.15f, 0.025f};
    // Position of the minerals hud
    static constexpr gf::Vector2f MineralsPosition = {0.95f, 0.80f};
    // Offset of the icon (to the left)
    static constexpr float OffsetIconMineralsLeft = 0.13f;
    static constexpr float OffsetIconEnergyLeft = 0.13f;
// Offset of the icon (to the top)
    static constexpr float OffsetIconMineralsTop = -0.02f;
    static constexpr float OffsetIconEnergyTop = -0.02f;
    static constexpr float OffsetBar = 0.12f;
    // Relative vertical distance between the 2 elements of the HUD
    static constexpr float YDistance = 0.10f;
    // Scale of the oxygen icon
    static constexpr float Scale = 7500.0f;
    // Display time of popup message
    static constexpr float DisplayTime = 3.0f;

    gf::Sprite mineralsIcon, energyIcon; // Icons
    gf::RectangleShape energyBackground, energy; // Energy bar
    gf::Coordinates coordinates(target);
    gf::Text text, shipText;

    if (m_messageDisplayed && m_time.asSeconds() < DisplayTime) {
      shipText.setFont(m_font);
      shipText.setColor({0.0, 0.0, 0.0, 1.0 - m_time.asSeconds() / DisplayTime});
      shipText.setOutlineColor({1.0, 1.0, 1.0, 1.0 - m_time.asSeconds() / DisplayTime});
      shipText.setOutlineThickness(coordinates.getRelativeCharacterSize(0.008f));
      shipText.setCharacterSize(coordinates.getRelativeCharacterSize(0.1f));
      shipText.setString("Your inventory is full!");
      shipText.setParagraphWidth(target.getSize().x);
      shipText.setPosition(coordinates.getRelativeSize({0.5f, 0.5f}));
      shipText.setAlignment(gf::Alignment::Center);
      shipText.setAnchor(gf::Anchor::Center);
      target.draw(shipText, states);
    }

    text.setFont(m_font);
    text.setOutlineColor(gf::Color::White);
    text.setOutlineThickness(coordinates.getRelativeCharacterSize(0.002f));
    text.setCharacterSize(coordinates.getRelativeCharacterSize(0.03f));
    text.setString(std::to_string(m_minerals) + " / " + std::to_string(MaxMinerals));
    text.setParagraphWidth(coordinates.getRelativeCharacterSize(0.03f) * 5.0f);
    text.setAnchor(gf::Anchor::TopRight);
    text.setPosition(coordinates.getRelativeSize(MineralsPosition));
    text.setAlignment(gf::Alignment::Center);

    mineralsIcon.setTexture(m_mineralsIcon);
    mineralsIcon.setScale(coordinates.getRelativeSize({1.0f, 1.0f}).y / Scale);
    mineralsIcon.setAnchor(gf::Anchor::CenterRight);
    mineralsIcon.setPosition(coordinates.getRelativeSize({MineralsPosition.x - OffsetIconMineralsLeft, MineralsPosition.y - OffsetIconMineralsTop}));
    mineralsIcon.setColor({1.0f, 0.0f, 1.0f, 1.0f});

    energyIcon.setTexture(m_energyIcon);
    energyIcon.setScale(coordinates.getRelativeSize({1.0f, 1.0f}).y / Scale);
    energyIcon.setAnchor(gf::Anchor::CenterRight);
    energyIcon.setPosition(coordinates.getRelativeSize({MineralsPosition.x - OffsetIconEnergyLeft, MineralsPosition.y + YDistance  - OffsetIconEnergyTop}));
    energyIcon.setColor({0.0f, 1.0f, 0.0f, 1.0f});

    energyBackground.setColor(gf::Color::Black);
    energyBackground.setOutlineColor(gf::Color::Black);
    energyBackground.setOutlineThickness(1.0f);
    energyBackground.setAnchor(gf::Anchor::TopRight);
    energyBackground.setSize(coordinates.getRelativeSize(EnergySize));
    energyBackground.setPosition(coordinates.getRelativeSize({MineralsPosition.x - OffsetBar, MineralsPosition.y + YDistance}));

    energy.setColor({0.0f, 1.0f, 0.0f, 1.0f});
    energy.setSize(coordinates.getRelativeSize({EnergySize.x * m_energy / MaxEnergy, EnergySize.y}));
    energy.setPosition(coordinates.getRelativeSize({MineralsPosition.x - OffsetBar, MineralsPosition.y + YDistance}));

    target.draw(text, states);
    target.draw(mineralsIcon, states);
    target.draw(energyIcon, states);
    target.draw(energyBackground, states);
    target.draw(energy, states);
  }

  void ResourcesHud::update(gf::Time time) {
    if (m_minerals >= MaxMinerals && m_energy >= MaxEnergy) {
      m_messageDisplayed = true;
      MaxResources info;
      gMessageManager().sendMessage(&info);
    }
    if (m_messageDisplayed) {
      m_time += time;
    }
  }

  gf::MessageStatus ResourcesHud::onResourceHarvested(gf::Id id, gf::Message *msg) {
    assert(id == HarvestResource::type);
    HarvestResource *message = static_cast<HarvestResource*>(msg);
    if (message->resourceType == SupplyType::Metal) {
      if (message->quantity + m_minerals > MaxMinerals) {
        m_minerals = MaxMinerals;
      } else {
        m_minerals += message->quantity;
      }
    } else if (message->resourceType == SupplyType::Energy) {
      if (message->quantity + m_energy > MaxEnergy) {
        m_energy = MaxEnergy;
      } else {
        m_energy += message->quantity;
      }
    }
    return gf::MessageStatus::Keep;
  }
}
