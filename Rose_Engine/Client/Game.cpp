/*
    $Header: /Client/Game.cpp 296   07-03-20 10:27a Raning $
*/

#include "stdAFX.h"

#include "Game.h"
//#include "CMouse.h" 
//#include "CKeyboard.h"
#include "IO_Basic.h"
#include "IO_Terrain.h"
#include "IO_Effect.h" 
#include "IO_Event.h"
#include "CModelCHAR.h"
#include "CCamera.h"
#include "Network/CNetwork.h"
#include "OBJECT.h"
#include "BULLET.h"
#include "CSkyDOME.h"
#include "util\classMD5.h"
#include "Common/IO_AI.h"
#include "Common/IO_STB.h"
#include "TGameCtrl.h"
#include "InterFace/it_mgr.h"
#include "InterFace/IO_ImageRes.h"

#include "Game_FUNC.h"
#include "CViewMSG.h"
#include "Debug.h"
#include "Common/IO_Skill.h"
#include "Common/IO_Quest.h"
#include "Interface/CLoading.h"
#include "Interface/CKeyMove.h"	
//#include "Interface\\CUITextManager.h"
#include "Interface/TypeResource.h"
#include "Interface/CUIMediator.h"
#include "Interface/ExternalUI/ExternalUILobby.h"
#include "../TGameCtrl/ResourceMgr.h"
#include "Interface/IO_ImageRes.h"
#include "Interface/CTDrawImpl.h"
#include "Interface/CTSoundImpl.h"
#include "Interface/CTFontImpl.h"
#include "CClientStorage.h"
#include "GameProc/CDayNNightProc.h"
#include "System/CGame.h"



CGAMEDATA _GameData;



CObjUSER	   *g_pAVATAR=NULL;


D3DCOLOR	g_dwRED		= D3DCOLOR_ARGB( 255, 255, 0, 0);
D3DCOLOR	g_dwGREEN	= D3DCOLOR_ARGB( 255, 0, 255, 0);
D3DCOLOR	g_dwBLUE	= D3DCOLOR_ARGB( 255, 0,  0,255);
D3DCOLOR	g_dwBLACK	= D3DCOLOR_ARGB( 255, 0, 0, 0);
D3DCOLOR	g_dwWHITE	= D3DCOLOR_ARGB( 255, 255, 255, 255);
D3DCOLOR	g_dwYELLOW  = D3DCOLOR_ARGB( 255, 255, 255, 0);
D3DCOLOR	g_dwGRAY    = D3DCOLOR_ARGB( 255, 150, 150, 150);
D3DCOLOR	g_dwVIOLET  = D3DCOLOR_ARGB( 255, 255,   0, 255);
D3DCOLOR	g_dwORANGE  = D3DCOLOR_ARGB( 255, 255, 128, 0);
D3DCOLOR g_dwCOLOR[] = { g_dwRED,
						g_dwGREEN,
						g_dwBLUE,
						g_dwBLACK,
						g_dwWHITE,
						g_dwYELLOW,
						g_dwGRAY,
						g_dwVIOLET,
						g_dwORANGE
						};			 

//// Util The Output string The Implementation
void WriteLOG( char* )
{
	;
}



//
//void AddMsgToChatWND (char *szMsg, D3DCOLOR Color, int iType )
//{
//	if ( NULL != szMsg ) 
//	{
//		g_itMGR.AppendChatMsg(szMsg, Color , iType );						
//	}
//}


void DrawLoadingImage()
{
	//bool bLostFocus = g_pCApp->GetMessage ();
	//
	//if( !bLostFocus )
	{
		if ( !::beginScene() ) //  If the device is lost, the status returns 0, so skip all the rendering
		{
			return;
		}

		g_Loading.LoadTexture();

		::clearScreen();

		::beginSprite( D3DXSPRITE_ALPHABLEND );	

		g_Loading.Draw();

		::endSprite();

		::endScene();
		::swapBuffers();
		g_Loading.UnloadTexture();
	}		
}

void DestroyWaitDlg()
{
	CGame::GetInstance().ProcWndMsg(WM_USER_CLOSE_MSGBOX,0,0 ); 
}



//-------------------------------------------------------------------------------------------------

