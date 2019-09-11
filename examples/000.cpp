#include <draw.hpp>
#include "console.hpp"

const char* vShader = R"shader(
#version 330 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 norm;
layout(location = 2) in vec4 col;
layout(location = 3) in vec2 uv;

uniform mat4 matProj;
uniform mat4 matModelView;

out vec4 vCol;
out vec2 vUV;
out vec4 vNormal;

void main()
{
  gl_Position = matProj * matModelView * vec4(pos, 1.0f);
  vCol = col;
  vUV = uv;
  vNormal = normalize(matModelView * vec4(norm, 1.0f));
}

)shader";

const char* fShader = R"shader(
#version 330 core

layout(location = 0) out vec4 fCol;

in vec4 vCol;
in vec2 vUV;
in vec4 vNormal;

float zNear = 1.0f;
float zFar  = 15.0f;

uniform sampler2D mainTex;

const vec3 lightDir = normalize(vec3(-1.0f, -1.0f, -1.0f));

void main()
{
  vec4 texColor = texture(mainTex, vUV);
  float light = max(dot(vNormal.xyz, lightDir), 0.0f);

  fCol = vec4(texColor.rgb*vCol.rgb*light, texColor.a*vCol.a);
}

)shader";

class sampleFrame:public draw::frameStage_t
{
  draw::camera_t camera;

  draw::actor_t car;
  draw::actor_t car2;
  draw::actor_t road;
  draw::actor_t text;

  draw::glSharedResource_t shader;
  draw::glSharedResource_t planeTexture;
  draw::glSharedResource_t fontTexture;

  void OnRender() override
  {
    draw::system_t& instance = draw::system_t::Instance();
    instance.Bind(*this->shader);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    instance.Bind(*this->planeTexture);
    this->road.Draw(this->camera);
  
    instance.Bind(*draw::glTexture_t::None());
    this->car.Draw(this->camera);

    instance.Bind(*draw::glTexture_t::None());
    this->car2.Draw(this->camera);

    instance.Bind(*this->fontTexture);
    this->text.Draw(this->camera);
  }

protected:

  ~sampleFrame() = default;
  
public:

  sampleFrame()
  {
    draw::system_t& instance = draw::system_t::Instance();

    draw::LoadObj(
      instance.Settings().Param<const char*>("scene/model"),
      *this->car.Mesh()
    );

    draw::MakePlane(
      glm::xy(instance.Settings().Param("scene/floor/size", glm::vec3(1.0f, 1.0f, 0.0f))),
      *this->road.Mesh()
    );

    draw::MakeTextString(
      glm::vec2(0.1f, 0.3f), glm::ivec2(16, 16), 
      instance.Settings().Param("scene/text", "Sample"),
      *this->text.Mesh()
    );

    this->car.Mesh()->CopyToGPU();
    this->car.Transform() = glm::translate(this->car.Transform(), glm::vec3(1.0f, 0.0f, 0.0f));

    this->car2.Mesh() = this->car.Mesh();
    this->car2.Transform() = glm::rotate(this->car2.Transform(), glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    this->car2.Transform() = glm::translate(this->car2.Transform(), glm::vec3(1.0f, 0.0f, -0.5f));

    this->road.Mesh()->CopyToGPU();
    
    this->text.Mesh()->CopyToGPU();
    this->text.Transform() = glm::translate(this->text.Transform(), glm::vec3(-1.0f, 1.3f, 0.0f));
    this->text.Transform() = glm::rotate(this->text.Transform(), glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));

    this->shader = std::make_shared<draw::glShader_t>(vShader, fShader);

    this->planeTexture = draw::LoadTGA(instance.Settings().Param<const char*>("scene/floor/texture"));
    this->fontTexture = draw::LoadTGA(instance.Settings().Param<const char*>("font"));

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

    auto stageID = instance.AddFrameStage(new sampleFrame());
    auto consoleID = instance.AddFrameStage(new consoleView_t());

    double start = instance.Timestamp();
    double mark  = start;

    instance.Info("Start Draw: %f\n", start);
    
    while (instance.IsRunning())
    {
      double now = instance.Timestamp();
      instance.Render();
      if (now - mark >= 1.0)
      {
        consoleView_t& view = *static_cast<consoleView_t*>((*consoleID).get());
        draw::PrintScreen(
          glm::ivec2(16, 16),
          glm::ivec2(85, 32),
          glm::ivec2(0, 0),
          *view.Console().Mesh(), "Time Passed: %f\n", now - start
        );
        view.Console().Mesh()->CopyToGPU();
        mark = now;
      }
      instance.Update();
    }
    instance.Info("After Draw Finished: %f\n", instance.Timestamp());
    instance.RemoveFrameStage(consoleID);
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
