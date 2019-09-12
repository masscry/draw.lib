#include <draw.hpp>

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

  using objMatLib = std::unordered_map<std::string, glm::vec4>;

  objMatLib LoadMaterialLibrary(const char* filename)
  {
    objMatLib result;
    std::ifstream input;

    input.open(filename, std::ios::in);
    if (!input.good())
    {
      THROW_ERROR("Can't open MTL file");
    }

    std::string line;
    std::string cmd;
    std::istringstream tokenizer;
    uint32_t lineCount = 0;

    glm::vec4 tempMatColor;
    std::string tempMatName;

    while(std::getline(input, line))
    {
      ++lineCount;
      if ((line.empty()) || (line[0] == '#'))
      { // Skip comments
        continue;
      }

      tokenizer.str(line);
      tokenizer.clear();

      tokenizer >> cmd;

      if (cmd == "newmtl")
      {
        tokenizer >> tempMatName;
        continue;
      }

      if (cmd == "Kd")
      {
        tokenizer >> tempMatColor.r >> tempMatColor.g >> tempMatColor.b;
        tempMatColor.a = 1.0f;
        if (result.find(tempMatName) != result.end())
        { // Invalid format! Already has that
          THROW_ERROR(
            FormatLoadObjError(filename, lineCount, tempMatName.c_str())
          );
        }
        result[tempMatName] = tempMatColor;
        continue;
      }
    }
    return result;
  }

  void LoadObj(const char* filename, mesh_t& result)
  {
    std::string fullPath;
    std::ifstream input;

    fullPath = draw::system_t::Instance().AbsolutePath(filename);

    input.open(fullPath, std::ios::in);
    if (!input.good())
    {
      THROW_ERROR("Can't open OBJ file");
    }

    result.Clear();

    std::string line;
    std::string cmd;
    std::istringstream tokenizer;
    uint32_t lineCount = 0;

    std::vector<glm::vec3> tempVertecies;
    std::vector<glm::vec3> tempNormals;
    std::vector<glm::vec2> tempUV;
    objMatLib matLib;
    glm::vec4 curMatColor;

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
        {
          size_t lastSplit = fullPath.find_last_of(DRAW_PATH_DELIM);
          std::string mtlPath = fullPath.substr(0, lastSplit);
          std::string mtlName;

          tokenizer >> mtlName;

          mtlPath += DRAW_PATH_DELIM;
          mtlPath += mtlName;

          matLib = LoadMaterialLibrary(mtlPath.c_str());
          break;
        }
        case OC_USEMTL:
        {
          std::string mtlName;
          tokenizer >> mtlName;
          auto foundMat = matLib.find(mtlName);
          if (foundMat == matLib.end())
          {
            THROW_ERROR(FormatLoadObjError(filename, lineCount, "Can't find material"));
          }
          curMatColor = foundMat->second;
          break;
        }
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
          std::string textIndex; // v/vt/vn
          vertex_t tempVertex[3];
          int vCursor = 0;
          bool hasNormals = false;

          while(!tokenizer.eof())
          {
            uint32_t vi;
            uint32_t vti;
            uint32_t vni;

            if (vCursor >= 3)
            {
              THROW_ERROR(FormatLoadObjError(filename, lineCount, "Only 3-vertex faces supported"));
            }

            tokenizer >> textIndex;

            SplitIndex(&textIndex[0], &vi, &vti, &vni);

            if (vi != 0)
            {
              assert(vi <= tempVertecies.size());
              tempVertex[vCursor].pos = tempVertecies[vi - 1];
            }

            if (vti != 0)
            {
              assert(vti <= tempUV.size());
              tempVertex[vCursor].uv = tempUV[vti - 1];
            }
            else
            {
              tempVertex[vCursor].uv = glm::vec2(0.0f);
            }

            if (vni != 0)
            {
              assert(vni <= tempNormals.size());
              tempVertex[vCursor].norm = tempNormals[vni - 1];
              hasNormals = true;
            }
            else
            {
              tempVertex[vCursor].norm = glm::vec3(0.0f);
            }

            tempVertex[vCursor].pos = tempVertecies[vi-1];
            tempVertex[vCursor].col = curMatColor;

            ++vCursor;
          }

          if (vCursor != 3)
          {
            THROW_ERROR(FormatLoadObjError(filename, lineCount, "Only 3-vertex faces supported"));
          }

          if (!hasNormals)
          {
            glm::vec3 leftArm = tempVertex[1].pos - tempVertex[0].pos;
            glm::vec3 rightArm = tempVertex[2].pos - tempVertex[0].pos;
            glm::vec3 normal = glm::normalize(glm::cross(leftArm, rightArm));

            for (int index = 0; index < vCursor; ++index)
            {
              tempVertex[index].norm = normal;
            }
          }

          auto vs = result.AllocateVertecies(vCursor);
          for (int index = 0; index < vCursor; ++index)
          {
            *vs++ = tempVertex[index];
          }
          result.AllocateIndeciesRange(vCursor);
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

