#include <menu.hpp>
#include <events.hpp>

namespace {

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

  std::vector<std::string> mainMenuTextLines = {
    "New Game",  // ML_NEW_GAME
    "Load Game", // ML_LOAD_GAME
    "About",     // ML_ABOUT
    "Exit"       // ML_EXIT
  };

} // namespace

namespace evoid
{

  void menuInput_t::OnKeyPressed(draw::keyInputCodes_t id)
  {
    switch(id)
    {
      case draw::KEY_DOWN:
        draw::system_t::Instance().Event(evoid::EE_NEXT_MENU_ITEM);
        break;
      case draw::KEY_UP:
        draw::system_t::Instance().Event(evoid::EE_PREV_MENU_ITEM);
        break;
      case draw::KEY_SELECT:
        draw::system_t::Instance().Event(evoid::EE_SELECT_MENU_ITEM);
        break;
      default:
        break;
    }
  }

  void menuModel_t::OnUserEvent(int eventID)
  {
    switch(eventID)
    {
      case evoid::EE_NEXT_MENU_ITEM:
        this->Next();
        draw::system_t::Instance().Event(evoid::EE_UPDATE_MENU);
        break;
      case evoid::EE_PREV_MENU_ITEM:
        this->Prev();
        draw::system_t::Instance().Event(evoid::EE_UPDATE_MENU);
        break;
      case evoid::EE_SELECT_MENU_ITEM:
        this->Activate();
        draw::system_t::Instance().Event(evoid::EE_UPDATE_MENU);
        break;
    }
  }

  void menuModel_t::Next()
  {
    ++this->selected;
    if (this->selected == ML_TOTAL)
    {
      this->selected = ML_FIRST;
    }
  }

  void menuModel_t::Prev()
  {
    if (this->selected == ML_FIRST)
    {
      this->selected = ML_TOTAL;
    }
    --this->selected;
  }

  void menuModel_t::Activate()
  {
    switch(this->selected)
    {
      case ML_NEW_GAME:
        draw::system_t::Instance().Info("%s\n", "Start New Game");
        break;
      case ML_LOAD_GAME:
        draw::system_t::Instance().Info("%s\n", "Load Saved Game");
        break;
      case ML_ABOUT:
        draw::system_t::Instance().Info("%s\n", "Show About Scene");
        break;
      case ML_EXIT:
        draw::system_t::Instance().StopSystem();
        break;
      default:
        assert(0);
        break;
    }
  }

  menuModel_t::menuModel_t(draw::eventListener_t::list_t* parent)
  :eventListener_t(parent), selected(ML_NEW_GAME)
  {
    ;
  }

  void menuView_t::OnRender()
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

  menuView_t::menuView_t(const std::vector<std::string>& menuLinesText)
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


    this->menuLines.resize(menuLinesText.size());

    auto curMenuLine = this->menuLines.begin();
    for (int i = 0, e = menuLinesText.size(); i < e; ++i)
    {
      draw::MakeTextString(glm::vec2(0.2, 0.3), glm::ivec2(16, 16), menuLinesText[i].c_str(), *curMenuLine->Mesh());
      curMenuLine->Mesh()->CopyToGPU();
      curMenuLine->Transform() = glm::mat4(1.0f);
      curMenuLine->Transform() = glm::translate(curMenuLine->Transform(), glm::vec3(-3.0f, -i*0.4f + 0.5, -5.0f));
      curMenuLine->Transform() = glm::rotate(curMenuLine->Transform(), glm::radians(30.0f), glm::vec3(0.0f, 1.0f, 0.0f));
      ++curMenuLine;
    }
    this->menuSelected = this->menuLines.begin();

  }

  void menuView_t::SelectItem(int line)
  {
    this->menuSelected = this->menuLines.begin() + line;
  }

  void menuPresenter_t::OnUserEvent(int eventID)
  {
    if (eventID == evoid::EE_UPDATE_MENU)
    {
      auto mmp = static_cast<menuView_t*>(this->menuView->get());
      auto mmm = static_cast<menuModel_t*>(this->menuModel->get());
      mmp->SelectItem(mmm->Selected());
    }
  }

  menuPresenter_t::menuPresenter_t(draw::eventListener_t::list_t* parent)
  :draw::eventListener_t(parent)
  {
    this->menuModel = draw::system_t::Instance().AddEventListener<menuModel_t>();
    this->menuView = draw::system_t::Instance().AddFrameStage(
      new menuView_t(mainMenuTextLines)
    );
    this->menuInput = draw::system_t::Instance().AddInputListener(
      new menuInput_t()
    );
  }

  menuPresenter_t::~menuPresenter_t()
  {
    auto& instance = draw::system_t::Instance();
    instance.RemoveInputListener(this->menuInput);
    instance.RemoveFrameStage(this->menuView);
    instance.RemoveEventListener(this->menuModel);
  }

}