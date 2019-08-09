cmake_minimum_required(VERSION 3.13)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)
get_filename_component(PARENT_SOURCE_DIR ${CMAKE_SOURCE_DIR} DIRECTORY)
get_filename_component(PARENT_BINARY_DIR ${CMAKE_BINARY_DIR} DIRECTORY)

include(package_info)
include(package_check_info)
if(COMMAND callback_start)
	callback_start()
endif()

project(${PACKAGE_NAME})

include(package_options)

include(find_ncine)
if(NOT NCINE_DYNAMIC_LIBRARY)
	include(package_imported_targets)
endif()

if(NOT IS_DIRECTORY ${PACKAGE_DATA_DIR})
	message(WARNING "${PACKAGE_NAME} data directory not found at: ${PACKAGE_DATA_DIR}")
else()
	message(STATUS "${PACKAGE_NAME} data directory: ${PACKAGE_DATA_DIR}")
endif()

include(package_get_version)
include(package_installation)

if(COMMAND callback_before_target)
	callback_before_target()
endif()

add_executable(${PACKAGE_EXE_NAME} ${PACKAGE_SOURCES})
include(package_generated_sources)
target_sources(${PACKAGE_EXE_NAME} PRIVATE ${GENERATED_SOURCES})

if(NOT DEFAULT_DATA_DIR_DIST) # Don't set the startup project if it wouldn't find the data directory
	set_property(DIRECTORY ${CMAKE_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PACKAGE_EXE_NAME})
endif()

target_include_directories(${PACKAGE_EXE_NAME} PRIVATE ${PACKAGE_INCLUDE_DIRS})
if(IS_DIRECTORY ${GENERATED_INCLUDE_DIR})
	target_include_directories(${PACKAGE_EXE_NAME} PRIVATE ${GENERATED_INCLUDE_DIR})
endif()
target_link_libraries(${PACKAGE_EXE_NAME} PRIVATE ncine::ncine_main ncine::ncine)

target_compile_definitions(${PACKAGE_EXE_NAME} PRIVATE "$<$<CONFIG:Debug>:${PACKAGE_UPPER_NAME}_DEBUG>")

if(MSVC)
	target_compile_options(${PACKAGE_EXE_NAME} PRIVATE /MP)
endif()

if(APPLE)
	file(RELATIVE_PATH RELPATH_TO_LIB ${CMAKE_INSTALL_PREFIX}/${RUNTIME_INSTALL_DESTINATION}/ ${CMAKE_INSTALL_PREFIX}/${LIBRARY_INSTALL_DESTINATION})
	set_target_properties(${PACKAGE_EXE_NAME} PROPERTIES INSTALL_RPATH "@executable_path/${RELPATH_TO_LIB}")
endif()

if(DEFAULT_DATA_DIR_DIST)
	if(MSVC OR APPLE)
		# Relative path from tests to data on Windows and OS X, where the user can choose the installation directory
		file(RELATIVE_PATH PACKAGE_DEFAULT_DATA_DIR
			${CMAKE_INSTALL_PREFIX}/${RUNTIME_INSTALL_DESTINATION}
			${CMAKE_INSTALL_PREFIX}/${DATA_INSTALL_DESTINATION}) # Always strips trailing slash
		set(PACKAGE_DEFAULT_DATA_DIR "${PACKAGE_DEFAULT_DATA_DIR}/")
	else()
		set(PACKAGE_DEFAULT_DATA_DIR "${CMAKE_INSTALL_PREFIX}/${DATA_INSTALL_DESTINATION}/")
	endif()
elseif(NOT PACKAGE_DEFAULT_DATA_DIR)
	set(PACKAGE_DEFAULT_DATA_DIR "${PACKAGE_DATA_DIR}/data/")
endif()

