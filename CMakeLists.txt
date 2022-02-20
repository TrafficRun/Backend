cmake_minimum_required(VERSION 3.22)
project(TrafficRun LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# 普通文件
aux_source_directory(. SOURCE_FILES)
file(GLOB HEADER_FILES ./*.h)

include_directories(./)
include_directories(./core/)
# 核心文件
aux_source_directory(./core/ CORE_SOURCE_FILES)
file(GLOB CORE_HEADER_FILES ./core/*.h)

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

find_package(Boost REQUIRED program_options system chrono atomic timer json)

add_executable(${PROJECT_NAME} ${SOURCE_FILES} ${CORE_SOURCE_FILES} ${CORE_HEADER_FILES} ${MODELS_SRC} ${MODELS_HEADER} ${REWARD_GENS_SRC} ${REWARD_GENS_SRC})
target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES})
