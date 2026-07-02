#pragma once

#include "Ferret/Core/Event/Event.h"
#include "Ferret/Core/Event/KeyEvent.h"
#include "Ferret/Core/Event/ApplicationEvent.h"
#include "Ferret/Layer/Layer.h"

#include "FerretApp/Core/Table.h"

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

  const std::map<int, AccountTable> &GetTables() const { return m_Tables; }

  // Used to autofill tables referenced when inserting an entry manually
  void SubmitEntryDataToTable(const int &toTable, const bool &isCredit, const Date_t &date, const int &fromTable, const float &amount);

  // Tells the system when the context needs saved
  void SetDirty() { m_ContextDirty = true; }

private:
  // Serializes AccountTable to disk
  void Serialize();
  void SerializeTables(YAML::Emitter &out, AccountTable *table);
  // Deserializes all accounts saved an disk
  bool Deserialize();

  // When creating a table we should reload each tables' m_Next pointer
  void ReloadTables();

  bool OnWindowClose(WindowCloseEvent &e);
  bool OnKeyPressedEvent(KeyPressedEvent &e);

  // Used to render the table creation window
  void RenderCreateTablePopup();
  // Used to render the save screen when trying to close with a
  // dirty context
  void RenderSavePopup();
private:
  enum class RenderPopup {
    NONE = 0,
    CreateTable,
    Save,
  };
  RenderPopup m_RenderPopup = RenderPopup::NONE;
private:
  std::map<int, AccountTable> m_Tables;
  std::vector<std::string> m_TableNames; // Get's reloaded everytime we call ReloadTables(); Contains a list of all table names (along with the AccountNumber)
  bool m_ContextDirty = false; // Used to tell if there is unsaved data

  inline static FerretLayer *s_Instance = nullptr;
};

} // namespace Ferret
