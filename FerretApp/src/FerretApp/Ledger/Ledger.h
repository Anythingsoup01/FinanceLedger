#pragma once

#include "Table.h"

namespace Ferret {

enum class RenderPopup {
  NONE = 0,
  CreateTable,
  SaveAndExit, SaveAndOpenExistingTables,
  EntryDetails, TableDetails
};

class Ledger {
public:
  // Renders all tables
  void OnRenderData();

  // Get's all tables we have stored
  static const std::map<int, AccountTable> &GetTables() { return m_Tables; }

  // Sets the table map
  static void SetTables(const std::map<int, AccountTable> &tables) { m_Tables = tables; ReloadTables(); }

  // Used to autofill tables referenced when inserting an entry manually
  static void SubmitEntryDataToTable(const int &toTable, const bool &isCredit, const Date &date, const int &fromTable, const float &amount);

  // Tells the system when the context needs saved
  static void SetDirty(const bool &dirty = true) { m_ContextDirty = dirty; }
  static bool IsDirty() { return m_ContextDirty; }

  // Sets the RenderPopup State
  static void SetRenderPopup(const RenderPopup &popup) { m_RenderPopup = popup; }

  // Sends the table removal to the main thread; Clears all entries pertaining to the
  // provided table id
  static void RemoveTable(const int &tableID);

  // Creates a table on the main thread
  static void CreateTable(const int &tableID, const std::string &name, const bool &isCredit, const TableTracking &tracking);

  // Sets all internal variables to view the selected entry in a popup window
  static void ViewEntry(const int &tableID, const bool &isCredit, const int &entryID);

  // Sets all internal variables to view the selected table in a popup window
  static void ViewTable(const int &tableID);
private:
  // When creating a table we should reload each tables' m_Next pointer
  static void ReloadTables();

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
  // TODO: Create a RenderPopup Class / Abstract class so we can assign popups to sections
  static inline RenderPopup m_RenderPopup = RenderPopup::NONE;
private:
  static inline std::map<int, AccountTable> m_Tables;
  static inline std::vector<std::string> m_TableNames; // Get's reloaded everytime we call ReloadTables(); Contains a list of all table names (along with the AccountNumber)

  static inline int m_ViewingTableID; // Used to see which table we are accessing to view
  static inline int m_ViewingEntryID; // Used to see which entry is being viewed
  static inline bool m_IsViewingCreditEntry; // Used to see if the entry we have selected is credit

  static inline bool m_ContextDirty = false;
};

}
