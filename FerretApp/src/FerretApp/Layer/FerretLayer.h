#pragma once

#include "Ferret/Core/Event/Event.h"
#include "Ferret/Core/Event/KeyEvent.h"
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

  bool OnKeyPressedEvent(KeyPressedEvent &e);

  inline static FerretLayer *Get() { return s_Instance; }

  const std::map<int, AccountTable> &GetTables() const { return m_Tables; }

  void SubmitEntryDataToTable(const int &toTable, const bool &isCredit, const Date_t &date, const int &fromTable, const float &amount);

private:
  // Serializes AccountTable to disk
  void SerializeTables(YAML::Emitter &out, AccountTable *table);

  // Deserializes all accounts saved an disk
  bool DeserializeTables();

  // When creating a table we should reload each tables' m_Next pointer
  void ReloadTables();
private:
  std::map<int, AccountTable> m_Tables;
  std::vector<std::string> m_TableNames; // Get's reloaded everytime we call ReloadTables(); Contains a list of all table names (along with the AccountNumber)
  bool m_RenderCreateTable = false; // TODO: Make this an enum for overwriting and summing entries as well

  inline static FerretLayer *s_Instance = nullptr;
};

} // namespace Ferret
