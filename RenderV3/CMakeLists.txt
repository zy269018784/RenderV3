cmake_minimum_required(VERSION 3.14)
project(Render3)


 
enable_language (CXX)
set (CMAKE_CXX_STANDARD  17)
if (${USE_QT})
	#set (CMAKE_PREFIX_PATH D:/application/qt6/6.8.0/msvc2022_64/lib/cmake)
	
	set (CMAKE_PREFIX_PATH C:/Development/Qt6/6.8.3/msvc2022_64/lib/cmake)
	set (CMAKE_AUTOMOC ON)
	set (CMAKE_AUTOUIC ON)
	set (CMAKE_AUTORCC ON)
	find_package(Qt6 COMPONENTS Widgets Qml Quick QuickControls2 QuickWidgets REQUIRED)
	list (APPEND RENDER_DEFINITIONS "USE_QT")
endif ()

set (SRC_PREFIX ${CMAKE_SOURCE_DIR}/Source)


#add_subdirectory(${PROJECT_SOURCE_DIR}/../QXlsx QXlsx)
add_subdirectory(${PROJECT_SOURCE_DIR}/../RHI2  RHI2)
add_subdirectory(${PROJECT_SOURCE_DIR}/../Pbrt Pbrt)
add_subdirectory(${PROJECT_SOURCE_DIR}/../Modules/RHIForQt  RHIForQt)



set (INCLUDE_PATH
	${SRC_PREFIX}
	${SRC_PREFIX}/Render
	${SRC_PREFIX}/Scene
	${SRC_PREFIX}/Editor
	${SRC_PREFIX}/SceneFileFormat
	${SRC_PREFIX}/ErrorCode
	${CMAKE_SOURCE_DIR}/../Library/Windows/glm/include
	${CMAKE_SOURCE_DIR}/../Library/Windows/boost/include
)

set (LINK_PATH

)

#set(QML_IMPORT_PATH D:/application/qt6/6.8.0/msvc2022_64/qml)
#set(QML2_IMPORT_PATH D:/application/qt6/6.8.0/msvc2022_64/qml)

set (LIBRARYS 
	RHI2
	RenderLib
	RHIForQt
)

if (CMAKE_SYSTEM_NAME STREQUAL Windows)
	list (APPEND LIBRARYS
		libboost_system-vc143-mt-x64-1_85
		libboost_json-vc143-mt-x64-1_85
		libboost_filesystem-vc143-mt-x64-1_85
		libboost_thread-vc143-mt-x64-1_85
	)
	list (APPEND LINK_PATH
		${CMAKE_SOURCE_DIR}/../Library/Windows/boost/lib
	)
elseif (CMAKE_SYSTEM_NAME STREQUAL Linux)
	list (APPEND LIBRARYS
		boost_system
		boost_json
		boost_filesystem
		boost_thread
		boost_iostreams
	)
endif ()


if (${USE_QT})
	list (APPEND INCLUDE_PATH
		${CMAKE_SOURCE_DIR}/../Library/Windows/QXlsx/include
		${CMAKE_SOURCE_DIR}/../Library/Windows/sqlite/include
	)

	list (APPEND LINK_PATH
		${CMAKE_SOURCE_DIR}/../Library/Windows/QXlsx/lib
		${CMAKE_SOURCE_DIR}/../Library/Windows/sqlite/lib
	)

	list (APPEND LIBRARYS
		QXlsxQt6.lib
		Qt6::Widgets
		Qt6::QuickWidgets
		Qt6::Quick
		Qt6::Qml
		Qt6::QuickControls2
		sqlite3.lib
	)
endif ()

#set (SRC_LIST
#  ${CMAKE_SOURCE_DIR}/Source/main.cpp
#)

if(${USE_QT})
	file(GLOB_RECURSE SRC_LIST ${SRC_PREFIX} *.cpp *.c *.h *.qrc)
