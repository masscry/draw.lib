#pragma once
#ifndef __DRAW_LIB_MESH_T_HEADER__
#define __DRAW_LIB_MESH_T_HEADER__

namespace draw
{
  
  struct vertex_t final
  {
    glm::vec3 pos;
    glm::vec3 norm;
    glm::vec4 col;
    glm::vec2 uv;

    vertex_t() = default;

    vertex_t(const vertex_t&) = default;
    vertex_t& operator =(const vertex_t&) = default;

    vertex_t(vertex_t&&) = default;
    vertex_t& operator =(vertex_t&&) = default;

    vertex_t(glm::vec3 pos, glm::vec3 norm, glm::vec4 col, glm::vec2 uv)
    :pos(pos), norm(norm), col(col), uv(uv)
    {
      ;
    }

    ~vertex_t() = default;
  };

  typedef std::vector<vertex_t> vertexArray_t;
  typedef std::vector<uint32_t> indexArray_t;

  class mesh_t final
  {
    vertexArray_t vertecies;
    indexArray_t  indecies;

    glSharedResource_t vao;
    glSharedResource_t vBuffer;
    glSharedResource_t iBuffer;

    mesh_t(const mesh_t&) = delete;
    mesh_t& operator=(const mesh_t&) = delete;

    mesh_t(mesh_t&&) = delete;
    mesh_t& operator=(mesh_t&&) = delete;

    void GPUAllocate();
    void GPUUpdate();

  public:

    mesh_t() = default;
    ~mesh_t() = default;

    const indexArray_t& Indecies() const
    {
      return this->indecies;
    }

    const vertexArray_t& Vertecies() const
    {
      return this->vertecies;
    }

    indexArray_t& Indecies()
    {
      return this->indecies;
    }

    vertexArray_t& Vertecies()
    {
      return this->vertecies;
    }

    void CopyToGPU();

    void Draw();

  };

  typedef std::shared_ptr<mesh_t> sharedMesh_t;

  class actor_t final
  {
    glm::mat4 transform;
    sharedMesh_t mesh;
  public:

    sharedMesh_t& Mesh()
    {
      return this->mesh;
    }

    const sharedMesh_t& Mesh() const
    {
      return this->mesh;
    }

    glm::mat4& Transform()
    {
      return this->transform;
    }

    const glm::mat4& Transform() const
    {
      return this->transform;
    }

    void Draw(const camera_t& camView) const;

    actor_t();

    actor_t(glm::mat4 transform, sharedMesh_t mesh);

    ~actor_t() = default;

  };

  void LoadObj(const char* filename, mesh_t& result);

  void MakePlane(glm::vec2 size, mesh_t& result, glm::vec4 color = glm::vec4(1.0f));

  void MakeTextString(glm::vec2 size, glm::ivec2 count, const char* text, mesh_t& result);

} // namespace draw


#endif /* __DRAW_LIB_MESH_T_HEADER__ */