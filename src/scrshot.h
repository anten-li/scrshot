#pragma once

#define UNICODE

#define mkStr(str) scr::lnString((LPWSTR)str, sizeof(str) / sizeof(TCHAR) - 1)

#include <Windows.h>
//#include <gdiplus.h>

namespace scr {

	enum class progMenu
	{
		main,
		SaveOption
	};

	enum class SaveOption
	{
		file,
		dropbox
	};

	template <class T>
	class Handle
	{
	protected:
		T SHandle;
	public:
		Handle(T Handle);
		operator bool();
		operator T();
	};

	class objectHandle : public Handle<HANDLE>
	{
	public:
		objectHandle(HANDLE Handle);
		~objectHandle();
	};

	class HDC : public Handle<::HDC>
	{
	public:
		HDC(::HDC Handle);
		~HDC();
	};

	class Buffer : public Handle<LPVOID>
	{
	public:
		Buffer(int size);
		Buffer();
		~Buffer();
		void allocate(int size);
	};

	struct lnString
	{
		DWORD len;
		LPWCH str;

		lnString(LPWCH str, DWORD len);
	};

	class String
	{
	public:

		String();
		String(LPWCH str);
		~String();
		
		WCHAR ReadChar();
		void ReadStr();
		void stdWrite();
		void stdWrite(lnString str);

		DWORD BuffUsed();
		DWORD Left();
		DWORD LeftLen();
		DWORD GetOffset();
		void SetOffset(DWORD Offset);
		LPWSTR GetStr(DWORD lpStr);
		void SetLen(DWORD len);
		void Push();

		//String* operator = (LPWCH str);
		String* operator = (lnString str);
		String* operator += (lnString str);
		operator LPWSTR();

	private:
		static const DWORD PageSize = 0x40;

		LPVOID Buff;
		DWORD Size;
		DWORD CurrentString;
		DWORD Len;
		DWORD LastReadWrite;

		void Alloc(DWORD len);
		void CheckSize(DWORD len);
		void stdRead();
		void strAdd(DWORD cLen, DWORD nLen, LPWCH str);
	};
}

struct TreadPadam
{
	scr::String strBuff;
	scr::objectHandle ExitEvent;

	TreadPadam();
};

DWORD WINAPI MakeScreen(LPVOID lpParameter);
inline HANDLE GetHeap();
void ClearConsole();
void ErrExit(DWORD Kode);