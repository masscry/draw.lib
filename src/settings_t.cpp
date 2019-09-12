#include <draw.hpp>

namespace draw
{

  void settings_t::onSettingsError (void* context, int line)
  {
    auto self = reinterpret_cast<settings_t*>(context);
    self->parseError = std::string("config:") + std::to_string(line);
  }

  settings_t::settings_t(const char* path)
  :root(nullptr),rootOwner(nullptr)
  {
    FILE* input = fopen(path, "r");
    if (input != nullptr)
    {
      this->root = j2ParseFileStreamEx(input, settings_t::onSettingsError, this);
      fclose(input);
    }
    if (this->root == nullptr)
    { 
      THROW_ERROR(this->ParseError().c_str());
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

  const J2VAL settings_t::TraversePath(const char* path) const
  {
    std::string full_path = std::string("/") + path;
    return joFind(this->root, full_path.c_str());
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
