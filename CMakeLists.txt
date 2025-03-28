cmake_minimum_required(VERSION 3.0)
project(maverik)

option(ENABLE_XR "Enable XR" OFF)

include_directories(include)

file(GLOB_RECURSE SOURCES_COMMON "src/common/*.cpp")

message("ENABLE_XR: ${ENABLE_XR}")

if(ENABLE_XR)
    set(CMAKE_CXX_STANDARD 20)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
    set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)

    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -u ANativeActivity_onCreate")

    file(GLOB_RECURSE SOURCES_XR "src/xr/*.cpp")

    add_library(maverik SHARED ${SOURCES_COMMON} ${SOURCES_XR})

else()
    set(CMAKE_CXX_STANDARD 23)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)

    file(GLOB_RECURSE SOURCES_VK "src/vk/*.cpp")

    add_executable(maverik ${SOURCES_COMMON} ${SOURCES_VK})
endif()

