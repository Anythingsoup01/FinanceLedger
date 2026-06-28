#pragma once

#include "Ferret/Core/Event/Event.h"
#include "Ferret/Core/Event/KeyEvent.h"
#include "Ferret/Layer/Layer.h"

#include "FerretApp/Core/Table.h"
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
  // TODO: Make a proper renderer to display multiple of these
  AccountTable m_Table;
  AccountTable m_Table2;
};

} // namespace Ferret
