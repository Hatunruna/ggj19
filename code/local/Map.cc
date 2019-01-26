#include "Map.h"

#include <gf/Log.h>
#include <gf/RenderTarget.h>
#include <gf/Shapes.h>
#include <gf/Tmx.h>

#include "Singletons.h"

namespace home {

  namespace {

    class LayersMaker : public gf::TmxVisitor {
    public:
      LayersMaker(gf::TileLayer& layer, std::vector<gf::Sprite>& sprites, SupplyManager& supplies)
      : m_layer(layer)
      , m_sprites(sprites)
      , m_supplies(supplies)
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

      virtual void visitObjectLayer(const gf::TmxLayers& map, const gf::TmxObjectLayer& layer) override {
        gf::Log::info("Parsing object layer '%s'\n", layer.name.c_str());

        bool isResources = (layer.name == "Resources");

        for (auto& object : layer.objects) {
          if (object->kind != gf::TmxObject::Tile) {
            continue;
          }

          auto tile = static_cast<gf::TmxTileObject *>(object.get());

          auto tileset = map.getTileSetFromGID(tile->gid);
          assert(tileset);
          assert(tileset->image);

          // compute texture rect
          auto lid = tile->gid - tileset->firstGid;
          auto subTexture = tileset->getSubTexture(lid, tileset->image->size);

          const gf::Texture& texture = gResourceManager().getTexture(tileset->image->source);
          gf::RectF textureRect = texture.computeTextureCoords(subTexture);

          gf::Sprite sprite(texture, textureRect);
          sprite.setPosition(tile->position);
          sprite.setRotation(gf::degreesToRadians(tile->rotation));
          sprite.setAnchor(gf::Anchor::BottomLeft); // see http://docs.mapeditor.org/en/stable/reference/tmx-map-format/#object

          m_sprites.push_back(std::move(sprite));

          if (isResources) {
            gf::Vector2f position = tile->position;
            position.x += subTexture.width / 2;
            position.y -= subTexture.height / 2;

            switch (lid) {
              case 0:
                m_supplies.addSupply(SupplyType::Energy, position);
                break;
              case 1:
                m_supplies.addSupply(SupplyType::Metal, position);
                break;
              case 2:
                m_supplies.addSupply(SupplyType::Oxygen, position);
                break;
              default:
                assert(false);
                break;
            }
          }
        }
      }

    private:
      gf::TileLayer& m_layer;
      std::vector<gf::Sprite>& m_sprites;
      SupplyManager& m_supplies;
    };
  }

  Map::Map(const gf::TmxLayers& layers, SupplyManager& supplies)
  : m_layer(layers.mapSize, gf::TileLayer::Staggered)
  {
    LayersMaker maker(m_layer, m_sprites, supplies);
    layers.visitLayers(maker);

    gMessageManager().registerHandler<CursorMovedPosition>(&Map::onCursorMovedPosition, this);
  }

  void Map::render(gf::RenderTarget& target, const gf::RenderStates& states) {
    target.draw(m_layer, states);

    for (auto& sprite : m_sprites) {
      target.draw(sprite, states);
    }
  }

  gf::MessageStatus Map::onCursorMovedPosition(gf::Id id, gf::Message *msg) {
    assert(id == CursorMovedPosition::type);
    CursorMovedPosition *message = static_cast<CursorMovedPosition*>(msg);
//     gf::Log::debug("Mouse coordinates: %f, %f\n", message->position.x, message->position.y);
    return gf::MessageStatus::Keep;
  }
}
