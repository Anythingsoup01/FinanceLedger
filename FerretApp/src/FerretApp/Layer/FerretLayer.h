#pragma once

#include "Ferret/Core/Event/Event.h"
#include "Ferret/Core/Event/KeyEvent.h"
#include "Ferret/Core/Event/ApplicationEvent.h"
#include "Ferret/Layer/Layer.h"

#include "FerretApp/Ledger/Ledger.h"
#include "FerretApp/Statements/Statements.h"
#include "FerretApp/Journal/Journal.h"

#include <yaml-cpp/yaml.h>

namespace Ferret {

class FerretLayer : public Layer {
public:
  virtual void OnAttach() override;
  virtual void OnDetach() override;

  virtual void OnUpdate() override;
  virtual void OnUIRender() override;
  virtual void OnEvent(Event &event) override;

  inline static FerretLayer &Get() { return *s_Instance; }

  void SetContextDirty(const bool &dirty) { m_ContextDirty = dirty; }

  const Ledger &GetLedger() const { return m_Ledger; }
  Ledger &GetLedger() { return m_Ledger; }

  const Statements &GetStatements() const { return m_Statements; }
  Statements &GetStatements() { return m_Statements; }

  const Journal &GetJournal() const { return m_Journal; }
  Journal &GetJournal() { return m_Journal; }

private:
  // Opens a save file from a given path
  void Open(const std::filesystem::path &path);

  // Opens a save file from the stored temp path
  void OpenAtTmpPath();

  // Prompts the user to select a save file
  void Open();

  // Saves all data to the stored path
  void Save();

  // Saves all data to a new path
  void SaveAs();

  bool IsSavePathValid() { return !m_SavePath.empty(); }
private:
  // OnWindowClose override for Ferret
  bool OnWindowClose(WindowCloseEvent &e);

  bool OnKeyPressedEvent(KeyPressedEvent &e);
private:
  enum class RenderState {
    Ledger = 0,
    Journal,
    Statements,
    MAX_VALUE // Keep Last
  };
  RenderState m_State = RenderState::Ledger;
private:
  std::filesystem::path m_SavePath; // The path we save to
  std::filesystem::path m_TempLoadPath; // The path we want to swap to in the event a popup happens

  // TODO: If we ever move toward multi ledger updating, with complete journal entries and statements, we need a class / struct to contain them together
  Ledger m_Ledger;
  Statements m_Statements;
  Journal m_Journal;
  bool m_ContextDirty = false;

  inline static FerretLayer *s_Instance = nullptr;
private:
  friend class Popup;
};

} // namespace Ferret
