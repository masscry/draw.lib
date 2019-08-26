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

#include <vector>
#include <memory>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace draw
{

  class frameStage_t;

  class glResource_t;

  typedef std::shared_ptr<glResource_t> glSharedResource_t;

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

} // namespace draw

#endif /* __DRAW_LIB_SYSTEM_T_HEADER__ */
