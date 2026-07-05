#pragma once

namespace Ferret {

struct AccountTableStatementData {
  std::string Name;
  int Account;
  float Amount;
};

class Statements {
public:
  static void OnRenderData();
  static void NewDataAvailable();

  // This simply adds the amount to the beginning balance
  static void UpdateBeginningBalance(const float &amount) { m_BeginningBalance += amount; }

private:
  static void OnRenderIncome();
  static void OnRenderRetainedEarnings();
private:
  static inline float m_RowHeight = 0;
  static inline float m_TypeColWidth = 0;
  static inline float m_AccountColWidth = 0;
  static inline float m_AmountColWidth = 0;

  static inline float m_IncomeAccountsTotal = 0;
  static inline float m_ExpenseAccountsTotal = 0;

  static inline float m_BeginningBalance = 0;
  static inline float m_RetainedEarnings = 0;

  static inline std::vector<AccountTableStatementData> m_IncomeAccounts;
  static inline std::vector<AccountTableStatementData> m_ExpenseAccounts;
};

}
