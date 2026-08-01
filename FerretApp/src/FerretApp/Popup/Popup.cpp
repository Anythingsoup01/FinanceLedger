#include "Popup.h"

#include <imgui.h>

extern ImVec2 g_EntrySize;
extern ImVec2 g_GenericTableSize;

namespace Ferret {

void Popup::Render() {
  switch (m_PopupType) {
    case PopupType::Ledger: {
      m_LedgerPopup.Render();
      break;
    }
    case PopupType::Journal: {
      break;
    }
    case PopupType::Statement: {
      m_StatementPopup.Render();
    }
    default: break;
  }
}

}
