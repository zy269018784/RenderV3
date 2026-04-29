cmake_minimum_required(VERSION 3.20)
set (TARGET_NAME RHI)
project (${TARGET_NAME})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set (LIBRARY_PREFIX_PATH Library)
set (SRC_PREFIX ${PROJECT_SOURCE_DIR}/Source)



#if (CMAKE_SYSTEM_NAME STREQUAL Windows)
#	set (CMAKE_PREFIX_PATH D:/application/qt6/6.8.0/msvc2022_64/lib/cmake)
#	find_package(Qt6 COMPONENTS Widgets REQUIRED)
#elseif (CMAKE_SYSTEM_NAME STREQUAL Linux)
#	set (LIBRARY_PREFIX_DIR ${CMAKE_SOURCE_DIR}/../Library)
#endif()

if (CMAKE_SYSTEM_NAME STREQUAL Windows)
	include (${PROJECT_SOURCE_DIR}/glfw.txt)
	if (${USE_QT})
		include (${PROJECT_SOURCE_DIR}/Qt.txt)
	endif ()
endif ()
include (${PROJECT_SOURCE_DIR}/Vulkan.txt)

#add_subdirectory(${PROJECT_SOURCE_DIR}/../libsrc/glfw GLFW)

message (STATUS "RHI SRC_PREFIX ${SRC_PREFIX}")


set (INCLUDE_PATH 
	${SRC_PREFIX}
	${SRC_PREFIX}/RHI
	${Vulkan_INCLUDE_PATH}
)

set (LINK_PATH 
	${Vulkan_LINK_PATH}
)	

set (LINK_LIBRARYS 
	${Vulkan_LIBS}
)	

if (CMAKE_SYSTEM_NAME STREQUAL Windows)
	list (APPEND INCLUDE_PATH 	${GLFW_INCLUDE_PATH})
elseif (CMAKE_SYSTEM_NAME STREQUAL Linux)
	list (APPEND INCLUDE_PATH 	/usr/local/include/GLFW)
endif ()

if (${USE_QT})
	list (APPEND INCLUDE_PATH 	${Qt_INCLUDE_PATH})
endif ()

if (CMAKE_SYSTEM_NAME STREQUAL Windows)
	list (APPEND LINK_PATH 	${GLFW_LINK_PATH})
	elseif (CMAKE_SYSTEM_NAME STREQUAL Linux)
	list (APPEND LINK_PATH 	/usr/local/lib)
endif ()

if (${USE_QT})
	list (APPEND LINK_PATH 	${Qt_LINK_PATH})
endif ()


if (CMAKE_SYSTEM_NAME STREQUAL Windows)
	list (APPEND LINK_LIBRARYS 	${GLFW_LIBS})
elseif (CMAKE_SYSTEM_NAME STREQUAL Linux)
	# Linux需要预先安装好glfw
	list (APPEND LINK_LIBRARYS 	glfw3)
endif ()

if (${USE_QT})
	list (APPEND LINK_LIBRARYS 	${Qt_LIBS})
endif ()




file(GLOB_RECURSE SRC_LIST ${SRC_PREFIX} ${SRC_PREFIX}/*.cpp ${SRC_PREFIX}/*.c ${SRC_PREFIX}/*.h )


if (CMAKE_SYSTEM_NAME STREQUAL Windows)
	source_group (TREE "${SRC_PREFIX}" PREFIX "RHI" FILES ${SRC_LIST})
endif ()

#message (STATUS "RHI SRC_LIST ${SRC_LIST}")
add_library (${TARGET_NAME} ${SRC_LIST})
target_include_directories (${TARGET_NAME} PUBLIC  ${INCLUDE_PATH})
target_link_directories(${TARGET_NAME} PUBLIC  ${LINK_PATH})
target_link_libraries  (${TARGET_NAME} ${LINK_LIBRARYS})

if (MSVC)
   # target_compile_options(${TARGET_NAME} PUBLIC /permissive- /Zc:__cplusplus /std:c++17)
	target_compile_options(${TARGET_NAME} PUBLIC /permissive- /Zc:__cplusplus /std:c++17)
endif()