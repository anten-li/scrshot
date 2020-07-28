#include "scrshot.h"

#pragma comment(linker, "/MERGE:.CRT=.rdata")

/*
// CRT заглушка
#ifndef _DEBUG

#pragma section(".CRT$XCA",long,read)
#pragma section(".CRT$XCZ",long,read)

using _PVFV = void(__cdecl*)(void);

const int STATIC_ARR_SIZE = 32;

__declspec(allocate(".CRT$XCA")) _PVFV __xc_a[] = { 0 };
__declspec(allocate(".CRT$XCZ")) _PVFV __xc_z[] = { 0 };

_PVFV staticArr[STATIC_ARR_SIZE];
int LPstaticArr = 0;

inline void __cdecl _initterm(_PVFV* const first, _PVFV* const last)
{
	for (_PVFV* it = first; it != last; ++it)
	{
		if (*it == nullptr)
			continue;

		(**it)();
	}
}

int atexit(_PVFV func) {
	if (LPstaticArr < STATIC_ARR_SIZE) {
		staticArr[LPstaticArr++] = func;
		return 0;
	}
	RaiseException(ERROR_NOT_FOUND, NULL, NULL, NULL);
	return 0;
}

inline void unInit() {
	for (size_t i = 0; i < STATIC_ARR_SIZE; i++)
	{
		if (staticArr[i])
			(*staticArr[i])();
		else
			break;
	}
}

int main();

int mainCRT() {
	_initterm(__xc_a, __xc_z);
	int resalt = main();
	unInit();
	return resalt;
}

#endif
// CRT заглушка конец
*/

const int BUFFER_SIZE = 256;

HANDLE hstdOUT;
HANDLE hstdIN;
HANDLE hHeap;

scr::SaveOption saveOption = scr::SaveOption::file;

int main()
{

	if ((hstdOUT = GetStdHandle(STD_OUTPUT_HANDLE)) != INVALID_HANDLE_VALUE &&
		(hstdIN = GetStdHandle(STD_INPUT_HANDLE)) != INVALID_HANDLE_VALUE &&
		(hHeap = GetProcessHeap()) != NULL)
	{
		HMODULE User32;
		FARPROC SetDPI;

		if ((User32 = GetModuleHandle(L"User32.dll")) &&
			(SetDPI = GetProcAddress(User32, "SetProcessDPIAware")))
		{
			SetDPI();
		}

		TreadPadam scrParam;
		scr::String* strBuff = &scrParam.strBuff;
		scr::progMenu menu = scr::progMenu::main;

		while (true)
		{
			WCHAR Readed;

			if (menu == scr::progMenu::main)
			{
				*strBuff = L"введите число:\n\t1 - запуск\n\t2 - настройки сохранения: ";

				if (saveOption == scr::SaveOption::file)
					*strBuff += L"в файл\n\n";
				else if (saveOption == scr::SaveOption::dropbox)
					*strBuff += L"в dropbox\n\n";

				stdWrite(*strBuff);
				Readed = strBuff->ReadChar();
				if (Readed == *L"1")
					break;
				else if (Readed == *L"2")
				{
					menu = scr::progMenu::SaveOption;
				}
			}
			else if (menu == scr::progMenu::SaveOption)
			{
				stdWrite(L"настройки сохранения\n\nвведите число:\n\t1 - в файл\n\t2 - dropbox\n\n");
				Readed = strBuff->ReadChar();
				if (Readed == *L"1")
				{
					saveOption = scr::SaveOption::file;
					menu = scr::progMenu::main;
				}
				else if (Readed == *L"2")
				{
					stdWrite(L"введите токен dropbox:\n");

					strBuff->Reset();
					strBuff->ReadStr();
					strBuff->Push();

					saveOption = scr::SaveOption::dropbox;
					menu = scr::progMenu::main;
				}
			}
			ClearConsole();
		}
		HANDLE ScreenThread = CreateThread(NULL, NULL, MakeScreen, &scrParam, NULL, NULL);
		stdWrite(L"нажмите enter для завершения:\n");

		DWORD Readed;
		DWORD DWbuff;
		ReadConsole(hstdIN, &DWbuff, sizeof(DWORD) / sizeof(TCHAR), &Readed, NULL);

		SetEvent(scrParam.ExitEvent);
		WaitForSingleObject(ScreenThread, INFINITE);
	}

	return 0;
}

