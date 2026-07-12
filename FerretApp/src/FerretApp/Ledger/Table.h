#pragma once

#include <imgui.h>

#include "EntryTable.h"

namespace Ferret {

// TODO: Allow Users to define custom tracking and set what tracking is for what table,
// FUTURE THOUGHT: Allow the user to create custom tables and allot them as they want, I.e. creating a custom Income Statement table and formatting it
enum class TableTracking {
  Untracked = 0,
  Cash,
  OtherAsset,
  ShortTermLiabilities,
  LongTermLiabilities,
  Income,
  Expenses,
  MAX_ITEM // Leave at bottom
};

// Used to convert a string to a TableTracking enum
TableTracking StringToTableTracking(const std::string &trackStr);

// Used to convert a TableTracking enum to a string
std::string TableTrackingToString(const TableTracking &tracking);

// This class will essentially contain all the account data
// and be used as a layer between the data and the renderer
class AccountTable {
public:
  AccountTable(const std::string &accountName, const int &accountNumber, const bool &isCredit, const TableTracking &tracking);
  ~AccountTable() = default;

  // Used to retrieve the Debit EntryTable_t as a const pointer
  const EntryTable *GetDebitTable() const { return &m_DebitTable; }

  // Used to retrieve the Credit EntryTable_t as a const pointer
  const EntryTable *GetCreditTable() const { return &m_CreditTable; }

  // This will set the Next variable
  inline void SetNext(AccountTable *next) { m_Next = next; }

  // This will get the Next variable
  inline AccountTable *GetNext() { return m_Next; }

  const std::string &GetName() const { return m_Name; }

  void SetName(const std::string &name) { m_Name = name; }

  const int &GetAccountNumber() const { return m_Number; }

  const bool &IsCreditAccount() const { return m_CreditAccount; }

  void SetCreditAccount(const bool &isCredit) { m_CreditAccount = isCredit; }

  const TableTracking &GetTracking() const { return m_Tracking; }

  inline void SetTracking(const TableTracking &tracking) { m_Tracking = tracking; }

  // This will draw the entire table
  void Draw();

  void InsertEntry(const bool &isCredit, const Date &date, const int &accountID, const float &amount, bool updateOther, const std::string &journalEntry = "Insert Entry");

  // Loops over both credit and debit tables removing entries with a given table id
  void RemoveEntriesFromTable(const int &tableID);

  // Removes a specific entry from the required table
  void RemoveEntry(const bool &isCredit, const int &entryID);

  // Used to retrieve entry data
  Entry &GetEntry(const bool &isCredit, const int &entryID);

  const float GetBalance() const;

private:
  // Used by both Draw and DrawIndividual to actually render the table
  void DrawHelper();

  // This is used internally to set the table height to the largest subTable height
  void ResizeTable();
private:
  int m_Number;                   // Must be unique
  std::string m_Name;             // Will display in Account Dropdown
  bool m_CreditAccount;           // If this is true then the final calculation for will be credit - debit, otherwise debit - credit
  TableTracking m_Tracking;       // Holds whether the account is an expense, income, or untracked account;
  EntryTable m_DebitTable;        // Holds all debit entries and value
  EntryTable m_CreditTable;       // Holds all credit entries and value
  ImVec2 m_TableSize;             // Holds the current size of the table
  AccountTable *m_Next = nullptr; // Used to get the next table id (for now);
};

}
