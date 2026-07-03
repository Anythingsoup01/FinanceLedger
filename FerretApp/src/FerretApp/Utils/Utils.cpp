#include "Utils.h"

#include <imgui.h>

namespace Utils {

void HeaderCentered(uint32_t columnCount) {
  ImGui::TableNextRow(ImGuiTableRowFlags_Headers);
  for (int column = 0; column < columnCount; column++) {
    ImGui::TableSetColumnIndex(column);
    const char* header_name = ImGui::TableGetColumnName(column);

    // Calculate dimensions
    float column_width = ImGui::GetContentRegionAvail().x;
    float text_width = ImGui::CalcTextSize(header_name).x;

    // Offset the cursor position to center the text
    if (column_width > text_width) {
      ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (column_width - text_width) * 0.5f);
    }

    ImGui::TableHeader(header_name);
  }
}

bool IsSubitted() {
  if (!ImGui::IsItemHovered())
    return false;

  return ImGui::IsKeyDown(ImGuiKey_Enter) || ImGui::IsKeyDown(ImGuiKey_KeypadEnter);
}

// Returns the digit count; Used for account sorting
int GetPositiveDigitCount(const int &val) {
  int digits = 0;
  int tmp = val;
  while (true) {
    if (tmp < 10 && tmp > 0) {
      digits++;
      break;
    }
    digits++;
    tmp /= 10;
  }
  return digits;
}

// Gets the very first int from a val; val = 123 : return 1
int GetTopDigit(const int &val) {
  int tmp = val;
  while (true) {
    if (tmp < 10 && tmp > 0) {
      break;
    }
    tmp /= 10;
  }
  return tmp;
}


}
