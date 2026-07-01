#include "Table.h"
#include "FerretApp/Layer/FerretLayer.h"

#include <imgui.h>

namespace Ferret {

namespace Utils {

void HeaderCentered(uint32_t columnCount) {
  ImGui::TableNextRow(ImGuiTableRowFlags_Headers);
  for (int column = 0; column < columnCount; column++) 
  {
    ImGui::TableSetColumnIndex(column);
    const char* header_name = ImGui::TableGetColumnName(column);

    // Calculate dimensions
    float column_width = ImGui::GetContentRegionAvail().x;
    float text_width = ImGui::CalcTextSize(header_name).x;

    // Offset the cursor position to center the text
    if (column_width > text_width) {
      ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (column_width - text_width) * 0.5f);
    }

    ImGui::TableHeader(header_name);
  }
}

bool HoveredAndEntered() {
  return ImGui::IsItemHovered() && (ImGui::IsKeyDown(ImGuiKey_Enter) || ImGui::IsKeyDown(ImGuiKey_KeypadEnter));
}

bool IsSubitted() {
  if (!ImGui::IsItemActive())
    return false;

  return ImGui::IsKeyDown(ImGuiKey_Enter) || ImGui::IsKeyDown(ImGuiKey_KeypadEnter);
}

}

//////////////////
//  EntryTable //
////////////////

EntryTable::EntryTable(int accountID, bool creditTable)
  : m_DateBuffer({0,0,0}), m_AccountIDBuffer(0), m_AmountBuffer(0), m_TotalValue(0), m_AccountID(accountID), m_CreditTable(creditTable) {}

bool EntryTable::InsertEntryData(const Date_t &date, const int &accountID, const float &amount, bool updateOther) {
  // The ID is to allow users to click and edit the entry
  if (date.Day == 0 || date.Month == 0 || date.Year == 0 ||
      accountID == 0 || amount == 0) {
    return false;
  }
  int id = date.Day + date.Month + date.Year + accountID;

  // Entry already exists
  if (m_Entries.find(id) != m_Entries.end()) {
    return false;
  }

  // Insert the new data to the table
  EntryData_t data = {date, accountID, amount};
  m_Entries.emplace(std::pair<int, EntryData_t>(id, data));

  if (updateOther)
    FerretLayer::Get()->SubmitEntryDataToTable(m_AccountIDBuffer, m_CreditTable, m_DateBuffer, m_AccountID, m_AmountBuffer);

  // Increment the total value of the table
  m_TotalValue += amount;

  return true;
}

bool EntryTable::OverWriteEntryData() {
  // The ID is to allow users to click and edit the entry
  int id = m_DateBuffer.Day + m_DateBuffer.Month + m_DateBuffer.Year + m_AccountIDBuffer;

  // Entry doesn't exist
  if (m_Entries.find(id) == m_Entries.end()) {
    return false;
  }
  // Decrement the value we are overwriting
  m_TotalValue -= m_Entries[id].GetAmount();

  // Completely overwrite the data found at this location
  EntryData_t data = {m_DateBuffer, m_AccountIDBuffer, m_AmountBuffer};
  m_Entries[id] = data;

  // Increment the total value of the table
  m_TotalValue += m_AmountBuffer;

  return true;
}

bool EntryTable::InsertSumEntryData() {
  // The ID is to allow users to click and edit the entry
  int id = m_DateBuffer.Day + m_DateBuffer.Month + m_DateBuffer.Year + m_AccountIDBuffer;

  // Entry doesn't exist
  if (m_Entries.find(id) == m_Entries.end()) {
    return false;
  }

  // Set the amount to equal the sum of the current amount and the added amount
  float newAmount = m_Entries[id].GetAmount() + m_AmountBuffer;
  m_Entries[id].SetAmount(newAmount);

  // Increment the total value of the table
  m_TotalValue += m_AmountBuffer;

  return true;
}

void EntryTable::RemoveEntryData(int id) {
  // Entry doesn't exist
  if (m_Entries.find(id) == m_Entries.end()) {
    return;
  }

  // Decrement the amount before erasing
  m_TotalValue -= m_Entries[id].GetAmount();

  m_Entries.erase(id);
}

