#ifndef __CPUNK_H__
#define __CPUNK_H__
#include <Windows.h>
#include <vector>
#include <string>
using std::vector;
using std::string;

#define IDT_PUNK 0x7659

typedef vector<string> LST;
class CPunk
{
public :
	bool Check();
	CPunk(HWND hwnd, DWORD interval );
	~CPunk();
private :
	static LST suspect;
	static bool KillPid( DWORD pid );
	static void convertolower(char *txt, size_t len);
	
	HWND m_hWnd;
	DWORD m_interval;
	static BOOL CALLBACK KilWindow(HWND hwnd, LPARAM lParam );
	static char WndText[256];
};

#endif