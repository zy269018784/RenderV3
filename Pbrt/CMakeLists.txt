cmake_minimum_required(VERSION 3.14)
project(RenderLib)

if (${OPENGL_RENDER})
	message(STATUS "OPENGL_RENDER ${OPENGL_RENDER}")
endif()

if (${USE_CUDA})
	message(STATUS "USE_CUDA ${USE_CUDA}")
endif()


set (SRC_PREFIX 
	${PROJECT_SOURCE_DIR}/Source
)

if (CMAKE_SYSTEM_NAME STREQUAL Windows)
	set (LIBRARY_PREFIX_DIR ${CMAKE_SOURCE_DIR}/../Library)
	if (${OPENGL_RENDER})
		set (CMAKE_PREFIX_PATH ${LIBRARY_PREFIX_DIR}/qt/5.9.2/msvc2017_64/lib/cmake)
	endif()
	#set (EXECUTABLE_OUTPUT_PATH D:/application/myapp)
elseif (CMAKE_SYSTEM_NAME STREQUAL Linux)
	set (LIBRARY_PREFIX_DIR ${CMAKE_SOURCE_DIR}/../Library)
endif()

# 打开CUDA
if (${USE_CUDA})
	# CheckLanguage
	include (CheckLanguage)
	check_language(CUDA)
	if (CMAKE_CUDA_COMPILER)
		message (STATUS "CUDA  found")
	else ()
		message (STATUS "CUDA not found")
	endif ()
	enable_language (CUDA)
	set (CMAKE_CUDA_STANDARD 17)
endif ()

# language
enable_language (CXX)
set (CMAKE_CXX_STANDARD  17)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -msse2 -msse4.1 -mpclmul")

# QT
if (${OPENGL_RENDER})
	set (CMAKE_AUTOMOC ON)
	set (CMAKE_AUTOUIC ON)
	set (CMAKE_AUTORCC ON)
endif()

# Package
if (${USE_CUDA})
	find_package(CUDA REQUIRED)
endif()
if (${OPENGL_RENDER})
	find_package(Qt5 COMPONENTS Widgets REQUIRED)
	list (APPEND RENDER_DEFINITIONS "OPENGL_RENDER")
	message (STATUS "RENDER_DEFINITIONS ${RENDER_DEFINITIONS}")
endif()

# Definition
if (${USE_CUDA})
	list (APPEND RENDER_DEFINITIONS "RENDER_IS_GPU_CODE")
	endif()
if (CMAKE_SYSTEM_NAME STREQUAL Windows)
  list (APPEND RENDER_DEFINITIONS "RENDER_IS_WINDOWS" "NOMINMAX")
elseif (CMAKE_SYSTEM_NAME STREQUAL Darwin)
  list (APPEND RENDER_DEFINITIONS "RENDER_IS_OSX")
elseif (CMAKE_SYSTEM_NAME STREQUAL Linux)
  list (APPEND RENDER_DEFINITIONS "RENDER_IS_LINUX")
endif()

include (CheckIncludeFiles)

