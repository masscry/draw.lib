#pragma once
#ifndef __DRAW_LIB_EVENT_HEADER__
#define __DRAW_LIB_EVENT_HEADER__

namespace draw
{

  class eventListener_t
  {
  public:

    struct Destructor
    {
      void operator()(eventListener_t* oldEventListener)
      {
        delete oldEventListener;
      }
    };

    using unique_t = std::unique_ptr<eventListener_t, eventListener_t::Destructor>;
    using list_t = std::list<eventListener_t::unique_t>;

  private:

    friend class system_t;

    list_t* parent;
    list_t::iterator registeredID;

    virtual void OnUserEvent(int eventID) = 0;

    eventListener_t(const eventListener_t&) = delete;
    eventListener_t(eventListener_t&&) = delete;

    eventListener_t& operator = (const eventListener_t&) = delete;
    eventListener_t& operator = (eventListener_t&&) = delete;

    void SetRegisteredID(list_t::iterator id)
    {
      this->registeredID = id;
    }

  protected:

    void Discard();

    virtual ~eventListener_t();

  public:

    list_t::iterator RegisteredID() const
    {
      return this->registeredID;
    }

    void SendEvent(int eventID);

    eventListener_t(list_t* parent);

  };

  using constructEventListenerFunc_t = eventListener_t* (draw::eventListener_t::list_t* parent);

} // namespace draw

#endif /* __DRAW_LIB_EVENT_HEADER__ */