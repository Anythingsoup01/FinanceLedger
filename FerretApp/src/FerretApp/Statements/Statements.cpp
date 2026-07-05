#include "Statements.h"

#include "FerretApp/Ledger/Ledger.h"

#include "FerretApp/Utils/Utils.h"

#include <format>

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

  float incomeAccountsTotal = 0;
  float expenseAccountsTotal = 0;

  int largestAccountNameSize = 0;

  for (auto &[id, table] : Ledger::GetTables()) {
    switch (table.GetTracking()) {
      case TableTracking::Income: {
        incomeTables.push_back(table);
        incomeAccountsTotal += table.GetBalance();
        break;
      }
      case TableTracking::Expenses: {
        expenseTables.push_back(table);
        expenseAccountsTotal += table.GetBalance();
        break;
      }
      default: break;
    }

    int accountNameSize = Utils::GetDigitCount(table.GetAccountNumber()) + table.GetName().length() + 3;
    if (largestAccountNameSize < accountNameSize) {
      largestAccountNameSize = accountNameSize;
    }
  }

  std::string fmt = "{0" + std::to_string(largestAccountNameSize * 3) + "}";
  std::string formattedStr = fmt::format(, "#");
  ImVec2 entrySize = ImGui::CalcTextSize(formattedStr.c_str());
  ImGuiTableFlags flags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersV | ImGuiTableFlags_SizingStretchProp;

  ImVec2 tableSize = ImVec2(entrySize.x, (incomeTables.size() + expenseTables.size() + 8) * entrySize.y); // 4 being headers, Income Statement, Income, Expense + 2 for dummy items + 1 for total

  float total = 0;
  ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(0, 0));
  if (ImGui::BeginTable("##IncomeStatement", 3, flags, tableSize)) {
    ImGui::TableSetupColumn("##EmptyHeader", ImGuiTableColumnFlags_WidthFixed, tableSize.x / 3.0f);
    ImGui::TableSetupColumn("Income Statement", ImGuiTableColumnFlags_WidthFixed, tableSize.x / 3.0f);
    ImGui::TableSetupColumn("##EmptyHeader", ImGuiTableColumnFlags_WidthFixed, tableSize.x / 3.0f);
    Utils::HeaderCentered(3);

    ImGui::TableNextRow();
    ImGui::TableSetColumnIndex(0);

    ImGui::Text("Income");

    for (auto &table : incomeTables) {
      ImGui::TableNextRow();
      ImGui::TableSetColumnIndex(1);
      ImGui::Text("%s (%d)", table.GetName().c_str(), table.GetAccountNumber());
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

    ImGui::TableNextRow();
    ImGui::TableSetColumnIndex(0);
    ImGui::Text("Total");
    ImGui::TableSetColumnIndex(2);
    ImGui::Text("%.2f", incomeAccountsTotal);

    ImGui::TableNextRow();
    ImGui::TableSetColumnIndex(0);
    ImGui::Text("Expenses");

    for (auto &table : expenseTables) {
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

    ImGui::TableNextRow();
    ImGui::TableSetColumnIndex(0);
    ImGui::Text("Total");
    ImGui::TableSetColumnIndex(2);
    ImGui::Text("%.2f", expenseAccountsTotal);

    ImGui::TableNextRow();
    ImGui::TableSetColumnIndex(0);

    float netTotal = incomeAccountsTotal - expenseAccountsTotal;
    ImGui::Text("Net %s", netTotal < 0 ? "Loss" : "Income");

    ImGui::TableSetColumnIndex(2);
    ImGui::Text("%.2f", netTotal);

    ImGui::EndTable();
  }

  ImGui::PopStyleVar();
  // |  Income Statement  |
  // | Income |     |     |
  // |        | 101 | AMT |
}

}
