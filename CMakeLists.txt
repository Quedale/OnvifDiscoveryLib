cmake_minimum_required(VERSION 3.0.0)
project(OnvifDiscoveryLib VERSION 0.1.0)

set(OUTPUT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/release)
set(LIBRARY_OUTPUT_PATH    ${OUTPUT_DIR})
set(EXECUTABLE_OUTPUT_PATH ${OUTPUT_DIR})

if (NOT DEFINED GSOAP_SRC_DIR)
  set(GSOAP_SRC_DIR  ${CMAKE_SOURCE_DIR}/subprojects/gsoap-2.8)
endif()

set(ENV{PKG_CONFIG_PATH} "$ENV{PKG_CONFIG_PATH}:${CMAKE_CURRENT_SOURCE_DIR}/subprojects/CUtils/build/dist/lib/pkgconfig")
find_package(PkgConfig)
pkg_check_modules(CUTILS REQUIRED cutils)
include_directories(${CUTILS_INCLUDE_DIRS})

find_package(Threads REQUIRED)

include_directories(${GSOAP_SRC_DIR}/gsoap/ 
                    ${GSOAP_SRC_DIR}/gsoap/import 
                    ${GSOAP_SRC_DIR}/gsoap/custom 
                    ${GSOAP_SRC_DIR}/gsoap/plugin 
                    src/generated
                    src)

if(NOT EXISTS ${CMAKE_SOURCE_DIR}/src/generated)
file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/src/generated)
endif()

aux_source_directory(src src_dir)
aux_source_directory(src/generated generated_dir)

#########################
#
# onvif ibrary
#
#########################
add_library(onvifdisco STATIC  ${src_dir}
                          ${GSOAP_SRC_DIR}/gsoap/dom.c 
                          ${GSOAP_SRC_DIR}/gsoap/stdsoap2.c
                          ${GSOAP_SRC_DIR}/gsoap/plugin/logging.c
                          ${GSOAP_SRC_DIR}/gsoap/plugin/wsaapi.c 
                          ${GSOAP_SRC_DIR}/gsoap/plugin/wsddapi.c
                          ${generated_dir})

target_link_libraries(onvifdisco PRIVATE ${CUTILS_LDFLAGS})
target_compile_options(onvifdisco PRIVATE "SHELL: -c -DWITH_DOM -DSOAP_H_FILE=discosoapH.h")
if (MSVC)
  target_compile_options(onvifdisco PRIVATE "SHELL: -wd4244 -wd4267")
else()
  target_compile_options(onvifdisco PRIVATE "SHELL: -Wformat-truncation=0 -fPIC")
endif()
if(NOT WIN32)
  target_compile_options(onvifdisco PRIVATE "SHELL: -fvisibility=hidden")
endif()

target_compile_options(onvifdisco PRIVATE "SHELL: -c -Wall -DBUILD_SHARD")

add_subdirectory(example)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
set(CMAKE_INSTALL_BINDIR ${CMAKE_INSTALL_PREFIX}/bin)
set(CMAKE_INSTALL_LIBDIR ${CMAKE_INSTALL_PREFIX}/lib)
set(CMAKE_INSTALL_INCLUDEDIR ${CMAKE_INSTALL_PREFIX}/include)

include(CPack)

#Defining headers to export
set(headers ${PROJECT_SOURCE_DIR}/src/discoverer.h 
		        ${PROJECT_SOURCE_DIR}/src/onvif_discovery.h 
		        ${PROJECT_SOURCE_DIR}/src/probmatch.h
            ${PROJECT_SOURCE_DIR}/src/discovered_obj.h)

#Setting pkgconfig file file variables
set(prefix ${CMAKE_INSTALL_PREFIX})
set(exec_prefix ${CMAKE_INSTALL_BINDIR})
set(libdir ${CMAKE_INSTALL_LIBDIR})
set(includedir ${CMAKE_INSTALL_INCLUDEDIR})

if(NOT WIN32)
  #Create pkgconfig file
  configure_file(onvifdisco.pc.in onvifdisco.pc @ONLY) 
  install(FILES ${PROJECT_BINARY_DIR}/onvifdisco.pc DESTINATION "${CMAKE_INSTALL_LIBDIR}/pkgconfig")

endif()

install(TARGETS onvifdisco EXPORT onvifdisco DESTINATION "${CMAKE_INSTALL_LIBDIR}")
# install(TARGETS onvifsoapstc EXPORT onvifsoap DESTINATION "${CMAKE_INSTALL_LIBDIR}")
install(FILES ${headers} DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")