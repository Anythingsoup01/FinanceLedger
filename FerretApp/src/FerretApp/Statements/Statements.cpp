#include "Statements.h"

#include "FerretApp/DataTypes/AccountTable.h"
#include "FerretApp/Layer/FerretLayer.h"

#include "FerretApp/Utils/Utils.h"

extern ImVec2 g_EntrySize;

namespace Ferret {

Statements::Statements() {
  uint64_t tableHash = Utils::GenerateHash64("Test");
  m_RootTableHashes.push_back(tableHash);
  m_TableMap.emplace(std::pair<uint64_t, Table>(tableHash, Table("Test", std::string(), 0)));
}

void Statements::OnRenderData() {
  OnRenderIncomeStatement();
  ImGui::Dummy(ImVec2(0, 50));
  OnRenderRetainedEarnings();
  ImGui::Dummy(ImVec2(0, 50));
  OnRenderBalanceStatement();

  for (auto &tableHash : m_RootTableHashes) {
    auto &table = m_TableMap.at(tableHash);
    table.Render();
  }

}

void Statements::NewExpenseOrIncomeDataAvailable() {
  m_IncomeAccounts.clear();
  m_ExpenseAccounts.clear();

  m_IncomeAccountsTotal = 0;
  m_ExpenseAccountsTotal = 0;

  for (auto &[id, table] : FerretLayer::Get().GetLedger().GetTables()) {
    switch (table.GetTracking()) {
      case TableTracking::Income: {
        m_IncomeAccounts.push_back({table.GetName(), table.GetAccountNumber(), table.GetBalance()});
        m_IncomeAccountsTotal += table.GetBalance();
        break;
      }
      case TableTracking::Expenses: {
        m_ExpenseAccounts.push_back({table.GetName(), table.GetAccountNumber(), table.GetBalance()});
        m_ExpenseAccountsTotal += table.GetBalance();
        break;
      }
      default: break;
    }
  }

  m_RetainedEarnings = m_IncomeAccountsTotal - m_ExpenseAccountsTotal;
}

void Statements::NewCashDataAvailable() {
  m_CashAccounts.clear();
  m_OtherAssetsAccounts.clear();

  m_CashAccountsTotal = 0;
  m_OtherAssetAccountsTotal = 0;

  for (auto &[id, table] : FerretLayer::Get().GetLedger().GetTables()) {
    switch (table.GetTracking()) {
      case TableTracking::Cash: {
        m_CashAccounts.push_back({table.GetName(), table.GetAccountNumber(), table.GetBalance()});
        m_CashAccountsTotal += table.GetBalance();
        break;
      }
      case TableTracking::OtherAsset: {
        m_OtherAssetsAccounts.push_back({table.GetName(), table.GetAccountNumber(), table.GetBalance()});
        m_OtherAssetAccountsTotal += table.GetBalance();
        break;
      }
      default: break;
    }
  }
}

void Statements::NewLiabilityDataAvailable() {
  m_ShortTermLiabilityAccounts.clear();
  m_LongTermLiabilityAccounts.clear();

  m_ShortTermLiabilityAccountsTotal = 0;
  m_LongTermLiabilityAccountsTotal = 0;

  for (auto &[id, table] : FerretLayer::Get().GetLedger().GetTables()) {
    switch (table.GetTracking()) {
      case TableTracking::ShortTermLiabilities: {
        m_ShortTermLiabilityAccounts.push_back({table.GetName(), table.GetAccountNumber(), table.GetBalance()});
        m_ShortTermLiabilityAccountsTotal += table.GetBalance();
        break;
      }
      case TableTracking::LongTermLiabilities: {
        m_LongTermLiabilityAccounts.push_back({table.GetName(), table.GetAccountNumber(), table.GetBalance()});
        m_LongTermLiabilityAccountsTotal += table.GetBalance();
        break;
      }
      default: break;
    }
  }
}

void Statements::RenderIncomeStatementTables(const std::string &tableName, const std::vector<AccountTableStatementData> &accounts, const float &tableTotal) {
  ImGuiTableFlags flags = ImGuiTableFlags_Borders | ImGuiTableFlags_BordersV | ImGuiTableFlags_SizingStretchProp;
  float availableWidth = ImGui::GetContentRegionAvail().x;
  char buf[32] = {0};
  snprintf(buf, sizeof(buf), "##%sSection", tableName.c_str());
  if (ImGui::BeginTable(buf, 3, flags)) {
    ImGui::TableSetupColumn(tableName.c_str(), ImGuiTableColumnFlags_WidthFixed, availableWidth * 0.25);
    ImGui::TableSetupColumn("##EmptyHeader", ImGuiTableColumnFlags_WidthFixed, availableWidth * 0.5);
    ImGui::TableSetupColumn("##EmptyHeader", ImGuiTableColumnFlags_WidthFixed, availableWidth * 0.25);

    Utils::HeaderCentered(3);
    ImGui::TableNextRow();

    for (auto &table : accounts) {
      ImGui::TableSetColumnIndex(1);
      ImGui::Text("%s (%d)", table.Name.c_str(), table.Account);
      ImGui::TableSetColumnIndex(2);
      ImGui::Text("%.2f", table.Amount);
      ImGui::TableNextRow();
    }

    ImGui::TableSetColumnIndex(0);
    ImGui::Dummy(ImVec2(ImGui::GetColumnWidth(), g_EntrySize.y));

    ImGui::EndTable();
  }

  ImGui::TableNextRow();
  ImGui::TableSetColumnIndex(0);

  snprintf(buf, sizeof(buf), "##%sTotal", tableName.c_str());
  if (ImGui::BeginTable(buf, 2, flags)) {
    char buf[32] = {0};
    ImGui::TableSetupColumn("Total", ImGuiTableColumnFlags_WidthFixed, availableWidth * 0.67f);

    snprintf(buf, sizeof(buf), "%.2f", tableTotal);
    ImGui::TableSetupColumn(buf, ImGuiTableColumnFlags_WidthFixed, availableWidth * 0.33f);

    Utils::HeaderCentered(2);
    ImGui::EndTable();
  }
}

void Statements::OnRenderIncomeStatement() {
  ImGuiTableFlags flags = ImGuiTableFlags_Borders | ImGuiTableFlags_BordersV | ImGuiTableFlags_SizingStretchProp;
  ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(0,0));
  if (ImGui::BeginTable("##IncomeStatement", 1, flags | ImGuiTableFlags_RowBg)) {
    ImGui::TableSetupColumn("Income Statment");
    Utils::HeaderCentered(1);

    ImGui::TableNextRow();
    ImGui::TableSetColumnIndex(0);

    RenderIncomeStatementTables("Income", m_IncomeAccounts, m_IncomeAccountsTotal);
    
    ImGui::TableNextRow();
    ImGui::TableSetColumnIndex(0);

    RenderIncomeStatementTables("Expenses", m_ExpenseAccounts, m_ExpenseAccountsTotal);
    
    ImGui::TableNextRow();
    ImGui::TableSetColumnIndex(0);

    float availableWidth = ImGui::GetContentRegionAvail().x;
    if (ImGui::BeginTable("##RetainedEarningsSection", 2, flags)) {
      char buf[32] = {0};
      snprintf(buf, sizeof(buf), "Net %s", m_RetainedEarnings < 0 ? "Loss" : "Income");
      ImGui::TableSetupColumn(buf, ImGuiTableColumnFlags_WidthFixed, availableWidth * 0.67f);

      snprintf(buf, sizeof(buf), "%.2f", m_RetainedEarnings);
      ImGui::TableSetupColumn(buf, ImGuiTableColumnFlags_WidthFixed, availableWidth * 0.33f);

      Utils::HeaderCentered(2);
      ImGui::EndTable();
    }

    ImGui::EndTable();
  }

