#pragma once
#ifndef __DRAW_LIB_INPUT_HEADER__
#define __DRAW_LIB_INPUT_HEADER__

namespace draw
{

  enum keyInputCodes_t {
    KEY_UNDEF = -1,
    KEY_FIRST = 0,
    KEY_RIGHT = KEY_FIRST,
    KEY_LEFT,
    KEY_DOWN,
    KEY_UP,
    KEY_TOTAL
  };

  class systemKeyMap_t
  {
    typedef std::unordered_map<int, keyInputCodes_t> keyMap_t;

    keyMap_t keyMap;

    systemKeyMap_t();
    ~systemKeyMap_t();

  public:

    keyInputCodes_t MapKey(int key);

    static systemKeyMap_t& Instance();

  };

  class inputListener_t 
  {
    friend class system_t;

    int registeredID;

    virtual void OnKeyPressed(keyInputCodes_t id) = 0;
    virtual void OnKeyReleased(keyInputCodes_t id);

    inputListener_t(const inputListener_t&) = delete;
    inputListener_t(inputListener_t&&) = delete;

    inputListener_t& operator = (const inputListener_t&) = delete;
    inputListener_t& operator = (inputListener_t&&) = delete;

    void SetRegisteredID(int id)
    {
      this->registeredID = id;
    }

  protected:

    virtual ~inputListener_t();

  public:

    struct Destructor
    {
      void operator()(inputListener_t* oldInputListener)
      {
        delete oldInputListener;
      }
    };

    void SendKeyPress(int id);
    void SendKeyReleased(int id);

    int RegisteredID() const
    {
      return this->registeredID;
    }

    inputListener_t();
    
  };

  typedef std::unique_ptr<inputListener_t, inputListener_t::Destructor> uniqueInputListener_t;
  
} // namespace draw

#endif