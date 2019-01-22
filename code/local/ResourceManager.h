#ifndef GGJ_RESOURCEMANAGER_H
#define GGJ_RESOURCEMANAGER_H

#include <gf/ResourceManager.h>
#include <SFML/Audio.hpp>

namespace ggj {

  class ResourceManager : public gf::ResourceManager {
  public:
    ResourceManager();
    sf::SoundBuffer& getSound(const gf::Path &path);
    sf::Music& getMusic(const gf::Path &path);

  private:
    gf::ResourceCache<sf::SoundBuffer> m_sounds;
    gf::ResourceCache<sf::Music> m_musics;
  };

}

#endif // GGJ_RESOURCEMANAGER_H
