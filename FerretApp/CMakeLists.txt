add_executable(FerretApp)

set_property(TARGET FerretApp PROPERTY CXX_STANDARD 20)
target_sources(FerretApp PRIVATE
src/FerretApp/Layer/FerretLayer.cpp
src/FerretApp/Core/Entrypoint.cpp
src/FerretApp/Core/Table.cpp
src/FerretApp/Layer/FerretLayer.h
src/FerretApp/Core/Table.h
)

target_include_directories(FerretApp PRIVATE
src
${CMAKE_SOURCE_DIR}/Ferret/Ferret/src
${CMAKE_SOURCE_DIR}/Ferret/vendor/glfw/include
${CMAKE_SOURCE_DIR}/Ferret/vendor/glad/include
${CMAKE_SOURCE_DIR}/Ferret/vendor/spdlog/include
${CMAKE_SOURCE_DIR}/Ferret/vendor/glm
${CMAKE_SOURCE_DIR}/Ferret/vendor/imgui
${CMAKE_SOURCE_DIR}/Ferret/vendor/yaml-cpp/include/
)

target_link_libraries(FerretApp
Ferret
)



