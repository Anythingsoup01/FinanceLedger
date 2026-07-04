#pragma once

#include "Table.h"

namespace Ferret {

enum class RenderPopup {
  NONE = 0,
  CreateTable,
  SaveAndExit,
  SaveAndOpenExistingTables,
  EntryDetails,
};

class Ledger {
public:
  Ledger();
  ~Ledger();

  // Renders all tables
  void OnRenderData();


  // Get's all tables we have stored
  const std::map<int, AccountTable> &GetTables() const { return m_Tables; }

  // Sets the table map
  void SetTables(const std::map<int, AccountTable> &tables) { m_Tables = tables; ReloadTables(); }


  // Used to autofill tables referenced when inserting an entry manually
  void SubmitEntryDataToTable(const int &toTable, const bool &isCredit, const Date &date, const int &fromTable, const float &amount);

  // Tells the system when the context needs saved
  void SetDirty(const bool &dirty = true) { m_ContextDirty = dirty; }
  bool IsDirty() { return m_ContextDirty; }

  // Sets the RenderPopup State
  void SetRenderPopup(const RenderPopup &popup) { m_RenderPopup = popup; }

  // Sends the table removal to the main thread; Clears all entries pertaining to the
  // provided table id
  void RemoveTable(const int &tableID);

  // Sets all internal variables to view the selected entry in a popup window
  void ViewEntry(const int &tableID, const bool &isCredit, const int &entryID);

  static Ledger &Get() { return *s_Instance; }
private:
  // When creating a table we should reload each tables' m_Next pointer
  void ReloadTables();

  // Used to render the table creation window
  void RenderCreateTablePopup();

  // Used to render the save screen when trying to close with a
  // dirty context
  void RenderSavePopup();

  // Used to render a detailed look at an entry, showing details like Insertion Date,
  // Last Modified Date, and the corresponding Journal Entry for it.
  void RenderEntryDetailsPopup();
private:
  // TODO: Create a RenderPopup Class / Abstract class so we can assign popups to sections
  RenderPopup m_RenderPopup = RenderPopup::NONE;
private:
  std::map<int, AccountTable> m_Tables;
  std::vector<std::string> m_TableNames; // Get's reloaded everytime we call ReloadTables(); Contains a list of all table names (along with the AccountNumber)

  int m_ViewingTableID; // Used to see which table we are accessing to view
  int m_ViewingEntryID; // Used to see which entry is being viewed
  bool m_IsViewingCreditEntry; // Used to see if the entry we have selected is credit

  bool m_ContextDirty = false;

  static inline Ledger *s_Instance = nullptr;
};

}
