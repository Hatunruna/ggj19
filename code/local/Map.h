#ifndef HOME_MAP_H
#define HOME_MAP_H

#include <gf/Entity.h>
#include "Messages.h"

namespace home {
  class Map : public gf::Entity {
    public :
      Map();
      virtual void render(gf::RenderTarget& target, const gf::RenderStates& states) override;
      gf::MessageStatus onCursorPosition(gf::Id id, gf::Message *msg);
  };
}

#endif // HOME_MAP_H