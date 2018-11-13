cmake_minimum_required(VERSION 3.6)

project(pjson)

  if (NOT WIN32)
    if (NOT CMAKE_BUILD_TYPE)
      set(CMAKE_BUILD_TYPE "Release" CACHE STRING
        "Choose the type of build, options are: Debug Release RelWithDebInfo MinSizeRel."
        FORCE)
    endif (NOT CMAKE_BUILD_TYPE)
  endif ()

set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 14)

if (MSVC)
  # /wd4127 disables check for "conditional expression is constant"
  # /wd4714 disables warning about not inlining forceinline functions
  set(COMMON_COMPILER_FLAGS "/W4 /EHsc /wd4127 /wd4714 /nologo")
  set(CMAKE_C_FLAGS "${COMMON_COMPILER_FLAGS}")
  set(CMAKE_CXX_FLAGS "${COMMON_COMPILER_FLAGS}")
  add_definitions(-D_CRT_SECURE_NO_WARNINGS)
  add_definitions(-DNOMINMAX)
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
endif()

add_executable(runtests pjson.h fpconv.c test/test_main.cpp test/test_json.cpp)
