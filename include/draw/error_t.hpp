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

  /**
   * @brief Default draw.lib exception
   */
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

    error_t(const error_t& copy) noexcept
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

} // namespace draw

/**
 * @brief Default way to throw exceptions in draw.lib
 */
#define THROW_ERROR(TEXT) throw draw::error_t(TEXT, __FILE__, __LINE__)

#endif /* __DRAW_LIB_ERROR_T_HEADER__ */