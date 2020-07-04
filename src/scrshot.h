#pragma once

#define UNICODE

#include <Windows.h>
//#include <gdiplus.h>

enum class progMenu {
	main,
	SaveOption
};

enum class SaveOption {
	file,
	dropbox
};

template <class T>
class SmartHandle {
protected:
	T SHandle;
public:
	SmartHandle(T Handle);
	operator bool();
	operator T();
};

class SmartHDC : public SmartHandle<HDC> {
public:
	SmartHDC(HDC Handle);
	~SmartHDC();
};

class Smartbuffer : public SmartHandle<LPVOID> {
public:
	Smartbuffer(int size);
	~Smartbuffer();
};

void stdWrite(LPWCH str);
WCHAR ReadMenu(LPVOID buff);
void ClearConsole();