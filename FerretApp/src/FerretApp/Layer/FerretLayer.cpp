#include "FerretLayer.h"
#include "Ferret.h"
#include "imgui.h"

namespace Ferret {

typedef struct EntryData {
  std::string Date;
  int Account;
  float DollarAmount;
} EntryData_t;

typedef struct EntryTable {
  std::string TableName;
  std::vector<EntryData_t> Entries;
  bool IsDebit;
} EntryTable_t;

typedef struct TableData {
  int AccountNumber;
  std::string AccountName;
  std::vector<EntryTable_t> EntryTables;
} TableData_t;

static TableData_t s_CashTable;

void TableDataInit(TableData_t *table, const std::string &accName, int accNum) {
  std::vector<EntryData_t> defaultEntries;

  EntryTable_t defaultDebit = {.TableName="Debit", .Entries=defaultEntries, .IsDebit=true};
  EntryTable_t defaultCredit = {.TableName="Credit", .Entries=defaultEntries, .IsDebit=false};

  table->AccountName = accName;
  table->AccountNumber = accNum;

  table->EntryTables.push_back(defaultDebit);
  table->EntryTables.push_back(defaultCredit);
}

void entryTableInsertEntry(EntryTable_t *table, const std::string &date, int account, float dollarAmount) {
  table->Entries.push_back({date, account, dollarAmount});
}

void FerretLayer::OnAttach() {
  TableDataInit(&s_CashTable, "Cash", 101);
  entryTableInsertEntry(&s_CashTable.EntryTables[0], "06/25/26", 101, 100.50);
  entryTableInsertEntry(&s_CashTable.EntryTables[0], "06/25/26", 101, 100.50);
}

void FerretLayer::OnDetach() {}

void FerretLayer::OnUpdate() {}

void DrawSubTable(EntryTable_t *subTable) {
  ImGuiTableFlags flags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersV | ImGuiTableFlags_SizingStretchProp;
  char buf[32] = {0};
  sprintf(buf, "##%s", subTable->TableName.c_str());

  ImGui::BeginTable(buf, 3, flags);

  ImGui::TableSetupColumn("Date", ImGuiTableColumnFlags_WidthStretch, ImGui::CalcTextSize("##/##/####").x);
  ImGui::TableSetupColumn("Account", ImGuiTableColumnFlags_WidthStretch, ImGui::CalcTextSize("Account").x);
  ImGui::TableSetupColumn("Amount", ImGuiTableColumnFlags_WidthStretch, ImGui::CalcTextSize("Amount").x);
  ImGui::TableHeadersRow();

  for (auto &entry : subTable->Entries) {
    ImGui::TableNextRow();
    ImGui::TableSetColumnIndex(0);
    ImGui::TextUnformatted(entry.Date.c_str());

    ImGui::TableSetColumnIndex(1);
    ImGui::Text("%i", entry.Account);

    ImGui::TableSetColumnIndex(2);
    ImGui::Text("%.2f", entry.DollarAmount);
  }

  ImGui::TableNextRow();
  bool submitted = false;
  static char newDate[64] = "";
  static int newAccount = 0;
  static float newAmount = 0.0f;

  ImGui::TableSetColumnIndex(0);
  ImGui::SetNextItemWidth(-FLT_MIN);
  sprintf(buf, "##NewData%s", subTable->TableName.c_str());
  if (ImGui::InputText(buf, newDate, sizeof(newDate), ImGuiInputTextFlags_EnterReturnsTrue))
    submitted = true;

  ImGui::TableSetColumnIndex(1);
  ImGui::SetNextItemWidth(-FLT_MIN);
  sprintf(buf, "##NewAcc%s", subTable->TableName.c_str());
  if (ImGui::InputInt(buf, &newAccount, 0, 0, ImGuiInputTextFlags_EnterReturnsTrue))
    submitted = true;

  ImGui::TableSetColumnIndex(2);
  ImGui::SetNextItemWidth(-FLT_MIN);
  sprintf(buf, "##NewAmt%s", subTable->TableName.c_str());
  if (ImGui::InputFloat(buf, &newAmount, 0, 0, "%.2f", ImGuiInputTextFlags_EnterReturnsTrue))
    submitted = true;

  if (submitted) {
    entryTableInsertEntry(subTable, newDate, newAccount, newAmount);
    memset(newDate, 0, sizeof(newDate));
    memset(&newAccount, 0, sizeof(int));
    memset(&newAmount, 0, sizeof(float));
  }


  ImGui::EndTable();

}

void DrawTable(TableData_t *tableData) {
  ImGui::Begin("Item 2.0");
  ImGui::Text("%s (Acc %i)", tableData->AccountName.c_str(), tableData->AccountNumber);

  float scale_x = ImGui::GetContentRegionAvail().x;

  ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(0, 0));

