#include "Ferret/Core/Application.h"
#include "Ferret/Core/Entrypoint.h"
#include "FerretApp/Layer/FerretLayer.h"
#include "imgui.h"

Ferret::Application *Ferret::CreateApplication(int argc, char **argv) {
  Ferret::ApplicationSpecifications spec;
  spec.Title = "Ferret Example";
  spec.ClientControlsExitEvent = true;

  Ferret::Application *app = new Ferret::Application(spec);

  FerretLayer *layer = new FerretLayer();
  app->PushLayer(layer);
  app->SetMenubarCallback([app, layer]() {
    if (ImGui::BeginMenu("File")) {
      if (ImGui::MenuItem("Exit")) {
        app->OnApplicationExit();
      }

      ImGui::EndMenu();
    }
  });

  return app;
}
