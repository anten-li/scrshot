# CMakeList.txt : CMake project for scrshot, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

project ("scrshot")

STRING(COMPARE EQUAL ${CMAKE_BUILD_TYPE} "Release" isRelease)

if(isRelease)
	# GS выключаем security check
	# GL Генерация кода на этапе линковки
	# Gy неоходимо для удаления неиспользуемых функций
	# Zc:wchar_t wchar_t как встроенный тип

	#/Zi /Fd\"Release\\vc142.pdb\""
	set(CMAKE_CXX_FLAGS_RELEASE "/permissive- /GS- /GL /analyze- /W0 /Gy /Zc:wchar_t /Zc:threadSafeInit- /Ox /DNDEBUG /FA")
	SET(CMAKE_CXX_FLAGS "")

	# /PDB:\"D:\\AnTeN\\prog\\VS\\scrshot\\out\\build\\x86-Release\\src\\scrshot.pdb\
	SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /MANIFEST:NO /LTCG /NODEFAULTLIB /ENTRY:main /OPT:REF")
endif()

#/nologo /TP /DWIN32 /D_WINDOWS /W3 /GR /EHsc /Zc:threadSafeInit- /O2 /DNDEBUG /GS- /FA /showIncludes /Fosrc\CMakeFiles\scrshot.dir\scrshot.cpp.obj /Fdsrc\CMakeFiles\scrshot.dir\ /FS -c 
#/nologo /TP /W3 /Zc:threadSafeInit- /Os /DNDEBUG /GS- /FA /showIncludes /Fosrc\CMakeFiles\scrshot.dir\scrshot.cpp.obj /Fdsrc\CMakeFiles\scrshot.dir\ /FS -c

add_subdirectory("src")