#include <draw.hpp>
#include <cstdio>

const char* vShader = R"shader(
#version 330 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 norm;
layout(location = 2) in vec4 col;
layout(location = 3) in vec2 uv;

out vec4 vCol;

void main()
{
  gl_Position = vec4(pos, 1.0f);
  vCol = col;
}

)shader";

const char* fShader = R"shader(
#version 330 core

layout(location = 0) out vec4 fCol;

in vec4 vCol;

void main()
{
  fCol = vCol;
}

)shader";

class sampleFrame:public draw::frameStage_t
{
  draw::mesh_t mesh;
  draw::glSharedResource_t shader;

  void OnRender() override
  {
    draw::system_t& instance = draw::system_t::Instance();
    instance.Bind(*this->shader);
    glClear(GL_COLOR_BUFFER_BIT);
    mesh.Draw();
  }
  
  bool IsDynamic() override
  {
    return false;
  }

public:

  sampleFrame()
  {
    this->mesh.Vertecies().emplace_back(
      draw::vertex_t({ -0.5f, -0.5f, 0.0f }, { 0.0f, 0.0f, 0.0f }, {  1.0f, 1.0f, 0.0f, 1.0f }, { 0.0f, 0.0f })
    );
    this->mesh.Vertecies().emplace_back(
      draw::vertex_t({ -0.5f,  0.5f, 0.0f }, { 0.0f, 0.0f, 0.0f }, {  0.0f, 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f })
    );
    this->mesh.Vertecies().emplace_back(
      draw::vertex_t({  0.5f, -0.5f, 0.0f }, { 0.0f, 0.0f, 0.0f }, {  1.0f, 0.0f, 1.0f, 1.0f }, { 0.0f, 0.0f })
    );
    this->mesh.Vertecies().emplace_back(
      draw::vertex_t({  0.5f,  0.5f, 0.0f }, { 0.0f, 0.0f, 0.0f }, {  0.0f, 1.0f, 0.0f, 1.0f }, { 0.0f, 0.0f })
    );

    this->mesh.Indecies().emplace_back(0);
    this->mesh.Indecies().emplace_back(1);
    this->mesh.Indecies().emplace_back(2);
    this->mesh.Indecies().emplace_back(2);
    this->mesh.Indecies().emplace_back(1);
    this->mesh.Indecies().emplace_back(3);

    this->mesh.CopyToGPU();

    this->shader = std::make_shared<draw::glShader_t>(vShader, fShader);
  }

  ~sampleFrame() override = default;
};

int main(int /*unused*/, char** /*unused*/)
{
  try {
    draw::system_t& instance = draw::system_t::Instance();
    sampleFrame frame;

    int stageID = instance.AddFrameStage(&frame);

    double start = instance.Timestamp();
    double mark  = start;

    fprintf(stderr, "Start Draw: %f\n", start);

    fprintf(stderr, "Let's sleep for 3 sec\n");
    instance.Sleep(3.0);
    
    while (instance.IsRunning())
    {
      double now = instance.Timestamp();
      instance.Render();
      if (now - mark >= 1.0)
      {
        fprintf(stderr, "Time Passed: %f\n", now - start);
        mark = now;
      }
    }
    fprintf(stderr, "After Draw Finished: %f\n", instance.Timestamp());
    instance.RemoveFrameStage(stageID);
  }
  catch(const draw::error_t& err)
  {
    fprintf(stderr, "%s:%d: %s\n", err.whatFile(), err.whatLine(), err.what());
    return -1;
  }
  return 0;
}
