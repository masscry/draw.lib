/**
 * @file draw.cpp
 * @author masscry (masscry@gmail.com)
 * @brief main library routines
 * @version 0.0.1.1
 * @date 2019-08-14
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <draw.hpp>

#include <ctime>
#include <cmath>
#include <cerrno>
#include <cstring>
#include <cassert>

namespace draw
{

  system_t::system_t():window(nullptr),stages()
  {
    if (glfwInit() == GLFW_FALSE)
    {
      THROW_ERROR("Library Initialization Failed");
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    this->window = glfwCreateWindow(800, 600, "draw.lib", nullptr, nullptr);
    if (this->window == nullptr)
    {
      THROW_ERROR("Window Creating Failed");
    }

    glfwMakeContextCurrent(this->window);

    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);

    glClearColor(0.16, 0.16, 0.16, 1.0);
  }

  system_t::~system_t()
  {
    for(frameStage_t* stage: this->stages)
    {
      stage->Delete();
    }

    if (this->window != nullptr)
    {
      glfwDestroyWindow(this->window);
    }

    glfwTerminate();
  }

  system_t& system_t::Instance()
  {
    static system_t self;
    return self;
  }

  bool system_t::IsCloseExpected() const noexcept
  {
    return glfwWindowShouldClose(this->window);
  }

  void system_t::Render() noexcept
  {
    for(frameStage_t* stage: this->stages)
    {
      /*
       * @todo implement better frame stage management 
       */
      if (stage != nullptr)
      {
        stage->Render();
      }
    }
    glfwSwapBuffers(system_t::Instance().window);
    glfwPollEvents();
  }

  void system_t::Bind(const glResource_t& res)
  {
    res.Bind(res.handle);
  }

  int system_t::AddFrameStage(frameStage_t* stage)
  {
    assert(stage != nullptr);            // only valid pointers expected
    assert(stage->RegisteredID() == -1); // only not registered stages expected
    this->stages.push_back(stage);
    stage->SetRegisteredID(this->stages.size()-1);
    return stage->RegisteredID();
  }

  void system_t::RemoveFrameStage(int id)
  {
    assert(id < this->stages.size());
    if (this->stages[id] != nullptr)
    {
      this->stages[id]->SetRegisteredID(-1);
      this->stages[id]->Delete();
      this->stages[id] = nullptr;
    }
    while (this->stages.back() == nullptr)
    { // remove obsolete stages in very end
      this->stages.pop_back();
    }
  }

#if DRAW_PLATFORM_UNIX

  double system_t::Timestamp() const noexcept
  {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return static_cast<double>(ts.tv_sec)
      + static_cast<double>(ts.tv_nsec) * 1.0e-9;
  }

  void system_t::Sleep(double sec) const noexcept
  {
    assert(sec >= 0.0);
    struct timespec ts;
    double frac = modf(sec, &sec);
    ts.tv_sec = sec;
    ts.tv_nsec = frac * 1e9;

    // this may not work properly when process receives signal
    clock_nanosleep(CLOCK_MONOTONIC, 0, &ts, nullptr); 
  }

#endif /* DRAW_PLATFORM_UNIX */

  void glResource_t::DoNothing(GLuint handle)
  {
    ;
  }

  glResource_t::glResource_t(GLuint handle, resCleanupFunc_t cleanup)
  :handle(handle),cleanup(cleanup)
  {
    assert((this->handle != 0) && (this->cleanup != nullptr));
    if (this->cleanup == nullptr)
    {
      this->cleanup = glResource_t::DoNothing;
    }
  }

  glResource_t::~glResource_t()
  {
    this->cleanup(this->handle);
    this->handle = 0;
  }

  void glShader_t::Bind(GLuint handle) const
  {
    glUseProgram(handle);
  }

  static GLuint LoadShader(GLenum shaderType, const char* shaderText) {
    GLuint result = glCreateShader(shaderType);
    glShaderSource(result, 1, &shaderText, NULL);
    glCompileShader(result);

    GLint compileResult = GL_FALSE;
    GLint compileInfoLen = 0;

    glGetShaderiv(result, GL_COMPILE_STATUS, &compileResult);
    glGetShaderiv(result, GL_INFO_LOG_LENGTH, &compileInfoLen);
    if ( compileInfoLen > 1 ){
      compileInfoLen += 1;
      std::string text(compileInfoLen, '\0');
      glGetShaderInfoLog(result, compileInfoLen, NULL, &text[0]);
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
      glGetProgramInfoLog(result, linkInfoLen, NULL, &text[0]);
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

  glShader_t::glShader_t()
  :glResource_t(0, nullptr)
  {
    ;
  }

  glSharedResource_t glShader_t::None()
  {
    struct MakeSharedEnabledClass_t: public glShader_t {};
    static glSharedResource_t self = std::make_shared<MakeSharedEnabledClass_t>();
    return self;
  }
  
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

  glVertexArray_t::glVertexArray_t()
  :glResource_t(0, nullptr)
  {
    ;
  }

  glVertexArray_t::glVertexArray_t(int)
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

  glDataBuffer_t::glDataBuffer_t()
  :glResource_t(0, nullptr)
  {

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

} // namespace draw
