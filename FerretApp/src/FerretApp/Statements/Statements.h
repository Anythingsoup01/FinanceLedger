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

  const Table &GetTable(const uint64_t &hash) const { return m_TableMap.at(hash); }
  Table &GetTable(const uint64_t &hash) { return m_TableMap.at(hash); }

  const bool TableExists(const uint64_t &hash) const { return m_TableMap.find(hash) != m_TableMap.end(); }

  void ReplaceTable(const uint64_t &oldHash, const std::string &newName, const std::string &newParentLegalName, const uint64_t &parentHash);
  
  void RemoveTable(const uint64_t &hash, const uint64_t &parentHash);

  // Used to create a table at a given index, if idx is 0 and parentHash is 0, it's considered a root table
  void AddTable(const int32_t &index, const std::string &name, const std::string &parentLegalName, const uint64_t &parentHash);

  const DataSet &GetDataSet(const uint64_t &hash) const { return m_DataSetMap.at(hash); }
  DataSet &GetDataSet(const uint64_t &hash) { return m_DataSetMap.at(hash); }

  const bool DataSetExists(const uint64_t &hash) const { return m_DataSetMap.find(hash) != m_DataSetMap.end(); }

  void ReplaceDataSet(const uint64_t &oldHash, const std::string &newName, const std::string &newParentLegalName, const uint64_t &parentHash, const TableTracking &tracking, const bool &incrementsTable);

  void RemoveDataSet(const uint64_t &hash, const uint64_t &parentHash);

  void AddDataSet(const int32_t &index, const std::string &name, const std::string &parentLegalName, const uint64_t &parentHash, const TableTracking &tracking, const bool &incrementsTable);

  const std::string &GetString(const uint64_t &hash) const { return m_Strings.at(hash); }
  std::string &GetString(const uint64_t &hash) { return m_Strings.at(hash); }

  const bool StringExists(const uint64_t &hash) const { return m_Strings.find(hash) != m_Strings.end(); }

  void ReplaceString(const uint64_t &oldHash, const std::string &newString, const std::string &parentName, const uint64_t &parentHash);

  void RemoveString(const uint64_t &hash, const uint64_t &parentHash);

  void AddString(const int32_t &index, const std::string &newString, const std::string &parentLegalName, const uint64_t &parentHash);
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

  std::unordered_map<uint64_t, Table> m_TableMap;
  std::unordered_map<uint64_t, DataSet> m_DataSetMap;
  std::unordered_map<uint64_t, std::string> m_Strings;
};

}
