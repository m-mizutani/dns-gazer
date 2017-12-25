CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(dns-gazer)


IF(NOT CMAKE_BUILD_TYPE)
    message(STATUS "No build type selected, default to Release")
    SET(CMAKE_BUILD_TYPE "Release")
ENDIF()

IF(POLICY CMP0042)
    cmake_policy(SET CMP0042 NEW)
ENDIF()
IF(POLICY CMP0015)
    cmake_policy(SET CMP0015 NEW)
ENDIF()


SET(FLUENT_INSTALL OFF)
SET(CMAKE_CXX_FLAGS_RELEASE "-Wall -O3 -std=c++0x")
SET(CMAKE_CXX_FLAGS_DEBUG   "-Wall -O0 -std=c++0x -g")

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)


INCLUDE_DIRECTORIES(${INC_DIR} ./src)
INCLUDE_DIRECTORIES(external/packetmachine/src/)
INCLUDE_DIRECTORIES(external/libfluent/src/)
ADD_SUBDIRECTORY(external/packetmachine)
ADD_SUBDIRECTORY(external/libfluent)

LINK_DIRECTORIES(${LIB_DIR})
IF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    INCLUDE_DIRECTORIES(/usr/local/include)
    LINK_DIRECTORIES(/usr/local/lib)
ENDIF()

# Build library

FILE(GLOB BASESRCS
	"src/gazer.cc"
	"src/dns.cc"	"src/dns.hpp"	
)

# Application (CLI) code
ADD_EXECUTABLE(dns-gazer ${BASESRCS})
TARGET_LINK_LIBRARIES(dns-gazer pm-static fluent-static)

INSTALL(TARGETS dns-gazer RUNTIME DESTINATION bin)