  ImGui::PopStyleVar();
}

void Statements::OnRenderRetainedEarnings() {
  ImGuiTableFlags flags = ImGuiTableFlags_Borders | ImGuiTableFlags_BordersV | ImGuiTableFlags_SizingStretchProp;
  ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(0,0));
  if (ImGui::BeginTable("##RetainedEarnings", 1, flags | ImGuiTableFlags_RowBg)) {
    ImGui::TableSetupColumn("Retained Earnings");

    Utils::HeaderCentered(1);

    ImGui::TableNextRow();
    ImGui::TableSetColumnIndex(0);

    float availableWidth = ImGui::GetContentRegionAvail().x;
    if (ImGui::BeginTable("##IncomeSection", 3, flags)) {
      ImGui::TableSetupColumn("Category", ImGuiTableColumnFlags_WidthFixed, availableWidth * 0.25);
      ImGui::TableSetupColumn("Description", ImGuiTableColumnFlags_WidthFixed, availableWidth * 0.5);
      ImGui::TableSetupColumn("Amount", ImGuiTableColumnFlags_WidthFixed, availableWidth * 0.25);

      Utils::HeaderCentered(3);

      ImGui::TableNextRow();

      ImGui::TableSetColumnIndex(0);
      ImGui::Text("Beginning Balance");

      ImGui::TableSetColumnIndex(1);
      ImGui::Text("Balance retained from previous period");

      ImGui::TableSetColumnIndex(2);
      ImGui::Text("%.2f", m_BeginningBalance);

      ImGui::TableNextRow();
      ImGui::TableSetColumnIndex(0);
      ImGui::Dummy(ImVec2(ImGui::GetColumnWidth(), g_EntrySize.y));

      ImGui::TableNextRow();

      ImGui::TableSetColumnIndex(0);
      ImGui::Text("Income Statement");

      ImGui::TableSetColumnIndex(1);
      ImGui::Text("Total retained from this period alone");

      ImGui::TableSetColumnIndex(2);
      ImGui::Text("%.2f", m_RetainedEarnings);

      ImGui::TableNextRow();
      ImGui::TableSetColumnIndex(0);
      ImGui::Dummy(ImVec2(ImGui::GetColumnWidth(), g_EntrySize.y));

      ImGui::EndTable();
    }

    ImGui::TableNextRow();
    ImGui::TableSetColumnIndex(0);

    if (ImGui::BeginTable("##TotalRetainedEarnings", 2)) {
      ImGui::TableSetupColumn("Retained Earnings", ImGuiTableColumnFlags_WidthFixed, availableWidth * 0.8);
      char buf[32] = {0};
      snprintf(buf, sizeof(buf), "%.2f", m_RetainedEarnings + m_BeginningBalance);
      ImGui::TableSetupColumn(buf, ImGuiTableColumnFlags_WidthFixed, availableWidth * 0.2f);

      Utils::HeaderCentered(2);
      ImGui::EndTable();
    }

    ImGui::EndTable();
  }

  ImGui::PopStyleVar();
}

