#include "CrashedShip.h"

#include <gf/Log.h>
#include <gf/RenderTarget.h>

#include "Messages.h"
#include "Singletons.h"

namespace home {
  static constexpr float OxygenHarvestSpeed = 6.0f; // 6 unit / sec;
  static constexpr float OxygenQuantity = 1000.0f;
  static constexpr float UnloadSpeed = 1.0f; // 1 u / s
  static constexpr float MaxMinerals = 8000.0f; // 8000
  static constexpr float MaxEnergy = 9000.0f; // 9000

  CrashedShip::CrashedShip()
  : m_hitbox({5318.0f, 2350.0f}, {350.0f, 200.0f})
  , m_heroLocation({0.0f, 0.0f})
  , m_oxygenLevel(OxygenQuantity)
  , m_cristalQuantity(0.0f)
  , m_metalQuantity(0.0f) {
    gMessageManager().registerHandler<HeroPosition>(&CrashedShip::onHeroPosition, this);
  }

  void CrashedShip::render(gf::RenderTarget& target, const gf::RenderStates& states) {
  }

  void CrashedShip::update(gf::Time time) {
    if (m_hitbox.contains(m_heroLocation)) {
      if (m_cristalQuantity >= MaxEnergy && m_metalQuantity >= MaxMinerals) {
        Victory msg;
        gMessageManager().sendMessage(&msg);
      }
      // Reffil oxygen
      float quantity = OxygenHarvestSpeed * time.asSeconds();
      if (m_oxygenLevel - quantity < 0.0f) {
        quantity = m_oxygenLevel;
      }
      m_oxygenLevel -= quantity;

      HarvestResource message;
      message.resourceType = SupplyType::Oxygen;
      message.quantity = quantity;
      gMessageManager().sendMessage(&message);

      // Add the remainder to the supply
      m_oxygenLevel += message.quantity;

      // Unload the backpack
      UnloadBackpack unload;
      unload.dt = time.asSeconds() * UnloadSpeed;
      unload.cristalQuantity = 0.0f;
      unload.metalQuantity = 0.0f;
      gMessageManager().sendMessage(&unload);

      m_cristalQuantity += unload.cristalQuantity;
      m_metalQuantity += unload.metalQuantity;
    }

    InfoSupplies info;
    info.cristalQuantity = std::round(m_cristalQuantity);
    info.metalQuantity = std::round(m_metalQuantity);
    gMessageManager().sendMessage(&info);
  }

  gf::MessageStatus CrashedShip::onHeroPosition(gf::Id id, gf::Message *msg) {
    assert(id == HeroPosition::type);
    auto location = static_cast<HeroPosition*>(msg);

    m_heroLocation = location->position;

    return gf::MessageStatus::Keep;
  }
}
