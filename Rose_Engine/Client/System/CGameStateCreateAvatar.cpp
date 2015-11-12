#include "stdafx.h"
#include "CGameStateCreateAvatar.h"
#include "CGame.h"
#include "SystemProcScript.h"

#include "../Interface/ExternalUI/ExternalUILobby.h"
#include "../interface/CUIMediator.h"
#include "../IO_Terrain.h"
#include "../CCamera.h"
#include "../CJustModelAVT.h"

#include "../GameData/CGameDataCreateAvatar.h"

#include "TIme2.h"
CGameStateCreateAvatar::CGameStateCreateAvatar( int iID )
{ 
	m_iStateID = iID; 
}

CGameStateCreateAvatar::~CGameStateCreateAvatar(void)
{

}

int	CGameStateCreateAvatar::Update( bool bLostFocus )
{
	g_EUILobby.Update();

	g_pCamera->Update ();

	D3DVECTOR PosENZIN = g_pCamera->Get_Position ();	
	g_pTerrain->SetCenterPosition( PosENZIN.x, PosENZIN.y );

	//조성현 2005 12 - 17  Display된 Node 알기.
	hCreateAvatarNode = CGameDataCreateAvatar::GetInstance().m_CreateAvatar;

	::updateScene ();

	// processing  ...
	if ( !bLostFocus ) 
	{
		if ( !::beginScene() ) //  디바이스가 손실된 상태라면 0을 리턴하므로, 모든 렌더링 스킵
		{
			return 0;
		}

		::clearScreen();
		::renderScene();

		::beginSprite( D3DXSPRITE_ALPHABLEND );

		/// Screen message display
		g_UIMed.Draw();

		/// UI display
		g_EUILobby.Draw();
		CTIme::GetInstance().Draw();

		//		g_GameDATA.DrawFontCameraPosition();   // 조성현 10 - 19

		::endSprite();		

		CCreateAvata* pCreateAvata = (CCreateAvata*)g_EUILobby.GetEUI( EUI_CREATE_AVATA );
		if(pCreateAvata)
		{

			::setAvatarViewPort((float)testRegion.left,
				(float)testRegion.top,
				(float)(testRegion.right - testRegion.left),
				(float)(testRegion.bottom - testRegion.top)); 
			//::setAvatarViewPort(400, 100, 300, 300);   //조성현 11 - 1 캐릭터 선택화면
			::RenderSelectedAvatar(hCreateAvatarNode);
			::setDefaultViewPort();
		}


		::endScene ();
		::swapBuffers();
	}
	return 0;
}

int CGameStateCreateAvatar::Enter( int iPrevStateID )
{
	CSystemProcScript::GetSingleton().CallLuaFunction( "EnterCreateAvatar", ZZ_PARAM_END );
	g_EUILobby.CreateCreateAvataDlg();

	//조성현 2005 12 - 17  Display된 Node 알기.
	hCreateAvatarNode = CGameDataCreateAvatar::GetInstance().m_CreateAvatar;
	//testRegion.left = 400; testRegion.right = 800;
	//testRegion.bottom = 400; testRegion.top = 100;

	CCreateAvata* pCreateAvata = (CCreateAvata*)g_EUILobby.GetEUI( EUI_CREATE_AVATA );
	if(pCreateAvata)
	{
		int marginValue = 100;

		testRegion.left = pCreateAvata->GetCreateAvatarPos().x - marginValue;
		testRegion.top = pCreateAvata->GetCreateAvatarPos().y - marginValue ;	
		testRegion.right = pCreateAvata->GetCreateAvatarPos().x+pCreateAvata->GetCreateAvatarSize().cx + marginValue;
		testRegion.bottom = pCreateAvata->GetCreateAvatarPos().y+pCreateAvata->GetCreateAvatarSize().cy + marginValue;
	}

	return 0;
}

int CGameStateCreateAvatar::Leave( int iNextStateID )
{
	CSystemProcScript::GetSingleton().CallLuaFunction( "LeaveCreateAvatar", ZZ_PARAM_END );
	
	return 0;
}

int CGameStateCreateAvatar::ProcMouseInput( UINT uiMsg, WPARAM wParam, LPARAM lParam )
{
	static bool startOnOff = false;
	short zDelta;

	switch( uiMsg)
	{
	case WM_MOUSEMOVE:

		int xx, yy;

		mouse_x = LOWORD(lParam);
		mouse_y = HIWORD(lParam);

		if(!startOnOff)
		{
			pre_mouse_x = mouse_x;
			pre_mouse_y = mouse_y;

			startOnOff = true;
		}

		xx = mouse_x - pre_mouse_x;
		yy = mouse_y - pre_mouse_y;

		pre_mouse_x = mouse_x;
		pre_mouse_y = mouse_y;


		if( (testRegion.left < mouse_x) && (testRegion.right > mouse_x) && (testRegion.top < mouse_y) && (testRegion.bottom > mouse_y))
		{
			if(wParam == 0x0001)
			{
				if((xx!=0) || (yy!=0))
				{

					if((xx*xx) < (yy*yy))
					{
						//::updateAvatarSelectionCameraHeight(-0.02f*yy);  
					}
					else
						::updateAvatarSelectionCameraSeta(-0.02f*xx);
				}
			}
			else if(wParam == 0x0002)
			{
				if(yy != 0)
				{
					//::updateAvatarSelectionCameraLength(0.05f*yy);
				}
			}


		}

		break;
	}

	if( g_EUILobby.MsgProc( uiMsg ,wParam, lParam) )
		return 1;



	switch( uiMsg )
	{
	case WM_MOUSEMOVE:

		int xx, yy;

		mouse_x = LOWORD(lParam);
		mouse_y = HIWORD(lParam);

		if(!startOnOff)
		{
			pre_mouse_x = mouse_x;
			pre_mouse_y = mouse_y;

			startOnOff = true;
		}

		xx = mouse_x - pre_mouse_x;
		yy = mouse_y - pre_mouse_y;

		pre_mouse_x = mouse_x;
		pre_mouse_y = mouse_y;


#if defined(_DEBUG) || defined(_D_RELEASE)

		if( g_GameDATA.m_bObserverCameraMode)
		{
			if((wParam & MK_RBUTTON))
			{
				if((wParam & MK_RBUTTON))
				{
					ObserverCameraTransform( (short)xx, (short)yy);
				}
			}
		}

#endif		
		break;


	case WM_MOUSEWHEEL:


#if defined(_DEBUG) || defined(_D_RELEASE)

		zDelta = GET_WHEEL_DELTA_WPARAM(wParam);

		if(g_GameDATA.m_bObserverCameraMode)
		{
			ObserverCameraZoomInOut( (float)-(zDelta) );
		}

#endif


		break;


	case WM_MBUTTONDOWN:

#if defined(_DEBUG) || defined(_D_RELEASE)
		g_GameDATA.m_bObserverCameraMode = !g_GameDATA.m_bObserverCameraMode;
		SetObserverCameraOnOff();
#endif
		break;

	}

	return 0;
}

int CGameStateCreateAvatar::ProcKeyboardInput( UINT uiMsg, WPARAM wParam, LPARAM lParam )
{
	if( g_EUILobby.MsgProc( uiMsg ,wParam, lParam) )
		return 1;

	return 0;
}

