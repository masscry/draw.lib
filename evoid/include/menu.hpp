#pragma once
#ifndef __EVOID_MENU_HEADER__
#define __EVOID_MENU_HEADER__

#include <draw.hpp>

class mainMenuPage_t: public draw::frameStage_t 
{
  draw::glSharedResource_t menuFont;
  draw::glSharedResource_t shader;

  draw::camera_t menuCam;
  draw::actor_t spaceShip;
  
  std::list<draw::actor_t> menuLines;
  std::list<draw::actor_t>::iterator menuSelected;
  int menuSelectedUniform;

  void OnRender() override;

protected:

  ~mainMenuPage_t() = default;

public:

  void Next();
  void Prev();

  mainMenuPage_t();

};

#endif /* __EVOID_MENU_HEADER__ */