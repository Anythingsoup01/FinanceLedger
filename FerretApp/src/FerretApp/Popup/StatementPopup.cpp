#include "StatementPopup.h"

#include "Ferret/Core/Application.h"
#include "Ferret/Core/Input.h"

#include "FerretApp/Layer/FerretLayer.h"
#include "FerretApp/Utils/Utils.h"

#include <imgui.h>

extern ImVec2 g_EntrySize;

namespace Ferret {

void StatementPopup::Render() {
  switch (m_StatementPopupType) {
    case StatementPopupType::StatementTableDetails: {
      // This is so we only ever render one popup, rather than three
      if (m_ViewingStatementDataSetID != 0) {
        RenderStatementTableDataSetDetailsPopup();
      } else if (m_CurrentTableAddHash != 0) {
        RenderStatementTableAddElement();
      } else {
        RenderStatementTableDetailsStackPopup();
      }
      break;
    }
    default: break;
  }
}

void StatementPopup::ViewStatementTable(const uint64_t &tableHash) {
  m_ViewingStatementTableStack.push_back(tableHash);
  m_StatementPopupType = StatementPopupType::StatementTableDetails;
}

void StatementPopup::CloseLastTable() {
  m_ViewingStatementTableStack.pop_back();
  if (m_ViewingStatementTableStack.empty()) {
    m_StatementPopupType = StatementPopupType::NONE;
  }
}

void StatementPopup::RenderStatementTableDetails(Table *table, char *nameBuf, const size_t &nameBufSize) {
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

void StatementPopup::RenderStatementTableEdit(Table *table, char *nameBuf, const size_t &nameBufSize) {
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

        int rowCount = std::ceil((float)table->GetElementCount() / (float)numCols) + 1; // We add one so the user has an optional row below to add

        rowCount += rowCount; // Padding
        int32_t idx = 0;
        for (int y = 0; y < rowCount; y++) {
          if (y % 2 == 0) {
            ImGui::TableSetColumnIndex(0);
            ImGui::Dummy(ImVec2(ImGui::GetContentRegionAvail().x, g_EntrySize.y));
            ImGui::TableNextRow();
            continue;
          }
          for (int x = 1; x < totalCols; x+=2) {
            ImGui::PushID(idx);
            ImGui::TableSetColumnIndex(x % totalCols);
            ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(4.0f, 4.0f));
            ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, ImGui::GetColorU32(ImGuiCol_Border));
            const ElementData &element = table->GetElement(idx);
            switch (element.Type) {
              case ElementType::Table: {
                auto &table = FerretLayer::Get().GetStatements().GetTable(element.ElementHash);
                ImGui::PushItemWidth(itemSize * 0.75f);
                ImGui::Text("Table - %s", table.GetName().c_str());
                ImGui::PopItemWidth();
                break;
              }
              case ElementType::TableTotal: {
                ImGui::PushItemWidth(itemSize * 0.75f);
                ImGui::Text("Table Total");
                ImGui::PopItemWidth();
                break;
              }
              case ElementType::DataSet: {
                auto &dataSet = FerretLayer::Get().GetStatements().GetDataSet(element.ElementHash);
                ImGui::PushItemWidth(itemSize * 0.75f);
                ImGui::Text("DataSet - %s", dataSet.GetName().c_str());
                ImGui::PopItemWidth();
                break;
              }
              case ElementType::String: {
                auto &string = FerretLayer::Get().GetStatements().GetString(element.ElementHash);
                ImGui::PushItemWidth(itemSize * 0.75f);
                ImGui::Text("String - %s", string.c_str());
                ImGui::PopItemWidth();
                break;
              }
              default: {
                ImGui::PushItemWidth(itemSize * 0.75f);
                ImGui::TextUnformatted("Empty");
                ImGui::PopItemWidth();
                break;
              }
            }

            ImGui::SameLine(itemSize * 0.85f);
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0,0,0,0));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0,0,0,0.1f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0,0,0,0.15f));
            if (element.Type == ElementType::NONE) {
              if (ImGui::Button("+")) {
                ViewAddTableElement(table->GetHash(), idx);
              }
            } else {
              if (ImGui::Button("X")) {
                Application::Get().SubmitToMainThread([element, table, idx](){
                  Statements &statements = FerretLayer::Get().GetStatements();
                  switch (element.Type) {
                    case ElementType::Table: {
                      statements.RemoveTable(element.ElementHash, table->GetHash());
                      break;
                    }
                    case ElementType::TableTotal: {
                      table->RemoveElement(idx);
                      break;
                    }
                    case ElementType::DataSet: {
                      statements.RemoveDataSet(element.ElementHash, table->GetHash());
                      break;
                    }
                    case ElementType::String: {
                      statements.RemoveString(element.ElementHash, table->GetHash());
                      break;
                    }
                    default: break;
                  }
                });
              }
            }

            ImGui::PopStyleColor(3);

            ImGui::PopStyleVar();
            ImGui::PopID();
            idx++;
          }
          ImGui::TableNextRow();
        }

        ImGui::EndTable();
      }
      ImGui::PopStyleColor();

      ImGui::EndTable();
    }

    ImVec2 buttonSize = ImGui::CalcTextSize("#");
    buttonSize.x += 10;
    buttonSize.y += 10;
    bool shift = Input::IsKeyPressed(KeyCode::LeftShift) || Input::IsKeyPressed(KeyCode::RightShift);

    ImGui::SetCursorPos(ImVec2(2, (ImGui::GetWindowHeight() - buttonSize.y) * 0.5f));
    ImGui::PushID("Left");
    if (shift) {
      if (ImGui::Button("-"))
        table->RemoveColumn(table->GetCols(), 1);
    } else {
      if (ImGui::Button("+"))
        table->AddColumn(1);
    }
    ImGui::PopID();

    ImGui::SetCursorPos(ImVec2(ImGui::GetWindowWidth() - (buttonSize.x + 2), (ImGui::GetWindowHeight() - buttonSize.y) * 0.5f));
    ImGui::PushID("Right");
    if (shift) {
      if (ImGui::Button("-"))
        table->RemoveColumn(table->GetCols(), 1);
    } else {
      if (ImGui::Button("+"))
        table->AddColumn(1);
    }
    ImGui::PopID();
  }

  ImGui::EndChild();
}

