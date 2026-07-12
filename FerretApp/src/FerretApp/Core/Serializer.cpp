#include "Serializer.h"

#include "FerretApp/Layer/FerretLayer.h"

#include <yaml-cpp/yaml.h>

namespace Ferret {

void SerializeEntry(YAML::Emitter &out, const Entry &entry) {
  out << YAML::BeginMap; // Entry

  const Date &date = entry.GetDate();
  out << YAML::Key << "Month" << YAML::Value << date.Month;
  out << YAML::Key << "Day" << YAML::Value << date.Day;
  out << YAML::Key << "Year" << YAML::Value << date.Year;

  out << YAML::Key << "AccountID" << YAML::Value << entry.GetAccountID();
  out << YAML::Key << "Amount" << YAML::Value << entry.GetAmount();

  out << YAML::Key << "JournalEntry" << YAML::Value << entry.GetJournalEntry();

  out << YAML::EndMap; // Entry
}

void SerializeTable(YAML::Emitter &out, const AccountTable &table) {
  out << YAML::BeginMap; // IndiviualAccounts
  out << YAML::Key << "AccountNumber" << YAML::Value << table.GetAccountNumber();
  out << YAML::Key << "AccountName" << YAML::Value << table.GetName();
  out << YAML::Key << "IsCreditAccount" << YAML::Value << table.IsCreditAccount();
  out << YAML::Key << "Tracking" << YAML::Value << TableTrackingToString(table.GetTracking());
  out << YAML::Key << "DebitEntries" << YAML::BeginSeq;

  for (auto &[id, entry] : table.GetDebitTable()->GetEntries()) {
    SerializeEntry(out, entry);
  }

  out << YAML::EndSeq; // DebitEntries
  out << YAML::Key << "CreditEntries" << YAML::BeginSeq;

  for (auto &[id, entry] : table.GetCreditTable()->GetEntries()) {
    SerializeEntry(out, entry);
  }
  out << YAML::EndSeq; // CreditEntries

  out << YAML::EndMap; // IndiviualAccounts
}

void TableSerializer::Serialize(const std::map<int, AccountTable> &tables, const std::filesystem::path &filePath) {
  YAML::Emitter out;

  out << YAML::BeginMap;
  out << YAML::Key << "Accounts" << YAML::BeginSeq;

  for (auto &[id, table] : tables) {
    SerializeTable(out, table);
  }

  out << YAML::EndSeq; // Accounts
  out << YAML::EndMap;

  std::ofstream fout(filePath);
  fout << out.c_str();
  fout.close();
}


bool TableSerializer::Deserialize(std::map<int, AccountTable> *tables, const std::filesystem::path &filePath) {
  YAML::Node data;
  try {
    data = YAML::LoadFile(filePath);
  } catch (std::exception &ex) {
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
    TableTracking tracking = TableTracking::Untracked;
    if (account["Tracking"].IsDefined())
      tracking = StringToTableTracking(account["Tracking"].as<std::string>());
    AccountTable accountTable = AccountTable(accountName, accountNum, creditAcc, tracking);
    auto debitEntries = account["DebitEntries"];
    for (auto entry : debitEntries) {
      Date date = Date(
        entry["Month"].as<int>(),
        entry["Day"].as<int>(),
        entry["Year"].as<int>()
      );
      int accountId = entry["AccountID"].as<int>();
      if (accountId == -100) { // Old ID
        accountId = FerretLayer::Get().GetLedger().GetRetainedEarningsID();
      }
      float amount = entry["Amount"].as<float>();
      std::string journalEntry = entry["JournalEntry"].IsDefined() ? entry["JournalEntry"].as<std::string>() : "Insert Entry";
      accountTable.InsertEntry(false, date, accountId, amount, false, journalEntry);
      if (accountId == FerretLayer::Get().GetLedger().GetRetainedEarningsID()) {
        FerretLayer::Get().GetStatements().UpdateBeginningBalance(amount);
      }
    }
    auto creditEntries = account["CreditEntries"];
    for (auto entry : creditEntries) {
      Date date = Date(
        entry["Month"].as<int>(),
        entry["Day"].as<int>(),
        entry["Year"].as<int>()
      );
      int accountId = entry["AccountID"].as<int>();
      if (accountId == -100) { // Old ID
        accountId = FerretLayer::Get().GetLedger().GetRetainedEarningsID();
      }
      float amount = entry["Amount"].as<float>();
      std::string journalEntry = entry["JournalEntry"].IsDefined() ? entry["JournalEntry"].as<std::string>() : "Insert Entry";
      accountTable.InsertEntry(true, date, accountId, amount, false, journalEntry);
    }
    (*tables).emplace(std::pair<int, AccountTable>(accountNum, accountTable));
    if (prevId != -1) {
      (*tables).at(prevId).SetNext(&(*tables).at(accountNum));
    }
    prevId = accountNum;
  }

  return true;
}

}
