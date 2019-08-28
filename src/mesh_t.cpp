#include <draw.hpp>
#include <cstddef>

namespace draw
{

#define BUFFER_OFFSET(OFFSET) ((void*) (OFFSET))

  void mesh_t::CopyToGPU()
  {
    system_t& instance = system_t::Instance();

    this->vao = std::make_shared<glVertexArray_t>(0);
    instance.Bind(*this->vao);

    this->vBuffer = std::make_shared<glDataBuffer_t>(
      &this->vertecies[0], sizeof(vertex_t)*this->vertecies.size(), GL_STATIC_DRAW
    );

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(3);
    instance.Bind(*this->vBuffer);
    
    // Bind position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_t), BUFFER_OFFSET(offsetof(vertex_t, pos)));

    // Bind normal
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, sizeof(vertex_t), BUFFER_OFFSET(offsetof(vertex_t, norm)));

    // Bind color
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(vertex_t), BUFFER_OFFSET(offsetof(vertex_t, col)));

    // Bind UV
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(vertex_t), BUFFER_OFFSET(offsetof(vertex_t, uv)));

    this->iBuffer = std::make_shared<glIndexBuffer_t>(
      &this->indecies[0], sizeof(uint32_t)*this->indecies.size(), GL_STATIC_DRAW
    );

    instance.Bind(*glVertexArray_t::None());
  }

  void mesh_t::Draw()
  {
    system_t& instance = system_t::Instance();

    instance.Bind(*this->vao);
    glDrawElements(GL_TRIANGLES, this->indecies.size(), GL_UNSIGNED_INT, nullptr);
  }

} /* namespace draw */
