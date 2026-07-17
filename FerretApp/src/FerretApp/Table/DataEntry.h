#pragma once

namespace Ferret {

// This class strictly requires 3 columns to work with,
// may change in the future
class DataEntry {
public:
  DataEntry(const std::string &name, const int &account, const float &amount)
    : m_Name(name), m_Account(account), m_Amount(amount) {}

  void Render();

  const std::string &GetName() { return m_Name; }

  const int &GetAccount() { return m_Account; }

  const float &GetAmount() { return m_Amount; }

private:
  std::string m_Name;
  int m_Account;
  float m_Amount;
};

}
