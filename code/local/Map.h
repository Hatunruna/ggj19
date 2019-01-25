#ifndef HOME_MAP_H
#define HOME_MAP_H

#include <gf/Entity.h>

namespace home {
    class Map : public gf::Entity {
        public :
            Map();
            virtual void render(gf::RenderTarget& target, const gf::RenderStates& states) override;
    };
}

#endif // HOME_MAP_H