#include "Journal.h"

#include "FerretApp/Layer/FerretLayer.h"

#include "FerretApp/Utils/Utils.h"

#include "FerretApp/Popup/Popup.h"

extern ImVec2 g_EntrySize;

namespace Ferret {

void Journal::OnRenderData() {
  for (auto &[id, table] : m_JournalEntryMap) {
    OnRenderJournalDateTable(table);
  }
}

void Journal::NewDataAvailable(const Date &date, const int &tableID) {
  int dateID = date.GetDateID();
  if (m_JournalEntryMap.find(dateID) == m_JournalEntryMap.end()) { // Date doesn't exist, create it
    m_JournalEntryMap.emplace(std::pair<int, JournalDateTable>(dateID, JournalDateTable{}));
    m_JournalEntryMap.at(dateID).DateString = fmt::format("{}/{}/{}", date.Month, date.Day, date.Year);
  }

  auto *dateTableEntryMap = &m_JournalEntryMap.at(dateID).Entries;
  if ((*dateTableEntryMap).find(tableID) == (*dateTableEntryMap).end()) { // Table doesn't exist in DateTable Entries, create it
    (*dateTableEntryMap).emplace(std::pair<int, JournalEntry>(tableID, JournalEntry{.CreditAccountID=tableID}));
  }

  auto *entries = &(*dateTableEntryMap).at(tableID).DebitEntryIDs;
  (*entries).clear();

  const AccountTable &table = FerretLayer::Get().GetLedger().GetTables().at(tableID);
  const EntryTable *creditEntryTable = table.GetCreditTable();

  for (auto &[id, entry] : (*creditEntryTable).GetEntries()) {
    if (entry.GetDate().GetDateID() != dateID) { // If not the same date
      continue;
    }

    (*entries).push_back(id); // track the entry id for future use
  }
}

void Journal::OnRenderJournalDateTable(const JournalDateTable &table) {
  ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(0, 0));
  ImGuiTableFlags flags = ImGuiTableFlags_Borders | ImGuiTableFlags_BordersV | ImGuiTableFlags_SizingStretchProp;
  if (ImGui::BeginTable(table.DateString.c_str(), 1, flags)) {
    ImGui::TableSetupColumn(table.DateString.c_str());
    Utils::HeaderCentered(1);

    ImGui::TableNextRow();
    ImGui::TableSetColumnIndex(0);

    float availableSpace = ImGui::GetContentRegionAvail().x;
    std::string subTableName = table.DateString + "Entries";
    if (ImGui::BeginTable(subTableName.c_str(), 4, flags)) {
      ImGui::TableSetupColumn("Account", ImGuiTableColumnFlags_WidthFixed, availableSpace * 0.25f); // The account name will only be a tenth of the window space
      ImGui::TableSetupColumn("Description", ImGuiTableColumnFlags_WidthFixed, availableSpace * 0.55f); // The decription will only be half of the window space
      ImGui::TableSetupColumn("Debit", ImGuiTableColumnFlags_WidthFixed, availableSpace * 0.1f); // The decription will only be 20% of the window space
      ImGui::TableSetupColumn("Credit", ImGuiTableColumnFlags_WidthFixed, availableSpace * 0.1f); // The decription will only be 20% of the window space
      Utils::HeaderCentered(4);

      ImGui::TableNextRow();

      for (auto &[id, entry] : table.Entries) {
        OnRenderJournalEntry(entry);
      }

      ImGui::EndTable();
    }

    ImGui::EndTable();
  }
  ImGui::PopStyleVar();
}

void Journal::OnRenderJournalEntry(const JournalEntry &entry) {
  float total = 0.0f;
  for (auto &id : entry.DebitEntryIDs) {
    // Long and convoluted line to get the entry in the credit table with the stored id
    const Entry &debitEntry = FerretLayer::Get().GetLedger().GetTables().at(entry.CreditAccountID).GetCreditTable()->GetEntries().at(id);
    const std::string &otherTableName = debitEntry.GetAccountID() != FerretLayer::Get().GetLedger().GetRetainedEarningsID() ?
                                        FerretLayer::Get().GetLedger().GetTables().at(debitEntry.GetAccountID()).GetName() : "Retained";
    ImGui::TableSetColumnIndex(0);
    ImGui::Text("%s (%i)", otherTableName.c_str(), debitEntry.GetAccountID());
    if (ImGui::IsItemHovered()) {
      ImGui::SetTooltip("Double Click to Edit Entry");

      if (ImGui::IsMouseDoubleClicked(0)) {
        Popup::ViewEntry(entry.CreditAccountID, id, true);
      }
    }

    ImGui::TableSetColumnIndex(1);
    ImGui::TextUnformatted(debitEntry.GetJournalEntry().c_str());
    if (ImGui::IsItemHovered()) {
      ImGui::SetTooltip("Double Click to Edit Entry");

      if (ImGui::IsMouseDoubleClicked(0)) {
        Popup::ViewEntry(entry.CreditAccountID, id, true);
      }
    }

    float entryAmount = debitEntry.GetAmount();
    ImGui::TableSetColumnIndex(2);
    ImGui::Text("%.2f", entryAmount);
    if (ImGui::IsItemHovered()) {
      ImGui::SetTooltip("Double Click to Edit Entry");

      if (ImGui::IsMouseDoubleClicked(0)) {
        Popup::ViewEntry(entry.CreditAccountID, id, true);
      }
    }

    total+=entryAmount;

    ImGui::TableNextRow();
  }

  const std::string &tableName = FerretLayer::Get().GetLedger().GetTables().at(entry.CreditAccountID).GetName();
  ImGui::TableSetColumnIndex(0);
  ImGui::Text("    %s (%i)", tableName.c_str(), entry.CreditAccountID);

  ImGui::TableSetColumnIndex(3);
  ImGui::Text("%.2f", total);

  ImGui::TableNextRow();

  ImGui::TableSetColumnIndex(0);
  ImGui::Dummy(ImVec2(ImGui::GetColumnWidth(), g_EntrySize.y));

  ImGui::TableNextRow();
}

}
