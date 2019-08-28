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

  glm::vec4 ParseColor(const char* hexColStr)
  { // parses strings like '#RRGGBBAA'
    // where RR, GG, BB, AA - hex byte
    if (*hexColStr == '#')
    {
      ++hexColStr;
    }

    uint32_t colorNum = strtoul(hexColStr, nullptr, 16);
    return glm::vec4{
      ((colorNum >> 24) & 0xFF)/255.0f,
      ((colorNum >> 16) & 0xFF)/255.0f,
      ((colorNum >> 8) & 0xFF)/255.0f,
      ((colorNum ) & 0xFF)/255.0f
    };
  }


} // namespace draw
