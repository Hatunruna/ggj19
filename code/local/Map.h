#ifndef HOME_MAP_H
#define HOME_MAP_H

#include <vector>

#include <gf/Entity.h>
#include <gf/Sprite.h>
#include <gf/TileLayer.h>
#include <gf/Tmx.h>

#include "Messages.h"
#include "SupplyManager.h"

namespace home {

  struct MapGraphicsData {
    MapGraphicsData(const gf::TmxLayers& layers, SupplyManager& supplies);

    std::vector<gf::TileLayer> tiles;
    std::vector<gf::Sprite> sprites;
  };

  class Map : public gf::Entity {
  public :
    enum Type {
      Below,
      Above,
    };

    Map(Type type, MapGraphicsData& data);

    virtual void render(gf::RenderTarget& target, const gf::RenderStates& states) override;

  private:
    gf::MessageStatus onHeroPosition(gf::Id id, gf::Message *msg);

  private:
    Type m_type;
    MapGraphicsData& m_data;
    gf::Vector2f m_hero;
  };
}

#endif // HOME_MAP_H
