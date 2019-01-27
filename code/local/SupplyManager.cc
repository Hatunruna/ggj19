#include "SupplyManager.h"

#include <gf/Coordinates.h>
#include <gf/Log.h>
#include <gf/RenderTarget.h>
#include <gf/Sprite.h>
#include <gf/Shapes.h>
#include <gf/VectorOps.h>

#include "Messages.h"
#include "Singletons.h"

namespace home {
  static constexpr gf::Vector2i TileSize = {128, 128};

  static constexpr float HarvestSpeed = 100.0f; // 100 unit / sec;
  static constexpr float HarvestQuantity = 1000.0f;
  static constexpr float OxygenHarvestSpeed = 6.0f; // 6 unit / sec;
  static constexpr float OxygenQuantity = 10.0f;

  SupplyManager::SupplyManager()
  : gf::Entity(10)
  , m_heroLocation({0.0f, 0.0f})
  , m_supplySet(gResourceManager().getTexture("map/ResourceSet.png")) {
    gMessageManager().registerHandler<HeroPosition>(&SupplyManager::onHeroPosition, this);
  }

  void SupplyManager::update(gf::Time time) {
    for (auto &supply: m_supplies) {
      gf::RectF rect(supply.position - TileSize * 0.5f, TileSize);
      if (rect.contains(m_heroLocation)) {
        float quantity = 0.0f;

        if (supply.type == SupplyType::Oxygen) {
          quantity = OxygenHarvestSpeed * time.asSeconds();
        } else {
          quantity = HarvestSpeed * time.asSeconds();
        }

        if (supply.quantity - quantity < 0.0f) {
          quantity = supply.quantity;
        }

        supply.quantity -= quantity;

        if (!supply.soundStarted) {
          supply.miningSound.play();
          supply.soundStarted = true;
        }

        HarvestResource message;
        message.resourceType = supply.type;
        message.quantity = quantity;
        gMessageManager().sendMessage(&message);
      } else {
        if (supply.soundStarted) {
          supply.soundStarted = false;
          supply.miningSound.stop();
        }
      }
    }

    // Remove empty supplies
    m_supplies.erase(std::remove_if(m_supplies.begin(), m_supplies.end(), [this](const Supply &supply) {
      return supply.quantity <= 0.0f;
    }), m_supplies.end());
  }

  void SupplyManager::render(gf::RenderTarget& target, const gf::RenderStates& states) {
    static constexpr gf::Vector2f AssestSize = {128.0f, 128.0f};
    static constexpr gf::Vector2f SetSize = {896.0f, 640.0f};

    gf::Coordinates coordinates(target);

    for (auto &supply: m_supplies) {
      gf::Sprite sprite;
      gf::RectF rect({static_cast<uint8_t>(supply.type) * AssestSize.width / SetSize.width, 0.0f}, AssestSize / SetSize);

      sprite.setTexture(m_supplySet);
      sprite.setTextureRect(rect);
      sprite.setPosition(supply.position);
      sprite.setAnchor(gf::Anchor::Center);
      target.draw(sprite, states);

      // Life bar of supply
      float life = supply.quantity / supply.initialQuantity;
      if (life < 1.0f) {
        gf::Vector2f pos = supply.position - TileSize * 0.5f;
        gf::RectangleShape bar;
        pos.y -= TileSize.y * 0.5f;
        bar.setSize({static_cast<float>(TileSize.width), coordinates.getRelativeSize({1.0f, 0.01f}).height});
        bar.setColor(gf::Color::White);
        bar.setOutlineColor(gf::Color::Black);
        bar.setOutlineThickness(coordinates.getRelativeSize({1.0f, 0.002f}).height);
        bar.setPosition(pos);
        bar.setAnchor(gf::Anchor::TopLeft);
        target.draw(bar, states);

        bar.setSize({TileSize.width * life, coordinates.getRelativeSize({1.0f, 0.01f}).height});
        bar.setColor(gf::Color::Green);
        bar.setPosition(pos);
        bar.setAnchor(gf::Anchor::TopLeft);
        target.draw(bar, states);
      }
    }
  }

  void SupplyManager::addSupply(SupplyType type, gf::Vector2f position) {
    if (type == SupplyType::Oxygen) {
      m_supplies.push_back({type, OxygenQuantity, position});
    } else {
      m_supplies.push_back({type, HarvestQuantity, position});
    }
  }

  gf::MessageStatus SupplyManager::onHeroPosition(gf::Id id, gf::Message *msg) {
    assert(id == HeroPosition::type);
    auto location = static_cast<HeroPosition*>(msg);

    m_heroLocation = location->position;

    return gf::MessageStatus::Keep;
  }
}
