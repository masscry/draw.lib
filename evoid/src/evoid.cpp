#include <draw.hpp>
#include <menu.hpp>
#include <events.hpp>
#include <iostream>

int main(int /*argc*/, char* /*argv*/[])
{
  try
  {
    auto& instance = draw::system_t::Instance();

    auto menu = instance.AddEventListener(new evoid::menuPresenter_t());
    while(instance.IsRunning())
    {
      instance.Render();
      instance.Update();
    }
    instance.RemoveEventListener(menu);
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
