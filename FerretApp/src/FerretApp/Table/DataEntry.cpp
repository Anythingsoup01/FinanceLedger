#include "DataEntry.h"

#include <imgui.h>

namespace Ferret {

void DataEntry::Render() {
  ImGui::TableSetColumnIndex(1);
  ImGui::Text("*  %s (%d)", m_Name.c_str(), m_Account);
  ImGui::TableSetColumnIndex(2);
  ImGui::Text("%.2f", m_Amount);
  ImGui::TableNextRow();
}

}
