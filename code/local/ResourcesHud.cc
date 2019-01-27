#include "ResourcesHud.h"

#include <gf/Coordinates.h>
#include <gf/Log.h>
#include <gf/RenderTarget.h>
#include <gf/Shapes.h>
#include <gf/Sprite.h>
#include <gf/StringUtils.h>
#include <gf/Text.h>
#include <gf/Unused.h>

#include "Singletons.h"

namespace home {

  static constexpr float MaxMinerals = 8000.0f;
  static constexpr float MaxEnergy = 9000.0f;
  static constexpr float LimitBackpack = 4000.0f;

  void drawSlideBar(gf::RenderTarget& target, const gf::RenderStates& states, gf::Texture &icon, gf::Vector2f position, gf::Color4f color, float ratio, bool iconColored = true) {
    static constexpr gf::Vector2f IconTextureSize = {300.0f, 300.0f};

    gf::Coordinates coordinates(target);
    gf::Vector2f slideBarSize(coordinates.getRelativeSize({0.015f, 0.25f}));
    gf::Vector2f IconSize = coordinates.getRelativeSize({0.12f, 0.12f});
    float ThicknessSize = coordinates.getRelativeSize({0.002f, 0.002f}).height;
    // gf::Vector2f Offset = coordinates.getRelativeSize({0.005f, 0.005f});

    gf::Sprite sprite;
    sprite.setTexture(icon);
    sprite.setScale(IconSize.height / IconTextureSize);
    sprite.setAnchor(gf::Anchor::TopCenter);
    sprite.setPosition(position);
    if (iconColored)
      sprite.setColor(color);
    target.draw(sprite, states);

    gf::RectangleShape rectangle;
    position.height += IconSize.height;
    rectangle.setSize(slideBarSize);
    rectangle.setColor(gf::Color::Black);
    rectangle.setOutlineThickness(ThicknessSize);
    rectangle.setOutlineColor(gf::Color::Black);
    rectangle.setPosition(position);
    rectangle.setAnchor(gf::Anchor::TopCenter);
    target.draw(rectangle, states);

    position.height += rectangle.getSize().height;
    rectangle.setColor(color);
    rectangle.setSize({slideBarSize.width, ratio * slideBarSize.height});
    rectangle.setAnchor(gf::Anchor::BottomCenter);
    rectangle.setPosition(position);
    target.draw(rectangle, states);
  }


  ResourcesHud::ResourcesHud()
  : m_minerals(0.0f)
  , m_energy(0.0f)
  , m_backpackIcon(gResourceManager().getTexture("images/inventory_full_icon.png"))
  , m_mineralsIcon(gResourceManager().getTexture("images/metal_icon.png"))
  , m_energyIcon(gResourceManager().getTexture("images/energy_icon.png"))
  , m_font(gResourceManager().getFont("fonts/dejavu_sans.ttf"))
  , m_cristalQuantity(0.0f)
  , m_metalQuantity(0.0f)
  , m_messageSended(false) {
    gMessageManager().registerHandler<HarvestResource>(&ResourcesHud::onResourceHarvested, this);
    gMessageManager().registerHandler<UnloadBackpack>(&ResourcesHud::onUnloadBackpack, this);
    gMessageManager().registerHandler<InfoSupplies>(&ResourcesHud::onInfoSupplies, this);
  }

  void ResourcesHud::render(gf::RenderTarget& target, const gf::RenderStates& states) {
    gf::Coordinates coordinates(target);
    static constexpr gf::Vector2f offset = { 0.02f, 0.6f };
    gf::Vector2f slideBarSize(coordinates.getRelativeSize({0.015f, 0.25f}));

    auto pos = coordinates.getRelativePoint(offset);
    pos.x = 2.0f * slideBarSize.width;
    drawSlideBar(target, states, m_energyIcon, pos, gf::Color::Yellow, m_energy / MaxEnergy);

    pos.x += offset.width + 4.0f * slideBarSize.width;
    drawSlideBar(target, states, m_mineralsIcon, pos, gf::Color::Gray(0.5f), m_minerals / MaxMinerals);

    pos.x = target.getSize().width - offset.width - 2.0f * slideBarSize.width;
    drawSlideBar(target, states, m_backpackIcon, pos, {0.44f, 0.36f, 0.36f, 1.0f}, (m_cristalQuantity + m_metalQuantity) / LimitBackpack, false);
  }

  void ResourcesHud::update(gf::Time time) {
    gf::unused(time);

    if (!m_messageSended && m_cristalQuantity + m_metalQuantity >= LimitBackpack) {
      MessageToDisplay msg;
      msg.message = "Your inventory is full!";
      msg.displayTime = 4.0f;
      gMessageManager().sendMessage(&msg);

      m_messageSended = true;
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

    m_messageSended = false;

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
