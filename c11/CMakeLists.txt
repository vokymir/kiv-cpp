cmake_minimum_required(VERSION 3.16)
project(cpp LANGUAGES CXX)

# Use C++23
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Collect sources
file(GLOB SOURCES "${PROJECT_SOURCE_DIR}/*.cpp"
)

# Add the executable
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR})
add_executable(${PROJECT_NAME} ${SOURCES})

# 1. highest level of debug info
# 2. NO optimization
# 3. sure
target_compile_options(${PROJECT_NAME} PRIVATE -g3 -O0 -fno-omit-frame-pointer)

# ASAN - don't compile for perf
target_compile_options(${PROJECT_NAME} PRIVATE -fsanitize=address -fno-omit-frame-pointer -g)
target_link_options(${PROJECT_NAME} PRIVATE -fsanitize=address)
