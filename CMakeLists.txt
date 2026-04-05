# SPDX-License-Identifier: GPL-3.0-only
cmake_minimum_required(VERSION 3.20.0)

# Board root — this repo contains the `boards/akiraconsole` board definition.
list(APPEND BOARD_ROOT ${CMAKE_CURRENT_LIST_DIR})
list(APPEND DTS_ROOT   ${CMAKE_CURRENT_LIST_DIR})

# AkiraOS is fetched by west into the `akiraos/` sub-directory.
# MODULE_EXT_ROOT is set on the command-line by the CI workflow and build.sh
# so WAMR and other out-of-tree modules are found correctly.
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(akiraconsole_firmware VERSION 1.4.9)

# AkiraOS application sources are provided by the platform repo.
# We include them via their CMakeLists.txt rather than duplicating source lists.
include(${CMAKE_CURRENT_LIST_DIR}/../akiraos/CMakeLists.txt OPTIONAL)
