#include "FerretLayer.h"
#include "Ferret.h"
#include "imgui.h"

ImVec2 g_EntrySize {0,0};
ImVec2 g_GenericTableSize {0,0};

namespace Ferret {

namespace Utils {

// Returns the digit count; Used for account sorting
int GetPositiveDigitCount(const int &val) {
  int digits = 0;
  int tmp = val;
  while (true) {
    if (tmp < 10 && tmp > 0) {
      digits++;
      break;
    }
    digits++;
    tmp /= 10;
  }
  return digits;
}

// Gets the very first int from a val; val = 123 : return 1
int GetTopDigit(const int &val) {
  int tmp = val;
  while (true) {
    if (tmp < 10 && tmp > 0) {
      break;
    }
    tmp /= 10;
  }
  return tmp;
}

}

void FerretLayer::OnAttach() {
  s_Instance = this;
  Deserialize();
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

  if (m_Tables.empty()) {
    if (ImGui::Button("Add Table", g_GenericTableSize))
      m_RenderPopup = RenderPopup::CreateTable;

    if (ImGui::IsItemHovered())
      ImGui::SetTooltip("Used to create a new table either in the row or in the first column");
  }

  for (auto &[id, table] : m_Tables) {
    ImGui::PushID(id);
    table.Draw();
    ImGui::SameLine();
    if (!table.GetNext() ||
        Utils::GetPositiveDigitCount(id) != Utils::GetPositiveDigitCount(table.GetNext()->GetAccountNumber()) ||
        Utils::GetTopDigit(id) != Utils::GetTopDigit(table.GetNext()->GetAccountNumber())) { // Within the same block
      if (ImGui::Button("Add Table", g_GenericTableSize))
        m_RenderPopup = RenderPopup::CreateTable;

      if (ImGui::IsItemHovered())
        ImGui::SetTooltip("Used to create a new table either in the row or in the first column");
    }
    ImGui::PopID();
  }
  ImGui::End();

  switch (m_RenderPopup) {
    case RenderPopup::CreateTable: {
      RenderCreateTablePopup();
      break;
    }
    case RenderPopup::Save: {
      RenderSavePopup();
      break;
    }
    default: break;
  }
}

void FerretLayer::SubmitEntryDataToTable(const int &toTable, const bool &isCredit, const Date_t &date, const int &fromTable, const float &amount) {
  auto &table = m_Tables.at(toTable);
  if (isCredit) {
    table.InsertDebitEntry(date, fromTable, amount, false);
  } else {
    table.InsertCreditEntry(date, fromTable, amount, false);
  }
}



void FerretLayer::RemoveTable(const int &tableID) {
  Application::Get().SubmitToMainThread([this, tableID](){
    for (auto &[id, table] : m_Tables) {
      if (id == tableID) continue;
      table.RemoveEntriesFromTable(tableID);
    }

    m_Tables.erase(tableID);
    ReloadTables();
    m_ContextDirty = true;
  });
}

void FerretLayer::OnEvent(Event &e) {
  EventDispatcher dispatcher(e);

  dispatcher.Dispatch<KeyPressedEvent>(BIND_EVENT_FN(FerretLayer::OnKeyPressedEvent));
  dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(FerretLayer::OnWindowClose));
}

void FerretLayer::Serialize() {
  YAML::Emitter out;

  out << YAML::BeginMap;
  out << YAML::Key << "Accounts" << YAML::BeginSeq;

  for (auto &[id, table] : m_Tables) {
    SerializeTables(out, &table);
  }

  out << YAML::EndSeq; // Accounts
  out << YAML::EndMap;

  std::ofstream fout("test.yaml");
  fout << out.c_str();
  fout.close();
}

