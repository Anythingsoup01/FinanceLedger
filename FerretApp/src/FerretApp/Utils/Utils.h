#pragma once

namespace Utils {

void HeaderCentered(uint32_t columnCount);

bool IsSubitted();

// Returns the digit count; Used for account sorting
int GetPositiveDigitCount(const int &val);

// Gets the very first int from a val; val = 123 : return 1
int GetTopDigit(const int &val);

}
