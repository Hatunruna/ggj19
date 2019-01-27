#ifndef HOME_SUPPLY_MANAGER_H
#define HOME_SUPPLY_MANAGER_H

#include <vector>

#include <SFML/Audio.hpp>

#include <gf/Entity.h>
#include <gf/Message.h>
#include <gf/Vector.h>

#include "Singletons.h"

namespace home {
  enum class SupplyType: uint8_t {
    Energy = 0,
    Metal = 1,
    Oxygen = 2,
  };

  struct Supply {
    Supply(SupplyType type, float quantity, gf::Vector2f position)
    : type(type)
    , initialQuantity(quantity)
    , quantity(quantity)
    , position(position)
    , soundStarted(false) {
      switch (type) {
        case SupplyType::Oxygen:
          miningSound.setBuffer(gResourceManager().getSound("sounds/o2_filling.ogg"));
          break;
        default:
          miningSound.setBuffer(gResourceManager().getSound("sounds/mining.ogg"));
          break;
      }
      miningSound.setLoop(true);
      miningSound.setVolume(50.0f);
    }

    SupplyType type;
    float initialQuantity;
    float quantity;
    gf::Vector2f position;
    sf::Sound miningSound;
    bool soundStarted;
  };

  class SupplyManager: public gf::Entity {
  public:
    SupplyManager();

    virtual void update(gf::Time time) override;
    virtual void render(gf::RenderTarget& target, const gf::RenderStates& states) override;

    gf::MessageStatus onHeroPosition(gf::Id id, gf::Message *msg);

    void addSupply(SupplyType type, gf::Vector2f position);

  private:
    std::vector<Supply> m_supplies;
    gf::Vector2f m_heroLocation;
    gf::Texture &m_supplySet;
  };
}

#endif // HOME_SUPPLY_MANAGER_H
