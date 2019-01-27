#include "Physics.h"

#include <set>
#include <vector>

#include <gf/Array2D.h>
#include <gf/Color.h>
#include <gf/Curves.h>
#include <gf/Log.h>
#include <gf/Polyline.h>
#include <gf/RenderTarget.h>
#include <gf/Shapes.h>
#include <gf/VectorOps.h>

namespace home {

  namespace {

    constexpr float PhysicsScale = 0.02f;

    b2Vec2 fromVec(gf::Vector2f vec) {
      return { vec.x * PhysicsScale, vec.y * PhysicsScale };
    }

    gf::Vector2f toVec(b2Vec2 vec) {
      return { vec.x / PhysicsScale, vec.y / PhysicsScale };
    }

    gf::Color4f toColor(b2Color color) {
      return { color.r, color.g, color.b, color.a * 0.8f };
    }

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

              base.y -= tileSize.height / 2; // HACK

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

              base.y -= tileSize.height / 2; // HACK

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
          std::vector<b2Vec2> line;

          for (auto& point : polyline) {
            line.push_back(fromVec(point));
          }

          b2ChainShape shape;

          if (polyline.isLoop()) {
            shape.CreateLoop(line.data(), line.size());
          } else {
            assert(polyline.isChain());
            shape.CreateChain(line.data(), line.size());
          }

          b2BodyDef bodyDef;
          bodyDef.type = b2_staticBody;
          bodyDef.position = { 0.0f, 0.0f };
          auto body = m_world.CreateBody(&bodyDef);

          b2FixtureDef fixtureDef;
          fixtureDef.density = 1.0f;
          fixtureDef.friction = 0.0f;
          fixtureDef.restitution = 0.0f;
          fixtureDef.shape = &shape;

