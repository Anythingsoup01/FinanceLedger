#pragma once

#include "FerretApp/DataTypes/Entry.h"

namespace Utils {

void HeaderCentered(uint32_t columnCount);

bool IsSubitted();

// Returns the digit count; Used for account sorting
int GetDigitCount(const int &val);

// Gets the very first int from a val; val = 123 : return 1
int GetTopDigit(const int &val);

// Calculates what an entry id should be based on the given data
int CalculateEntryID(const Ferret::Date &date, const int &accountID);

uint64_t GenerateHash64(const std::string_view& str);

}