if(PACKAGE_DEFAULT_DATA_DIR)
	file(TO_CMAKE_PATH "${PACKAGE_DEFAULT_DATA_DIR}" PACKAGE_DEFAULT_DATA_DIR) # Always strips trailing slash
	set(PACKAGE_DEFAULT_DATA_DIR "${PACKAGE_DEFAULT_DATA_DIR}/")

	message(STATUS "Default data directory: ${PACKAGE_DEFAULT_DATA_DIR}")
	target_compile_definitions(${PACKAGE_EXE_NAME} PRIVATE "PACKAGE_DEFAULT_DATA_DIR=\"${PACKAGE_DEFAULT_DATA_DIR}\"")
endif()

if(COMMAND callback_after_target)
	callback_after_target()
endif()

if(EMSCRIPTEN)
	target_compile_options(${PACKAGE_EXE_NAME} PRIVATE "SHELL:--preload-file ${PACKAGE_DATA_DIR}/data@ --no-heap-copy")
	target_link_options(${PACKAGE_EXE_NAME} PRIVATE "SHELL:--preload-file ${PACKAGE_DATA_DIR}/data@ --no-heap-copy")
	configure_file(${CMAKE_SOURCE_DIR}/emscripten_shell.html.in ${CMAKE_BINARY_DIR}/${PACKAGE_EXE_NAME}.html @ONLY)
	install(FILES ${CMAKE_BINARY_DIR}/${PACKAGE_EXE_NAME}.html DESTINATION ${RUNTIME_INSTALL_DESTINATION})
	install(FILES ${CMAKE_BINARY_DIR}/${PACKAGE_EXE_NAME}.data DESTINATION ${RUNTIME_INSTALL_DESTINATION})
	install(FILES ${CMAKE_BINARY_DIR}/${PACKAGE_EXE_NAME}.wasm DESTINATION ${RUNTIME_INSTALL_DESTINATION})
	if(EXISTS ${PACKAGE_DATA_DIR}/icons/icon.ico)
		file(COPY ${PACKAGE_DATA_DIR}/icons/icon.ico DESTINATION ${CMAKE_BINARY_DIR})
		file(RENAME ${CMAKE_BINARY_DIR}/icon.ico ${CMAKE_BINARY_DIR}/favicon.ico)
		install(FILES ${CMAKE_BINARY_DIR}/favicon.ico DESTINATION ${RUNTIME_INSTALL_DESTINATION})
	endif()
endif()

install(TARGETS ${PACKAGE_EXE_NAME} RUNTIME DESTINATION ${RUNTIME_INSTALL_DESTINATION})
install(FILES README.md DESTINATION ${README_INSTALL_DESTINATION})
if((MSVC OR APPLE) AND EXISTS "${CMAKE_SOURCE_DIR}/LICENSE")
	install(FILES LICENSE DESTINATION . RENAME LICENSE.txt)
endif()
if(NOT EMSCRIPTEN)
	install(DIRECTORY ${PACKAGE_DATA_DIR}/data/ DESTINATION ${DATA_INSTALL_DESTINATION})
endif()
install(FILES ${PACKAGE_DATA_DIR}/README.md DESTINATION ${DATA_INSTALL_DESTINATION})
if(IS_DIRECTORY ${NCINE_SHADERS_DIR})
	install(DIRECTORY ${NCINE_SHADERS_DIR} DESTINATION ${SHADERS_INSTALL_DESTINATION})
endif()

if(MSVC)
	install(FILES ${NCINE_LOCATION} DESTINATION ${RUNTIME_INSTALL_DESTINATION})
	install(DIRECTORY ${MSVC_BINDIR}/ DESTINATION ${RUNTIME_INSTALL_DESTINATION} FILES_MATCHING PATTERN "*.dll")
elseif(APPLE)
	install(FILES ${NCINE_LOCATION} DESTINATION ${LIBRARY_INSTALL_DESTINATION})
	install(DIRECTORY ${FRAMEWORKS_DIR}/ DESTINATION ${FRAMEWORKS_INSTALL_DESTINATION})
endif()

include(package_copy_targets)
include(package_build_android)
include(package_strip_binaries)
if(COMMAND callback_end)
	callback_end()
endif()