//////////////////
/////////////////
////////////////

AccountTable::AccountTable(const std::string &accountName, const int &accountNumber, const bool &isCredit)
  : m_Name(accountName),
    m_Number(accountNumber),
    m_CreditAccount(isCredit),
    m_DebitTable(accountNumber, false),
    m_CreditTable(accountNumber, true) {
  ResizeTable();
}

void AccountTable::InsertDebitEntry(const Date_t &date, const int &accountID, const float &amount, bool updateOther) {
  m_DebitTable.InsertEntryData(date, accountID, amount, updateOther);
}

void AccountTable::InsertCreditEntry(const Date_t &date, const int &accountID, const float &amount, bool updateOther) {
  m_CreditTable.InsertEntryData(date, accountID, amount, updateOther);
}

void AccountTable::Draw() {
  if (m_EntryHeight <= 0) {
    ImVec2 entrySize = ImGui::CalcTextSize("##/##/####|##########|##########");
    m_EntryHeight = entrySize.y;
    m_EntryWidth = entrySize.x;

    // The initial height is just the root title, debit/credit title, and 
    // sub table title + 1 for adding entries; Notice we don't include the total
    // to this!
    m_MaxTableHeight = entrySize.y * 4.0f;
    m_MaxTableWidth = entrySize.x * 2.0f;
  }
  // Since this draws normally, all we need is the helper
  DrawHelper();
}

