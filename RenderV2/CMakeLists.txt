cmake_minimum_required(VERSION 3.14)
project(Render2)

enable_language (CXX)
set (CMAKE_CXX_STANDARD  17)

if (${OPENGL_RENDER})
	message (STATUS "OPENGL_RENDER")
endif()
add_subdirectory(${PROJECT_SOURCE_DIR}/../Pbrt Pbrt)

set (INCLUDE_PATH

)

set (LINK_PATH

)

set (LIBRARYS 
	RenderLib
)


set (SRC_LIST
  ${CMAKE_SOURCE_DIR}/Source/main.cpp
)

add_executable(${PROJECT_NAME} ${SRC_LIST})
target_include_directories(${PROJECT_NAME} PUBLIC  ${INCLUDE_PATH})
target_link_directories(${PROJECT_NAME} PUBLIC  ${LINK_PATH})
target_link_libraries(${PROJECT_NAME} ${LIBRARYS})