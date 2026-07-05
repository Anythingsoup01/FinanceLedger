#include "EntryTable.h"

#include "FerretApp/Utils/Utils.h"

#include "Ledger.h"
#include "FerretApp/Statements/Statements.h"

extern ImVec2 g_EntrySize;

namespace Ferret {

bool EntryTable::InsertEntryData(const Date &date, const int &account, const float &amount, bool updateOther) {
  // The ID is to allow users to click and edit the entry
  if (date.Day == 0 || date.Month == 0 || date.Year == 0 ||
      account == 0 || amount == 0) {
    return false;
  }
  int id = date.Day + date.Month + date.Year + account;

  // Entry already exists
  if (m_Entries.find(id) != m_Entries.end()) {
    return false;
  }

  // Insert the new data to the table
  Entry data = Entry(date, account, amount);
  m_Entries.emplace(std::pair<int, Entry>(id, data));

  if (updateOther)
    Ledger::SubmitEntryDataToTable(account, !m_CreditTable, date, m_AccountID, amount);

  // Increment the total value of the table
  m_TotalValue += amount;
  if (!m_CreditTable &&
      Ledger::GetTables().find(m_AccountID) != Ledger::GetTables().end() &&
      !Ledger::GetTables().at(m_AccountID).IsCreditAccount()) {
    Statements::UpdateBeginningBalance(amount);
  }

  return true;
}

void EntryTable::RemoveEntry(int id) {
  // Entry doesn't exist
  if (m_Entries.find(id) == m_Entries.end()) {
    return;
  }

  // Decrement the amount before erasing
  const Entry &entry = m_Entries[id];
  m_TotalValue -= entry.GetAmount();
  if (!m_CreditTable && !Ledger::GetTables().at(m_AccountID).IsCreditAccount()) {
    Statements::UpdateBeginningBalance(-entry.GetAmount());
  }


  m_Entries.erase(id);
}

void EntryTable::RemoveEntriesFromTable(const int &tableID) {
  for (auto it = m_Entries.begin(); it != m_Entries.end();) {
    if (it->second.GetAccountID() == tableID) {
      m_TotalValue -= it->second.GetAmount();
      it = m_Entries.erase(it);
    } else {
      ++it;
    }
  }
}

Entry &EntryTable::GetEntry(const int &entryID) {
  if (m_Entries.find(entryID) == m_Entries.end()) {
    static Entry nullEntry = {};
    return nullEntry;
  }

  return m_Entries.at(entryID);
}

bool EntryTable::RenderTable(const std::string &tableName, const int &tableIndex) {
  ImVec2 tableSize = ImVec2(g_EntrySize.x, g_EntrySize.y * m_Entries.size());
  ImGuiTableFlags flags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersV | ImGuiTableFlags_SizingStretchProp;

  ImGui::TableSetColumnIndex(tableIndex);

  char buf[32];
  sprintf(buf, "##%s-%i", tableName.c_str(), m_AccountID);
  if (ImGui::BeginTable(buf, 3, flags, tableSize)) {
    ImGui::TableSetupColumn("Date (MDY)", ImGuiTableColumnFlags_WidthStretch, ImGui::CalcTextSize("##/##/####").x);
    ImGui::TableSetupColumn("Account", ImGuiTableColumnFlags_WidthStretch, ImGui::CalcTextSize("Account").x);
    ImGui::TableSetupColumn("Amount", ImGuiTableColumnFlags_WidthStretch, ImGui::CalcTextSize("Amount").x);

    Utils::HeaderCentered(3);

    for (auto &[id, entry] : m_Entries) {
      ImGui::TableNextRow();
      if (entry.RenderEntry())
        Ledger::ViewEntry(m_AccountID, m_CreditTable, id);
    }
  }

  ImGui::TableNextRow();
  bool submitted = false;

  ImGui::TableSetColumnIndex(0);
  float inputWidth = ImGui::GetColumnWidth() / 3.f;
  ImGui::PushItemWidth(inputWidth);
  ImGui::DragInt("##M", &m_DateBuffer.Month, 0, 0, 0, "%02d");
  if (Utils::IsSubitted()) {
    submitted = true;
  }
  ImGui::SameLine(inputWidth);
  ImGui::DragInt("##D", &m_DateBuffer.Day, 0, 0, 0, "%02d");
  if (Utils::IsSubitted()) {
    submitted = true;
  }
  ImGui::SameLine((inputWidth * 2));
  ImGui::DragInt("##Y", &m_DateBuffer.Year, 0, 0, 0, "%04d");
  if (Utils::IsSubitted()) {
    submitted = true;
  }
  ImGui::PopItemWidth();

  ImGui::TableSetColumnIndex(1);
  ImGui::SetNextItemWidth(-FLT_MIN);
  sprintf(buf, "##NewAcc%s", tableName.c_str());
  auto &tables = Ledger::GetTables();
  const AccountTable &retainedEarnigns = Ledger::GetRetainedEarningsTable();
  char accBuf[32] = { 0 };
  if (m_AccountBuffer == retainedEarnigns.GetAccountNumber()) {
    snprintf(accBuf, sizeof(accBuf), "Retained");
  } else {
    snprintf(accBuf, sizeof(accBuf), "%i", m_AccountBuffer != 0 ? Ledger::GetTables().at(m_AccountBuffer).GetAccountNumber() : 0);
  }
  if (ImGui::BeginCombo(buf, accBuf)) {
    if (m_AccountBuffer != retainedEarnigns.GetAccountNumber()) {
      const bool is_selected = (m_AccountBuffer == retainedEarnigns.GetAccountNumber());
      if (ImGui::Selectable("Retained", is_selected)) {
        m_AccountBuffer = retainedEarnigns.GetAccountNumber();
      }

      if (is_selected)
        ImGui::SetItemDefaultFocus();
    }
    for (auto &[id, table] : tables) {
      if (id == m_AccountID) { // We can't add or remove money into the same account
        continue;
      }
      const bool is_selected = (m_AccountBuffer == id);
      char buf[32] = { 0 };
      snprintf(buf, sizeof(buf), "%i", table.GetAccountNumber());
      if (ImGui::Selectable(buf, is_selected)) {
        m_AccountBuffer = id;
      }

      // Set the initial focus when opening the combo (keyboard navigation)
      if (is_selected) {
        ImGui::SetItemDefaultFocus();
      }
    }
    ImGui::EndCombo();
  }

  ImGui::TableSetColumnIndex(2);
  ImGui::SetNextItemWidth(-FLT_MIN);
  sprintf(buf, "##NewAmt%s", tableName.c_str());
  ImGui::DragFloat(buf, &m_AmountBuffer, 0, 0, 0, "$%.2f");
  if (Utils::IsSubitted()) {
    submitted = true;
  }

  if (submitted && InsertEntryData(m_DateBuffer, m_AccountBuffer, m_AmountBuffer, m_AccountBuffer != retainedEarnigns.GetAccountNumber())) {
    memset(&m_DateBuffer, 0, sizeof(Date));
    memset(&m_AccountBuffer, 0, sizeof(int));
    memset(&m_AmountBuffer, 0, sizeof(float));
    Ledger::SetDirty();
  }

  ImGui::EndTable();

  return submitted;
}

void EntryTable::RenderTableTotal(const std::string &tableName, const int &columnIndex) {
ImGui::TableSetColumnIndex(columnIndex);
  ImGuiTableFlags flags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersV | ImGuiTableFlags_SizingStretchProp;

  char buf[32];
  sprintf(buf, "##%s-%i", tableName.c_str(), m_AccountID);
  if (ImGui::BeginTable(buf, 2, flags, g_EntrySize)) {
    ImGui::TableSetupColumn("##TotalTxt", ImGuiTableColumnFlags_WidthFixed, (g_EntrySize.x * 2) / 3.0) ;
    ImGui::TableSetupColumn("##Amount", ImGuiTableColumnFlags_WidthFixed, g_EntrySize.x / 3.0);

    ImGui::TableNextRow(ImGuiTableRowFlags_Headers);
    ImGui::TableSetColumnIndex(0);

    const char *totalTxt = "Total";
    // Calculate dimensions
    float column_width = ImGui::GetContentRegionAvail().x;
    float text_width = ImGui::CalcTextSize(totalTxt).x;

    // Offset the cursor position to center the text
    if (column_width > text_width) {
      ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (column_width - text_width) * 0.5f);
    }

    ImGui::TableHeader(totalTxt);

    ImGui::TableSetColumnIndex(1);

    snprintf(buf, sizeof(buf), "$%.2f", m_TotalValue);
    // Calculate dimensions
    column_width = ImGui::GetContentRegionAvail().x;
    text_width = ImGui::CalcTextSize(buf).x;

    // Offset the cursor position to center the text
    if (column_width > text_width) {
      ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (column_width - text_width) * 0.5f);
    }

    ImGui::TextUnformatted(buf);

    ImGui::EndTable();
  }
}

}
