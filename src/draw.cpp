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

} // namespace draw