DWORD __stdcall MakeScreen(LPVOID lpParameter)
{
	TreadPadam* scrParam = (TreadPadam*)lpParameter;

	scr::HDC hDisp(GetDC(NULL));
	if (hDisp) {

		do
		{
			SYSTEMTIME TimeBigin;
			scr::Buffer DataString;
			scr::Buffer FileName;
			const WCHAR FileFormat[] = L".png";

			GetSystemTime(&TimeBigin);
			int size = GetDateFormat(LOCALE_SYSTEM_DEFAULT, NULL, &TimeBigin, L"yyyyMMdd_HHmmss_UTC", NULL, NULL);
			DataString.allocate(size * sizeof(TCHAR));
			GetDateFormat(LOCALE_SYSTEM_DEFAULT, NULL, &TimeBigin, L"yyyyMMdd_HHmmss_UTC", (LPWSTR)(LPVOID)DataString, size);
			size = GetTimeFormat(LOCALE_SYSTEM_DEFAULT, NULL, &TimeBigin, (LPWSTR)(LPVOID)DataString, NULL, NULL);
			FileName.allocate(size * sizeof(TCHAR) + sizeof(FileFormat));
			GetTimeFormat(LOCALE_SYSTEM_DEFAULT, NULL, &TimeBigin, (LPWSTR)(LPVOID)DataString, (LPWSTR)(LPVOID)FileName, size);


		} while (WaitForSingleObject(scrParam->ExitEvent, 5000) != WAIT_OBJECT_0);

	}
	return 0;
}

inline HANDLE GetHeap()
{
	return hHeap;
}

void stdWrite(LPWCH str)
{
	DWORD Writen;
	WriteConsole(hstdOUT, str, lstrlen(str), &Writen, NULL);
}

WCHAR ReadMenu(LPVOID buff)
{
	if (ReadBuffer(buff) == 1 + sizeof(L"\n") / sizeof(TCHAR)) return *(LPWCH)buff;
	else return 0;
}

inline DWORD ReadBuffer(LPVOID buff)
{
	DWORD Readed;
	ReadConsole(hstdIN, buff, BUFFER_SIZE / sizeof(TCHAR), &Readed, NULL);
	return Readed;
}

void ClearConsole() {
	CONSOLE_SCREEN_BUFFER_INFO screen;
	DWORD Writen;
	const COORD home = { 0, 0 };

	if (GetConsoleScreenBufferInfo(hstdOUT, &screen))
	{
		FillConsoleOutputCharacter(hstdOUT, *L" ", screen.dwSize.X * screen.dwSize.Y,
			home, &Writen);
		SetConsoleCursorPosition(hstdOUT, home);
	}
}

[[noreturn]] void ErrExit(DWORD Kode)
{
	RaiseException(Kode, NULL, NULL, NULL);
}

template<class T>
inline scr::Handle<T>::Handle(T Handle) : SHandle(Handle) {}

template<class T>
inline scr::Handle<T>::operator bool() { return SHandle; }

template<class T>
inline scr::Handle<T>::operator T() { return SHandle; }

inline scr::HDC::HDC(::HDC Handle) : Handle(Handle) {}

inline scr::HDC::~HDC() { if (SHandle) ReleaseDC(NULL, SHandle); }

inline scr::Buffer::Buffer(int size) : Handle(HeapAlloc(GetHeap(), HEAP_NO_SERIALIZE, size)) {}

inline scr::Buffer::Buffer() : Handle(NULL) {}

