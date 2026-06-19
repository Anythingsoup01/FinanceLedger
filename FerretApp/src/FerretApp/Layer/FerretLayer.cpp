#include "FerretLayer.h"
#include "Ferret.h"
#include "imgui.h"

namespace Ferret {

void FerretLayer::OnAttach() {}

void FerretLayer::OnDetach() {}

void FerretLayer::OnUpdate() {}

void FerretLayer::OnUIRender() {
  ImGui::Begin("##MAINPAGE");
  {
    ImGui::Text("Welcome to Ferret");

    ImGui::End();
  }

  ImGui::ShowDemoWindow();
}

void FerretLayer::OnEvent(Event &e) {
  EventDispatcher dispatcher(e);

  dispatcher.Dispatch<KeyPressedEvent>(
      BIND_EVENT_FN(FerretLayer::OnKeyPressedEvent));
}

bool FerretLayer::OnKeyPressedEvent(KeyPressedEvent &e) {
  bool ctrl = Input::IsKeyPressed(KeyCode::RightControl) ||
              Input::IsKeyPressed(KeyCode::LeftControl);

  switch (e.GetKeyCode()) {
  case KeyCode::C: {
    if (ctrl)
      FE_CLI_INFO("Ctrl + C Pressed");
    else
      FE_CLI_INFO("C Pressed");
    break;
  }
  default:
    break;
  }
  return false;
}

void FerretLayer::LogExample() { FE_CLI_INFO("Logging from MenuBar"); }
} // namespace Ferret
