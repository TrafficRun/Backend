cmake_minimum_required(VERSION 3.22)
set(CMAKE_CXX_STANDARD 17)

project(TrafficRun LANGUAGES CXX VERSION "0.1.0")

execute_process(
  COMMAND git rev-parse --short HEAD
  WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
  OUTPUT_VARIABLE GIT_SHORT_HASH
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

configure_file("core/version.cpp.in" "${CMAKE_CURRENT_BINARY_DIR}/version.cpp" @ONLY)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Boost REQUIRED program_options system chrono atomic timer json)
find_package(Threads REQUIRED)

set(SQLITECPP_RUN_CPPLINT OFF)

add_subdirectory(./thirdparty/cpp-httplib/)
add_subdirectory(./thirdparty/SQLiteCpp)

include_directories(./thirdparty/)

# 普通文件
aux_source_directory(. SOURCE_FILES)
file(GLOB HEADER_FILES ./*.h)

include_directories(./)

# 核心文件
aux_source_directory(./core/ CORE_SOURCE_FILES)
file(GLOB CORE_HEADER_FILES ./core/*.h)

# 数据库文件
aux_source_directory(./core/database CORE_DATABASE_SOURCE_FILES)

# 网络
aux_source_directory(./http/ NET_SOURCE_FILES)
file(GLOB NET_HEADER_FILES ./http/*.h)

# 模型文件夹
file(GLOB MODEL_FILES true ./model/*)

foreach(MFILE ${MODEL_FILES})
  if (IS_DIRECTORY ${MFILE})
    list(APPEND MODEL_DIR ${MFILE})
    aux_source_directory(${MFILE} MODEL_SRC)
    list(APPEND MODELS_SRC ${MODEL_SRC})
    file(GLOB MODEL_HEADER ${MFILE})
    list(APPEND MODELS_HEADER ${MODEL_HEADER})
  endif()
endforeach()

# 奖励生成器文件
file(GLOB REWARD_FILES true ./reward_generator/*)

foreach(MFILE ${REWARD_FILES})
  if(IS_DIRECTORY ${MFILE})
    list(APPEND REWARD_GEN_DIR ${MFILE})
    aux_source_directory(${MFILE} MODEL_SRC)
    list(APPEND REWARD_GENS_SRC ${MODEL_SRC})
    file(GLOB MODEL_HEADER ${MFILE})
    list(APPEND REWARD_GENS_HEADER ${MODEL_HEADER})
  endif()
endforeach()

list(APPEND SOURCE_FILES "${CMAKE_CURRENT_BINARY_DIR}/version.cpp")

add_executable(${PROJECT_NAME} 
  ${SOURCE_FILES} 
  ${CORE_SOURCE_FILES} ${CORE_HEADER_FILES} 
  ${MODELS_SRC} ${MODELS_HEADER} 
  ${REWARD_GENS_SRC} ${REWARD_GENS_SRC}
  ${NET_SOURCE_FILES} ${NET_HEADER_FILES}
  ${CORE_DATABASE_SOURCE_FILES}
)

target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES} Threads::Threads SQLiteCpp)
