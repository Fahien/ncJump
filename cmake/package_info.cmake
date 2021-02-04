set(PACKAGE_NAME "ncJump")
set(PACKAGE_EXE_NAME "ncjump")
set(PACKAGE_VENDOR "Antonio Caggiano")
set(PACKAGE_COPYRIGHT "Copyright © 2020-2021 ${PACKAGE_VENDOR}")
set(PACKAGE_DESCRIPTION "A jumping project made with the nCine")
set(PACKAGE_HOMEPAGE "https://antoniocaggiano.eu")
set(PACKAGE_REVERSE_DNS "eu.antoniocaggiano.ncjump")

set(PACKAGE_INCLUDE_DIRS include)

set(PACKAGE_SOURCES
	include/types.h
	include/main.h
	include/config.h
	include/input.h
	include/game.h
	include/entity.h
	include/tileset.h
	include/tilemap.h
	include/editor.h
	include/physics.h
	include/camera.h
	include/component/transform.h
	include/component/graphics.h
	include/component/physics.h
	include/component/state.h
	include/serialization/file.h
	include/serialization/config.h
	include/serialization/tileset.h
	include/serialization/tilemap.h
	include/system/physics/destruction.h
	include/factory/entity_factory.h
	src/main.cpp
	src/input.cpp
	src/game.cpp
	src/entity.cpp
	src/tileset.cpp
	src/tilemap.cpp
	src/editor.cpp
	src/physics.cpp
	src/camera.cpp
	src/component/transform.cpp
	src/component/graphics.cpp
	src/component/physics.cpp
	src/component/state.cpp
	src/serialization/file.cpp
	src/serialization/config.cpp
	src/serialization/tileset.cpp
	src/serialization/tilemap.cpp
	src/system/physics/destruction.cpp
	src/factory/entity_factory.cpp
)

function(callback_before_target)
	if(EMSCRIPTEN)
		find_path(EMSCRIPTEN_LIBDIR_EXT
			NAMES libbox2d.a
			PATHS ${NCINE_EXTERNAL_DIR} ${PARENT_SOURCE_DIR}/nCine-external-emscripten ${PARENT_BINARY_DIR}/nCine-external-emscripten
			PATH_SUFFIXES lib
			DOC "Path to the nCine external Emscripten libraries directory"
			NO_DEFAULT_PATH # To avoid finding MSYS/MinGW libraries
			NO_CMAKE_FIND_ROOT_PATH) # To avoid using the cross-compiler root

		get_filename_component(EXTERNAL_EMSCRIPTEN_DIR_EXT ${EMSCRIPTEN_LIBDIR_EXT} DIRECTORY)

		add_library(box2d::box2d STATIC IMPORTED)
		set_target_properties(box2d::box2d PROPERTIES
			IMPORTED_LOCATION ${EMSCRIPTEN_LIBDIR_EXT}/libbox2d.a
			INTERFACE_INCLUDE_DIRECTORIES "${EXTERNAL_EMSCRIPTEN_DIR_EXT}/include")
		set(BOX2D_FOUND 1)
	elseif(ANDROID)
		if(EXISTS ${EXTERNAL_ANDROID_DIR}/box2d/${ANDROID_ABI}/libbox2d.a)
			add_library(box2d::box2d STATIC IMPORTED)
			set_target_properties(box2d::box2d PROPERTIES
				IMPORTED_LOCATION ${EXTERNAL_ANDROID_DIR}/box2d/${ANDROID_ABI}/libbox2d.a
				INTERFACE_INCLUDE_DIRECTORIES "${EXTERNAL_ANDROID_DIR}/box2d/include")
			set(BOX2D_FOUND 1)
		endif()
	elseif(MSVC)
		find_path(MSVC_BINDIR_EXT
			NAMES box2d.dll
			PATHS ${NCINE_EXTERNAL_DIR} ${PARENT_SOURCE_DIR}/nCine-external ${PARENT_BINARY_DIR}/nCine-external
			PATH_SUFFIXES bin bin/${MSVC_ARCH_SUFFIX}
			DOC "Path to the nCine external MSVC DLL libraries directory"
			NO_DEFAULT_PATH) # To avoid finding MSYS/MinGW libraries

		find_path(MSVC_LIBDIR
			NAMES box2d.lib
			PATHS ${NCINE_EXTERNAL_DIR} ${PARENT_SOURCE_DIR}/nCine-external ${PARENT_BINARY_DIR}/nCine-external
			PATH_SUFFIXES lib/${MSVC_ARCH_SUFFIX}
			DOC "Path to the nCine external MSVC import libraries directory"
			NO_DEFAULT_PATH) # To avoid finding MSYS/MinGW libraries

		get_filename_component(EXTERNAL_MSVC_DIR ${MSVC_LIBDIR} DIRECTORY)
		get_filename_component(EXTERNAL_MSVC_DIR ${EXTERNAL_MSVC_DIR} DIRECTORY)

		if(EXISTS ${MSVC_LIBDIR}/box2d.lib AND EXISTS ${MSVC_BINDIR_EXT}/box2d.dll)
			add_library(box2d::box2d SHARED IMPORTED)
			set_target_properties(box2d::box2d PROPERTIES
				IMPORTED_IMPLIB ${MSVC_LIBDIR}/box2d.lib
				IMPORTED_LOCATION ${MSVC_BINDIR_EXT}/box2d.dll
				INTERFACE_INCLUDE_DIRECTORIES "${EXTERNAL_MSVC_DIR}/include")
			set(BOX2D_FOUND 1)
		endif()
	else()
		find_package(box2d REQUIRED)
	endif()

	if(APPLE)
		set(CMAKE_MACOSX_RPATH ON)

		if(BOX2D_FOUND)
			set_target_properties(box2d::box2d PROPERTIES
				IMPORTED_LOCATION ${BOX2D_LIBRARY}/box2d
				IMPORTED_LOCATION_RELEASE ${BOX2D_LIBRARY}/box2d
				IMPORTED_LOCATION_DEBUG ${BOX2D_LIBRARY}/box2d)
		endif()
	endif()
