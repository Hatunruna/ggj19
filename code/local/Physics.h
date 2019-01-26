#ifndef HOME_PHYSICS_H
#define HOME_PHYSICS_H

#include <Box2D/Box2D.h>

#include <gf/Model.h>
#include <gf/Tmx.h>

namespace home {

  class Physics : public gf::Model {
  public:
    Physics(const gf::TmxLayers& layers);

    virtual void update(gf::Time time) override;

  private:
    b2World m_world;
  };

}


#endif // HOME_PHYSICS_H
