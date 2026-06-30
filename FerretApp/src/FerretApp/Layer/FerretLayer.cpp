#include "FerretLayer.h"
#include "Ferret.h"
#include "imgui.h"

namespace Ferret {


void FerretLayer::OnAttach() {
  s_Instance = this;
  DeserializeTables();
}


void FerretLayer::OnDetach() {
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

  s_Instance = nullptr;
}

void FerretLayer::OnUpdate() {}

void FerretLayer::OnUIRender() {
  ImGuiViewport *viewport = ImGui::GetMainViewport();
  ImGui::SetNextWindowSize(viewport->Size);
  ImGui::SetNextWindowPos(viewport->Pos);
  ImGuiWindowFlags wflags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove;
  ImGui::Begin("Full Screen", nullptr, wflags);

  for (auto &[id, table] : m_Tables) {
    ImGui::PushID(id);
    table.Draw();
    ImGui::SameLine();
    if (!table.GetNext() || !(table.GetNext()->GetAccountNumber() - id < 100)) { // Within the same block; TODO: Let users define this block
      ImVec2 genericTableSize(table.GetGenericTableWidth(), table.GetGenericTableHeight());
      if (ImGui::Button("Add Table", genericTableSize))
        m_RenderCreateTable = true;

      if (ImGui::IsItemHovered())
        ImGui::SetTooltip("Used to create a new table either in the row or in the first column");
    }
    ImGui::PopID();
  }
  ImGui::End();

  if (m_RenderCreateTable) {
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize;
    // Sets the window size to be a third of the application window size
    float windowSizeX = viewport->Size.x / 3.0f;
    float windowSizeY = viewport->Size.y / 3.0f;

    // Puts to window in the center of the application window
    float windowPosX = (viewport->Size.x - windowSizeX) / 2.0f;
    float windowPosY = (viewport->Size.y - windowSizeY) / 2.0f;

    ImGui::SetNextWindowSize(ImVec2(windowSizeX, windowSizeY)); 
    ImGui::SetNextWindowPos(ImVec2(windowSizeX, windowSizeY));
    ImGui::Begin("Create New Table", &m_RenderCreateTable, flags);

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
      m_RenderCreateTable = false;
    }

    ImGui::EndDisabled();

    if (disabled)
      ImGui::PopStyleColor(3);

    ImGui::End();
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

void FerretLayer::OnEvent(Event &e) {
  EventDispatcher dispatcher(e);

  dispatcher.Dispatch<KeyPressedEvent>(
      BIND_EVENT_FN(FerretLayer::OnKeyPressedEvent));
}

bool FerretLayer::OnKeyPressedEvent(KeyPressedEvent &e) {
  return false;
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

bool FerretLayer::DeserializeTables() {
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

} // namespace Ferret
