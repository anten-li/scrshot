# CMakeList.txt : CMake project for scrshot, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

project ("scrshot")

STRING(COMPARE EQUAL ${CMAKE_BUILD_TYPE} "Release" isRelease)
if(isRelease)
	set(CMAKE_CXX_FLAGS_RELEASE "/O1 /DNDEBUG")
	SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /NODEFAULTLIB /ENTRY:main /MANIFEST:NO")
endif()

add_subdirectory("src")