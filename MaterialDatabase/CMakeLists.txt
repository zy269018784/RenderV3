cmake_minimum_required(VERSION 3.14)
project(MaterialDatabase)

enable_language (CXX)
set (CMAKE_CXX_STANDARD  17)


set (SRC_PREFIX ${CMAKE_SOURCE_DIR}/Source)
file(GLOB_RECURSE SRC_LIST ${SRC_PREFIX}/*.cpp ${SRC_PREFIX}/*.c ${SRC_PREFIX}/*.h *.qrc)

SET (INCLUDE_PATH ${CMAKE_SOURCE_DIR}/Library/include)
SET (LINK_PATH ${CMAKE_SOURCE_DIR}/Library/lib)
SET (LIBRARYS sqlite3.lib)


add_executable(${CMAKE_PROJECT_NAME} ${SRC_LIST} )
target_compile_definitions (${CMAKE_PROJECT_NAME} PUBLIC ${RENDER_DEFINITIONS})
target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC  ${INCLUDE_PATH})
target_link_directories(${CMAKE_PROJECT_NAME} PUBLIC  ${LINK_PATH})
target_link_libraries(${CMAKE_PROJECT_NAME} ${LIBRARYS})