# Policies <= CMP0128 default to NEW
# Maximum version allowed for the minimum version is 3.22 because of Ubuntu 22.04 LTS so
# currently can't use the target_sources(FILE_SET) 😔
# Minimum version must always correspond to the version that is in the latest Ubuntu LTS
# 3.20 - because is used NEWLINE_STYLE in the file(GENERATE) command
# 3.22 - because is used CMP0127: cmake_dependent_option() supports full Condition Syntax
cmake_minimum_required(VERSION 3.22...3.30 FATAL_ERROR)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
# Set the AUTOMOC property explicitly only when needed (eg. unit tests need AUTOMOC)
set(CMAKE_AUTOMOC OFF)

# Initialize variables
# ---
# This is the crucial part, you need to set the TinyMainDir CMake variable.
# It links against the TinyORM build tree.

set(Tom_ns tom)
set(Tom_target tom)

# Path to the PARENT TinyORM directory❗
file(REAL_PATH "../../../TinyORM" TinyMainDir)

set(TinyOrmSourceDir "${TinyMainDir}/TinyORM")
set(TinyOrmBuildDir "${TinyMainDir}/TinyORM-builds-cmake/build-TinyORM-Desktop_Qt_6_7_2_MSVC2022_64bit-Debug")
#set(TinyOrmBuildDir "${TinyMainDir}/TinyORM-builds-cmake/build-debug")

# TinyORM CMake modules (needed to set the executable version and RC file on Windows)
list(APPEND CMAKE_MODULE_PATH "${TinyOrmSourceDir}/cmake/CommonModules")

# Build tree
list(APPEND CMAKE_PREFIX_PATH "${TinyOrmBuildDir}")

# Initialize Project Version
# ---

include(TinyHelpers)
tiny_read_version(TINY_VERSION
    TINY_VERSION_MAJOR TINY_VERSION_MINOR TINY_VERSION_PATCH TINY_VERSION_TWEAK
    VERSION_HEADER "${TinyOrmSourceDir}/tom/include/tom/version.hpp"
    PREFIX TINYTOM
    HEADER_FOR "${Tom_ns}"
)

# Basic project
# ---

project(${Tom_ns}
    DESCRIPTION "Tom console application for TinyORM C++ library"
    HOMEPAGE_URL "https://www.tinyorm.org"
    LANGUAGES CXX
    VERSION ${TINY_VERSION}
)

# Tom command-line application
# ---

add_executable(${Tom_target}
    main.cpp
)
add_executable(${Tom_ns}::${Tom_target} ALIAS ${Tom_target})

# Tom command-line application specific configuration
# ---

set_target_properties(${Tom_target}
    PROPERTIES
        C_VISIBILITY_PRESET "hidden"
        CXX_VISIBILITY_PRESET "hidden"
        VISIBILITY_INLINES_HIDDEN YES
        VERSION ${PROJECT_VERSION}
)

target_include_directories(${Tom_target} PRIVATE
    "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/database>"
)

# Tom command-line application defines
# ---

target_compile_definitions(${Tom_target} PRIVATE PROJECT_TOM)

# Windows resource and manifest files
# ---

# Find icons, tom/version.hpp, and Windows manifest file for MinGW
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    tiny_set_rc_flags("-I \"${TinyOrmSourceDir}/tom/resources\"")
endif()

include(TinyResourceAndManifest)
tiny_resource_and_manifest(${Tom_target}
    OUTPUT_DIR "${TINY_BUILD_GENDIR}/tmp/"
    RESOURCES_DIR "${TinyOrmSourceDir}/tom/resources"
)

# Resolve and link dependencies
# ---

find_package(QT NAMES Qt6 COMPONENTS Core REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core REQUIRED)
find_package(TinyOrm 0.38.1 CONFIG REQUIRED)

# Unconditional dependencies
target_link_libraries(${Tom_target}
    PRIVATE
        # Never use versionless Qt targets
        Qt${QT_VERSION_MAJOR}::Core
        TinyOrm::TinyOrm
)
