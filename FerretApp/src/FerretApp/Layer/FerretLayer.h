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

private:
  void SerializeTables(YAML::Emitter &out, AccountTable *table);
  bool DeserializeTables();
private:
  std::map<int, AccountTable> m_Tables;
};

} // namespace Ferret
