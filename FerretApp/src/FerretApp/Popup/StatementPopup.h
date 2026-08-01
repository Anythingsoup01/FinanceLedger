#pragma once

#include "FerretApp/Table/Table.h"

namespace Ferret {

enum class StatementPopupType {
  NONE = 0,
  StatementTableDetails,
};

class StatementPopup {
public:
  StatementPopup() = default;

  void SetPopup(const StatementPopupType &popup) { m_StatementPopupType = popup; }


  void Render();

  // Sets the context required to view a statement table
  void ViewStatementTable(const uint64_t &tableHash);
private:
  void CloseLastTable();

  void RenderStatementTableDetails(Table *table, char *nameBuf, const size_t &nameBufSize);

  void RenderStatementTableEdit(Table *table, char *nameBuf, const size_t &nameBufSize);

  // Used to render any sub tables the user views while viewing a table
  void RenderStatementTableDetailsStackPopup();

  void ViewAddTableElement(const uint64_t &tableHash, const int32_t &index);

  void RenderStatementTableAddElement();

  // Used to render the current data set selected
  void RenderStatementTableDataSetDetailsPopup();

private:
  StatementPopupType                          m_StatementPopupType;
  uint64_t                                    m_ViewingStatementDataSetID = 0;  // Used to track the current data set that is being viewed
  std::vector<uint64_t>                       m_ViewingStatementTableStack;  // Used to let the user traverse statement sub tables
  uint64_t                                    m_CurrentTableAddHash = 0;
  int32_t                                     m_CurrentTableElementIndex = 0;
  std::unordered_map<uint64_t, ElementType>   m_DirtyElements;
};

}
