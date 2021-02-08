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
	include/factory/graphics_factory.h
	include/command/command.h
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
	src/factory/graphics_factory.cpp
	src/command/command.cpp
)