  char buf[32] = {0};
  sprintf(buf, "##%s%i", tableData->AccountName.c_str(), tableData->AccountNumber);

  ImGuiTableFlags flags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersV | ImGuiTableFlags_SizingStretchProp;

  ImGui::BeginTable(buf, tableData->EntryTables.size(), flags);

  for (auto &tableEntry : tableData->EntryTables) {
    ImGui::TableSetupColumn(tableEntry.TableName.c_str(), ImGuiTableColumnFlags_WidthFixed, scale_x / 2.0f);
  }
  ImGui::TableHeadersRow();
  ImGui::TableNextRow();

  int column = 0;
  
  for (EntryTable_t &tableEntry : tableData->EntryTables) {
    ImGui::TableSetColumnIndex(column++);
    DrawSubTable(&tableEntry);
  }

  ImGui::EndTable();

  ImGui::PopStyleVar();

  ImGui::End();

}

void FerretLayer::OnUIRender() {
  ImGui::Begin("Item");

  ImGui::Text("Cash (Acc 101)");

  ImGuiTableFlags flags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersV | ImGuiTableFlags_SizingStretchProp;

  // MAIN TABLE (Displays Both Parts of account (Debit & Credit))
  ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(0, 0));
  ImGui::BeginTable("Cash", 2, flags);

  ImGui::TableSetupColumn("Debit");
  ImGui::TableSetupColumn("Credit");
  ImGui::TableHeadersRow();

  ImGui::TableNextRow();

  for (int i = 0; i < 2; i++) {
    ImGui::TableSetColumnIndex(i);
    ImGui::BeginTable(i == 0 ? "##Debit" : "##Credit", 3, flags);

    ImGui::TableSetupColumn("Date", ImGuiTableColumnFlags_WidthStretch);
    ImGui::TableSetupColumn("Account", ImGuiTableColumnFlags_WidthStretch);
    ImGui::TableSetupColumn("Amount", ImGuiTableColumnFlags_WidthStretch);
    ImGui::TableHeadersRow();

    for (int x = 0; x < 5; x++) {
      ImGui::TableNextRow();
      for (int j = 0; j < 3; j++) {
        ImGui::TableSetColumnIndex(j);
        char buf[32];
        sprintf(buf, "Hello %d,%d", j, x);
          ImGui::TextUnformatted(buf);
      }
    }

    ImGui::EndTable();
  }

  ImGui::EndTable(); // Cash

  ImGui::PopStyleVar();

  ImGui::End();

  DrawTable(&s_CashTable);


  ImGui::ShowDemoWindow();
}

void FerretLayer::OnEvent(Event &e) {
  EventDispatcher dispatcher(e);

  dispatcher.Dispatch<KeyPressedEvent>(
      BIND_EVENT_FN(FerretLayer::OnKeyPressedEvent));
}

bool FerretLayer::OnKeyPressedEvent(KeyPressedEvent &e) {
  return false;
}

void FerretLayer::LogExample() { FE_CLI_INFO("Logging from MenuBar"); }
} // namespace Ferret
