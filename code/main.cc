#include <gf/Action.h>
#include <gf/Clock.h>
#include <gf/Color.h>
#include <gf/EntityContainer.h>
#include <gf/Event.h>
#include <gf/Log.h>
#include <gf/RenderWindow.h>
#include <gf/Singleton.h>
#include <gf/ViewContainer.h>
#include <gf/Views.h>
#include <gf/Window.h>

#include <SFML/Audio.hpp>

#include "local/FieldOfView.h"
#include "local/Map.h"
#include "local/Messages.h"
#include "local/Player.h"
#include "local/Singletons.h"
#include "config.h"

int main() {
  static constexpr gf::Vector2u ScreenSize(1024, 576);
  static constexpr gf::Vector2f ViewSize(800.0f, 800.0f); // dummy values
  static constexpr gf::Vector2f ViewCenter(0.0f, 0.0f); // dummy values

  // initialization

  gf::Window window("Game", ScreenSize);
  window.setVerticalSyncEnabled(true);
  window.setFramerateLimit(60);

  gf::RenderWindow renderer(window);

  // singletons

  gf::SingletonStorage<home::ResourceManager> storageForResourceManager(home::gResourceManager);
  gf::Log::debug("data dir: %s\n", HOME_DATA_DIR);
  home::gResourceManager().addSearchDir(HOME_DATA_DIR);

  gf::SingletonStorage<gf::MessageManager> storageForMessageManager(home::gMessageManager);
  gf::SingletonStorage<gf::Random> storageForRandom(home::gRandom);

  // views

  gf::ViewContainer views;

  gf::ExtendView mainView(ViewCenter, ViewSize);
  views.addView(mainView);

  gf::ScreenView hudView;
  views.addView(hudView);

  views.setInitialScreenSize(ScreenSize);

  // background music
  sf::Sound bgm(home::gResourceManager().getSound("sounds/main_theme.ogg"));
  bgm.setLoop(true);
  bgm.setVolume(10);
  bgm.play();

  // actions

  gf::ActionContainer actions;

  gf::Action closeWindowAction("Close window");
  closeWindowAction.addCloseControl();
  closeWindowAction.addKeycodeKeyControl(gf::Keycode::Escape);
  actions.addAction(closeWindowAction);

  gf::Action fullscreenAction("Fullscreen");
  fullscreenAction.addKeycodeKeyControl(gf::Keycode::F);
  actions.addAction(fullscreenAction);

  gf::Action leftAction("Left");
  leftAction.addScancodeKeyControl(gf::Scancode::A);
  leftAction.addScancodeKeyControl(gf::Scancode::Left);
  leftAction.setContinuous();
  actions.addAction(leftAction);

  gf::Action rightAction("Right");
  rightAction.addScancodeKeyControl(gf::Scancode::D);
  rightAction.addScancodeKeyControl(gf::Scancode::Right);
  rightAction.setContinuous();
  actions.addAction(rightAction);

  gf::Action upAction("Up");
  upAction.addScancodeKeyControl(gf::Scancode::W);
  upAction.addScancodeKeyControl(gf::Scancode::Up);
  upAction.setContinuous();
  actions.addAction(upAction);

  gf::Action downAction("Down");
  downAction.addScancodeKeyControl(gf::Scancode::S);
  downAction.addScancodeKeyControl(gf::Scancode::Down);
  downAction.setContinuous();
  actions.addAction(downAction);

  // entities
  home::Map map;
  home::FieldOfView fov;
  home::Player player;

  gf::EntityContainer mainEntities;
  // add entities to mainEntities
  mainEntities.addEntity(map);
  mainEntities.addEntity(fov);
  mainEntities.addEntity(player);

  gf::EntityContainer hudEntities;
  // add entities to hudEntities


  // game loop

  home::gMessageManager().registerHandler<home::HeroPosition>([&mainView](gf::Id type, gf::Message *msg) {
    assert(type == home::HeroPosition::type);
    auto positionHeroMessage = static_cast<home::HeroPosition*>(msg);
    mainView.setCenter(positionHeroMessage->position);
    return gf::MessageStatus::Keep;
  });

  renderer.clear(gf::Color::White);

  gf::Clock clock;

  while (window.isOpen()) {
    // 1. input

    gf::Event event;

    while (window.pollEvent(event)) {
      actions.processEvent(event);
      views.processEvent(event);
      switch (event.type) {
        case gf::EventType::MouseMoved:
        {
          auto mapCoord = renderer.mapPixelToCoords(event.mouseCursor.coords, mainView);
          home::CursorMovedPosition pos;
          pos.position = mapCoord;
          home::gMessageManager().sendMessage(&pos);
          break;
        }
        case gf::EventType::MouseButtonReleased:
        {
          auto coord = renderer.mapPixelToCoords(event.mouseButton.coords, mainView);
          home::CursorClickedPosition pos;
          pos.position = coord;
          home::gMessageManager().sendMessage(&pos);break;
        }
        default:break;
      }
    }

    if (closeWindowAction.isActive()) {
      window.close();
    }

    if (fullscreenAction.isActive()) {
      window.toggleFullscreen();
    }

    if (rightAction.isActive()) {
      // do something
    } else if (leftAction.isActive()) {
      // do something
    } else if (upAction.isActive()) {
      // do something
    } else if (downAction.isActive()) {
      // do something
    } else {
      // do something
    }


    // 2. update

    gf::Time time = clock.restart();
    mainEntities.update(time);
    hudEntities.update(time);


    // 3. draw

    renderer.clear();

    renderer.setView(mainView);
    mainEntities.render(renderer);

    renderer.setView(hudView);
    hudEntities.render(renderer);

    renderer.display();

    actions.reset();
  }

  return 0;
}
