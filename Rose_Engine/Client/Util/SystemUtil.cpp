#include "stdafx.h"
#include ".\systemutil.h"
#include <Tlhelp32.h>


CSystemUtil::CSystemUtil(void)
{
}

CSystemUtil::~CSystemUtil(void)
{
}

DWORD CSystemUtil::FindProcess ( char *szExeName )
{
    HANDLE hSnapshot;
    PROCESSENTRY32  sEntry32 = {0};
    DWORD dwPID=0;
    /*
    typedef struct tagPROCESSENTRY32 {
        DWORD dwSize;
    */
    hSnapshot = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    sEntry32.dwSize = sizeof(PROCESSENTRY32); 
    if ( ::Process32First( hSnapshot, &sEntry32 ) ) {
        do {
            if ( 0 == _strcmpi(szExeName, sEntry32.szExeFile) ) {
                dwPID = sEntry32.th32ProcessID;
                break;
            }
        } while( ::Process32Next( hSnapshot, &sEntry32 ) );
    }
    ::CloseHandle( hSnapshot );
    return dwPID;
}

BOOL CSystemUtil::KillProcess ( DWORD dwPID )
{
    HANDLE hProcess;
    hProcess = ::OpenProcess( PROCESS_ALL_ACCESS, TRUE, dwPID );
    if ( hProcess ) {
        return ::TerminateProcess( hProcess, 0 );
    }
    return FALSE;
}

//=======================================================================================
//
//  Time Sprite
//
//=======================================================================================
//---------------------------------------------------------------------------------------
//  Name  : trgTimeSprite()
//  Desc  : Default c'tor
//  Author: Antonio - 2006-08-23  오전 11:56:42
//---------------------------------------------------------------------------------------
trgTimeSprite::trgTimeSprite()
{
	time = 0;
	hTex = 0;
} //def c'tor

//---------------------------------------------------------------------------------------
//  Name  : ~trgTimeSprite()
//  Desc  : The d'tor
//  Author: Antonio - 2006-08-23  오전 11:56:52
//---------------------------------------------------------------------------------------
trgTimeSprite::~trgTimeSprite()
{
	Unload();
	time = 0;
} //the d'tor

//---------------------------------------------------------------------------------------
//  Name  : Create()
//  Desc  : Load digit texture
//  Author: Antonio - 2006-08-23  오전 11:57:32
//---------------------------------------------------------------------------------------
bool trgTimeSprite::Load( const char *pFileName )
{
	// Load digit
	hTex = loadTexture( pFileName, pFileName, 1, 0 );
	if( hTex == NULL )
	{
		g_pCApp->ErrorBOX( "Load failed for clan time image!!", "File open error" );
		return false;
	}
	return true;
} //Create

//---------------------------------------------------------------------------------------
//  Name  : Delete()
//  Desc  : unload digit texture
//  Author: Antonio - 2006-08-23  오전 11:56:55
//---------------------------------------------------------------------------------------
void trgTimeSprite::Unload()
{
	if ( hTex )
	{
		::unloadTexture( hTex );
		hTex = NULL;
	}
} //Delete

//---------------------------------------------------------------------------------------
//  Name  : SetTime()
//  Desc  : 시간 설정 함수
//  Author: Antonio - 2006-08-23  오전 11:57:52
//---------------------------------------------------------------------------------------
void trgTimeSprite::SetTime( int time )
{
	this->time = time;
} //SetTime

//---------------------------------------------------------------------------------------
//  Name  : DrawDigitSprite()
//  Desc  : Global helper for below function
//  Author: Antonio - 2006-07-31  오후 4:56:24
//---------------------------------------------------------------------------------------
bool DrawDigitSprite( HNODE hTex, int x, int y, int width, int height, int num )
{
	::beginSprite( D3DXSPRITE_ALPHABLEND | D3DXSPRITE_SORT_DEPTH_BACKTOFRONT );	

	int left	= num  * width;
	int top		= 0;
	int right	= left + width;
	int bottom	= top  + height;

	RECT rc = { left, top, right, bottom };
	D3DXVECTOR3 pt( static_cast<float>(x), static_cast<float>(y), 0.0f );

	::drawSprite( hTex, &rc, NULL, &pt, D3DCOLOR_RGBA( 255, 255, 255, 255 ) );

	::endSprite();

	return true;
} //DrawDigitSprite

