cmake_minimum_required(VERSION 3.24)
cmake_policy(VERSION 3.24)  # make policies consistent with modern CMake

project(Snake_AI)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

# Don't refetch every time
set(FETCHCONTENT_UPDATES_DISCONNECTED TRUE)

# Auto-download SFML 3.0.0
FetchContent_Declare(
        sfml
        GIT_REPOSITORY https://github.com/SFML/SFML.git
        GIT_TAG 3.0.0
)

FetchContent_MakeAvailable(sfml)

add_executable(Snake_AI
        main.cpp
        snake.cpp
        world.cpp
        food.cpp
        HitCheck.cpp
        GridMath.cpp
        PostGameMenu.cpp
        AI.cpp
        Evolution.cpp
        Observe.cpp
)

target_link_libraries(Snake_AI PRIVATE
        SFML::System
        SFML::Window
        SFML::Graphics
        SFML::Audio
)
