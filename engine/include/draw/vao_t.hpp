/**
 * @file vao_t.hpp
 * @author masscry (masscry@gmail.com)
 * @brief vertex array data
 * @version 0.0.1.1
 * @date 2019-08-26
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#pragma once
#ifndef __DRAW_LIB_VAO_T_HEADER__
#define __DRAW_LIB_VAO_T_HEADER__

namespace draw
{

  class glVertexArray_t: public glResource_t
  {
    static void DeleteVertexArray(GLuint handle);
    void Bind(GLuint handle) const override;

    glVertexArray_t() = default;

  public:

    glVertexArray_t(int dummy); // size is always 0, ignore it
    ~glVertexArray_t() override = default;

    static glSharedResource_t None();

  };

  class glDataBuffer_t: public glResource_t
  {
    static void DeleteDataBuffer(GLuint handle);

    void Bind(GLuint handle) const override;

    glDataBuffer_t() = default;

  public:

    glDataBuffer_t(const void* data, GLsizei size, GLenum usage);
    ~glDataBuffer_t() override = default;

    static glSharedResource_t None();

  };

  class glIndexBuffer_t: public glResource_t
  {
    static void DeleteIndexBuffer(GLuint handle);

    void Bind(GLuint handle) const override;

    glIndexBuffer_t() = default;

  public:

    glIndexBuffer_t(const void* data, GLsizei size, GLenum usage);
    ~glIndexBuffer_t() override = default;

    static glSharedResource_t None();

  };

}

#endif /* __DRAW_LIB_VAO_T_HEADER__ */