#include <draw.hpp>

namespace draw
{
  camera_t::camera_t(float fov, float width, float height, float near, float far)
  :projection(), view(1.0f)
  {
    this->projection = glm::perspective(glm::radians(fov), width/height, near, far);
  }

  camera_t::camera_t()
  :projection(), view(1.0f)
  {
    glm::vec2 size = system_t::Instance().WindowSize();
    this->projection = glm::perspective(glm::radians(45.0f), size.x/size.y, 1.0f, 15.0f);
  }

  void camera_t::Bind(GLuint projUniform, GLuint viewUniform, GLuint modelUniform)
  {
    this->projUniform  = projUniform;
    this->viewUniform  = viewUniform;
    this->modelUniform = modelUniform;
  }

  void camera_t::Use() const
  {
    glUniformMatrix4fv(this->projUniform, 1, GL_FALSE, &this->projection[0][0]);
    glUniformMatrix4fv(this->viewUniform,   1, GL_FALSE, &this->view[0][0]);
  }

  void camera_t::Use(const glm::mat4& transform) const
  {
    glUniformMatrix4fv(this->projUniform,  1, GL_FALSE, &this->projection[0][0]);
    glUniformMatrix4fv(this->viewUniform,  1, GL_FALSE, &this->view[0][0]);
    glUniformMatrix4fv(this->modelUniform, 1, GL_FALSE, &transform[0][0]);
  }

} // namespace draw
