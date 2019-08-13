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

namespace draw
{

  class system_t final {

    system_t(const system_t&) = delete;
    system_t& operator= (const system_t&) = delete;

    system_t(system_t&&) = delete;
    system_t& operator= (system_t&&) = delete;

    system_t();
    ~system_t();

  public:

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
    double Timestamp() const;

    /**
     * @brief Put current thread to sleep for N seconds with fraction
     * 
     * @param sec number of seconds to sleep with fraction
     */
    void Sleep(double sec);

  };

}

#endif /* __DRAW_LIB_HEADER__ */
