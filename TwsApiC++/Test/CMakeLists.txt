project(TWSAPICPP_TEST VERSION 1.0.0)

set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Src)

add_definitions(-D_REENTRANT -D_DEBUG)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

include_directories(${API_INCLUDE_DIR} ${IB_POSIX_CLIENT_INCLUDE_DIR})

add_executable(Test ${SOURCE_DIR}/Test.cpp)
target_link_libraries(Test tws_cpp_api Threads::Threads)
target_compile_options(Test PRIVATE -Wall)

add_executable(History ${SOURCE_DIR}/History.cpp)
target_link_libraries(History tws_cpp_api Threads::Threads)
target_compile_options(History PRIVATE -Wall)

add_executable(Bars ${SOURCE_DIR}/Bars.cpp)
target_link_libraries(Bars tws_cpp_api Threads::Threads)
target_compile_options(Bars PRIVATE -Wall)

add_executable(Clients ${SOURCE_DIR}/Clients.cpp)
target_link_libraries(Clients tws_cpp_api Threads::Threads)
target_compile_options(Clients PRIVATE -Wall)