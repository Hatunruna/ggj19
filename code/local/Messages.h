#ifndef HOME_MESSAGES_H
#define HOME_MESSAGES_H

#include <gf/Message.h>
#include <gf/Vector.h>

#include "SupplyManager.h"

using namespace gf::literals;

namespace home {
  struct CursorMovedPosition : public gf::Message {
    static const gf::Id type = "CursorMovedPosition"_id;
    gf::Vector2f position;
  };

  struct CursorClickedPosition : public gf::Message {
    static const gf::Id type = "CursorClickedPosition"_id;
    gf::Vector2f position;
  };

  struct HeroPosition : public gf::Message {
    static const gf::Id type = "HeroPosition"_id;
    gf::Vector2f position;
  };

  struct ClockInfo: public gf::Message {
    static const gf::Id type = "ClockInfo"_id;
    float time;
    int nbDays;
  };

  struct HarvestResource: public gf::Message {
    static const gf::Id type = "HarvestResource"_id;
    SupplyType resourceType;
    float quantity;
  };
}

#endif // HOME_MESSAGES_H
