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
  class Map : public gf::Entity {
  public :
    Map(const gf::TmxLayers& layers, SupplyManager& supplies);

    virtual void render(gf::RenderTarget& target, const gf::RenderStates& states) override;

  private:
    gf::MessageStatus onCursorMovedPosition(gf::Id id, gf::Message *msg);

  private:
    gf::TileLayer m_layer;
    std::vector<gf::Sprite> m_sprites;
  };
}

#endif // HOME_MAP_H
