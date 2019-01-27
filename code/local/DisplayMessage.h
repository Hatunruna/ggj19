#ifndef HOME_DISPLAYMESSAGE_H
#define HOME_DISPLAYMESSAGE_H

#include <gf/Entity.h>
#include <gf/Font.h>

#include "Messages.h"

namespace home {
  enum class State: uint8_t {
    Idle = 0,
    Displaying = 1,
    Finished = 2,
  };
  
  class DisplayMessage : public gf::Entity {
    public :
    DisplayMessage();
      virtual void render(gf::RenderTarget& target, const gf::RenderStates& states) override;
      virtual void update(gf::Time time) override;
      gf::MessageStatus onMessageReceived(gf::Id id, gf::Message *msg);        
    private :
      State m_state;
      float m_displayTime;
      std::string m_message;
      gf::Time m_time;
      gf::Font &m_font;
  };
}

#endif // HOME_DISPLAYMESSAGE_H