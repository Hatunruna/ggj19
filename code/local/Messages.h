#ifndef HOME_MESSAGES_H
#define HOME_MESSAGES_H

#include <gf/Message.h>
#include <gf/Vector.h>

using namespace gf::literals;

namespace home {
  struct CursorMovedPosition : public gf::Message {
    static const gf::Id type = "CursorMovedPosition"_id; // compile-time definition
    gf::Vector2f position;
  };

  struct CursorClickedPosition : public gf::Message {
    static const gf::Id type = "CursorClickedPosition"_id; // compile-time definition
    gf::Vector2f position;
  };

  struct HeroPosition : public gf::Message {
    static const gf::Id type = "HeroPosition"_id; // compile-time definition
    gf::Vector2f position;
  };
}

#endif // HOME_MESSAGES_H
