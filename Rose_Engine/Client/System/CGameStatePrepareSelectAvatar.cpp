#include "stdafx.h"
#include "CGameStatePrepareSelectAvatar.h"
#include <process.h>
#include "CGame.h"
#include "SystemProcScript.h"
#include "../IO_Terrain.h"
#include "../CCamera.h"
#include "../CSkyDOME.H"
#include "../Interface/ExternalUI/ExternalUILobby.h"
#include "../Interface/CLoading.h"
#include "../Interface/CUIMediator.h"
#include "../../TGameCtrl/TMsgBox.h"
CGameStatePrepareSelectAvatar::CGameStatePrepareSelectAvatar( int iID )
{ 
	m_iStateID = iID; 
}
CGameStatePrepareSelectAvatar::~CGameStatePrepareSelectAvatar(void)
{
}
int	CGameStatePrepareSelectAvatar::Update( bool bLostFocus )
{
	/*Draw();	
	DWORD ret = WaitForSingleObject( m_hThread, 100 );

	switch( ret )
	{
	case WAIT_OBJECT_0:
		CGame::GetInstance().ChangeState( CGame::GS_SELECTAVATAR );
		break;
	case WAIT_TIMEOUT:
		break;
	case WAIT_FAILED:
		g_pCApp->SetExitGame();
		break;
	default:
		break;
	}*/
	return 0;
}

int CGameStatePrepareSelectAvatar::Enter( int iPrevStateID )
{
	CSystemProcScript::GetSingleton().CallLuaFunction( "EnterLoadSelectAvatar", ZZ_PARAM_END );

	//m_hThread = (HANDLE)_beginthreadex( NULL, 0, &ThreadFunc, NULL, CREATE_SUSPENDED, NULL );
//	SetThreadPriority( m_hThread,THREAD_PRIORITY_HIGHEST  );

	//if( m_hThread )
	//{
	//	ResumeThread( m_hThread );
	//}
	//else///Thread 생성 실패시 메인쓰레드에서 로딩하고 State를 바꾸어 준다.
	{
		Draw();
		ThreadFunc(NULL);
		CGame::GetInstance().ChangeState( CGame::GS_SELECTAVATAR );
	}
	return 0;
}

int CGameStatePrepareSelectAvatar::Leave( int iNextStateID )
{
	g_EUILobby.HideMsgBox();
	::setDeltaTime( 0 ); // 데이터 로딩 때문에, 프레임간 시간차가 너무 많이 나서, 카메라 애니메이션이 종료될 수 있다. 따라서, 프레임간 시간차를 0으로 함.
	CSystemProcScript::GetSingleton().CallLuaFunction( "LeaveLoadSelectAvatar", ZZ_PARAM_END ); //test 10 - 18 조성현...
	return 0;
}

unsigned __stdcall CGameStatePrepareSelectAvatar::ThreadFunc( void* pArguments )
{
//	::setDelayedLoad( 0 );
//	::setDelayedLoad( 1 );
	CGame::GetInstance().Load_BasicDATA2();
	return 0;
}

void CGameStatePrepareSelectAvatar::Draw()
{
	//bool bLostFocus = g_pCApp->GetMessage ();

//	g_EUILobby.Update();
	
//	g_pCamera->Update ();

//	D3DVECTOR PosENZIN = g_pCamera->Get_Position ();	
//	g_pTerrain->SetCenterPosition( PosENZIN.x, PosENZIN.y );

	::updateScene ();

	// processing  ...
	//if ( !bLostFocus ) 
	
	if( g_pCApp->IsActive() )
	{
		if ( !::beginScene() )
		{
			return;
		}
		::clearScreen();
		::renderScene(); 
 		
	
		::beginSprite( D3DXSPRITE_ALPHABLEND );


		g_UIMed.Draw();
//		g_GameDATA.DrawFontCameraPosition();   // 조성현 10 - 19  카메라 위치 정보..
		/// UI display
//		g_EUILobby.Draw();

		::endSprite();		
	

		::endScene ();
		::swapBuffers();
	}

/*

	g_EUILobby.Update();

	if( !bLostFocus )
	{
		if ( !::beginScene() )
		{
			return;
		}
		::clearScreen();

		::beginSprite( D3DXSPRITE_ALPHABLEND );	

//		g_Loading.DrawBackground();

		::endSprite();

		::endScene();
		::swapBuffers();
	}		*/
}