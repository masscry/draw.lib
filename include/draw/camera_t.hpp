#pragma once
#ifndef __DRAW_LIB_CAMERA_T_HEADER__
#define __DRAW_LIB_CAMERA_T_HEADER__

namespace draw
{
  
  class camera_t final
  {
    glm::mat4 projection;
    glm::mat4 modelView;

    GLuint projUniform;
    GLuint mvUniform;

  public:

    glm::mat4& Projection()
    {
      return this->projection;
    }

    const glm::mat4& Projection() const
    {
      return this->projection;
    }

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

    void Use(const glm::mat4& transform) const;

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