include (CheckCXXSourceCompiles)
check_cxx_source_compiles ( "
#include <malloc.h>
int main() { void * ptr = _aligned_malloc(1024, 32); }
" HAVE__ALIGNED_MALLOC )

check_cxx_source_compiles ( "
#include <stdlib.h>
int main() {
  void *ptr;
  posix_memalign(&ptr, 32, 1024);
} " HAVE_POSIX_MEMALIGN )

if (HAVE__ALIGNED_MALLOC)
  list (APPEND RENDER_DEFINITIONS "RENDER_HAVE__ALIGNED_MALLOC")
elseif (HAVE_POSIX_MEMALIGN)
  list (APPEND RENDER_DEFINITIONS "RENDER_HAVE_POSIX_MEMALIGN")
else ()
  message (SEND_ERROR "Unable to find a way to allocate aligned memory")
endif ()

# Library
if (CMAKE_SYSTEM_NAME STREQUAL Windows)
	set (LIBRARYS 
		assimp-vc143-mt
		double-conversion
		IlmThread-3_2
		OpenEXR-3_2
		OpenEXRCore-3_2
		OpenEXRUtil-3_2
		utf8proc_static
		zlibstatic
	)
if (${USE_CUDA})
	 list (APPEND LIBRARYS 
		cudadevrt.lib
		cudart_static.lib
	 )
endif()
elseif (CMAKE_SYSTEM_NAME STREQUAL Linux)
	set (LIBRARYS 
		assimp
		double-conversion
		IlmThread-3_2
		OpenEXR-3_2
		OpenEXRCore-3_2
		OpenEXRUtil-3_2
		utf8proc
		z
)
endif()

if (${OPENGL_RENDER})
	list (APPEND LIBRARYS Qt5::Widgets)
endif ()

# Link path
set (LINK_PATH )

# Include path
set (INCLUDE_PATH
	${SRC_PREFIX}
	${PROJECT_SOURCE_DIR}/../libsrc
)

if (CMAKE_SYSTEM_NAME STREQUAL Windows)
	list (APPEND INCLUDE_PATH 
		${SRC_PREFIX}/Windows/Qt
		${LIBRARY_PREFIX_DIR}		
		${LIBRARY_PREFIX_DIR}/Windows/assimp/include
		${LIBRARY_PREFIX_DIR}/Windows/Imath_3_1/include
		${LIBRARY_PREFIX_DIR}/Windows/OpenEXR_3_2_1/include
		${LIBRARY_PREFIX_DIR}/Windows/double-conversion/include
		${LIBRARY_PREFIX_DIR}/Windows/utf8proc/include
		${LIBRARY_PREFIX_DIR}/Windows/zlib/include
		${LIBRARY_PREFIX_DIR}/Windows/stb/include
		${LIBRARY_PREFIX_DIR}/Windows/nanovdb/include
	)

	list (APPEND LINK_PATH 
		${LIBRARY_PREFIX_DIR}/lib/
		${LIBRARY_PREFIX_DIR}/Windows/assimp/lib
		${LIBRARY_PREFIX_DIR}/Windows/Imath_3_1/lib
		${LIBRARY_PREFIX_DIR}/Windows/OpenEXR_3_2_1/lib
		${LIBRARY_PREFIX_DIR}/Windows/double-conversion/lib
		${LIBRARY_PREFIX_DIR}/Windows/utf8proc/lib
		${LIBRARY_PREFIX_DIR}/Windows/zlib/lib
	)

elseif (CMAKE_SYSTEM_NAME STREQUAL Darwin)
  
elseif (CMAKE_SYSTEM_NAME STREQUAL Linux)
  list (APPEND INCLUDE_PATH 
	/usr/local/cuda/include
	/usr/local/include
	/usr/local/include/Imath
	/usr/local/include/stb
	/usr/local/include/double-conversion
	/usr/local/include/utf8proc
	/usr/local/include/assimp
	/usr/local/include/OpenEXR
  )

  	list (APPEND LINK_PATH 
		${LIBRARY_PREFIX_DIR}/lib/
		/usr/local/lib/
	)
endif()

message (STATUS "Renderlib INCLUDE_PATH ${INCLUDE_PATH}")
message (STATUS "Renderlib LINK_PATH ${LINK_PATH}")

file(GLOB  MAIN_SOURCE							${SRC_PREFIX}/*.h ${SRC_PREFIX}/*.cpp)
file(GLOB  MATH_SOURCE							${SRC_PREFIX}/Math/*.h ${SRC_PREFIX}/Math/*.cpp)
file(GLOB  UTIL_SOURCE							${SRC_PREFIX}/Util/*.h ${SRC_PREFIX}/Util/*.cpp)
file(GLOB  CAMERA_SOURCE						${SRC_PREFIX}/Camera/*.h ${SRC_PREFIX}/Camera/*.cpp)
file(GLOB  SAMLPER_SOURCE						${SRC_PREFIX}/Sampler/*.h ${SRC_PREFIX}/Sampler/*.cpp)
file(GLOB  SHAPE_SOURCE							${SRC_PREFIX}/Shape/*.h ${SRC_PREFIX}/Shape/*.cpp)
file(GLOB  MATERIAL_SOURCE						${SRC_PREFIX}/Material/*.h ${SRC_PREFIX}/Material/*.cpp)
file(GLOB  PRIMITIVE_SOURCE						${SRC_PREFIX}/Primitive/*.h ${SRC_PREFIX}/Primitive/*.cpp)
file(GLOB  SCENE_SOURCE							${SRC_PREFIX}/Scene/*.h ${SRC_PREFIX}/Scene/*.cpp)
file(GLOB  MEMORY_SOURCE						${SRC_PREFIX}/Memory/*.h ${SRC_PREFIX}/Memory/*.cpp)
file(GLOB  MEMORY_GPU_SOURCE					${SRC_PREFIX}/Memory/GPU/*.h ${SRC_PREFIX}/Memory/GPU/*.cpp)
file(GLOB  MEMORY_CPU_SOURCE					${SRC_PREFIX}/Memory/CPU/*.h ${SRC_PREFIX}/Memory/CPU/*.cpp)
file(GLOB  CONTAINER_SOURCE						${SRC_PREFIX}/Container/*.h ${SRC_PREFIX}/Container/*.cpp)
file(GLOB  INTERSECTION_SOURCE					${SRC_PREFIX}/Intersection/*.h ${SRC_PREFIX}/Intersection/*.cpp)
file(GLOB  RENDER_SOURCE						${SRC_PREFIX}/Render/*.h ${SRC_PREFIX}/Render/*.cpp)
file(GLOB  LOADER_SOURCE						${SRC_PREFIX}/Loader/*.h ${SRC_PREFIX}/Loader/*.cpp)
file(GLOB  TEXTURE_SOURCE						${SRC_PREFIX}/Texture/*.h ${SRC_PREFIX}/Texture/*.cpp)
file(GLOB  FILM_SOURCE							${SRC_PREFIX}/Film/*.h ${SRC_PREFIX}/Film/*.cpp)
file(GLOB  MEDIA_SOURCE							${SRC_PREFIX}/Medium/*.h ${SRC_PREFIX}/Medium/*.cpp)
file(GLOB  LIGHT_SOURCE							${SRC_PREFIX}/Light/*.h ${SRC_PREFIX}/Light/*.cpp)
file(GLOB  RAY_SOURCE							${SRC_PREFIX}/Ray/*.h ${SRC_PREFIX}/Ray/*.cpp)
file(GLOB  TEST_SOURCE							${SRC_PREFIX}/Test/*.h ${SRC_PREFIX}/Test/*.cpp)
file(GLOB  IMAGE_SOURCE							${SRC_PREFIX}/Image/*.h ${SRC_PREFIX}/Image/*.cpp)
file(GLOB  MESH_SOURCE							${SRC_PREFIX}/Mesh/*.h ${SRC_PREFIX}/Mesh/*.cpp)
file(GLOB  BXDF_SOURCE							${SRC_PREFIX}/BxDF/*.h ${SRC_PREFIX}/BxDF/*.cpp)
file(GLOB  INTEGRATOR_SOURCE					${SRC_PREFIX}/Integrator/*.h ${SRC_PREFIX}/Integrator/*.cpp)
file(GLOB  COOKTORRANCE_SOURCE					${SRC_PREFIX}/CookTorrance/*.h ${SRC_PREFIX}/CookTorrance/*.cpp)
file(GLOB  HDRI_SOURCE							${SRC_PREFIX}/HDRI/*.h ${SRC_PREFIX}/HDRI/*.cpp)
file(GLOB  QT_RESOURCE_FILE                     ${SRC_PREFIX}/*.qrc)

source_group ("Main" FILES ${MAIN_SOURCE})
source_group ("Math" FILES ${MATH_SOURCE})
source_group ("Util" FILES ${UTIL_SOURCE})
source_group ("Memory" FILES ${MEMORY_SOURCE})
source_group ("Memory/GPU" FILES ${MEMORY_GPU_SOURCE})
source_group ("Memory/CPU" FILES ${MEMORY_CPU_SOURCE})
source_group ("Camera" FILES ${CAMERA_SOURCE})
source_group ("Sampler" FILES ${SAMLPER_SOURCE})
source_group ("Shape" FILES ${SHAPE_SOURCE})
source_group ("Primitive" FILES ${PRIMITIVE_SOURCE})
source_group ("Material" FILES ${MATERIAL_SOURCE})
source_group ("Scene" FILES ${SCENE_SOURCE})
source_group ("Container" FILES ${CONTAINER_SOURCE})
source_group ("Intersection" FILES ${INTERSECTION_SOURCE})
source_group ("Render" FILES ${RENDER_SOURCE})
source_group ("Loader" FILES ${LOADER_SOURCE})
source_group ("Texture" FILES ${TEXTURE_SOURCE})
source_group ("Film" FILES ${FILM_SOURCE})
source_group ("Medium" FILES ${MEDIA_SOURCE})
source_group ("Light" FILES ${LIGHT_SOURCE})
source_group ("Ray" FILES ${RAY_SOURCE})
source_group ("Test" FILES ${TEST_SOURCE})
source_group ("Image" FILES ${IMAGE_SOURCE})
source_group ("Mesh" FILES ${MESH_SOURCE})
source_group ("BxDF" FILES ${BXDF_SOURCE})
source_group ("Integrator" FILES ${INTEGRATOR_SOURCE})
source_group ("CookTorrance" FILES ${COOKTORRANCE_SOURCE})		
source_group ("HDRI" FILES ${HDRI_SOURCE})	

file(GLOB  TYPE_SOURCE					${SRC_PREFIX}/Type/*.h ${SRC_PREFIX}/Type/*.cpp)
source_group ("Type" FILES ${TYPE_SOURCE})	

file(GLOB  WEATHER_SOURCE					${SRC_PREFIX}/Weather/*.h ${SRC_PREFIX}/Weather/*.cpp)
source_group ("Weather" FILES ${WEATHER_SOURCE})	


if (OPENGL_RENDER)
	file(GLOB  WINDOWS_SOURCE						${SRC_PREFIX}/Windows/*.h ${SRC_PREFIX}/Windows/*.cpp)
	file(GLOB  WINDOWS_QT_SOURCE					${SRC_PREFIX}/Windows/QT/*.h ${SRC_PREFIX}/Windows/QT/*.cpp)
	source_group ("Windows" FILES ${WINDOWS_SOURCE})
	source_group ("Windows/QT" FILES ${WINDOWS_QT_SOURCE})
endif()



set (CPU_SRC_LIST
  ${UTIL_SOURCE}
  #${LEARNOPENGL_COMMON_SOURCE}
  ${WINDOWS_SOURCE}
  #${WINDOWS_GLFW_SOURCE}
  ${WINDOWS_QT_SOURCE}
  ${SCENE_SOURCE}
  ${MEMORY_SOURCE}
  ${MEMORY_CPU_SOURCE}
  ${LOADER_SOURCE}
  ${IMAGE_SOURCE}
  ${HDRI_SOURCE}
  #${IMGUI_SOURCE}
  ${QT_RESOURCE_FILE}
)

set (GPU_SRC_LIST 
  ${TYPE_SOURCE}
  ${MATH_SOURCE}
  ${MAIN_SOURCE}
  ${MEMORY_GPU_SOURCE} 
  ${CAMERA_SOURCE}
  ${SAMLPER_SOURCE}
  ${SHAPE_SOURCE}
  ${MATERIAL_SOURCE}
  ${PRIMITIVE_SOURCE}
  ${CONTAINER_SOURCE}
  ${INTERSECTION_SOURCE}
  ${RENDER_SOURCE}
  ${TEXTURE_SOURCE}
  ${FILM_SOURCE}
  ${MEDIA_SOURCE}
  ${LIGHT_SOURCE}
  ${RAY_SOURCE}
  ${MESH_SOURCE}
  ${BXDF_SOURCE}
  ${INTEGRATOR_SOURCE}	
  ${COOKTORRANCE_SOURCE}  
  ${WEATHER_SOURCE}
  #${PARTICLE_SYSTEM_SOURCE}
  ${TEST_SOURCE}
)
if (${USE_CUDA})
	set_source_files_properties (
		${GPU_SRC_LIST}
		PROPERTIES LANGUAGE CUDA
	)
endif ()

set (SRC_LIST
  ${GPU_SRC_LIST}
  ${CPU_SRC_LIST}
  ${PROJECT_SOURCE_DIR}/../libsrc/lodepng/lodepng.cpp
  ${SRC_PREFIX}/main.cpp
)


if (${USE_CUDA})
	add_library (cuda_build_configuration INTERFACE)
	target_compile_options (
				cuda_build_configuration
				INTERFACE
					"$<$<COMPILE_LANGUAGE:CUDA>:--use_fast_math;--expt-relaxed-constexpr;--extended-lambda;--forward-unknown-to-host-compiler>"
					"$<$<COMPILE_LANGUAGE:CUDA>:$<IF:$<AND:$<CONFIG:Debug>,$<NOT:$<BOOL:$<TARGET_PROPERTY:CUDA_PTX_COMPILATION>>>>,-G;-g,-lineinfo;-maxrregcount;128>>"   
		)
	
	get_target_property (v1 cuda_build_configuration INTERFACE_COMPILE_OPTIONS)	
	message (STATUS "cuda_build_configuration ${v1}")	
endif ()		

add_library(${PROJECT_NAME} ${SRC_LIST})
target_include_directories(${PROJECT_NAME} PUBLIC  ${INCLUDE_PATH})
target_link_directories(${PROJECT_NAME} PUBLIC  ${LINK_PATH})
if (${USE_CUDA})
	target_link_libraries(${PROJECT_NAME} ${LIBRARYS} $<$<BOOL:PBRT_CUDA_ENABLED>:cuda_build_configuration>)
else ()
	target_link_libraries(${PROJECT_NAME} ${LIBRARYS})
endif ()
target_compile_definitions (${PROJECT_NAME} PUBLIC ${RENDER_DEFINITIONS})

message(STATUS "${LIBRARYS}  ${LIBRARYS}")
message(STATUS "${PROJECT_NAME} INCLUDE_DIRECTORY is ${INCLUDE_DIRECTORY}")
message(STATUS "${PROJECT_NAME} PROJECT_SOURCE_DIR is ${PROJECT_SOURCE_DIR}")
if (${USE_CUDA})
	set_property(TARGET ${PROJECT_NAME} PROPERTY CUDA_SEPARABLE_COMPILATION ON)
	set_target_properties(${PROJECT_NAME} PROPERTIES CUDA_RESOLVE_DEVICE_SYMBOLS ON)
endif ()
