cmake_minimum_required(VERSION 3.0)

project(HOME
  VERSION 0.1.0
)

include(GNUInstallDirs)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/modules")

find_package(gf REQUIRED)
find_package(Box2D REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(SFML2 REQUIRED sfml-audio>=2.1)

set(HOME_DATA_DIR "${CMAKE_INSTALL_PREFIX}/share/games/home")
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/code/config.h.in ${CMAKE_CURRENT_BINARY_DIR}/config.h @ONLY)

if(NOT MSVC)
  add_definitions(-Wall -Wextra -g -O2 -std=c++14 -pedantic)
endif()

add_executable(home
  code/main.cc
  code/local/ClockHud.cc
  code/local/FieldOfView.cc
  code/local/Map.cc
  code/local/Physics.cc
  code/local/Player.cc
  code/local/ResourceManager.cc
  code/local/ResourcesViewer.cc
  code/local/Singletons.cc
  code/local/SupplyManager.cc
)

target_include_directories(home
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/code
    ${CMAKE_CURRENT_BINARY_DIR}
    ${BOX2D_INCLUDE_DIRS}
)

target_link_libraries(home
  gf::gf0
  ${BOX2D_LIBRARIES}
  ${SFML2_LIBRARIES}
)

install(
  TARGETS home
  RUNTIME DESTINATION games
)

install(
  DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/data/home"
  DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/games"
)
