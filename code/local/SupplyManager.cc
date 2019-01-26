#include "SupplyManager.h"

#include <gf/RenderTarget.h>
#include <gf/Shapes.h>
#include <gf/VectorOps.h>

namespace home {
  static constexpr gf::Vector2i TileSize = {128, 64};
  SupplyManager::SupplyManager() {
    m_supplies.push_back({SupplyType::Metal, 1000.0f, {5, 5}});
  }

  void SupplyManager::render(gf::RenderTarget& target, const gf::RenderStates& states) {
    for (auto &supply: m_supplies) {
      gf::RectangleShape rectangle;
      rectangle.setSize(TileSize);
      rectangle.setColor(gf::Color::Red);
      rectangle.setPosition(TileSize * supply.position);
      target.draw(rectangle, states);
    }
  }
}
