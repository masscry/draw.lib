#include <draw.hpp>
#include <fstream>
#include <sstream>

namespace draw
{

  enum objCommand_t
  {
    OC_MTLLIB = 0,
    OC_USEMTL,
    OC_OBJECT,
    OC_GROUP,
    OC_VERTEX,
    OC_NORMAL,
    OC_UV,
    OC_FACE,
    OC_SMOOTH,
    OC_UNDEF
  };

  objCommand_t SelectObjCommand(const char* cmd)
  {
    switch(cmd[0])
    {
      case 'v':
      {
        switch(cmd[1])
        {
          case '\0':
            return OC_VERTEX;
          case 'n':
            return OC_NORMAL;
          case 't':
            return OC_UV;
          default:
            return OC_UNDEF;
        }
      }
      case 'f':
        if (cmd[1] == 0)
        {
          return OC_FACE;
        }
        return OC_UNDEF;
      case 'o':
        if (cmd[1] == 0)
        {
          return OC_OBJECT;
        }
        return OC_UNDEF;
      case 'g':
        if (cmd[1] == 0)
        {
          return OC_GROUP;
        }
        return OC_UNDEF;
      case 's':
        if (cmd[1] == 0)
        {
          return OC_SMOOTH;
        }
        return OC_UNDEF;
      case 'm':
        if (strcmp(cmd+1, "tllib") == 0)
        {
          return OC_MTLLIB;
        }
        return OC_UNDEF;
      case 'u':
        if (strcmp(cmd+1, "semtl") == 0)
        {
          return OC_USEMTL;
        }
        return OC_UNDEF;
      default:
        return OC_UNDEF;
    }
    return OC_UNDEF;
  }

  std::string FormatLoadObjError(const char* filename, int line, const char* error)
  {
    std::string result;
    result = filename;
    result += ":";
    result += std::to_string(line);
    result += ": error: ";
    result += error;
    return result;
  }

  char* SplitIndexItem(char* token, uint32_t* result)
  {
    char* end = strchr(token, '/');
    if (end != nullptr)
    {
      *end = 0;
      *result = strtoul(token, nullptr, 10);
      return end + 1;
    }

    *result = strtoul(token, nullptr, 10);
    return nullptr;
  }

  int SplitIndex(char* str, uint32_t* v, uint32_t* t, uint32_t* n)
  {
    str = SplitIndexItem(str, v);
    if (str == nullptr)
    {
      *t = 0; 
      *n = 0;
      return 1;
    }
    str = SplitIndexItem(str, t);
    if (str == nullptr)
    {
      *n = 0;
      return 2;
    }
    SplitIndexItem(str, n);
    return 3;
  }

  void LoadObj(const char* filename, mesh_t& result)
  {
    std::ifstream input;
    input.open(filename, std::ios::in);
    if (!input.good())
    {
      THROW_ERROR("Can't open OBJ file");
    }

    result.Vertecies().clear();
    result.Indecies().clear();

    std::string line;
    std::string cmd;
    std::istringstream tokenizer;
    uint32_t lineCount = 0;

    std::vector<glm::vec3> tempVertecies;
    std::vector<glm::vec3> tempNormals;
    std::vector<glm::vec2> tempUV;

    while(std::getline(input, line))
    {
      ++lineCount;
      if (line[0] == '#')
      { // Skip comments
        continue;
      }

      tokenizer.str(line);
      tokenizer.clear();

      tokenizer >> cmd;

      switch(SelectObjCommand(cmd.c_str()))
      {
        case OC_MTLLIB:
        case OC_USEMTL:
        case OC_OBJECT:
        case OC_GROUP:
        case OC_SMOOTH:
          // ignore
          break;
        case OC_VERTEX:
        { // read vertex position
          glm::vec3 tempVec3;
          tokenizer >> tempVec3.x >> tempVec3.y >> tempVec3.z;
          tempVertecies.push_back(tempVec3);
          break;
        }
        case OC_NORMAL:
        { // read vertex normal
          glm::vec3 tempVec3;
          tokenizer >> tempVec3.x >> tempVec3.y >> tempVec3.z;
          tempNormals.push_back(tempVec3);
          break;
        }
        case OC_UV:
        { // read vertex UV
          glm::vec2 tempVec2;
          tokenizer >> tempVec2.x >> tempVec2.y;
          tempUV.push_back(tempVec2);
          break;
        }
        case OC_FACE:
        { // read face indecies
          vertex_t tempVertex;

          tempVertex.col.r = 0.5f;
          tempVertex.col.g = 0.5f;
          tempVertex.col.b = 0.5f;
          tempVertex.col.a = 1.0f;

          while(!tokenizer.eof())
          {
            std::string textIndex; // v/vt/vn
            uint32_t vi;
            uint32_t vti;
            uint32_t vni;

            tokenizer >> textIndex;

            SplitIndex(&textIndex[0], &vi, &vti, &vni);

            if (vi != 0)
            {
              assert(vi <= tempVertecies.size());
              tempVertex.pos = tempVertecies[vi - 1];
            }

            if (vti != 0)
            {
              assert(vti <= tempUV.size());
              tempVertex.uv = tempUV[vti - 1];
            }

            if (vni != 0)
            {
              assert(vni <= tempNormals.size());
              tempVertex.norm = tempNormals[vni - 1];
            }

            tempVertex.pos = tempVertecies[vi-1];
            result.Vertecies().push_back(tempVertex);
            result.Indecies().push_back(result.Vertecies().size()-1);
          }
          break;
        }
        case OC_UNDEF:
        default:
        {
          THROW_ERROR(FormatLoadObjError(filename, lineCount, cmd.c_str()));
        }
      }

    }

  }

} // namespace draw

