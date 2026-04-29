cmake_minimum_required(VERSION 3.20)
project (RHIQt)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set (SRC_PREFIX ${PROJECT_SOURCE_DIR}/Source)

file(GLOB_RECURSE SRC_LIST ${SRC_PREFIX}
		${SRC_PREFIX}/*.h
		${SRC_PREFIX}/*.c 
		${SRC_PREFIX}/*.cpp 
)


if (CMAKE_SYSTEM_NAME STREQUAL Windows)
	source_group (TREE "${SRC_PREFIX}" PREFIX "RHIForQt" FILES ${SRC_LIST})
endif ()

add_library (${PROJECT_NAME} ${SRC_LIST})
target_include_directories (${PROJECT_NAME} PUBLIC  ${INCLUDE_PATH})
target_link_directories(${PROJECT_NAME} PUBLIC  ${LINK_PATH})
target_link_libraries  (${PROJECT_NAME} ${LINK_LIBRARYS})

if (MSVC)
	target_compile_options(${PROJECT_NAME} PUBLIC /permissive- /Zc:__cplusplus /std:c++17)
endif()