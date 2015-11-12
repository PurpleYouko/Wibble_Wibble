#include "stdafx.h"
#include ".\cgamestateMoveMain.h"
#include "CGame.h"
#include "SystemProcScript.h"
#include "../IO_Terrain.h"
#include "../CCamera.h"
#include "../interface/CUIMediator.h"
#include "../interface/CLoading.h"


CGameStateMoveMain::CGameStateMoveMain( int iID )
{
	m_iStateID = iID;
	m_bswitchImage = false;
	m_iMoveMainState = 0;
}

CGameStateMoveMain::~CGameStateMoveMain(void)
{
	m_mMapLoadingImageTableByEvent.clear();
}

void CGameStateMoveMain::ServerDisconnected()
{
	g_pCApp->SetExitGame();
}

int CGameStateMoveMain::ProcWndMsgInstant( unsigned uiMsg, WPARAM wParam, LPARAM lParam )
{
	return 0;
}

int CGameStateMoveMain::ProcMouseInput( UINT uiMsg, WPARAM wParam, LPARAM lParam )
{
	return 0;
}

int CGameStateMoveMain::ProcKeyboardInput( UINT uiMsg, WPARAM wParam, LPARAM lParam )
{
	return 0;
}

int	CGameStateMoveMain::Update( bool bLostFocus )
{
	g_pCamera->Update ();

	D3DVECTOR PosENZIN = g_pCamera->Get_Position ();	
	g_pTerrain->SetCenterPosition( PosENZIN.x, PosENZIN.y );

	::updateScene ();

	// processing  ...
	if ( !bLostFocus ) 
	{


		if ( ::beginScene() ) // 성공한 경우에만 렌더링
		{

			::clearScreen();

			if(m_iMoveMainState == 0)
				::renderScene();


			::beginSprite( D3DXSPRITE_ALPHABLEND );

			if(m_iMoveMainState == 0)
			{
				/// Screen message display
				g_UIMed.Draw();
				/// UI display
				///카메라 모션이 멈춰있을경우에만 보이게 한다.
			}

			if(m_iMoveMainState == 1)
				g_Loading.Draw();

			::endSprite();		


			::endScene ();
			::swapBuffers();
		}

		if( getAnimatableState( g_pCamera->GetZHANDLE() ) == 0 )
		{
			CheckInterruptTime();
			if(m_iMoveMainState == 0)
				::ScreenFadeInStart( 1.5f, 1.0f, 1.5f, 255, 255, 255);
			if(InputOnOff)
			{

				CGame::GetInstance().ChangeState( CGame::GS_PREPAREMAIN );
			}
		}
	}

	return 0;	

}

int	CGameStateMoveMain::Enter( int iPrevStateID )
{
	CSystemProcScript::GetSingleton().CallLuaFunction( "EnterMoveMain", ZZ_PARAM_END );

	InputInterruptTime = 0;
	InputOnOff = false;
	m_iMoveMainState = 0;

	gsv_TELEPORT_REPLY Reply;
	CGame::GetInstance().GetLoadingData( Reply );

	g_Loading.InitLoadingImageManager();
	m_EventLoading = g_Loading.GetEventLoadingTable();


#define	MAX_OBJ_VAR_CNT		20

	struct tagObjVAR 
	{
		union 
		{
			struct 
			{
				int			m_iNextCheckTIME;
				t_HASHKEY	m_HashNextTrigger;
				int			m_iPassTIME;
				union {
					short	m_nEventSTATUS;
					short	m_nAI_VAR[ MAX_OBJ_VAR_CNT ];
				} ;
			} ;
			BYTE m_pVAR[ 1 ];
		};		
	};

	tagObjVAR* pObjVAR = (tagObjVAR*)Reply.m_pDATA;

	for(int i = 0; i < m_EventLoading.EventLoadingCount; i++)
	{
		for( int j = 0 ; j < MAX_OBJ_VAR_CNT; ++j )
		{
			if(m_EventLoading.EventTable[i].FirstNO == j && m_EventLoading.EventTable[i].SecondNO == pObjVAR->m_nAI_VAR[ j ])
			{
				m_bswitchImage = true;
				m_mMapLoadingImageTableByEvent.push_back(m_EventLoading.EventTable[i].sFileName );
			}
		}
	}

	if(m_bswitchImage == true)
	{
		g_Loading.LoadTexture( m_mMapLoadingImageTableByEvent );
	}
	else
	{
		if (iPrevStateID == CGame::GS_SELECTAVATAR)
		{
			g_Loading.LoadTexture( );
		}
		else
		{
			g_Loading.LoadTexture( Reply.m_nZoneNO, ZONE_PLANET_NO( Reply.m_nZoneNO ) );
		}
	}		

	m_bswitchImage = false;
	return 0;
}

int	CGameStateMoveMain::Leave( int iNextStateID )
{
	CSystemProcScript::GetSingleton().CallLuaFunction( "LeaveMoveMain", ZZ_PARAM_END );	

	g_Loading.UnloadTexture();

	return 0;
}

void CGameStateMoveMain::CheckInterruptTime()
{
	if(InputInterruptTime < 4500)
	{
		InputInterruptTime +=  ::getTimeDiff(); //g_GameDATA.GetElapsedFrameTime();
		if(InputInterruptTime > 2000 && m_iMoveMainState == 0)
		{
			m_iMoveMainState = 1;
		}

	}
	else  
	{
		InputOnOff = true;
	}
}
