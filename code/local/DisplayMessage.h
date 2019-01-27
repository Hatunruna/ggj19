#ifndef HOME_DISPLAYMESSAGE_H
#define HOME_DISPLAYMESSAGE_H

#include <deque>

#include <gf/Entity.h>
#include <gf/Message.h>
#include <gf/Font.h>

#include "Messages.h"

namespace home {
  struct Notification {
    std::string message;
    gf::Time time;
    gf::Time initTime;
  };

  class DisplayMessage : public gf::Entity {
    public :
    DisplayMessage();
      virtual void render(gf::RenderTarget& target, const gf::RenderStates& states) override;
      virtual void update(gf::Time time) override;
      gf::MessageStatus onMessageReceived(gf::Id id, gf::Message *msg);

    private:
      enum class State {
        WaitingMessage,
        DisplayingMessage,
      };

    private :
      State m_state;
      std::deque<Notification> m_notif;
      gf::Font &m_font;
  };
}

#endif // HOME_DISPLAYMESSAGE_H
