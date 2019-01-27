#ifndef HOME_RESOURCESHUD_H
#define HOME_RESOURCESHUD_H

#include <gf/Entity.h>
#include <gf/Font.h>
#include <gf/Texture.h>

#include "Messages.h"

namespace home {
  class ResourcesHud : public gf::Entity {
  public :
    ResourcesHud();
    virtual void render(gf::RenderTarget& target, const gf::RenderStates& states) override;
    virtual void update(gf::Time time) override;

    gf::MessageStatus onResourceHarvested(gf::Id id, gf::Message *msg);
    gf::MessageStatus onUnloadBackpack(gf::Id id, gf::Message *msg);
    gf::MessageStatus onInfoSupplies(gf::Id id, gf::Message *msg);

  private :
    float m_minerals;
    float m_energy;
    gf::Time m_time;
    gf::Texture &m_backpackIcon;
    gf::Texture &m_mineralsIcon;
    gf::Texture &m_energyIcon;
    gf::Font &m_font;
    float m_cristalQuantity;
    float m_metalQuantity;
    bool m_messageSended;
  };
}

#endif // HOME_RESOURCESHUD_H
