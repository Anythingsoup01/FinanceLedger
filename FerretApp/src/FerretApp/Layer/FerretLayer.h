#pragma once

#include "Ferret/Core/Event/Event.h"
#include "Ferret/Core/Event/KeyEvent.h"
#include "Ferret/Core/Event/ApplicationEvent.h"
#include "Ferret/Layer/Layer.h"

#include "FerretApp/Ledger/Ledger.h"

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

  bool IsSavePathValid() { return m_SavePath.empty(); }
private:
  // OnWindowClose override for Ferret
  bool OnWindowClose(WindowCloseEvent &e);

  bool OnKeyPressedEvent(KeyPressedEvent &e);
private:
  enum class RenderState {
    Ledger = 0,
    Journal,
    Statements
  };
  RenderState m_State = RenderState::Ledger;
private:
  Ledger m_LedgerView;

  std::filesystem::path m_SavePath; // The path we save to
  std::filesystem::path m_TempLoadPath; // The path we want to swap to in the event a popup happens

  inline static FerretLayer *s_Instance = nullptr;
};

} // namespace Ferret
