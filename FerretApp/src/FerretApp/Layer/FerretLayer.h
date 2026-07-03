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
  void SubmitEntryDataToTable(const int &toTable, const bool &isCredit, const Date &date, const int &fromTable, const float &amount);

  // Tells the system when the context needs saved
  void SetDirty() { m_ContextDirty = true; }

  // Sends the table removal to the main thread; Clears all entries pertaining to the
  // provided table id
  void RemoveTable(const int &tableID);

  // Sets all internal variables to view the selected entry in a popup window
  void ViewEntry(const int &tableID, const bool &isCredit, const int &entryID);

private:
  // When creating a table we should reload each tables' m_Next pointer
  void ReloadTables();

  bool OnWindowClose(WindowCloseEvent &e);
  bool OnKeyPressedEvent(KeyPressedEvent &e);

  // Used to render the table creation window
  void RenderCreateTablePopup();

  // Used to render the save screen when trying to close with a
  // dirty context
  void RenderSavePopup();

  // Used to render a detailed look at an entry, showing details like Insertion Date,
  // Last Modified Date, and the corresponding Journal Entry for it.
  void RenderEntryDetailsPopup();

  void OpenTables(const std::filesystem::path &path);
  // Used to dynamically open another set of account tables
  void OpenTables();

  // Used to save the tables if the path is already known
  void SaveTables();

  // Used to set the table path before saving
  void SaveTablesAs();
private:
  enum class RenderPopup {
    NONE = 0,
    CreateTable,
    SaveAndExit,
    SaveAndOpenExistingTables,
    EntryDetails,
  };
  RenderPopup m_RenderPopup = RenderPopup::NONE;
private:
  std::map<int, AccountTable> m_Tables;
  std::vector<std::string> m_TableNames; // Get's reloaded everytime we call ReloadTables(); Contains a list of all table names (along with the AccountNumber)

  int m_ViewingTableID; // Used to see which table we are accessing to view
  int m_ViewingEntryID; // Used to see which entry is being viewed
  bool m_IsViewingCreditEntry; // Used to see if the entry we have selected is credit

  bool m_ContextDirty = false; // Used to tell if there is unsaved data
  std::filesystem::path m_TablePath; // The path we save to
  std::filesystem::path m_TempTablePath; // The path we want to swap to in the event a popup happens

  inline static FerretLayer *s_Instance = nullptr;
};

} // namespace Ferret
