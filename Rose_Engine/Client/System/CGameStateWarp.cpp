#include "stdafx.h"
#include "CGameStateWarp.h"
#include <process.h>
#include "../Object.h"
#include "../CCamera.h"
#include "../CClientStorage.h"
#include "../IO_Terrain.h"
#include "../CSkyDOME.h"
#include "../Network/CNetwork.h"
#include "../interface/ExternalUI/ExternalUILobby.h"
#include "../interface/CLoading.h"
#include "../interface/it_mgr.h"
#include "../interface/CUIMediator.h"
#include "../interface/Dlgs/CMinimapDlg.h"
#include "../Country.h"

#include "CGame.h"
#include <crtdbg.h>
CGameStateWarp::CGameStateWarp(int iID)
{
	m_iStateID = iID;
	m_bswitchImage = false;
}

CGameStateWarp::~CGameStateWarp(void)
{
	m_mMapLoadingImageTableByEvent.clear();
}

int	CGameStateWarp::Update( bool bLostFocus )
{
	Draw();
	DWORD ret = WaitForSingleObject( m_hThread, 100 );

	switch( ret )
	{
	case WAIT_OBJECT_0:
		CGame::GetInstance().ChangeState( CGame::GS_MAIN );

		break;
	case WAIT_TIMEOUT:
		break;
	case WAIT_FAILED:
		g_pCApp->SetExitGame();
		break;
	default:
		break;
	}
	return 0;
}

