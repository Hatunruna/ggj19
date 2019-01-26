#ifndef HOME_MAP_H
#define HOME_MAP_H

#include <gf/Entity.h>
#include <gf/TileLayer.h>

#include "Messages.h"

namespace home {
  class Map : public gf::Entity {
  public :
    Map();

    virtual void render(gf::RenderTarget& target, const gf::RenderStates& states) override;

  private:
    gf::MessageStatus onCursorMovedPosition(gf::Id id, gf::Message *msg);

  private:
    gf::TileLayer m_layer;
  };
}

#endif // HOME_MAP_H
