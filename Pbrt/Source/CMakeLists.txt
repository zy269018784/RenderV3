cmake_minimum_required(VERSION 3.14)
project(Render)

add_subdirectory(${PROJECT_SOURCE_DIR}/RHI)

if (CMAKE_SYSTEM_NAME STREQUAL Windows)
	set (LIBRARY_PREFIX_DIR ${CMAKE_SOURCE_DIR}/../Library)
	set (CMAKE_PREFIX_PATH ${LIBRARY_PREFIX_DIR}/qt/5.9.2/msvc2017_64/lib/cmake)
	#set (EXECUTABLE_OUTPUT_PATH D:/application/myapp)
elseif (CMAKE_SYSTEM_NAME STREQUAL Linux)
	set (LIBRARY_PREFIX_DIR ${CMAKE_SOURCE_DIR}/../Library)
endif()

# CheckLanguage
include (CheckLanguage)
check_language(CUDA)
if (CMAKE_CUDA_COMPILER)
	message (STATUS "CUDA  found")
else ()
    message (STATUS "CUDA not found")
endif ()

# language
enable_language (CXX)
enable_language (CUDA)
set (CMAKE_CXX_STANDARD  17)
set (CMAKE_CUDA_STANDARD 17)

# QT
if (${OPENGL_RENDER})
	set (CMAKE_AUTOMOC ON)
	set (CMAKE_AUTOUIC ON)
	set (CMAKE_AUTORCC ON)
endif()
set (OPENGL_RENDER OFF CACHE BOOL "Bool")
# Package
find_package(CUDA REQUIRED)
if (${OPENGL_RENDER})
	find_package(Qt5 COMPONENTS Widgets REQUIRED)
	list (APPEND RENDER_DEFINITIONS "OPENGL_RENDER")
endif()

# Definition
list (APPEND RENDER_DEFINITIONS "RENDER_IS_GPU_CODE")
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
#set_property(GLOBAL PROPERTY USE_FOLDERS ON)
#默认编译静态库
#option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

#list (INSERT CMAKE_MODULE_PATH 0 "${CMAKE_SOURCE_DIR}/cmake")
#源码导入库
#add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/libsrc)

# Library
if (CMAKE_SYSTEM_NAME STREQUAL Windows)
set (LIBRARYS 
	assimp-vc143-mt
	double-conversion
	IlmImf-2_5
	Half-2_5
	Iex-2_5
	IexMath-2_5
	IlmThread-2_5
	Imath-2_5
	utf8proc_static
	zlibstatic
	RHI
)
elseif (CMAKE_SYSTEM_NAME STREQUAL Linux)
set (LIBRARYS 
	assimp
	double-conversion
	IlmImf-2_5
	Half-2_5
	Iex-2_5
	IexMath-2_5
	IlmThread-2_5
	Imath-2_5
	utf8proc
	zlibstatic
	RHI
)
endif()

if (${OPENGL_RENDER})
	list (APPEND LIBRARYS Qt5::Widgets)
endif ()

# Link path
set (LINK_PATH 
	 ${LIBRARY_PREFIX_DIR}/lib/
	 ${LIBRARY_PREFIX_DIR}/lib/win/assimp
	 ${LIBRARY_PREFIX_DIR}/lib/win/double-conversion
	 #${LIBRARY_PREFIX_DIR}/lib/win/glad
	 #${LIBRARY_PREFIX_DIR}/lib/win/glfw
	 #${LIBRARY_PREFIX_DIR}/lib/win/Imath
	 ${LIBRARY_PREFIX_DIR}/lib/win/OpenEXR
	 ${LIBRARY_PREFIX_DIR}/lib/win/utf8proc
	 ${LIBRARY_PREFIX_DIR}/lib/win/zlib
	 # ${LIBRARY_PREFIX_DIR}/lib/win/imgui
)

# Include path
set (INCLUDE_PATH
	#${PROJECT_SOURCE_DIR}
	${CMAKE_SOURCE_DIR}
	#${CMAKE_SOURCE_DIR}/LearnOpenGL
	${CMAKE_SOURCE_DIR}/Windows/Qt
	${CMAKE_SOURCE_DIR}/../libsrc
	${LIBRARY_PREFIX_DIR}
	#${LIBRARY_PREFIX_DIR}/${CMAKE_SOURCE_DIR}/
	${LIBRARY_PREFIX_DIR}/include
	${LIBRARY_PREFIX_DIR}/include/assimp
	${LIBRARY_PREFIX_DIR}/include/double-conversion
	#${LIBRARY_PREFIX_DIR}/include/glad
	#${LIBRARY_PREFIX_DIR}/include/glfw
	#${LIBRARY_PREFIX_DIR}/include/Imath
	${LIBRARY_PREFIX_DIR}/include/nanovdb
	${LIBRARY_PREFIX_DIR}/include/openEXR
	${LIBRARY_PREFIX_DIR}/include/stb
	${LIBRARY_PREFIX_DIR}/include/utf8proc
	${LIBRARY_PREFIX_DIR}/include/zlib
	#${LIBRARY_PREFIX_DIR}/include/imgui
)

