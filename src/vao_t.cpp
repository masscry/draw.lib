/**
 * @brief vertex array buffer object releated code
 * 
 */
#include <draw.hpp>

namespace draw
{

  void glVertexArray_t::DeleteVertexArray(GLuint handle)
  {
    glDeleteVertexArrays(1, &handle);
  }

  void glVertexArray_t::Bind(GLuint handle) const
  {
    glBindVertexArray(handle);
  }

  static GLuint CreateVertexArray()
  {
    GLuint result;
    glGenVertexArrays(1, &result);
    return result;
  }

  glVertexArray_t::glVertexArray_t(int /*unused*/)
  :glResource_t(CreateVertexArray(), glVertexArray_t::DeleteVertexArray)
  {
    ;
  }

  glSharedResource_t glVertexArray_t::None()
  {
    struct MakeSharedEnabledClass_t: public glVertexArray_t {};
    static glSharedResource_t self = std::make_shared<MakeSharedEnabledClass_t>();
    return self;
  }

  void glDataBuffer_t::DeleteDataBuffer(GLuint handle)
  {
    glDeleteBuffers(1, &handle);
  }

  void glDataBuffer_t::Bind(GLuint handle) const
  {
    glBindBuffer(GL_ARRAY_BUFFER, handle);
  }

  static GLuint CreateDataBuffer(const void* data, GLsizei size, GLenum usage)
  {
    GLuint result;
    glGenBuffers(1, &result);
    glBindBuffer(GL_ARRAY_BUFFER, result);
    glBufferData(GL_ARRAY_BUFFER, size, data, usage);
    return result;
  }

  glDataBuffer_t::glDataBuffer_t(const void* data, GLsizei size, GLenum usage)
  :glResource_t(CreateDataBuffer(data, size, usage), glDataBuffer_t::DeleteDataBuffer)
  {
    ;
  }

  glSharedResource_t glDataBuffer_t::None()
  {
    struct MakeSharedEnabledClass_t: public glDataBuffer_t {};
    static glSharedResource_t self = std::make_shared<MakeSharedEnabledClass_t>();
    return self;
  }  
  
  void glIndexBuffer_t::DeleteIndexBuffer(GLuint handle)
  {
    glDeleteBuffers(1, &handle);
  }

  void glIndexBuffer_t::Bind(GLuint handle) const
  {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, handle);
  }

  static GLuint CreateIndexBuffer(const void* data, GLsizei size, GLenum usage)
  {
    GLuint result;
    glGenBuffers(1, &result);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, result);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, usage);
    return result;
  }

  glIndexBuffer_t::glIndexBuffer_t(const void* data, GLsizei size, GLenum usage)
  :glResource_t(CreateIndexBuffer(data, size, usage), glIndexBuffer_t::DeleteIndexBuffer)
  {
    ;
  }

  glSharedResource_t glIndexBuffer_t::None()
  {
    struct MakeSharedEnabledClass_t: public glIndexBuffer_t {};
    static glSharedResource_t self = std::make_shared<MakeSharedEnabledClass_t>();
    return self;
  }

} // namespace draw
