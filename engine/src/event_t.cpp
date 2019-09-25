#include <draw.hpp>

namespace draw
{

  eventListener_t::~eventListener_t()
  {
    assert(this->registeredID == this->parent->end());
  }

  eventListener_t::eventListener_t(list_t* parent)
  :parent(parent)
  {
    assert(this->parent != nullptr);
    this->registeredID = this->parent->end();
  }

  void eventListener_t::SendEvent(int eventID)
  {
    system_t::Instance().Debug("Event %d\n", eventID);
    this->OnUserEvent(eventID);
  }

  void eventListener_t::Discard()
  {
    auto regID = this->registeredID;
    this->registeredID = this->parent->end();
    this->parent->erase(regID);
  }

}