cmake_minimum_required(VERSION 3.20)
project(GameEngine VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_OSX_ARCHITECTURES arm64)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

#find Pacakage
find_package(SDL3 REQUIRED)
find_package(SDL3_image REQUIRED)
find_package(glm REQUIRED)

# Engine library
add_library(Engine
    src/Engine/Core/Application.cpp
    src/Engine/Core/Window.cpp
    src/Engine/Input/Input.cpp
    src/Engine/Core/Time.cpp
    src/Engine/Entity/Entity.cpp
    src/Engine/Entity/Player.cpp
    src/Engine/Physics/Collision.cpp
    src/Engine/Scene/Scene.cpp
    src/Engine/Renderer/Texture.cpp
    src/Engine/Resources/TextureManager.cpp
    # src/Engine/Renderer/Renderer.cpp
)

target_include_directories(Engine PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${SDL3_INCLUDE_DIRS}
)

target_link_libraries(Engine PUBLIC
    SDL3::SDL3
    SDL3_image::SDL3_image
    glm::glm
)

# Game executable
add_executable(Game
    src/Game/main.cpp
    # src/Game/Game.cpp
)

target_link_libraries(Game PRIVATE Engine)

# Copy assets to build folder (optional but useful)
add_custom_command(TARGET Game POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:Game>/assets
)