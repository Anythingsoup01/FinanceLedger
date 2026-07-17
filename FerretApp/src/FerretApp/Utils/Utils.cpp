#include "Utils.h"

#include "Ferret/Core/Input.h"

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

    if (strncmp(header_name, "##EmptyHeader", strlen("##EmptyHeader")) != 0) {
      ImGui::TextUnformatted(header_name);
    } else {
      ImGui::Dummy(ImVec2(0,0));
    }
  }
}

bool IsSubitted() {
  if (!ImGui::IsItemHovered())
    return false;

  return Ferret::Input::IsKeyPressed(Ferret::KeyCode::Enter) || Ferret::Input::IsKeyPressed(Ferret::KeyCode::KPEnter);
}

int GetDigitCount(const int &val) {
  if (val == 0) {
    return 1;
  }

  int digits = 0;
  int tmp = val;
  if (tmp < 0) {
    tmp *= -1;
    digits++; // Accounting for the '-' sign
  }
  while (true) {
    if (tmp < 10) {
      digits++;
      break;
    }
    digits++;
    tmp /= 10;
  }
  return digits;
}

int GetTopDigit(const int &val) {
  if (val == 0) {
    return 0;
  }
  int tmp = val;
  if (tmp < 0) {
    tmp *= -1;
  }
  while (true) {
    if (tmp < 10) {
      break;
    }
    tmp /= 10;
  }
  return tmp;
}

int CalculateEntryID(const Ferret::Date &date, const int &accountID) {
  const int LARGEST_ACCOUNT_NUMBER = 999999;
  const int DAY_WEIGHT = LARGEST_ACCOUNT_NUMBER + 1;
  return (date.Day * DAY_WEIGHT) + date.Month + date.Year + LARGEST_ACCOUNT_NUMBER - accountID;
}

uint64_t GenerateHash64(const std::string_view& str) {
  uint64_t hash = 14695981039346656037ull;
  for (unsigned char c : str) {
    hash ^= c;
    hash *= 1099511628211ull;
  }
  return hash;
}


}
