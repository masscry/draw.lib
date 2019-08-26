#include <draw.hpp>
#include <cstdio>
#include <cstring>

namespace draw
{
  
  settings_t::settings_t(const char* path)
  :root(nullptr)
  {
    FILE* input = fopen(path, "r");
    if (input != nullptr)
    {
      this->root = j2ParseFile(input);
      fclose(input);
    }
    if (this->root == nullptr)
    { // on fail, create empty root object
      this->root = j2InitObject();

      // here root can't be nullptr
      assert(this->root != nullptr);
    }
  }

  settings_t::~settings_t()
  {
    j2Cleanup(&this->root);
  }

  J2VAL settings_t::TraversePath(const char* path) const
  {
    if (path[0] == '\0')
    {
      return this->root;
    }

    J2VAL objCursor = this->root;
    const char* pathCursor = path;
    const char* pathEnd = nullptr;

    do
    {
      if (j2Type(objCursor) != J2_OBJECT)
      { // inmost object reached, but path is not
        return nullptr;
      }

      pathEnd = strchr(pathCursor, '/');
      if (pathEnd == nullptr)
      { // last path part do not have /, so take whole
        pathEnd = pathCursor + strlen(pathCursor);
      }

      std::string token(pathCursor, pathEnd);

      J2VAL item = j2ValueObjectItem(objCursor, token.c_str());
      if (item == nullptr)
      {
        return nullptr;
      }

      objCursor = item;
      pathCursor = pathEnd + 1;

    } while(*pathEnd != '\0');
    return objCursor;
  }

  double settings_t::Number(const char* path) const
  {
    J2VAL subroot = this->TraversePath(path);
    if (subroot == nullptr)
    {
      THROW_ERROR("settings_t::Number: Path not found");
    }
    if (j2Type(subroot) != J2_NUMBER)
    {
      THROW_ERROR("settings_t::Number: Invalid type");
    }
    return j2ValueNumber(subroot);
  }

  const char* settings_t::String(const char* path) const
  {
    J2VAL subroot = this->TraversePath(path);
    if (subroot == nullptr)
    {
      THROW_ERROR("settings_t::String: Path not found");
    }
    if (j2Type(subroot) != J2_STRING)
    {
      THROW_ERROR("settings_t::String: Invalid type");
    }
    return j2ValueString(subroot);
  }

  double settings_t::NumberDefault(const char* path, double defval) const
  {
    J2VAL subroot = this->TraversePath(path);
    if (subroot == nullptr)
    {
      return defval;
    }
    if (j2Type(subroot) != J2_NUMBER)
    {
      return defval;
    }
    return j2ValueNumber(subroot);
  }

  const char* settings_t::StringDefault(const char* path, const char* defval) const
  {
    J2VAL subroot = this->TraversePath(path);
    if (subroot == nullptr)
    {
      return defval;
    }
    if (j2Type(subroot) != J2_STRING)
    {
      return defval;
    }
    return j2ValueString(subroot);
  }

} // namespace draw
