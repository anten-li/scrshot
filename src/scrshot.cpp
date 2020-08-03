#include "scrshot.h"

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
					*strBuff = mkStr(L"введите число:\n\t1 - запуск\n\t2 - настройки сохранения: ");

					if (saveOption == scr::SaveOption::file)
						*strBuff += mkStr(L"в файл\n\n");
					else if (saveOption == scr::SaveOption::dropbox)
						*strBuff += mkStr(L"в dropbox\n\n");

					strBuff->stdWrite();
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
					strBuff->stdWrite(mkStr(L"настройки сохранения\n\nвведите число:\n\t1 - в файл\n\t2 - dropbox\n\n"));
					Readed = strBuff->ReadChar();
					if (Readed == *L"1")
					{
						saveOption = scr::SaveOption::file;
						menu = scr::progMenu::main;
					}
					else if (Readed == *L"2")
					{
						strBuff->stdWrite(mkStr(L"введите токен dropbox:\n"));

						strBuff->SetOffset(0);
						strBuff->ReadStr();
						strBuff->Push();

						saveOption = scr::SaveOption::dropbox;
						menu = scr::progMenu::main;
					}
				}
				ClearConsole();
			}
			HANDLE ScreenThread = CreateThread(NULL, NULL, MakeScreen, &scrParam, NULL, NULL);
			if (ScreenThread)
			{
				strBuff->stdWrite(mkStr(L"нажмите enter для завершения:\n"));

				DWORD Readed;
				DWORD DWbuff;
				ReadConsole(hstdIN, &DWbuff, sizeof(DWORD) / sizeof(TCHAR), &Readed, NULL);

				SetEvent(scrParam.ExitEvent);
				WaitForSingleObject(ScreenThread, INFINITE);
			}
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
			DWORD Writen;
			scr::String* strBuff = &scrParam->strBuff;
			DWORD StertOffset = strBuff->GetOffset();

			GetSystemTime(&TimeBigin);
			Writen = GetDateFormat(LOCALE_SYSTEM_DEFAULT, NULL, &TimeBigin, L"yyyyMMdd_HHmmss_UTC", (LPWSTR)(*strBuff), strBuff->LeftLen());
			if (!Writen) ExitProcess(NULL);
			strBuff->SetLen(Writen - 1);
			
			strBuff->Push();
			Writen = GetTimeFormat(LOCALE_SYSTEM_DEFAULT, NULL, &TimeBigin, strBuff->GetStr(StertOffset), (LPWSTR)(*strBuff), strBuff->LeftLen());
			if (!Writen) ExitProcess(NULL);
			strBuff->SetLen(Writen - 1);
			*strBuff += mkStr(L".png");
			DWORD FileName = strBuff->GetOffset();

			strBuff->Push();



			strBuff->SetOffset(StertOffset);
		} while (WaitForSingleObject(scrParam->ExitEvent, 5000) != WAIT_OBJECT_0);

	}
	return 0;
}

inline HANDLE GetHeap()
{
	return hHeap;
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
	LastReadWrite(0)
{}

inline scr::String::String(LPWCH str) : String()
{
	*this = str;
}

inline scr::String::~String()
{
	if (this->Buff) HeapFree(GetHeap(), HEAP_NO_SERIALIZE, this->Buff);
}

inline WCHAR scr::String::ReadChar()
{
	this->stdRead();
	if (this->LastReadWrite == 1 + sizeof(L"\n") / sizeof(TCHAR)) return *((LPWCH)(*this));
	else return 0;
}

inline void scr::String::ReadStr()
{
	this->stdRead();
	this->Len = this->Len - sizeof(L"\n") / sizeof(TCHAR);
	*((LPWCH)(*this) + this->Len) = 0;
}

inline DWORD scr::String::BuffUsed()
{
	return this->CurrentString + this->Len * sizeof(TCHAR) + sizeof(*L"");
}

inline DWORD scr::String::Left()
{
	return this->Size - this->BuffUsed();
}

DWORD scr::String::LeftLen()
{
	return this->Left() / sizeof(TCHAR) - 1;
}

DWORD scr::String::GetOffset()
{
	return this->CurrentString;
}

void scr::String::SetOffset(DWORD Offset)
{
	this->CurrentString = Offset;
	this->Len = 0;
}

LPWSTR scr::String::GetStr(DWORD lpStr)
{
	return (LPWCH)((LPBYTE)(this->Buff) + lpStr);
}

void scr::String::SetLen(DWORD len)
{
	this->Len = len;
	//this->CheckSize(len * sizeof(TCHAR) + sizeof(*L""));
}

inline void scr::String::Push()
{
	this->CurrentString += this->Len * sizeof(TCHAR) + sizeof(*L"");
	this->Len = 0;
	this->CheckSize(1);
}

inline scr::String* scr::String::operator=(lnString str)
{
	this->Len = 0;
	this->strAdd(0, str.len, str.str);
	return this;
}

inline scr::String* scr::String::operator+=(lnString str)
{
	this->strAdd(this->Len, str.len, str.str);
	return this;
}

inline scr::String::operator LPWSTR()
{
	return (LPWSTR)((LPBYTE)(this->Buff) + this->CurrentString);
}

inline void scr::String::Alloc(DWORD len)
{
	DWORD NewSize = scr::String::PageSize * ((this->CurrentString + len) / scr::String::PageSize + 1);
	LPVOID Newbuff;
	if (this->Buff)
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

inline void scr::String::CheckSize(DWORD len)
{
	//bool BuffIsSet = (bool)this->Buff;

	if (len > this->Size - this->CurrentString)
		this->Alloc(len);
}

inline void scr::String::stdRead()
{
	this->Len = 0;

	while (true)
	{
		ReadConsole(hstdIN, (LPWCH)(*this) + this->Len, this->Left() / sizeof(TCHAR), &(this->LastReadWrite), NULL);
		this->Len += this->LastReadWrite;
		if (this->Left() == 0)
			this->CheckSize((this->Len + 1) * sizeof(TCHAR) + sizeof(*L""));
		else break;
	}
}

inline void scr::String::strAdd(DWORD cLen, DWORD nLen, LPWCH str)
{
	unsigned int NewLen = cLen + nLen; //!!

	this->CheckSize(NewLen * sizeof(TCHAR) + sizeof(*L""));

	if (!lstrcpy((LPWCH)(*this) + cLen, str))
		ErrExit(ERROR_NOT_ENOUGH_MEMORY);

	this->Len = NewLen;
}

inline void scr::String::stdWrite()
{
	WriteConsole(hstdOUT, *this, this->Len, &this->LastReadWrite, NULL);
}

void scr::String::stdWrite(lnString str)
{
	WriteConsole(hstdOUT, str.str, str.len, &this->LastReadWrite, NULL);
}

/* lnString */

scr::lnString::lnString(LPWCH str, DWORD len) : str(str), len(len) {}