int CGameStateWarp::Enter( int iPrevStateID )
{
	pauseSpriteSFX( true );

	m_iPrevStateID = iPrevStateID;

	LogString( LOG_NORMAL, "FreeZone(%d)\n", g_pTerrain->GetZoneNO() );

	int iPrevZoneNum = g_pTerrain->GetZoneNO();
	int iNextZoneNum = 0;
	g_pTerrain->FreeZONE();

//	m_hThread = (HANDLE)_beginthreadex( NULL, 0, &ThreadFunc, NULL, CREATE_SUSPENDED, NULL );
////	SetThreadPriority( m_hThread,THREAD_PRIORITY_HIGHEST  );
//
//	if( m_hThread )
//	{
//		ResumeThread( m_hThread );
//	}
//	else///Thread ���� ���н� ���ξ����忡�� �ε��ϰ� State�� �ٲپ� �ش�.




	gsv_TELEPORT_REPLY Reply;
	CGame::GetInstance().GetLoadingData( Reply );

	iNextZoneNum = Reply.m_nZoneNO;


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
		g_Loading.LoadTexture( Reply.m_nZoneNO, ZONE_PLANET_NO( Reply.m_nZoneNO ) );
	}

	Draw();
	ThreadFunc(NULL);
	CGame::GetInstance().ChangeState( CGame::GS_MAIN );



	if( IsApplyNewVersion() && g_pTerrain )
	{
		//int iZoneNum = TELEPORT_ZONE(nWarpIDX);
		///�߿� �ʵ忡�� �������� ����� ��Ȱ���� �����Ǿ� �ִٸ�
		if( (0 == ZONE_IS_UNDERGROUND( iPrevZoneNum )) && ZONE_IS_UNDERGROUND(iNextZoneNum) && ZONE_REVIVE_ZONENO( iNextZoneNum ))
		{
			g_itMGR.AppendChatMsg( STR_CHANGED_STOREDTOWN_POS, IT_MGR::CHAT_TYPE_SYSTEM, D3DCOLOR_ARGB( 255, 206, 223, 136)  );
		}
		///��Ȱ������ ����Ÿ�� ���ǵ� �������� �ʵ�� �̵���
		else if( ZONE_IS_UNDERGROUND( iPrevZoneNum ) && ZONE_REVIVE_ZONENO( iPrevZoneNum ) && (0 == ZONE_IS_UNDERGROUND( iNextZoneNum )) )
		{
			g_itMGR.AppendChatMsg( STR_RECOVERD_STOREDTOWN_POS, IT_MGR::CHAT_TYPE_SYSTEM, D3DCOLOR_ARGB( 255, 206, 223, 136)   );
		}
	}




	m_bswitchImage = false;


	return 0;
}
int CGameStateWarp::Leave( int iNextStateID )
{
	g_Loading.UnloadTexture();
	gsv_TELEPORT_REPLY Reply;
	CGame::GetInstance().GetLoadingData( Reply );

	g_pAVATAR->Warp_POSITION( Reply.m_PosWARP.x, Reply.m_PosWARP.y );

	g_pCamera->Attach ( g_pAVATAR->GetZMODEL() );	
	g_pCamera->Update ();
	/// Loading mode change	
//	g_pTerrain->SetLoadingMode( true );
//	::setDelayedLoad( true );

	CSkyDOME::Init( g_GameDATA.m_hShader_sky, g_GameDATA.m_hLight, ZONE_BG_IMAGE( g_pTerrain->GetZoneNO() ) );

	/// PAT Setting
	g_pAVATAR->m_bRunMODE	= Reply.m_btRunMODE;
	g_pAVATAR->m_btMoveMODE = Reply.m_btRideMODE;


//-------------------------------------------------------------------------------------------------
//����ȣ::2�ν��϶� �ڽ��� īƮ�� �������� �ʵ��� �Ѵ�. 
#if defined(_GBC)	
	if( g_pAVATAR->m_btMoveMODE == MOVE_MODE_DRIVE)
	{
#else if 
	if( g_pAVATAR->m_btMoveMODE == MOVE_MODE_DRIVE || g_pAVATAR->m_btMoveMODE == MOVE_MODE_RIDEON )
	{
#endif 
//-------------------------------------------------------------------------------------------------
		g_pAVATAR->CreateCartFromMyData();
		g_pAVATAR->GetSkillSlot()->SetActiveSkillEnableByRideState( true );
	}
	else
	{
		g_pAVATAR->GetSkillSlot()->SetActiveSkillEnableByRideState( false );
	}

    pauseSpriteSFX( false );



/*
	///īƮ, ĳ����� �̿�Ұ� ������ īƮ, ĳ����� ž���ϰ� ����� ������ ������ �Ѵ�.
	if( IsApplyNewVersion() && g_pAVATAR->GetPetMode() >= 0 )
	{
		int iItemType = g_pAVATAR->m_Inventory.m_ItemRIDE[ 0 ].GetTYPE();
		int iItemNo   = g_pAVATAR->m_Inventory.m_ItemRIDE[ 0 ].GetItemNO();

		switch( ZONE_RIDING_REFUSE_FLAG( g_pTerrain->GetZoneNO() ) )
		{
		case 1:
			if( (ITEM_TYPE( iItemType, iItemNo ) / 100 == 5) && (ITEM_TYPE( iItemType, iItemNo ) % 10 == 1 ) )
			{
				g_pAVATAR->SetCMD_TOGGLE( Reply.m_btRideMODE + Reply.m_btRunMODE );				
				g_itMGR.AppendChatMsg( STR_CANT_USE_CART , IT_MGR::CHAT_TYPE_SYSTEM , D3DCOLOR_ARGB( 255, 206, 223, 136));				
				return false;
			}
			break;
		case 2:
			if( (ITEM_TYPE( iItemType, iItemNo ) / 100 == 5) && (ITEM_TYPE( iItemType, iItemNo ) % 10 == 2 ) )
			{
				g_pAVATAR->SetCMD_TOGGLE( Reply.m_btRideMODE + Reply.m_btRunMODE);
				g_itMGR.AppendChatMsg( STR_CANT_USE_CASTLEGEAR , IT_MGR::CHAT_TYPE_SYSTEM , D3DCOLOR_ARGB( 255, 206, 223, 136));				
				return false;
			}
			break;
		case 3:
			g_pAVATAR->SetCMD_TOGGLE( Reply.m_btRideMODE + Reply.m_btRunMODE);
			g_itMGR.AppendChatMsg(STR_CANT_USE_DRIVESKILL, IT_MGR::CHAT_TYPE_SYSTEM, D3DCOLOR_ARGB( 255, 206, 223, 136) );
			break;
		default:
			break;
		}

		//g_pAVATAR->m_btMoveMODE;
		
		g_pAVATAR->UpdateAbility();///�̵��ӵ� ����
	}
*/

	///īƮ, ĳ����� �̿�Ұ� ������ īƮ, ĳ����� ž���ϰ� ����� ������ ������ �Ѵ�.
	if( ZONE_RIDING_REFUSE_FLAG( g_pTerrain->GetZoneNO() ) > 0 )
	{
		//ȫ�� �̵��ӵ� ����� �ȵǾ ����������� �ӵ� ������ �ȵǾ���. �̵��Ұ����� ��� �ѹ��� ������༭ ������ �ذ�.
		g_pAVATAR->UpdateAbility();///�̵��ӵ� ����

		if( IsApplyNewVersion() &&
			g_pAVATAR->GetCur_MOVE_MODE() >= MOVE_MODE_DRIVE )
		{
			int iItemType = g_pAVATAR->m_Inventory.m_ItemRIDE[ 0 ].GetTYPE();
			int iItemNo   = g_pAVATAR->m_Inventory.m_ItemRIDE[ 0 ].GetItemNO();

			switch( ZONE_RIDING_REFUSE_FLAG( g_pTerrain->GetZoneNO() ) )
			{
			case 1:
				if( (ITEM_TYPE( iItemType, iItemNo ) / 100 == 5) && (ITEM_TYPE( iItemType, iItemNo ) % 10 == 1 ) )
				{
					g_pAVATAR->SetCMD_TOGGLE( Reply.m_btRideMODE + Reply.m_btRunMODE );				
					g_itMGR.AppendChatMsg( STR_CANT_USE_CART , IT_MGR::CHAT_TYPE_SYSTEM , D3DCOLOR_ARGB( 255, 206, 223, 136));				
					return false;
				}
				break;
			case 2:
				if( (ITEM_TYPE( iItemType, iItemNo ) / 100 == 5) && (ITEM_TYPE( iItemType, iItemNo ) % 10 == 2 ) )
				{
					g_pAVATAR->SetCMD_TOGGLE( Reply.m_btRideMODE + Reply.m_btRunMODE);
					g_itMGR.AppendChatMsg( STR_CANT_USE_CASTLEGEAR , IT_MGR::CHAT_TYPE_SYSTEM , D3DCOLOR_ARGB( 255, 206, 223, 136));				
					return false;
				}
				break;
			case 3:
				g_pAVATAR->SetCMD_TOGGLE( Reply.m_btRideMODE + Reply.m_btRunMODE);
				g_itMGR.AppendChatMsg(STR_CANT_USE_DRIVESKILL, IT_MGR::CHAT_TYPE_SYSTEM, D3DCOLOR_ARGB( 255, 206, 223, 136) );
				break;
			default:
				break;
			}

			g_pAVATAR->UpdateAbility();///�̵��ӵ� ����
		}
	}
	

	return 0;
}

void CGameStateWarp::Draw()///Test Code
{
	if( g_pCApp->IsActive() )
	{
		if ( !::beginScene() ) //  ����̽��� �սǵ� ���¶�� 0�� �����ϹǷ�, ��� ������ ��ŵ
		{
			return;
		}
		
		::clearScreen();

		::beginSprite( D3DXSPRITE_ALPHABLEND );	

		g_Loading.Draw();

		::endSprite();

		::endScene();
		::swapBuffers();
	}
}

unsigned __stdcall CGameStateWarp::ThreadFunc( void* pArguments )
{
	gsv_TELEPORT_REPLY Reply;
	CGame::GetInstance().GetLoadingData( Reply );

//	::setDelayedLoad( 2 );
	
	::setDelayedLoad( 0 );

	LogString( LOG_NORMAL, "LoadZone(%d)\n", Reply.m_nZoneNO  );
	g_pTerrain->LoadZONE( Reply.m_nZoneNO );
	g_pTerrain->InitZONE( Reply.m_PosWARP.x, Reply.m_PosWARP.y );

	/// ���λ��� ����Ʈ Ŭ����
	g_UIMed.ResetPersonalStore();

	CMinimapDLG* pDlg = g_itMGR.GetMinimapDLG();
	
	if( pDlg )
		pDlg->SetMinimap( ZONE_MINIMAP_NAME( Reply.m_nZoneNO ), "3DData\\Control\\Res\\MiniMap_Arrow.tga", 
							ZONE_MINIMAP_STARTX( Reply.m_nZoneNO ), ZONE_MINIMAP_STARTY( Reply.m_nZoneNO ) );

	::setDelayedLoad( 2 );
	::setDelayedLoad( 1 );

	return 0;
}