void FerretLayer::SerializeTables(YAML::Emitter &out, AccountTable *table) {
  out << YAML::BeginMap; // IndiviualAccounts
  out << YAML::Key << "AccountNumber" << YAML::Value << table->GetAccountNumber();
  out << YAML::Key << "AccountName" << YAML::Value << table->GetName();
  out << YAML::Key << "IsCreditAccount" << YAML::Value << table->IsCreditAccount();
  out << YAML::Key << "DebitEntries" << YAML::BeginSeq;

  for (auto &[id, entry] : table->GetDebitTable()->GetEntries()) {
    out << YAML::BeginMap; // DebitEntry

    const Date_t &date = entry.GetDate();
    out << YAML::Key << "Month" << YAML::Value << date.Month;
    out << YAML::Key << "Day" << YAML::Value << date.Day;
    out << YAML::Key << "Year" << YAML::Value << date.Year;

    out << YAML::Key << "AccountID" << YAML::Value << entry.GetAccountID();
    out << YAML::Key << "Amount" << YAML::Value << entry.GetAmount();

    out << YAML::EndMap; // DebitEntry
  }

  out << YAML::EndSeq; // DebitEntries
  out << YAML::Key << "CreditEntries" << YAML::BeginSeq;

  for (auto &[id, entry] : table->GetCreditTable()->GetEntries()) {
    out << YAML::BeginMap; // CreditEntry

    const Date_t &date = entry.GetDate();
    out << YAML::Key << "Month" << YAML::Value << date.Month;
    out << YAML::Key << "Day" << YAML::Value << date.Day;
    out << YAML::Key << "Year" << YAML::Value << date.Year;

    out << YAML::Key << "AccountID" << YAML::Value << entry.GetAccountID();
    out << YAML::Key << "Amount" << YAML::Value << entry.GetAmount();

    out << YAML::EndMap; // CreditEntry
  }
  out << YAML::EndSeq; // CreditEntries

  out << YAML::EndMap; // IndiviualAccounts
}

bool FerretLayer::Deserialize() {
  YAML::Node data;
  try {
    data = YAML::LoadFile("test.yaml");
  } catch (YAML::ParserException &ex) {
    FE_CLI_ERROR("Failed to load test.yaml file '{0}'", ex.what());
    return false;
  }

  auto accounts = data["Accounts"];

  if (!accounts.IsSequence()) {
    return false;
  }

  int prevId = -1;

  for (auto account : accounts) {
    int accountNum = account["AccountNumber"].as<int>();
    std::string accountName = account["AccountName"].as<std::string>();
    bool creditAcc = account["IsCreditAccount"].as<bool>();
    AccountTable accountTable = AccountTable(accountName, accountNum, creditAcc);
    auto debitEntries = account["DebitEntries"];
    for (auto entry : debitEntries) {
      Date_t date = Date(
        entry["Month"].as<int>(),
        entry["Day"].as<int>(),
        entry["Year"].as<int>()
      );
      int accountId = entry["AccountID"].as<int>();
      float amount = entry["Amount"].as<float>();
      accountTable.InsertDebitEntry(date, accountId, amount, false);
    }
    auto creditEntries = account["CreditEntries"];
    for (auto entry : creditEntries) {
      Date_t date = Date(
        entry["Month"].as<int>(),
        entry["Day"].as<int>(),
        entry["Year"].as<int>()
      );
      int accountId = entry["AccountID"].as<int>();
      float amount = entry["Amount"].as<float>();
      accountTable.InsertCreditEntry(date, accountId, amount, false);
    }
    m_Tables.emplace(std::pair<int, AccountTable>(accountNum, accountTable));
    if (prevId != -1) {
      m_Tables.at(prevId).SetNext(&m_Tables.at(accountNum));
    }
    prevId = accountNum;
  }

  return true;
}

void FerretLayer::ReloadTables() {
  m_TableNames.clear();
  int prevID = -1;
  for (auto &[id, table] : m_Tables) {
    table.SetNext(nullptr);
    if (prevID != -1) {
      m_Tables.at(prevID).SetNext(&table);
    }
    prevID = id;
  }
}

