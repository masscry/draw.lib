#include <draw.hpp>
#include <events.hpp>
#include <scenes.hpp>
#include <iostream>

int main(int /*argc*/, char* /*argv*/[])
{
  try
  {
    auto& instance = draw::system_t::Instance();

    instance.ConstructEventListener(
      evoid::Scenes()[instance.Settings().Param("scene/start", "MainMenu")]
    );
    while(instance.IsRunning())
    {
      instance.Render();
      instance.Update();
    }
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
