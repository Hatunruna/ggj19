#include "Map.h"

#include <gf/Log.h>
#include <gf/RenderTarget.h>
#include <gf/Shapes.h>

#include "Singletons.h"

namespace home {
    Map::Map() {
        gMessageManager().registerHandler<CursorPosition>(&Map::onCursorPosition, this);
    }

    void Map::render(gf::RenderTarget& target, const gf::RenderStates& states) {
        gf::RectangleShape rect;
        rect.setColor(gf::Color::Red);
        rect.setSize({50.0f, 50.0f});
        rect.setPosition({10.0f, 10.0f});
        rect.setAnchor(gf::Anchor::Center);
        target.draw(rect, states);
    }

    gf::MessageStatus Map::onCursorPosition(gf::Id id, gf::Message *msg) {
        assert(id == CursorPosition::type);
        CursorPosition *message = static_cast<CursorPosition*>(msg);
        gf::Log::debug("Mouse coordinates: %f, %f\n", message->position.x, message->position.y);
        return gf::MessageStatus::Keep;
    }   
}