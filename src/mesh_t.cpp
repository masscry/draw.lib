#include <draw.hpp>

namespace draw
{

#define BUFFER_OFFSET(OFFSET) ((void*) (OFFSET))

  mesh_t::mesh_t()
  :expired(true)
  {

  }

  vertexArray_t::iterator mesh_t::AllocateVertecies(size_t total)
  {
    this->expired = true;
    size_t offset = this->vertecies.size();
    this->vertecies.resize(this->vertecies.size() + total);
    return this->vertecies.begin() + offset;
  }

  indexArray_t::iterator mesh_t::AllocateIndecies(size_t total)
  {
    this->expired = true;
    size_t offset = this->indecies.size();
    this->indecies.resize(this->indecies.size() + total);
    return this->indecies.begin() + offset;
  }

  void mesh_t::AllocateIndeciesRange(size_t total)
  {
    this->expired = true;
    size_t start = 0;

    if (!this->indecies.empty())
    {
      start = this->indecies.back() + 1;
    }

    this->indecies.reserve(this->indecies.size() + total);
    for (size_t i = 0; i < total; ++i)
    {
      this->indecies.emplace_back(start++);
    }
  }

  vertexArray_t::iterator mesh_t::UpdateVertecies(size_t offset, size_t total)
  {
    if (this->vertecies.size() - offset > total)
    {
      THROW_ERROR("Not enough vertecies is available");
    }
    return this->vertecies.begin() + offset;
  }

  indexArray_t::iterator mesh_t::UpdateIndecies(size_t offset, size_t total)
  {
    if (this->indecies.size() - offset > total)
    {
      THROW_ERROR("Not enough vertecies is available");
    }
    return this->indecies.begin() + offset;
  }

