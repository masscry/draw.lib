#include <draw.hpp>
#include <menu.hpp>
#include <iostream>

enum evoidEvents_t
{
  EE_EXIT = 0
};

class evoidInputController_t final: public draw::inputListener_t
{
  draw::system_t::listOfFrameStages_t::iterator menuScreen;

  void OnKeyPressed(draw::keyInputCodes_t id) override
  {
    auto mmp = static_cast<mainMenuPage_t*>(menuScreen->get());

    switch (id)
    {
    case draw::KEY_UP:
      mmp->Prev();
      break;
    case draw::KEY_DOWN:
      mmp->Next();
      break;
    case draw::KEY_SELECT:
      if (mmp->Selected() == ML_EXIT)
      {
        draw::system_t::Instance().Event(EE_EXIT);
      }
      break;
    default:
      /* Ignore other keys */
      break;
    }
  }

public:

  evoidInputController_t(draw::system_t::listOfFrameStages_t::iterator menuScreen)
  :menuScreen(menuScreen)
  {
    ;
  }

};

class evoidEventController_t: public draw::eventListener_t
{

  void OnUserEvent(int eventID) override
  {
    if (eventID == EE_EXIT)
    {
      draw::system_t::Instance().StopSystem();
    }
  }

public:

  evoidEventController_t() = default;
  ~evoidEventController_t() = default;

};

int main(int /*argc*/, char* /*argv*/[])
{
  try
  {
    auto& instance = draw::system_t::Instance();

    auto menu = instance.AddFrameStage(new mainMenuPage_t());
    auto event = instance.AddEventListener(new evoidEventController_t());
    auto input = instance.AddInputListener(new evoidInputController_t(menu));
    while(instance.IsRunning())
    {
      instance.Render();
      instance.Update();
    }
    instance.RemoveInputListener(input);
    instance.RemoveEventListener(event);
    instance.RemoveFrameStage(menu);
  }
  catch(const draw::error_t& err)
  {
    std::cerr << err.whatFile() << ':' << err.whatLine() << ": " << err.what() << std::endl;
    std::cerr << "=== STACK TRACE ===" << std::endl;
    std::cerr << err.whatTrace() << std::endl;
    return -1;
  }

  return 0;
}
