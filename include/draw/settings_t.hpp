/**
 * @brief Global and local settings class
 */
#include <json2.h>
#include <glm/glm.hpp>

namespace draw
{

  glm::vec4 ParseColor(const char* hexColStr);

  template<typename T>
  constexpr int jsonExpectedType();

  template<>
  constexpr int jsonExpectedType<glm::vec4>()
  {
    return J2_STRING;
  }

  template<>
  constexpr int jsonExpectedType<const char*>()
  {
    return J2_STRING;
  }

  template<>
  constexpr int jsonExpectedType<double>()
  {
    return J2_NUMBER;
  }

  template<typename T>
  inline T jsonUnbox(J2VAL value);

  template<>
  inline const char* jsonUnbox<const char*>(J2VAL value)
  {
    return j2ValueString(value);
  }

  template<>
  inline double jsonUnbox<double>(J2VAL value)
  {
    return j2ValueNumber(value);
  }

  template<>
  inline glm::vec4 jsonUnbox<glm::vec4>(J2VAL value)
  {
    return ParseColor(j2ValueString(value));
  }

  class settings_t final
  {
    J2VAL root;

    settings_t(const settings_t&) = delete;
    settings_t& operator = (const settings_t&) = delete;

    J2VAL TraversePath(const char* path) const;

  public:

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
