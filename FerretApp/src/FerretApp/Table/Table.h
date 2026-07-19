#pragma once

namespace Ferret {

enum class ElementType {
  NONE = 0,
  Table,
  TableTotal,
  DataSet,
  String,
  MAX_ITEM // Keep Last
};

ElementType StringToElementType(const std::string &typeStr);

std::string ElementTypeToString(const ElementType &type);

struct ElementData {
  uint64_t ElementHash; // IF 0 THEN IT'S BLANK
  ElementType Type;
};

class Table {
public:
  Table() = default;
  Table(const std::string &name, const std::string &parentName, const uint64_t &parentHash);

  // This will draw the root table node, the title will be centered,
  // and it will also draw a table with the internal column count with no
  // headers right under
  void Render();

  const uint64_t &GetTableHash() const { return m_Hash; }

  void UpdateTotal() const;

  // INTERNAL USE ONLY
  //
  // Used by statements to change the name and reload the hash
  // for rendering.
  //
  // Users must not utilize this in any capacity as it may cause
  // crashes and/or unexpected behavior.
  void SetName(const std::string &name, const std::string &parentLegalName);

  // INTERNAL USE ONLY
  //
  // Used to set the internal parent hash for pushing updates.
  //
  // Users must not utilize this in any capacity as it may cause
  // crashes and/or unexpected behavior.
  void SetParentHash(const uint64_t &parentHash);

  const uint64_t &GetParentHash() const { return m_ParentHash; }

  const uint64_t &GetHash() const { return m_Hash; }

  const std::string &GetName() const { return m_Name; }

  const std::string &GetParentLegalName() const { return m_ParentLegalName; }

  const uint32_t &GetCols() const { return m_Cols; }

  const float &GetTotal() const { return m_TableTotal; }

  const uint32_t GetElementCount() const { return m_Elements.size(); }

  const std::unordered_map<int32_t, ElementData> &GetElements() const { return m_Elements; }
  const ElementData &GetElement(const int32_t &index) const;
public:
  //////////////////////////////
  /// Element Manipulation ////
  ////////////////////////////

  int32_t GetElementIdx(const uint64_t &elementHash);

  void ReplaceElement(const uint64_t &oldHash, const uint64_t &newHash, const ElementType &type);

  void RemoveElement(const uint64_t &hash);

  void RemoveElement(const int32_t &index);

  void AddElement(const int32_t &index, const uint64_t &hash, const ElementType &type);

  //////////////////////////////
  /// Table Operations ////////
  ////////////////////////////

  // Adds n columns to the table
  void AddColumn(const int32_t &n);

  // Removes n columns from the given index, going to the right
  void RemoveColumn(const int32_t &idx, const int32_t &n);

  // Element count is capped to be a multiple of m_Cols
  void RecalculateElementCount();
private:
  // This will setup the table with the correct amount of columns,
  // called by Render
  void RenderDrawTable();
private:
  std::string m_Name;
  std::string m_ParentLegalName;
  uint32_t    m_Cols;
  float       m_TableTotal;
  uint64_t    m_Hash;
  uint64_t    m_ParentHash;

  // The data sets below are treated like a render order which goes off
  // an independent count based on the total element count
  std::unordered_map<int32_t, ElementData> m_Elements;
};

}
