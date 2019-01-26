#include "SupplyManager.h"

#include <gf/Coordinates.h>
#include <gf/Log.h>
#include <gf/RenderTarget.h>
#include <gf/Shapes.h>
#include <gf/VectorOps.h>

#include "Messages.h"
#include "Singletons.h"

namespace home {
  static constexpr gf::Vector2i TileSize = {128, 64};
  static constexpr float OxygenHarvestSpeed = 4.0f; // 12 unit / sec;

  SupplyManager::SupplyManager()
  : m_heroLocation({0.0f, 0.0f}){
    m_supplies.push_back({SupplyType::Metal, 10.0f, {41, 96}});

    gMessageManager().registerHandler<HeroPosition>(&SupplyManager::onHeroPosition, this);
  }

  void SupplyManager::update(gf::Time time) {
    for (auto &supply: m_supplies) {
      gf::RectF rect(TileSize * supply.position - TileSize * 0.5f, TileSize);
      if (rect.contains(m_heroLocation)) {
        float quantity = OxygenHarvestSpeed * time.asSeconds();
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
    gf::Coordinates coordinates(target);

    for (auto &supply: m_supplies) {
      // Ressource sprite
      gf::RectangleShape rectangle;
      rectangle.setSize(TileSize);
      switch (supply.type)
      {
        case SupplyType::Metal:
          rectangle.setColor(gf::Color::Gray(0.75f));
          break;
        case SupplyType::Oxygen:
          rectangle.setColor(gf::Color::Blue);
          break;
        default:
          rectangle.setColor(gf::Color::Yellow);
          break;
      }
      rectangle.setPosition(TileSize * supply.position);
      rectangle.setAnchor(gf::Anchor::Center);
      target.draw(rectangle, states);

      // Life bar of supply
      float life = supply.quantity / supply.initialQuantity;
      if (life < 1.0f) {
        gf::Vector2f pos = TileSize * supply.position - TileSize * 0.5f;
        pos.y -= TileSize.y * 0.5f;
        rectangle.setSize({static_cast<float>(TileSize.width), coordinates.getRelativeSize({1.0f, 0.01f}).height});
        rectangle.setColor(gf::Color::White);
        rectangle.setOutlineColor(gf::Color::Black);
        rectangle.setOutlineThickness(coordinates.getRelativeSize({1.0f, 0.002f}).height);
        rectangle.setPosition(pos);
        rectangle.setAnchor(gf::Anchor::TopLeft);
        target.draw(rectangle, states);

        gf::RectangleShape bar;
        bar.setSize({TileSize.width * life, coordinates.getRelativeSize({1.0f, 0.01f}).height});
        bar.setColor(gf::Color::Green);
        bar.setPosition(pos);
        bar.setAnchor(gf::Anchor::TopLeft);
        target.draw(bar, states);
      }
    }
  }

  gf::MessageStatus SupplyManager::onHeroPosition(gf::Id id, gf::Message *msg) {
    assert(id == HeroPosition::type);
    auto location = static_cast<HeroPosition*>(msg);

    m_heroLocation = location->position;

    return gf::MessageStatus::Keep;
  }
}
