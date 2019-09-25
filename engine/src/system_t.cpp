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

#include <iconv.h>

namespace draw
{

#ifdef __unix__
  char wcharConvert(uint32_t smb)
  {
    iconv_t code;
    char* smbHead;
    size_t smbLeft;
    char result[1];

    char* resultHead;
    size_t resultLeft;
    size_t size;

    code = iconv_open(JSON_ENCODING_IN_PROGRAM, "WCHAR_T");
    if (code == (reinterpret_cast<iconv_t>(-1))) {
      return '?';
    }

    smbHead = reinterpret_cast<char*>(&smb);
    smbLeft = 4;

    resultHead = result;
    resultLeft = 1;

    size = iconv(code, &smbHead, &smbLeft, &resultHead, &resultLeft);
    if (size == (static_cast<size_t>(-1))) {
        iconv_close(code);
        return '?';
    }

    iconv_close(code);
    return result[0];
  }
#else
  char wcharConvert(uint32_t smb)
  {
    if (smb < 128)
    {
      return (char)(smb);
    }
    return '?';
  }
#endif 


  void system_t::onKeyInput(GLFWwindow* window, int key, int /*scancode*/, int action, int /*mods*/)
  {
    auto sys = reinterpret_cast<system_t*>(glfwGetWindowUserPointer(window));

    switch (action)
    {
    case GLFW_RELEASE:
      for(auto& inputs: sys->inputListeners)
      {
        inputs->SendKeyReleased(key);
      }
      break;
    case GLFW_PRESS:
    case GLFW_REPEAT:
      for(auto& inputs: sys->inputListeners)
      {
        inputs->SendKeyPress(key);
      }
      break;
    }


  }

  system_t::system_t()
  :window(nullptr),settings("./draw.json"),logLevel(INFO),userStopRequest(false)
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
    glfwSetWindowUserPointer(this->window, this);
    glfwSwapInterval(1);
    glfwSetKeyCallback(this->window, system_t::onKeyInput);

    gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress));

    glm::vec4 clearColor = this->settings.Param("scene/clcolor", glm::vec4{0.16, 0.16, 0.16, 1.0});

    glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);

    glDepthFunc(GL_LESS);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
  }

  system_t::~system_t()
  {
    for (auto stage = this->stages.begin(), e = this->stages.end(); stage!=e;)
    {
      this->Log(WARNING, "Stage [%d] removed implicitly\n", (*stage)->RegisteredID());
      this->stages.erase(stage++);
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
    return (glfwWindowShouldClose(this->window) == 0)
      && (this->userStopRequest == false);
  }

  void system_t::StopSystem()
  {
    this->userStopRequest = true;
  }

  void system_t::Update() noexcept
  {
    glfwPollEvents();
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

  system_t::listOfFrameStages_t::iterator system_t::AddFrameStage(frameStage_t* stage)
  {
    assert(stage != nullptr);            // only valid pointers expected
    assert(stage->RegisteredID() == -1); // only not registered stages expected
    this->stages.emplace_back(stage);
    stage->SetRegisteredID(this->stages.size()-1);
    return std::prev(this->stages.end());
  }

  void system_t::RemoveFrameStage(listOfFrameStages_t::iterator stageID)
  {
    (*stageID)->SetRegisteredID(-1);
    this->stages.erase(stageID);
  }

  system_t::listOfInputListeners_t::iterator system_t::AddInputListener(inputListener_t* listener)
  {
    assert(listener != nullptr);
    assert(listener->RegisteredID() == -1);
    this->inputListeners.emplace_back(listener);
    listener->SetRegisteredID(this->inputListeners.size()-1);
    return std::prev(this->inputListeners.end());
  }

  void system_t::RemoveInputListener(system_t::listOfInputListeners_t::iterator listenerID)
  {
    (*listenerID)->SetRegisteredID(-1);
    this->inputListeners.erase(listenerID);
  }

  eventListener_t::list_t::iterator system_t::AddEventListener(eventListener_t* listener)
  {
    this->eventListeners.emplace_back(listener);
    listener->SetRegisteredID(std::prev(this->eventListeners.end()));
    return listener->RegisteredID();
  }

  void system_t::RemoveEventListener(eventListener_t::list_t::iterator eventID)
  {
    (*eventID)->SetRegisteredID(this->eventListeners.end());
    this->eventListeners.erase(eventID);
  }

  void system_t::Event(int event)
  {
    for (auto& i: this->eventListeners)
    {
      i->SendEvent(event);
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
    return glfwGetTime();
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