  void mesh_t::GPUAllocate()
  {
    if ((this->vertecies.size() == 0) || (this->indecies.size() == 0))
    {
      THROW_ERROR("Mesh is not prepared");
    }

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

  void mesh_t::GPUUpdate()
  {
    system_t& instance = system_t::Instance();
    instance.Bind(*this->vao);
    instance.Bind(*this->vBuffer);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertex_t)*this->vertecies.size(), &this->vertecies[0]);
  }
  
  void mesh_t::CopyToGPU()
  {
    if (this->expired)
    {
      this->GPUAllocate();
      this->expired = false;
    }
    else
    {
      this->GPUUpdate();
    }
  }

  void mesh_t::Draw()
  {
    system_t& instance = system_t::Instance();

    if (!(this->vao))
    {
      THROW_ERROR("Need mesh_t::CopyToGPU first!");
    }

    instance.Bind(*this->vao);
    glDrawElements(GL_TRIANGLES, this->indecies.size(), GL_UNSIGNED_INT, nullptr);
  }

  void mesh_t::Clear()
  {
    this->vertecies.clear();
    this->indecies.clear();
    this->expired = true;
  }

  void MakePlane(glm::vec2 size, mesh_t& result, glm::vec4 color)
  {
    vertex_t temp;

    temp.col = color;
    temp.norm = glm::vec3(0.0f, 1.0f, 0.0f);

    result.Clear();

    auto vs = result.AllocateVertecies(4);
    auto ins = result.AllocateIndecies(6);

    temp.pos = glm::vec3(-size.x, 0.0f, -size.y);
    temp.uv  = glm::vec2(0.0f, 1.0f);
    *vs++ = temp;

    temp.pos = glm::vec3( size.x, 0.0f, -size.y);
    temp.uv  = glm::vec2(0.0f, 0.0f);
    *vs++ = temp;

    temp.pos = glm::vec3(-size.x, 0.0f, size.y);
    temp.uv  = glm::vec2(1.0f, 1.0f);
    *vs++ = temp;

    temp.pos = glm::vec3( size.x, 0.0f, size.y);
    temp.uv  = glm::vec2(1.0f, 0.0f);
    *vs++ = temp;

    *ins++ = 0;
    *ins++ = 2;
    *ins++ = 1;

    *ins++ = 1;
    *ins++ = 2;
    *ins++ = 3;
  }

  void MakeTextString(glm::vec2 size, glm::ivec2 count, const char* text, mesh_t& result)
  {
    vertex_t temp;

    temp.col = glm::vec4(1.0f);
    temp.norm = glm::vec3(0.0f, 0.0f, 1.0f);

    result.Clear();

    size_t textLength = strlen(text);

    auto vs = result.AllocateVertecies(textLength*4);
    auto ins = result.AllocateIndecies(textLength*6);

    glm::vec2 cursor = glm::vec2(0.0f);

    glm::vec2 uvPart(1.0f/count.x, 1.0f/count.y);

    uint32_t totalVertex = 0;
    for (size_t index = 0; index < textLength; ++index)
    {
      int smb = static_cast<uint8_t>(text[index]);
      glm::ivec2 smbPos(smb%count.x, smb/count.y);

      temp.pos = glm::vec3(cursor.x, cursor.y, 0.0f);
      temp.uv  = glm::vec2(smbPos.x*uvPart.x, (smbPos.y+1)*uvPart.y);
      *vs++ = temp;

      temp.pos = glm::vec3(cursor.x+size.x, cursor.y, 0.0f);
      temp.uv  = glm::vec2((smbPos.x+1)*uvPart.x, (smbPos.y+1)*uvPart.y);
      *vs++ = temp;

      temp.pos = glm::vec3(cursor.x, cursor.y+size.y, 0.0f);
      temp.uv  = glm::vec2(smbPos.x*uvPart.x, smbPos.y*uvPart.y);
      *vs++ = temp;

      temp.pos = glm::vec3(cursor.x+size.x, cursor.y+size.y, 0.0f);
      temp.uv  = glm::vec2((smbPos.x+1)*uvPart.x, smbPos.y*uvPart.y);
      *vs++ = temp;

      *ins++ = totalVertex+0;
      *ins++ = totalVertex+1;
      *ins++ = totalVertex+2;
      *ins++ = totalVertex+1;
      *ins++ = totalVertex+3;
      *ins++ = totalVertex+2;

      totalVertex += 4;
      cursor.x += size.x;
    }
  }

  void MakeTextScreen(glm::vec2 size, glm::ivec2 countFont, glm::ivec2 countChars, mesh_t& result)
  {
    vertex_t temp;

    temp.col = glm::vec4(1.0f);
    temp.norm = glm::vec3(0.0f, 0.0f, 1.0f);

    result.Clear();

    auto vs = result.AllocateVertecies(countChars.x*countChars.y*4);
    auto ins = result.AllocateIndecies(countChars.x*countChars.y*6);

    glm::vec2 cursor = glm::vec2(0.0f);

    glm::vec2 uvPart(1.0f/countFont.x, 1.0f/countFont.y);

    glm::vec2 charSize(size.x/countChars.x, size.y/countChars.y);

    uint32_t totalVertex = 0;

    for (size_t curY = 0; curY < countChars.y; ++curY)
    {
      for (size_t curX = 0; curX < countChars.x; ++curX)
      {
        int smb = rand()%96 + 32;
        glm::ivec2 smbPos(smb%countFont.x, smb/countFont.y);

        temp.pos = glm::vec3(cursor.x, cursor.y, 0.0f);
        temp.uv  = glm::vec2(smbPos.x*uvPart.x, (smbPos.y+1)*uvPart.y);
        *vs++ = temp;
  
        temp.pos = glm::vec3(cursor.x+charSize.x, cursor.y, 0.0f);
        temp.uv  = glm::vec2((smbPos.x+1)*uvPart.x, (smbPos.y+1)*uvPart.y);
        *vs++ = temp;
  
        temp.pos = glm::vec3(cursor.x, cursor.y+charSize.y, 0.0f);
        temp.uv  = glm::vec2(smbPos.x*uvPart.x, smbPos.y*uvPart.y);
        *vs++ = temp;
  
        temp.pos = glm::vec3(cursor.x+charSize.x, cursor.y+charSize.y, 0.0f);
        temp.uv  = glm::vec2((smbPos.x+1)*uvPart.x, smbPos.y*uvPart.y);
        *vs++ = temp;
  
        *ins++ = totalVertex+0;
        *ins++ = totalVertex+1;
        *ins++ = totalVertex+2;
        *ins++ = totalVertex+1;
        *ins++ = totalVertex+3;
        *ins++ = totalVertex+2;
  
        totalVertex += 4;
        cursor.x += charSize.x;
      }
      cursor.x = 0.0f;
      cursor.y += charSize.y;
    }
  }

  void actor_t::Draw(const camera_t& camView) const
  {
    camView.Use(this->transform);
    this->mesh->Draw();
  }

  actor_t::actor_t()
  :transform(1.0),mesh()
  {
    this->mesh = std::make_shared<mesh_t>();
  }

  actor_t::actor_t(glm::mat4 transform, sharedMesh_t mesh)
  :transform(transform),mesh(mesh)
  {
    ;
  }


} /* namespace draw */
