#pragma once

#define UNICODE

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

	class String
	{
	public:

		String();
		String(LPWCH str);
		~String();
		
		WCHAR ReadChar();
		void ReadStr();

		DWORD BuffUsed();
		DWORD Left();

		void Push();
		void Reset();

		String* operator = (LPWCH str);
		String* operator += (LPWCH str);
		operator LPWSTR();

	private:
		static const DWORD PageSize = 0x40;

		LPVOID Buff;
		DWORD Size;
		DWORD CurrentString;
		DWORD Len;
		DWORD LastRead;

		void Alloc(DWORD len, bool ReAlloc = false);
		void CheckSize(DWORD len);
		void stdRead();
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
void stdWrite(LPWCH str);
WCHAR ReadMenu(LPVOID buff);
DWORD ReadBuffer(LPVOID buff);
void ClearConsole();
void ErrExit(DWORD Kode);