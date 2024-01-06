cmake_minimum_required(VERSION 3.0.0)
project(examples VERSION 0.1.0)

include(CTest)
enable_testing()

add_executable(udp-test udp-test.c)
target_link_libraries(udp-test onvifdisco)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
