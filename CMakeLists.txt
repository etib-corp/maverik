cmake_minimum_required(VERSION 3.0)
project(maverik)

option(ENABLE_XR "Enable XR" OFF)

include_directories(include)
include(FetchContent)

file(GLOB_RECURSE SOURCES_COMMON "src/common/*.cpp" "src/Version.cpp")

FetchContent_Declare(glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG 1.0.1
    GIT_SHALLOW TRUE
    GIT_PROGRESS TRUE
)
FetchContent_MakeAvailable(glm)

if(APPLE)
    include_directories(/opt/homebrew/include)
    link_directories(/opt/homebrew/lib)
endif()

if(ENABLE_XR)
    option(LIBRARY_TYPE "Build as a shared library" ON)

    set(CMAKE_CXX_STANDARD 20)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
    set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)

    FetchContent_Declare(OpenXR-SDK
        GIT_REPOSITORY https://github.com/KhronosGroup/OpenXR-SDK.git
        GIT_TAG release-1.0.33 #must match the meta quest loader OpenXR version
        #https://developer.oculus.com/downloads/package/oculus-openxr-mobile-sdk#current-openxr-version
        GIT_SHALLOW TRUE
        GIT_PROGRESS TRUE
    )
    FetchContent_MakeAvailable(OpenXR-SDK)

    file(GLOB_RECURSE SOURCES_XR "src/xr/*.cpp")

    if(LIBRARY_TYPE)
        add_library(maverik SHARED ${SOURCES_COMMON} ${SOURCES_XR})
    else()
        add_library(maverik STATIC ${SOURCES_COMMON} ${SOURCES_XR})
    endif()

    target_link_libraries(maverik PRIVATE openxr_loader vulkan)
else()
    set(CMAKE_CXX_STANDARD 23)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)

    FetchContent_Declare(glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG 3.3.8
        GIT_SHALLOW TRUE
        GIT_PROGRESS TRUE
    )
    FetchContent_MakeAvailable(glfw)

    include_directories("include" "include/vk")

    file(GLOB_RECURSE SOURCES_VK "src/vk/*.cpp")

    add_library(maverik ${SOURCES_COMMON} ${SOURCES_VK})

    target_link_libraries(maverik PRIVATE glfw)
endif()

target_include_directories(maverik PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

