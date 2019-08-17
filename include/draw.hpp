/**
 * @file draw.hpp
 * @author masscry (masscry@gmail.com)
 * @brief Main DRAW library header
 * @version 0.0.1.1
 * @date 2019-08-14
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#pragma once
#ifndef __DRAW_LIB_HEADER__
#define __DRAW_LIB_HEADER__

#include <draw/config.h>
#include <cstddef>
#include <cstdint>
#include <cassert>
#include <stdexcept>
#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace draw
{

  class error_t final: public std::runtime_error {

    const char* filename;
    int line;

  public:

    const char* whatFile() const noexcept {
      return this->filename;
    }

    int whatLine() const noexcept {
      return this->line;
    }

    explicit error_t(const char* error, const char* filename, int line)
    :std::runtime_error(error), filename(filename), line(line) {
      ;
    }

    error_t(const error_t& copy)
    :std::runtime_error(copy), filename(copy.filename), line(copy.line) {
      ;
    }

    error_t& operator=(const error_t& copy) {
      if (this != &copy)
      {
        this->std::runtime_error::operator=(copy);
        this->filename = copy.filename;
        this->line = copy.line;
      }
      return *this;
    }

  };

#define THROW_ERROR(TEXT) throw draw::error_t(TEXT, __FILE__, __LINE__)

  class frameStage_t
  {
    friend class system_t;

    int index; /**< Index is set, when frame stage is registered in system_t */

    /**
     * @brief Called when frame stage must be rendered.
     */
    virtual void OnRender()  = 0;

    /**
     * @brief Must return true if this frame stage is not a singleton and
     * allocated on heap
     * 
     * @return true frame stage is allocated on heap
     * @return false frame stage is singleton
     */
    virtual bool IsDynamic() = 0;

    frameStage_t(const frameStage_t&) = delete;
    frameStage_t& operator = (const frameStage_t&) = delete;

    frameStage_t(frameStage_t&&) = delete;
    frameStage_t& operator = (frameStage_t&&) = delete;

    /**
     * @brief Set the ID to object
     * 
     * This function called in AddFrameStage/RemoveFrameStage
     * 
     * @param index new ID
     */
    void SetRegisteredID(int index)
    {
      this->index = index;
    }

  public:

    /**
     * @brief Get registered ID
     * 
     * @return int if not registered yet returns -1
     */
    int RegisteredID() const 
    {
      return this->index;
    }

    void Render() noexcept
    {
      this->OnRender();
    }

    void Delete() noexcept
    {
      if (this->IsDynamic())
      {
        delete this;
      }
    }

    frameStage_t() noexcept
    :index(-1) {
      ;
    }
    
    virtual ~frameStage_t()
    { // Must unregister this stage before its termination 
      assert(this->RegisteredID() < 0);
    }
  };

  class system_t final {

    friend class frameStage_t;

    typedef std::vector<frameStage_t*> vectorOfFrameStages_t;

    GLFWwindow*           window;
    vectorOfFrameStages_t stages;

    system_t(const system_t&) = delete;
    system_t& operator= (const system_t&) = delete;

    system_t(system_t&&) = delete;
    system_t& operator= (system_t&&) = delete;

    system_t();
    ~system_t();

  public:

    /**
     * @brief Add new frame stage
     * 
     * @param stage valid frame stage pointer
     * @return index of frame stage 
     */
    int AddFrameStage(frameStage_t* stage);

    /**
     * @brief Remove existing frame stage
     * 
     * @param id existing frame stage ID
     */
    void RemoveFrameStage(int id);

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
     * @return true when system wants to close window
     * @return false when system do not want to close window
     */
    bool IsCloseExpected() const noexcept;

    /**
     * @brief Render all registered frame stages, then swap buffers
     */
    void Render() noexcept;

  };

}

#endif /* __DRAW_LIB_HEADER__ */
