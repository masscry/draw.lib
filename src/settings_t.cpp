#include <draw.hpp>

namespace draw
{
  
  settings_t::settings_t(const char* path)
  :root(nullptr),rootOwner(nullptr)
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
    if (this->rootOwner == nullptr)
    {
      j2Cleanup(&this->root);
    }
  }

  settings_t::settings_t(J2VAL root, const settings_t* rootOwner)
  :root(root),rootOwner(rootOwner)
  {
    ;
  }

  settings_t settings_t::Subroot(const char* path) const
  {
    J2VAL subroot = this->TraversePath(path);
    if (subroot == nullptr)
    {
      THROW_ERROR("No subroot found");
    }
    return settings_t(subroot, this);
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

  glm::vec3 ParseVector3D(J2VAL vec)
  {
    return glm::vec3{
      jaGetNumber(vec, 0, 0.0),
      jaGetNumber(vec, 1, 0.0),
      jaGetNumber(vec, 2, 0.0)
    };
  }

} // namespace draw
