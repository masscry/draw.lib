#include <draw.hpp>

namespace draw
{

    const char* keyMapNames[KEY_TOTAL] = {
      "keyboard/right", // KEY_RIGHT
      "keyboard/left",  // KEY_LEFT
      "keyboard/down",  // KEY_DOWN
      "keyboard/up"     // KEY_UP
    };

    systemKeyMap_t::systemKeyMap_t()
    {
      auto& instance = draw::system_t::Instance();
      for (int i = KEY_FIRST; i < KEY_TOTAL; ++i)
      {
        int key = instance.Settings().Param(keyMapNames[i], KEY_UNDEF);
        if (key != KEY_UNDEF)
        {
          keyMap[key] = static_cast<keyInputCodes_t>(i);
        }
      }
    }
    
    systemKeyMap_t::~systemKeyMap_t()
    {
      ;
    }

    keyInputCodes_t systemKeyMap_t::MapKey(int key)
    {
      auto icode = this->keyMap.find(key);
      if (icode != this->keyMap.end())
      {
        return icode->second;
      }
      return KEY_UNDEF;
    }

    systemKeyMap_t& systemKeyMap_t::Instance()
    {
      static systemKeyMap_t self;
      return self;
    }

    void inputListener_t::OnKeyReleased(keyInputCodes_t /*id*/)
    {
      ;
    }

    void inputListener_t::SendKeyPress(int id)
    {
      auto& instance = system_t::Instance();
      auto& keyMapper = systemKeyMap_t::Instance();

      keyInputCodes_t kic = keyMapper.MapKey(id);

      if (kic != KEY_UNDEF)
      {
        instance.Info("Key Pressed: %s\n", keyMapNames[kic]);
        this->OnKeyPressed(kic);
      }
    }

    void inputListener_t::SendKeyReleased(int id)
    {
      auto& instance = system_t::Instance();
      auto& keyMapper = systemKeyMap_t::Instance();
      keyInputCodes_t kic = keyMapper.MapKey(id);

      if (kic != KEY_UNDEF)
      {
        instance.Info("Key Released: %s\n", keyMapNames[kic]);
        this->OnKeyReleased(kic);
      }
    }

    inputListener_t::~inputListener_t()
    {
      assert(this->RegisteredID() < 0);
    }

    inputListener_t::inputListener_t()
    :registeredID(-1)
    {
      ;
    }

}