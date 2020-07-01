#include "scrshot.h"

const int buffSize = 256;

HANDLE hstdOUT;
HANDLE hstdIN;
HANDLE hHeap;

int main()
{
	if ((hstdOUT = GetStdHandle(STD_OUTPUT_HANDLE)) != INVALID_HANDLE_VALUE &&
		(hstdIN = GetStdHandle(STD_INPUT_HANDLE)) != INVALID_HANDLE_VALUE &&
		(hHeap = GetProcessHeap())) {

		HMODULE User32;
		FARPROC SetDPI;
		if ((User32 = GetModuleHandle(L"User32.dll")) &&
			(SetDPI = GetProcAddress(User32, "SetProcessDPIAware"))) {
			SetDPI();
		}

		//SmartHDC hDisp(GetDC(NULL));
		//if (hDisp) {

		Smartbuffer buff(buffSize);
		if (buff) {
			progMenu menu = progMenu::main;

			while (true)
			{
				WCHAR Readed;

				if (menu == progMenu::main) {
					stdWrite(L"введите число:\n\t1 - запуск\n\t2 - настройки\n\n");
					Readed = ReadMenu(buff);
					if (Readed == *L"1") {
						break;
					}
					else if (Readed == *L"2")
					{
						menu = progMenu::settings;
					}
				}
				else if (menu == progMenu::settings)
				{
					stdWrite(L"введите число:\n\t1 - главное меню\n\n");
					Readed = ReadMenu(buff);
					if (Readed == *L"1") {
						menu = progMenu::main;
					}
				}
				ClearConsole();
			}
		}
		//}
	}
	return 0;
}

void stdWrite(LPWCH str)
{
	DWORD Writen;
	WriteConsole(hstdOUT, str, lstrlen(str), &Writen, NULL);
}

WCHAR ReadMenu(LPVOID buff)
{
	DWORD Readed;
	ReadConsole(hstdIN, buff, buffSize / sizeof(TCHAR) - sizeof(TCHAR), &Readed, NULL);
	if (Readed == 1 + sizeof(TCHAR)) return *(LPWCH)buff;
	else return NULL;
}

void ClearConsole() {
	CONSOLE_SCREEN_BUFFER_INFO screen;
	DWORD Writen;
	const COORD home = { 0, 0 };

	if (GetConsoleScreenBufferInfo(hstdOUT, &screen)) {
		FillConsoleOutputCharacter(hstdOUT, *L" ", screen.dwSize.X * screen.dwSize.Y,
			home, &Writen);
		SetConsoleCursorPosition(hstdOUT, home);
	}
}

template<class T>
inline SmartHandle<T>::SmartHandle(T Handle) : SHandle(Handle) {}

template<class T>
inline SmartHandle<T>::operator bool() { return SHandle; }

template<class T>
inline SmartHandle<T>::operator T() { return SHandle; }

inline SmartHDC::SmartHDC(HDC Handle) : SmartHandle<HDC>(Handle) {}

inline SmartHDC::~SmartHDC() {
	if (SHandle) ReleaseDC(NULL, SHandle);
}

inline Smartbuffer::Smartbuffer(int size) : SmartHandle(HeapAlloc(hHeap, HEAP_NO_SERIALIZE, size)) {}

inline Smartbuffer::~Smartbuffer() {
	if (SHandle) HeapFree(hHeap, HEAP_NO_SERIALIZE, SHandle);
}
