#pragma once
#ifndef __DRAW_LIB_EVENT_HEADER__
#define __DRAW_LIB_EVENT_HEADER__

namespace draw
{

  class eventListener_t
  {
    friend class system_t;

    int registeredID;

    virtual void OnUserEvent(int eventID) = 0;

    eventListener_t(const eventListener_t&) = delete;
    eventListener_t(eventListener_t&&) = delete;

    eventListener_t& operator = (const eventListener_t&) = delete;
    eventListener_t& operator = (eventListener_t&&) = delete;

    void SetRegisteredID(int id)
    {
      this->registeredID = id;
    }

  protected:

    virtual ~eventListener_t();

  public:

    int RegisteredID() const
    {
      return this->registeredID;
    }

    struct Destructor
    {
      void operator()(eventListener_t* oldEventListener)
      {
        delete oldEventListener;
      }
    };

    void SendEvent(int eventID);

    eventListener_t();

  };

  typedef std::unique_ptr<eventListener_t, eventListener_t::Destructor> uniqueEventListener_t;

} // namespace draw

#endif /* __DRAW_LIB_EVENT_HEADER__ */