void StatementPopup::RenderStatementTableDetailsStackPopup() {
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

  uint64_t currID = m_ViewingStatementTableStack.at(m_ViewingStatementTableStack.size() - 1);

  Table &currTable = FerretLayer::Get().GetStatements().GetTable(currID);

  if (strlen(nameBuf) == 0) { // Memset will make these 0, since they are static we need to reset it
    snprintf(nameBuf, sizeof(nameBuf), "%s", currTable.GetName().c_str());
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
        Application::Get().SubmitToMainThread([this](){ CloseLastTable(); });
      }

      ImGui::SameLine();
      if (ImGui::Button("Close", ImVec2(buttonWidth, g_EntrySize.y * 1.5f))) {
        memset(nameBuf, 0, sizeof(nameBuf));
        Application::Get().SubmitToMainThread([this](){ CloseLastTable(); });
      }
    }
    ImGui::EndChild();
  } else {
    RenderStatementTableEdit(&currTable, nameBuf, sizeof(nameBuf));

    bool nameDirty = strncmp(nameBuf, currTable.GetName().c_str(), currTable.GetName().length());
    if (ImGui::BeginChild("##ButtonsView", ImGui::GetContentRegionAvail())) {
      float buttonWidth = ImGui::GetContentRegionAvail().x * 0.325f;
      if (ImGui::Button("Confirm", ImVec2(buttonWidth, g_EntrySize.y * 1.5f))) {
        if (nameDirty) {
          const std::string &parentName = FerretLayer::Get().GetStatements().GetTable(currTable.GetParentHash()).GetName();
          FerretLayer::Get().GetStatements().ReplaceTable(currTable.GetHash(), currTable.GetName(), parentName, currTable.GetParentHash());
        }
        editingTable = false;
      }
      ImGui::SameLine();
      if (ImGui::Button("Cancel", ImVec2(buttonWidth, g_EntrySize.y * 1.5f))) {
        snprintf(nameBuf, sizeof(nameBuf), "%s", currTable.GetName().c_str());

        for (auto &[hash, type] : m_DirtyElements) {
          switch (type) {
            case ElementType::Table: {
              FerretLayer::Get().GetStatements().RemoveTable(hash, currTable.GetHash());
              break;
            }
            case ElementType::DataSet: {
              FerretLayer::Get().GetStatements().RemoveDataSet(hash, currTable.GetHash());
              break;
            }
            case ElementType::String: {
              FerretLayer::Get().GetStatements().RemoveString(hash, currTable.GetHash());
              break;
            }
            default: break;
          }
        }

        m_DirtyElements.clear();

        editingTable = false;
      }
    }
    ImGui::EndChild();
  }

  ImGui::End();
  ImGui::PopStyleVar();
}

