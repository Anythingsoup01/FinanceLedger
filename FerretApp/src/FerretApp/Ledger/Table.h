#pragma once

#include <imgui.h>

#include "EntryTable.h"

namespace Ferret {

enum class TableTracking {
  Untracked = 0,
  Income,
  Expenses,
};

// This class will essentially contain all the account data
// and be used as a layer between the data and the renderer
class AccountTable {
public:
  AccountTable() = default;
  AccountTable(const std::string &accountName, const int &accountNumber, const bool &isCredit);
  ~AccountTable() = default;

  // Used to retrieve the Debit EntryTable_t as a const pointer
  const EntryTable *GetDebitTable() const { return &m_DebitTable; }

  // Used to retrieve the Credit EntryTable_t as a const pointer
  const EntryTable *GetCreditTable() const { return &m_CreditTable; }

  void InsertEntry(const bool &isCredit, const Date &date, const int &accountID, const float &amount, bool updateOther);

  const int &GetAccountNumber() const { return m_Number; }
  const std::string &GetName() const { return m_Name; }
  const bool &IsCreditAccount() const { return m_CreditAccount; }

  // This will draw the entire table as an item with other tables,
  // rather than being it's own window
  void Draw();

  // This will set the Next variable
  inline void SetNext(AccountTable *next) { m_Next = next; }

  // This will get the Next variable
  inline AccountTable *GetNext() { return m_Next; }

  // Loops over both credit and debit tables removing entries with a given table id
  void RemoveEntriesFromTable(const int &tableID);

  // Removes a specific entry from the required table
  void RemoveEntry(const bool &isCredit, const int &entryID);

  // Used to retrieve entry data
  Entry &GetEntry(const bool &isCredit, const int &entryID);

private:
  // Used by both Draw and DrawIndividual to actually render the table
  void DrawHelper();

  // This is used internally to set the table height to the largest subTable height
  void ResizeTable();
private:
  int m_Number;                   // Must be unique
  std::string m_Name;             // Will display in Account Dropdown
  bool m_CreditAccount;           // If this is true then the final calculation for will be credit - debit, otherwise debit - credit
  EntryTable m_DebitTable;        // Holds all debit entries and value
  EntryTable m_CreditTable;       // Holds all credit entries and value
  ImVec2 m_TableSize;             // Holds the current size of the table
  AccountTable *m_Next = nullptr; // Used to get the next table id (for now);
};

}
