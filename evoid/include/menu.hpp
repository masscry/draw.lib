#pragma once
#ifndef __EVOID_MENU_HEADER__
#define __EVOID_MENU_HEADER__

#include <draw.hpp>

namespace evoid
{

  enum menuLines_t 
  {
    ML_FIRST = 0,
    ML_NEW_GAME = ML_FIRST,
    ML_LOAD_GAME,
    ML_ABOUT,
    ML_EXIT,
    ML_TOTAL
  };

  class menuInput_t final: public draw::inputListener_t
  {
    void OnKeyPressed(draw::keyInputCodes_t id) override;
  protected:
    ~menuInput_t() override = default;
  public:
    menuInput_t() = default;
  };


  class menuModel_t final: public draw::eventListener_t
  {
    int selected;

    menuModel_t(const menuModel_t&) = delete;
    menuModel_t(menuModel_t&&) = delete;

    menuModel_t& operator=(const menuModel_t&) = delete;
    menuModel_t& operator=(menuModel_t&&) = delete;

    void OnUserEvent(int eventID) override;

  protected:
    ~menuModel_t() = default;

  public:

    int Selected() const {
      return selected;
    }

    void Next();
    void Prev();
    void Activate();

    menuModel_t(draw::eventListener_t::list_t* parent);

  };

  class menuView_t: public draw::frameStage_t 
  {
    draw::glSharedResource_t menuFont;
    draw::glSharedResource_t shader;

    draw::camera_t menuCam;
    draw::actor_t spaceShip;
    
    std::vector<draw::actor_t> menuLines;
    std::vector<draw::actor_t>::iterator menuSelected;
    int menuSelectedUniform;

    void OnRender() override;

  protected:

    ~menuView_t() = default;

  public:

    menuView_t(const std::vector<std::string>& menuLinesText);

    void SelectItem(int line);

  };

  using menuModelIterator = draw::eventListener_t::list_t::iterator;
  using menuViewIterator = draw::system_t::listOfFrameStages_t::iterator;
  using menuInputIterator = draw::system_t::listOfInputListeners_t::iterator;

  class menuPresenter_t final: public draw::eventListener_t
  {
    menuModelIterator menuModel;
    menuViewIterator menuView;
    menuInputIterator menuInput;

    menuPresenter_t(const menuPresenter_t&) = delete;
    menuPresenter_t(menuPresenter_t&&) = delete;

    menuPresenter_t& operator=(const menuPresenter_t&) = delete;
    menuPresenter_t& operator=(menuPresenter_t&&) = delete;

    void OnUserEvent(int eventID) override;

  public:

    menuPresenter_t(draw::eventListener_t::list_t* parent);
    ~menuPresenter_t() override;
  };

} // namespace evoid

#endif /* __EVOID_MENU_HEADER__ */