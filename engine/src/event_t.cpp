#include <draw.hpp>

namespace draw
{

  eventListener_t::~eventListener_t()
  {
    assert(this->RegisteredID() < 0);
  }

  eventListener_t::eventListener_t()
  :registeredID(-1)
  {
    ;
  }

  void eventListener_t::SendEvent(int eventID)
  {
    system_t::Instance().Debug("Event %d\n", eventID);
    this->OnUserEvent(eventID);
  }

}