#pragma once

#include "DataEntry.h"

#include "FerretApp/DataTypes/AccountTable.h"

// This class automatically generates a table with 3 columns
namespace Ferret {
class DataSet {
public:
  DataSet(const std::string &header, const std::string &parentLegalName, const uint64_t &parentHash, const TableTracking &tracking, const bool &incrementsTotal);

  void Render();

  void NewDataAvailable();

  // INTERNAL USE ONLY
  //
  // Used by statements to change the name and reload the hash
  // for rendering.
  //
  // Users must not utilize this in any capacity as it may cause
  // crashes and/or unexpected behavior.
  void SetName(const std::string &name, const std::string &parentName);

  // INTERNAL USE ONLY
  //
  // Used to set the internal parent hash for pushing updates.
  //
  // Users must not utilize this in any capacity as it may cause
  // crashes and/or unexpected behavior.
  void SetParentHash(const uint64_t &parentHash);

  const uint64_t &GetHash() const { return m_Hash; }

  const std::string &GetName() const { return m_Header; }

  void SetIncrementsTotal(const bool &increments) { m_IncrementsTotal = increments; }
  const bool &GetIncrementsTotal() const { return m_IncrementsTotal; }

  void SetTracking(const TableTracking &tracking) { m_Tracking = tracking; NewDataAvailable(); }
  const TableTracking &GetTracking() const { return m_Tracking; }

  const float &GetTotal() const { return m_Total; }
private:
  std::string             m_Header;
  std::string             m_ParentLegalName;
  uint64_t                m_Hash;
  uint64_t                m_ParentHash; // Used to update the parent when new data has been placed in the set
  uint16_t                m_RowPadding;
  float                   m_Total;
  bool                    m_IncrementsTotal; // Internally makes the account add to the total or subtract.
  std::vector<DataEntry>  m_Entries;
  TableTracking           m_Tracking;
};

}
