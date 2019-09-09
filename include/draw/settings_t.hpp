/**
 * @brief Global and local settings class
 */

#pragma once
#ifndef __DRAW_LIB_SETTINGS_T_HEADER__
#define __DRAW_LIB_SETTINGS_T_HEADER__

namespace draw
{

  /**
   * @brief This template used to detect how json types maps to c++ types
   * 
   * @tparam T c++ type to get json type
   * @return constexpr int json type ID
   */
  template<typename T>
  constexpr int jsonExpectedType()
  {
    static_assert(jsonCXXTypeTraits<T>::JSON_TYPE != J2_UNDEF, "There is no known conversion to this type!");
    return jsonCXXTypeTraits<T>::JSON_TYPE;
  }

  /**
   * @brief Function unboxes value inside json object
   * 
   * @tparam T c++ data type expected in json object
   * @param value json object to unbox
   * @return T unboxed value
   */
  template<typename T>
  inline T jsonUnbox(J2VAL value)
  {
    return jsonCXXTypeTraits<T>::unbox(value);
  }

  class settings_t final
  {
    J2VAL root;
    const settings_t* rootOwner;

    settings_t(const settings_t&) = delete;
    settings_t& operator = (const settings_t&) = delete;

    const J2VAL TraversePath(const char* path) const;

    settings_t(J2VAL root, const settings_t* rootOwner);

  public:

    settings_t Subroot(const char* path) const;

    bool IsRootOwner() const
    {
      return this->rootOwner == nullptr;
    }

    settings_t(settings_t&& move) = default;
    settings_t& operator = (settings_t&& move) = default;

    template<typename T>
    T Param(const char* path, T defVal) const;
  
    template<typename T>
    T Param(const char* path) const;

    settings_t(const char* path);
    ~settings_t();
  };

  template<typename T>
  T settings_t::Param(const char* path, T defVal) const
  {
    J2VAL subroot = this->TraversePath(path);
    if (subroot == nullptr)
    {
      return defVal;
    }
    if (j2Type(subroot) != jsonExpectedType<T>())
    {
      return defVal;
    }
    return jsonUnbox<T>(subroot);
  }

  template<typename T>
  T settings_t::Param(const char* path) const
  {
    J2VAL subroot = this->TraversePath(path);
    if (subroot == nullptr)
    {
      THROW_ERROR("Path not found");
    }
    if (j2Type(subroot) != jsonExpectedType<T>())
    {
      THROW_ERROR("Invalid type");
    }
    return jsonUnbox<T>(subroot);
  }

} // namespace draw

#endif /* __DRAW_LIB_SETTINGS_T_HEADER__ */