else()
 	file(GLOB_RECURSE SRC_LIST ${SRC_PREFIX} ${SRC_PREFIX}/*.cpp ${SRC_PREFIX}/*.c ${SRC_PREFIX}/*.h ${SRC_PREFIX}/*.qrc)
endif()

message (STATUS "SRC_LIST ${SRC_LIST}")

if (CMAKE_SYSTEM_NAME STREQUAL Windows)
	#source_group (TREE "${SRC_PREFIX}" PREFIX "Render3" FILES ${SRC_LIST})
	source_group (TREE "${SRC_PREFIX}"  FILES ${SRC_LIST})
endif ()

add_executable(${PROJECT_NAME} ${SRC_LIST} )
target_compile_definitions (${PROJECT_NAME} PUBLIC ${RENDER_DEFINITIONS})
target_include_directories(${PROJECT_NAME} PUBLIC  ${INCLUDE_PATH})
target_link_directories(${PROJECT_NAME} PUBLIC  ${LINK_PATH})
target_link_libraries(${PROJECT_NAME} ${LIBRARYS})

#[[
if(${USE_QT})
	#自动执行deployqt，拷贝依赖  -- start
	get_target_property(_qmake_executable Qt6::qmake IMPORTED_LOCATION)
	get_filename_component(_qt_bin_dir "${_qmake_executable}" DIRECTORY)
	find_program(DEPLOYQT_EXECUTABLE NAMES windeployqt macdeployqt HINTS "${_qt_bin_dir}")

	if(APPLE)
	    configure_file(${CMAKE_SOURCE_DIR}/Info.plist.in ${CMAKE_BINARY_DIR}/Info.plist)
	    set_target_properties(${PROJECT_NAME} PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${CMAKE_BINARY_DIR}/Info.plist)
	    add_custom_command(TARGET ${PROJECT_NAME}
	        POST_BUILD
	        COMMAND ${DEPLOYQT_EXECUTABLE} "$<TARGET_BUNDLE_DIR:${PROJECT_NAME}>" -qmldir=${CMAKE_SOURCE_DIR}
	    )
	    #todo: copy dll to apple folder
	elseif(WIN32)
	    add_custom_command(TARGET ${PROJECT_NAME}
	        POST_BUILD
	        COMMAND ${DEPLOYQT_EXECUTABLE} "$<TARGET_FILE:${PROJECT_NAME}>"
	            --$<LOWER_CASE:$<CONFIG>> --qmldir ${CMAKE_SOURCE_DIR}
	    )
	endif()
	#自动执行deployqt，拷贝依赖  -- end

	#编译glsl
	set(SHADER_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../asset/shaders)
	add_custom_target(RunBatCommands ALL
    	COMMAND ${CMAKE_COMMAND} -E echo "compile shaders start "
    	COMMAND glslangValidator.exe --target-env vulkan1.2 ${SHADER_ROOT_DIR}/glsl/rtscene/rtscene.rgen  -o ${SHADER_ROOT_DIR}/rtscene/rtscene_rgs.spv
		COMMAND glslangValidator.exe --target-env vulkan1.2 ${SHADER_ROOT_DIR}/glsl/rtscene/rtscene.rmiss -o ${SHADER_ROOT_DIR}/rtscene/rtscene_rms.spv
		COMMAND glslangValidator.exe --target-env vulkan1.2 ${SHADER_ROOT_DIR}/glsl/rtscene/shadow.rmiss  -o ${SHADER_ROOT_DIR}/rtscene/shadow_rms.spv 
		COMMAND glslangValidator.exe --target-env vulkan1.2 ${SHADER_ROOT_DIR}/glsl/rtscene/rtscene.rchit -o ${SHADER_ROOT_DIR}/rtscene/rtscene_rhs.spv
		COMMAND glslangValidator.exe --target-env vulkan1.2 ${SHADER_ROOT_DIR}/glsl/rtscene/rtscene.rahit -o ${SHADER_ROOT_DIR}/rtscene/rtscene_ras.spv
    	WORKING_DIRECTORY ${SHADER_ROOT_DIR}/glsl/rtscene
    	COMMENT "compile..."
    	VERBATIM
	)

	# 设置资源目录路径
	set(ASSET_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../asset")
	
	# 创建拷贝命令
	add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
	    COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:${PROJECT_NAME}>/asset/"
	    COMMAND ${CMAKE_COMMAND} -E copy_directory
	        "${ASSET_DIR}"
	        "$<TARGET_FILE_DIR:${PROJECT_NAME}>/asset/"
	    COMMENT "Copying assets directory to build folder"
	    VERBATIM
	)

	# 自动拷贝Windows目录下面的dll，查找所有子目录中的DLL文件（排除隐藏目录）
	file(GLOB DLL_FILES
    	LIST_DIRECTORIES false
    	"${CMAKE_CURRENT_SOURCE_DIR}/../Library/Windows/*.dll"
    	"${CMAKE_CURRENT_SOURCE_DIR}/../Library/Windows/*/*.dll"
    	"${CMAKE_CURRENT_SOURCE_DIR}/../Library/Windows/*/*/*.dll"
	)
	# 创建自定义目标用于集中管理拷贝操作
	add_custom_target(CopyDLLs ALL
	    COMMENT "Copying DLL files to output directory"
	    VERBATIM
	)
	# 为每个DLL添加拷贝命令
	foreach(DLL ${DLL_FILES})
	    add_custom_command(TARGET CopyDLLs POST_BUILD
	        # COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
	        COMMAND ${CMAKE_COMMAND} -E copy_if_different
	            "${DLL}"
	            "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
	        COMMENT "Copying ${DLL}"
	        VERBATIM
	    )
	endforeach()
endif()


install (DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../asset" DESTINATION "$<TARGET_FILE_DIR:${PROJECT_NAME}>/../asset")
]]