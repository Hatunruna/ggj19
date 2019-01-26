#ifndef HOME_FIELD_OF_VIEW_H
#define HOME_FIELD_OF_VIEW_H

#include <gf/Entity.h>
#include <gf/Message.h>
#include <gf/Texture.h>

namespace home {
  class FieldOfView: public gf::Entity {
  public:
    FieldOfView();

    virtual void render(gf::RenderTarget& target, const gf::RenderStates& states) override;

    gf::MessageStatus onHeroPosition(gf::Id id, gf::Message *msg);

  private:
    gf::Texture &m_texture;
    gf::Vector2f m_position;
  };
}

#endif // HOME_FIELD_OF_VIEW_H
