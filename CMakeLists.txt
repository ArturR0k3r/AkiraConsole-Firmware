# SPDX-License-Identifier: GPL-3.0-only
cmake_minimum_required(VERSION 3.20.0)

# Suppress Kconfig warnings-as-errors so undefined symbols (during incremental
# migration) do not abort the configuration phase.
set(KCONFIG_WARN_UNDEF OFF CACHE BOOL "" FORCE)
set(KCONFIG_WERROR     OFF CACHE BOOL "" FORCE)

# Board root — this repo contains the `boards/akiraconsole` board definition.
list(APPEND BOARD_ROOT ${CMAKE_CURRENT_LIST_DIR})
list(APPEND DTS_ROOT   ${CMAKE_CURRENT_LIST_DIR})

# AkiraOS is fetched by west into the `akiraos/` sub-directory.
# MODULE_EXT_ROOT is set on the command-line by the CI workflow and build.sh
# so WAMR and other out-of-tree modules are found correctly.
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(akiraconsole_firmware VERSION 1.4.9)

# AkiraOS application sources are provided by the platform repo.
# CMAKE_CURRENT_SOURCE_DIR must point at the AkiraOS directory before include()
# so that all target_sources() relative paths inside AkiraOS CMakeLists.txt
# resolve to /workspace/akiraos/src/... instead of the firmware source dir.
set(CMAKE_CURRENT_SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/../akiraos")
include(${CMAKE_CURRENT_LIST_DIR}/../akiraos/CMakeLists.txt OPTIONAL)
