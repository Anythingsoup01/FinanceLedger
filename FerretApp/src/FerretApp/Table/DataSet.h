#pragma once

#include "DataEntry.h"

#include "FerretApp/DataTypes/AccountTable.h"

// This class automatically generates a table with 3 columns
namespace Ferret {
class DataSet {
public:
  DataSet(const std::string &header, const std::string &parentName, const TableTracking &tracking);

  void Render();

  void NewDataAvailable();

  void SetIncrementsTotal(const bool &increments) { m_IncrementsTotal = increments; }
  const bool &GetIncrementsTotal() const { return m_IncrementsTotal; }

  const float &GetTotal() const { return m_Total; }
private:
  std::string             m_Header;
  uint64_t                m_Hash;
  uint64_t                m_ParentHash; // Used to update the parent when new data has been placed in the set
  uint16_t                m_RowPadding;
  float                   m_Total;
  bool                    m_IncrementsTotal; // Internally makes the account add to the total or subtract.
  std::vector<DataEntry>  m_Entries;
  TableTracking           m_Tracking;
};

}
