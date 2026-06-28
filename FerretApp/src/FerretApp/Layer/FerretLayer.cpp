#include "FerretLayer.h"
#include "Ferret.h"
#include "imgui.h"

#include <yaml-cpp/yaml.h>

namespace Ferret {

void FerretLayer::OnAttach() {
  m_Table = AccountTable("Cash", 101, false);
  m_Table2 = AccountTable("Will Income", 402, false);
}

void FerretLayer::OnDetach() {
  YAML::Emitter out;

  out << YAML::Key << "Accounts" << YAML::Value << YAML::BeginSeq;
  out << YAML::BeginMap; // IndiviualAccounts
  out << YAML::Key << "AccountNumber" << YAML::Value << m_Table.GetAccountNumber();
  out << YAML::Key << "AccountName" << YAML::Value << m_Table.GetName();
  out << YAML::Key << "IsCreditAccount" << YAML::Value << m_Table.IsCreditAccount();
  out << YAML::Key << "DebitEntries" << YAML::BeginSeq;

  for (auto &[id, entry] : m_Table.GetDebitTable()->GetEntries()) {
    out << YAML::BeginMap; // DebitEntry

    const Date_t &date = entry.GetDate();
    out << YAML::Key << "Month" << YAML::Value << date.Month;
    out << YAML::Key << "Day" << YAML::Value << date.Day;
    out << YAML::Key << "Year" << YAML::Value << date.Year;

    out << YAML::Key << "AccountID" << YAML::Value << entry.GetAccountID();
    out << YAML::Key << "Amount" << YAML::Value << entry.GetAmount();

    out << YAML::EndMap; // DebitEntry
  }

  out << YAML::EndSeq; // DebitEntries
  out << YAML::Key << "CreditEntries" << YAML::Value << YAML::BeginSeq;

  for (auto &[id, entry] : m_Table.GetCreditTable()->GetEntries()) {
    out << YAML::BeginMap; // CreditEntry

    const Date_t &date = entry.GetDate();
    out << YAML::Key << "Month" << YAML::Value << date.Month;
    out << YAML::Key << "Day" << YAML::Value << date.Day;
    out << YAML::Key << "Year" << YAML::Value << date.Year;

    out << YAML::Key << "AccountID" << YAML::Value << entry.GetAccountID();
    out << YAML::Key << "Amount" << YAML::Value << entry.GetAmount();

    out << YAML::EndMap; // CreditEntry
  }
  out << YAML::EndSeq; // CreditEntries

  out << YAML::EndMap; // IndiviualAccounts
  out << YAML::EndSeq; // Accounts

  std::ofstream fout("test.yaml");
  fout << out.c_str();
}

void FerretLayer::OnUpdate() {}

void FerretLayer::OnUIRender() {
  ImGuiViewport *viewport = ImGui::GetMainViewport();
  ImGui::SetNextWindowSize(viewport->Size);
  ImGui::SetNextWindowPos(viewport->Pos);
  ImGuiWindowFlags wflags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove;
  ImGui::Begin("Full Screen", nullptr, wflags);

  m_Table.Draw();
  ImGui::SameLine();
  m_Table2.Draw();

  ImGui::End();

  //ImGui::ShowDemoWindow();
}

void FerretLayer::OnEvent(Event &e) {
  EventDispatcher dispatcher(e);

  dispatcher.Dispatch<KeyPressedEvent>(
      BIND_EVENT_FN(FerretLayer::OnKeyPressedEvent));
}

bool FerretLayer::OnKeyPressedEvent(KeyPressedEvent &e) {
  return false;
}

} // namespace Ferret
