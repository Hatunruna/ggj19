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

#include "local/FieldOfView.h"
#include "local/Map.h"
#include "local/Messages.h"
#include "local/Singletons.h"
#include "config.h"

int main() {
  static constexpr gf::Vector2u ScreenSize(1024, 576);
  static constexpr gf::Vector2f ViewSize(100.0f, 100.0f); // dummy values
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

  gf::EntityContainer mainEntities;
  // add entities to mainEntities
  mainEntities.addEntity(map);
  mainEntities.addEntity(fov);

  gf::EntityContainer hudEntities;
  // add entities to hudEntities


  // game loop

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
          home::CursorPosition pos;
          pos.position = mapCoord;
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
