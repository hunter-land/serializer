cmake_minimum_required(VERSION 3.12)
project(serializer VERSION 0.0.1)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_CXX_FLAGS_RELEASE "-Os ${CMAKE_CXX_FLAGS_RELEASE}")

set(SOURCE_DIR  "${CMAKE_SOURCE_DIR}")
set(INCLUDE_DIR "${CMAKE_SOURCE_DIR}/include") #Local include directory for headers
include(GNUInstallDirs) #GNUInstallDirs (a default module) is required for this CMake

#Options
option(BUILD_SHARED_LIBS "Build using shared libraries" ON)
option(CMAKE_BUILD_TYPE "Build type" Release)

#Source files:
set(SOURCE_FILES "${SOURCE_DIR}/serialOut.cpp"  "${SOURCE_DIR}/serialIn.cpp"  "${SOURCE_DIR}/serializer.cpp"                                      "${SOURCE_DIR}/times.cpp"                                  "${SOURCE_DIR}/endian.cpp")
set(HEADER_FILES "${INCLUDE_DIR}/serialOut.hpp" "${INCLUDE_DIR}/serialIn.hpp" "${INCLUDE_DIR}/serializer.hpp" "${INCLUDE_DIR}/deserialLimits.hpp" "${INCLUDE_DIR}/times.hpp" "${INCLUDE_DIR}/containers.hpp" "${INCLUDE_DIR}/endian.hpp")

add_library(serializer ${SOURCE_FILES})
# Set target properties for version and the related (public) headers
set_target_properties(serializer PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(serializer PROPERTIES PUBLIC_HEADER "${HEADER_FILES}")
# Declare target as something to be installed with the install target
install(TARGETS serializer
		LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
		PUBLIC_HEADER DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/serializer")
