#pragma once

#include "FerretApp/DataTypes/AccountTable.h"

namespace Ferret {

enum class PopupType {
  NONE = 0,
  CreateTable,
  SaveAndExit, SaveAndOpenExistingTables,
  EntryDetails, TableDetails
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
private:
  static inline PopupType m_PopupType = PopupType::NONE;

  static inline int m_ViewingTableID; // Used to see which table we are accessing to view
  static inline int m_ViewingEntryID; // Used to see which entry is being viewed
  static inline bool m_IsViewingCreditEntry; // Used to see if the entry we have selected is credit

private:
  friend class FerretLayer;
  friend class Ledger;
};

}
