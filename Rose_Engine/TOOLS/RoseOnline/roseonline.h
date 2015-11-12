#pragma once
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#pragma comment(lib, "Urlmon.lib")
#pragma comment(lib, "Wininet.lib")
#pragma comment(lib, "User32.lib")
#pragma comment(lib, "msimg32.lib")

#define PATCHER_VERSION 1000

#define MAX_GRAPHICS 22
#define MAX_NEWSTITLES 10

#define GFX_MAINWND 0
#define GFX_PLAYOVER 1
#define GFX_PROGRESSEMPTY 2
#define GFX_PROGRESSFULL 3
#define GFX_NEWSBAR 4
#define GFX_NEWSBAROVER 5
#define GFX_NEWSBG 6
#define GFX_PLAYDISABLED 7
#define GFX_PLAYDOWN 8
#define GFX_PLAYENABLED 9
//#define GFX_MASKWND 10
#define GFX_EXIT1 10
#define GFX_EXIT2 11
#define GFX_EXIT3 12
#define GFX_MANUALNORM 13
#define GFX_MANUALDOWN 14
#define GFX_OPTIONSNORM 15
#define GFX_OPTIONSDOWN 16
#define GFX_DONATENORM 17
#define GFX_DONATEDOWN 18
#define GFX_FORUMNORM 19
#define GFX_FORUMDOWN 20
#define GFX_REGNORM 21
#define GFX_REGDOWN 22

#define ICO_MAIN 17

#define newsx 290
#define newsy 112
#define windowsizex 625
#define windowsizey 468

#include "stdafx.h"
#include "downloader.h"
#include "vfs.h"
#include "mydownloader.h"

int APIENTRY WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow );

extern bool bRunning;
extern HINSTANCE hMyInstance;
extern HWND hWnd;
extern HDC hBackBuffer;
extern HDC hMyGraphics[ MAX_GRAPHICS ];
extern CMyDownloader dMyDownloader;
extern ULONG uPlayBtnState;
extern ULONG uExitBtnState;
extern ULONG uUpdateBtnState;
extern ULONG uOptionsBtnState;
extern ULONG uDonateBtnState;
extern ULONG uForumBtnState;
extern ULONG uRegisterBtnState;
extern ULONG uNewsBarState[ MAX_NEWSTITLES ];
extern ULONG uPrg1Max, uPrg1Val, uPrg2Max, uPrg2Val;
extern char cpStatusText[ 1024 ];
extern char cpNews[10][2][1024];
extern char cpStartStr[ 1024 ];
extern int cpStartPortStr;
extern int ifsupdateid;
extern BLENDFUNCTION m_bf;