void Statements::RenderBalanceStatementTables(const std::string &tableName, const std::vector<AccountTableStatementData> &accounts) {
  ImGui::TextUnformatted(tableName.c_str());

  ImGui::TableNextRow();
  for (auto &table : accounts) {
    ImGui::TableSetColumnIndex(0);
    ImGui::Text("*  %s (%d)", table.Name.c_str(), table.Account);
    ImGui::TableSetColumnIndex(1);
    ImGui::Text("%.2f", table.Amount);
    ImGui::TableNextRow();
  }

  ImGui::TableSetColumnIndex(0);
  ImGui::Dummy(ImVec2(ImGui::GetColumnWidth(), g_EntrySize.y));
}

void Statements::OnRenderBalanceStatement() {
  ImGuiTableFlags flags = ImGuiTableFlags_Borders | ImGuiTableFlags_BordersV | ImGuiTableFlags_SizingStretchProp;
  ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(0,0));
  if (ImGui::BeginTable("##BalanceStatment", 1, flags | ImGuiTableFlags_RowBg)) {
    ImGui::TableSetupColumn("Balance Statement");
    Utils::HeaderCentered(1);

    ImGui::TableNextRow();
    ImGui::TableSetColumnIndex(0);

    float availableWidth = ImGui::GetContentRegionAvail().x;
    if (ImGui::BeginTable("##BalanceStatementSections", 2, flags)) {
      ImGui::TableSetupColumn("Assets", ImGuiTableColumnFlags_WidthFixed, availableWidth * 0.5f);
      ImGui::TableSetupColumn("Liabilites", ImGuiTableColumnFlags_WidthFixed, availableWidth * 0.5f);

      Utils::HeaderCentered(2);
      ImGui::TableNextRow();

      ImGui::TableSetColumnIndex(0);
      if (ImGui::BeginTable("BalanceStatementAssets", 2, flags)) {
        ImGui::TableSetupColumn("##EmptyHeader", ImGuiTableColumnFlags_WidthFixed, availableWidth * 0.25f);
        ImGui::TableSetupColumn("##EmptyHeader", ImGuiTableColumnFlags_WidthFixed, availableWidth * 0.25f);

        Utils::HeaderCentered(2);

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);

        RenderBalanceStatementTables("Cash and Cash Equivalents", m_CashAccounts);

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);

        RenderBalanceStatementTables("Other Assets", m_OtherAssetsAccounts);

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::TextUnformatted("Total");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%.2f", m_OtherAssetAccountsTotal + m_CashAccountsTotal);

        ImGui::EndTable();
      }

      ImGui::TableSetColumnIndex(1);

      if (ImGui::BeginTable("BalanceStatementLiabilites", 2, flags)) {
        ImGui::TableSetupColumn("##EmptyHeader", ImGuiTableColumnFlags_WidthFixed, availableWidth * 0.25f);
        ImGui::TableSetupColumn("##EmptyHeader", ImGuiTableColumnFlags_WidthFixed, availableWidth * 0.25f);

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);

        RenderBalanceStatementTables("Short Term Liabilities", m_ShortTermLiabilityAccounts);

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);

        RenderBalanceStatementTables("Long Term Liabilities", m_LongTermLiabilityAccounts);

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::TextUnformatted("Total");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%.2f", m_LongTermLiabilityAccountsTotal + m_ShortTermLiabilityAccountsTotal);

        ImGui::EndTable();
      }
      ImGui::EndTable();
    }
    ImGui::TableNextRow();
    ImGui::TableSetColumnIndex(0);

    if (ImGui::BeginTable("##BalanceStatementTotal", 2, flags)) {
      char buf[32] = {0};
      float total = (m_CashAccountsTotal + m_OtherAssetAccountsTotal) - (m_ShortTermLiabilityAccountsTotal + m_LongTermLiabilityAccountsTotal);
      ImGui::TableSetupColumn("Net Worth", ImGuiTableColumnFlags_WidthFixed, availableWidth * 0.8);
      snprintf(buf, sizeof(buf), "%.2f", total);
      ImGui::TableSetupColumn(buf, ImGuiTableColumnFlags_WidthFixed, availableWidth * 0.2f);

      Utils::HeaderCentered(2);

      ImGui::EndTable();
    }
    ImGui::EndTable();
  }
  ImGui::PopStyleVar();
}

