#include "FerretLayer.h"
#include "Ferret.h"
#include "imgui.h"

#include "FerretApp/Utils/Utils.h"

#include "FerretApp/Core/Serializer.h"
#include "FerretApp/FileDialog/FileDialog.h"

ImVec2 g_EntrySize {0,0};
ImVec2 g_GenericTableSize {0,0};

namespace Ferret {

void FerretLayer::OnAttach() {
  s_Instance = this;
  m_SavePath = FileDialog::OpenFile({});
  if (!m_SavePath.empty()) {
    std::map<int, AccountTable> tables;
    TableSerializer::Deserialize(&tables, m_SavePath);
    m_LedgerView.SetTables(tables);
  }
}

void FerretLayer::OnDetach() {
  s_Instance = nullptr;
}

void FerretLayer::OnUpdate() {}

void FerretLayer::OnUIRender() {
  ImGuiViewport *viewport = ImGui::GetMainViewport();
  ImGui::SetNextWindowSize(viewport->Size);
  ImGui::SetNextWindowPos(viewport->Pos);
  ImGuiWindowFlags wflags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove;
  ImGui::Begin("Full Screen", nullptr, wflags);

  if (g_EntrySize.x >= 0) {
    g_EntrySize = ImGui::CalcTextSize("##/##/####|##########|##########");
    g_GenericTableSize = ImVec2(g_EntrySize.x * 2.0f, g_EntrySize.y * 7.0f);
  }

  switch(m_State) {
    case RenderState::Ledger: {
      m_LedgerView.OnRender();
      break;
    }
    default: break;
  }
}

void FerretLayer::OnEvent(Event &e) {
  EventDispatcher dispatcher(e);

  dispatcher.Dispatch<KeyPressedEvent>(BIND_EVENT_FN(FerretLayer::OnKeyPressedEvent));
  dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(FerretLayer::OnWindowClose));
}

bool FerretLayer::OnKeyPressedEvent(KeyPressedEvent &e) {
  bool ctrl = Input::IsKeyPressed(KeyCode::LeftControl) || Input::IsKeyPressed(KeyCode::RightControl);
  bool shift = Input::IsKeyPressed(KeyCode::LeftShift) || Input::IsKeyPressed(KeyCode::RightShift);

  switch (e.GetKeyCode()) {

    case KeyCode::O: {
      if (!ctrl) break;

      Open();
      break;
    }

    case KeyCode::S: {
      if (!ctrl) break;

      if (shift) {
        SaveAs();
        break;
      }

      if (m_SavePath.empty()) {
        SaveAs();
      } else {
        Save();
      }

      break;
    }

    default: break;
  }
  return false;
}

bool FerretLayer::OnWindowClose(WindowCloseEvent &e) {
  if (!m_LedgerView.IsDirty()) {
    Application::Get().OnApplicationExit();
    return true; // Not sure if it needs to complete this function or not before closing
  }

  m_LedgerView.SetRenderPopup(RenderPopup::SaveAndExit);

  return true;
}


void FerretLayer::Open(const std::filesystem::path &filePath) {
  std::map<int, AccountTable> tables;
  TableSerializer::Deserialize(&tables, filePath);
  m_LedgerView.SetTables(tables);

  m_SavePath = filePath;
}

void FerretLayer::OpenAtTmpPath() {
  Open(m_TempLoadPath);
}

void FerretLayer::Open() {
  std::filesystem::path tmp = FileDialog::OpenFile({});
  if (tmp.empty()) { // Dialog was closed
    return;
  }

  if (m_LedgerView.IsDirty()) { // Prompt the user to save before swapping tables
    m_LedgerView.SetRenderPopup(RenderPopup::SaveAndOpenExistingTables);
    m_TempLoadPath = tmp;
    return;
  }

  Open(tmp);
}

void FerretLayer::Save() {
  TableSerializer::Serialize(m_LedgerView.GetTables(), m_SavePath);
  m_LedgerView.SetDirty(false);
}

void FerretLayer::SaveAs() {
  m_SavePath = FileDialog::SaveFile({});
  if (!m_SavePath.empty()) {
    Save();
  }
}

} // namespace Ferret
