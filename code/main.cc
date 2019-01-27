#include <thread>

#include <gf/Action.h>
#include <gf/Clock.h>
#include <gf/Color.h>
#include <gf/Cursor.h>
#include <gf/EntityContainer.h>
#include <gf/Event.h>
#include <gf/Log.h>
#include <gf/RenderWindow.h>
#include <gf/Singleton.h>
#include <gf/Unused.h>
#include <gf/ViewContainer.h>
#include <gf/Views.h>
#include <gf/Window.h>

#include <SFML/Audio.hpp>

#include "local/CrashedShip.h"
#include "local/ClockHud.h"
#include "local/DisplayMessage.h"
#include "local/FieldOfView.h"
#include "local/Map.h"
#include "local/Messages.h"
#include "local/OxygenHud.h"
#include "local/Physics.h"
#include "local/Player.h"
#include "local/ResourcesHud.h"
#include "local/Singletons.h"
#include "local/SupplyManager.h"
#include "config.h"

using namespace std::literals;

int main() {
  static constexpr gf::Vector2u ScreenSize(1024, 576);
  static constexpr gf::Vector2f ViewSize(800.0f, 800.0f);
  static constexpr gf::Vector2f ViewCenter(0.0f, 0.0f);

  static constexpr float MaxVol = 100.0f;

  // initialization

  gf::Window window("H.O.M.E. - Harvest Oxygen in the Maldoran Ecosystem", ScreenSize);
  window.setVerticalSyncEnabled(true);
  window.setFramerateLimit(60);

  gf::RenderWindow renderer(window);

  // singletons

  gf::SingletonStorage<home::ResourceManager> storageForResourceManager(home::gResourceManager);
  home::gResourceManager().addSearchDir(HOME_DATA_DIR);

  gf::SingletonStorage<gf::MessageManager> storageForMessageManager(home::gMessageManager);
  gf::SingletonStorage<gf::Random> storageForRandom(home::gRandom);

  // cursor

  gf::Image image;
  image.loadFromFile(home::gResourceManager().getAbsolutePath("cursor.png"));

  gf::Cursor cursor;
  cursor.loadFromImage(image, { 32u, 32u });

  window.setMouseCursor(cursor);


  // views

  gf::ViewContainer views;

  gf::ExtendView mainView(ViewCenter, ViewSize);
  views.addView(mainView);

  gf::ScreenView hudView;
  views.addView(hudView);

  views.setInitialScreenSize(ScreenSize);

  // display splash screen
  renderer.setView(hudView);
  renderer.clear();
  gf::Sprite sprite;
  sprite.setTexture(home::gResourceManager().getTexture("images/home_page.jpg"));
  sprite.scale(0.55f);
  renderer.draw(sprite);
  renderer.display();
  std::this_thread::sleep_for(2s);

  // background music
  float bgmVol = 10.0f;
  bool bgmMuted = true;
  sf::Sound bgm(home::gResourceManager().getSound("sounds/main_theme.ogg"));
  bgm.setLoop(true);
  bgm.setVolume(0.0f);
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

  gf::Action muteBgmAction("Mute");
  muteBgmAction.addKeycodeKeyControl(gf::Keycode::M);
  actions.addAction(muteBgmAction);

  gf::Action volumeUpAction("VolUp");
  volumeUpAction.addKeycodeKeyControl(gf::Keycode::O);
  actions.addAction(volumeUpAction);

  gf::Action volumeDownAction("VolDown");
  volumeDownAction.addKeycodeKeyControl(gf::Keycode::L);
  actions.addAction(volumeDownAction);

  // entities

  gf::Path filename = home::gResourceManager().getAbsolutePath("map/Map.tmx");
  gf::TmxLayers layers;
  if (!layers.loadFromFile(filename)) {
    gf::Log::error("Unable to load the map!\n");
    return EXIT_FAILURE;
  }

  home::SupplyManager supplies;
  home::MapGraphicsData data(layers, supplies);
  home::Map mapAbove(home::Map::Above, data);
  home::Map mapBelow(home::Map::Below, data);

  home::FieldOfView fov;
  home::Player player;
  home::ClockHud clockHud;
  home::OxygenHud oxygenHud;
  home::ResourcesHud resourcesHud;
  home::DisplayMessage displayMessage;
  home::CrashedShip ship;

  home::Physics physics(layers, player);
  home::PhysicsDebugger debugger(physics);
  home::PhysicsDraw draw(debugger);

//   physics.setDraw(&draw);

  gf::EntityContainer mainEntities;
  // add entities to mainEntities
  mainEntities.addEntity(mapAbove);
  mainEntities.addEntity(mapBelow);
  mainEntities.addEntity(fov);
  mainEntities.addEntity(player);
  mainEntities.addEntity(supplies);
  mainEntities.addEntity(debugger);
  mainEntities.addEntity(ship);

  gf::EntityContainer hudEntities;
  // add entities to hudEntities
  hudEntities.addEntity(clockHud);
  hudEntities.addEntity(oxygenHud);
  hudEntities.addEntity(resourcesHud);
  hudEntities.addEntity(displayMessage);



  // game loop

  home::gMessageManager().registerHandler<home::HeroPosition>([&mainView](gf::Id type, gf::Message *msg) {
    assert(type == home::HeroPosition::type);
    auto positionHeroMessage = static_cast<home::HeroPosition*>(msg);
    mainView.setCenter(positionHeroMessage->position);
    return gf::MessageStatus::Keep;
  });

  bool isGameOver = false;
  home::gMessageManager().registerHandler<home::GameOver>([&isGameOver](gf::Id type, gf::Message *msg) {
    assert(type == home::GameOver::type);
    gf::unused(type, msg);
    isGameOver = true;
    return gf::MessageStatus::Keep;
  });

  renderer.clear(gf::Color::Gray(0.2f));

  gf::Clock clock;

  while (window.isOpen()) {
    // 1. input

    gf::Event event;

    while (window.pollEvent(event)) {
      actions.processEvent(event);
      views.processEvent(event);
      if (!isGameOver) {
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
    }

    if (closeWindowAction.isActive()) {
      window.close();
    }

    if (fullscreenAction.isActive()) {
      window.toggleFullscreen();
    }

    // Sound control
    if (muteBgmAction.isActive()) {
      bgmMuted = !bgmMuted;
      if (bgmMuted) {
        bgm.setVolume(0.0f);
      } else {
        bgm.setVolume(bgmVol);
      }
    }
    if (volumeUpAction.isActive()) {
      bgmVol = bgmVol >= MaxVol ? MaxVol : bgmVol + 5.0f;
      if (!bgmMuted) {
        bgm.setVolume(bgmVol);
      }
    }
    if (volumeDownAction.isActive()) {
      bgmVol = bgmVol <= 0.0f ? 0.0f : bgmVol - 5.0f;
      if (!bgmMuted) {
        bgm.setVolume(bgmVol);
      }
    }

    // 2. update

    gf::Time time = clock.restart();
    mainEntities.update(time);
    hudEntities.update(time);
    physics.update(time);


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
