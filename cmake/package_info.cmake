set(PACKAGE_NAME "ncJump")
set(PACKAGE_EXE_NAME "ncjump")
set(PACKAGE_VENDOR "Antonio Caggiano")
set(PACKAGE_COPYRIGHT "Copyright © 2020 ${PACKAGE_VENDOR}")
set(PACKAGE_DESCRIPTION "A jumping project made with the nCine")
set(PACKAGE_HOMEPAGE "https://antoniocaggiano.eu")
set(PACKAGE_REVERSE_DNS "eu.antoniocaggiano.ncjump")

set(PACKAGE_INCLUDE_DIRS include)

set(PACKAGE_SOURCES
	include/types.h
	include/main.h
	include/config.h
	include/game.h
	include/entity.h
	include/tileset.h
	include/tilemap.h
	include/editor.h
	src/main.cpp
	src/game.cpp
	src/entity.cpp
	src/tileset.cpp
	src/tilemap.cpp
	src/editor.cpp
)
