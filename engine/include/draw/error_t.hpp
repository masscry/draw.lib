/**
 * @file error_t.hpp
 * @author masscry (masscry@gmail.com)
 * @brief default exception class
 * @version 0.0.1.1
 * @date 2019-08-18
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#pragma once
#ifndef __DRAW_LIB_ERROR_T_HEADER__
#define __DRAW_LIB_ERROR_T_HEADER__

#include <stdexcept>

namespace draw
{

  std::string BuildStackTrace();

  /**
   * @brief Default draw.lib exception
   */
  class error_t final: public std::runtime_error {

    const char* filename;
    int line;
    std::string trace;

  public:

    const char* whatTrace() const noexcept {
      return this->trace.c_str();
    }

    const char* whatFile() const noexcept {
      return this->filename;
    }

    int whatLine() const noexcept {
      return this->line;
    }

    explicit error_t(std::string error, const char* filename, int line, std::string trace)
    :std::runtime_error(error), filename(filename), line(line),trace(trace) {
      ;
    }

    explicit error_t(const char* error, const char* filename, int line, std::string trace)
    :std::runtime_error(error), filename(filename), line(line),trace(trace) {
      ;
    }

    error_t(const error_t& copy) = default;
    error_t& operator=(const error_t& copy) = default;
    error_t(error_t&& move) = default;
    error_t& operator=(error_t&& move) = default;

  };

} // namespace draw

/**
 * @brief Default way to throw exceptions in draw.lib
 */
#define THROW_ERROR(TEXT) throw draw::error_t(TEXT, __FILE__, __LINE__, BuildStackTrace())

#endif /* __DRAW_LIB_ERROR_T_HEADER__ */