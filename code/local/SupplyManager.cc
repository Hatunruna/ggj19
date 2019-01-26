#include "SupplyManager.h"

#include <gf/Log.h>
#include <gf/RenderTarget.h>
#include <gf/Shapes.h>
#include <gf/VectorOps.h>

#include "Messages.h"
#include "Singletons.h"

namespace home {
  static constexpr gf::Vector2i TileSize = {128, 64};
  static constexpr float HarvestSpeed = 100.0f; // 100 unit / sec;

  SupplyManager::SupplyManager()
  : m_heroLocation({0.0f, 0.0f}){
    m_supplies.push_back({SupplyType::Metal, 1000.0f, {41, 96}});

    gMessageManager().registerHandler<HeroPosition>(&SupplyManager::onHeroPosition, this);
  }

  void SupplyManager::update(gf::Time time) {
    for (auto &supply: m_supplies) {
      gf::RectF rect(TileSize * supply.position - TileSize * 0.5f, TileSize);
      if (rect.contains(m_heroLocation)) {
        float quantity = HarvestSpeed * time.asSeconds();
        if (supply.quantity - quantity < 0.0f) {
          quantity = supply.quantity;
        }

        supply.quantity -= quantity;

        HarvestResource message;
        message.resourceType = supply.type;
        message.quantity = quantity;
        gMessageManager().sendMessage(&message);
      }
    }

    // Remove empty supplies
    m_supplies.erase(std::remove_if(m_supplies.begin(), m_supplies.end(), [this](const Supply &supply) {
      return supply.quantity <= 0.0f;
    }), m_supplies.end());
  }

  void SupplyManager::render(gf::RenderTarget& target, const gf::RenderStates& states) {
    for (auto &supply: m_supplies) {
      gf::RectangleShape rectangle;
      rectangle.setSize(TileSize);
      rectangle.setColor(gf::Color::Red);
      rectangle.setPosition(TileSize * supply.position);
      rectangle.setAnchor(gf::Anchor::Center);
      target.draw(rectangle, states);
    }
  }

  gf::MessageStatus SupplyManager::onHeroPosition(gf::Id id, gf::Message *msg) {
    assert(id == HeroPosition::type);
    auto location = static_cast<HeroPosition*>(msg);

    m_heroLocation = location->position;

    return gf::MessageStatus::Keep;
  }
}
