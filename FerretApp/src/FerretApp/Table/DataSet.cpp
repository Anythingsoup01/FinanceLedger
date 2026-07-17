#include "DataSet.h"

#include "FerretApp/Layer/FerretLayer.h"
#include "FerretApp/Utils/Utils.h"

extern ImVec2 g_EntrySize;

namespace Ferret {

DataSet::DataSet(const std::string &header, const std::string &parentName, const TableTracking &tracking)
  : m_Header(header) {
  m_Hash = Utils::GenerateHash64(parentName + header);
  m_ParentHash = Utils::GenerateHash64(parentName);

  m_Entries.clear();

  for (auto &[id, table] : FerretLayer::Get().GetLedger().GetTables()) {
    if (table.GetTracking() == tracking) {
      float balance = table.GetBalance();
      m_Entries.push_back(DataEntry(table.GetName(), table.GetAccountNumber(), balance));
      m_Total += balance;
    }
  }
}

void DataSet::Render() {
  ImGuiTableFlags flags = ImGuiTableFlags_Borders | ImGuiTableFlags_BordersV | ImGuiTableFlags_SizingStretchProp;
  ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(0,0));
  char buf[128] = {0};
  snprintf(buf, sizeof(buf), "##%sDataSet", m_Header.c_str());
  ImGui::PushID(m_Hash);
  float availableWidth = ImGui::GetContentRegionAvail().x; // TODO: Prompt the user to set the size, if it's fixed, screen proportional or dynamically set!
  if (ImGui::BeginTable(buf, 3, flags)) {
    ImGui::TableSetupColumn(m_Header.c_str(), ImGuiTableColumnFlags_WidthFixed, availableWidth * 0.25f);
    ImGui::TableSetupColumn("##EmptyHeader", ImGuiTableColumnFlags_WidthFixed, availableWidth * 0.5);
    ImGui::TableSetupColumn("##EmptyHeader", ImGuiTableColumnFlags_WidthFixed, availableWidth * 0.25);

    Utils::HeaderCentered(3);
    ImGui::TableNextRow();

    for (auto &entry : m_Entries)
      entry.Render();

    ImGui::TableSetColumnIndex(0);
    ImGui::Dummy(ImVec2(ImGui::GetColumnWidth(), g_EntrySize.y));

    ImGui::EndTable();
  }
  ImGui::PopID();
  ImGui::PopStyleVar();
}

void DataSet::NewDataAvailable() {
  m_Entries.clear();
  m_Total = 0;

  for (auto &[id, table] : FerretLayer::Get().GetLedger().GetTables()) {
    if (table.GetTracking() == m_Tracking) {
      float balance = table.GetBalance();
      m_Entries.push_back(DataEntry(table.GetName(), table.GetAccountNumber(), balance));
      m_Total += balance;
    }
  }

  auto &parentTable = FerretLayer::Get().GetStatements().GetTable(m_ParentHash);
  parentTable.UpdateTotal();
}

}
