#pragma once

#include "FerretApp/Table/Table.h"
#include "FerretApp/Table/DataSet.h"

namespace Ferret {

//
//  Used as a storage medium between the Ledger and Statements class
//   which stores the accounts name, id / number, and the balance of
//   the account and is used to display it as needed per account
//
struct AccountTableStatementData {
  std::string Name;
  int Account;
  float Amount;
};

//
//  Used to render the Statements that are currently implemented
//
class Statements {
public:
  Statements();
  // Renders all Statements to the screen
  void OnRenderData();

  // Used to let the class object know there new date in an Income or
  //  Expense account
  void NewExpenseOrIncomeDataAvailable();

  // Used to let the class object know there new date in an Cash or
  //  Other Asset account
  void NewCashDataAvailable();

  // Used to let the class object know there new date in a Short
  //  Term Liability or Long Term Liability account
  void NewLiabilityDataAvailable();

  // This simply adds the amount to the beginning balance
  void UpdateBeginningBalance(const float &amount) { m_BeginningBalance += amount; }

  const Table &GetTable(const uint64_t &id) const { return m_TableMap.at(id); }
  Table &GetTable(const uint64_t &id) { return m_TableMap.at(id); }

  const DataSet &GetDataSet(const uint64_t &id) const { return m_DataSetMap.at(id); }
  DataSet &GetDataSet(const uint64_t &id) { return m_DataSetMap.at(id); }


private:
  // Renders the accounts given for the income statement; Used by
  //  OnRenderIncomeStatement();
  void RenderIncomeStatementTables(const std::string &tableName, const std::vector<AccountTableStatementData> &accounts, const float &tableTotal);

  // Renders the Income Statement to the screen, using the Income and
  //  Expense Accounts and Totals
  void OnRenderIncomeStatement();

  // Renders the Retained Earnings to the screen, using the Beginning
  //  Balance and Retained Earnings from the period
  void OnRenderRetainedEarnings();

  // Renders the accounts given for the balance statement; Used by
  //  OnRenderBalanceStatement();
  void RenderBalanceStatementTables(const std::string &tableName, const std::vector<AccountTableStatementData> &accounts);

  // Renders the Balance Statement to the screen, using the Cash, Other Assets,
  //  Short Term Liability and Long Term Liability Accounts and Totals
  void OnRenderBalanceStatement();
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

  std::vector<uint64_t> m_RootTableHashes;

  std::unordered_map<TableHash, Table> m_TableMap;
  std::unordered_map<DataSetHash, DataSet> m_DataSetMap;

};

}
