#include <draw.hpp>
#include <cstdio>
#include <glm/gtc/matrix_transform.hpp>

const char* vShader = R"shader(
#version 330 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 norm;
layout(location = 2) in vec4 col;
layout(location = 3) in vec2 uv;

uniform mat4 matProj;
uniform mat4 matModelView;

out vec4 vCol;

void main()
{
  gl_Position = matProj * matModelView * vec4(pos, 1.0f);
  vCol = col;
}

)shader";

const char* fShader = R"shader(
#version 330 core

layout(location = 0) out vec4 fCol;

in vec4 vCol;

float zNear = 1.0f;
float zFar  = 20.0f;

float linearDepth(float depth) 
{
    float z = depth * 2.0 - 1.0; 
    return (2.0 * zNear * zFar) / (zFar + zNear - z * (zFar - zNear));	
}

void main()
{
  float depth = linearDepth(gl_FragCoord.z) / zFar;
  fCol = vCol*max(depth, 0.6);
}

)shader";

class sampleFrame:public draw::frameStage_t
{
  draw::mesh_t mesh;
  draw::glSharedResource_t shader;
  draw::camera_t camera;

  void OnRender() override
  {
    draw::system_t& instance = draw::system_t::Instance();
    instance.Bind(*this->shader);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    this->camera.Use();
    this->mesh.Draw();
  }

protected:

  ~sampleFrame() = default;
  
public:

  sampleFrame()
  {
    draw::system_t& instance = draw::system_t::Instance();

    draw::LoadObj(instance.Settings().Param<const char*>("scene/model"), this->mesh);

    this->mesh.CopyToGPU();

    this->shader = std::make_shared<draw::glShader_t>(vShader, fShader);

    this->camera.Bind(
      glGetUniformLocation(this->shader->Handle(), "matProj"),
      glGetUniformLocation(this->shader->Handle(), "matModelView")
    );

    this->camera.ModelView() = glm::lookAt(
      instance.Settings().Param("scene/camera/pos",    glm::vec3(5.0f, 5.0f, 5.0f)),
      instance.Settings().Param("scene/camera/origin", glm::vec3(0.0f, 0.0f, 0.0f)),
      instance.Settings().Param("scene/camera/up",     glm::vec3(0.0f, 1.0f, 0.0f))
    );

  }

};

int main(int /*unused*/, char** /*unused*/)
{
  try 
  {
    draw::system_t& instance = draw::system_t::Instance();

    int stageID = instance.AddFrameStage(new sampleFrame());

    double start = instance.Timestamp();
    double mark  = start;

    instance.Log(draw::system_t::INFO, "Start Draw: %f\n", start);
    
    while (instance.IsRunning())
    {
      double now = instance.Timestamp();
      instance.Render();
      if (now - mark >= 1.0)
      {
        instance.Log(draw::system_t::INFO, "Time Passed: %f\n", now - start);
        mark = now;
      }
    }
    instance.Log(draw::system_t::INFO, "After Draw Finished: %f\n", instance.Timestamp());
    instance.RemoveFrameStage(stageID);
  }
  catch(const draw::error_t& err)
  {
    fprintf(stderr, "%s:%d: %s\n", err.whatFile(), err.whatLine(), err.what());
    fprintf(stderr, "%s\n", "=== STACK TRACE ===");
    fprintf(stderr, "%s\n", err.whatTrace());
    return -1;
  }
  return 0;
}
