#include "stdafx.h"	
#include "CKeyMove.h"	
#include "../CCamera.h"	
#include "../OBJECT.h"	
#include "../Network\\CNetwork.h"
#include "../interface/it_mgr.h"
#include "../system/CGame.h"


CKeyMove		g_KeyMove;


const float DEFAULT_YAW_VALUE = 3.0f;


CKeyMove::CKeyMove()
{
	m_bLeftKeyDown		= false;
	m_bRightKeyDown		= false;

	m_bUpKeyDown		= false;
	m_bDownKeyDown		= false;

	m_fYawValue			= 0.0f;
}

CKeyMove::~CKeyMove()
{
}

#define CAMERA_MOVE_SPEED 10
void CKeyMove::Proc()
{	
	/// 제련중에는 커서키로 이동불가 2004.12.14
	if( g_itMGR.IsDlgOpened( DLG_TYPE_UPGRADE ) )
		return;

	///IME Candidate Window 가 떠 있을경우에는 무시한다.
	if( it_IsVisibleImeCandidateWindow() )
		return;

	m_bUpKeyDown =	 ( GetAsyncKeyState( VK_UP ) < 0 ) ?true:false;
	m_bDownKeyDown = ( GetAsyncKeyState( VK_DOWN ) < 0 )?true:false;
	m_bLeftKeyDown = ( GetAsyncKeyState( VK_LEFT ) < 0 )?true:false;
	m_bRightKeyDown = ( GetAsyncKeyState( VK_RIGHT ) < 0 )?true:false;

	bool bHeightUp = ( GetAsyncKeyState( VK_END ) < 0 )?true:false;
	bool bHeightDown = ( GetAsyncKeyState( VK_HOME ) < 0 )?true:false;
	
	if( m_bLeftKeyDown )
		g_pCamera->Add_YAW( CAMERA_MOVE_SPEED/2 );

	if( m_bRightKeyDown )
		g_pCamera->Add_YAW( -CAMERA_MOVE_SPEED/2 );

	/// 촬영 모드.
	if( g_GameDATA.m_bFilmingMode )
	{
		if( m_bUpKeyDown )
			g_pCamera->Add_Distance( CAMERA_MOVE_SPEED * 3 );

		if( m_bDownKeyDown )
			g_pCamera->Add_Distance( -CAMERA_MOVE_SPEED * 3 );

		if( bHeightUp )
			g_pAVATAR->m_fAltitude += 10.0f;
		if( bHeightDown )
			g_pAVATAR->m_fAltitude -= 10.0f;

	}else
	{
		if( m_bUpKeyDown || CGame::GetInstance().IsAutoRun() )
		{
			if( g_pAVATAR == NULL )
				return;

			static int sLastShiftGoFrame = 0;
			int CurrentShiftGoFrame = g_GameDATA.GetGameTime();

			if( CurrentShiftGoFrame - sLastShiftGoFrame > 200 )
			{				
				D3DVECTOR Dir = g_pAVATAR->Get_CurPOS() - g_pCamera->Get_Position();
				D3DXVECTOR3 vDir = D3DXVECTOR3( Dir.x, Dir.y, Dir.z );				
				D3DXVec3Normalize( &vDir, &vDir );
				vDir.z = 0.0f;

				vDir = vDir * 1500.0f;               
				Dir.x = vDir.x;
				Dir.y = vDir.y;
				Dir.z = vDir.z;

				D3DVECTOR vPos = g_pAVATAR->Get_CurPOS() + Dir;
				g_pNet->Send_cli_MOUSECMD( 0, vPos );

				///CGame::GetInstance().SetMouseTargetEffect( vPos.x, vPos.y, vPos.z );

				sLastShiftGoFrame = CurrentShiftGoFrame ;
			}
		}
	}
	
}