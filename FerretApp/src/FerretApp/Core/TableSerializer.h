#pragma once

#include "Table.h"

namespace Ferret {

class TableSerializer {
public:
  // Used to serialize the given tables to a given file path
  static void Serialize(const std::map<int, AccountTable> &tables, const std::filesystem::path &filePath);

  // Used to deserialize tables from the given file path
  static bool Deserialize(std::map<int, AccountTable> *tables, const std::filesystem::path &filePath);

};

}
