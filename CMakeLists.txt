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
# MUST use add_subdirectory() — not include() — so that CMake sets
# CMAKE_CURRENT_SOURCE_DIR to the akiraos dir when processing its
# CMakeLists.txt.  target_sources() resolves relative paths from the
# internal CMAKE_CURRENT_SOURCE_DIR; include() does NOT update it.
if(EXISTS "${CMAKE_CURRENT_LIST_DIR}/../akiraos/CMakeLists.txt")
  add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/../akiraos"
                   "${CMAKE_CURRENT_BINARY_DIR}/akiraos")
endif()

# ── WAMR include-path fix ───────────────────────────────────────────────────
# WAMR's runtime_lib.cmake registers its headers via directory-scoped
# include_directories().  When AkiraOS is pulled in via add_subdirectory()
# those directory-scoped includes are confined to the akiraos/ CMake scope
# and do NOT propagate to the parent (firmware) scope where 'app' target
# sources are actually compiled.  The result is fatal missing-header errors
# for platform_api_vmcore.h, bh_platform.h, and friends.
# Fix: explicitly add every WAMR include dir that runtime_lib.cmake would
# have registered globally in a standalone AkiraOS build.
if(CONFIG_AKIRA_WASM_RUNTIME)
  set(_wamr "${CMAKE_CURRENT_LIST_DIR}/../akiraos/modules/wasm-micro-runtime")
  target_include_directories(app PRIVATE
    ${_wamr}/core/shared/platform/include   # platform_api_vmcore.h, platform_common.h
    ${_wamr}/core/shared/platform/zephyr    # platform_internal.h (included by platform_common.h)
    ${_wamr}/core/shared/mem-alloc          # mem_alloc.h
    ${_wamr}/core/shared/utils              # bh_common.h, bh_log.h
    ${_wamr}/core/shared/utils/uncommon     # bh_assert.h variants
  )
endif()

# ── AkiraConsole product-specific sources ──────────────────────────────────
# These files live in this repo and can be freely modified without touching
# AkiraOS.  Add new console-specific features here.
target_sources(app PRIVATE
  src/console_board.c
)

target_include_directories(app PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/src
)
