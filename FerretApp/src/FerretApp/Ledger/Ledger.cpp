#include "Ledger.h"

#include "Ferret/Core/Application.h"
#include "FerretApp/Layer/FerretLayer.h"

#include "Table.h"
#include "EntryTable.h"
#include "Entry.h"

#include "FerretApp/Utils/Utils.h"

#include <imgui.h>

extern ImVec2 g_EntrySize;
extern ImVec2 g_GenericTableSize;

namespace Ferret {

Ledger::Ledger() {
  s_Instance = this;
}

Ledger::~Ledger() {
  s_Instance = nullptr;
}

void Ledger::OnRender() {
if (m_Tables.empty()) {
    if (ImGui::Button("Add Table", g_GenericTableSize))
      m_RenderPopup = RenderPopup::CreateTable;

    if (ImGui::IsItemHovered())
      ImGui::SetTooltip("Used to create a new table either in the row or in the first column");
  }

  bool shouldNewLine = false;
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
      Utils::GetPositiveDigitCount(id) != Utils::GetPositiveDigitCount(table.GetNext()->GetAccountNumber())
      || Utils::GetTopDigit(id) != Utils::GetTopDigit(table.GetNext()->GetAccountNumber()))) {

      if (g_GenericTableSize.x * (rowTableCount + 1) > ImGui::GetMainViewport()->Size.x) {
        rowTableCount = 0;
      } else {
        ImGui::SameLine();
      }

      if (ImGui::Button("Add Table", g_GenericTableSize))
        m_RenderPopup = RenderPopup::CreateTable;

      if (ImGui::IsItemHovered())
        ImGui::SetTooltip("Used to create a new table either in the row or in the first column");
      shouldNewLine = true;
    }

    ImGui::PopID();
  }
  ImGui::End();

  switch (m_RenderPopup) {
    case RenderPopup::CreateTable: {
      RenderCreateTablePopup();
      break;
    }
    case RenderPopup::SaveAndExit:
    case RenderPopup::SaveAndOpenExistingTables: {
      RenderSavePopup();
      break;
    }
    case RenderPopup::EntryDetails: {
      RenderEntryDetailsPopup();
      break;
    }
    default: break;
  }
}

void Ledger::SubmitEntryDataToTable(const int &toTable, const bool &isCredit, const Date &date, const int &fromTable, const float &amount) {
  auto &table = m_Tables.at(toTable);
  table.InsertEntry(isCredit, date, fromTable, amount, true);
}

void Ledger::RemoveTable(const int &tableID) {
  Application::Get().SubmitToMainThread([this, tableID](){
    for (auto &[id, table] : m_Tables) {
      if (id == tableID) continue;
      table.RemoveEntriesFromTable(tableID);
    }

    m_Tables.erase(tableID);
    ReloadTables();
    m_ContextDirty = true;
  });
}

