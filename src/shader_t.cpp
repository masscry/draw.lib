/**
 * @brief shader releated code
 */
#include <draw.hpp>

namespace draw
{

  void glShader_t::Bind(GLuint handle) const
  {
    glUseProgram(handle);
  }

  static GLuint LoadShader(GLenum shaderType, const char* shaderText) {
    GLuint result = glCreateShader(shaderType);
    glShaderSource(result, 1, &shaderText, nullptr);
    glCompileShader(result);

    GLint compileResult = GL_FALSE;
    GLint compileInfoLen = 0;

    glGetShaderiv(result, GL_COMPILE_STATUS, &compileResult);
    glGetShaderiv(result, GL_INFO_LOG_LENGTH, &compileInfoLen);
    if ( compileInfoLen > 1 ){
      compileInfoLen += 1;
      std::string text(compileInfoLen, '\0');
      glGetShaderInfoLog(result, compileInfoLen, nullptr, &text[0]);
      THROW_ERROR(text.data());
    }
    return result;
  }

  static GLuint CreateProgram(const char* vshader, const char* fshader) {
    GLuint VertexShader = LoadShader(GL_VERTEX_SHADER, vshader);
    GLuint FragmentShader = LoadShader(GL_FRAGMENT_SHADER, fshader);

    int result = glCreateProgram();
    glAttachShader(result, VertexShader);
    glAttachShader(result, FragmentShader);
    glLinkProgram(result);

    GLint linkResult = GL_FALSE;
    GLint linkInfoLen = 0;

    glGetProgramiv(result, GL_LINK_STATUS, &linkResult);
    glGetProgramiv(result, GL_INFO_LOG_LENGTH, &linkInfoLen);
    if ( linkInfoLen > 1 ){
      linkInfoLen += 1;
      std::string text(linkInfoLen, '\0');
      glGetProgramInfoLog(result, linkInfoLen, nullptr, &text[0]);
      THROW_ERROR(text.data());
    }
    glDeleteShader(VertexShader);
    glDeleteShader(FragmentShader);
    return result;
  }

  void glShader_t::DeleteShader(GLuint handle)
  {
    glDeleteProgram(handle);
  }

  glShader_t::glShader_t(const char* vpSource, const char* fpSource)
  :glResource_t(CreateProgram(vpSource, fpSource), glShader_t::DeleteShader)
  {
    ;
  }

  glSharedResource_t glShader_t::None()
  {
    struct MakeSharedEnabledClass_t: public glShader_t {};
    static glSharedResource_t self = std::make_shared<MakeSharedEnabledClass_t>();
    return self;
  }

} /* namespace draw */