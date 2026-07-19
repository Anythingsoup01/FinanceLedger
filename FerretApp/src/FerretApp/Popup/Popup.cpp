#include "Popup.h"

#include "Ferret/Core/Application.h"
#include "Ferret/Core/Input.h"

#include "FerretApp/Layer/FerretLayer.h"
#include "FerretApp/Ledger/Ledger.h"

#include "FerretApp/Utils/Utils.h"

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