void Ledger::ViewEntry(const int &tableID, const bool &isCredit, const int &entryID) {
  m_ViewingTableID = tableID;
  m_ViewingEntryID = entryID;
  m_IsViewingCreditEntry = isCredit;

  m_RenderPopup = RenderPopup::EntryDetails;
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

void Ledger::RenderCreateTablePopup() {
  ImGuiViewport *viewport = ImGui::GetMainViewport();
  ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize;
  // Sets the window size to be a third of the application window size
  float windowSizeX = viewport->Size.x / 3.0f;
  float windowSizeY = viewport->Size.y / 3.0f;

  // Puts to window in the center of the application window
  float windowPosX = (viewport->Size.x - windowSizeX) / 2.0f;
  float windowPosY = (viewport->Size.y - windowSizeY) / 2.0f;

  ImGui::SetNextWindowSize(ImVec2(windowSizeX, windowSizeY)); 
  ImGui::SetNextWindowPos(ImVec2(windowPosX, windowPosY));
  ImGui::Begin("Create New Table", nullptr, flags);

  static char accountName[32] = {0};
  static int accountNumber = 0;
  static bool isCredit = false;

  ImGui::Text("Account Name");
  ImGui::SameLine();
  ImGui::InputText("##accName", accountName, sizeof(accountName));

  ImGui::Text("Account Number");
  ImGui::SameLine();
  ImGui::DragInt("##accNum", &accountNumber, 0, 0, 0, "%03d");

  ImGui::Text("Is Credit Account");
  ImGui::SameLine();
  ImGui::Checkbox("##isCredit", &isCredit);

  bool disabled = false;

  if (m_Tables.find(accountNumber) != m_Tables.end() || accountNumber == 0) { // account already exists
    ImGui::PushStyleColor(ImGuiCol_Button,        ImVec4(1.0f, 0.0f, 0.0f, 1.0f)); // Pure Red (Normal)
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 0.3f, 0.3f, 1.0f)); // Lighter Red (Hovered)
    ImGui::PushStyleColor(ImGuiCol_ButtonActive,  ImVec4(0.8f, 0.0f, 0.0f, 1.0f)); // Darker Red (Clicked)
    disabled = true;
  }

  ImGui::BeginDisabled(disabled);

  if (ImGui::Button("Confirm")) {
    AccountTable table(accountName, accountNumber, isCredit);
    m_Tables.emplace(std::pair<int, AccountTable>(accountNumber, table));
    ReloadTables();
    memset(accountName, 0, sizeof(accountName));
    memset(&accountNumber, 0, sizeof(int));
    memset(&isCredit, 0, sizeof(bool));
    m_ContextDirty = true;
    m_RenderPopup = RenderPopup::NONE;
  }

  ImGui::EndDisabled();

  ImGui::SameLine();

  if (ImGui::Button("Cancel")) {
    memset(accountName, 0, sizeof(accountName));
    memset(&accountNumber, 0, sizeof(int));
    memset(&isCredit, 0, sizeof(bool));
    m_RenderPopup = RenderPopup::NONE;
  }

  if (disabled)
    ImGui::PopStyleColor(3);

  ImGui::End();
}

void Ledger::RenderSavePopup() {
  ImGuiViewport *viewport = ImGui::GetMainViewport();
  ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize;
  // Sets the window size to be a third of the application window size
  float windowSizeX = viewport->Size.x / 3.0f;
  float windowSizeY = viewport->Size.y / 3.0f;

  // Puts to window in the center of the application window
  float windowPosX = (viewport->Size.x - windowSizeX) / 2.0f;
  float windowPosY = (viewport->Size.y - windowSizeY) / 2.0f;

  ImGui::SetNextWindowSize(ImVec2(windowSizeX, windowSizeY)); 
  ImGui::SetNextWindowPos(ImVec2(windowPosX, windowPosY));
  ImGui::Begin("Context Dirty!", nullptr, flags);

  ImGui::Text("Save?");
  ImGui::SameLine();

  if (ImGui::Button("Yes")) {
    if (!FerretLayer::Get().IsSavePathValid()) {
      FerretLayer::Get().SaveAs();
    } else {
      FerretLayer::Get().Save();
    }

    RenderPopup tmp = m_RenderPopup;
    m_RenderPopup = RenderPopup::NONE;

    if (tmp == RenderPopup::SaveAndExit)
      Application::Get().OnApplicationExit();

    if (tmp == RenderPopup::SaveAndOpenExistingTables)
      FerretLayer::Get().OpenAtTmpPath();

  }

  ImGui::SameLine();

  if (ImGui::Button("No")) {
    RenderPopup tmp = m_RenderPopup;
    m_RenderPopup = RenderPopup::NONE;

    if (tmp == RenderPopup::SaveAndExit)
      Application::Get().OnApplicationExit();

    if (tmp == RenderPopup::SaveAndOpenExistingTables)
      FerretLayer::Get().OpenAtTmpPath();
  }

  ImGui::SameLine();

  if (ImGui::Button("Cancel")) {
    m_RenderPopup = RenderPopup::NONE;
  }

  ImGui::End();
}

