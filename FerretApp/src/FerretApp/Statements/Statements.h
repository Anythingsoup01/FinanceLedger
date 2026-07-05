#pragma once

namespace Ferret {

class Statements {
public:
  static void OnRenderData();
private:
  static void OnRenderIncome();
private:
  static inline bool m_RenderIncome = true;
};

}
