cmake_minimum_required(VERSION 3.7.1)
project(as)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}")

find_package(Cplex)
include_directories(SYSTEM ${CPLEX_INCLUDE_DIRS})

find_package(Boost)
include_directories(SYSTEM ${Boost_INCLUDE_DIRS})

find_package(Exactcolors)
include_directories(SYSTEM ${EXACTCOLORS_INCLUDE_DIRS})

find_package(Concorde)
include_directories(SYSTEM ${CONCORDE_INCLUDE_DIRS})

find_package(Discorde)
include_directories(SYSTEM ${DISCORDE_INCLUDE_DIRS})

find_package(Cimg)
include_directories(SYSTEM ${CIMG_INCLUDE_DIRS})

find_package(X11)
include_directories(SYSTEM ${X11_INCLUDE_DIR})

find_package(Pmc)
include_directories(SYSTEM ${PMC_INCLUDE_DIRS})

enable_testing()

find_package(GTest REQUIRED)
include(GoogleTest)

add_executable(as_test test.cpp)
target_link_libraries(as_test GTest::GTest GTest::Main ${EXACTCOLORS_LIBRARIES} ${DISCORDE_LIBRARIES} ${CONCORDE_LIBRARIES} ${CPLEX_LIBRARIES} ${PMC_LIBRARIES})
gtest_add_tests(TARGET as_test test.cpp)

add_executable(as_test_plot test_plot.cpp)
target_link_libraries(as_test_plot GTest::GTest GTest::Main ${X11_LIBRARIES})
gtest_add_tests(TARGET as_test_plot test_plot.cpp)

add_executable(as_test_alns test_alns.cpp)
target_link_libraries(as_test_alns ${EXACTCOLORS_LIBRARIES} ${DISCORDE_LIBRARIES} ${CONCORDE_LIBRARIES} ${CPLEX_LIBRARIES} ${PMC_LIBRARIES})