#pragma once
#ifndef __DRAW_LIB_CAMERA_T_HEADER__
#define __DRAW_LIB_CAMERA_T_HEADER__

namespace draw
{
  
  class camera_t final
  {
    glm::mat4 projection;
    glm::mat4 view;

    GLuint projUniform;
    GLuint viewUniform;
    GLuint modelUniform;

  public:

    glm::mat4& Projection()
    {
      return this->projection;
    }

    const glm::mat4& Projection() const
    {
      return this->projection;
    }

    glm::mat4& View()
    {
      return this->view;
    }

    const glm::mat4& View() const
    {
      return this->view;
    }

    void Bind(GLuint projUniform, GLuint viewUniform, GLuint modelUniform);

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