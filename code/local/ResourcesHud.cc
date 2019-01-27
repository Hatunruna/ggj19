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

  static constexpr float MaxMinerals = 8000.0f;
  static constexpr float MaxEnergy = 9000.0f;
  static constexpr float LimitBackpack = 4000.0f;


  ResourcesHud::ResourcesHud()
  : m_minerals(0.0f)
  , m_energy(0.0f)
  , m_messageDisplayed(false)
  , m_displayable(true)
  , m_backpackIcon(gResourceManager().getTexture("images/inventory_full_icon.png"))
  , m_mineralsIcon(gResourceManager().getTexture("images/metal_icon.png"))
  , m_energyIcon(gResourceManager().getTexture("images/energy_icon.png"))
  , m_font(gResourceManager().getFont("fonts/dejavu_sans.ttf"))
  , m_cristalQuantity(0.0f)
  , m_metalQuantity(0.0f)  {
    gMessageManager().registerHandler<HarvestResource>(&ResourcesHud::onResourceHarvested, this);
    gMessageManager().registerHandler<UnloadBackpack>(&ResourcesHud::onUnloadBackpack, this);
    gMessageManager().registerHandler<InfoSupplies>(&ResourcesHud::onInfoSupplies, this);
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
    static constexpr float Scale = 3000.0f;

    gf::Sprite mineralsIcon, energyIcon, backpackIcon; // Icons
    gf::RectangleShape energyBackground, energy, backpackBackground, backpack; // Energy bar
    gf::Coordinates coordinates(target);
    gf::Text text;

    float backpackLoad = m_cristalQuantity + m_metalQuantity;

    backpackIcon.setTexture(m_backpackIcon);
    backpackIcon.setScale(coordinates.getRelativeSize({1.0f, 1.0f}).y / Scale);
    backpackIcon.setAnchor(gf::Anchor::CenterRight);
    backpackIcon.setPosition(coordinates.getRelativeSize({MineralsPosition.x - OffsetIconEnergyLeft, MineralsPosition.y - YDistance}));

    backpackBackground.setColor(gf::Color::Black);
    backpackBackground.setOutlineColor(gf::Color::Black);
    backpackBackground.setOutlineThickness(1.0f);
    backpackBackground.setAnchor(gf::Anchor::TopRight);
    backpackBackground.setSize(coordinates.getRelativeSize(EnergySize));
    backpackBackground.setPosition(coordinates.getRelativeSize({MineralsPosition.x - OffsetBar, MineralsPosition.y - YDistance}));

    backpack.setColor({0.44f, 0.36f, 0.36f, 1.0f});
    backpack.setSize(coordinates.getRelativeSize({EnergySize.x * backpackLoad / LimitBackpack, EnergySize.y}));
    backpack.setPosition(coordinates.getRelativeSize({MineralsPosition.x - OffsetBar, MineralsPosition.y - YDistance}));

    text.setFont(m_font);
    text.setOutlineColor(gf::Color::White);
    text.setOutlineThickness(coordinates.getRelativeCharacterSize(0.002f));
    text.setCharacterSize(coordinates.getRelativeCharacterSize(0.03f));
    text.setString(gf::niceNum(m_minerals, 1) + " / " + gf::niceNum(MaxMinerals, 1));
    text.setParagraphWidth(coordinates.getRelativeCharacterSize(0.03f) * 5.0f);
    text.setAnchor(gf::Anchor::TopRight);
    text.setPosition(coordinates.getRelativeSize(MineralsPosition));
    text.setAlignment(gf::Alignment::Center);

    mineralsIcon.setTexture(m_mineralsIcon);
    mineralsIcon.setScale(coordinates.getRelativeSize({1.0f, 1.0f}).y / Scale);
    mineralsIcon.setAnchor(gf::Anchor::CenterRight);
    mineralsIcon.setPosition(coordinates.getRelativeSize({MineralsPosition.x - OffsetIconMineralsLeft, MineralsPosition.y - OffsetIconMineralsTop}));
    mineralsIcon.setColor({0.5f, 0.5f, 0.5f, 1.0f});

    energyIcon.setTexture(m_energyIcon);
    energyIcon.setScale(coordinates.getRelativeSize({1.0f, 1.0f}).y / Scale);
    energyIcon.setAnchor(gf::Anchor::CenterRight);
    energyIcon.setPosition(coordinates.getRelativeSize({MineralsPosition.x - OffsetIconEnergyLeft, MineralsPosition.y + YDistance  - OffsetIconEnergyTop}));
    energyIcon.setColor({1.0f, 1.0f, 0.0f, 1.0f});

    energyBackground.setColor(gf::Color::Black);
    energyBackground.setOutlineColor(gf::Color::Black);
    energyBackground.setOutlineThickness(1.0f);
    energyBackground.setAnchor(gf::Anchor::TopRight);
    energyBackground.setSize(coordinates.getRelativeSize(EnergySize));
    energyBackground.setPosition(coordinates.getRelativeSize({MineralsPosition.x - OffsetBar, MineralsPosition.y + YDistance}));

    energy.setColor({1.0f, 1.0f, 0.0f, 1.0f});
    energy.setSize(coordinates.getRelativeSize({EnergySize.x * m_energy / MaxEnergy, EnergySize.y}));
    energy.setPosition(coordinates.getRelativeSize({MineralsPosition.x - OffsetBar, MineralsPosition.y + YDistance}));

    target.draw(backpackIcon, states);
    target.draw(backpackBackground, states);
    target.draw(backpack, states);
    target.draw(text, states);
    target.draw(mineralsIcon, states);
    target.draw(energyIcon, states);
    target.draw(energyBackground, states);
    target.draw(energy, states);

    if (m_messageDisplayed) {
      MessageToDisplay msg;
      msg.message = "Your inventory is full!";
      msg.displayTime = 4.0f;
      gMessageManager().sendMessage(&msg);
      m_displayable = false;
    }
  }

  void ResourcesHud::update(gf::Time time) {
    if (m_displayable && m_cristalQuantity + m_metalQuantity >= LimitBackpack) {
      m_messageDisplayed = true;
    }
    if (m_messageDisplayed) {
      m_time += time;
    }
  }

  gf::MessageStatus ResourcesHud::onResourceHarvested(gf::Id id, gf::Message *msg) {
    assert(id == HarvestResource::type);
    HarvestResource *message = static_cast<HarvestResource*>(msg);
    if (message->resourceType == SupplyType::Oxygen) {
      return gf::MessageStatus::Keep;
    }

    float newQuantity = message->quantity;
    float remainder = 0.0f;

    // If the backpack is full
    if (newQuantity + m_metalQuantity + m_cristalQuantity > LimitBackpack) {
      remainder = (newQuantity + m_metalQuantity + m_cristalQuantity) - LimitBackpack;
      newQuantity = newQuantity - remainder;
    }

    if (message->resourceType == SupplyType::Metal) {
      m_metalQuantity += newQuantity;
    } else if (message->resourceType == SupplyType::Energy) {
      m_cristalQuantity += newQuantity;
    }

    message->quantity = remainder;

    return gf::MessageStatus::Keep;
  }

  gf::MessageStatus ResourcesHud::onUnloadBackpack(gf::Id id, gf::Message *msg) {
    assert(id == UnloadBackpack::type);
    UnloadBackpack *message = static_cast<UnloadBackpack*>(msg);

    message->cristalQuantity = m_cristalQuantity * message->dt;
    message->metalQuantity = m_metalQuantity * message->dt;

    m_cristalQuantity -= message->cristalQuantity;
    m_metalQuantity -= message->metalQuantity;

    return gf::MessageStatus::Keep;
  }

  gf::MessageStatus ResourcesHud::onInfoSupplies(gf::Id id, gf::Message *msg) {
    assert(id == InfoSupplies::type);
    InfoSupplies *message = static_cast<InfoSupplies*>(msg);

    m_energy = gf::clamp(message->cristalQuantity, 0.0f, MaxEnergy);
    m_minerals = gf::clamp(message->metalQuantity, 0.0f, MaxMinerals);

    return gf::MessageStatus::Keep;
  }
}