void Statements::ReplaceTable(const uint64_t &oldHash, const std::string &newName, const std::string &newParentLegalName, const uint64_t &parentHash) {
  if (!TableExists(oldHash))
    return;

  Table table_copy = GetTable(oldHash); // Copy Table
  table_copy.SetName(newName, newParentLegalName);
  uint64_t newHash = table_copy.GetHash();
  m_TableMap.emplace(std::pair<uint64_t, Table>(newHash, table_copy));

  Table &table = m_TableMap.at(newHash);
  for (auto &[index, data] : table.GetElements()) {
    switch (data.Type) {
      case ElementType::Table: {
        Table &currTable = m_TableMap.at(data.ElementHash);
        ReplaceTable(data.ElementHash, currTable.GetName(), newParentLegalName + newName, table.GetHash());
        break;
      }
      case ElementType::DataSet: {
        DataSet &currDataSet = m_DataSetMap.at(data.ElementHash);
        ReplaceDataSet(data.ElementHash, currDataSet.GetName(), newParentLegalName + newName, table.GetHash(), currDataSet.GetTracking(), currDataSet.GetIncrementsTotal());
        break;
      }
      case ElementType::String: {
        std::string currString = m_Strings.at(data.ElementHash);
        ReplaceString(data.ElementHash, currString, newParentLegalName + newName, table.GetHash());
      }
      default: break;
    }
  }
 
  m_TableMap.erase(oldHash);
}

