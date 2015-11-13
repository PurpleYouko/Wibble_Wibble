#ifndef _GAME_UTIL_
#define _GAME_UTIL_

#include <d3d9.h>
#include <d3dx9.h>
#include <crtdbg.h>
//----------------------------------------------------------------------------------------------------
/// Miscellaneous utility functions for games
/// class CGameUtil
//----------------------------------------------------------------------------------------------------

class CGameUtil
{
public:
	CGameUtil(void);
	~CGameUtil(void);


	static D3DXCOLOR	GetRGBFromString( unsigned int iColor );
	static WORD			GetPrimaryLanguage();
	static WORD			GetSubLanguage();
	static void			ConvertMoney2String( __int64 money, char* buffer, int buffer_size );	
	static void 		Convert2String( __int64 i64, char * buffer );


};

extern CGameUtil g_GameUtil;

#endif //_GAME_UTIL_
