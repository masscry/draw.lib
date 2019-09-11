#pragma once
#ifndef __EX000_CONSOLE_HEADER__
#define __EX000_CONSOLE_HEADER__

#include <draw.hpp>

class consoleView_t:public draw::frameStage_t
{
  draw::camera_t orthoCam;

  draw::actor_t console;

  draw::glSharedResource_t shader;
  draw::glSharedResource_t fontTexture;

  void OnRender() override;

protected:

  ~consoleView_t() = default;
  
public:

  draw::actor_t& Console() {
    return this->console;
  }

  consoleView_t();

};

#endif /* __EX000_CONSOLE_HEADER__ */