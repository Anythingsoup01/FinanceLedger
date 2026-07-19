add_executable(FerretApp)

set_property(TARGET FerretApp PROPERTY CXX_STANDARD 20)
target_sources(FerretApp PRIVATE
src/FerretApp/Statements/Statements.cpp
src/FerretApp/Journal/Journal.cpp
src/FerretApp/Core/Entrypoint.cpp
src/FerretApp/Core/Serializer.cpp
src/FerretApp/FileDialog/FileDialog.cpp
src/FerretApp/Popup/Popup.cpp
src/FerretApp/Popup/LedgerPopup.cpp
src/FerretApp/Popup/StatementPopup.cpp
src/FerretApp/Utils/Utils.cpp
src/FerretApp/Layer/FerretLayer.cpp
src/FerretApp/Ledger/Ledger.cpp
src/FerretApp/Table/Table.cpp
src/FerretApp/Table/DataEntry.cpp
src/FerretApp/Table/DataSet.cpp
src/FerretApp/DataTypes/EntryTable.cpp
src/FerretApp/DataTypes/Entry.cpp
src/FerretApp/DataTypes/AccountTable.cpp
src/FerretApp/Statements/Statements.h
src/FerretApp/Journal/Journal.h
src/FerretApp/Core/Serializer.h
src/FerretApp/FileDialog/FileDialog.h
src/FerretApp/Popup/StatementPopup.h
src/FerretApp/Popup/Popup.h
src/FerretApp/Popup/LedgerPopup.h
src/FerretApp/Utils/Utils.h
src/FerretApp/Layer/FerretLayer.h
src/FerretApp/Ledger/Ledger.h
src/FerretApp/Table/Table.h
src/FerretApp/Table/DataSet.h
src/FerretApp/Table/DataEntry.h
src/FerretApp/DataTypes/AccountTable.h
src/FerretApp/DataTypes/Entry.h
src/FerretApp/DataTypes/EntryTable.h
)

target_include_directories(FerretApp PRIVATE
src
${CMAKE_SOURCE_DIR}/Ferret/Ferret/src
${CMAKE_SOURCE_DIR}/Ferret/vendor/glfw/include
${CMAKE_SOURCE_DIR}/Ferret/vendor/glad/include
${CMAKE_SOURCE_DIR}/Ferret/vendor/spdlog/include
${CMAKE_SOURCE_DIR}/Ferret/vendor/glm
${CMAKE_SOURCE_DIR}/Ferret/vendor/imgui
${CMAKE_SOURCE_DIR}/Ferret/vendor/yaml-cpp/include
${CMAKE_SOURCE_DIR}/vendor/nfd-e/src/include
)

target_link_libraries(FerretApp
Ferret
nfd
)



