/**
 * @brief shader releated code
 * 
 */
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <draw.hpp>

namespace 
{
  uint32_t WHITE_PIXEL = 0xFFFFFFFF;
} 

namespace draw
{

  void glTexture_t::DeleteTexture(GLuint handle)
  {
    glDeleteTextures(1, &handle);
  }

  void glTexture_t::Bind(GLuint handle) const
  {
    glBindTexture(GL_TEXTURE_2D, handle);
  }

  static GLuint CreateTexture(glm::ivec2 size, const void* data)
  {
    GLuint result;
    glGenTextures(1, &result);
    glBindTexture(GL_TEXTURE_2D, result);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, size.x, size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);
    return result;
  }

  glTexture_t::glTexture_t(glm::ivec2 size, const void* data)
  :glResource_t(CreateTexture(size, data), glTexture_t::DeleteTexture)
  {
    ;
  }

  glSharedResource_t glTexture_t::None()
  {
    static glSharedResource_t self = std::make_shared<glTexture_t>(glm::ivec2(1, 1), &WHITE_PIXEL);
    return self;
  }

} /* namespace draw */