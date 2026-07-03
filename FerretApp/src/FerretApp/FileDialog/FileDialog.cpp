#include "FileDialog.h"

#include <GLFW/glfw3.h>

#include <nfd.hpp>
#include <nfd_glfw3.h>

namespace Ferret {
std::string
FileDialog::OpenFile(const std::initializer_list<FilterElements> &filters) {
  NFD::Guard nfdGaurd;

  NFD::UniquePath outPath;

  std::vector<nfdfilteritem_t> nfdFilters;
  for (auto &[name, spec] : filters) {
    nfdFilters.push_back({name, spec});
  }

  nfdresult_t result =
      NFD::OpenDialog(outPath, nfdFilters.data(), nfdFilters.size());
  if (result == NFD_OKAY) {
    return (const char *)outPath.get();
  } else if (result == NFD_CANCEL) {
    return "";
  } else {
    FE_CLI_ERROR("Failed to open file at {}", (const char *)outPath.get());
    return "";
  }
}

std::string
FileDialog::SaveFile(const std::initializer_list<FilterElements> &filters) {
  NFD::Guard nfdGaurd;

  NFD::UniquePath outPath;

  std::vector<nfdfilteritem_t> nfdFilters;
  for (auto &[name, spec] : filters) {
    nfdFilters.push_back({name, spec});
  }

  nfdresult_t result =
      NFD::SaveDialog(outPath, nfdFilters.data(), nfdFilters.size());
  if (result == NFD_OKAY) {
    return (const char *)outPath.get();
  } else if (result == NFD_CANCEL) {
    return "";
  } else {
    FE_CLI_ERROR("Failed to save file at {}", (const char *)outPath.get());
    return "";
  }
}
} // namespace CatEngine
