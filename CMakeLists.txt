cmake_minimum_required(VERSION 3.5)
project(teplomerAPI CXX)

include(CheckIncludeFileCXX)

check_include_file_cxx(any HAS_ANY)
check_include_file_cxx(string_view HAS_STRING_VIEW)
check_include_file_cxx(coroutine HAS_COROUTINE)
if (NOT "${CMAKE_CXX_STANDARD}" STREQUAL "")
    # Do nothing
elseif (HAS_ANY AND HAS_STRING_VIEW AND HAS_COROUTINE)
    set(CMAKE_CXX_STANDARD 20)
elseif (HAS_ANY AND HAS_STRING_VIEW)
    set(CMAKE_CXX_STANDARD 17)
else ()
    set(CMAKE_CXX_STANDARD 14)
endif ()

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(${PROJECT_NAME} main.cc)

# ##############################################################################
# If you include the drogon source code locally in your project, use this method
# to add drogon 
# add_subdirectory(drogon) 
# target_link_libraries(${PROJECT_NAME} PRIVATE drogon)
#
# and comment out the following lines
find_package(Drogon CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE Drogon::Drogon)

# ##############################################################################

if (CMAKE_CXX_STANDARD LESS 17)
    message(FATAL_ERROR "c++17 or higher is required")
elseif (CMAKE_CXX_STANDARD LESS 20)
    message(STATUS "use c++17")
else ()
    message(STATUS "use c++20")
endif()

if(DEFINED ENV{DB_HOST})
    set(DB_HOST "$ENV{DB_HOST}")
else()
    message(FATAL_ERROR "DB_HOST není definována v prostředí")
endif()

if(DEFINED ENV{DB_NAME})
    set(DB_NAME "$ENV{DB_NAME}")
else()
    message(FATAL_ERROR "DB_NAME není definována v prostředí")
endif()

if(DEFINED ENV{DB_USER})
    set(DB_USER "$ENV{DB_USER}")
else()
    message(FATAL_ERROR "DB_USER není definována v prostředí")
endif()

if(DEFINED ENV{DB_PASSWD})
    set(DB_PASSWD "$ENV{DB_PASSWD}")
else()
    message(FATAL_ERROR "DB_PASSWD není definována v prostředí")
endif()

configure_file(${CMAKE_SOURCE_DIR}/config_template.json
               ${CMAKE_BINARY_DIR}/config.json)

aux_source_directory(controllers CTL_SRC)
aux_source_directory(controllers/callbacks CALLBACKS_SRC)
aux_source_directory(filters FILTER_SRC)
aux_source_directory(plugins PLUGIN_SRC)
aux_source_directory(models MODEL_SRC)
aux_source_directory(models/request MODEL_REQUEST_SRC)
aux_source_directory(models/list MODEL_LIST_SRC)
aux_source_directory(models/model_utils MODEL_UTILS_SRC)
aux_source_directory(plugins/facades FACADES_SRC)

drogon_create_views(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/views
                    ${CMAKE_CURRENT_BINARY_DIR})
# use the following line to create views with namespaces.
# drogon_create_views(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/views
#                     ${CMAKE_CURRENT_BINARY_DIR} TRUE)
# use the following line to create views with namespace CHANGE_ME prefixed
# and path namespaces.
# drogon_create_views(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/views
#                     ${CMAKE_CURRENT_BINARY_DIR} TRUE CHANGE_ME)

target_include_directories(${PROJECT_NAME}
                           PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
                                   ${CMAKE_CURRENT_SOURCE_DIR}/models)
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
               ${CALLBACKS_SRC})
# ##############################################################################
# uncomment the following line for dynamically loading views 
# set_property(TARGET ${PROJECT_NAME} PROPERTY ENABLE_EXPORTS ON)

# ##############################################################################

add_subdirectory(test)
