#ifndef HOME_OXYGENHUD_H
#define HOME_OXYGENHUD_H

#include <SFML/Audio.hpp>

#include <gf/Entity.h>
#include <gf/Font.h>
#include <gf/Texture.h>


#include "Messages.h"
namespace home {
  class OxygenHud : public gf::Entity {
    public :
    OxygenHud();
      virtual void render(gf::RenderTarget& target, const gf::RenderStates& states) override;
      virtual void update(gf::Time time) override;
      gf::MessageStatus onOxygenHarvested(gf::Id id, gf::Message *msg);
    private :
      float m_oxygen;
      gf::Texture &m_oxygenIcon;

      sf::Sound m_lowO2Sound;
      float m_lowO2Volume;
      bool m_lowO2SoundStarted;
      bool m_gameOver;
      bool m_gameFinished;
      gf::Font &m_font;
  };
}

#endif // HOME_OXYGENHUD_H
