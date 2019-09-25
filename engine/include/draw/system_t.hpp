/**
 * @file system_t.hpp
 * @author masscry (masscry@gmail.com)
 * @brief Main system class
 * @version 0.0.1.1
 * @date 2019-08-18
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#pragma once
#ifndef __DRAW_LIB_SYSTEM_T_HEADER__
#define __DRAW_LIB_SYSTEM_T_HEADER__

namespace draw
{

  class frameStage_t;

  class glResource_t;

  class inputListener_t;

  class eventListener_t;

  typedef std::shared_ptr<glResource_t> glSharedResource_t;

  class system_t final {
  public:

    enum logLevel_t
    {
      DEBUG   = -1,
      INFO    =  0,
      WARNING,
      ERROR,
      TOTAL
    };

    typedef std::list<uniqueFrameStage_t> listOfFrameStages_t;
    typedef std::list<uniqueInputListener_t> listOfInputListeners_t;
    typedef std::list<uniqueEventListener_t> lisfOfEventListeners_t;

  private:

    friend class frameStage_t;
    friend class inputListener_t;
    friend class eventListener_t;

    GLFWwindow*            window;
    listOfFrameStages_t    stages;
    settings_t             settings;
    glm::vec2              winsize;
    logLevel_t             logLevel;
    listOfInputListeners_t inputListeners;
    lisfOfEventListeners_t eventListeners;
    bool                   userStopRequest;

    system_t(const system_t&) = delete;
    system_t& operator= (const system_t&) = delete;

    system_t(system_t&&) = delete;
    system_t& operator= (system_t&&) = delete;

    system_t();
    ~system_t();

    template<typename ...args_t>
    void Log(logLevel_t level, const char* format, args_t... args)
    {
      if (level >= this->logLevel)
      {
        fprintf(stderr, format, args...);
      }
    }

    static void onKeyInput(GLFWwindow* window, int key, int scancode, int action, int mods);

  public:

    template<typename ...args_t>
    void Debug(const char* format, args_t... args)
    {
      this->Log(DEBUG, format, args...);
    }

    template<typename ...args_t>
    void Info(const char* format, args_t... args)
    {
      this->Log(INFO, format, args...);
    }

    template<typename ...args_t>
    void Warning(const char* format, args_t... args)
    {
      this->Log(WARNING, format, args...);
    }

    template<typename ...args_t>
    void Error(const char* format, args_t... args)
    {
      this->Log(ERROR, format, args...);
    }

    glm::vec2 WindowSize() const
    {
      return this->winsize;
    }

    /**
     * @brief Bind given resource
     * 
     * @param res valid resource
     */
    void Bind(const glResource_t& res);

    /**
     * @brief Add new frame stage
     * 
     * @param stage valid frame stage pointer
     * @return index of frame stage 
     */
    listOfFrameStages_t::iterator AddFrameStage(frameStage_t* stage);

    /**
     * @brief Remove existing frame stage
     * 
     * @param id existing frame stage ID
     */
    void RemoveFrameStage(listOfFrameStages_t::iterator stageID);

    listOfInputListeners_t::iterator AddInputListener(inputListener_t* listener);

    void RemoveInputListener(listOfInputListeners_t::iterator);

    lisfOfEventListeners_t::iterator AddEventListener(eventListener_t* listener);

    void RemoveEventListener(lisfOfEventListeners_t::iterator);

    /**
     * @brief Get singleton instance
     * 
     * @return system_t& instance of system_t
     */
    static system_t& Instance();

    /**
     * @brief Get time from some point in time in seconds
     * 
     * @return double seconds with fractions passed from some point in time
     */
    double Timestamp() const noexcept;

    /**
     * @brief Put current thread to sleep for N seconds with fraction
     * 
     * @param sec number of seconds to sleep with fraction
     */
    void Sleep(double sec) const noexcept;

    /**
     * @brief Return current window close expected status
     * 
     * @return false when system wants to close window
     * @return true when system do not want to close window
     */
    bool IsRunning() const noexcept;

    void StopSystem();

    /**
     * @brief Render all registered frame stages, then swap buffers
     */
    void Render() noexcept;

    void Update() noexcept;

    void Event(int event);

    /**
     * @brief Global settings file.
     */
    const settings_t& Settings() const noexcept;

    std::string AbsolutePath(const char* relpath);

  };

} // namespace draw

#endif /* __DRAW_LIB_SYSTEM_T_HEADER__ */
