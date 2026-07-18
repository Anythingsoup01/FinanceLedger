#include "Table.h"
#include "FerretApp/Utils/Utils.h"

#include "FerretApp/Layer/FerretLayer.h"

#include "FerretApp/Popup/Popup.h"

#include <imgui.h>

extern ImVec2 g_EntrySize;

namespace Ferret {

Table::Table(const std::string &name, const std::string &parentLegalName, const uint64_t &parentHash)
  : m_Name(name), m_ParentLegalName(parentLegalName), m_ParentHash(parentHash), m_Cols(1) {
  m_Hash = Utils::GenerateHash64(parentLegalName + name);
  m_Elements.emplace(std::pair<int32_t, ElementData>(0, ElementData{0, ElementType::NONE}));
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
      Popup::ViewStatementTable(m_Hash);
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
  ImGui::PushStyleColor(ImGuiCol_TableHeaderBg, ImVec4(0,0,0,0));
  char buf[128] = {0};
  snprintf(buf, sizeof(buf), "##%sTable", m_Name.c_str());
  float availableWidth = ImGui::GetContentRegionAvail().x;
  ImGui::PushID(m_Hash);
  if (ImGui::BeginTable(buf, m_Cols, flags)) {
    for (int i = 0; i < m_Cols; i++) {
      ImGui::TableSetupColumn("##EmptyHeader", ImGuiTableColumnFlags_WidthFixed,availableWidth / (float)m_Cols);
    }
    Utils::HeaderCentered(m_Cols);

    int rowCount = std::ceil((float)GetElementCount() / (float)m_Cols);

    for (int y = 0; y < rowCount; y++) {
      for (int x = 0; x < m_Cols; x++) {
        ImGui::TableSetColumnIndex(x % m_Cols);
        uint32_t idx = x + (y * m_Cols);
        const ElementData &element = m_Elements.at(idx);

        switch (element.Type) {
          case ElementType::Table: {
            auto &table = FerretLayer::Get().GetStatements().GetTable(element.ElementHash);
            table.Render();
            break;
          }
          case ElementType::DataSet: {
            auto &dataSet = FerretLayer::Get().GetStatements().GetDataSet(element.ElementHash);
            dataSet.Render();
            break;
          }
          case ElementType::String: {
            auto &string = FerretLayer::Get().GetStatements().GetString(element.ElementHash);
            ImGui::TextUnformatted(string.c_str());
            break;
          }
          default: {
            ImGui::Dummy(ImVec2(ImGui::GetContentRegionAvail().x, g_EntrySize.y));
            break;
          }
        }
      }
      ImGui::TableNextRow();
    }

    ImGui::EndTable();
  }
  ImGui::PopID();
  ImGui::PopStyleColor();
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

// INTERNAL USE ONLY
//
// Used by statements to change the name and reload the hash
// for rendering.
//
// Users must not utilize this in any capacity as it may cause
// crashes and/or unexpected behavior.
void Table::SetName(const std::string &name, const std::string &parentLegalName) {
  m_Name = name;
  m_ParentLegalName = parentLegalName;
  m_Hash = Utils::GenerateHash64(parentLegalName + name);
}

// INTERNAL USE ONLY
//
// Used to set the internal parent hash for pushing updates.
//
// Users must not utilize this in any capacity as it may cause
// crashes and/or unexpected behavior.
void Table::SetParentHash(const uint64_t &parentHash) {
  m_ParentHash = parentHash;
}

const ElementData &Table::GetElement(const int32_t &index) const {
  if (m_Elements.find(index) == m_Elements.end()) { // Doesn't Exist, return an empty element (for editing purposes)
    static ElementData s_EmptyData = ElementData{0, ElementType::NONE};
    return s_EmptyData;
  }

  return m_Elements.at(index);
}

int32_t Table::GetElementIdx(const uint64_t &hash) {
  for (auto &[index, data] : m_Elements) {
    if (hash == data.ElementHash) {
      return index;
    }
  }
  return -1;
}

void Table::ReplaceElement(const uint64_t &oldHash, const uint64_t &newHash, const ElementType &type) {
  int32_t idx = GetElementIdx(oldHash);
  if (idx == -1) {
    return;
  }

  RemoveElement(idx);
  AddElement(idx, newHash, type);
}

void Table::RemoveElement(const uint64_t &hash) {
  int32_t idx = GetElementIdx(hash);
  if (idx == -1) {
    return;
  }

  m_Elements.erase(idx);
}

void Table::RemoveElement(const int32_t &index) {
  m_Elements.erase(index);
}

void Table::AddElement(const int32_t &index, const uint64_t &hash, const ElementType &type) {
  if (index >= m_Elements.size() && index <= m_Elements.size() + m_Cols - 1) {
    for (int i = m_Elements.size(); i < m_Elements.size() + m_Cols; i++) {
      m_Elements.emplace(std::pair<int32_t, ElementData>(i, ElementData{0, ElementType::NONE}));
    }
  }

  ElementData &data = m_Elements.at(index);
  data.ElementHash = hash;
  data.Type = type;
}

}
