/**
 * @file texture_t.hpp
 * @author masscry (masscry@gmail.com)
 * @brief Texture support
 * @version 0.0.1.1
 * @date 2019-08-30
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#pragma once
#ifndef __DRAW_LIB_TEXTURE_T_HEADER__
#define __DRAW_LIB_TEXTURE_T_HEADER__

namespace draw
{

  class glTexture_t: public glResource_t
  {
    static void DeleteTexture(GLuint handle);
    void Bind(GLuint handle) const override;

    glTexture_t() = default;

  public:
    glTexture_t(glm::ivec2 size, const void* data);
    ~glTexture_t() override = default;

    static glSharedResource_t None();
  };

  glSharedResource_t LoadTGA(const char* filename);

}

#endif /* __DRAW_LIB_TEXTURE_T_HEADER__ */

