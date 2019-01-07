cmake_minimum_required(VERSION 3.8)

project("qqipa")

include_directories(${PROJECT_SOURCE_DIR})

set (TEST_DIR ${PROJECT_SOURCE_DIR}/test)
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
set (CMAKE_CXX_STANDARD 17)
set (CMAKE_AUTOMOC ON)
set (CMAKE_AUTOUIC ON)
set (CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt5Widgets)
find_package(Qt5Concurrent)

# add_executable(index_test
#    ${TEST_DIR}/gtest/gtest.h
#    ${TEST_DIR}/gtest/gtest_main.cc
#    ${TEST_DIR}/gtest/gtest-all.cc
#    ${TEST_DIR}/trigram_test.cpp
#    ${INCLUDE_DIR}/trigramindex.h
#    ${SOURCE_DIR}/trigramindex.cpp
#    )

add_executable(qqipa
    ${INCLUDE_DIR}/mainwindow.h
    ${INCLUDE_DIR}/filesstableview.h
    ${INCLUDE_DIR}/indexedfile.h
    ${INCLUDE_DIR}/magic_number.h
    ${INCLUDE_DIR}/patternsearcher.h
    ${INCLUDE_DIR}/searcherutil.h
    ${INCLUDE_DIR}/textviewer.h
    ${INCLUDE_DIR}/trigramcontainer.h

    ${SOURCE_DIR}/mainwindow.cpp
    ${SOURCE_DIR}/filesstableview.cpp
    ${SOURCE_DIR}/indexedfile.cpp
    ${SOURCE_DIR}/magic_number.cpp
    ${SOURCE_DIR}/patternsearcher.cpp
    ${SOURCE_DIR}/searcherutil.cpp
    ${SOURCE_DIR}/textviewer.cpp     
    ${SOURCE_DIR}/trigramcontainer.cpp
    ${SOURCE_DIR}/mainwindow.ui    
    )


set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -O3")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++17")

if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_GLIBCXX_DEBUG -fsanitize=undefined")
endif()

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Weverything -Wno-c++98-compat -std=c++17 -fsanitize=address")
endif()


target_link_libraries(index_test Qt5::Widgets  -lpthread stdc++fs)
target_link_libraries(librarian Qt5::Widgets Qt5::Concurrent -lpthread)
