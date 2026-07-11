#pragma once

namespace Ferret {

struct AccountTableStatementData {
  std::string Name;
  int Account;
  float Amount;
};

class Statements {
public:
  void OnRenderData();
  void NewDataAvailable();

  // This simply adds the amount to the beginning balance
  void UpdateBeginningBalance(const float &amount) { m_BeginningBalance += amount; }

private:
  void OnRenderIncome();
  void OnRenderRetainedEarnings();
private:
  float m_RowHeight = 0;
  float m_TypeColWidth = 0;
  float m_AccountColWidth = 0;
  float m_AmountColWidth = 0;

  float m_IncomeAccountsTotal = 0;
  float m_ExpenseAccountsTotal = 0;

  float m_BeginningBalance = 0;
  float m_RetainedEarnings = 0;

  std::vector<AccountTableStatementData> m_IncomeAccounts;
  std::vector<AccountTableStatementData> m_ExpenseAccounts;
};

}
