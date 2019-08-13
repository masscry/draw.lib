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

#include <ctime>
#include <cmath>
#include <cerrno>
#include <cstring>

namespace draw
{

  system_t::system_t()
  {

  }

  system_t::~system_t()
  {

  }

  system_t& system_t::Instance()
  {
    static system_t self;
    return self;
  }

#if DRAW_PLATFORM_UNIX

  double system_t::Timestamp() const 
  {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return static_cast<double>(ts.tv_sec)
      + static_cast<double>(ts.tv_nsec) * 1.0e-9;
  }

  void system_t::Sleep(double sec)
  {
    struct timespec ts;
    double frac = modf(sec, &sec);
    ts.tv_sec = sec;
    ts.tv_nsec = frac * 1e9;

    // this may not work properly when process receives signal
    clock_nanosleep(CLOCK_MONOTONIC, 0, &ts, nullptr); 
  }

#endif /* DRAW_PLATFORM_UNIX */



} // namespace draw
