#include "Ledger.h"

#include "Ferret/Core/Application.h"
#include "FerretApp/Layer/FerretLayer.h"

#include "Table.h"
#include "Entry.h"

#include "FerretApp/Utils/Utils.h"

#include "FerretApp/Popup/Popup.h"

#include <imgui.h>

extern ImVec2 g_EntrySize;
extern ImVec2 g_GenericTableSize;

namespace Ferret {

void Ledger::OnRenderData() {
  if (m_Tables.empty()) {
    if (ImGui::Button("Add Table", g_GenericTableSize))
      Popup::SetRenderPopup(PopupType::CreateTable);

    if (ImGui::IsItemHovered())
      ImGui::SetTooltip("Used to create a new table either in the row or in the first column");
  }

  bool shouldNewLine = true;
  int rowTableCount = 0;
  for (auto &[id, table] : m_Tables) {
    ImGui::PushID(id);
    if (shouldNewLine || (g_GenericTableSize.x * (rowTableCount + 1) > ImGui::GetMainViewport()->Size.x)) {
      rowTableCount = 0;
      shouldNewLine = false;
    } else {
      ImGui::SameLine();
    }

    table.Draw();
    rowTableCount++;

    if (!table.GetNext() || (
      Utils::GetDigitCount(id) != Utils::GetDigitCount(table.GetNext()->GetAccountNumber())
      || Utils::GetTopDigit(id) != Utils::GetTopDigit(table.GetNext()->GetAccountNumber()))) {

      if (g_GenericTableSize.x * (rowTableCount + 1) > ImGui::GetMainViewport()->Size.x) {
        rowTableCount = 0;
      } else {
        ImGui::SameLine();
      }

      if (ImGui::Button("Add Table", g_GenericTableSize))
        Popup::SetRenderPopup(PopupType::CreateTable);

      if (ImGui::IsItemHovered())
        ImGui::SetTooltip("Used to create a new table either in the row or in the first column");
      shouldNewLine = true;
    }

    ImGui::PopID();
  }
}

void Ledger::SubmitEntryDataToTable(const int &toTable, const bool &isCredit, const Date &date, const int &fromTable, const float &amount, const std::string &journalEntry) {
  auto &table = m_Tables.at(toTable);
  table.InsertEntry(isCredit, date, fromTable, amount, true, journalEntry);
}

void Ledger::RemoveTable(const int &tableID) {
  Application::Get().SubmitToMainThread([this, tableID](){
    for (auto &[id, table] : m_Tables) {
      if (id == tableID) continue;
      table.RemoveEntriesFromTable(tableID);
    }

    m_Tables.erase(tableID);
    ReloadTables();
    FerretLayer::Get().SetContextDirty(true);
  });
}

void Ledger::CreateTable(const int &tableID, const std::string &name, const bool &isCredit, const TableTracking &tracking) {
  AccountTable table(name, tableID, isCredit, tracking);
  m_Tables.emplace(std::pair<int, AccountTable>(tableID, table));
  if (tracking == TableTracking::Income || tracking == TableTracking::Expenses) {
    Application::Get().SubmitToMainThread([](){FerretLayer::Get().GetStatements().NewDataAvailable();});
  }
  ReloadTables();
}

void Ledger::ReloadTables() {
  m_TableNames.clear();
  int prevID = -1;
  for (auto &[id, table] : m_Tables) {
    table.SetNext(nullptr);
    if (prevID != -1) {
      m_Tables.at(prevID).SetNext(&table);
    }
    prevID = id;
  }
}

}
