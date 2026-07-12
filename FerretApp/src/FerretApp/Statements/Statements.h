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
  void NewExpenseOrIncomeDataAvailable();
  void NewCashDataAvailable();
  void NewLiabilityDataAvailable();

  // This simply adds the amount to the beginning balance
  void UpdateBeginningBalance(const float &amount) { m_BeginningBalance += amount; }

private:
  void OnRenderIncome();
  void OnRenderRetainedEarnings();
  void OnRenderBalance();
private:
  float m_IncomeAccountsTotal = 0;
  float m_ExpenseAccountsTotal = 0;

  float m_CashAccountsTotal = 0;
  float m_OtherAssetAccountsTotal = 0;

  float m_ShortTermLiabilityAccountsTotal = 0;
  float m_LongTermLiabilityAccountsTotal = 0;

  float m_BeginningBalance = 0;
  float m_RetainedEarnings = 0;

  std::vector<AccountTableStatementData> m_IncomeAccounts;
  std::vector<AccountTableStatementData> m_ExpenseAccounts;

  std::vector<AccountTableStatementData> m_CashAccounts;
  std::vector<AccountTableStatementData> m_OtherAssetsAccounts;

  std::vector<AccountTableStatementData> m_ShortTermLiabilityAccounts;
  std::vector<AccountTableStatementData> m_LongTermLiabilityAccounts;

};

}
