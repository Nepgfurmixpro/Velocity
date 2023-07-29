cmake_minimum_required(VERSION 3.25)
project(Velocity)

set(CMAKE_CXX_STANDARD 23)

find_package(Vulkan REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(spdlog REQUIRED)



file(GLOB_RECURSE
        SOURCE_FILES

        Source/*.cpp
        Source/*.hpp
        )

foreach (sourceFile ${SOURCE_FILES})
    file(RELATIVE_PATH targetFile ${CMAKE_SOURCE_DIR} ${sourceFile})
    message(STATUS "Found compile target: ${targetFile}")
endforeach ()

include_directories(Velocity ${CMAKE_SOURCE_DIR}/Source)

add_executable(Velocity ${SOURCE_FILES})

target_link_libraries(Velocity PRIVATE Vulkan::Vulkan)
target_link_libraries(Velocity PRIVATE glfw)
target_link_libraries(Velocity PRIVATE glm::glm)
target_link_libraries(Velocity PRIVATE spdlog::spdlog $<$<BOOL:${MINGW}>:ws2_32>)