//---------------------------------------------------------------------------------------
//  Name  : DrawTime()
//  Desc  : 시간 스프라이트를 실질적으로 그리는 함수
//  Author: Antonio - 2006-08-23  오전 11:43:23
//---------------------------------------------------------------------------------------
bool trgTimeSprite::DrawTime( int x, int y )
{
	if ( time )
	{
		int stride = 16;
		int h, m, s;

		h = m = s = 0;

		m = time * 10 / 60;
		if ( m >= 60 )
		{
			h = m / 60;
			m = m % 60;
		}
		s = time * 10 % 60;

		char sz[2], szH[32], szM[32], szS[32];
		_itoa( h, szH, 10 );
		_itoa( m, szM, 10 );
		_itoa( s, szS, 10 );

		int c = strlen( szS );
		for ( int i = 0; i < c; ++i )
		{
			sz[0] = szS[c-1-i];
			sz[1] = '\0';
			x -= stride;
			DrawDigitSprite( hTex, x, y, stride, stride, atoi( sz ) );
		}
		if ( c == 1 )
		{
			x -= stride;
			DrawDigitSprite( hTex, x, y, stride, stride, 0 );
		}

		if ( m != 0 || h != 0 )
		{
			x-= stride / 2;
			DrawDigitSprite( hTex, x, y, stride, stride, 10 );

			c = strlen( szM );
			for ( int i = 0; i < c; ++i )
			{
				sz[0] = szM[c-1-i];
				sz[1] = '\0';
				x -= stride;
				DrawDigitSprite( hTex, x, y, stride, stride, atoi( sz ) );
			}
			if ( c == 1 )
			{
				x -= stride;
				DrawDigitSprite( hTex, x, y, stride, stride, 0 );
			}
		}
		if ( h != 0 )
		{
			x-= stride / 2;
			DrawDigitSprite( hTex, x, y, stride, stride, 10 );

			c = strlen( szH );
			for ( int i = 0; i < c; ++i )
			{
				sz[0] = szH[c-1-i];
				sz[1] = '\0';
				x -= stride;
				DrawDigitSprite( hTex, x, y, stride, stride, atoi( sz ) );
			}
		}
	}

	return true;
} //DrawTime


//=======================================================================================
//
//  Time text
//
//=======================================================================================
//---------------------------------------------------------------------------------------
//  Name  : trgTimeText
//  Desc  : 
//  Author: Antonio - 2006-08-23  오후 12:17:32
//---------------------------------------------------------------------------------------
struct trgTimeText
{
public:
	int DeleteText();
	int AddText( const char *str, D3DCOLOR fore, D3DCOLOR back, int time );
	int DrawText( HNODE hFont, int y, int dt );

	trgTimeText();
	~trgTimeText();

	char *			p;
	int				t;
	D3DCOLOR		c1;
	D3DCOLOR		c2;
}; //trgTimeText

//---------------------------------------------------------------------------------------
//  Name  : trgTimeText()
//  Desc  : Default c'tor
//  Author: Antonio - 2006-08-23  오후 2:04:38
//---------------------------------------------------------------------------------------
inline trgTimeText::trgTimeText()
{
	p  = NULL;
	t  = 0;
	c1 = 0;
	c2 = 0;
} //def d'tor

//---------------------------------------------------------------------------------------
//  Name  : ~trgTimeText()
//  Desc  : The d'tor
//  Author: Antonio - 2006-08-23  오후 2:04:54
//---------------------------------------------------------------------------------------
inline trgTimeText::~trgTimeText()
{
	DeleteText();
} //the d'tor

//---------------------------------------------------------------------------------------
//  Name  : AddText()
//  Desc  : 
//  Author: Antonio - 2006-08-23  오후 2:05:10
//---------------------------------------------------------------------------------------
inline int trgTimeText::AddText( const char *str, D3DCOLOR fore, D3DCOLOR back, int time )
{
	int n = DeleteText();
	p  = new char[strlen( str )+1];
	strcpy( p, str );
	t  = time;
	c1 = fore;
	c2 = back;

	return ( 1 - n );
} //AddText

//---------------------------------------------------------------------------------------
//  Name  : DeleteText()
//  Desc  : 
//  Author: Antonio - 2006-08-23  오후 2:05:16
//---------------------------------------------------------------------------------------
inline int trgTimeText::DeleteText()
{
	if ( p )
	{
		delete[] p;
		p  = NULL;
		t  = 0;
		c1 = 0;
		c2 = 0;
		return 1;
	}
	return 0;
} //DeleteText

