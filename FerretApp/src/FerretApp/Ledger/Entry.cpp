#include "Entry.h"

#include "FerretApp/Layer/FerretLayer.h"

#include <imgui.h>

namespace Ferret {

bool Entry::RenderEntry() {
  bool editIntent = false;
  ImGui::TableSetColumnIndex(0);
  ImGui::Text("%i/%i/%i", m_Date.Month, m_Date.Day, m_Date.Year);
  if (ImGui::IsItemHovered()) {
    ImGui::SetTooltip("Double Click to view Details");

    if (ImGui::IsMouseDoubleClicked(0)) {
      editIntent = true;
    }
  }

  ImGui::TableSetColumnIndex(1);
  char buf[32];
  const int &retainedEarningsID = FerretLayer::Get().GetLedger().GetRetainedEarningsTable().GetAccountNumber();
  if (retainedEarningsID == m_Account) {
    snprintf(buf, sizeof(buf), "Retained");
  } else {
    snprintf(buf, sizeof(buf), "%i", m_Account);
  }
  ImGui::TextUnformatted(buf);
  if (ImGui::IsItemHovered()) {
    ImGui::SetTooltip("Double Click to view Details");

    if (ImGui::IsMouseDoubleClicked(0)) {
      editIntent = true;
    }
  }

  ImGui::TableSetColumnIndex(2);
  ImGui::Text("$%.2f", m_Amount);
  if (ImGui::IsItemHovered()) {
    ImGui::SetTooltip("Double Click to view Details");

    if (ImGui::IsMouseDoubleClicked(0)) {
      editIntent = true;
    }
  }

  return editIntent;
}

}
