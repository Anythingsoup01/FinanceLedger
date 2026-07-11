#include "Table.h"

#include "FerretApp/Statements/Statements.h"

#include "FerretApp/Layer/FerretLayer.h"
#include "FerretApp/Popup/Popup.h"

#include "Ferret/Core/Application.h"

#include "FerretApp/Utils/Utils.h"

extern ImVec2 g_EntrySize;

namespace Ferret {

TableTracking StringToTableTracking(const std::string &trackStr) {
  if (trackStr == "Untracked") {
    return TableTracking::Untracked;
  } else if (trackStr == "Income") {
    return TableTracking::Income;
  } else if (trackStr == "Expenses") {
    return TableTracking::Expenses;
  }

  return TableTracking::Untracked;
}

std::string TableTrackingToString(const TableTracking &tracking) {
  switch (tracking) {
    case TableTracking::Untracked: { return "Untracked"; }
    case TableTracking::Income:    { return "Income"; }
    case TableTracking::Expenses:  { return "Expenses"; }
    default: return "NOT IMPLEMENTED!";
  }
}

AccountTable::AccountTable(const std::string &accountName, const int &accountNumber, const bool &isCredit, const TableTracking &tracking)
  : m_Name(accountName),
    m_Number(accountNumber),
    m_CreditAccount(isCredit),
    m_Tracking(tracking),
    m_DebitTable(accountNumber, false),
    m_CreditTable(accountNumber, true) {
}

void AccountTable::InsertEntry(const bool &isCredit, const Date &date, const int &accountID, const float &amount, bool updateOther) {
  if (isCredit) {
    m_CreditTable.InsertEntryData(date, accountID, amount, updateOther);
  } else {
    m_DebitTable.InsertEntryData(date, accountID, amount, updateOther);
  }

  if (m_Tracking == TableTracking::Income || m_Tracking == TableTracking::Expenses) {
    Application::Get().SubmitToMainThread([](){FerretLayer::Get().GetStatements().NewDataAvailable();});
  }

  if (isCredit) {
    int id = m_Number;
    Application::Get().SubmitToMainThread([id, date](){FerretLayer::Get().GetJournal().NewDataAvailable(date, id);});
  }
}

void AccountTable::Draw() {
  // Since this draws normally, all we need is the helper
  if (m_TableSize.x == 0 || m_TableSize.y == 0) {
    ResizeTable();
  }

  ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(0, 0));
  ImGuiTableFlags flags = ImGuiTableFlags_Borders | ImGuiTableFlags_BordersV | ImGuiTableFlags_SizingStretchProp;

  char buf[32] = {0};
  snprintf(buf, sizeof(buf), "##%s-%i-root", m_Name.c_str(), m_Number);
  if (ImGui::BeginTable(buf, 1, flags, m_TableSize)) {


    sprintf(buf, "%s (%i)", m_Name.c_str(), m_Number);
    ImGui::TableSetupColumn(buf, ImGuiTableColumnFlags_WidthFixed, m_TableSize.x);
    
    ImGui::TableNextRow(ImGuiTableRowFlags_Headers);
    ImGui::TableSetColumnIndex(0);

    // TODO: Make the header interactable to display its config for editing and viewing

    if (ImGui::Button("X")) {
      FerretLayer::Get().GetLedger().RemoveTable(m_Number);
    }
    if (ImGui::IsItemHovered())
      ImGui::SetTooltip("Delete Table");

    ImGui::SameLine();

    const char* header_name = ImGui::TableGetColumnName(0);

    // Calculate dimensions
    float column_width = ImGui::GetContentRegionAvail().x;
    float text_width = ImGui::CalcTextSize(header_name).x;

    // Offset the cursor position to center the text
    if (column_width > text_width) {
      ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (column_width - text_width) * 0.5f);
    }

    ImGui::TableHeader(header_name);

    if (ImGui::IsItemHovered()) {
      ImGui::SetTooltip("Double Click to view Details");
    }

