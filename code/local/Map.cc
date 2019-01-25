#include "Map.h"

#include <gf/Log.h>
#include <gf/RenderTarget.h>
#include <gf/Shapes.h>

#include "Singletons.h"

namespace home {
  Map::Map() {
    gMessageManager().registerHandler<CursorMovedPosition>(&Map::onCursorMovedPosition, this);
  }

  void Map::render(gf::RenderTarget& target, const gf::RenderStates& states) {
    gf::RectangleShape rect;
    rect.setColor(gf::Color::Red);
    rect.setSize({50.0f, 50.0f});
    rect.setPosition({10.0f, 10.0f});
    rect.setAnchor(gf::Anchor::Center);
    target.draw(rect, states);
  }

  gf::MessageStatus Map::onCursorMovedPosition(gf::Id id, gf::Message *msg) {
    assert(id == CursorMovedPosition::type);
    CursorMovedPosition *message = static_cast<CursorMovedPosition*>(msg);
    //"Mouse coordinates: %f, %f\n", message->position.x, message->position.y);
    return gf::MessageStatus::Keep;
  }   
}