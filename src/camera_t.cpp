#include <draw.hpp>

namespace draw
{
  camera_t::camera_t(float fov, float width, float height, float near, float far)
  :projection(), modelView(1.0f)
  {
    this->projection = glm::perspective(glm::radians(fov), width/height, near, far);
  }

  camera_t::camera_t()
  :projection(), modelView(1.0f)
  {
    glm::vec2 size = system_t::Instance().WindowSize();
    this->projection = glm::perspective(glm::radians(45.0f), size.x/size.y, 1.0f, 15.0f);
  }

  void camera_t::Bind(GLuint projUniform, GLuint mvUniform)
  {
    this->projUniform = projUniform;
    this->mvUniform = mvUniform;
  }

  void camera_t::Use() const
  {
    glUniformMatrix4fv(this->projUniform, 1, GL_FALSE, &this->projection[0][0]);
    glUniformMatrix4fv(this->mvUniform,   1, GL_FALSE, &this->modelView[0][0]);
  }

} // namespace draw
