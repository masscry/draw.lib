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
#include <draw.hpp>

namespace draw
{

  system_t::system_t():window(nullptr),settings("./draw.json"),logLevel(INFO)
  {
    if (glfwInit() == GLFW_FALSE)
    {
      THROW_ERROR("Library Initialization Failed");
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_RESIZABLE, 0);

    settings_t windowSettings = this->Settings().Subroot("window");

    this->winsize.x = windowSettings.Param("width" , 800.0);
    this->winsize.y = windowSettings.Param("height", 600.0);

    this->window = glfwCreateWindow(
      this->winsize.x,
      this->winsize.y,
      windowSettings.Param("title" , "draw.lib"),
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

    glDepthFunc(GL_LESS);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  }

  system_t::~system_t()
  {
    for (auto& stage: this->stages)
    {
      if (stage)
      {
        this->Log(WARNING, "Stage [%d] removed implicitly\n", stage->RegisteredID());
        this->RemoveFrameStage(stage->RegisteredID());
      }
    }
    this->stages.clear();

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
    for(auto& stage: this->stages)
    {
      /*
       * @todo implement better frame stage management 
       */
      if (stage != nullptr)
      {
        stage->Render();
      }
    }
    glFlush();
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
    this->stages.emplace_back(stage);
    stage->SetRegisteredID(this->stages.size()-1);
    return stage->RegisteredID();
  }

  void system_t::RemoveFrameStage(int id)
  {
    assert(id < this->stages.size());
    if (this->stages[id] != nullptr)
    {
      this->stages[id]->SetRegisteredID(-1);
      this->stages[id].reset();
      this->stages[id] = nullptr;
    }
    while (this->stages.back() == nullptr)
    { // remove obsolete stages in very end
      this->stages.pop_back();
    }
  }

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

#ifdef DRAW_PLATFORM_UNIX

#include <execinfo.h>
#include <unistd.h>

#define STACK_TRACE_DEEP (10)
#define STACK_TRACE_FULL (STACK_TRACE_DEEP + 1)

namespace draw
{

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

  std::string system_t::AbsolutePath(const char* relpath)
  {
    char* tempAbsPath = realpath(relpath, nullptr);
    if (tempAbsPath == nullptr)
    {
      THROW_ERROR("Failed to get absolute path");
    }
    std::string result(tempAbsPath);
    free(tempAbsPath);
    return result;
  }

  std::string BuildStackTrace()
  {
    void*  funcs[STACK_TRACE_FULL];
    char** texts;

    int total = backtrace(funcs, STACK_TRACE_FULL);
    if (total == 0)
    {
      return std::string("EMPTY");
    }

    // skip this func, because it is just helper 
    texts = backtrace_symbols(funcs+1, total - 1);
    if (texts == nullptr)
    {
      return std::string("NO SYMBOLS");
    }

    std::string result;

    for (int i = 0; i < total-1; ++i)
    {
      result += texts[i];
      result += '\n';
    }
    free(texts);
    return result;
  }

} // namespace draw

#endif /* DRAW_PLATFORM_UNIX */
