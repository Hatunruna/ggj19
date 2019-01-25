#include "Map.h"
#include <gf/Shapes.h>
#include <gf/RenderTarget.h>

namespace home {
    Map::Map() {

    }

    void Map::render(gf::RenderTarget& target, const gf::RenderStates& states) {
        gf::RectangleShape rect;
        rect.setColor(gf::Color::Red);
        rect.setSize({50.0f, 50.0f});
        rect.setPosition({10.0f, 10.0f});
        rect.setAnchor(gf::Anchor::Center);
        target.draw(rect, states);
    }
}