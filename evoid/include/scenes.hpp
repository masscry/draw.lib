#pragma once
#ifndef __EVOID_SCENES_HEADER__
#define __EVOID_SCENES_HEADER__

#include "menu.hpp"

namespace evoid
{

  using sceneConstructFunc_t = draw::constructEventListenerFunc_t;
  
  template<typename T>
  struct scene_t
  {
    static draw::eventListener_t* Construct(draw::eventListener_t::list_t* parent)
    {
      return new T(parent);
    }
  };

  std::unordered_map<std::string, sceneConstructFunc_t*>& Scenes();

}

#endif /* __EVOID_SCENES_HEADER__ */