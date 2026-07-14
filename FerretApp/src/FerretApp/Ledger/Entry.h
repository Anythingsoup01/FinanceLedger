#pragma once

namespace Ferret {

// Used to time keep each ledger and journal entry
struct Date {
  int Day = 0;
  int Month = 0;
  int Year = 0;

  Date() = default;
  Date(int month, int day, int year) {
    Month = month > 12 ? 12 : month < 0 ? 0 : month;
    Day = day > 31 ? 31 : day < 1 ? 1 : day;
    Year = year > 3000 ? 3000 : year < 0 ? 0 : year;
  }

  // Generates a DateID used by primarily the journal
  int GetDateID() { return Day + Month + Year; }
};

// Used to keep track of each entry provided to the ledger and journal
class Entry {
public:
  Entry() = default;
  Entry(const Date &date, const int &account, const float &amount, const std::string &journalEntry)
    : m_Date(date), m_Account(account), m_Amount(amount), m_JournalEntry(journalEntry) {}

  // Gets the Date of the entry
  const Date &GetDate() const { return m_Date; }

  // Gets the Account the entry is going to
  const int &GetAccountID() const { return m_Account; }

  // Gets the Amount the entry is
  const float &GetAmount() const { return m_Amount; }

  // Gets the Journal Entry for the entry
  const std::string &GetJournalEntry() const { return m_JournalEntry; }

  // Generates the entry id so we can easily access it later
  int32_t GetEntryID() { return m_Date.GetDateID() + m_Account; }

  // Used to render an entry, returns whether or not it's been double clicked
  // as a entry detailed view intent
  bool RenderEntry();
private:
  Date m_Date;
  int m_Account;
  float m_Amount;
  std::string m_JournalEntry;
};

}