//---------------------------------------------------------------------------------------
//  Name  : DrawText()
//  Desc  : 
//  Author: Antonio - 2006-08-23  오후 2:05:24
//---------------------------------------------------------------------------------------
inline int trgTimeText::DrawText( HNODE hFont, int y, int dt )
{
	if ( p )
	{
		if ( t > 0 )
		{
			t -= dt;
		}
		else
		{
			int alpha = c1 >> 24;
			if ( alpha > 0 )
			{
				alpha -= 25;
				if ( alpha < 0 )
				{
					return -DeleteText();
				}
				c1 = ( c1 & 0x00ffffff ) | ( alpha << 24 );
				c2 = ( c2 & 0x00ffffff ) | ( alpha << 24 );
			}
		}

		int x = ::getScreenWidth() / 2 - getFontTextExtent( hFont, p ).cx / 2;

		::beginSprite( D3DXSPRITE_ALPHABLEND );
		::drawFontf( hFont, true, x+1, y+1, c2, "%s", p );
		::drawFontf( hFont, true, x,   y,   c1, "%s", p );
		::endSprite();
	}

	return 0;
} //DrawText


//=======================================================================================
//
//  Time text array
//
//=======================================================================================
static int			g_numActiveTT;


//---------------------------------------------------------------------------------------
//  Name  : trgTimeTextArray()
//  Desc  : Default c'tor
//  Author: Antonio - 2006-08-23  오후 2:07:50
//---------------------------------------------------------------------------------------
trgTimeTextArray::trgTimeTextArray()
{
	texts	= NULL;
	mru		= 0;
	numUsed	= 0;
	numSlot	= 0;
	hFont1	= 0;
	hFont2	= 0;
} //def c'tor

//---------------------------------------------------------------------------------------
//  Name  : ~trgTimeTextArray()
//  Desc  : The d'tor
//  Author: Antonio - 2006-08-23  오후 2:08:04
//---------------------------------------------------------------------------------------
trgTimeTextArray::~trgTimeTextArray()
{
	Destroy();
} //the d'tor

//---------------------------------------------------------------------------------------
//  Name  : Create()
//  Desc  : 타임 텍스트 배열 생성
//  Author: Antonio - 2006-08-23  오후 2:08:19
//---------------------------------------------------------------------------------------
bool trgTimeTextArray::Create( int numCol, HNODE font1, HNODE font2 )
{
	Destroy();
	texts   = new trgTimeText[numCol];
	numSlot = numCol;
	hFont1  = font1;
	hFont2  = font2;
	return true;
} //Create

//---------------------------------------------------------------------------------------
//  Name  : Destroy()
//  Desc  : 타임 텍스트 배열 파괴
//  Author: Antonio - 2006-08-23  오후 2:08:38
//---------------------------------------------------------------------------------------
void trgTimeTextArray::Destroy()
{
	SAFE_DELETE_ARRAY( texts );
	mru	    = 0;
	numUsed = 0;
	numSlot	= 0;
	hFont1	= 0;
	hFont2	= 0;
} //Destroy

//---------------------------------------------------------------------------------------
//  Name  : IsEmpty()
//  Desc  : 배열이 비어 있느냐?
//  Author: Antonio - 2006-08-23  오후 2:08:56
//---------------------------------------------------------------------------------------
bool trgTimeTextArray::IsEmpty()
{
	return ( ( numUsed == 0 ) ? true : false );
} //IsEmpty

//---------------------------------------------------------------------------------------
//  Name  : AddText()
//  Desc  : 새로운 텍스트 추가
//  Author: Antonio - 2006-08-23  오후 2:09:17
//---------------------------------------------------------------------------------------
bool trgTimeTextArray::AddText( const char *szMsg,
								D3DCOLOR fore		/*= TIME_TEXT_YELLOW*/,
								D3DCOLOR back		/*= TIME_TEXT_BLACK*/,
								int time			/*= 7000*/ )
{
	if ( mru >= numSlot )
	{
		mru = 0;
	}
	numUsed += texts[mru++].AddText( szMsg, fore, back, time );

	return true;
}

//---------------------------------------------------------------------------------------
//  Name  : DeleteTexts()
//  Desc  : 배열 비우기
//  Author: Antonio - 2006-08-23  오후 2:09:27
//---------------------------------------------------------------------------------------
void trgTimeTextArray::DeleteTexts()
{
	for ( int i = 0; i < numSlot; ++i )
	{
		numUsed -= texts[i].DeleteText();
	}
} //DeleteTexts


