#pragma once

#include "FerretApp/DataTypes/AccountTable.h"

namespace Ferret {

enum class RenderPopup {
  NONE = 0,
  CreateTable,
  SaveAndExit, SaveAndOpenExistingTables,
  EntryDetails, TableDetails
};

// Used to hold all Account Tables, render, and interact with them
class Ledger {
public:
  // Renders all tables
  void OnRenderData();

  const int &GetRetainedEarningsID() { return m_RetainedEarningsID; }

  // Get's all tables we have stored
  const std::map<int, AccountTable> &GetTables() const { return m_Tables; }
  std::map<int, AccountTable> &GetTables() { return m_Tables; }

  // Sets the table map
  void SetTables(const std::map<int, AccountTable> &tables) { m_Tables = tables; ReloadTables(); }

  // Used to autofill tables referenced when inserting an entry manually
  void SubmitEntryDataToTable(const int &toTable, const bool &isCredit, const Date &date, const int &fromTable, const float &amount, const std::string &journalEntry);

  // Sends the table removal to the main thread; Clears all entries pertaining to the
  // provided table id
  void RemoveTable(const int &tableID);

  // Creates a table on the main thread
  void CreateTable(const int &tableID, const std::string &name, const bool &isCredit, const TableTracking &tracking);

  // Sets all internal variables to view the selected entry in a popup window
  void ViewEntry(const int &tableID, const bool &isCredit, const int &entryID);

  // Sets all internal variables to view the selected table in a popup window
  void ViewTable(const int &tableID);
private:
  void ReloadTables();
private:
  std::map<int, AccountTable> m_Tables;
  std::vector<std::string> m_TableNames; // Get's reloaded everytime we call ReloadTables(); Contains a list of all table names (along with the AccountNumber)

  int m_RetainedEarningsID = 99000;
private:
  friend class Popup;
};

}