if (CMAKE_SYSTEM_NAME STREQUAL Windows)
 
elseif (CMAKE_SYSTEM_NAME STREQUAL Darwin)
  
elseif (CMAKE_SYSTEM_NAME STREQUAL Linux)
  list (APPEND INCLUDE_PATH /usr/local/cuda/include)
endif()

file(GLOB  MAIN_SOURCE							${CMAKE_SOURCE_DIR}/*.h ${CMAKE_SOURCE_DIR}/*.cpp)
file(GLOB  MATH_SOURCE							${CMAKE_SOURCE_DIR}/Math/*.h ${CMAKE_SOURCE_DIR}/Math/*.cpp)
file(GLOB  UTIL_SOURCE							${CMAKE_SOURCE_DIR}/Util/*.h ${CMAKE_SOURCE_DIR}/Util/*.cpp)
file(GLOB  CAMERA_SOURCE						${CMAKE_SOURCE_DIR}/Camera/*.h ${CMAKE_SOURCE_DIR}/Camera/*.cpp)
file(GLOB  SAMLPER_SOURCE						${CMAKE_SOURCE_DIR}/Sampler/*.h ${CMAKE_SOURCE_DIR}/Sampler/*.cpp)
file(GLOB  SHAPE_SOURCE							${CMAKE_SOURCE_DIR}/Shape/*.h ${CMAKE_SOURCE_DIR}/Shape/*.cpp)
file(GLOB  MATERIAL_SOURCE						${CMAKE_SOURCE_DIR}/Material/*.h ${CMAKE_SOURCE_DIR}/Material/*.cpp)
file(GLOB  PRIMITIVE_SOURCE						${CMAKE_SOURCE_DIR}/Primitive/*.h ${CMAKE_SOURCE_DIR}/Primitive/*.cpp)
file(GLOB  SCENE_SOURCE							${CMAKE_SOURCE_DIR}/Scene/*.h ${CMAKE_SOURCE_DIR}/Scene/*.cpp)
file(GLOB  MEMORY_SOURCE						${CMAKE_SOURCE_DIR}/Memory/*.h ${CMAKE_SOURCE_DIR}/Memory/*.cpp)
file(GLOB  MEMORY_GPU_SOURCE					${CMAKE_SOURCE_DIR}/Memory/GPU/*.h ${CMAKE_SOURCE_DIR}/Memory/GPU/*.cpp)
file(GLOB  MEMORY_CPU_SOURCE					${CMAKE_SOURCE_DIR}/Memory/CPU/*.h ${CMAKE_SOURCE_DIR}/Memory/CPU/*.cpp)
file(GLOB  CONTAINER_SOURCE						${CMAKE_SOURCE_DIR}/Container/*.h ${CMAKE_SOURCE_DIR}/Container/*.cpp)
file(GLOB  INTERSECTION_SOURCE					${CMAKE_SOURCE_DIR}/Intersection/*.h ${CMAKE_SOURCE_DIR}/Intersection/*.cpp)
file(GLOB  RENDER_SOURCE						${CMAKE_SOURCE_DIR}/Render/*.h ${CMAKE_SOURCE_DIR}/Render/*.cpp)
file(GLOB  LOADER_SOURCE						${CMAKE_SOURCE_DIR}/Loader/*.h ${CMAKE_SOURCE_DIR}/Loader/*.cpp)
file(GLOB  TEXTURE_SOURCE						${CMAKE_SOURCE_DIR}/Texture/*.h ${CMAKE_SOURCE_DIR}/Texture/*.cpp)
file(GLOB  FILM_SOURCE							${CMAKE_SOURCE_DIR}/Film/*.h ${CMAKE_SOURCE_DIR}/Film/*.cpp)
file(GLOB  MEDIA_SOURCE							${CMAKE_SOURCE_DIR}/Medium/*.h ${CMAKE_SOURCE_DIR}/Medium/*.cpp)
file(GLOB  LIGHT_SOURCE							${CMAKE_SOURCE_DIR}/Light/*.h ${CMAKE_SOURCE_DIR}/Light/*.cpp)
file(GLOB  RAY_SOURCE							${CMAKE_SOURCE_DIR}/Ray/*.h ${CMAKE_SOURCE_DIR}/Ray/*.cpp)
file(GLOB  TEST_SOURCE							${CMAKE_SOURCE_DIR}/Test/*.h ${CMAKE_SOURCE_DIR}/Test/*.cpp)
file(GLOB  IMAGE_SOURCE							${CMAKE_SOURCE_DIR}/Image/*.h ${CMAKE_SOURCE_DIR}/Image/*.cpp)
file(GLOB  MESH_SOURCE							${CMAKE_SOURCE_DIR}/Mesh/*.h ${CMAKE_SOURCE_DIR}/Mesh/*.cpp)
file(GLOB  BXDF_SOURCE							${CMAKE_SOURCE_DIR}/BxDF/*.h ${CMAKE_SOURCE_DIR}/BxDF/*.cpp)
file(GLOB  INTEGRATOR_SOURCE					${CMAKE_SOURCE_DIR}/Integrator/*.h ${CMAKE_SOURCE_DIR}/Integrator/*.cpp)
file(GLOB  COOKTORRANCE_SOURCE					${CMAKE_SOURCE_DIR}/CookTorrance/*.h ${CMAKE_SOURCE_DIR}/CookTorrance/*.cpp)
file(GLOB  HDRI_SOURCE							${CMAKE_SOURCE_DIR}/HDRI/*.h ${CMAKE_SOURCE_DIR}/HDRI/*.cpp)
file(GLOB  QT_RESOURCE_FILE                     ${CMAKE_SOURCE_DIR}/*.qrc)

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

file(GLOB  TYPE_SOURCE					${CMAKE_SOURCE_DIR}/Type/*.h ${CMAKE_SOURCE_DIR}/Type/*.cpp)
source_group ("Type" FILES ${TYPE_SOURCE})	

file(GLOB  WEATHER_SOURCE					${CMAKE_SOURCE_DIR}/Weather/*.h ${CMAKE_SOURCE_DIR}/Weather/*.cpp)
source_group ("Weather" FILES ${WEATHER_SOURCE})	

#file(GLOB  PARTICLE_SYSTEM_SOURCE	${CMAKE_SOURCE_DIR}/ParticleSystem/*.h ${CMAKE_SOURCE_DIR}/ParticleSystem/*.cpp)
#source_group ("Particle System" FILES ${PARTICLE_SYSTEM_SOURCE})	

if (OPENGL_RENDER)
	#file(GLOB  LEARNOPENGL_COMMON_SOURCE			${CMAKE_SOURCE_DIR}/LearnOpenGL/Common/*.h ${CMAKE_SOURCE_DIR}/LearnOpenGL/Common/*.cpp)
	file(GLOB  WINDOWS_SOURCE						${CMAKE_SOURCE_DIR}/Windows/*.h ${CMAKE_SOURCE_DIR}/Windows/*.cpp)
	#file(GLOB  WINDOWS_GLFW_SOURCE					${CMAKE_SOURCE_DIR}/Windows/GLFW/*.h  ${CMAKE_SOURCE_DIR}/Windows/GLFW/*.cpp)
	file(GLOB  WINDOWS_QT_SOURCE					${CMAKE_SOURCE_DIR}/Windows/QT/*.h ${CMAKE_SOURCE_DIR}/Windows/QT/*.cpp)
	#source_group ("LearnOpenGL/Common" FILES ${LEARNOPENGL_COMMON_SOURCE})	
	source_group ("Windows" FILES ${WINDOWS_SOURCE})
	#source_group ("Windows/GLFW" FILES ${WINDOWS_GLFW_SOURCE})
	source_group ("Windows/QT" FILES ${WINDOWS_QT_SOURCE})


	#set (imgui_src_list
	#${CMAKE_SOURCE_DIR}/Imgui/imgui.cpp
	#${CMAKE_SOURCE_DIR}/Imgui/imgui_demo.cpp
	#${CMAKE_SOURCE_DIR}/Imgui/imgui_draw.cpp
	#${CMAKE_SOURCE_DIR}/Imgui/imgui_tables.cpp
	#${CMAKE_SOURCE_DIR}/Imgui/imgui_widgets.cpp
	#${CMAKE_SOURCE_DIR}/Imgui/ImGuiFileDialog.cpp
	#${CMAKE_SOURCE_DIR}/Imgui/imgui_widgets.cpp
	#${CMAKE_SOURCE_DIR}/Imgui/imgui_widgets.cpp
	#${CMAKE_SOURCE_DIR}/Imgui/backends/imgui_impl_glfw.cpp
	#)
	#
	#set(imgui_glfw_opengl3_src_list
	#	${imgui_src_list}
	#	${CMAKE_SOURCE_DIR}/Imgui/backends/imgui_impl_opengl3.cpp
	#)
	#
	#set(imgui_glfw_opengl2_src_list
	#	${imgui_src_list}
	#	${CMAKE_SOURCE_DIR}/Imgui/backends/imgui_impl_opengl2.cpp
	#)
	#
	#set(imgui_glfw_vulkan_src_list
	#	${imgui_src_list}
	#	${CMAKE_SOURCE_DIR}/Imgui/backends/imgui_impl_vulkan.cpp
	#)
	#
	#set(imgui_win32_dx9_src_list
	#	${imgui_src_list}
	#	${CMAKE_SOURCE_DIR}/Imgui/backends/imgui_impl_dx9.cpp
	#	${CMAKE_SOURCE_DIR}/Imgui/backends/imgui_impl_win32.cpp
	#)
	#
	#set(imgui_win32_dx10_src_list
	#	${imgui_src_list}
	#	${CMAKE_SOURCE_DIR}/Imgui/backends/imgui_impl_dx10.cpp
	#	${CMAKE_SOURCE_DIR}/Imgui/backends/imgui_impl_win32.cpp
	#)
	#
	#set(imgui_win32_dx11_src_list
	#	${imgui_src_list}
	#	${CMAKE_SOURCE_DIR}/Imgui/backends/imgui_impl_dx11.cpp
	#	${CMAKE_SOURCE_DIR}/Imgui/backends/imgui_impl_win32.cpp
	#)
	#
	#set(imgui_win32_dx12_src_list
	#	${imgui_src_list}
	#	${CMAKE_SOURCE_DIR}/Imgui/backends/imgui_impl_dx12.cpp
	#	${CMAKE_SOURCE_DIR}/Imgui/backends/imgui_impl_win32.cpp
	#)
	#
	#set (IMGUI_SOURCE  ${imgui_glfw_opengl3_src_list})
	##file(GLOB  IMGUI_SOURCE	 ${imgui_glfw_opengl3_src_list})
	#source_group ("ImGUI" FILES ${IMGUI_SOURCE})	
	#message(STATUE "IMGUI_SOURCE ${IMGUI_SOURCE}")
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

set_source_files_properties (
	${GPU_SRC_LIST}
	PROPERTIES LANGUAGE CUDA
)

set (SRC_LIST
  ${GPU_SRC_LIST}
  ${CPU_SRC_LIST}
  ${CMAKE_SOURCE_DIR}/../libsrc/lodepng/lodepng.cpp
  ${CMAKE_SOURCE_DIR}/main.cpp
)

add_library (cuda_build_configuration INTERFACE)
target_compile_options (
            cuda_build_configuration
            INTERFACE
                "$<$<COMPILE_LANGUAGE:CUDA>:--use_fast_math;--expt-relaxed-constexpr;--extended-lambda;--forward-unknown-to-host-compiler>"
				"$<$<COMPILE_LANGUAGE:CUDA>:$<IF:$<AND:$<CONFIG:Debug>,$<NOT:$<BOOL:$<TARGET_PROPERTY:CUDA_PTX_COMPILATION>>>>,-G;-g,-lineinfo;-maxrregcount;128>>"   
	)
	
get_target_property (v1 cuda_build_configuration INTERFACE_COMPILE_OPTIONS)	
message (STATUS "cuda_build_configuration ${v1}")	
		
add_executable(${PROJECT_NAME} ${SRC_LIST})
target_include_directories(${PROJECT_NAME} PUBLIC  ${INCLUDE_PATH})
target_link_directories(${PROJECT_NAME} PUBLIC  ${LINK_PATH})
target_link_libraries(${PROJECT_NAME} ${LIBRARYS} $<$<BOOL:PBRT_CUDA_ENABLED>:cuda_build_configuration>)
target_compile_definitions (${PROJECT_NAME} PRIVATE ${RENDER_DEFINITIONS})


message(STATUS "${PROJECT_NAME} INCLUDE_DIRECTORY is ${INCLUDE_DIRECTORY}")
message(STATUS "${PROJECT_NAME} SRC_LIST is ${SRC_LIST}")
message(STATUS "${PROJECT_NAME} PROJECT_SOURCE_DIR is ${PROJECT_SOURCE_DIR}")

set_property(TARGET ${PROJECT_NAME} PROPERTY CUDA_SEPARABLE_COMPILATION ON)

