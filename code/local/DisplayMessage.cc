#include "DisplayMessage.h"

#include <gf/Coordinates.h>
#include <gf/Log.h>
#include <gf/RenderTarget.h>
#include <gf/StringUtils.h>
#include <gf/Text.h>

#include "Singletons.h"

namespace home {
  DisplayMessage::DisplayMessage()
  : m_state(State::Idle)
  , m_displayTime(0.0f)
  , m_font(gResourceManager().getFont("fonts/dejavu_sans.ttf")) {
    gMessageManager().registerHandler<MessageToDisplay>(&DisplayMessage::onMessageReceived, this);
  }

  void DisplayMessage::render(gf::RenderTarget& target, const gf::RenderStates& states) {
    if (m_state == State::Displaying && m_displayTime > 0) {
      gf::Coordinates coordinates(target);
      gf::Text text;
      text.setFont(m_font);
      text.setColor({0.0f, 0.0f, 0.0f, 1.0f - m_time.asSeconds() / m_displayTime});
      text.setOutlineColor({1.0f, 1.0f, 1.0f, 1.0f - m_time.asSeconds() / m_displayTime});
      text.setOutlineThickness(coordinates.getRelativeCharacterSize(0.008f));
      text.setCharacterSize(coordinates.getRelativeCharacterSize(0.1f));
      text.setString(m_message);
      text.setParagraphWidth(target.getSize().x);
      text.setPosition(coordinates.getRelativeSize({0.5f, 0.5f}));
      text.setAlignment(gf::Alignment::Center);
      text.setAnchor(gf::Anchor::Center);
      target.draw(text, states);
    }/* else if (m_time.asSeconds() >= m_displayTime) {
      m_state = State::Finished;
    }*/
  }

  void DisplayMessage::update(gf::Time time) {
    switch (m_state) {
      case State::Displaying:
      m_time += time;
      if (m_time.asSeconds() >= m_displayTime) { 
        m_state = State::Finished;
      }
      ;break;
      case State::Finished:
        m_time = gf::Time::zero();
        m_state = State::Idle;
        break;
      default: gf::Log::debug("I went here yooo\n"); break;
    }
  }

  gf::MessageStatus DisplayMessage::onMessageReceived(gf::Id id, gf::Message *msg) {
    assert(id == MessageToDisplay::type);
    MessageToDisplay *message = static_cast<MessageToDisplay*>(msg);
    m_message = message->message;
    m_displayTime = message->displayTime;
    m_state = State::Displaying;
    gf::Log::debug("Message received\n");
    return gf::MessageStatus::Keep;
  }
}
