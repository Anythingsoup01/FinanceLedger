#include "Popup.h"

#include "Ferret/Core/Application.h"

#include "FerretApp/Layer/FerretLayer.h"
#include "FerretApp/Ledger/Ledger.h"

#include "FerretApp/Utils/Utils.h"

#include <imgui.h>

extern ImVec2 g_EntrySize;
extern ImVec2 g_GenericTableSize;

namespace Ferret {

void Popup::Render() {
  switch (m_PopupType) {
    case PopupType::CreateTable: {
      RenderCreateTablePopup();
      break;
    }
    case PopupType::SaveAndExit:
    case PopupType::SaveAndOpenExistingTables: {
      RenderSavePopup();
      break;
    }
    case PopupType::EntryDetails: {
      RenderEntryDetailsPopup();
      break;
    }
    case PopupType::TableDetails: {
      RenderTableDetailsPopup();
      break;
    }
    case PopupType::StatementTableDetails: {
      // This is so we only ever render one popup, rather than three
      if (m_ViewingStatementDataSetID != 0) {
        RenderStatementTableDataSetDetailsPopup();
      } else {
        RenderStatementTableDetailsStackPopup();
      }
      break;
    }
    default: break;
  }
}

void Popup::ViewEntry(const int &tableID, const int &entryID, const bool &isEntryCredit) {
  m_ViewingTableID = tableID;
  m_ViewingEntryID = entryID;
  m_IsViewingCreditEntry = isEntryCredit;

  m_PopupType = PopupType::EntryDetails;
}

void Popup::ViewTable(const int &tableID) {
  m_ViewingTableID = tableID;
  m_PopupType = PopupType::TableDetails;
}

void Popup::ViewStatementTable(const uint64_t &tableHash) {
  m_ViewingStatementTableStack.push_back(tableHash);
  m_PopupType = PopupType::StatementTableDetails;
}

void Popup::RenderCreateTablePopup() {
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
  static TableTracking tracking = TableTracking::Untracked;

  static bool renderingCombo = false;

  if (!renderingCombo) {
    ImGui::SetWindowFocus();
  }

  ImGui::Text("Account Name");
  ImGui::SameLine();
  ImGui::InputText("##accName", accountName, sizeof(accountName));

  ImGui::Text("Account Number");
  ImGui::SameLine();
  ImGui::DragInt("##accNum", &accountNumber, 0, 0, 0, "%03d");

  ImGui::Text("Is Credit Account");
  ImGui::SameLine();
  ImGui::Checkbox("##isCredit", &isCredit);

  ImGui::Text("Account Tracking");
  ImGui::SameLine();
  if (ImGui::BeginCombo("##tracking", TableTrackingToString(tracking).c_str())) {
    renderingCombo = true;
    for (int i = 0; i < (int)TableTracking::MAX_ITEM; i++) {
      TableTracking track = (TableTracking)i;
      const bool is_selected = (tracking == track);
      if (ImGui::Selectable(TableTrackingToString(track).c_str(), is_selected)) {
        tracking = track;
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

  bool disabled = false;

  if (FerretLayer::Get().GetLedger().GetTables().find(accountNumber) != FerretLayer::Get().GetLedger().GetTables().end() || accountNumber == 0) { // account already exists
    ImGui::PushStyleColor(ImGuiCol_Button,        ImVec4(1.0f, 0.0f, 0.0f, 1.0f)); // Pure Red (Normal)
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 0.3f, 0.3f, 1.0f)); // Lighter Red (Hovered)
    ImGui::PushStyleColor(ImGuiCol_ButtonActive,  ImVec4(0.8f, 0.0f, 0.0f, 1.0f)); // Darker Red (Clicked)
    disabled = true;
  }

  ImGui::BeginDisabled(disabled);

  if (ImGui::Button("Confirm")) {
    FerretLayer::Get().GetLedger().CreateTable(accountNumber, accountName, isCredit, tracking);

    switch (tracking) {
      case TableTracking::Cash:
      case TableTracking::OtherAsset: {
        Application::Get().SubmitToMainThread([](){FerretLayer::Get().GetStatements().NewCashDataAvailable();});
        break;
      }
      case TableTracking::ShortTermLiabilities:
      case TableTracking::LongTermLiabilities: {
        Application::Get().SubmitToMainThread([](){FerretLayer::Get().GetStatements().NewLiabilityDataAvailable();});
        break;
      }
      case TableTracking::Income:
      case TableTracking::Expenses: {
        Application::Get().SubmitToMainThread([](){FerretLayer::Get().GetStatements().NewExpenseOrIncomeDataAvailable();});
        break;
      }
      default: break;
    }

    memset(accountName, 0, sizeof(accountName));
    memset(&accountNumber, 0, sizeof(int));
    memset(&isCredit, 0, sizeof(bool));
    memset(&tracking, 0, sizeof(TableTracking));
    FerretLayer::Get().SetContextDirty(false);
    m_PopupType = PopupType::NONE;
  }

  ImGui::EndDisabled();

  if (disabled)
    ImGui::PopStyleColor(3);

  ImGui::SameLine();

  if (ImGui::Button("Cancel")) {
    memset(accountName, 0, sizeof(accountName));
    memset(&accountNumber, 0, sizeof(int));
    memset(&isCredit, 0, sizeof(bool));
    m_PopupType = PopupType::NONE;
  }
  ImGui::End();
}

void Popup::RenderSavePopup() {
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

  ImGui::SetWindowFocus();

  ImGui::Text("Save?");
  ImGui::SameLine();

  if (ImGui::Button("Yes")) {
    if (!FerretLayer::Get().IsSavePathValid()) {
      FerretLayer::Get().SaveAs();
    } else {
      FerretLayer::Get().Save();
    }

    if (m_PopupType == PopupType::SaveAndExit)
      Application::Get().OnApplicationExit();

    if (m_PopupType == PopupType::SaveAndOpenExistingTables)
      FerretLayer::Get().OpenAtTmpPath();

    m_PopupType = PopupType::NONE;
  }

  ImGui::SameLine();

  if (ImGui::Button("No")) {

    if (m_PopupType == PopupType::SaveAndExit)
      Application::Get().OnApplicationExit();

    if (m_PopupType == PopupType::SaveAndOpenExistingTables)
      FerretLayer::Get().OpenAtTmpPath();

    m_PopupType = PopupType::NONE;
  }

  ImGui::SameLine();

  if (ImGui::Button("Cancel")) {
    m_PopupType = PopupType::NONE;
  }

  ImGui::End();
}

void Popup::RenderEntryDetailsPopup() {
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

  AccountTable &table = FerretLayer::Get().GetLedger().GetTables().at(m_ViewingTableID);
  Entry entryData = table.GetEntry(m_IsViewingCreditEntry, m_ViewingEntryID);

  static bool editingEntry = false;

  static Date date;
  static int accountID = 0;
  static float amount = 0;
  static char journalEntry[256] = {0};

  if (accountID == 0) { // Memset will make these 0, since they are static we need to reset it
    date = entryData.GetDate();
    accountID = entryData.GetAccountID();
    amount = entryData.GetAmount();
    snprintf(journalEntry, sizeof(journalEntry), "%s", entryData.GetJournalEntry().c_str());
  }

  if (!editingEntry) {
    if (ImGui::Button("Delete")) {
      m_PopupType = PopupType::NONE;

      if (entryData.GetAccountID() != FerretLayer::Get().GetLedger().GetRetainedEarningsID()) {
        AccountTable *otherTable = &FerretLayer::Get().GetLedger().GetTables().at(entryData.GetAccountID());
        int otherEntryID = Utils::CalculateEntryID(entryData.GetDate(), m_ViewingTableID);
        otherTable->RemoveEntry(!m_IsViewingCreditEntry, otherEntryID);
        table.RemoveEntry(m_IsViewingCreditEntry, m_ViewingEntryID);

        if (table.GetTracking() == TableTracking::Expenses || table.GetTracking() == TableTracking::Income
          || otherTable->GetTracking() == TableTracking::Expenses || otherTable->GetTracking() == TableTracking::Income) {
          FerretLayer::Get().GetStatements().NewExpenseOrIncomeDataAvailable();
        }
      } else {
        if (table.GetTracking() == TableTracking::Expenses || table.GetTracking() == TableTracking::Income) {
          FerretLayer::Get().GetStatements().UpdateBeginningBalance(entryData.GetAmount());
        }
        table.RemoveEntry(m_IsViewingCreditEntry, m_ViewingEntryID);
      }

      memset(&date, 0, sizeof(Date));
      memset(&accountID, 0, sizeof(int));
      memset(&amount, 0, sizeof(float));
      memset(&journalEntry, 0, sizeof(journalEntry));

      FerretLayer::Get().SetContextDirty(true);
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

      static int retainedEarningsTableID = FerretLayer::Get().GetLedger().GetRetainedEarningsID();

      if (!editingEntry) {
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("%i/%i/%i", entryData.GetDate().Month, entryData.GetDate().Day, entryData.GetDate().Year);

        ImGui::TableSetColumnIndex(1);
        if (entryData.GetAccountID() == retainedEarningsTableID) {
          ImGui::Text("Retained");
        } else {
          ImGui::Text("%i", entryData.GetAccountID());
        }

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
        if (accountID == retainedEarningsTableID) {
          snprintf(accBuf, sizeof(accBuf), "Retained");
        } else {
          snprintf(accBuf, sizeof(accBuf), "%i", accountID != 0 ? accountID : 0);
        }
        if (ImGui::BeginCombo("##EditAccountID", accBuf)) {
          renderingCombo = true;
          if (accountID != retainedEarningsTableID) {
            const bool is_selected = (accountID == retainedEarningsTableID);
            if (ImGui::Selectable("Retained", is_selected)) {
              accountID = retainedEarningsTableID;
            }

            if (is_selected)
              ImGui::SetItemDefaultFocus();
          }
          for (auto &[id, table] : FerretLayer::Get().GetLedger().GetTables()) {
            if (id == m_ViewingTableID) { // We can't add or remove money into the same account
              continue;
            }

            const bool is_selected = (accountID == id);
            char buf[32] = { 0 }; 
            snprintf(buf, sizeof(buf), "%i", id);
            
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
  if (!editingEntry) {
    ImGui::Text("    %s", journalEntry);
  } else {
    ImGui::InputText("##JournalEntryEdit", journalEntry, sizeof(journalEntry));
  }

  if (!editingEntry) {
    if (ImGui::Button("Edit")) {
      editingEntry = true;
    }
    ImGui::SameLine();
    if (ImGui::Button("Close")) {
      m_PopupType = PopupType::NONE;
      memset(&date, 0, sizeof(Date));
      memset(&accountID, 0, sizeof(int));
      memset(&amount, 0, sizeof(float));
      memset(&journalEntry, 0, sizeof(journalEntry));
    }

  } else {
    if (ImGui::Button("Confirm")) {
      if (entryData.GetAccountID() != FerretLayer::Get().GetLedger().GetRetainedEarningsID()) {
        AccountTable *otherTable = &FerretLayer::Get().GetLedger().GetTables().at(entryData.GetAccountID());
        int otherEntryID = Utils::CalculateEntryID(entryData.GetDate(), m_ViewingTableID);
        otherTable->RemoveEntry(!m_IsViewingCreditEntry, otherEntryID);
      }

      table.RemoveEntry(m_IsViewingCreditEntry, m_ViewingEntryID);
      table.InsertEntry(m_IsViewingCreditEntry, date, accountID, amount, accountID != FerretLayer::Get().GetLedger().GetRetainedEarningsID(), journalEntry);

      memset(&date, 0, sizeof(Date));
      memset(&accountID, 0, sizeof(int));
      memset(&amount, 0, sizeof(float));
      memset(&journalEntry, 0, sizeof(journalEntry));

      editingEntry = false;
      m_PopupType = PopupType::NONE;
      FerretLayer::Get().SetContextDirty(true);
    }
    ImGui::SameLine();
    if (ImGui::Button("Cancel")) {
      editingEntry = false;
      memset(&date, 0, sizeof(Date));
      memset(&accountID, 0, sizeof(int));
      memset(&amount, 0, sizeof(float));
      memset(&journalEntry, 0, sizeof(journalEntry));
    }

  }

  ImGui::End();
}

void Popup::LoadEntriesToTable(AccountTable *toTable, const EntryTable &fromEntryTable) {
  for (auto &[id, entry] : fromEntryTable.GetEntries()) {
    toTable->InsertEntry(fromEntryTable.IsCreditAccount(), entry.GetDate(), entry.GetAccountID(), entry.GetAmount(), true);
  }
}

void Popup::RenderTableDetailsPopup() {
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
  ImGui::Begin("Table Details", nullptr, flags);

  static bool renderingCombo = false;

  if (!renderingCombo) {
    ImGui::SetWindowFocus();
  }

  AccountTable &table = FerretLayer::Get().GetLedger().GetTables().at(m_ViewingTableID);

  static bool editingTable = false;

  static char nameBuf[32] = {0};
  static int accountID = 0;
  static bool creditAcc = false;
  static TableTracking tracking = TableTracking::Untracked;

  if (accountID == 0) { // Memset will make these 0, since they are static we need to reset it
    snprintf(nameBuf, sizeof(nameBuf), "%s", table.GetName().c_str());
    accountID = table.GetAccountNumber();
    creditAcc = table.IsCreditAccount();
    tracking = table.GetTracking();
  }

  int month = 0, day = 0, year = 0, hour = 0, minute = 0;
  ImGui::Text("Created: %02d/%02d/%04d - %02d:%02d", month, day, year, hour, minute);
  ImGui::Text("Last Updated: %02d/%02d/%04d - %02d:%02d", month, day, year, hour, minute);

    if (!editingTable) {

    ImGui::Text("Table Name: %s", nameBuf);
    ImGui::Text("Table ID: %i", accountID);
    ImGui::Text("IsCreditAccount: %s", creditAcc ? "Y" : "N");
    ImGui::Text("Table Tracking: %s", TableTrackingToString(tracking).c_str());


    if (ImGui::Button("Edit")) {
      editingTable = true;
    }
    ImGui::SameLine();
    if (ImGui::Button("Close")) {
      m_PopupType = PopupType::NONE;
      memset(&nameBuf, 0, sizeof(strlen(nameBuf)));
      memset(&accountID, 0, sizeof(int));
      memset(&creditAcc, 0, sizeof(bool));
      memset(&tracking, 0, sizeof(TableTracking));
    }

  } else {
    ImGui::Text("Table Name:");
    ImGui::SameLine();
    ImGui::InputText("##EditTableName", nameBuf, sizeof(nameBuf));
    ImGui::Text("Table ID:");
    ImGui::SameLine();
    ImGui::InputInt("##EditTableNum", &accountID, 0, 0);
    ImGui::Text("IsCreditAccount:");
    ImGui::SameLine();
    ImGui::Checkbox("##EditIsCredit", &creditAcc);
    ImGui::Text("Table Tracking:");
    ImGui::SameLine();
    if (ImGui::BeginCombo("##tracking", TableTrackingToString(tracking).c_str())) {
      renderingCombo = true;
      for (int i = 0; i < (int)TableTracking::MAX_ITEM; i++) {
        TableTracking track = (TableTracking)i;
        const bool is_selected = (tracking == track);
        if (ImGui::Selectable(TableTrackingToString(track).c_str(), is_selected)) {
          tracking = track;
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

    bool disabled = false;

    if ((FerretLayer::Get().GetLedger().GetTables().find(accountID) != FerretLayer::Get().GetLedger().GetTables().end() || accountID == 0) && accountID != table.GetAccountNumber()) { // account already exists
      ImGui::PushStyleColor(ImGuiCol_Button,        ImVec4(1.0f, 0.0f, 0.0f, 1.0f)); // Pure Red (Normal)
      ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 0.3f, 0.3f, 1.0f)); // Lighter Red (Hovered)
      ImGui::PushStyleColor(ImGuiCol_ButtonActive,  ImVec4(0.8f, 0.0f, 0.0f, 1.0f)); // Darker Red (Clicked)
      disabled = true;
    }

    ImGui::BeginDisabled(disabled);

    if (ImGui::Button("Confirm")) {

      table.SetName(nameBuf);

      table.SetCreditAccount(creditAcc);
      table.SetTracking(tracking);

      // Done last in case accessing the table after being removed causes issues
      if (accountID != table.GetAccountNumber()) {
        FerretLayer::Get().GetLedger().CreateTable(accountID, nameBuf, creditAcc, tracking);

        AccountTable *newTable = &FerretLayer::Get().GetLedger().GetTables().at(accountID);

        LoadEntriesToTable(newTable, *table.GetCreditTable());
        LoadEntriesToTable(newTable, *table.GetDebitTable());

        FerretLayer::Get().GetLedger().RemoveTable(table.GetAccountNumber());
      }

      if (tracking == TableTracking::Income || tracking == TableTracking::Expenses) {
        Application::Get().SubmitToMainThread([](){FerretLayer::Get().GetStatements().NewExpenseOrIncomeDataAvailable();});
      }

      switch (tracking) {
        case TableTracking::Cash:
        case TableTracking::OtherAsset: {
          Application::Get().SubmitToMainThread([](){FerretLayer::Get().GetStatements().NewCashDataAvailable();});
          break;
        }
        case TableTracking::ShortTermLiabilities:
        case TableTracking::LongTermLiabilities: {
          Application::Get().SubmitToMainThread([](){FerretLayer::Get().GetStatements().NewLiabilityDataAvailable();});
          break;
        }
        case TableTracking::Income:
        case TableTracking::Expenses: {
          Application::Get().SubmitToMainThread([](){FerretLayer::Get().GetStatements().NewExpenseOrIncomeDataAvailable();});
          break;
        }
        default: break;
      }

      m_PopupType = PopupType::NONE;
      memset(&nameBuf, 0, sizeof(strlen(nameBuf)));
      memset(&accountID, 0, sizeof(int));
      memset(&creditAcc, 0, sizeof(bool));
      memset(&tracking, 0, sizeof(TableTracking));
      editingTable = false;

      FerretLayer::Get().SetContextDirty(true);
    }

    ImGui::EndDisabled();

    if (disabled)
      ImGui::PopStyleColor(3);


    ImGui::SameLine();
    if (ImGui::Button("Cancel")) {
      snprintf(nameBuf, sizeof(nameBuf), "%s", table.GetName().c_str());
      accountID = table.GetAccountNumber();
      creditAcc = table.IsCreditAccount();
      tracking = table.GetTracking();
      editingTable = false;
    }

    
    if (ImGui::Button("Delete")) {
      m_PopupType = PopupType::NONE;

      memset(&accountID, 0, sizeof(int));
      memset(&creditAcc, 0, sizeof(bool));
      memset(&tracking, 0, sizeof(TableTracking));
    }

    if (ImGui::IsItemHovered()) {
      ImGui::SetTooltip("WARNING - Pressing this will delete this table!");
    }
  }

  ImGui::End();
}

void Popup::CloseLastTable() {
  m_ViewingStatementTableStack.pop_back();
  if (m_ViewingStatementTableStack.empty()) {
    m_PopupType = PopupType::NONE;
  }
}

void Popup::RenderStatementTableDetails(Table *table, char *nameBuf, const size_t &nameBufSize) {
  ImVec2 availableSize = ImGui::GetContentRegionAvail();
  ImVec2 tableViewSize = ImVec2(availableSize.x, availableSize.y - (g_EntrySize.y * 2.0));

  if (ImGui::BeginChild("##TableView", tableViewSize)) {
    ImGuiTableFlags tflags = ImGuiTableFlags_SizingStretchProp;
    if (ImGui::BeginTable("##Viewing-Statement-Table", 1, tflags)) {
      ImGui::TableSetupColumn(table->GetName().c_str());
      Utils::HeaderCentered(1);

      ImGui::TableNextRow();
      ImGui::TableSetColumnIndex(0);

      uint32_t numCols = table->GetCols();
      uint32_t numPaddingCols = numCols > 1 ? numCols + 1 : 2;
      uint32_t totalCols = numCols + numPaddingCols;

      float paddingAmount = 20.0f; // TODO: Let the user define this!
      float totalPadding = paddingAmount * (float)numPaddingCols;

      float itemSize = (tableViewSize.x - totalPadding) / float(numCols);


      ImGui::PushStyleColor(ImGuiCol_TableHeaderBg, ImVec4(0,0,0,0));
      tflags = ImGuiTableFlags_SizingStretchProp;
      if (ImGui::BeginTable("##TableRender", totalCols, tflags)) {
        bool paddingCell = true;
        for (int i = 0; i < totalCols; i++) {
          ImGui::TableSetupColumn("##EmptyHeader", ImGuiTableColumnFlags_WidthFixed, paddingCell ? paddingAmount : itemSize);
          paddingCell = !paddingCell;
        }
        Utils::HeaderCentered(totalCols);

        int rowCount = std::ceil((float)table->GetElementCount() / (float)numCols);

        rowCount += rowCount; // Padding
        uint32_t idx = 0;
        for (int y = 0; y < rowCount; y++) {
          if (y % 2 == 0) {
            ImGui::TableSetColumnIndex(0);
            ImGui::Dummy(ImVec2(ImGui::GetContentRegionAvail().x, g_EntrySize.y));
            ImGui::TableNextRow();
            continue;
          }
          for (int x = 1; x < totalCols; x+=2) {
            ImGui::TableSetColumnIndex(x % totalCols);
            ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(4.0f, 4.0f));
            ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, ImGui::GetColorU32(ImGuiCol_Border));
            const ElementData &element = table->GetElement(idx);
            switch (element.Type) {
              case ElementType::Table: {
                auto &table = FerretLayer::Get().GetStatements().GetTable(element.ElementHash);
                ImGui::Text("Table - %s", table.GetName().c_str());
                break;
              }
              case ElementType::DataSet: {
                auto &dataSet = FerretLayer::Get().GetStatements().GetDataSet(element.ElementHash);
                ImGui::Text("DataSet - %s", dataSet.GetName().c_str());
                break;
              }
              case ElementType::String: {
                auto &string = FerretLayer::Get().GetStatements().GetString(element.ElementHash);
                ImGui::Text("String - %s", string.c_str());
                break;
              }
              default: {
                ImGui::Dummy(ImVec2(ImGui::GetContentRegionAvail().x, g_EntrySize.y));
                break;
              }
            }
            ImGui::PopStyleVar();
            idx++;
          }
          ImGui::TableNextRow();
        }

        ImGui::EndTable();
      }
      ImGui::PopStyleColor();

      ImGui::EndTable();
    }
  }
  ImGui::EndChild();
}

void Popup::RenderStatementTableEdit(Table *table, char *nameBuf, const size_t &nameBufSize) {
  ImVec2 availableSize = ImGui::GetContentRegionAvail();
  ImVec2 tableViewSize = ImVec2(availableSize.x, availableSize.y - (g_EntrySize.y * 2.0));

  if (ImGui::BeginChild("##TableView", tableViewSize)) {
    ImGuiTableFlags tflags = ImGuiTableFlags_SizingStretchProp;
    if (ImGui::BeginTable("##Viewing-Statement-Table", 1, tflags)) {
      ImGui::TableSetupColumn(table->GetName().c_str());
      Utils::HeaderCentered(1);

      ImGui::TableNextRow();
      ImGui::TableSetColumnIndex(0);

      uint32_t numCols = table->GetCols();
      uint32_t numPaddingCols = numCols > 1 ? numCols + 1 : 2;
      uint32_t totalCols = numCols + numPaddingCols;

      float paddingAmount = 20.0f; // TODO: Let the user define this!
      float totalPadding = paddingAmount * (float)numPaddingCols;

      float itemSize = (tableViewSize.x - totalPadding) / float(numCols);


      ImGui::PushStyleColor(ImGuiCol_TableHeaderBg, ImVec4(0,0,0,0));
      tflags = ImGuiTableFlags_SizingStretchProp;
      if (ImGui::BeginTable("##TableRender", totalCols, tflags)) {
        bool paddingCell = true;
        for (int i = 0; i < totalCols; i++) {
          ImGui::TableSetupColumn("##EmptyHeader", ImGuiTableColumnFlags_WidthFixed, paddingCell ? paddingAmount : itemSize);
          paddingCell = !paddingCell;
        }
        Utils::HeaderCentered(totalCols);

        int rowCount = std::ceil((float)table->GetElementCount() + 1 / (float)numCols); // We add one so the user has an optional row below to add

        rowCount += rowCount; // Padding
        uint32_t idx = 0;
        for (int y = 0; y < rowCount; y++) {
          if (y % 2 == 0) {
            ImGui::TableSetColumnIndex(0);
            ImGui::Dummy(ImVec2(ImGui::GetContentRegionAvail().x, g_EntrySize.y));
            ImGui::TableNextRow();
            continue;
          }
          for (int x = 1; x < totalCols; x+=2) {
            ImGui::TableSetColumnIndex(x % totalCols);
            ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(4.0f, 4.0f));
            ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, ImGui::GetColorU32(ImGuiCol_Border));
            const ElementData &element = table->GetElement(idx);
            switch (element.Type) {
              case ElementType::Table: {
                auto &table = FerretLayer::Get().GetStatements().GetTable(element.ElementHash);
                ImGui::Text("Table - %s", table.GetName().c_str());
                break;
              }
              case ElementType::DataSet: {
                auto &dataSet = FerretLayer::Get().GetStatements().GetDataSet(element.ElementHash);
                ImGui::Text("DataSet - %s", dataSet.GetName().c_str());
                break;
              }
              case ElementType::String: {
                auto &string = FerretLayer::Get().GetStatements().GetString(element.ElementHash);
                ImGui::Text("String - %s", string.c_str());
                break;
              }
              default: {
                ImGui::Dummy(ImVec2(ImGui::GetContentRegionAvail().x, g_EntrySize.y));
                break;
              }
            }
            ImGui::PopStyleVar();
            idx++;
          }
          ImGui::TableNextRow();
        }

        ImGui::EndTable();
      }
      ImGui::PopStyleColor();

      ImGui::EndTable();
    }
  }
  ImGui::EndChild();
}

void Popup::RenderStatementTableDetailsStackPopup() {
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
  ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(0, 0));
  ImGui::Begin("Table Details", nullptr, flags);
  static bool renderingCombo = false;

  if (!renderingCombo) {
    ImGui::SetWindowFocus();
  }

  static bool editingTable = false;
  static char nameBuf[128] = {0};
  static Table tableCopy = {};

  uint64_t currID = m_ViewingStatementTableStack.at(m_ViewingStatementTableStack.size() - 1);

  Table &currTable = FerretLayer::Get().GetStatements().GetTable(currID);

  if (strlen(nameBuf) == 0) { // Memset will make these 0, since they are static we need to reset it
    snprintf(nameBuf, sizeof(nameBuf), "%s", currTable.GetName().c_str());
    tableCopy = currTable;
  }

  if (!editingTable) {
    RenderStatementTableDetails(&currTable, nameBuf, sizeof(nameBuf));

    if (ImGui::BeginChild("##ButtonsView", ImGui::GetContentRegionAvail())) {
      float buttonWidth = ImGui::GetContentRegionAvail().x * 0.325f;
      if (ImGui::Button("Edit", ImVec2(buttonWidth, g_EntrySize.y * 1.5f))) {
        editingTable = true;
      }
      ImGui::SameLine();
      if (ImGui::Button("Delete", ImVec2(buttonWidth, g_EntrySize.y * 1.5f))) {
        memset(nameBuf, 0, sizeof(nameBuf));
        Application::Get().SubmitToMainThread([](){ CloseLastTable(); });
      }

      ImGui::SameLine();
      if (ImGui::Button("Close", ImVec2(buttonWidth, g_EntrySize.y * 1.5f))) {
        memset(nameBuf, 0, sizeof(nameBuf));
        Application::Get().SubmitToMainThread([](){ CloseLastTable(); });
      }
    }
    ImGui::EndChild();
  } else {
    RenderStatementTableEdit(&tableCopy, nameBuf, sizeof(nameBuf));

    bool nameDirty = strncmp(nameBuf, currTable.GetName().c_str(), currTable.GetName().length());
    if (ImGui::BeginChild("##ButtonsView", ImGui::GetContentRegionAvail())) {
      float buttonWidth = ImGui::GetContentRegionAvail().x * 0.325f;
      if (ImGui::Button("Confirm", ImVec2(buttonWidth, g_EntrySize.y * 1.5f))) {
        if (!nameDirty) {
          currTable = tableCopy;
        } else {
          const std::string &parentName = FerretLayer::Get().GetStatements().GetTable(tableCopy.GetParentHash()).GetName();
          FerretLayer::Get().GetStatements().ReplaceTable(tableCopy.GetHash(), tableCopy.GetName(), parentName, tableCopy.GetParentHash());
        }
        Application::Get().SubmitToMainThread([](){ CloseLastTable(); });
      }
      ImGui::SameLine();
      if (ImGui::Button("Cancel", ImVec2(buttonWidth, g_EntrySize.y * 1.5f))) {
        snprintf(nameBuf, sizeof(nameBuf), "%s", currTable.GetName().c_str());
        tableCopy = currTable;

        editingTable = false;
      }
    }
    ImGui::EndChild();
  }
  ImGui::End();
  ImGui::PopStyleVar();
}

void Popup::RenderStatementTableDataSetDetailsPopup() {
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
  ImGui::Begin("Data Set Details", nullptr, flags);

  static bool renderingCombo = false;

  if (!renderingCombo) {
    ImGui::SetWindowFocus();
  }

  auto &dataSet = FerretLayer::Get().GetStatements().GetDataSet(m_ViewingStatementDataSetID);

  static bool editingTable = false;

  static char nameBuf[128] = {0};
  static bool increments = false;
  static TableTracking tracking = TableTracking::Untracked;

  bool nameDirty = strncmp(nameBuf, dataSet.GetName().c_str(), sizeof(nameBuf)) != 0;

  if (strlen(nameBuf) == 0) { // Memset will make these 0, since they are static we need to reset it
    snprintf(nameBuf, sizeof(nameBuf), "%s", dataSet.GetName().c_str());
    increments = dataSet.GetIncrementsTotal();
    tracking = dataSet.GetTracking();
  }

  if (!editingTable) {
    ImGui::Text("Data Set Header: %s", nameBuf);
    ImGui::Text("Table Tracking: %s", TableTrackingToString(tracking).c_str());
    ImGui::Text("Increments Total: %s", dataSet.GetIncrementsTotal() ? "Y" : "N");


    if (ImGui::Button("Edit")) {
      editingTable = true;
    }
    ImGui::SameLine();
    if (ImGui::Button("Close")) {
      memset(&nameBuf, 0, sizeof(strlen(nameBuf)));
      memset(&tracking, 0, sizeof(TableTracking));
      memset(&increments, 0, sizeof(bool));
      m_ViewingStatementDataSetID = 0;
    }
  } else {
    ImGui::Text("Data Set Header:");
    ImGui::SameLine();
    ImGui::InputText("##EditTableName", nameBuf, sizeof(nameBuf));

    ImGui::Text("Table Tracking:");
    ImGui::SameLine();
    if (ImGui::BeginCombo("##tracking", TableTrackingToString(tracking).c_str())) {
      renderingCombo = true;
      for (int i = 0; i < (int)TableTracking::MAX_ITEM; i++) {
        TableTracking track = (TableTracking)i;
        const bool is_selected = (tracking == track);
        if (ImGui::Selectable(TableTrackingToString(track).c_str(), is_selected)) {
          tracking = track;
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

    ImGui::TextUnformatted("Increments Total: ");
    ImGui::SameLine();
    ImGui::Checkbox("##Increments-Total", &increments);


    bool disabled = false;

    auto &statements = FerretLayer::Get().GetStatements();
    uint64_t currID = m_ViewingStatementTableStack.at(m_ViewingStatementTableStack.size() - 1);
    std::string parentName = statements.GetTable(currID).GetName();

    if (statements.DataSetExists(Utils::GenerateHash64(parentName + nameBuf)) && nameDirty) {
      disabled = true;
    }

    ImGui::BeginDisabled(disabled);

    if (ImGui::Button("Confirm")) {

      if (!nameDirty) {
        dataSet.SetIncrementsTotal(increments);
        dataSet.SetTracking(tracking);
        dataSet.NewDataAvailable();
      } else {
        statements.ReplaceDataSet(m_ViewingStatementDataSetID, nameBuf, parentName, currID, tracking, increments);
      }

      m_PopupType = PopupType::NONE;
      memset(&nameBuf, 0, sizeof(strlen(nameBuf)));
      memset(&increments, 0, sizeof(bool));
      memset(&tracking, 0, sizeof(TableTracking));
      editingTable = false;

      FerretLayer::Get().SetContextDirty(true);
    }

    ImGui::EndDisabled();

    if (disabled)
      ImGui::PopStyleColor(3);


    ImGui::SameLine();
    if (ImGui::Button("Cancel")) {
      snprintf(nameBuf, sizeof(nameBuf), "%s", dataSet.GetName().c_str());
      increments = dataSet.GetIncrementsTotal();
      tracking = dataSet.GetTracking();
      editingTable = false;
    }

    
    if (ImGui::Button("Delete")) {
      m_PopupType = PopupType::NONE;

      memset(&increments, 0, sizeof(bool));
      memset(&tracking, 0, sizeof(TableTracking));

      statements.RemoveDataSet(m_ViewingStatementDataSetID, currID);
    }

    if (ImGui::IsItemHovered()) {
      ImGui::SetTooltip("WARNING - Pressing this will delete this table!");
    }
  }

  ImGui::End();

}

}