void AccountTable::DrawSubTable(EntryTable_t *table, const char *tableName, const int &tableIndex) {
  // Sets the table size to be equal to the size of the table minus the above 2 headers
  ImVec2 tableSize = ImVec2(m_EntryWidth, m_MaxTableHeight - (m_EntryHeight * 2));
  ImGuiTableFlags flags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersV | ImGuiTableFlags_SizingStretchProp;

  ImGui::TableSetColumnIndex(tableIndex);

  char buf[32];
  sprintf(buf, "##%s-%i", tableName, m_Number);
  if (ImGui::BeginTable(buf, 3, flags, tableSize)) {
    ImGui::TableSetupColumn("Date (MDY)", ImGuiTableColumnFlags_WidthStretch, ImGui::CalcTextSize("##/##/####").x);
    ImGui::TableSetupColumn("Account", ImGuiTableColumnFlags_WidthStretch, ImGui::CalcTextSize("Account").x);
    ImGui::TableSetupColumn("Amount", ImGuiTableColumnFlags_WidthStretch, ImGui::CalcTextSize("Amount").x);

    Utils::HeaderCentered(3);

    for (const auto &[id, entry] : table->GetEntries()) {
      ImGui::TableNextRow();
      ImGui::TableSetColumnIndex(0);
      ImGui::Text("%i/%i/%i", entry.GetDate().Month, entry.GetDate().Day, entry.GetDate().Year);

      ImGui::TableSetColumnIndex(1);
      ImGui::Text("%i", entry.GetAccountID());

      ImGui::TableSetColumnIndex(2);
      ImGui::Text("$%.2f", entry.GetAmount());
    }

    ImGui::TableNextRow();
    bool submitted = false;

    ImGui::TableSetColumnIndex(0);
    Date_t *dateBuffer = table->GetDateBuffer();
    float inputWidth = ImGui::GetColumnWidth() / 3.f;
    ImGui::PushItemWidth(inputWidth);
    ImGui::DragInt("##M", &dateBuffer->Month, 0, 0, 0, "%02d");
    if (Utils::IsSubitted()) {
      submitted = true;
    }
    ImGui::SameLine(inputWidth);
    ImGui::DragInt("##D", &dateBuffer->Day, 0, 0, 0, "%02d");
    if (Utils::IsSubitted()) {
      submitted = true;
    }
    ImGui::SameLine((inputWidth * 2));
    ImGui::DragInt("##Y", &dateBuffer->Year, 0, 0, 0, "%04d");
    if (Utils::IsSubitted()) {
      submitted = true;
    }
    ImGui::PopItemWidth();

    ImGui::TableSetColumnIndex(1);
    ImGui::SetNextItemWidth(-FLT_MIN);
    sprintf(buf, "##NewAcc%s", tableName);
    auto &tables = FerretLayer::Get()->GetTables();
    char accBuf[32] = { 0 };
    int *idBuffer = table->GetAccountIDBuffer();
    snprintf(accBuf, sizeof(accBuf), "%i", (*idBuffer) != 0 ? tables.at((*idBuffer)).GetAccountNumber() : 0);
    if (ImGui::BeginCombo(buf, accBuf)) {
      for (auto &[id, table] : tables) {
        if (id == m_Number) { // We can't add or remove money into the same account
          continue;
        }
        const bool is_selected = (*idBuffer == id);
        char buf[32] = { 0 };
        snprintf(buf, sizeof(buf), "%i", table.GetAccountNumber());
        if (ImGui::Selectable(buf, is_selected)) {
          *idBuffer = id;
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
    sprintf(buf, "##NewAmt%s", tableName);
    float *amountBuffer = table->GetAmountBuffer();
    ImGui::DragFloat(buf, amountBuffer, 0, 0, 0, "$%.2f");
    if (Utils::IsSubitted()) {
      submitted = true;
    }

    if (submitted && table->InsertEntryData(*dateBuffer, *idBuffer, *amountBuffer, true)) {
      memset(table->GetDateBuffer(), 0, sizeof(Date_t));
      memset(table->GetAccountIDBuffer(), 0, sizeof(int));
      memset(table->GetAmountBuffer(), 0, sizeof(float));
    }

    ImGui::EndTable();
  }
}

void AccountTable::DrawSubTableTotal(EntryTable_t *table, const char *tableName, const int &tableIndex) {
  ImGui::TableSetColumnIndex(tableIndex);
  ImGuiTableFlags flags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersV | ImGuiTableFlags_SizingStretchProp;

  char buf[32];
  sprintf(buf, "##%s-%i", tableName, m_Number);
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

    snprintf(buf, sizeof(buf), "$%.2f", table->GetTotalValue());
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
}

void AccountTable::DrawHelper() {
  ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(0, 0));
  ImGuiTableFlags flags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersV | ImGuiTableFlags_SizingStretchProp;

  char buf[32] = {0};
  snprintf(buf, sizeof(buf), "##%s-%i-root", m_Name.c_str(), m_Number);
  if (ImGui::BeginTable(buf, 1, flags, m_TableSize)) {

    sprintf(buf, "%s (%i)", m_Name.c_str(), m_Number);
    ImGui::TableSetupColumn(buf, ImGuiTableColumnFlags_WidthFixed, m_TableSize.x);
    Utils::HeaderCentered(1);

    ImGui::TableNextRow();

    ImGui::TableSetColumnIndex(0);

    // Current table height - one entry; to account for main table
    ImVec2 tableSize = ImVec2(m_TableSize.x, m_TableSize.y - g_EntrySize.y);

    snprintf(buf, sizeof(buf), "##%s-%i-entries", m_Name.c_str(), m_Number);
    if (ImGui::BeginTable(buf, 2, flags, tableSize)) {
      ImGui::TableSetupColumn("Debit", ImGuiTableColumnFlags_WidthFixed, tableSize.x / 2.0);
      ImGui::TableSetupColumn("Credit", ImGuiTableColumnFlags_WidthFixed, tableSize.x / 2.0);

      Utils::HeaderCentered(2);

      ImGui::TableNextRow();

      DrawSubTable(&m_DebitTable, "Debit", 0);
      DrawSubTable(&m_CreditTable, "Credit", 1);

      ImGui::TableNextRow();

      DrawSubTableTotal(&m_DebitTable, "Debit", 0);
      DrawSubTableTotal(&m_CreditTable, "Credit", 1);

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

void AccountTable::ResizeTable() {
  float creditTableSize = m_CreditTable.GetEntries().size();
  float debitTableSize = m_DebitTable.GetEntries().size();
  m_TableSize = ImVec2(
    g_EntrySize.x * 2.0f,
    creditTableSize > debitTableSize
    ? g_EntrySize.y * (creditTableSize + 4)
    : g_EntrySize.y * (debitTableSize + 4)
  );
}

}
