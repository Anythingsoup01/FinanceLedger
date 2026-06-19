#include "Ferret/Core/Application.h"
#include "Ferret/Core/Entrypoint.h"
#include "FerretApp/Layer/FerretLayer.h"
#include "imgui.h"

Ferret::Application *Ferret::CreateApplication(int argc, char **argv) {
  Ferret::ApplicationSpecifications spec;
  spec.Title = "Ferret Example";

  Ferret::Application *app = new Ferret::Application(spec);

  FerretLayer *layer = new FerretLayer();
  app->PushLayer(layer);
  app->SetMenubarCallback([app, layer]() {
    if (ImGui::BeginMenu("File")) {
      if (ImGui::MenuItem("Example")) {
        // Do static function here
        layer->LogExample();
      }
      if (ImGui::MenuItem("Exit")) {
        app->OnApplicationExit();
      }

      ImGui::EndMenu();
    }
  });

  return app;
}
