#
# Copyright (C) 2018 CS - Systemes d'Information (CS-SI)
#
# This file is part of Sirius
#
#     https://github.com/CS-SI/SIRIUS
#
# Sirius is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# Sirius is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with Sirius.  If not, see <https://www.gnu.org/licenses/>.
#

cmake_minimum_required(VERSION 3.2)
project(Sirius CXX)

include(cmake/BuildType.cmake)

option(ENABLE_CACHE_OPTIMIZATION "Enable cache (FFTW plan, Filter FFT)" ON)
option(ENABLE_LOGS "Enable logs" ON)
option(ENABLE_GSL_CONTRACTS "Enable GSL contracts" OFF)
option(ENABLE_DOCUMENTATION "Enable documentation generation" OFF)
option(ENABLE_UNIT_TESTS "Enable unit test targets" OFF)

set(SIRIUS_VERSION "1.0.0")

if (CMAKE_BUILD_TYPE STREQUAL Debug)
    message(STATUS "Force GSL contracts on Debug build type")
    set(ENABLE_GSL_CONTRACTS "ON")
endif()

message(STATUS "Sirius ${SIRIUS_VERSION}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Install directory: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "Enable cache: ${ENABLE_CACHE_OPTIMIZATION}")
message(STATUS "Enable logs: ${ENABLE_LOGS}")
message(STATUS "Enable GSL contracts: ${ENABLE_GSL_CONTRACTS}")
message(STATUS "Enable documentation: ${ENABLE_DOCUMENTATION}")
message(STATUS "Enable unit tests: ${ENABLE_UNIT_TESTS}")

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# set CXX flags
set(SIRIUS_CXX_FLAGS "")
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" OR
      "${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    # Clang or GCC
    set(SIRIUS_CXX_FLAGS  "-Wall -Wextra")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    # MSVC compiler
endif()
set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${SIRIUS_CXX_FLAGS}")

# path to additional cmake modules
list(APPEND CMAKE_MODULE_PATH
     "${CMAKE_CURRENT_SOURCE_DIR}/cmake"
     "${CMAKE_CURRENT_SOURCE_DIR}/third_party/cmake-modules")

find_package(Threads)

add_subdirectory(third_party)
add_subdirectory(src)

if (${ENABLE_UNIT_TESTS})
    enable_testing()
    add_subdirectory(tests)
endif()

if (${ENABLE_DOCUMENTATION})
    find_package(Doxygen)
    find_package(LATEX)
    if(DOXYGEN_FOUND)
        set(DOXYFILE_CONFIG_IN ${CMAKE_CURRENT_SOURCE_DIR}/doc/doxygen/doxyfile.in)
        set(DOXYFILE_CONFIG_OUT ${CMAKE_CURRENT_BINARY_DIR}/doc/doxygen/doxyfile)

        configure_file(${DOXYFILE_CONFIG_IN} ${DOXYFILE_CONFIG_OUT} @ONLY)

        add_custom_target(doc ALL
            COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYFILE_CONFIG_OUT}
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            COMMENT "Generating API documentation with Doxygen"
            VERBATIM)
        #if (LATEX_FOUND)
        #    add_custom_command(TARGET doc
        #        POST_BUILD
        #        COMMAND "${CMAKE_MAKE_PROGRAM}"
        #        COMMENT	"Generating documentation pdf"
        #        WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/latex")
        #    # copy pdf manual into install directory
        #    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/latex/refman.pdf
        #            DESTINATION ${CMAKE_INSTALL_PREFIX}/doc/api-doc
        #            RENAME manual.pdf)
        #else()
        #    message(STATUS "Cannot build pdf manual (LaTeX not found)")
        #endif()
    # copy doxygen documentation into install directory
    install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/html
            DESTINATION ${CMAKE_INSTALL_PREFIX}/doc/api-doc)
    else()
        message(STATUS "Doxygen is needed to build the documentation.")
    endif()
endif()
