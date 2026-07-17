#pragma once

namespace Ferret {

using TableHash = uint64_t;
using DataSetHash = uint64_t;

class Table {
public:
  Table(const std::string &name, const std::string &parentName);

  // This will draw the root table node, the title will be centered,
  // and it will also draw a table with the internal column count with no
  // headers right under
  void Render();

  const uint64_t &GetTableHash() const { return m_Hash; }

  void UpdateTotal() const;
private:
  // This will setup the table with the correct amount of columns,
  // called by Render
  void RenderDrawTable();
private:
  std::string m_Name;
  uint32_t    m_Cols;
  float       m_TableTotal;
  uint32_t    m_ElementCount;
  uint64_t    m_Hash;
  uint64_t    m_ParentHash;

  // The data sets below are treated like a render order which goes off
  // an independent count based on the total element count
  std::unordered_map<uint32_t, DataSetHash> m_DataSets;
  std::unordered_map<uint32_t, TableHash> m_SubTables;
};

}
