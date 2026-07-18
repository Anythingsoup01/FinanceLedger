#pragma once

#include "FerretApp/DataTypes/AccountTable.h"
#include "FerretApp/Table/Table.h"

namespace Ferret {

enum class PopupType {
  NONE = 0,
  CreateTable,
  SaveAndExit, SaveAndOpenExistingTables,
  EntryDetails, TableDetails,
  StatementTableDetails,
};

class Popup {
public:
  // Renders the desired popup, if none is set it won't render a thing
  static void Render();

  // Sets the RenderPopup State
  static void SetRenderPopup(const PopupType &popup) { m_PopupType = popup; }

  // Sets the context required to view an entry
  static void ViewEntry(const int &tableID, const int &entryID, const bool &isEntryCredit);

  // Sets the context required to view a table
  static void ViewTable(const int &tableID);

  // Sets the context required to view a statement table
  static void ViewStatementTable(const uint64_t &tableHash);
private:
  // Used to render the table creation window
  static void RenderCreateTablePopup();

  // Used to render the save screen when trying to close with a
  // dirty context
  static void RenderSavePopup();

  // Used to render a detailed look at an entry, showing details like Insertion Date,
  // Last Modified Date, and the corresponding Journal Entry for it.
  static void RenderEntryDetailsPopup();

  static void LoadEntriesToTable(AccountTable *toTable, const EntryTable &fromEntryTable);

  // Used to render a detailed look at a table, showing details like Last Insertion Date,
  // Last Insertion ID, Name, ID, and Tracking along with an option to edit it
  static void RenderTableDetailsPopup();

  static void CloseLastTable();

  static void RenderStatementTableDetails(Table *table, char *nameBuf, const size_t &nameBufSize);

  static void RenderStatementTableEdit(Table *table, char *nameBuf, const size_t &nameBufSize);

  // Used to render any sub tables the user views while viewing a table
  static void RenderStatementTableDetailsStackPopup();

  // Used to render the current data set selected
  static void RenderStatementTableDataSetDetailsPopup();
private:
  static inline PopupType m_PopupType = PopupType::NONE;

  static inline int   m_ViewingTableID = 0;           // Used to see which table we are accessing to view
  static inline int   m_ViewingEntryID = 0;           // Used to see which entry is being viewed
  static inline bool  m_IsViewingCreditEntry = 0;     // Used to see if the entry we have selected is credit

  static inline uint64_t                m_ViewingStatementDataSetID = 0;  // Used to track the current data set that is being viewed
  static inline std::vector<uint64_t>   m_ViewingStatementTableStack;  // Used to let the user traverse statement sub tables
private:
  friend class FerretLayer;
};

}
