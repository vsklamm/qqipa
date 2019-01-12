cmake_minimum_required(VERSION 3.10)

project(qqipa)

include_directories(${PROJECT_SOURCE_DIR})

# set (TEST_DIR ${PROJECT_SOURCE_DIR}/test)
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
set (CMAKE_CXX_STANDARD 17)
set (CMAKE_AUTOMOC ON)
set (CMAKE_AUTOUIC ON)

add_subdirectory(abseil)
include_directories("${CMAKE_SOURCE_DIR}/abseil")

find_package(Qt5Widgets CONFIG REQUIRED)
find_package(Qt5Concurrent CONFIG REQUIRED)

# add_executable(index_test
#    ${TEST_DIR}/gtest/gtest.h
#    ${TEST_DIR}/gtest/gtest_main.cc
#    ${TEST_DIR}/gtest/gtest-all.cc
#    ${TEST_DIR}/trigram_test.cpp
#    ${INCLUDE_DIR}/trigramindex.h
#     trigramindex.cpp
#    )

add_library(qqipa_core STATIC
	trigramcontainer.h trigramcontainer.cpp
	magic_number.h magic_number.cpp
        indexedfile.h indexedfile.cpp
	directorywrapper.h directorywrapper.cpp
        patternsearcher.h patternsearcher.cpp
	controller.h controller.cpp)

add_library(widgets STATIC
        textviewer.h textviewer.cpp	
	filestableview.h filestableview.cpp)

add_library(main_gui STATIC
        mainwindow.h mainwindow.cpp mainwindow.ui)

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -O3")

add_executable(qqipa main.cpp)

target_link_libraries(widgets Qt5::Widgets)
target_link_libraries(qqipa_core widgets absl::flat_hash_set Qt5::Concurrent pthread)
target_link_libraries(main_gui qqipa_core)

target_link_libraries(qqipa main_gui)


