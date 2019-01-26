#ifndef HOME_SUPPLY_MANAGER_H
#define HOME_SUPPLY_MANAGER_H

#include <vector>

#include <SFML/Audio.hpp>

#include <gf/Entity.h>
#include <gf/Message.h>
#include <gf/Vector.h>

#include "Singletons.h"

namespace home {
  enum class SupplyType {
    Metal,
    Oxygen,
    Energy,
  };

  struct Supply {
    Supply(SupplyType _type, float _quantity, gf::Vector2i _position)
    : type(_type)
    , initialQuantity(_quantity)
    , quantity(_quantity)
    , position(_position)
    , soundStarted(false) {
      switch (type) {
        case SupplyType::Metal:
          miningSound.setBuffer(gResourceManager().getSound("sounds/mining.ogg"));
          break;
        default:
          miningSound.setBuffer(gResourceManager().getSound("sounds/o2_filling.ogg"));
          break;
      }
      miningSound.setLoop(true);
      miningSound.setVolume(50.0f);
    }

    SupplyType type;
    float initialQuantity;
    float quantity;
    gf::Vector2i position;
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
  };
}

#endif // HOME_SUPPLY_MANAGER_H
