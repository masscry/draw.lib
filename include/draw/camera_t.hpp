#pragma once
#ifndef __DRAW_LIB_CAMERA_T_HEADER__
#define __DRAW_LIB_CAMERA_T_HEADER__

#include <glm/glm.hpp>

namespace draw
{
  
  class camera_t final
  {
    glm::mat4 projection;
    glm::mat4 modelView;

    GLuint projUniform;
    GLuint mvUniform;

  public:

    glm::mat4& ModelView()
    {
      return this->modelView;
    }

    const glm::mat4& ModelView() const
    {
      return this->modelView;
    }

    void Bind(GLuint projUniform, GLuint mvUniform);

    void Use() const;

    camera_t(const camera_t&) = default;
    camera_t& operator = (const camera_t&) = default;
    camera_t(camera_t&&) = default;
    camera_t& operator = (camera_t&&) = default;
    ~camera_t() = default;

    camera_t();
    camera_t(float fov, float width, float height, float near, float far);
  };

} // namespace draw

#endif /* __DRAW_LIB_CAMERA_T_HEADER__ */