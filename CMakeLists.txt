cmake_minimum_required(VERSION 3.22)
project(TrafficRun LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# 普通文件
aux_source_directory(. SOURCE_FILES)
file(GLOB HEADER_FILES ./*.h)

# 核心文件
aux_source_directory(./core/ CORE_SOURCE_FILES)
file(GLOB CORE_HEADER_FILES ./core/*.h)

add_executable(${PROJECT_NAME} ${SOURCE_FILES} ${CORE_SOURCE_FILES} ${CORE_HEADER_FILES})
