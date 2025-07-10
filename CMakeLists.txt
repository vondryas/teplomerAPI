cmake_minimum_required(VERSION 3.5)
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

# C++ standard detection with fallback
include(CheckIncludeFileCXX)

check_include_file_cxx(any HAS_ANY)
check_include_file_cxx(string_view HAS_STRING_VIEW)
check_include_file_cxx(coroutine HAS_COROUTINE)

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
    message(FATAL_ERROR "C++17 or higher is required")
elseif(CMAKE_CXX_STANDARD LESS 20)
    message(STATUS "Using C++17")
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

# Source file collection with platform-specific considerations
set(SOURCE_DIRS
    controllers
    controllers/callbacks
    controllers/responses
    filters
    plugins
    models
    models/request
    models/list
    models/model_utils
    facades
)

foreach(dir ${SOURCE_DIRS})
    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${dir})
        aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/${dir} ${dir}_SRC)
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

# Include directories
target_include_directories(${PROJECT_NAME}
    PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/models
)

# Add all collected sources
target_sources(${PROJECT_NAME}
    PRIVATE
    ${SRC_DIR}
    ${CTL_SRC}
    ${FILTER_SRC}
    ${PLUGIN_SRC}
    ${MODEL_SRC}
    ${MODEL_REQUEST_SRC}
    ${MODEL_LIST_SRC}
    ${FACADES_SRC}
    ${MODEL_UTILS_SRC}
    ${CALLBACKS_SRC}
    ${RESPONSES_SRC}
)

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
