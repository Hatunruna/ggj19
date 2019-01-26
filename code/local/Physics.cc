#include "Physics.h"

#include <set>
#include <vector>

#include <gf/Array2D.h>
#include <gf/Log.h>
#include <gf/Polyline.h>
#include <gf/VectorOps.h>

namespace home {

  namespace {

    struct Segment {
      gf::Vector2i p1;
      gf::Vector2i p2;

      void reverse() {
        std::swap(p1, p2);
      }
    };

    bool operator<(const Segment& lhs, const Segment& rhs) {
      return std::tie(lhs.p1.x, lhs.p1.y, lhs.p2.x, lhs.p2.y) < std::tie(rhs.p1.x, rhs.p1.y, rhs.p2.x, rhs.p2.y);
    }

    template<typename Iterator>
    Iterator findNextSegment(Iterator begin, Iterator end, gf::Vector2i endPoint, bool& needReverse) {
      for (auto it = begin; it != end; ++it) {
        if (it->p1 == endPoint) {
          needReverse = false;
          return it;
        }

        if (it->p2 == endPoint) {
          needReverse = true;
          return it;
        }
      }

      return end;
    }

    std::vector<gf::Polyline> computeAutoCollision(const std::vector<Segment>& segments) {
      // not optimized at all
      std::vector<gf::Polyline> lines;
      std::set<Segment> remaining(segments.begin(), segments.end());

      for (auto it = remaining.begin(); it != remaining.end(); /* not here */) {
        // start a new line
        gf::Polyline polyline(gf::Polyline::Loop);

        Segment first = *it;
        polyline.addPoint(first.p1);

        gf::Vector2i endPoint = first.p2;

        for (;;) {
          polyline.addPoint(endPoint);

          bool needReverse;
          auto next = findNextSegment(std::next(it), remaining.end(), endPoint, needReverse);

          if (next == remaining.end()) {
            // the line is a chain
            polyline.setType(gf::Polyline::Chain);
            break;
          }

          Segment chosen = *next;
          remaining.erase(next);

          if (needReverse) {
            chosen.reverse();
          }

          assert(chosen.p1 == endPoint);
          endPoint = chosen.p2;

          if (endPoint == first.p1) {
            // the line is a loop
            break;
          }
        }

        auto prev = it++;
        remaining.erase(prev);

        polyline.simplify();

        lines.push_back(std::move(polyline));
      }

      return lines;
    }

    class PhysicsMaker : public gf::TmxVisitor {
      static constexpr int Land = 0;
      static constexpr int Void = 1;

    public:
      PhysicsMaker(b2World& world)
      : m_world(world)
      {

      }

      virtual void visitTileLayer(const gf::TmxLayers& map, const gf::TmxTileLayer& layer) override {
        if (!layer.visible) {
          return;
        }

        assert(map.orientation == gf::TmxOrientation::Staggered);
        gf::Log::info("Parsing layer '%s'\n", layer.name.c_str());

        gf::Vector2i tileSize = map.tileSize;

        gf::Array2D<int> biomes(map.mapSize, Void);

        unsigned k = 0;

        for (auto& cell : layer.cells) {
          unsigned i = k % map.mapSize.width;
          unsigned j = k / map.mapSize.width;
          assert(j < map.mapSize.height);

          unsigned gid = cell.gid;

          if (gid != 0) {
            biomes({ i, j }) = Land;
          }

          k++;
        }

        std::vector<Segment> segments;

        for (auto pos : biomes.getPositionRange()) {
          int biome = biomes(pos);

          static constexpr std::pair<gf::Vector2i, gf::Vector2i> NeighborsEven[] = {
            { { -1, -1 }, { -1, -1 } }, // NW
            { {  0, -1 }, {  1, -1 } }, // NE
            { { -1,  1 }, { -1,  1 } }, // SW
            { {  0,  1 }, {  1,  1 } }, // SE
          };

          static constexpr std::pair<gf::Vector2i, gf::Vector2i> NeighborsOdd[] = {
           { {  0, -1 }, { -1, -1 } }, // NW
           { {  1, -1 }, {  1, -1 } }, // NE
           { {  0,  1 }, { -1,  1 } }, // SW
           { {  1,  1 }, {  1,  1 } }, // SE
          };

          gf::Vector2i endPoint1, endPoint2;

          if (biome == Void) {
            if (pos.y % 2 == 0) {
              gf::Vector2i base(pos * tileSize);
              base.y /= 2;
              base += tileSize / 2;

              for (auto& rel : NeighborsEven) {
                gf::Vector2i neighbor = pos + rel.first;

                if (!biomes.isValid(neighbor)) {
                  continue;
                }

                if (biomes(neighbor) == Void) {
                  continue;
                }

                endPoint1 = endPoint2 = base + rel.second * tileSize / 2;
                endPoint1.x = base.x;
                endPoint2.y = base.y;

                segments.push_back({ endPoint1, endPoint2 });

//                 gf::Log::info("For pos (%i,%i) [even], base (%i,%i), add segment (%i, %i) -> (%i, %i)\n", pos.x, pos.y, base.x, base.y, endPoint1.x, endPoint1.y, endPoint2.x, endPoint2.y);
              }
            } else {
              gf::Vector2i base(pos * tileSize);
              base.y /= 2;
              base.x += tileSize.width;
              base.y += tileSize.height / 2;

              for (auto& rel : NeighborsOdd) {
                gf::Vector2i neighbor = pos + rel.first;

                if (!biomes.isValid(neighbor)) {
                  continue;
                }

                if (biomes(neighbor) == Void) {
                  continue;
                }

                endPoint1 = endPoint2 = base + rel.second * tileSize / 2;
                endPoint1.x = base.x;
                endPoint2.y = base.y;

                segments.push_back({ endPoint1, endPoint2 });

//                 gf::Log::info("For pos (%i,%i) [odd], base (%i,%i), add segment (%i, %i) -> (%i, %i)\n", pos.x, pos.y, base.x, base.y, endPoint1.x, endPoint1.y, endPoint2.x, endPoint2.y);
              }
            }
          }

        }

        gf::Log::info("Number of segments: %zu\n", segments.size());

        std::vector<gf::Polyline> polylines = computeAutoCollision(segments);

        gf::Log::info("Number of polylines: %zu\n", polylines.size());

        for (auto& polyline : polylines) {

        }

      }

    private:
      b2World& m_world;
    };


  }

  Physics::Physics(const gf::TmxLayers& layers)
  : m_world({ 0.0f, 0.0f })
  {
    PhysicsMaker maker(m_world);
    layers.visitLayers(maker);
  }

  void Physics::update(gf::Time time) {
    static constexpr int32 VelocityIterations = 10; // 6;
    static constexpr int32 PositionIterations = 8; // 2;
    m_world.Step(time.asSeconds(), VelocityIterations, PositionIterations);
  }

}
