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
      LayersMaker(std::vector<gf::TileLayer>& layers, std::vector<gf::Sprite>& sprites, SupplyManager& supplies)
      : m_layers(layers)
      , m_sprites(sprites)
      , m_supplies(supplies)
      {

      }

      virtual void visitTileLayer(const gf::TmxLayers& map, const gf::TmxTileLayer& layer) override {
        if (!layer.visible) {
          return;
        }

        assert(map.orientation == gf::TmxOrientation::Staggered);

        gf::TileLayer tileLayer(map.mapSize, gf::TileLayer::Staggered);

        gf::Log::info("Parsing layer '%s'\n", layer.name.c_str());

        tileLayer.setBlockSize(map.tileSize);

        unsigned k = 0;

        for (auto& cell : layer.cells) {
          unsigned i = k % map.mapSize.width;
          unsigned j = k / map.mapSize.width;
          assert(j < map.mapSize.height);

          unsigned gid = cell.gid;

          if (gid != 0) {
            auto tileset = map.getTileSetFromGID(gid);
            assert(tileset);
            tileLayer.setTileSize(tileset->tileSize);

            gid = gid - tileset->firstGid;
            tileLayer.setTile({ i, j }, gid, cell.flip);

            if (!tileLayer.hasTexture()) {
              assert(tileset->image);
              const gf::Texture& texture = gResourceManager().getTexture(tileset->image->source);
              tileLayer.setTexture(texture);
            } else {
              assert(&gResourceManager().getTexture(tileset->image->source) == &tileLayer.getTexture());
            }

          }

          k++;
        }

        m_layers.push_back(std::move(tileLayer));
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
          } else {
            const gf::Texture& texture = gResourceManager().getTexture(tileset->image->source);
            gf::RectF textureRect = texture.computeTextureCoords(subTexture);

            gf::Sprite sprite(texture, textureRect);
            sprite.setPosition(tile->position);
            sprite.setRotation(gf::degreesToRadians(tile->rotation));
            sprite.setAnchor(gf::Anchor::BottomLeft); // see http://docs.mapeditor.org/en/stable/reference/tmx-map-format/#object

            m_sprites.push_back(std::move(sprite));
          }
        }
      }

    private:
      std::vector<gf::TileLayer>& m_layers;
      std::vector<gf::Sprite>& m_sprites;
      SupplyManager& m_supplies;
    };
  }

  MapGraphicsData::MapGraphicsData(const gf::TmxLayers& layers, SupplyManager& supplies)
  {
    LayersMaker maker(tiles, sprites, supplies);
    layers.visitLayers(maker);
  }


  Map::Map(Type type, MapGraphicsData& data)
  : gf::Entity(type == Below ? 0 : 200)
  , m_type(type)
  , m_data(data)
  {
     gMessageManager().registerHandler<HeroPosition>(&Map::onHeroPosition, this);
  }


  void Map::render(gf::RenderTarget& target, const gf::RenderStates& states) {
    if (m_type == Below) {
      for (auto& layer : m_data.tiles) {
        target.draw(layer, states);
      }
    }

    float limit = m_hero.y + 70;

    if (m_type == Below) {
      for (auto& sprite : m_data.sprites) {
        if (sprite.getPosition().y < limit) {
          target.draw(sprite, states);
        }
      }
    } else {
      for (auto& sprite : m_data.sprites) {
        if (sprite.getPosition().y >= limit) {
          target.draw(sprite, states);
        }
      }
    }
  }

  gf::MessageStatus Map::onHeroPosition(gf::Id id, gf::Message *msg) {
    assert(id == HeroPosition::type);
    auto message = static_cast<HeroPosition*>(msg);
    m_hero = message->position;
    return gf::MessageStatus::Keep;
  }

}