bool FerretLayer::OnKeyPressedEvent(KeyPressedEvent &e) {
  bool ctrl = Input::IsKeyPressed(KeyCode::LeftControl) || Input::IsKeyPressed(KeyCode::RightControl);

  switch (e.GetKeyCode()) {

    case KeyCode::S: {
      if (!ctrl) break;
      Serialize();
      m_ContextDirty = false;
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

  m_RenderPopup = RenderPopup::Save;

  return true;
}

void FerretLayer::RenderCreateTablePopup() {
  ImGuiViewport *viewport = ImGui::GetMainViewport();
  ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize;
  // Sets the window size to be a third of the application window size
  float windowSizeX = viewport->Size.x / 3.0f;
  float windowSizeY = viewport->Size.y / 3.0f;

  // Puts to window in the center of the application window
  float windowPosX = (viewport->Size.x - windowSizeX) / 2.0f;
  float windowPosY = (viewport->Size.y - windowSizeY) / 2.0f;

  ImGui::SetNextWindowSize(ImVec2(windowSizeX, windowSizeY)); 
  ImGui::SetNextWindowPos(ImVec2(windowSizeX, windowSizeY));
  ImGui::Begin("Create New Table", nullptr, flags);

  static char accountName[32] = {0};
  static int accountNumber = 0;
  static bool isCredit = false;

  ImGui::Text("Account Name");
  ImGui::SameLine();
  ImGui::InputText("##accName", accountName, sizeof(accountName));

  ImGui::Text("Account Number");
  ImGui::SameLine();
  ImGui::DragInt("##accNum", &accountNumber, 0, 0, 0, "%03d");

  ImGui::Text("Is Credit Account");
  ImGui::SameLine();
  ImGui::Checkbox("##isCredit", &isCredit);

  bool disabled = false;

  if (m_Tables.find(accountNumber) != m_Tables.end() || accountNumber == 0) { // account already exists
    ImGui::PushStyleColor(ImGuiCol_Button,        ImVec4(1.0f, 0.0f, 0.0f, 1.0f)); // Pure Red (Normal)
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 0.3f, 0.3f, 1.0f)); // Lighter Red (Hovered)
    ImGui::PushStyleColor(ImGuiCol_ButtonActive,  ImVec4(0.8f, 0.0f, 0.0f, 1.0f)); // Darker Red (Clicked)
    disabled = true;
  }

  ImGui::BeginDisabled(disabled);

  if (ImGui::Button("Confirm")) {
    AccountTable table(accountName, accountNumber, isCredit);
    m_Tables.emplace(std::pair<int, AccountTable>(accountNumber, table));
    ReloadTables();
    memset(accountName, 0, sizeof(accountName));
    memset(&accountNumber, 0, sizeof(int));
    memset(&isCredit, 0, sizeof(bool));
    m_ContextDirty = true;
    m_RenderPopup = RenderPopup::NONE;
  }

  ImGui::EndDisabled();

  ImGui::SameLine();

  if (ImGui::Button("Cancel")) {
    memset(accountName, 0, sizeof(accountName));
    memset(&accountNumber, 0, sizeof(int));
    memset(&isCredit, 0, sizeof(bool));
    m_RenderPopup = RenderPopup::NONE;
  }

  if (disabled)
    ImGui::PopStyleColor(3);

  ImGui::End();
}

void FerretLayer::RenderSavePopup() {
  ImGuiViewport *viewport = ImGui::GetMainViewport();
  ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize;
  // Sets the window size to be a third of the application window size
  float windowSizeX = viewport->Size.x / 3.0f;
  float windowSizeY = viewport->Size.y / 3.0f;

  // Puts to window in the center of the application window
  float windowPosX = (viewport->Size.x - windowSizeX) / 2.0f;
  float windowPosY = (viewport->Size.y - windowSizeY) / 2.0f;

  ImGui::SetNextWindowSize(ImVec2(windowSizeX, windowSizeY)); 
  ImGui::SetNextWindowPos(ImVec2(windowSizeX, windowSizeY));
  ImGui::Begin("Context Dirty!", nullptr, flags);

  ImGui::Text("Save?");
  ImGui::SameLine();

  if (ImGui::Button("Yes")) {
    m_RenderPopup = RenderPopup::NONE;
    Serialize();
    Application::Get().OnApplicationExit();
  }

  ImGui::SameLine();

  if (ImGui::Button("No")) {
    m_RenderPopup = RenderPopup::NONE;
    Application::Get().OnApplicationExit();
  }

  ImGui::SameLine();

  if (ImGui::Button("Cancel")) {
    m_RenderPopup = RenderPopup::NONE;
  }

  ImGui::End();
}

} // namespace Ferret
