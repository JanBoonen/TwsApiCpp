cmake_minimum_required(VERSION 3.1.0)

#----------------------------
# CMAKE DEBUGING
#----------------------------

# Display a variable.
macro(display var)
  message(STATUS "${var}: " ${${var}})
endmacro()

#----------------------------

set(API_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Api)
set(IB_POSIX_CLIENT_SRC ${CMAKE_CURRENT_SOURCE_DIR}/../source/PosixClient)
set(IB_POSIX_CLIENT_INCLUDE_DIR ${IB_POSIX_CLIENT_SRC}/Shared)
set(IB_POSIX_CLIENT_SOURCE_DIR ${IB_POSIX_CLIENT_SRC}/src)

file(GLOB IB_POSIX_CLIENT_SOURCES ${IB_POSIX_CLIENT_SOURCE_DIR}/*)
file(GLOB IB_POSIX_CLIENT_INCLUDES ${IB_POSIX_CLIENT_INCLUDE_DIR}/*.h)
file(GLOB API_INCLUDES ${API_INCLUDE_DIR}/*.h)

add_subdirectory(Api)
add_subdirectory(Test)