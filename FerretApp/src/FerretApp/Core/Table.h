#pragma once

#include <imgui.h>

namespace Ferret {

typedef struct Date {
  int Month;
  int Day;
  int Year;

  Date() = default;

  Date(int month, int day, int year)
    : Month(month), Day(day), Year(year) {}

} Date_t;

typedef struct EntryData {
public:
  EntryData() = default;
  EntryData(const Date_t &date, int accountID, float amount)
    : m_Date(date), m_AccountID(accountID), m_DollarAmount(amount) {}

  const Date_t &GetDate() const { return m_Date; }
  void SetDate(const Date_t &date) { m_Date = date; }

  const int &GetAccountID() const { return m_AccountID; }
  void SetAccountID(const int &accountID) { m_AccountID = accountID; }

  const float &GetAmount() const { return m_DollarAmount; }
  void SetAmount(const float &amount) { m_DollarAmount = amount; }

private:
  Date_t m_Date; // Holds the month, day, and year
  int m_AccountID; // Value used to denote where the amount is going to or coming from
  float m_DollarAmount; // Value used to denote how much is being inserted or removed
} EntryData_t;


typedef struct EntryTable {
public:
  EntryTable(int accountID, bool creditTable);

  // Inserts a new data entry and optionally updates the referenced table.
  //
  // Returns false when trying to overlap entries, ie. same date and account.
  // If you want to utilize this as an overwrite use OverwriteEntryData,
  // else, if you want to add to the already existing entry, use InsertSumEntryData.
  bool InsertEntryData(const Date_t &date, const int &accountID, const float &amount, bool updateOther);

  // This utilizes the buffers found in the private fields and preserves
  // the location in the map.
  //
  // Returns false when trying to overwrite non-existing data, use
  // InsertEntryData instead.
  bool OverWriteEntryData();

  // This utilizes the buffers found in the private fields and preserves
  // the location in the map.
  //
  // Returns false when trying to Sum with non-existing data, use
  // InsertEntryData instead.
  bool InsertSumEntryData();

  // This utilizes the id of the selected entry to remove it from the entry list
  void RemoveEntryData(int id);

  // Used to completely remove entries from the given table id
  void RemoveEntriesFromTable(const int &tableID);

  // This returns the internal entries as a const map<int, EntryData_t> pointer
  const std::map<int, EntryData_t> &GetEntries() const { return m_Entries; }

  // This returns the internal non-const pointer to the Date buffer
  // used for when we want to add, overwrite, or sum an entry
  Date_t *GetDateBuffer() { return &m_DateBuffer; }

  // This returns the internal non-const pointer to the AccountID buffer
  // used for when we want to add, overwrite, or sum an entry
  int *GetAccountIDBuffer() { return &m_AccountIDBuffer; }

  // This returns the internal non-const pointer to the Amount buffer
  // used for when we want to add, overwrite, or sum an entry
  float *GetAmountBuffer() { return &m_AmountBuffer; }

  // This is used to return the total value of the entry table
  const float &GetTotalValue() { return m_TotalValue; }

private:
  Date_t m_DateBuffer;
  int m_AccountIDBuffer;
  float m_AmountBuffer;

  int m_AccountID;
  float m_TotalValue; // Holds the total value (positive)
  bool m_CreditTable; // Used to determine which entry table from which account to automatically update
  std::map<int, EntryData_t> m_Entries; // Holds all entries
} EntryTable_t;

// This class will essentially contain all the account data
// and be used as a layer between the data and the renderer
class AccountTable {
public:
  AccountTable() = default;
  AccountTable(const std::string &accountName, const int &accountNumber, const bool &isCredit);
  ~AccountTable() = default;

  // Used to retrieve the Debit EntryTable_t as a const pointer
  const EntryTable_t *GetDebitTable() const { return &m_DebitTable; }
  // Used to externally add an entry to the table, used with the serializer and (going to be used) with automatic entry insertion
  void InsertDebitEntry(const Date_t &date, const int &accountID, const float &amount, bool updateOther);

  // Used to retrieve the Credit EntryTable_t as a const pointer
  const EntryTable_t *GetCreditTable() const { return &m_CreditTable; }
  // Used to externally add an entry to the table, used with the serializer and (going to be used) with automatic entry insertion
  void InsertCreditEntry(const Date_t &date, const int &accountID, const float &amount, bool updateOther);

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

private:
  // Used by both Draw and DrawIndividual to actually render the table
  void DrawHelper();

  // Used to draw the entries tables
  void DrawSubTable(EntryTable_t *table, const char *tableName, const int &tableIndex);
  void DrawSubTableTotal(EntryTable_t *table, const char *tableName, const int &tableIndex);

  // This is used internally to set the table height to the largest subTable height
  void ResizeTable();
private:
  int m_Number;                 // Must be unique
  std::string m_Name;           // Will display in Account Dropdown
  bool m_CreditAccount;         // If this is true then the final calculation for will be credit - debit, otherwise debit - credit
  EntryTable_t m_DebitTable;    // Holds all debit entries and value
  EntryTable_t m_CreditTable;   // Holds all credit entries and value
  ImVec2 m_TableSize;           // Holds the current size of the table
  AccountTable *m_Next = nullptr; // Used to get the next table id (for now);
};

}