void StatementPopup::ViewAddTableElement(const uint64_t &tableHash, const int32_t &index) {
  m_CurrentTableAddHash = tableHash;
  m_CurrentTableElementIndex = index;
}

void StatementPopup::RenderStatementTableAddElement() {
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

  static ElementType elementType = ElementType::NONE;

  static char nameOrStrBuf[128] = {0};
  static TableTracking tracking = TableTracking::Untracked;
  static bool incrementsTable = false;

  ImVec2 availableSize = ImGui::GetContentRegionAvail();
  ImVec2 tableViewSize = ImVec2(availableSize.x, availableSize.y - (g_EntrySize.y * 2.0));
  if (ImGui::BeginChild("##ElementCreationView", tableViewSize)) {
    ImGui::TextUnformatted("Element Type: ");
    ImGui::SameLine();
    if (ImGui::BeginCombo("##ElementCreationTypeCombo", ElementTypeToString(elementType).c_str())) {
      renderingCombo = true;
      for (int i = 0; i < (int)ElementType::MAX_ITEM; i++) {
        ElementType type = (ElementType)i;
        const bool is_selected = (elementType == type);
        if (ImGui::Selectable(ElementTypeToString(type).c_str(), is_selected)) {
          elementType = type;
        }

        if (is_selected) {
          ImGui::SetItemDefaultFocus();
        }
      }
      ImGui::EndCombo();
    } else {
      renderingCombo = false;
    }

    switch (elementType) {
      case ElementType::Table: {
        ImGui::TextUnformatted("Table Name: ");
        ImGui::SameLine();
        ImGui::InputText("##TableNameInput", nameOrStrBuf, sizeof(nameOrStrBuf));
        break;
      }
      case ElementType::DataSet: {
        ImGui::TextUnformatted("Data Set Header: ");
        ImGui::SameLine();
        ImGui::InputText("##DataSetHeaderInput", nameOrStrBuf, sizeof(nameOrStrBuf));

        ImGui::TextUnformatted("Data Set Tracked Table: ");
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

        ImGui::TextUnformatted("Increments Table Balance: ");
        ImGui::Checkbox("##IncrementsTableTotal", &incrementsTable);
        break;
      }
      case ElementType::String: {
        ImGui::TextUnformatted("Static String: ");
        ImGui::SameLine();
        ImGui::InputText("##StringInput", nameOrStrBuf, sizeof(nameOrStrBuf));
        break;
      }
      default: break;
    }
  }
  ImGui::EndChild();
  if (ImGui::BeginChild("##AddElementButtonsView", ImGui::GetContentRegionAvail())) {
    if (ImGui::Button("Confirm", ImVec2(ImGui::GetContentRegionAvail().x * 0.25, g_EntrySize.y))) {
      Statements &statements = FerretLayer::Get().GetStatements();
      Table &table = statements.GetTable(m_CurrentTableAddHash);
      std::string parentLegalName = table.GetParentLegalName() + table.GetName();
      uint64_t elementHash = 0;
      switch (elementType) {
        case ElementType::Table: {
          statements.AddTable(m_CurrentTableElementIndex, nameOrStrBuf, parentLegalName, table.GetHash());
          break;
        }
        case ElementType::TableTotal: {
          break;
        }
        case ElementType::DataSet: {
          statements.AddDataSet(m_CurrentTableElementIndex, nameOrStrBuf, parentLegalName, table.GetHash(), tracking, incrementsTable);
          break;
        }
        case ElementType::String: {
          statements.AddString(m_CurrentTableElementIndex, nameOrStrBuf, parentLegalName, table.GetHash());
          break;
        }
        default: break;
      }
      elementHash = Utils::GenerateHash64(parentLegalName + nameOrStrBuf);

      m_DirtyElements.emplace(std::pair<uint64_t, ElementType>(elementHash, elementType));

      elementType = ElementType::NONE;
      memset(nameOrStrBuf, 0, sizeof(nameOrStrBuf));
      tracking = TableTracking::Untracked;
      incrementsTable = false;
      m_CurrentTableAddHash = 0;
    }

    ImGui::SameLine();
    if (ImGui::Button("Cancel")) {
      elementType = ElementType::NONE;
      memset(nameOrStrBuf, 0, sizeof(nameOrStrBuf));
      tracking = TableTracking::Untracked;
      incrementsTable = false;
      m_CurrentTableAddHash = 0;
    }
  } 
  ImGui::EndChild();

  ImGui::End();

}

void StatementPopup::RenderStatementTableDataSetDetailsPopup() {
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

      m_StatementPopupType = StatementPopupType::NONE;
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
      m_StatementPopupType = StatementPopupType::NONE;

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
