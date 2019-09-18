#include <draw.hpp>
#include "console.hpp"

const char* vShader = R"shader(
#version 330 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 norm;
layout(location = 2) in vec4 col;
layout(location = 3) in vec2 uv;

uniform mat4 matProj;
uniform mat4 matView;
uniform mat4 matModel;

out vec3 fragPos;
out vec3 fragNormal;
out vec4 fragColor;
out vec2 fragUV;

void main()
{
  fragPos     = pos;
  fragNormal  = norm;
  fragColor   = col;
  fragUV      = uv;
  gl_Position = matProj * matView * matModel * vec4(pos, 1.0f);
}

)shader";

const char* fShader = R"shader(
#version 330 core

layout(location = 0) out vec4 pixColor;

in vec3 fragNormal;
in vec4 fragColor;
in vec2 fragUV;
in vec3 fragPos;

uniform mat4 matModel;

uniform sampler2D mainTex;

const vec3 lightPos = vec3(3.0f, 3.0f, 3.0f);
const vec3 lightDir = normalize(vec3(1.0, 1.0, 1.0));

void main()
{
  mat3 matNormal = transpose(inverse(mat3(matModel)));
  vec3 normal = normalize(matNormal * fragNormal);
  
  vec3 fragPosition = vec3(matModel * vec4(fragPos, 1.0f));
  
  float brightness = dot(normal, lightDir);
  brightness = clamp(brightness, 0.0f, 1.0f);
    
  vec4 surfaceColor = texture(mainTex, fragUV);
  pixColor = vec4(brightness * surfaceColor.rgb * fragColor.rgb, surfaceColor.a * fragColor.a);
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

  ~sampleFrame() override = default;
  
public:

  draw::camera_t& Camera()
  {
    return this->camera;
  }

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
      glGetUniformLocation(this->shader->Handle(), "matView"),
      glGetUniformLocation(this->shader->Handle(), "matModel")
    );

    this->camera.View() = glm::lookAt(
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

      consoleView_t& view = *static_cast<consoleView_t*>((*consoleID).get());
      draw::PrintScreen(
        glm::ivec2(16, 16),
        glm::ivec2(85, 32),
        glm::ivec2(0, 0),
        *view.Console().Mesh(), "FPS: %3.1f\n", 1.0f/(now - mark)
      );
      view.Console().Mesh()->CopyToGPU();

      auto stage = static_cast<sampleFrame*>(stageID->get());

      stage->Camera().View() = glm::rotate(stage->Camera().View(), 0.01f, glm::vec3(0.0f, 1.0f, 0.0f));

      instance.Update();
      mark = now;
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
