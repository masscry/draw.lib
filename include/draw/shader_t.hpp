/**
 * @file shader_t.hpp
 * @author masscry (masscry@gmail.com)
 * @brief shader program wrapper
 * @version 0.0.1.1
 * @date 2019-08-20
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#pragma once
#ifndef __DRAW_LIB_SHADER_T_HEADER__
#define __DRAW_LIB_SHADER_T_HEADER__

#include <memory>

namespace draw
{

  class glResource_t
  {

    friend class system_t;

  public:

    typedef void (*resCleanupFunc_t)(GLuint);

  private:

    static void DoNothing(GLuint handle);

    GLuint handle;
    resCleanupFunc_t cleanup;

    glResource_t(const glResource_t& copy) = delete; 
    glResource_t& operator =(const glResource_t& copy) = delete;

    glResource_t(glResource_t&& move) = delete;
    glResource_t& operator =(glResource_t&& move) = delete;

    virtual void Bind(GLuint handle) const = 0;

  public:

    glResource_t();
    glResource_t(GLuint handle, resCleanupFunc_t cleanup);
    virtual ~glResource_t();
  };

  class glShader_t: public glResource_t
  {
    static void DeleteShader(GLuint handle);
    void Bind(GLuint handle) const override;

    glShader_t() = default;

  public:
    glShader_t(const char* vpSource, const char* fpSource);
    ~glShader_t() override = default;

    static glSharedResource_t None();
  };

} // namespace draw


#endif /* __DRAW_LIB_SHADER_T_HEADER__ */