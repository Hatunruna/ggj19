#include "ClockHud.h"

#include <gf/Coordinates.h>
#include <gf/RenderTarget.h>
#include <gf/StringUtils.h>
#include <gf/Text.h>

#include "Messages.h"
#include "Singletons.h"

namespace home {
  ClockHud::ClockHud()
  : m_font(gResourceManager().getFont("fonts/dejavu_sans.ttf"))
  , m_inGameTime(0.0f)
  , m_numDays(0) {
    gMessageManager().registerHandler<ClockInfo>(&ClockHud::onClockInfo, this);
  }

  void ClockHud::render(gf::RenderTarget& target, const gf::RenderStates& states) {
    float hour = std::floor(std::floor(m_inGameTime));
    float min = std::floor((m_inGameTime - hour) * 60.0f);
    std::string hourStr = gf::niceNum(hour, 1);
    std::string minStr = gf::niceNum(min, 1);

    if (hour < 10) {
      hourStr = "0" + hourStr;
    }
    if (min < 10) {
      minStr = "0" + minStr;
    }

    gf::Coordinates coordinates(target);

    gf::Text text;
    text.setFont(m_font);
    text.setOutlineColor(gf::Color::White);
    text.setOutlineThickness(coordinates.getRelativeCharacterSize(0.002f));
    text.setCharacterSize(coordinates.getRelativeCharacterSize(0.03f));
    text.setString("Day " + std::to_string(m_numDays) + "\n" + hourStr + ":" + minStr);
    text.setParagraphWidth(coordinates.getRelativeCharacterSize(0.03f) * 5.0f);
    text.setAnchor(gf::Anchor::TopRight);
    text.setPosition(coordinates.getRelativeSize({0.95f, 0.05f}));
    text.setAlignment(gf::Alignment::Center);
    target.draw(text, states);
  }

  gf::MessageStatus ClockHud::onClockInfo(gf::Id id, gf::Message *msg) {
    assert(id == ClockInfo::type);
    auto info = static_cast<ClockInfo*>(msg);

    m_inGameTime = info->time;
    m_numDays = info->nbDays;

    return gf::MessageStatus::Keep;
  }
}