//---------------------------------------------------------------------------------------
//  Name  : DrawTexts()
//  Desc  : 타임 텍스트들 출력
//  Author: Antonio - 2006-08-23  오후 2:09:40
//---------------------------------------------------------------------------------------
bool trgTimeTextArray::DrawTexts()
{
	if ( numUsed == 0 )
	{
		return false;
	}

	int   n	= mru - 1;
	int   y = ::getScreenHeight() / 4;

	for ( int i = 0; i < numSlot; ++i )
	{
		if ( n < 0 )
		{
			n = numSlot - 1;
		}
		HNODE hFont = ( i == 0 ) ? hFont1 : hFont2;
		numUsed += texts[n--].DrawText( hFont, y, ::getTimeDiff() );
		y -= getFontHeight( hFont );
	}

	return true;
} //DrawTexts


#include "../Game.h"
#include "../IO_Terrain.h"
#include "../GameData/CClan.h"

//=======================================================================================
//
//  Macros and Globals
//
//=======================================================================================
// Time sprite instance ptr
trgTimeSprite *		g_pTimeSprite;
// Time texts instance ptr
trgTimeTextArray *	g_pTimeTexts;
// Time lua functions ptr
trgTimeLuaArray *	g_pTimeLuaFuncs;


//---------------------------------------------------------------------------------------
//  Name  : CreateScreenSpaceText()
//  Desc  : 클랜전 관련 시간 출력용 이미지 생성
//  Author: Antonio - 2006-07-31  오후 4:22:59
//---------------------------------------------------------------------------------------
bool CreateScreenSpaceText()
{
#ifdef __CLAN_WAR_SET
	g_pTimeSprite = new trgTimeSprite;
	g_pTimeSprite->Load( "3DData\\Control\\Res\\ClanWarTime.tga" );

	g_pTimeTexts = new trgTimeTextArray;
	g_pTimeTexts->Create( 5, g_GameDATA.m_hFONT[ FONT_14_BOLD ], g_GameDATA.m_hFONT[ FONT_12_BOLD ] );
#endif

	g_pTimeLuaFuncs = new trgTimeLuaArray;

	return true;
}

//---------------------------------------------------------------------------------------
//  Name  : DeleteScreenSpaceText()
//  Desc  : 클랜전 관련 시간 출력용 이미지 삭제
//  Author: Antonio - 2006-07-31  오후 4:23:07
//---------------------------------------------------------------------------------------
void DeleteScreenSpaceText()
{
	SAFE_DELETE( g_pTimeLuaFuncs );
	SAFE_DELETE( g_pTimeTexts );
	SAFE_DELETE( g_pTimeSprite );
}

//---------------------------------------------------------------------------------------
//  Name  : SetSpriteTime()
//  Desc  : 클랜전 시간 설정 함수 (서버 패킷으로 설정)
//  Author: Antonio - 2006-08-23  오전 11:40:53
//---------------------------------------------------------------------------------------
void SetSpriteTime( int time )
{
	if ( g_pTimeSprite )
	{
		g_pTimeSprite->SetTime( time );
	}
} //SetSpriteTime

//---------------------------------------------------------------------------------------
//  Name  : DrawSpriteTime()
//  Desc  : 클랜전 관련 시간 출력 함수
//  Author: Antonio - 2006-08-23  오전 11:40:56
//---------------------------------------------------------------------------------------
bool DrawSpriteTime()
{
	if ( g_pTimeSprite )
	{
		return g_pTimeSprite->DrawTime( getScreenWidth() - 20, getScreenHeight() / 2 );
	}

	return true;
} //DrawSpriteTime

//---------------------------------------------------------------------------------------
//  Name  : DrawFPS
//  Desc  : 화면 hp bar 밑에 프레임 출력
//  Author: Antonio - 2006-08-24  오후 1:54:43
//---------------------------------------------------------------------------------------
void DrawFPS()
{
	char psz[32];
	sprintf( psz, "FPS:%d", g_pCApp->DisplayFrameRate() );

	::beginSprite( D3DXSPRITE_ALPHABLEND );
	::drawFontf( g_GameDATA.m_hFONT[ FONT_LARGE_BOLD ], true, 5, 70, g_dwWHITE, "%s", psz );
	::endSprite();
} //DrawFPS

