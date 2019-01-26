#ifndef HOME_FIELD_OF_VIEW_H
#define HOME_FIELD_OF_VIEW_H

#include <gf/Entity.h>
#include <gf/Texture.h>

namespace home {
  class FieldOfView: public gf::Entity {
  public:
    FieldOfView();

    virtual void render(gf::RenderTarget& target, const gf::RenderStates& states) override;

  private:
    gf::Texture &m_texture;
  };
}

#endif // HOME_FIELD_OF_VIEW_H
