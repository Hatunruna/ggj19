cmake_minimum_required(VERSION 3.0)

project(HOME
  VERSION 0.1.0
)

include(GNUInstallDirs)

find_package(gf REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(SFML2 REQUIRED sfml-audio>=2.1)

set(GGJ_DATA_DIR "${CMAKE_INSTALL_PREFIX}/share/games/krokodile")
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/code/config.h.in ${CMAKE_CURRENT_BINARY_DIR}/config.h @ONLY)

if(NOT MSVC)
  add_definitions(-Wall -Wextra -g -O2 -std=c++14 -pedantic)
endif()

add_executable(home
  code/main.cc
  code/local/Map.cc
  code/local/ResourceManager.cc
  code/local/Singletons.cc
)

target_include_directories(home
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/code
    ${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(home
  gf::gf0
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