endfunction()

function(callback_after_target)
	target_compile_features(${PACKAGE_EXE_NAME} PUBLIC cxx_std_17)
	target_link_libraries(${PACKAGE_EXE_NAME} PRIVATE box2d::box2d)
	if(MINGW)
		# MinGW and MSYS haven't yet updated Box2D to a version >=2.4.1
		target_compile_definitions(${PACKAGE_EXE_NAME} PRIVATE "BOX2D_PRE241")
	endif()

	if(NOT CMAKE_SYSTEM_NAME STREQUAL "Android")
		include(custom_nlohmannjson)

		if(IS_DIRECTORY ${PACKAGE_DATA_DIR})
			set(IMAGES_FILES
				"${PACKAGE_DATA_DIR}/data/img/hero/herochar_idle_anim_strip_4.png"
				"${PACKAGE_DATA_DIR}/data/img/hero/herochar_run_anim_strip_6.png"
				"${PACKAGE_DATA_DIR}/data/img/hero/herochar_jump_up_anim_strip_3.png"
				"${PACKAGE_DATA_DIR}/data/img/hero/herochar_jump_down_anim_strip_3.png"
				"${PACKAGE_DATA_DIR}/data/img/hero/herochar_pushing_foward_anim_strip_6.png"
				"${PACKAGE_DATA_DIR}/data/img/tile/tileset.png"
				"${PACKAGE_DATA_DIR}/data/img/tile/background.png"
			)
			file(GLOB JSON_FILES "${PACKAGE_DATA_DIR}/data/*.json")
		endif()
		set(PACKAGE_ANDROID_ASSETS ${IMAGES_FILES} ${JSON_FILES} CACHE STRING "" FORCE)
	else()
		target_include_directories(${PACKAGE_EXE_NAME} PRIVATE ${GENERATED_INCLUDE_DIR}/../../nlohmannjson-src/include)
	endif()
endfunction()