//---------------------------------------------------------------------------------------
//  Name  : DrawScreenSpaceTexts()
//  Desc  : Screen space 출력 함수
//  Author: Antonio - 2006-08-23  오전 11:52:15
//---------------------------------------------------------------------------------------
void DrawScreenSpaceTexts()
{
	if ( g_pTerrain->GetZoneNO() >= 101 && g_pTerrain->GetZoneNO() <= 150 )
	{
		DrawSpriteTime();
		if ( g_pTimeTexts->IsEmpty() )
		{
			DrawClanWarDescMessages();
		}
		g_pTimeTexts->DrawTexts();
	}
	else
	{
		if ( g_pTimeSprite )
		{
			g_pTimeSprite->SetTime( 0 );
		}
		ClearClanWarVariables();
	}

	g_pTimeLuaFuncs->CallTimeLuaFunctions();
} //DrawScreenSpaceTexts


#include "../System/SystemProcScript.h"

//=======================================================================================
//
//  For the Time Lua
//
//=======================================================================================
#define TTL_TYPE_DEFAULT		0
#define TTL_TYPE_OBJECT			1

trgTimeLua::trgTimeLua()
{
	fn   = NULL;
	time = 0;
	type = 0;
}

trgTimeLua::~trgTimeLua()
{
	DeleteTimeLua();
}

void trgTimeLua::operator =( const trgTimeLua& l )
{
	SetTimeLua( l.fn, l.time, l.type );
}

int trgTimeLua::SetTimeLua( const char *fn, int time, int type )
{
	SAFE_DELETE_ARRAY( this->fn );
	this->fn = new char[strlen(fn)+1];
	strcpy( this->fn, fn );
	this->time = time;
	this->type = type;
	return 1;
}

void trgTimeLua::DeleteTimeLua()
{
	SAFE_DELETE_ARRAY( fn );
	time = 0;
	type = 0;
}

int Call_Char_Lua_Func( int iObject, const char* pFunc );
int trgTimeLua::CallTimeLua( int dt )
{
	int r = 0;

	if ( fn )
	{
		if ( time > 0 )
		{
			time -= dt;
		}
		else
		{
			switch ( type )
			{
			case TTL_TYPE_DEFAULT:
				{
					CSystemProcScript::GetSingleton().CallLuaFunction( r, fn, ZZ_PARAM_END );
				}
			case TTL_TYPE_OBJECT:
				{
					Call_Char_Lua_Func( g_pAVATAR->Get_INDEX(), fn );
				}
			}
			DeleteTimeLua();
			return 1;
		}
	}
	return 0;
}


//=======================================================================================
//
//  For the Time Lua array
//
//=======================================================================================
#define TTL_REALLOC_SIZE			32

trgTimeLuaArray::trgTimeLuaArray()
{
	allocedSize = 0;
	num			= 0;
	p			= NULL;
	EnsureAlloced();
}

trgTimeLuaArray::~trgTimeLuaArray()
{
	SAFE_DELETE_ARRAY( p );
	allocedSize = 0;
	num			= 0;
}

bool trgTimeLuaArray::EnsureAlloced( bool keep /*=true*/ )
{
	return ReAllocate( allocedSize + TTL_REALLOC_SIZE );
}

bool trgTimeLuaArray::ReAllocate( int n, bool keep /*=true*/ )
{
	trgTimeLua *old = p;
	n = (n+3) & ~3;
	p = new trgTimeLua[n];
	memset( p, 0, n * sizeof( p[0] ) );
	if ( old )
	{
		if ( keep )
		{
			for ( int i = 0; i < allocedSize; ++i )
			{
				p[i] = old[i];
			}
		}
		delete[] old;
	}
	allocedSize = n;

	return true;
}

bool trgTimeLuaArray::AddTimeLuaFunction( const char *fn, int time, int type )
{
	int i;
	for ( i = 0; i < allocedSize; ++i )
	{
		if ( p[i].IsEmpty() )
		{
			break;
		}
	}
	if ( i == allocedSize )
	{
		if ( !EnsureAlloced() )
			return false;
	}

	time *= 1000;
	num += p[i].SetTimeLua( fn, time, type );

	return true;
}


void trgTimeLuaArray::CallTimeLuaFunctions( /*int dt*/ )
{
	if ( 0 == num )
	{
		return;
	}
	int dt = ::getTimeDiff();
	for ( int i = 0; i < allocedSize; ++i )
	{
		if ( !p[i].IsEmpty() )
		{
			num -= p[i].CallTimeLua( dt );
		}
	}
}
