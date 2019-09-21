#include <draw.hpp>
#include <menu.hpp>
#include <iostream>

class evoidController_t final: public draw::inputListener_t
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
    default:
      /* Ignore other keys */
      break;
    }
  }

public:

  evoidController_t(draw::system_t::listOfFrameStages_t::iterator menuScreen)
  :menuScreen(menuScreen)
  {
    ;
  }

};

int main(int /*argc*/, char* /*argv*/[])
{
  try
  {
    auto& instance = draw::system_t::Instance();

    auto menu = instance.AddFrameStage(new mainMenuPage_t());
    auto control = instance.AddInputListener(new evoidController_t(menu));
    while(instance.IsRunning())
    {
      instance.Render();
      instance.Update();
    }
    instance.RemoveInputListener(control);
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
