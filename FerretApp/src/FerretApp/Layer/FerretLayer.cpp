#include "FerretLayer.h"
#include "Ferret.h"
#include "imgui.h"

#include "FerretApp/Core/Serializer.h"
#include "FerretApp/FileDialog/FileDialog.h"

#include "FerretApp/Popup/Popup.h"

ImVec2 g_EntrySize {0,0};
ImVec2 g_GenericTableSize {0,0};

namespace Ferret {

void FerretLayer::OnAttach() {
  s_Instance = this;
  m_SavePath = FileDialog::OpenFile({});
  if (!m_SavePath.empty()) {
    std::map<int, AccountTable> tables;
    TableSerializer::Deserialize(&tables, m_SavePath);
    m_Ledger.SetTables(tables);
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

  std::string header;
  std::string prevMessage;
  std::string nextMessage;

  switch (m_State) {
    case RenderState::Ledger: {
      header = 
        " _              _                 \n"
        "| |            | |                \n"
        "| |     ___  __| | __ _  ___ _ __ \n"
        "| |    / _ \\/ _` |/ _` |/ _ \\ '__|\n"
        "| |___|  __/ (_| | (_| |  __/ |   \n"
        "|______\\___|\\__,_|\\__, |\\___|_|   \n"
        "                   __/ |          \n"
        "                  |___/           ";
      prevMessage = "Statments";
      nextMessage = "Journal Entries";
      break;
    }
    case RenderState::Journal: {
      header = 
        "      _                              _   ______       _        _           \n"
        "     | |                            | | |  ____|     | |      (_)          \n"
        "     | | ___  _   _ _ __ _ __   __ _| | | |__   _ __ | |_ _ __ _  ___  ___ \n"
        " _   | |/ _ \\| | | | '__| '_ \\ / _` | | |  __| | '_ \\| __| '__| |/ _ \\/ __|\n"
        "| |__| | (_) | |_| | |  | | | | (_| | | | |____| | | | |_| |  | |  __/\\__ \\\n"
        " \\____/ \\___/ \\__,_|_|  |_| |_|\\__,_|_| |______|_| |_|\\__|_|  |_|\\___||___/";
      prevMessage = "Ledger";
      nextMessage = "Statements";
      break;
    }
    case RenderState::Statements: {
      header = 
        "  _____ _        _                            _       \n"
        " / ____| |      | |                          | |      \n"
        "| (___ | |_ __ _| |_ ___ _ __ ___   ___ _ __ | |_ ___ \n"
        " \\___ \\| __/ _` | __/ _ \\ '_ ` _ \\ / _ \\ '_ \\| __/ __|\n"
        " ____) | || (_| | ||  __/ | | | | |  __/ | | | |_\\__ \\\n"
        "|_____/ \\__\\__,_|\\__\\___|_| |_| |_|\\___|_| |_|\\__|___/";
      prevMessage = "Ledger";
      nextMessage = "Statements";
      break;
    }
    default: break;
  }

  ImVec2 headerSize = ImGui::CalcTextSize(header.c_str());
  if (ImGui::Button("<", ImVec2(headerSize.y, headerSize.y))) {
    if ((int)m_State == 0) {
      m_State = (RenderState)((int)RenderState::MAX_VALUE - 1); // Sets m_State to the last item before MAX_VALUE
    } else {
      m_State = (RenderState)((int)m_State - 1); // Decrements m_State to the previous item in the list
    }
  }
  if (ImGui::IsItemHovered()) {
    ImGui::SetTooltip("%s", prevMessage.c_str());
  }

  ImGui::SameLine();
  ImGui::SetCursorPosX((viewport->Size.x - headerSize.x) / 2.0);
  ImGui::Text("%s", header.c_str());
  ImGui::SameLine();
  ImGui::SetCursorPosX(viewport->Size.x - headerSize.y);
  if (ImGui::Button(">", ImVec2(headerSize.y, headerSize.y))) {
    m_State = (RenderState)(((int)m_State + 1) % (int)RenderState::MAX_VALUE); // Automatically drops to 0 when reaching the max value, otherwise sets it to the next item in the list
  }

  if (ImGui::IsItemHovered()) {
    ImGui::SetTooltip("%s", nextMessage.c_str());
  }


  switch(m_State) {
    case RenderState::Ledger: {
      m_Ledger.OnRenderData();
      break;
    }
    case RenderState::Journal: {
      m_Journal.OnRenderData();
      break;
    }
    case RenderState::Statements: {
      m_Statements.OnRenderData();
      break;
    }
    default: break;
  }

  Popup::Render();

  ImGui::End();
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
  if (!m_ContextDirty) {
    Application::Get().OnApplicationExit();
    return true; // Not sure if it needs to complete this function or not before closing
  }

  Popup::SetRenderPopup(PopupType::SaveAndExit);

  return true;
}


void FerretLayer::Open(const std::filesystem::path &filePath) {
  std::map<int, AccountTable> tables;
  TableSerializer::Deserialize(&tables, filePath);
  m_Ledger.SetTables(tables);

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

  if (m_ContextDirty) { // Prompt the user to save before swapping tables
    Popup::SetRenderPopup(PopupType::SaveAndOpenExistingTables);
    m_TempLoadPath = tmp;
    return;
  }

  Open(tmp);
}

void FerretLayer::Save() {
  TableSerializer::Serialize(m_Ledger.GetTables(), m_SavePath);
  m_ContextDirty = false;
}

void FerretLayer::SaveAs() {
  m_SavePath = FileDialog::SaveFile({});
  if (!m_SavePath.empty()) {
    Save();
  }
}

} // namespace Ferret
