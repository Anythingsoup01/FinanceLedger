#pragma once

namespace Ferret {

struct Date {
  int Day = 0;
  int Month = 0;
  int Year = 0;

  Date() = default;
  Date(int day, int month, int year)
    : Day(day), Month(month), Year(year) {}

  int GetDateID() { return Day + Month + Year; }
  const int GetDateID() const { return Day + Month + Year; }
};

class Entry {
public:
  Entry() = default;
  Entry(const Date &date, const int &account, const float &amount)
    : m_Date(date), m_Account(account), m_Amount(amount) {}

  const Date &GetDate() const { return m_Date; }
  void SetDate(const Date &date) { m_Date = date; }

  const int &GetAccountID() const { return m_Account; }
  void SetAccountID(const int &accountID) { m_Account = accountID; }

  const float &GetAmount() const { return m_Amount; }
  void SetAmount(const float &amount) { m_Amount = amount; }

  // Generates the entry id so we can easily access it later
  int32_t GetEntryID() { return m_Date.GetDateID() + m_Account; }

  // Used to render an entry, returns whether or not it's been double clicked
  // as a entry detailed view intent
  bool RenderEntry();
private:
  Date m_Date;
  int m_Account;
  float m_Amount;
};

}
