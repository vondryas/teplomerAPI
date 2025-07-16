cmake_minimum_required(VERSION 3.10)

# vcpkg toolchain integration
if(DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    if(not defined CMAKE_TOOLCHAIN_FILE)
        set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
            CACHE STRING "Vcpkg toolchain file")
    endif()
endif()

project(teplomerAPI LANGUAGES CXX)

# Platform detection
if(WIN32)
    message(STATUS "Building on Windows")
elseif(UNIX AND NOT APPLE)
    message(STATUS "Building on Linux/Unix")
elseif(APPLE)
    message(STATUS "Building on macOS")
else()
    message(STATUS "Building on unknown platform")
endif()
set(CMAKE_CXX_STANDARD 20)  # Nebo 23, pokud používáte C++23



# C++ standard detection with fallback
include(CheckIncludeFileCXX)

check_include_file_cxx(any HAS_ANY)
check_include_file_cxx(string_view HAS_STRING_VIEW)
check_include_file_cxx(coroutine HAS_COROUTINE)
unset(CMAKE_REQUIRED_FLAGS)

if(NOT CMAKE_CXX_STANDARD)
    if(HAS_ANY AND HAS_STRING_VIEW AND HAS_COROUTINE)
        set(CMAKE_CXX_STANDARD 20)
    elseif(HAS_ANY AND HAS_STRING_VIEW)
        set(CMAKE_CXX_STANDARD 17)
    else()
        set(CMAKE_CXX_STANDARD 14)
    endif()
endif()

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Verify C++ standard
if(CMAKE_CXX_STANDARD LESS 17)
    message(FATAL_ERROR "C++20 or higher is required")
elseif(CMAKE_CXX_STANDARD LESS 20)
    message(FATAL_ERROR "Using C++20 or higher is required")
else()
    message(STATUS "Using C++20")
endif()

# Executable setup
add_executable(${PROJECT_NAME} main.cc)

# Drogon configuration - supports both local and installed versions
option(USE_LOCAL_DROGON "Use local Drogon subdirectory" OFF)
if(USE_LOCAL_DROGON)
    add_subdirectory(drogon) 
    target_link_libraries(${PROJECT_NAME} PRIVATE drogon)
    message(STATUS "Using local Drogon build")
else()
    find_package(Drogon CONFIG REQUIRED)
    target_link_libraries(${PROJECT_NAME} PRIVATE Drogon::Drogon)
    message(STATUS "Using system-installed Drogon")
endif()

# fmt
find_package(fmt CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE fmt::fmt)

find_package(stduuid CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE stduuid)

# Database configuration with cross-platform environment variable handling
function(check_required_env_var var_name)
    if(DEFINED ENV{${var_name}})
        set(${var_name} "$ENV{${var_name}}" PARENT_SCOPE)
    else()
        message(FATAL_ERROR "Environment variable ${var_name} is not defined")
    endif()
endfunction()

check_required_env_var(DB_HOST)
check_required_env_var(DB_NAME)
check_required_env_var(DB_USER)
check_required_env_var(DB_PASSWD)

# Make configuration file with database connection settings
configure_file(${CMAKE_SOURCE_DIR}/config_template.json
               ${CMAKE_BINARY_DIR}/config.json)

# Make model.json file with database connection settings
configure_file(${CMAKE_SOURCE_DIR}/models/orm_model/model_template.json
               ${CMAKE_SOURCE_DIR}/models/orm_model/model.json)

# Source file collection with platform-specific considerations
set(SOURCE_DIRS
    controllers
    controllers/exceptionWrapper
    controllers/responses
    filters
    plugins
    models/orm_model
    models/request_model
    models/list_model
    models/model_utils
    models/model_mapper
    models/model_interface
    facades
)

# Include directories (include without specifying subdirectories))
target_include_directories(${PROJECT_NAME}
    PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/models
)

set(ALL_SOURCES)

# Collect sources from source dirs and add them to all sources
foreach(dir ${SOURCE_DIRS})
    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${dir})
        aux_source_directory(${dir} ${dir}_SRC)
        list(APPEND ALL_SOURCES ${${dir}_SRC})
    else()
        message(WARNING "Directory ${dir} not found - skipping")
    endif()
endforeach()

# View generation
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/views)
    drogon_create_views(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/views
                      ${CMAKE_CURRENT_BINARY_DIR})
else()
    message(STATUS "Views directory not found - skipping view generation")
endif()


# including source files to target
target_sources(${PROJECT_NAME} PRIVATE ${SRC_DIR} ${ALL_SOURCES})

# Optional exports for dynamic loading
option(ENABLE_DYNAMIC_VIEWS "Enable dynamic loading of views" OFF)
if(ENABLE_DYNAMIC_VIEWS)
    set_property(TARGET ${PROJECT_NAME} PROPERTY ENABLE_EXPORTS ON)
    message(STATUS "Enabled dynamic view loading")
endif()

# Testing
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/test AND IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/test)
    add_subdirectory(test)
else()
    message(STATUS "Test directory not found - skipping tests")
endif()

# Platform-specific configurations
if(WIN32)
    # Windows-specific settings
    target_compile_definitions(${PROJECT_NAME} PRIVATE _WIN32_WINNT=0x0A00)
elseif(APPLE)
    # macOS-specific settings
    find_library(COREFOUNDATION CoreFoundation)
    target_link_libraries(${PROJECT_NAME} PRIVATE ${COREFOUNDATION})
endif()
