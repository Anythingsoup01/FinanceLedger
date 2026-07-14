#include "Statements.h"

#include "FerretApp/Ledger/Table.h"
#include "FerretApp/Layer/FerretLayer.h"

#include "FerretApp/Utils/Utils.h"

extern ImVec2 g_EntrySize;

namespace Ferret {

void Statements::OnRenderData() {
  OnRenderIncomeStatement();
  ImGui::Dummy(ImVec2(0, 50));
  OnRenderRetainedEarnings();
  ImGui::Dummy(ImVec2(0, 50));
  OnRenderBalanceStatement();
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

}
