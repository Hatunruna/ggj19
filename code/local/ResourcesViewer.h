#ifndef HOME_RESOURCESVIEWER_H
#define HOME_RESOURCESVIEWER_H

#include <gf/Entity.h>

namespace home {
  class ResourcesViewer : public gf::Entity {
    public :
      ResourcesViewer();
      virtual void render(gf::RenderTarget& target, const gf::RenderStates& states) override;
      virtual void update(gf::Time time) override;
    private :
      float m_oxygen;
  };
}

#endif // HOME_RESOURCESVIEWER_H
