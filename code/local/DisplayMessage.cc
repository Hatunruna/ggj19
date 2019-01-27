#include "DisplayMessage.h"

#include <gf/Coordinates.h>
#include <gf/Log.h>
#include <gf/RenderTarget.h>
#include <gf/StringUtils.h>
#include <gf/Text.h>

#include "Singletons.h"

namespace home {
  DisplayMessage::DisplayMessage()
  : m_state(State::WaitingMessage)
  , m_font(gResourceManager().getFont("fonts/dejavu_sans.ttf")) {
    gMessageManager().registerHandler<MessageToDisplay>(&DisplayMessage::onMessageReceived, this);
  }

  void DisplayMessage::render(gf::RenderTarget& target, const gf::RenderStates& states) {
    if (m_state == State::DisplayingMessage) {
      auto &notif = m_notif.front();
      gf::Coordinates coordinates(target);
      gf::Text text;

      auto timeRatio = notif.time.asSeconds() / notif.initTime.asSeconds();
      text.setFont(m_font);
      text.setColor({0.0f, 0.0f, 0.0f, 1.0f - timeRatio});
      text.setOutlineColor({1.0f, 1.0f, 1.0f, 1.0f - timeRatio});
      text.setOutlineThickness(coordinates.getRelativeCharacterSize(0.008f));
      text.setCharacterSize(coordinates.getRelativeCharacterSize(0.1f));
      text.setString(notif.message);
      text.setParagraphWidth(target.getSize().x);
      text.setPosition(coordinates.getRelativeSize({0.5f, 0.5f}));
      text.setAlignment(gf::Alignment::Center);
      text.setAnchor(gf::Anchor::Center);
      target.draw(text, states);
    }
  }

  void DisplayMessage::update(gf::Time time) {
    // Remove old message
    bool cont = false;
    do {
      cont = false;
      if (m_notif.size() != 0 && m_notif.front().time >= m_notif.front().initTime) {
        m_notif.pop_front();
        cont = true;
      }
    } while (m_notif.size() != 0 && cont);

    if (m_notif.size() != 0) {
      m_state = State::DisplayingMessage;
      auto &notif = m_notif.front();
      notif.time += time;
      return;
    }

    m_state = State::WaitingMessage;
  }

  gf::MessageStatus DisplayMessage::onMessageReceived(gf::Id id, gf::Message *msg) {
    assert(id == MessageToDisplay::type);
    MessageToDisplay *message = static_cast<MessageToDisplay*>(msg);

    gf::Time time = gf::seconds(message->displayTime);

    if (m_notif.size() == 0 || m_notif.back().message != message->message) {
      m_notif.push_back({ message->message, gf::Time::zero(), time });
      gf::Log::debug("Message Received: %s\n", message->message.c_str());
    }

    return gf::MessageStatus::Keep;
  }
}
