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
    bool expired;

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

    mesh_t();
    ~mesh_t() = default;

    void Clear();

    vertexArray_t::iterator AllocateVertecies(size_t total);

    indexArray_t::iterator AllocateIndecies(size_t total);
    void AllocateIndeciesRange(size_t total);

    vertexArray_t::iterator UpdateVertecies(size_t offset, size_t total);

    indexArray_t::iterator UpdateIndecies(size_t offset, size_t total);

    void CopyToGPU();

    void Draw();

  };

  typedef std::shared_ptr<mesh_t> sharedMesh_t;

  class actor_t final
  {
    glm::mat4 transform;
    sharedMesh_t mesh;
  public:

    void SetMesh(sharedMesh_t mesh)
    {
      this->mesh = mesh;
    }

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

  void MakeTextString(glm::vec2 size, glm::ivec2 countFont, const char* text, mesh_t& result);

  void MakeTextScreen(glm::vec2 size, glm::ivec2 countFont, glm::ivec2 countChars, mesh_t& result);

  void PutSymbolScreen(glm::ivec2 countFont, glm::ivec2 countChars, glm::ivec2 pos, int smb, mesh_t& result);

  template<typename ...formatArgs_t>
  void PrintScreen(glm::ivec2 countFont, glm::ivec2 countChars, glm::ivec2 pos, mesh_t& result, const char* format, formatArgs_t... args)
  {
    char tempBuffer[256];
    snprintf(tempBuffer, 256, format, args...);

    for (int i = 0; (i < 256) && (tempBuffer[i] != 0); ++i)
    {
      PutSymbolScreen(countFont, countChars, glm::ivec2(pos.x+i, countChars.y-pos.y-1), static_cast<uint8_t>(tempBuffer[i]), result);
    }
  }

} // namespace draw


#endif /* __DRAW_LIB_MESH_T_HEADER__ */