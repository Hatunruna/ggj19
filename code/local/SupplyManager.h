#ifndef HOME_SUPPLY_MANAGER_H
#define HOME_SUPPLY_MANAGER_H

#include <vector>

#include <gf/Entity.h>
#include <gf/Vector.h>

namespace home {
  enum class SupplyType {
    Metal,
  };

  struct Supply {
    SupplyType type;
    float quantity;
    gf::Vector2i position;
  };

  class SupplyManager: public gf::Entity {
  public:
    SupplyManager();

    virtual void render(gf::RenderTarget& target, const gf::RenderStates& states) override;

  private:
    std::vector<Supply> m_supplies;
  };
}

#endif // HOME_SUPPLY_MANAGER_H
