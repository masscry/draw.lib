#pragma once
#ifndef __EVOID_SCENES_HEADER__
#define __EVOID_SCENES_HEADER__

#include "menu.hpp"

namespace evoid
{

  using sceneConstructFunc_t = draw::eventListener_t* ();

  template<typename T>
  struct scene_t
  {
    static draw::eventListener_t* Construct()
    {
      return new T();
    }
  };

  std::unordered_map<std::string, sceneConstructFunc_t*>& Scenes();

}

#endif /* __EVOID_SCENES_HEADER__ */