#include "Table.h"
#include "FerretApp/Utils/Utils.h"

#include "FerretApp/Layer/FerretLayer.h"

#include "FerretApp/Popup/Popup.h"

#include <imgui.h>

namespace Ferret {

Table::Table(const std::string &name, const std::string &parentName)
  : m_Name(name), m_Cols(1), m_ElementCount(0) {
  m_Hash = Utils::GenerateHash64(name + parentName);
  m_ParentHash = parentName.empty() ? 0 : Utils::GenerateHash64(parentName);
  m_DataSets.clear();
  m_SubTables.clear();
}

void Table::Render() {
  ImGuiTableFlags flags = ImGuiTableFlags_Borders | ImGuiTableFlags_BordersV | ImGuiTableFlags_SizingStretchProp;
  ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(0,0));
  char buf[128] = {0};
  snprintf(buf, sizeof(buf), "##%sTable", m_Name.c_str());
  ImGui::PushID(m_Hash);
  if (ImGui::BeginTable(buf, 1, flags | ImGuiTableFlags_RowBg)) {
    ImGui::TableSetupColumn(m_Name.c_str());
    Utils::HeaderCentered(1);
    if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0)) {
      // TODO: Popup::ViewStatementTable(m_Hash);
    }

    ImGui::TableNextRow();
    ImGui::TableSetColumnIndex(0);

    RenderDrawTable();

    ImGui::EndTable();
  }
  ImGui::PopID();
  ImGui::PopStyleVar();
}

void Table::RenderDrawTable() {
  ImGuiTableFlags flags = ImGuiTableFlags_Borders | ImGuiTableFlags_BordersV | ImGuiTableFlags_SizingStretchProp;
  ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(0,0));
  char buf[128] = {0};
  snprintf(buf, sizeof(buf), "##%sTable", m_Name.c_str());
  float availableWidth = ImGui::GetContentRegionAvail().x;
  ImGui::PushID(m_Hash);
  if (ImGui::BeginTable(buf, m_Cols, flags)) {
    for (int i = 0; i < m_Cols; i++) {
      ImGui::TableSetupColumn("##EmptyHeader", ImGuiTableColumnFlags_WidthFixed,availableWidth / (float)m_Cols);
    }
    Utils::HeaderCentered(m_Cols);

    int rowCount = std::floor((float)m_ElementCount / (float)m_Cols) + 1;

    for (int x = 0; x < rowCount; x++) {
      for (int i = 0; i < m_Cols; i++) {
        ImGui::TableSetColumnIndex(i % m_Cols);

        if (m_SubTables.find(i) != m_SubTables.end()) { // Column Contains a SubTable!
          auto &table = FerretLayer::Get().GetStatements().GetTable(m_SubTables.at(i));
          table.Render();
        } else if (m_DataSets.find(i) != m_DataSets.end()) {
          auto &dataSet = FerretLayer::Get().GetStatements().GetDataSet(m_DataSets.at(i));
          dataSet.Render();
        } else {
          ImGui::Dummy(ImGui::GetContentRegionAvail());
        }
      }
      ImGui::TableNextRow();
    }

    ImGui::EndTable();
  }
  ImGui::PopID();
  ImGui::PopStyleVar();
}

void Table::UpdateTotal() const {
  // Update this table


  // Update parent if there is one
  if (m_ParentHash != 0) {
    auto &parent = FerretLayer::Get().GetStatements().GetTable(m_ParentHash);
    parent.UpdateTotal();
  }
}



}
