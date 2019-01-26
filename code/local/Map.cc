#include "Map.h"

#include <gf/Log.h>
#include <gf/RenderTarget.h>
#include <gf/Shapes.h>
#include <gf/Tmx.h>

#include "Singletons.h"

namespace home {

  namespace {

    gf::Vector2u getMapSize(const gf::Path& path) {
      gf::Path filename = gResourceManager().getAbsolutePath(path);
      gf::TmxLayers layers;

      bool present = layers.loadFromFile(filename);
      assert(present);

      return layers.mapSize;
    }

    class LayersMaker : public gf::TmxVisitor {
    public:
      LayersMaker(gf::TileLayer& layer)
      : m_layer(layer)
      {

      }

      virtual void visitTileLayer(const gf::TmxLayers& map, const gf::TmxTileLayer& layer) override {
        if (!layer.visible) {
          return;
        }

        assert(map.orientation == gf::TmxOrientation::Staggered);
        assert(m_layer.getMapSize() == map.mapSize);

        gf::Log::info("Parsing layer '%s'\n", layer.name.c_str());

        m_layer.setBlockSize(map.tileSize);

        unsigned k = 0;

        for (auto& cell : layer.cells) {
          unsigned i = k % map.mapSize.width;
          unsigned j = k / map.mapSize.width;
          assert(j < map.mapSize.height);

          unsigned gid = cell.gid;

          if (gid != 0) {
            auto tileset = map.getTileSetFromGID(gid);
            assert(tileset);
            m_layer.setTileSize(tileset->tileSize);

            gid = gid - tileset->firstGid;
            m_layer.setTile({ i, j }, gid, cell.flip);

            if (!m_layer.hasTexture()) {
              assert(tileset->image);
              const gf::Texture& texture = gResourceManager().getTexture(tileset->image->source);
              m_layer.setTexture(texture);
            } else {
              assert(&gResourceManager().getTexture(tileset->image->source) == &m_layer.getTexture());
            }

          }

          k++;
        }
      }

    private:
      gf::TileLayer& m_layer;
    };

    constexpr const char *MapName = "map/MapForDev.tmx";
  }

  Map::Map()
  : m_layer(getMapSize(MapName), gf::TileLayer::Staggered)
  {
    gf::Path filename = gResourceManager().getAbsolutePath(MapName);
    gf::TmxLayers layers;

    bool present = layers.loadFromFile(filename);
    assert(present);

    LayersMaker maker(m_layer);
    layers.visitLayers(maker);

    gMessageManager().registerHandler<CursorMovedPosition>(&Map::onCursorMovedPosition, this);
  }

  void Map::render(gf::RenderTarget& target, const gf::RenderStates& states) {
    target.draw(m_layer, states);
  }

  gf::MessageStatus Map::onCursorMovedPosition(gf::Id id, gf::Message *msg) {
    assert(id == CursorMovedPosition::type);
    CursorMovedPosition *message = static_cast<CursorMovedPosition*>(msg);
//     gf::Log::debug("Mouse coordinates: %f, %f\n", message->position.x, message->position.y);
    return gf::MessageStatus::Keep;
  }
}
