#ifndef HOME_MESSAGES_H
#define HOME_MESSAGES_H

#include <gf/Message.h>
#include <gf/Vector.h>

using namespace gf::literals;

namespace home {
  struct CursorPosition : public gf::Message {
    static const gf::Id type = "CursorPosition"_id; // compile-time definition
    gf::Vector2f position;
  };
}
#endif // HOME_MESSAGES_H