#pragma once

#include "Entry.h"

namespace Ferret {

class EntryTable {
public:
  EntryTable(int32_t accountID, bool isCredit)
    : m_AccountID(accountID), m_CreditTable(isCredit) {}

  // Inserts a new data entry and optionally updates the referenced table.
  //
  // Returns false when trying to overlap entries, ie. same date and account.
  bool InsertEntryData(const Date &date, const int &account, const float &amount, bool updateOther);

  // This utilizes the id of the selected entry to remove it from the entry list
  void RemoveEntry(int id);

  // Used to completely remove entries from the given table id
  void RemoveEntriesFromTable(const int &tableID);

  // Used to get a specific entry from the table
  Entry &GetEntry(const int &entryID);

  // Gets all entries from the table
  const std::map<int, Entry> &GetEntries() const { return m_Entries; }

  // Gets the total count of entries
  inline int GetCount() { return m_Entries.size(); }

  // Get's the total value from the table
  const float &GetTotalValue() const { return m_TotalValue; }

  // Used render the table and it's entries
  // Returns true when submitting a new entry
  bool RenderTable(const std::string &tableName, const int &columnIndex);

  // Used to render the table's total value
  void RenderTableTotal(const std::string &tableName, const int &columnIndex);
private:
  Date m_DateBuffer = {0,0,0}; // Buffer used for entry insertion
  int m_AccountBuffer = 0; // Buffer used for entry insertion
  float m_AmountBuffer = 0; // Buffer used for entry insertion

  int m_AccountID = 0; // Holds the account ID of the parent table
  float m_TotalValue = 0; // Holds the total value of all the entries combined
  bool m_CreditTable = false; // Used to determine which this is for auto filling other tables

  std::map<int, Entry> m_Entries; // Holds all entries, sorting from lowest id to highest
};

}
