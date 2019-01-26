#ifndef HOME_PHYSICS_H
#define HOME_PHYSICS_H

#include <vector>

#include <Box2D/Box2D.h>

#include <gf/Circ.h>
#include <gf/Entity.h>
#include <gf/Model.h>
#include <gf/Polygon.h>
#include <gf/Tmx.h>

#include "Player.h"

namespace home {
  class PhysicsDraw;

  class Physics : public gf::Model {
  public:
    Physics(const gf::TmxLayers& layers, Player& player);

    void debug();
    void setDraw(PhysicsDraw *draw);

    virtual void update(gf::Time time) override;

  private:
    b2World m_world;
    b2Body *m_hero;
    Player& m_player;
    PhysicsDraw *m_draw;
  };

  class PhysicsDebugger : public gf::Entity {
  public:
    PhysicsDebugger(Physics& physics)
    : gf::Entity(10000)
    , m_physics(physics)
    {

    }

    void addPolygon(gf::Polygon polygon, gf::Color4f color);
    void addSolidPolygon(gf::Polygon polygon, gf::Color4f color);
    void addCircle(gf::CircF circle, gf::Color4f color);
    void addSolidCircle(gf::CircF circle, gf::Vector2f axis, gf::Color4f color);
    void addSegment(gf::Vector2f p1, gf::Vector2f p2, gf::Color4f color);
    void addTransform(gf::Vector2f position, gf::Vector2f xAxis, gf::Vector2f yAxis);

    virtual void render(gf::RenderTarget& target, const gf::RenderStates& states) override;

  private:
    Physics& m_physics;

    struct Polygon {
      gf::Polygon shape;
      gf::Color4f color;
    };

    struct Circle {
      gf::CircF shape;
      gf::Color4f color;
    };

    struct SolidCircle {
      gf::CircF shape;
      gf::Vector2f axis;
      gf::Color4f color;
    };

    struct Segment {
      gf::Vector2f p1;
      gf::Vector2f p2;
      gf::Color4f color;
    };

    struct Transform {
      gf::Vector2f position;
      gf::Vector2f xAxis;
      gf::Vector2f yAxis;
    };

    std::vector<Polygon> m_polygons;
    std::vector<Polygon> m_solidPolygons;
    std::vector<Circle> m_circles;
    std::vector<SolidCircle> m_solidCircles;
    std::vector<Segment> m_segments;
    std::vector<Transform> m_transforms;
  };

  class PhysicsDraw : public b2Draw {
  public:
    PhysicsDraw(PhysicsDebugger& debug);

    /// Draw a closed polygon provided in CCW order.
    virtual void DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) override;

    /// Draw a solid closed polygon provided in CCW order.
    virtual void DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) override;

    /// Draw a circle.
    virtual void DrawCircle(const b2Vec2& center, float32 radius, const b2Color& color) override;

    /// Draw a solid circle.
    virtual void DrawSolidCircle(const b2Vec2& center, float32 radius, const b2Vec2& axis, const b2Color& color) override;

    /// Draw a line segment.
    virtual void DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color) override;

    /// Draw a transform. Choose your own length scale.
    /// @param xf a transform.
    virtual void DrawTransform(const b2Transform& xf) override;

    /// Draw a point.
    virtual void DrawPoint(const b2Vec2& p, float32 size, const b2Color& color);

  private:
    PhysicsDebugger& m_debug;
  };

}

#endif // HOME_PHYSICS_H
