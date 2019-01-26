#ifndef HOME_OXYGENHUD_H
#define HOME_OXYGENHUD_H

#include <gf/Entity.h>
#include <gf/Texture.h>

namespace home {
  class OxygenHud : public gf::Entity {
    public :
    OxygenHud();
      virtual void render(gf::RenderTarget& target, const gf::RenderStates& states) override;
      virtual void update(gf::Time time) override;
    private :
      float m_oxygen;
      gf::Texture &m_oxygenIcon;
  };
}

#endif // HOME_OXYGENHUD_H
