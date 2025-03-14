cmake_minimum_required(VERSION 3.31.5)
project(${MOD_NAME} VERSION ${MOD_VERSION})
message("CMake build type: ${CMAKE_BUILD_TYPE}")

# handle RPATH (but it looks like this won't work on Windows host
set(CMAKE_BUILD_RPATH_USE_ORIGIN TRUE)

# handle language flags
set(C_FLAGS "-Werror=format -fdata-sections -ffunction-sections")
#set(CXX_FLAGS "-fno-exceptions -fno-rtti")

# handle linker flags
# android bionic linker does not support RPATH
set(LINKER_FLAGS "-ffixed-x18 -Wl,--hash-style=both -Wl,-rpath,$ORIGIN")

# handle build type flags
if (NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(C_FLAGS "${C_FLAGS} -O2 -fvisibility=hidden -fvisibility-inlines-hidden")
    set(LINKER_FLAGS "${LINKER_FLAGS} -Wl,-exclude-libs,ALL -Wl,--gc-sections -Wl,--strip-all")
else ()
    set(C_FLAGS "${C_FLAGS} -O0")
endif ()

# set cmake flags
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${C_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${C_FLAGS}")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} ${LINKER_FLAGS}")
set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} ${LINKER_FLAGS}")

# compiler flag interface library
add_library(compiler_flags INTERFACE)
target_compile_features(compiler_flags INTERFACE cxx_std_23)

add_library(${CMAKE_PROJECT_NAME} SHARED
        main.cpp
        hookMain.cpp
        il2cppHook.cpp
        URH.cpp
        utils.cpp
)

#add_custom_command(
#        OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/UnityResolve.pcm
#        COMMAND ${CMAKE_CXX_COMPILER} "-std=c++23 ${CMAKE_CXX_FLAGS} -fmodule-header=user -xc++-header ${CMAKE_CURRENT_SOURCE_DIR}/includes/UnityResolve.hpp -o ${CMAKE_CURRENT_SOURCE_DIR}/includes/UnityResolve.pcm"
#        DEPENDS includes/UnityResolve.hpp
#        COMMENT "Compiling header unit UnityResolve.hpp"
#)
#set_source_files_properties(unityResolve.cpp PROPERTIES COMPILE_OPTIONS "-fmodule-file=${CMAKE_SOURCE_DIR}/includes/UnityResolve.pcm")
#target_sources(${CMAKE_PROJECT_NAME}
#        PUBLIC
#        FILE_SET CXX_MODULES FILES
#        hook.cpp
#        il2cppHook.cpp
#)

target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
        ${CMAKE_SOURCE_DIR}/includes
        ${CMAKE_SOURCE_DIR}/includes/UnityResolve.hpp)

target_compile_definitions(${CMAKE_PROJECT_NAME} PUBLIC __ANDROID__)

find_package(xdl REQUIRED CONFIG)
find_package(shadowhook REQUIRED CONFIG)

target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE
        compiler_flags
        log
        xdl::xdl
        shadowhook::shadowhook)