void Statements::RemoveTable(const uint64_t &hash, const uint64_t &parentHash) {
  if (!TableExists(hash))
    return;

  Table &table = m_TableMap.at(hash);
  for (auto &[index, data] : table.GetElements()) {
    switch (data.Type) {
      case ElementType::Table: {
        RemoveTable(data.ElementHash, hash);
        break;
      }
      case ElementType::DataSet: {
        RemoveDataSet(data.ElementHash, hash);
        break;
      }
      case ElementType::String: {
        RemoveString(data.ElementHash, hash);
      }
      default: break;
    }
  }

  GetTable(parentHash).RemoveElement(hash);
  m_TableMap.erase(hash);
}

void Statements::AddTable(const int32_t &index, const std::string &name, const std::string &parentLegalName, const uint64_t &parentHash) {
  uint64_t hash = Utils::GenerateHash64(parentLegalName + name);
  if (TableExists(hash)) {
    return;
  }

  if (index == 0 && parentHash == 0) { // Root table
    m_RootTableHashes.push_back(hash);
  } else {
    Table &parentTable = GetTable(parentHash);
    parentTable.AddElement(index, hash, ElementType::Table);
  }

  m_TableMap.emplace(std::pair<uint64_t, Table>(hash, Table(name, parentLegalName, parentHash)));
}

void Statements::ReplaceDataSet(const uint64_t &oldHash, const std::string &newName, const std::string &newParentLegalName, const uint64_t &parentHash, const TableTracking &tracking, const bool &incrementsTable) {
  if (!DataSetExists(oldHash))
    return;

  DataSet dataSet = GetDataSet(oldHash); // Copy Data Set
  dataSet.SetName(newName, newParentLegalName);
  dataSet.SetTracking(tracking);
  dataSet.SetIncrementsTotal(incrementsTable);
  uint64_t newID = dataSet.GetHash();

  Table &parentTable = GetTable(parentHash);

  parentTable.ReplaceElement(oldHash, newID, ElementType::DataSet);
  dataSet.NewDataAvailable();

  m_DataSetMap.erase(oldHash);
  m_DataSetMap.emplace(std::pair<uint64_t, DataSet>(newID, dataSet));
}

void Statements::RemoveDataSet(const uint64_t &hash, const uint64_t &parentHash) {
  if (!DataSetExists(hash)) {
    return;
  }

  Table &parentTable = m_TableMap.at(parentHash);
  parentTable.RemoveElement(hash);
  m_DataSetMap.erase(hash);
}

void Statements::AddDataSet(const int32_t &idx, const std::string &name, const std::string &parentLegalName, const uint64_t &parentHash, const TableTracking &tracking, const bool &incrementsTable) {
  uint64_t hash = Utils::GenerateHash64(parentLegalName + name);
  if (DataSetExists(hash)) {
    return;
  }

  Table &parentTable = GetTable(parentHash);
  parentTable.AddElement(idx, hash, ElementType::DataSet);
  m_DataSetMap.emplace(std::pair<uint64_t, DataSet>(hash, DataSet(name, parentLegalName, parentHash, tracking, incrementsTable)));
}

void Statements::ReplaceString(const uint64_t &oldHash, const std::string &newString, const std::string &newParentLegalName, const uint64_t &parentHash) {
  if (!StringExists(oldHash))
    return;

  uint64_t newHash = Utils::GenerateHash64(newParentLegalName + newString);
  Table &parentTable = GetTable(parentHash);

  parentTable.ReplaceElement(oldHash, newHash, ElementType::String);

  m_Strings.erase(oldHash);
  m_Strings.emplace(std::pair<uint64_t, std::string>(newHash, newString));
}

void Statements::RemoveString(const uint64_t &hash, const uint64_t &parentHash) {
  if (!StringExists(hash)) {
    return;
  }

  Table &parentTable = m_TableMap.at(parentHash);
  parentTable.RemoveElement(hash);
  m_DataSetMap.erase(hash);
}

void Statements::AddString(const int32_t &index, const std::string &str, const std::string &parentLegalName, const uint64_t &parentHash) {
  uint64_t hash = Utils::GenerateHash64(parentLegalName + str);
  if (StringExists(hash)) {
    return;
  }

  Table &parentTable = GetTable(parentHash);
  parentTable.AddElement(index, hash, ElementType::String);
  m_Strings.emplace(std::pair<uint64_t, std::string>(hash, str));
}

}
