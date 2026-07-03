#include "Entry.h"

#include <imgui.h>

namespace Ferret {

bool Entry::RenderEntry() {
  bool editIntent = false;
  ImGui::TableSetColumnIndex(0);
  ImGui::Text("%i/%i/%i", m_Date.Month, m_Date.Day, m_Date.Year);
  if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0)) {
    editIntent = true;
  }

  ImGui::TableSetColumnIndex(1);
  ImGui::Text("%i", m_Account);
  if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0)) {
    editIntent = true;
  }

  ImGui::TableSetColumnIndex(2);
  ImGui::Text("$%.2f", m_Amount);
  if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0)) {
    editIntent = true;
  }

  return editIntent;
}

}
