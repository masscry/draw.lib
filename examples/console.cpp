#include "console.hpp"

const char* vConsoleShader = R"shader(
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
  gl_Position = matProj * matView * matModel * vec4(pos, 1.0f);
  fragUV = uv;
}

)shader";

const char* fConsoleShader = R"shader(
#version 330 core

layout(location = 0) out vec4 fCol;

in vec2 fragUV;

uniform sampler2D mainTex;

void main()
{
  fCol = texture(mainTex, fragUV);
}

)shader";

void consoleView_t::OnRender()
{
  draw::system_t& instance = draw::system_t::Instance();
  instance.Bind(*this->shader);
  instance.Bind(*this->fontTexture);
  this->console.Draw(this->orthoCam);
}

consoleView_t::consoleView_t()
{
  draw::system_t& instance = draw::system_t::Instance();

  draw::MakeTextScreen(
    glm::vec2(1.0f, 1.0f), glm::ivec2(16, 16), glm::ivec2(85, 32),
    *this->console.Mesh()
  );
  this->console.Mesh()->CopyToGPU();
  this->shader = std::make_shared<draw::glShader_t>(vConsoleShader, fConsoleShader);
  this->fontTexture = draw::LoadTGA(instance.Settings().Param<const char*>("font"));;
  this->orthoCam.Bind(
    glGetUniformLocation(this->shader->Handle(), "matProj"),
    glGetUniformLocation(this->shader->Handle(), "matView"),
    glGetUniformLocation(this->shader->Handle(), "matModel")
  );
  this->orthoCam.Projection() = glm::ortho(0.0f, 1.0f, 0.0f, 1.0f);
  this->orthoCam.View() = glm::mat4(1.0f);
}
