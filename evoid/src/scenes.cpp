#include <scenes.hpp>

namespace evoid
{
  std::unordered_map<std::string, sceneConstructFunc_t*>& Scenes()
  {
    static std::unordered_map<std::string, sceneConstructFunc_t*> self = {
      std::make_pair(std::string("MainMenu"), scene_t<evoid::menuPresenter_t>::Construct)
    };
    return self;
  }
}
