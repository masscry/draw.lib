#include <menu.hpp>

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

uniform sampler2D mainTex;

const vec3 lightDir = normalize(vec3(1.0f, 1.0f, 1.0f));
const vec3 matrixGreen       = vec3(0.0f, 0.56f, 0.07f);
const vec3 matrixGreenEnable = vec3(0.0f, 1.0f,  0.26f);

uniform mat4  matModel;
uniform float itemEnabled;

void main()
{
  mat3 matNormal = transpose(inverse(mat3(matModel)));
  vec3 normal = normalize(matNormal * fragNormal);

  float brightness = dot(normal, lightDir);
  brightness = clamp(brightness, 0.0f, 1.0f);

  vec4 surfaceColor = texture(mainTex, fragUV);

  float alpha = surfaceColor.a * fragColor.a;
  vec3  rgb = (brightness * surfaceColor.rgb * fragColor.rgb) * mix(matrixGreen, matrixGreenEnable, itemEnabled);

  pixColor = vec4(rgb, alpha);
}

)shader";

const char* menuText[ML_TOTAL] = {
  "New Game",
  "Load Game",
  "About",
  "Exit"
};

void mainMenuPage_t::Next()
{
  this->menuSelected = std::next(this->menuSelected);
  if (this->menuSelected == menuLines.end())
  {
    this->menuSelected = menuLines.begin();
  }
}

void mainMenuPage_t::Prev()
{
  if (this->menuSelected == this->menuLines.begin())
  {
    this->menuSelected = this->menuLines.end();
  }
  this->menuSelected = std::prev(this->menuSelected);
}

int mainMenuPage_t::Selected() const
{
  return std::distance<decltype(this->menuLines.begin())>(this->menuLines.begin(), this->menuSelected);
}

void mainMenuPage_t::OnRender()
{
  draw::system_t& instance = draw::system_t::Instance();
  instance.Bind(*this->shader);
  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

  instance.Bind(*draw::glTexture_t::None());
  glUniform1f(this->menuSelectedUniform, 0.0f);
  this->spaceShip.Draw(this->menuCam);

  instance.Bind(*this->menuFont);
  auto curMenuLine = this->menuLines.begin();
  for (int i = ML_FIRST; i < ML_TOTAL; ++i)
  {
    glUniform1f(
      this->menuSelectedUniform,
      (curMenuLine == this->menuSelected)?1.0f:0.0f
    );
    curMenuLine->Draw(this->menuCam);
    ++curMenuLine;
  }

  this->spaceShip.Transform() = glm::rotate(this->spaceShip.Transform(), glm::radians(0.2f), glm::vec3(0.1f, 1.0f, 0.1f));

}

mainMenuPage_t::mainMenuPage_t()
{
  auto& instance = draw::system_t::Instance();

  this->menuFont = draw::LoadTGA(instance.Settings().Param<const char*>("menu/font"));
  this->shader = std::make_shared<draw::glShader_t>(vShader, fShader);

  this->menuCam.View() = glm::mat4(1.0f);
  this->menuCam.Projection() = glm::perspective(45.0f, instance.WindowSize().x / instance.WindowSize().y, 0.1f, 100.0f);
  this->menuCam.Bind(
      glGetUniformLocation(this->shader->Handle(), "matProj"),
      glGetUniformLocation(this->shader->Handle(), "matView"),
      glGetUniformLocation(this->shader->Handle(), "matModel")
  );
  this->menuSelectedUniform = glGetUniformLocation(this->shader->Handle(), "itemEnabled");

  draw::LoadObj(instance.Settings().Param<const char*>("menu/model"), *this->spaceShip.Mesh());
  this->spaceShip.Mesh()->CopyToGPU();
  this->spaceShip.Transform() = glm::translate(this->spaceShip.Transform(), glm::vec3(2.0f, 0.0f, -5.0f));
  this->spaceShip.Transform() = glm::rotate(this->spaceShip.Transform(), glm::radians(-45.0f), glm::vec3(0.0f, 1.0f, -0.3f));


  this->menuLines.resize(ML_TOTAL);

  auto curMenuLine = this->menuLines.begin();
  for (int i = ML_FIRST; i < ML_TOTAL; ++i)
  {
    draw::MakeTextString(glm::vec2(0.2, 0.3), glm::ivec2(16, 16), menuText[i], *curMenuLine->Mesh());
    curMenuLine->Mesh()->CopyToGPU();
    curMenuLine->Transform() = glm::mat4(1.0f);
    curMenuLine->Transform() = glm::translate(curMenuLine->Transform(), glm::vec3(-3.0f, -i*0.4f + 0.5, -5.0f));
    curMenuLine->Transform() = glm::rotate(curMenuLine->Transform(), glm::radians(30.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    ++curMenuLine;
  }
  this->menuSelected = this->menuLines.begin();

}
