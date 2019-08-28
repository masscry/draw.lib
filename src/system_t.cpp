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

#include <cassert>
#include <cerrno>
#include <cmath>
#include <cstring>
#include <ctime>

namespace draw
{

  system_t::system_t():window(nullptr),settings("./draw.json")
  {
    if (glfwInit() == GLFW_FALSE)
    {
      THROW_ERROR("Library Initialization Failed");
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    this->window = glfwCreateWindow(
      this->settings.Param("window/width" , 800.0),
      this->settings.Param("window/height", 600.0),
      this->settings.Param("window/title" , "draw.lib"),
      nullptr,
      nullptr
    );
    if (this->window == nullptr)
    {
      THROW_ERROR("Window Creating Failed");
    }

    glfwMakeContextCurrent(this->window);

    gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress));

    glm::vec4 clearColor = this->settings.Param("scene/clcolor", glm::vec4{0.16, 0.16, 0.16, 1.0});

    glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
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

  bool system_t::IsRunning() const noexcept
  {
    return (glfwWindowShouldClose(this->window) == 0);
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
    struct timespec ts{};
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return static_cast<double>(ts.tv_sec)
      + static_cast<double>(ts.tv_nsec) * 1.0e-9;
  }

  void system_t::Sleep(double sec) const noexcept
  {
    assert(sec >= 0.0);
    struct timespec ts{};
    double frac = modf(sec, &sec);
    ts.tv_sec = sec;
    ts.tv_nsec = frac * 1e9;

    // this may not work properly when process receives signal
    clock_nanosleep(CLOCK_MONOTONIC, 0, &ts, nullptr); 
  }

#endif /* DRAW_PLATFORM_UNIX */

  const settings_t& system_t::Settings() const noexcept
  {
    return this->settings;
  }

  void glResource_t::DoNothing(GLuint /*notused*/)
  {
    ;
  }

  glResource_t::glResource_t()
  :handle(0),cleanup(glResource_t::DoNothing)
  {
    ;
  }

  glResource_t::glResource_t(GLuint handle, resCleanupFunc_t cleanup)
  :handle(handle),cleanup(cleanup)
  {
    assert((this->handle != 0) && (this->cleanup != nullptr));
  }

  glResource_t::~glResource_t()
  {
    this->cleanup(this->handle);
    this->handle = 0;
  }

} // namespace draw
