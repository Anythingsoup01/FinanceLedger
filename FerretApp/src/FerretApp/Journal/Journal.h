#pragma once

#include "FerretApp/Ledger/Entry.h"

namespace Ferret {

struct JournalEntry {
  std::vector<int> DebitEntryIDs; // Accounts that will be debitted
  int CreditAccountID; // The table that will be creddited
};

struct JournalDateTable {
  std::string DateString;
  std::map<int, JournalEntry> Entries;
};

class Journal {
public:
  void OnRenderData();

  void NewDataAvailable(const Date &date, const int &tableID);
private:
  void OnRenderJournalDateTable(const JournalDateTable &table);

  void OnRenderJournalEntry(const JournalEntry &entry);

private:
  std::map<int, JournalDateTable> m_JournalEntryMap;
};

}