          body->CreateFixture(&fixtureDef);
        }

      }

      virtual void visitObjectLayer(const gf::TmxLayers& map, const gf::TmxObjectLayer& layer) override {
        gf::Log::info("Parsing object layer '%s'\n", layer.name.c_str());

        for (auto& object : layer.objects) {
          if (object->kind != gf::TmxObject::Tile) {
            continue;
          }

          auto tile = static_cast<gf::TmxTileObject *>(object.get());

          if (layer.name == "Trees") {
            gf::Vector2f position = tile->position + gf::Vector2f(384 / 2, -70);

            b2BodyDef bodyDef;
            bodyDef.type = b2_staticBody;
            bodyDef.position = fromVec(position);
            auto body = m_world.CreateBody(&bodyDef);

            b2CircleShape shape;
            shape.m_radius = 50.0f * PhysicsScale;

            b2FixtureDef fixtureDef;
            fixtureDef.density = 1.0f;
            fixtureDef.friction = 0.0f;
            fixtureDef.restitution = 0.0f;
            fixtureDef.shape = &shape;

            body->CreateFixture(&fixtureDef);
          }

        }
      }

    private:
      b2World& m_world;
    };


  }

  Physics::Physics(const gf::TmxLayers& layers, Player& player)
  : m_world({ 0.0f, 0.0f })
  , m_hero(nullptr)
  , m_player(player)
  {
    PhysicsMaker maker(m_world);
    layers.visitLayers(maker);

    gf::Vector2f initialPosition = m_player.getDynamics().position;

    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.position = fromVec(initialPosition);
    m_hero = m_world.CreateBody(&bodyDef);

    b2CircleShape shape;
    shape.m_radius = 20.0f * PhysicsScale; // same constants as in Player.cc

    b2FixtureDef fixtureDef;
    fixtureDef.density = 1.0f;
    fixtureDef.friction = 0.0f;
    fixtureDef.restitution = 0.0f;
    fixtureDef.shape = &shape;

    m_hero->CreateFixture(&fixtureDef);
  }

  void Physics::debug() {
    m_world.DrawDebugData();
  }

  void Physics::setDraw(PhysicsDraw *draw) {
    m_world.SetDebugDraw(draw);
  }

  void Physics::update(gf::Time time) {
    auto dynamics = m_player.getDynamics();
    m_hero->SetTransform(fromVec(dynamics.position), 0.0f);
    m_hero->SetLinearVelocity(fromVec(dynamics.velocity));

    static constexpr int32 VelocityIterations = 10; // 6;
    static constexpr int32 PositionIterations = 8; // 2;
    m_world.Step(time.asSeconds(), VelocityIterations, PositionIterations);

    m_player.setDynamics({ toVec(m_hero->GetPosition()), toVec(m_hero->GetLinearVelocity()) });
  }

  /*
   * PhysicsDebugger
   */

  namespace {
    constexpr float DebugOutlineThickness = 1.0f;
    constexpr float DebugTransformLength = 0.5f;
  }

  void PhysicsDebugger::addPolygon(gf::Polygon polygon, gf::Color4f color) {
    m_polygons.push_back({ std::move(polygon), color });
  }

  void PhysicsDebugger::addSolidPolygon(gf::Polygon polygon, gf::Color4f color) {
    m_solidPolygons.push_back({ std::move(polygon), color });
  }

  void PhysicsDebugger::addCircle(gf::CircF circle, gf::Color4f color) {
    m_circles.push_back({ circle, color });
  }

  void PhysicsDebugger::addSolidCircle(gf::CircF circle, gf::Vector2f axis, gf::Color4f color) {
    m_solidCircles.push_back({ circle, axis, color });
  }

  void PhysicsDebugger::addSegment(gf::Vector2f p1, gf::Vector2f p2, gf::Color4f color) {
    m_segments.push_back({ p1, p2, color });
  }

  void PhysicsDebugger::addTransform(gf::Vector2f position, gf::Vector2f xAxis, gf::Vector2f yAxis) {
    m_transforms.push_back({ position, xAxis, yAxis });
  }

  void PhysicsDebugger::render(gf::RenderTarget& target, const gf::RenderStates& states) {
    m_physics.debug();

    for (auto& polygon : m_polygons) {
      gf::ConvexShape shape(polygon.shape);
      shape.setColor(gf::Color::Transparent);
      shape.setOutlineColor(polygon.color);
      shape.setOutlineThickness(DebugOutlineThickness);
      target.draw(shape, states);
    }

    for (auto& polygon : m_solidPolygons) {
      gf::ConvexShape shape(polygon.shape);
      shape.setColor(polygon.color);
      target.draw(shape, states);
    }

    for (auto& circle : m_circles) {
      gf::CircleShape shape(circle.shape);
      shape.setColor(gf::Color::Transparent);
      shape.setOutlineColor(circle.color);
      shape.setOutlineThickness(DebugOutlineThickness);
      target.draw(shape, states);
    }

    for (auto& circle : m_solidCircles) {
      gf::CircleShape shape(circle.shape);
      shape.setColor(circle.color);
      target.draw(shape, states);

      gf::Line line(circle.shape.center, circle.shape.center + circle.axis);
      line.setWidth(2.5f * DebugOutlineThickness);
      line.setColor(gf::Color::darker(circle.color, 0.3f));
      target.draw(line, states);
    }

    for (auto& segment : m_segments) {
      gf::Line curve(segment.p1, segment.p2);
      curve.setWidth(DebugOutlineThickness);
      curve.setColor(segment.color);
      target.draw(curve, states);
    }

    for (auto& transform : m_transforms) {
      gf::Line lineX(transform.position, transform.position + DebugTransformLength * transform.xAxis);
      lineX.setWidth(DebugOutlineThickness);
      lineX.setColor(gf::Color::Red);
      target.draw(lineX, states);

      gf::Line lineY(transform.position, transform.position + DebugTransformLength * transform.yAxis);
      lineY.setWidth(DebugOutlineThickness);
      lineY.setColor(gf::Color::Green);
      target.draw(lineY, states);
    }

    m_polygons.clear();
    m_solidPolygons.clear();
    m_circles.clear();
    m_solidCircles.clear();
    m_segments.clear();
    m_transforms.clear();
  }


  PhysicsDraw::PhysicsDraw(PhysicsDebugger& debug)
  : m_debug(debug)
  {
    SetFlags(b2Draw::e_shapeBit /* | b2Draw::e_aabbBit */ | b2Draw::e_pairBit | b2Draw::e_centerOfMassBit);
  }

  void PhysicsDraw::DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) {
    gf::Polygon polygon;

    for (int32 i = 0; i < vertexCount; ++i) {
      polygon.addPoint(toVec(vertices[i]));
    }

    m_debug.addPolygon(std::move(polygon), toColor(color));
  }

  void PhysicsDraw::DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) {
    gf::Polygon polygon;

    for (int32 i = 0; i < vertexCount; ++i) {
      polygon.addPoint(toVec(vertices[i]));
    }

    m_debug.addSolidPolygon(std::move(polygon), toColor(color));
  }

  void PhysicsDraw::DrawCircle(const b2Vec2& center, float32 radius, const b2Color& color) {
    m_debug.addCircle(gf::CircF(toVec(center), radius / PhysicsScale), toColor(color));
  }

  void PhysicsDraw::DrawSolidCircle(const b2Vec2& center, float32 radius, const b2Vec2& axis, const b2Color& color) {
    m_debug.addSolidCircle(gf::CircF(toVec(center), radius / PhysicsScale), toVec(axis), toColor(color));
  }

  void PhysicsDraw::DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color) {
    m_debug.addSegment(toVec(p1), toVec(p2), toColor(color));
  }

  void PhysicsDraw::DrawTransform(const b2Transform& xf) {
    m_debug.addTransform(toVec(xf.p), toVec(xf.q.GetXAxis()), toVec(xf.q.GetYAxis()));
  }

  void PhysicsDraw::DrawPoint(const b2Vec2& p, float32 size, const b2Color& color) {

  }

}
