#pragma once


#include "LedgerPopup.h"
#include "StatementPopup.h"

namespace Ferret {

enum class PopupType {
  NONE = 0,
  Ledger,
  Journal,
  Statement,
};

class Popup {
public:
  // Renders the desired popup, if none is set it won't render a thing
  void Render();

  // Sets the RenderPopup State
  void SetRenderPopup(const PopupType &popup) { m_PopupType = popup; }

  inline void LedgerViewEntry(const int &tableID, const int &entryID, const bool &isEntryCredit) {
    m_LedgerPopup.ViewEntry(tableID, entryID, isEntryCredit);
    m_PopupType = PopupType::Ledger;
  }

  inline void LedgerViewTable(const int &tableID) {
    m_LedgerPopup.ViewTable(tableID);
    m_PopupType = PopupType::Ledger;
  }

  inline void StatementViewTable(const uint64_t &tableHash) {
    m_StatementPopup.ViewStatementTable(tableHash);
    m_PopupType = PopupType::Statement;
  }

private:
  PopupType m_PopupType = PopupType::NONE;

  LedgerPopup m_LedgerPopup;
  StatementPopup m_StatementPopup;


private:
  friend class FerretLayer;
};

}
