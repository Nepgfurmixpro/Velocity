cmake_minimum_required(VERSION 3.25)
project(Velocity)

set(CMAKE_CXX_STANDARD 23)

find_package(Vulkan REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(spdlog REQUIRED)
find_package(spirv_cross_core)
find_package(spirv_cross_glsl CONFIG REQUIRED)
find_package(spirv_cross_cpp CONFIG REQUIRED)


file(GLOB_RECURSE
        SOURCE_FILES

        Source/*.cpp
        Source/*.hpp
        )

foreach (sourceFile ${SOURCE_FILES})
    file(RELATIVE_PATH targetFile ${CMAKE_SOURCE_DIR} ${sourceFile})
    message(STATUS "Found compile target: ${targetFile}")
endforeach ()

list(LENGTH SOURCE_FILES SOURCE_FILE_COUNT)
message(STATUS "${SOURCE_FILE_COUNT} source files found.")

include_directories(Velocity ${CMAKE_SOURCE_DIR}/Source)

add_executable(Velocity ${SOURCE_FILES})

target_link_libraries(Velocity PRIVATE Vulkan::Vulkan)
target_link_libraries(Velocity PRIVATE glfw)
target_link_libraries(Velocity PRIVATE glm::glm)
target_link_libraries(Velocity PRIVATE spdlog::spdlog $<$<BOOL:${MINGW}>:ws2_32>)
target_link_libraries(Velocity PRIVATE spirv-cross-cpp)
