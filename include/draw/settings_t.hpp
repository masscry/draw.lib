/**
 * @brief Global and local settings class
 */
#include <json2.h>

namespace draw
{

  class settings_t final
  {
    J2VAL root;

    settings_t(const settings_t&) = delete;
    settings_t& operator = (const settings_t&) = delete;

    J2VAL TraversePath(const char* path) const;

  public:

    settings_t(settings_t&& move) = default;
    settings_t& operator = (settings_t&& move) = default;

    double Number(const char* path) const;

    const char* String(const char* path) const;

    double NumberDefault(const char* path, double defval) const;

    const char* StringDefault(const char* path, const char* defval) const;

    settings_t(const char* path);
    ~settings_t();
  };

} // namespace draw
