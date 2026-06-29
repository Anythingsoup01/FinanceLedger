#include "FerretLayer.h"
#include "Ferret.h"
#include "imgui.h"

#include <yaml-cpp/yaml.h>

namespace Ferret {

std::vector<AccountTable> DeserializeTables() {
  YAML::Node data;
  try {
    data = YAML::LoadFile("test.yaml");
  } catch (YAML::ParserException &ex) {
    FE_CLI_ERROR("Failed to load test.yaml file '{0}'", ex.what());
    return {};
  }

  auto accounts = data["Accounts"];

  if (!accounts.IsSequence()) {
    return {};
  }

  std::vector<AccountTable> out;

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
      accountTable.InsertDebitEntry(date, accountId, amount);
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
      accountTable.InsertCreditEntry(date, accountId, amount);
    }
    out.push_back(accountTable);
  }

  return out;
}

void FerretLayer::OnAttach() {
  m_Tables = DeserializeTables();
}

void SerializeTable(YAML::Emitter &out, AccountTable *table) {
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

void FerretLayer::OnDetach() {
  YAML::Emitter out;

  out << YAML::BeginMap;
  out << YAML::Key << "Accounts" << YAML::BeginSeq;

  for (auto &table : m_Tables) {
    SerializeTable(out, &table);
  }

  out << YAML::EndSeq; // Accounts
  out << YAML::EndMap;

  std::ofstream fout("test.yaml");
  fout << out.c_str();
}

void FerretLayer::OnUpdate() {}

void FerretLayer::OnUIRender() {
  ImGuiViewport *viewport = ImGui::GetMainViewport();
  ImGui::SetNextWindowSize(viewport->Size);
  ImGui::SetNextWindowPos(viewport->Pos);
  ImGuiWindowFlags wflags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove;
  ImGui::Begin("Full Screen", nullptr, wflags);

  for (auto &table : m_Tables) {
    table.Draw();
    ImGui::SameLine();
  }

  ImGui::End();
}

void FerretLayer::OnEvent(Event &e) {
  EventDispatcher dispatcher(e);

  dispatcher.Dispatch<KeyPressedEvent>(
      BIND_EVENT_FN(FerretLayer::OnKeyPressedEvent));
}

bool FerretLayer::OnKeyPressedEvent(KeyPressedEvent &e) {
  return false;
}

} // namespace Ferret
