#include "Statements.h"

#include "FerretApp/Ledger/Ledger.h"

#include "FerretApp/Utils/Utils.h"

namespace Ferret {

void Statements::OnRenderData() {
  OnRenderIncome();
  ImGui::SameLine();
  OnRenderRetainedEarnings();
}

void Statements::NewDataAvailable() {
  m_IncomeAccounts.clear();
  m_ExpenseAccounts.clear();

  m_IncomeAccountsTotal = 0;
  m_ExpenseAccountsTotal = 0;

  int maxAccountDigitCount = 0;
  int maxAmountDigitCount = 0;

  for (auto &[id, table] : Ledger::GetTables()) {
    switch (table.GetTracking()) {
      case TableTracking::Income: {
        m_IncomeAccounts.push_back({table.GetName(), table.GetAccountNumber(), table.GetBalance()});
        m_IncomeAccountsTotal += table.GetBalance();
        int accountDigitCount = Utils::GetDigitCount(table.GetAccountNumber()) + table.GetName().length() + 3;
        if (maxAccountDigitCount < accountDigitCount) {
          maxAccountDigitCount = accountDigitCount;
        }

        int amountDigitCount = Utils::GetDigitCount((int)(table.GetBalance() * 100)) + 3;
        if (maxAmountDigitCount < amountDigitCount) {
          maxAmountDigitCount = amountDigitCount;
        }
        break;
      }
      case TableTracking::Expenses: {
        m_ExpenseAccounts.push_back({table.GetName(), table.GetAccountNumber(), table.GetBalance()});
        m_ExpenseAccountsTotal += table.GetBalance();
        int accountDigitCount = Utils::GetDigitCount(table.GetAccountNumber()) + table.GetName().length() + 3;
        if (maxAccountDigitCount < accountDigitCount) {
          maxAccountDigitCount = accountDigitCount;
        }

        int amountDigitCount = Utils::GetDigitCount((int)(table.GetBalance() * 100)) + 3;
        if (maxAmountDigitCount < amountDigitCount) {
          maxAmountDigitCount = amountDigitCount;
        }
        break;
      }
      default: break;
    }
  }

  if (m_TypeColWidth == 0) {
    ImVec2 size = ImGui::CalcTextSize("Net Income  ");
    m_TypeColWidth = size.x;
    m_RowHeight = size.y;
  }

  std::string formattedStr = fmt::format("{:0{}}", 0, maxAccountDigitCount);
  std::string incomeStatementStr = "Income Statement";
  m_AccountColWidth = formattedStr.length() > incomeStatementStr.length() ? ImGui::CalcTextSize(formattedStr.c_str()).x : ImGui::CalcTextSize(incomeStatementStr.c_str()).x;
  formattedStr = fmt::format("{:0{}}", 0, maxAmountDigitCount);
  m_AmountColWidth = ImGui::CalcTextSize(formattedStr.c_str()).x;

  m_RetainedEarnings = m_IncomeAccountsTotal - m_ExpenseAccountsTotal;
}

void Statements::OnRenderIncome() {
  ImVec2 tableSize = ImVec2(m_TypeColWidth + m_AmountColWidth + m_AccountColWidth, (m_IncomeAccounts.size() + m_ExpenseAccounts.size() + 8) * m_RowHeight); // 4 being headers, Income Statement, Income, Expense + 2 for dummy items + 1 for total
  ImGuiTableFlags flags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersV | ImGuiTableFlags_SizingStretchProp;
  ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(0, 0));
  if (ImGui::BeginTable("##IncomeStatement", 3, flags, tableSize)) {
    ImGui::TableSetupColumn("##EmptyHeader", ImGuiTableColumnFlags_WidthFixed, m_TypeColWidth);
    ImGui::TableSetupColumn("Income Statement", ImGuiTableColumnFlags_WidthFixed, m_AccountColWidth);
    ImGui::TableSetupColumn("##EmptyHeader", ImGuiTableColumnFlags_WidthFixed, m_AmountColWidth);
    Utils::HeaderCentered(3);

    ImGui::TableNextRow();
    ImGui::TableSetColumnIndex(0);

    ImGui::Text("Income");

    for (auto &table : m_IncomeAccounts) {
      ImGui::TableNextRow();
      ImGui::TableSetColumnIndex(1);
      ImGui::Text("%s (%d)", table.Name.c_str(), table.Account);
      ImGui::TableSetColumnIndex(2);
      ImGui::Text("%.2f", table.Amount);
    }

    ImGui::TableNextRow();
    ImGui::TableSetColumnIndex(0);
    ImGui::Dummy(ImVec2(ImGui::GetColumnWidth(), m_RowHeight));
    ImGui::TableSetColumnIndex(1);
    ImGui::Dummy(ImVec2(ImGui::GetColumnWidth(), m_RowHeight));
    ImGui::TableSetColumnIndex(2);
    ImGui::Dummy(ImVec2(ImGui::GetColumnWidth(), m_RowHeight));

    ImGui::TableNextRow();
    ImGui::TableSetColumnIndex(0);
    ImGui::Text("Total");
    ImGui::TableSetColumnIndex(2);
    ImGui::Text("%.2f", m_IncomeAccountsTotal);

    ImGui::TableNextRow();
    ImGui::TableSetColumnIndex(0);
    ImGui::Text("Expenses");

    for (auto &table : m_ExpenseAccounts) {
      ImGui::TableNextRow();
      ImGui::TableSetColumnIndex(1);
      ImGui::Text("%s (%d)", table.Name.c_str(), table.Account);
      ImGui::TableSetColumnIndex(2);
      ImGui::Text("%.2f", table.Amount);
    }

    ImGui::TableNextRow();
    ImGui::TableSetColumnIndex(0);
    ImGui::Dummy(ImVec2(ImGui::GetColumnWidth(), m_RowHeight));
    ImGui::TableSetColumnIndex(1);
    ImGui::Dummy(ImVec2(ImGui::GetColumnWidth(), m_RowHeight));
    ImGui::TableSetColumnIndex(2);
    ImGui::Dummy(ImVec2(ImGui::GetColumnWidth(), m_RowHeight));

    ImGui::TableNextRow();
    ImGui::TableSetColumnIndex(0);
    ImGui::Text("Total");
    ImGui::TableSetColumnIndex(2);
    ImGui::Text("%.2f", m_ExpenseAccountsTotal);

    ImGui::TableNextRow();
    ImGui::TableSetColumnIndex(0);

    ImGui::Text("Net %s", m_RetainedEarnings < 0 ? "Loss" : "Income");

    ImGui::TableSetColumnIndex(2);
    ImGui::Text("%.2f", m_RetainedEarnings);

    ImGui::EndTable();
  }

  ImGui::PopStyleVar();
}

void Statements::OnRenderRetainedEarnings() {


}

}