CGAMEDATA::CGAMEDATA () 
{
	m_bWireMode				= false;
	m_bDrawBoundingVolume	= false;
	m_bNoUI					= false;
	m_bNoWeight				= false;;///Ignore the weight limit flag: future clients ppaelgeot
	m_fTestVal				= 0.0f;
	m_iPvPState				= PVP_CANT;

	m_dwGameStartTime		= GetTime();		/// Start time of the game.
	m_dwFrevFrameTime		= 0;				/// Previous frame time
	m_dwElapsedGameTime		= 0;				/// Elapsed time after the start of the game.
	m_dwElapsedFrameTime	= 0;				/// The elapsed time from the previous frame.
	m_bForOpenTestServer    = false;

	m_dwServerTimeGap		= 0;

	m_bTranslate			= false;
	m_iLangIDX				= 0;

	GetSystemTime( &m_SystemTime );

	RandomSeedInit( m_SystemTime.wMilliseconds );

#ifdef _DEBUG
	m_bShowCurPos			= false;
#endif

	m_bFilmingMode			= false;
	m_bShowCursor			= true;
	m_bShowDropItemInfo		= false;

	m_iWorldStaminaVal		= 0;


	m_dwElapsedGameFrame	= 0;

	m_bJustObjectLoadMode	= false;

#ifdef _DEBUG
	m_bObserverCameraMode	= false;; /// Reservoir-up camera for debugging.
#endif

	m_iReceivedAvatarEXP	= 0;
	m_is_NHN_JAPAN			= false;

	m_dwSeq					= 0;
	ZeroMemory( m_szSessionKey, sizeof(m_szSessionKey) );

	m_SkipIntroNote			= false;
	m_SkipIntroLogo			= false;

}

CGAMEDATA::~CGAMEDATA ()
{
}

void CGAMEDATA::UpdateGameTime()
{
	m_dwElapsedGameTime = GetTime() - m_dwGameStartTime;

	m_dwElapsedFrameTime = m_dwElapsedGameTime - m_dwFrevFrameTime;
	m_dwFrevFrameTime = m_dwElapsedGameTime;
}

void CGAMEDATA::Update()
{
	UpdateGameTime();

	m_dwElapsedGameFrame++;
}

int CGAMEDATA::AdjustAvatarStamina( int iGetExp )
{
	int iLostExp = ( ( iGetExp + 20) / ( g_pAVATAR->Get_LEVEL() + 4 ) + 5 ) * ( m_iWorldStaminaVal ) / 100;
	g_pAVATAR->m_GrowAbility.m_nSTAMINA -= iLostExp;

	if( g_pAVATAR->m_GrowAbility.m_nSTAMINA < 0 )
		g_pAVATAR->m_GrowAbility.m_nSTAMINA = 0;

	return g_pAVATAR->m_GrowAbility.m_nSTAMINA;
}

//Cho, Sung - Hyun test 10 - 19

void CGAMEDATA::DrawFontCameraPosition()
{
	char bufferString1[100];
	char bufferString2[100];
    char bufferString3[100];
    char bufferString4[100];

	HNODE hNode;

	D3DXMATRIX mat;	
	D3DXMatrixTranslation( &mat, 0.0f, 0.0f, 0.0f);
	float vEye[3], fPosition[3];
    float fNear, fFar, fFov, fRatio;

	::setTransformSprite( mat );
		
	hNode = ::getCameraDefault();
	::getCameraEye(hNode, vEye);
	::getPosition(hNode,  fPosition);
	::getCameraPerspectiveElement(hNode, &fFov, &fRatio, &fNear, &fFar);
	
	sprintf(bufferString1, " EYE = ( %f  %f  %f)", vEye[0], vEye[1], vEye[2] );
	sprintf(bufferString2, " POSITION = (%f  %f  %f)", fPosition[0], fPosition[1], fPosition[2]);
	sprintf(bufferString3, " FOV = %f  RATIO = %f  NEAR = %f  FAR = %f", fFov, fRatio, fNear, fFar);   
    sprintf(bufferString4, " Effect Number = %d", g_pEffectLIST->GetCount());
 

	RECT rcDraw;
	SetRect( &rcDraw, 10, 30, 500, 50 );
	drawFont( g_GameDATA.m_hFONT[ FONT_NORMAL ], true, &rcDraw, g_dwRED, DT_LEFT, bufferString1 );
    
	SetRect( &rcDraw, 10, 50, 5000, 70 );
	drawFont( g_GameDATA.m_hFONT[ FONT_NORMAL ], true, &rcDraw, g_dwRED, DT_LEFT, bufferString2 );
 
	SetRect( &rcDraw, 10, 70, 5000, 90 );
	drawFont( g_GameDATA.m_hFONT[ FONT_NORMAL ], true, &rcDraw, g_dwRED, DT_LEFT, bufferString3 );
	
	SetRect( &rcDraw, 10, 90, 5000, 110 );
	drawFont( g_GameDATA.m_hFONT[ FONT_NORMAL ], true, &rcDraw, g_dwRED, DT_LEFT, bufferString4 );




}

