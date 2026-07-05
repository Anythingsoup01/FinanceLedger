#include "Statements.h"

#include "FerretApp/Ledger/Ledger.h"

#include "FerretApp/Utils/Utils.h"

namespace Ferret {

void Statements::OnRenderData() {
  if (m_RenderIncome) {
    OnRenderIncome();
  }
}

void Statements::OnRenderIncome() {
  // TODO: Cache and Serialize these!
  std::vector<AccountTable> incomeTables;
  std::vector<AccountTable> expenseTables;

  int maxAmountLength = 0;
  int maxAccountLength = 0;

  for (auto &[id, table] : Ledger::GetTables()) {
    switch (table.GetTracking()) {
      case TableTracking::Income: {
        incomeTables.push_back(table);
        break;
      }
      case TableTracking::Expenses: {
        expenseTables.push_back(table);
        break;
      }
      default: break;
    }

    int currentAmountLen = Utils::GetPositiveDigitCount((int)(table.GetBalance() * 100)); // * 100 to make 100.00 to 10000 for accurate digit count
    int currentAccountLen = Utils::GetPositiveDigitCount(table.GetAccountNumber());

    if (currentAmountLen > maxAmountLength) {
      maxAmountLength = currentAmountLen;
    }

    if (currentAccountLen > maxAmountLength) {
      maxAccountLength = currentAccountLen;
    }
  }

  static char buf[128];
  snprintf(buf, sizeof(buf), "|123456Income Statment123456|");
  ImVec2 entrySize = ImGui::CalcTextSize(buf);
  ImGuiTableFlags flags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersV | ImGuiTableFlags_SizingStretchProp;

  ImVec2 tableSize = ImVec2(entrySize.x, (incomeTables.size() + expenseTables.size() + 6) * entrySize.y); // 4 being headers, Income Statement, Income, Expense + 2 for dummy items + 1 for total

  float total = 0;
  ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(0, 0));
  if (ImGui::BeginTable("##IncomeStatement", 1, flags, tableSize)) {
    ImGui::TableSetupColumn("Income Statement", ImGuiTableColumnFlags_WidthFixed);
    Utils::HeaderCentered(1);

    ImGui::TableNextRow();
    ImGui::TableSetColumnIndex(0);

    tableSize = ImVec2(entrySize.x, (incomeTables.size() + 2) * entrySize.y); // 2 being Income and total headers
    if (ImGui::BeginTable("##IncomeStatementIncome", 3, flags)) {
      ImGui::TableSetupColumn("Income", ImGuiTableColumnFlags_WidthFixed, tableSize.x / 3.0);
      ImGui::TableSetupColumn("##EmptyHeader", ImGuiTableColumnFlags_WidthFixed, tableSize.x / 3.0);
      ImGui::TableSetupColumn("##EmptyHeader", ImGuiTableColumnFlags_WidthFixed, tableSize.x / 3.0);
      Utils::HeaderCentered(1);

      for (auto &table : incomeTables) {
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%d", table.GetAccountNumber());
        ImGui::TableSetColumnIndex(2);
        ImGui::Text("%.2f", table.GetBalance());
      }

      ImGui::TableNextRow();
      ImGui::TableSetColumnIndex(0);
      ImGui::Dummy(ImVec2(ImGui::GetColumnWidth(), entrySize.y));
      ImGui::TableSetColumnIndex(1);
      ImGui::Dummy(ImVec2(ImGui::GetColumnWidth(), entrySize.y));
      ImGui::TableSetColumnIndex(2);
      ImGui::Dummy(ImVec2(ImGui::GetColumnWidth(), entrySize.y));
      ImGui::EndTable();
    }

    tableSize = ImVec2(entrySize.x, (expenseTables.size() + 2) * entrySize.y); // 2 being Income and total headers
    if (ImGui::BeginTable("##IncomeStatementExpenses", 3, flags)) {
      ImGui::TableSetupColumn("Expenses");
      Utils::HeaderCentered(1);

      ImGui::TableNextRow();
      ImGui::TableSetColumnIndex(0);

      for (auto &table : expenseTables) {
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%d", table.GetAccountNumber());
        ImGui::TableSetColumnIndex(2);
        ImGui::Text("%.2f", table.GetBalance());
        ImGui::TableNextRow();
      }
      ImGui::EndTable();
    }


    ImGui::EndTable();
  }

  ImGui::PopStyleVar();
  // |  Income Statement  |
  // | Income |     |     |
  // |        | 101 | AMT |
}

}
