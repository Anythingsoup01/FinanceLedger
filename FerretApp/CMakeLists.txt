add_executable(FerretApp)

set_property(TARGET FerretApp PROPERTY CXX_STANDARD 20)
target_sources(FerretApp PRIVATE
src/FerretApp/Statements/Statements.cpp
src/FerretApp/Core/Entrypoint.cpp
src/FerretApp/Core/Serializer.cpp
src/FerretApp/FileDialog/FileDialog.cpp
src/FerretApp/Utils/Utils.cpp
src/FerretApp/Layer/FerretLayer.cpp
src/FerretApp/Ledger/EntryTable.cpp
src/FerretApp/Ledger/Ledger.cpp
src/FerretApp/Ledger/Entry.cpp
src/FerretApp/Ledger/Table.cpp
src/FerretApp/Statements/Statements.h
src/FerretApp/Core/Serializer.h
src/FerretApp/FileDialog/FileDialog.h
src/FerretApp/Utils/Utils.h
src/FerretApp/Layer/FerretLayer.h
src/FerretApp/Ledger/Table.h
src/FerretApp/Ledger/Entry.h
src/FerretApp/Ledger/Ledger.h
src/FerretApp/Ledger/EntryTable.h
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