void Ledger::RenderEntryDetailsPopup() {
  ImGuiViewport *viewport = ImGui::GetMainViewport();
  ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize;
  // Sets the window size to be a third of the application window size
  float windowSizeX = viewport->Size.x / 3.0f;
  float windowSizeY = viewport->Size.y / 3.0f;

  // Puts to window in the center of the application window
  float windowPosX = (viewport->Size.x - windowSizeX) / 2.0f;
  float windowPosY = (viewport->Size.y - windowSizeY) / 2.0f;

  ImGui::SetNextWindowSize(ImVec2(windowSizeX, windowSizeY)); 
  ImGui::SetNextWindowPos(ImVec2(windowPosX, windowPosY));
  ImGui::Begin("Entry Details", nullptr, flags);

  static bool renderingCombo = false;

  if (!renderingCombo) {
    ImGui::SetWindowFocus();
  }

  AccountTable &table = m_Tables.at(m_ViewingTableID);
  Entry entryData = table.GetEntry(m_IsViewingCreditEntry, m_ViewingEntryID);

  static bool editingEntry = false;

  static Date date = entryData.GetDate();
  static int accountID = entryData.GetAccountID();
  static float amount = entryData.GetAmount();

  if (accountID == 0) { // Memset will make these 0, since they are static we need to reset it
    date = entryData.GetDate();
    accountID = entryData.GetAccountID();
    amount = entryData.GetAmount();
  }

  if (!editingEntry) {
    if (ImGui::Button("Delete")) {
      m_RenderPopup = RenderPopup::NONE;

      AccountTable *otherTable = &m_Tables.at(entryData.GetAccountID());
      int otherEntryID = entryData.GetDate().GetDateID() + m_ViewingTableID;
      otherTable->RemoveEntry(!m_IsViewingCreditEntry, otherEntryID);
      table.RemoveEntry(m_IsViewingCreditEntry, m_ViewingEntryID);

      memset(&date, 0, sizeof(Date));
      memset(&accountID, 0, sizeof(int));
      memset(&amount, 0, sizeof(float));
    }
    if (ImGui::IsItemHovered()) {
      ImGui::SetTooltip("WARNING - Pressing this will delete this entry!");
    }
    ImGui::SameLine();
  }


  ImGui::SetCursorPosX((windowSizeX - g_EntrySize.x) / 2.0);
  ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(0, 0));
  ImGuiTableFlags tflags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersV | ImGuiTableFlags_SizingStretchProp;
  if (ImGui::BeginTable("##Viewing-Table-Entry", 1, tflags, ImVec2(g_EntrySize.x, g_EntrySize.y * 4.0))) {
    char buf[32] = {0};
    sprintf(buf, "%s (%i)", table.GetName().c_str(), table.GetAccountNumber());
    ImGui::TableSetupColumn(buf, ImGuiTableColumnFlags_WidthFixed, g_EntrySize.x);
    Utils::HeaderCentered(1);

    ImGui::TableNextRow();
    ImGui::TableSetColumnIndex(0);
    std::string entryTableName = m_IsViewingCreditEntry ? "Credit" : "Debit";
    ImGui::SetCursorPosX(((ImGui::GetColumnWidth() - ImGui::CalcTextSize(entryTableName.c_str()).x) / 2.0f) + ImGui::GetCursorPosX());
    ImGui::TextUnformatted(entryTableName.c_str());
    
    ImGui::TableNextRow();
    ImGui::TableSetColumnIndex(0);
    if (ImGui::BeginTable("##EntryInfoHeader", 3, tflags, ImVec2(g_EntrySize.x, g_EntrySize.y * 2))) {
      ImGui::TableSetupColumn("Date (MDY)", ImGuiTableColumnFlags_WidthStretch, ImGui::CalcTextSize("##/##/####").x);
      ImGui::TableSetupColumn("Account", ImGuiTableColumnFlags_WidthStretch, ImGui::CalcTextSize("Account").x);
      ImGui::TableSetupColumn("Amount", ImGuiTableColumnFlags_WidthStretch, ImGui::CalcTextSize("Amount").x);

      Utils::HeaderCentered(3);

      ImGui::TableNextRow();

      if (!editingEntry) {
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("%i/%i/%i", entryData.GetDate().Month, entryData.GetDate().Day, entryData.GetDate().Year);

        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%i", entryData.GetAccountID());

        ImGui::TableSetColumnIndex(2);
        ImGui::Text("$%.2f", entryData.GetAmount());
      } else {
        ImGui::TableSetColumnIndex(0);
        float inputWidth = ImGui::GetColumnWidth() / 3.f;
        ImGui::PushItemWidth(inputWidth);
        ImGui::DragInt("##M", &date.Month, 0, 0, 0, "%02d");

        ImGui::SameLine(inputWidth);
        ImGui::DragInt("##D", &date.Day, 0, 0, 0, "%02d");

        ImGui::SameLine((inputWidth * 2));
        ImGui::DragInt("##Y", &date.Year, 0, 0, 0, "%04d");

        ImGui::PopItemWidth();

        ImGui::TableSetColumnIndex(1);
        ImGui::SetNextItemWidth(-FLT_MIN);
        char accBuf[32] = {0};
        snprintf(accBuf, sizeof(accBuf), "%i", accountID != 0 ? m_Tables.at(accountID).GetAccountNumber() : 0);
        if (ImGui::BeginCombo("##EditAccountID", accBuf)) {
          renderingCombo = true;
          for (auto &[id, table] : m_Tables) {
            if (id == m_ViewingTableID) { // We can't add or remove money into the same account
              continue;
            }

            const bool is_selected = (accountID == id);
            char buf[32] = { 0 }; 
            snprintf(buf, sizeof(buf), "%i", table.GetAccountNumber());
            if (ImGui::Selectable(buf, is_selected)) {
              accountID = id;
            }

            // Set the initial focus when opening the combo (keyboard navigation)
            if (is_selected) {
              ImGui::SetItemDefaultFocus();
            }
          }
          ImGui::EndCombo();
        } else {
          renderingCombo = false;
        }

        ImGui::TableSetColumnIndex(2);
        ImGui::SetNextItemWidth(-FLT_MIN);
        ImGui::DragFloat("##EditAmount", &amount, 0, 0, 0, "$%.2f");
      }

      ImGui::EndTable();
    }
    ImGui::EndTable();
  }
  ImGui::PopStyleVar();

  ImGui::SetCursorPosX(0);

  int month = 0, day = 0, year = 0, hour = 0, minute = 0;
  ImGui::Text("Created: %02d/%02d/%04d - %02d:%02d", month, day, year, hour, minute);
  ImGui::Text("Last Updated: %02d/%02d/%04d - %02d:%02d", month, day, year, hour, minute);

  ImGui::Text("Journal Entry");
  ImGui::Text("    %s", "JOURNAL ENTRY DATA");

  if (!editingEntry) {
    if (ImGui::Button("Edit")) {
      editingEntry = true;
    }
    ImGui::SameLine();
    if (ImGui::Button("Close")) {
      m_RenderPopup = RenderPopup::NONE;
      memset(&date, 0, sizeof(Date));
      memset(&accountID, 0, sizeof(int));
      memset(&amount, 0, sizeof(float));
    }

  } else {
    if (ImGui::Button("Confirm")) {
      AccountTable *otherTable = &m_Tables.at(entryData.GetAccountID());
      int otherEntryID = entryData.GetDate().GetDateID() + m_ViewingTableID;
      otherTable->RemoveEntry(!m_IsViewingCreditEntry, otherEntryID);

      table.RemoveEntry(m_IsViewingCreditEntry, m_ViewingEntryID);
      table.InsertEntry(m_IsViewingCreditEntry, date, accountID, amount, true);

      memset(&date, 0, sizeof(Date));
      memset(&accountID, 0, sizeof(int));
      memset(&amount, 0, sizeof(float));

      editingEntry = false;
      m_RenderPopup = RenderPopup::NONE;
      m_ContextDirty = true;
    }
    ImGui::SameLine();
    if (ImGui::Button("Cancel")) {
      editingEntry = false;
      memset(&date, 0, sizeof(Date));
      memset(&accountID, 0, sizeof(int));
      memset(&amount, 0, sizeof(float));
    }

  }

  ImGui::End();
}


}
