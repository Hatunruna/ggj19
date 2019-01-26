#include "ResourceManager.h"

namespace home {

  namespace {
    template<typename T>
    class ResourceLoader {
    public:
      std::unique_ptr<T> operator()(const gf::Path& filename) {
        auto ptr = std::make_unique<T>();
        bool loaded = ptr->loadFromFile(filename.string());
        return loaded ? std::move(ptr) : nullptr;
      }
    };

    class MusicLoader {
    public:
      std::unique_ptr<sf::Music> operator()(const gf::Path& filename) {
        auto ptr = std::make_unique<sf::Music>();
        bool loaded = ptr->openFromFile(filename.string());
        return loaded ? std::move(ptr) : nullptr;
      }
    };
  }

  ResourceManager::ResourceManager()
  : m_sounds(ResourceLoader<sf::SoundBuffer>())
  , m_musics(MusicLoader())
  {

  }

  sf::SoundBuffer& ResourceManager::getSound(const gf::Path &path) {
    return m_sounds.getResource(*this, path);
  }

  sf::Music& ResourceManager::getMusic(const gf::Path &path) {
    return m_musics.getResource(*this, path);
  }
}
