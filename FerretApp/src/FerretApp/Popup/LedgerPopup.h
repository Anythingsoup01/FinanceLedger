#pragma once

#include "FerretApp/DataTypes/AccountTable.h"

namespace Ferret {

enum class LedgerPopupType {
  NONE = 0,
  CreateTable,
  SaveAndExit, SaveAndOpenExistingTables,
  EntryDetails, TableDetails,
};

class LedgerPopup {
public:
  LedgerPopup() = default;

  void Render();

  // Sets the context required to view an entry
  void ViewEntry(const int &tableID, const int &entryID, const bool &isEntryCredit);

  // Sets the context required to view a table
  void ViewTable(const int &tableID);
private:
  // Used to render the table creation window
  void RenderCreateTablePopup();

  // Used to render the save screen when trying to close with a
  // dirty context
  void RenderSavePopup();

  // Used to render a detailed look at an entry, showing details like Insertion Date,
  // Last Modified Date, and the corresponding Journal Entry for it.
  void RenderEntryDetailsPopup();

  void LoadEntriesToTable(AccountTable *toTable, const EntryTable &fromEntryTable);

  // Used to render a detailed look at a table, showing details like Last Insertion Date,
  // Last Insertion ID, Name, ID, and Tracking along with an option to edit it
  void RenderTableDetailsPopup();
private:
  LedgerPopupType m_LedgerPopupType;

  int   m_ViewingTableID = 0;           // Used to see which table we are accessing to view
  int   m_ViewingEntryID = 0;           // Used to see which entry is being viewed
  bool  m_IsViewingCreditEntry = false; // Used to see if the entry we have selected is credit
};

}