    ImGui::TableSetColumnIndex(0);
    if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0)) {
      Popup::ViewTable(m_Number);
    }


    ImGui::TableNextRow();

    ImGui::TableSetColumnIndex(0);

    // Current table height - one entry; to account for main table
    ImVec2 tableSize = ImVec2(m_TableSize.x, m_TableSize.y - g_EntrySize.y);

    snprintf(buf, sizeof(buf), "##%s-%i-entries", m_Name.c_str(), m_Number);
    if (ImGui::BeginTable(buf, 2, flags, tableSize)) {
      ImGui::TableSetupColumn("Debit", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoSort | ImGuiTableColumnFlags_NoHeaderLabel, tableSize.x / 2.0);
      ImGui::TableSetupColumn("Credit", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoSort | ImGuiTableColumnFlags_NoHeaderLabel, tableSize.x / 2.0);

      Utils::HeaderCentered(2);

      ImGui::TableNextRow();

      if (m_DebitTable.RenderTable("Debit", 0)) {
        ResizeTable();
      }

      if (m_CreditTable.RenderTable("Credit", 1)) {
        ResizeTable();
      }

      ImGui::TableNextRow();

      m_DebitTable.RenderTableTotal("Debit", 0);
      m_CreditTable.RenderTableTotal("Credit", 1);

      ImGui::EndTable();
    }
    


    // Account balance
    ImGui::TableNextRow();
    ImGui::TableSetColumnIndex(0);

    snprintf(buf, sizeof(buf), "##%s-%i-balance", m_Name.c_str(), m_Number);
    if (ImGui::BeginTable(buf, 2, flags, ImVec2(m_TableSize.x, g_EntrySize.y))) {
      ImGui::TableSetupColumn("##Balance", ImGuiTableColumnFlags_WidthFixed, tableSize.x * 0.67);
      ImGui::TableSetupColumn("##Amount", ImGuiTableColumnFlags_WidthFixed, tableSize.x * 0.33);

      ImGui::TableNextRow(ImGuiTableRowFlags_Headers);
      ImGui::TableSetColumnIndex(0);

      const char *totalTxt = "Balance";
      // Calculate dimensions
      float column_width = ImGui::GetContentRegionAvail().x;
      float text_width = ImGui::CalcTextSize(totalTxt).x;

      // Offset the cursor position to center the text
      if (column_width > text_width) {
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (column_width - text_width) * 0.5f);
      }

      ImGui::TableHeader(totalTxt);

      ImGui::TableSetColumnIndex(1);

      float amount = m_CreditAccount ? m_CreditTable.GetTotalValue() - m_DebitTable.GetTotalValue() :
                                       m_DebitTable.GetTotalValue() - m_CreditTable.GetTotalValue();
      snprintf(buf, sizeof(buf), "$%.2f", amount);
      // Calculate dimensions
      column_width = ImGui::GetContentRegionAvail().x;
      text_width = ImGui::CalcTextSize(buf).x;

      // Offset the cursor position to center the text
      if (column_width > text_width) {
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (column_width - text_width) * 0.5f);
      }

      ImGui::TableHeader(buf);
      ImGui::EndTable();
    }

    ImGui::EndTable();
  }
  ImGui::PopStyleVar();
}

void AccountTable::RemoveEntriesFromTable(const int &tableID) {
  m_CreditTable.RemoveEntriesFromTable(tableID);
  m_DebitTable.RemoveEntriesFromTable(tableID);
  ResizeTable();
}

void AccountTable::RemoveEntry(const bool &isCredit, const int &entryID) {
  if (isCredit) {
    m_CreditTable.RemoveEntry(entryID);
  } else {
    m_DebitTable.RemoveEntry(entryID);
  }
}

Entry &AccountTable::GetEntry(const bool &isCredit, const int &entryID) {
  if (isCredit) {
    return m_CreditTable.GetEntry(entryID);
  } else {
    return m_DebitTable.GetEntry(entryID);
  }
}

void AccountTable::ResizeTable() {
  float creditTableSize = m_CreditTable.GetCount();
  float debitTableSize = m_DebitTable.GetCount();
  m_TableSize = ImVec2(
    g_EntrySize.x * 2.0f,
    creditTableSize > debitTableSize
    ? g_EntrySize.y * (creditTableSize + 4)
    : g_EntrySize.y * (debitTableSize + 4)
  );
}

const float AccountTable::GetBalance() const {
  return m_CreditAccount ? m_CreditTable.GetTotalValue() - m_DebitTable.GetTotalValue() : m_DebitTable.GetTotalValue() - m_CreditTable.GetTotalValue();
}

}