inline scr::Buffer::~Buffer() { if (SHandle) HeapFree(GetHeap(), HEAP_NO_SERIALIZE, SHandle); }

inline void scr::Buffer::allocate(int size)
{
	if (SHandle) HeapFree(GetHeap(), HEAP_NO_SERIALIZE, SHandle);
	SHandle = HeapAlloc(GetHeap(), HEAP_NO_SERIALIZE, size);
}

inline scr::objectHandle::objectHandle(HANDLE Handle) : Handle(Handle) {}

inline scr::objectHandle::~objectHandle() { if (SHandle) CloseHandle(SHandle); }

inline TreadPadam::TreadPadam() : ExitEvent(CreateEvent(NULL, FALSE, FALSE, NULL)) {}

/* String */

inline scr::String::String() :
	Buff(NULL),
	CurrentString(0),
	Len(0),
	Size(0),
	LastRead(0)
{}

inline scr::String::String(LPWCH str) : String()
{
	*this = str;
}

inline scr::String::~String()
{
	if (this->Buff) HeapFree(GetHeap(), HEAP_NO_SERIALIZE, this->Buff);
}

WCHAR scr::String::ReadChar()
{
	this->stdRead();
	if (this->LastRead == 1 + sizeof(L"\n") / sizeof(TCHAR)) return *((LPWCH)(*this));
	else return 0;
}

void scr::String::ReadStr()
{
	this->stdRead();
	this->Len = this->LastRead - sizeof(L"\n") / sizeof(TCHAR);
	*((LPWCH)(*this) + this->Len) = 0;
}

inline DWORD scr::String::BuffUsed()
{
	return this->CurrentString + this->Len * sizeof(TCHAR) + sizeof(*L"");
}

DWORD scr::String::Left()
{
	return this->Size - this->BuffUsed();
}

void scr::String::Push()
{
	this->CurrentString += this->Len * sizeof(TCHAR) + sizeof(*L"");
	this->Len = 0;
	this->CheckSize(1);
}

void scr::String::Reset()
{
	this->CurrentString = 0;
	this->Len = 0;
}

scr::String* scr::String::operator=(LPWCH str)
{
	this->Len = 0;
	return this->operator+=(str);
}

scr::String* scr::String::operator+=(LPWCH str)
{
	unsigned int NewLen = this->Len + lstrlen(str);

	this->CheckSize(NewLen * sizeof(TCHAR) + sizeof(*L""));

	if (!lstrcpy((LPWCH)(*this) + this->Len, str))
		ErrExit(ERROR_NOT_ENOUGH_MEMORY);

	this->Len = NewLen;
	return this;
}

scr::String::operator LPWSTR()
{
	return (LPWSTR)((LPBYTE)(this->Buff) + this->CurrentString);
}

void scr::String::Alloc(DWORD len, bool ReAlloc)
{
	DWORD NewSize = scr::String::PageSize * ((this->CurrentString + len) / scr::String::PageSize + 1);
	LPVOID Newbuff;
	if (ReAlloc)
		Newbuff = HeapReAlloc(GetHeap(), HEAP_NO_SERIALIZE, this->Buff, NewSize);
	else
		Newbuff = HeapAlloc(GetHeap(), HEAP_NO_SERIALIZE, NewSize);

	if (Newbuff)
	{
		this->Buff = Newbuff;
		this->Size = NewSize;
	}
	else
		ErrExit(ERROR_NOT_ENOUGH_MEMORY);
}

void scr::String::CheckSize(DWORD len)
{
	if (!this->Buff)
		this->Alloc(len);
	else if (len > this->Size - this->CurrentString)
		this->Alloc(len, true);
}

void scr::String::stdRead()
{
	this->Len = 0;
	ReadConsole(hstdIN, (LPWCH)(*this), this->Left() / sizeof(TCHAR), &(this->LastRead), NULL);
	if (this->LastRead * sizeof(TCHAR) == this->Size)
		ErrExit(ERROR_NOT_ENOUGH_MEMORY);
}
