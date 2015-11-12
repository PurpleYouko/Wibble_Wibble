#pragma once

class CSystemUtil
{
public:
	CSystemUtil(void);
	~CSystemUtil(void);

	DWORD		FindProcess( char* szExeName );
	BOOL		KillProcess ( DWORD dwPID );
};

//=======================================================================================
//
//  For the Clan War
//
//=======================================================================================
//---------------------------------------------------------------------------------------
//  Name  : trgTimeSprite
//  Desc  : 스프라이트 시간 그리기 클래스
//  Author: Antonio - 2006-08-23  오전 11:32:06
//---------------------------------------------------------------------------------------
class trgTimeSprite
{
public:
	bool	Load( const char *pFileName );
	void	SetTime( int time );
	bool	DrawTime( int x, int y );
	void	Unload();

	trgTimeSprite();
	~trgTimeSprite();

public:
	int		time;
	HNODE	hTex;
}; //trgTimeSprite


//---------------------------------------------------------------------------------------
//	Globals for trgTimeTextArray
//---------------------------------------------------------------------------------------
const D3DCOLOR TIME_TEXT_BLACK  = D3DCOLOR_ARGB( 255,   0,   0,   0 );
const D3DCOLOR TIME_TEXT_YELLOW = D3DCOLOR_ARGB( 255, 255, 152,  40 );
const D3DCOLOR TIME_TEXT_BLUE   = D3DCOLOR_ARGB( 255,  36,  36, 190 );
const D3DCOLOR TIME_TEXT_RED    = D3DCOLOR_ARGB( 255, 228,  26,  19 );


//---------------------------------------------------------------------------------------
//  Name  : trgTimeTextArray
//  Desc  : 
//  Author: Antonio - 2006-08-23  오후 1:51:19
//---------------------------------------------------------------------------------------
class trgTimeTextArray
{
public:
	bool	Create( int numCol, HNODE font1, HNODE font2 );
	void	Destroy();
	bool	IsEmpty();
	bool	AddText( const char *szMsg,
					 D3DCOLOR fore		= TIME_TEXT_YELLOW,
					 D3DCOLOR back		= TIME_TEXT_BLACK,
					 int time			= 7000 );
	void	DeleteTexts();
	bool	DrawTexts();

	trgTimeTextArray();
	~trgTimeTextArray();

public:
	struct trgTimeText *texts;
	int					mru;
	int					numUsed;
	int					numSlot;
	HNODE				hFont1;
	HNODE				hFont2;
}; //trgTimeTextArray


//=======================================================================================
//
//  For the Time Lua
//
//=======================================================================================
//---------------------------------------------------------------------------------------
//  Name  : trgTimeLua
//  Desc  : 타임 루아 데이타
//  Author: Antonio - 2006-09-11  오후 4:00:45
//---------------------------------------------------------------------------------------
struct trgTimeLua
{
	char *	fn;
	int		time;
	int		type;

	trgTimeLua();
	~trgTimeLua();

	void operator =( const trgTimeLua& l );
	int  SetTimeLua( const char *fn, int time, int type );
	int  CallTimeLua( int dt );
	void DeleteTimeLua();
	bool IsEmpty()	{ return ( fn == NULL ) ? true : false; }
}; //trgTimeLua

//---------------------------------------------------------------------------------------
//  Name  : trgTimeLuaArray
//  Desc  : 타임 루아 데이타 배열
//  Author: Antonio - 2006-09-11  오후 4:00:58
//---------------------------------------------------------------------------------------
struct trgTimeLuaArray
{
	trgTimeLuaArray();
	~trgTimeLuaArray();

	bool AddTimeLuaFunction( const char *fn, int time, int type );
	void CallTimeLuaFunctions( /*int dt*/ );
	bool EnsureAlloced( bool keep = true );
	bool ReAllocate( int n, bool keep = true );

	trgTimeLua *	p;
	int				allocedSize;
	int				num;
}; //trgTimeLuaArray


//---------------------------------------------------------------------------------------
//
//	Macros and Globals
//
//---------------------------------------------------------------------------------------
// Time sprite instance ptr
extern trgTimeSprite *		g_pTimeSprite;
// Time texts instance ptr
extern trgTimeTextArray *	g_pTimeTexts;
// Time lua functions ptr
extern trgTimeLuaArray *	g_pTimeLuaFuncs;

// 스프라이트 타임 출력 함수들
void SetSpriteTime( int time );
bool DrawSpriteTime();

// 스크린 텍스트 출력 관련 함수들
bool CreateScreenSpaceText();
void DeleteScreenSpaceText();
void DrawScreenSpaceTexts();
void DrawFPS();

