#include "stdafx.h"
#include "cgamestatelogin.h"
#include "CGame.h"

#include "../Game.h"
#include "../TGameCtrl/TMsgBox.h"
#include "../Network/CNetwork.h"

#include "../Interface/ExternalUI/ExternalUILobby.h"
#include "../Interface/CTFontImpl.h"
#include "../Interface/IO_ImageRes.h"
#include "../interface/CUIMediator.h"
#include "../Interface/IT_MGR.H"
#include "../Interface/Command/CTCmdHotExec.h"
#include "../Interface/Cursor/CCursor.h"

#include "../Common/IO_Quest.h"

#include "../IO_Terrain.h"
#include "../CCamera.h"
#include "../CSkyDOME.H"
#include "../Misc/GameUtil.h"
#include "TIme2.h"
#include "SystemProcScript.h"

//조성현 2005 12 - 12 Test 카메라...
HNODE hCameraMotion[6];
HNODE hCameraNode;
int iCurrentMotionCameraIndex;

CGameStateLogin::CGameStateLogin(int iID)
{
	m_iStateID = iID;
	LoginState = 0;
}

CGameStateLogin::~CGameStateLogin(void)
{	
}


int	CGameStateLogin::Update( bool bLostFocus )
{
	CheckInterruptTime();

	if(LoginState == 3)   //조성현 2005 12 - 19 LoginState에 따라..
		g_EUILobby.Update();

	g_pCamera->Update ();
    D3DVECTOR PosENZIN = g_pCamera->Get_Position ();	
	g_pTerrain->SetCenterPosition( PosENZIN.x, PosENZIN.y );
	g_DayNNightProc.ShowAllEffect( false );
	
	
	if(LoginState > 1)
	{
		
		//조성현 2005 12 - 14 카메라
		CheckMotionCamera();
		LoopFadeInOut();
	}

	::updateScene ();

	// processing  ...
	if ( !bLostFocus ) 
	{
		if ( !::beginScene() ) //  디바이스가 손실된 상태라면 0을 리턴하므로, 모든 렌더링 스킵
		{
			return 0;
		}

		::clearScreen();

		if(LoginState > 1)
			::renderScene();	
			


		::beginSprite( D3DXSPRITE_ALPHABLEND );

		if(LoginState < 2)
		{
			DrawLOGO();
		}
		else if(LoginState == 3)  //조성현 2005 12 - 19 LoginState에 따라..
		{
//			 ::SetFogOffMode(true);
			/// Screen message display
			g_UIMed.Draw();
			/// UI display
			g_EUILobby.Draw();
		}


		CTIme::GetInstance().Draw();

//		g_GameDATA.DrawFontCameraPosition();   // 조성현 10 - 19


		::endSprite();		


		::endScene ();



		::swapBuffers();
	}

	return 0;
}

int CGameStateLogin::ProcMouseInput( UINT uiMsg, WPARAM wParam, LPARAM lParam )
{
	//test 10 - 17 조성현

	if(!InputOnOff)
		return 0;



#if defined( _DEBUG ) || defined( _D_RELEASE)

	if( (uiMsg == WM_MOUSEMOVE) && (g_GameDATA.m_bObserverCameraMode))
	{
		static bool InitOnOff = false;
		static int pre_mouse_x, pre_mouse_y;

		int mouse_x, mouse_y;
		int xx, yy;

		mouse_x=LOWORD(lParam);
		mouse_y=HIWORD(lParam);

		if(!InitOnOff)
		{
			pre_mouse_x = mouse_x;
			pre_mouse_y = mouse_y;

			InitOnOff = true;
		}

		xx = mouse_x - pre_mouse_x;
		yy = mouse_y - pre_mouse_y;

		if ( ( wParam & MK_RBUTTON ) ) 
		{
			ObserverCameraTransform( (short)xx, (short)yy);

			pre_mouse_x = mouse_x;
			pre_mouse_y = mouse_y; 					
		}


	}



	if( (uiMsg == WM_MOUSEWHEEL) )
	{
		short zDelta = GET_WHEEL_DELTA_WPARAM(wParam);

		if(g_GameDATA.m_bObserverCameraMode)
		{
			ObserverCameraZoomInOut( (float)-(zDelta) );
		}

	}

	if( (uiMsg == WM_MBUTTONDOWN) )
	{
#if defined(_DEBUG) || defined(_D_RELEASE)
		g_GameDATA.m_bObserverCameraMode = !g_GameDATA.m_bObserverCameraMode;
		SetObserverCameraOnOff();
#endif
	}

#endif

	if( (uiMsg == WM_LBUTTONDOWN) )
	{
		if( LoginState == 2 )
		{
			LoginState = 3;//조성현 2005 12 - 19 LoginState에 따라..
			return 1;
		}	
	
	}

	if( (uiMsg == WM_RBUTTONDOWN) )
	{
		if( LoginState == 2 )
		{
			LoginState = 3;//조성현 2005 12 - 19 LoginState에 따라..
			return 1;
		}	
   
	}	


	if(LoginState == 3 )	//조성현 2005 12 - 19 LoginState에 따라..
	{
		if( g_EUILobby.MsgProc( uiMsg ,wParam, lParam) )
			return 1;
	}

	return 0;
}


int CGameStateLogin::ProcKeyboardInput( UINT uiMsg, WPARAM wParam, LPARAM lParam )
{
	if(!InputOnOff)
		return 0;

	switch( uiMsg )
	{
	case WM_CHAR :
		{
			switch ( wParam )
			{
			case '1':
				{
#ifdef __TEST
#ifdef __AUTOLOGIN
				if( g_GameDATA.m_dwSeq && g_GameDATA.m_szSessionKey )
				{					
					CTDialog* pDlg = g_EUILobby.GetEUI( EUI_LOGIN ) ;
					assert( pDlg );
					if( pDlg )
					{
						CLogin*	pLoginDlg = (CLogin*)pDlg;
						pLoginDlg->ConnectLoginServer();
					}
					
				}
#endif
#endif
				}
				break;
			case '2':
				break;
			}
		}		

		break;



	}

	if(LoginState == 3)  //조성현 2005 12 - 19 LoginState에 따라..
	{
		if( g_EUILobby.MsgProc( uiMsg ,wParam, lParam) )
			return 1;	
	}

	if( uiMsg == 256)
	{ 
		if( LoginState == 2 && (wParam == 0x0000000d) )
		{
			LoginState = 3;//조성현 2005 12 - 19 LoginState에 따라..
			return 1;
		}	
	}

	return 0;
}

int CGameStateLogin::Enter( int iPrevStateID )
{
	CSystemProcScript::GetSingleton().CallLuaFunction( "EnterLogin", ZZ_PARAM_INT, (int)g_GameDATA.GetTime(),ZZ_PARAM_END );

	//조성현 2005 12 - 14 카메라..
	//------------------------------------------------------------------------------------------------------------------	
	hCameraMotion[0] = ::loadMotion("LoginCameraMotion1", "3DData\\Title\\Camera01_intro01.zmo", 1, 4, 3, 1, 0); 
	hCameraMotion[1] = ::loadMotion("LoginCameraMotion2", "3DData\\Title\\Camera01_intro02.zmo", 1, 4, 3, 1, 0); 
	hCameraMotion[2] = ::loadMotion("LoginCameraMotion3", "3DData\\Title\\Camera01_intro03.zmo", 1, 4, 3, 1, 0); 
	hCameraMotion[3] = ::loadMotion("LoginCameraMotion4", "3DData\\Title\\Camera01_intro04.zmo", 1, 4, 3, 1, 0); 
	hCameraMotion[4] = ::loadMotion("LoginCameraMotion5", "3DData\\Title\\Camera01_intro05.zmo", 1, 4, 3, 1, 0); 
	hCameraMotion[5] = ::loadMotion("LoginCameraMotion6", "3DData\\Title\\Camera01_login01.zmo", 1, 4, 3, 1, 0); 
	//-------------------------------------------------------------------------------------------------------------------- 


	g_EUILobby.InitEUIManager();
	g_EUILobby.CreateLoginDlg();

	//	CTCommand* pCmd = new CTCmdExit;
	//	g_pTerrain->SetLoadingMode( false );
	//	::setDelayedLoad( 2 );	


	int iOldSightRange = g_GameDATA.m_nSightRange;
	g_GameDATA.m_nSightRange = 50;

	g_pCamera->Update ();
	D3DVECTOR PosENZIN = g_pCamera->Get_Position ();	
	g_pTerrain->InitZONEALL( PosENZIN.x, PosENZIN.y );
	


	/// Init skybox
	CSkyDOME::Init( g_GameDATA.m_hShader_sky, g_GameDATA.m_hLight, 0 );


	//::setDelayedLoad( 1 );

	g_GameDATA.m_nSightRange = iOldSightRange;

	if( g_GameDATA.m_is_NHN_JAPAN )
	{
		CTDialog* pDlg =  g_EUILobby.GetEUI( EUI_LOGIN );
		CLogin* pLoginDlg = ( CLogin*)pDlg;
		pLoginDlg->SetID( g_GameDATA.m_Account.Get() );
		///Decoding후 넣는다.
		//pLoginDlg->SetPassword( g_GameDATA.m_Password.Get() );
		pLoginDlg->ConnectLoginServer();
		pLoginDlg->Hide();
	}
	/// exec Enter script

#ifdef __AUTOLOGIN	
	if( g_GameDATA.m_dwSeq && g_GameDATA.m_szSessionKey )
	{
		CTDialog* pDlg = g_EUILobby.GetEUI( EUI_LOGIN ) ;
		assert( pDlg );
		if( pDlg )
		{
			pDlg->Hide();			
		}
	}
	
#endif

	InputInterruptTime = 0;
	InputOnOff = false;
    ::setAlphaFogRange( 15000 * 1.5f, 20000 * 1.5f );
    
	m_hTitleTexture[0] = loadTexture ( "3DData\\Control\\Res\\ExUI1.dds", "3DData\\Control\\Res\\ExUI1.dds", 1,	0 );
    m_hTitleTexture[1] = loadTexture ( "3DData\\Control\\Res\\Roselogo.dds", "3DData\\Control\\Res\\Roselogo.dds", 1,	0 );
   	
	hCameraNode = ::findNode("motion_camera");
	::attachMotion(hCameraNode, hCameraMotion[0]);
	::setRepeatCount(hCameraNode, 0);
	::controlAnimatable(hCameraNode, 0);
	
	if (g_GameDATA.m_SkipIntroLogo) {
		LoginState = 3;
		InputOnOff = true;
	} else {
		LoginState = 0;
	}

	return 0;
}

int CGameStateLogin::Leave( int iNextStateID )
{
	/// exec Leave script
	CSystemProcScript::GetSingleton().CallLuaFunction( "LeaveLogin", ZZ_PARAM_END );

	//조성현 2005 12- 12 Test

	::attachMotion(hCameraNode, 0);

	for(int ii = 0; ii < 6; ii += 1)
	{
		::unloadMotion(hCameraMotion[ii]);
	}
 
	for(int ii = 0; ii < 2; ii += 1)
	{
		if( m_hTitleTexture[ii] )
		{
			unloadTexture( m_hTitleTexture[ii] );
			m_hTitleTexture[ii] = NULL;
		}
	}

	//m_npKeyCrypt.CloseHandle();
	return 0;
}

void CGameStateLogin::ServerDisconnected()
{
	//g_EUILobby.ServerDisconnected();
}

void CGameStateLogin::AcceptedConnectLoginSvr()
{
	CGame::GetInstance().ProcWndMsg(WM_USER_CLOSE_MSGBOX,0,0 ); 
	///접속되었다 ID와 패스워드를 날리자.

	CTDialog* pDlg = g_EUILobby.GetEUI( EUI_LOGIN ) ;
	assert( pDlg );
	if( pDlg )
	{
		CLogin*	pLoginDlg = (CLogin*)pDlg;
		pLoginDlg->SendLoginReq();
	}
}

void CGameStateLogin::CheckMotionCamera()
{
	static bool StartOnOff = false;

	if(LoginState == 2)
	{
		if(!::getAnimatableState(hCameraNode))
		{
			if(iCurrentMotionCameraIndex < 4)
			{
				iCurrentMotionCameraIndex += 1;
			}
			else
				iCurrentMotionCameraIndex = 0;

			::attachMotion(hCameraNode, hCameraMotion[iCurrentMotionCameraIndex]);
			::setRepeatCount(hCameraNode, 1);
			::controlAnimatable(hCameraNode, 1);

		}
	}
	else if( LoginState == 3)
	{
		if(!StartOnOff)
		{
			::attachMotion(hCameraNode, hCameraMotion[5]);
			::setRepeatCount(hCameraNode, 0);
			::controlAnimatable(hCameraNode, 1);
			StartOnOff = true;
		}

	}
}

void CGameStateLogin::CheckInterruptTime()
{
	
	if( LoginState < 2)
	{
		InputInterruptTime +=  ::getTimeDiff(); 
	
		if(LoginState == 0)
		{
			if(InputInterruptTime < 1000)
				 ::ScreenFadeInStart( 1.5f, 1.0f, 1.5f, 0, 0, 0);
			else if( InputInterruptTime > 1500)
			{
				LoginState = 1;
                InputInterruptTime = 0;
			}
		}
		else if(LoginState == 1)
		{
			
			if(InputInterruptTime > 2000  && InputInterruptTime < 4000)
			{
				 ::ScreenFadeInStart( 1.5f, 0.5f, 1.5f, 0, 0, 0);
			}
			else if(InputInterruptTime > 3000)
			{
			    LoginState = 2;
                InputInterruptTime = 0;
				hCameraNode = ::findNode("motion_camera");
				::controlAnimatable(hCameraNode, 1);
				::setRepeatCount(hCameraNode, 1);
				::setCameraDefault(hCameraNode);
				iCurrentMotionCameraIndex = 0;
			}
		
		}
	}
	else if( LoginState == 2)
	{	
		if(InputInterruptTime < 1000)
		{
			InputInterruptTime +=  ::getTimeDiff(); //g_GameDATA.GetElapsedFrameTime();
		}
		else
		{
			InputOnOff = true;
		}
	}

	else if( LoginState == 3)
	{
#ifdef __AUTOLOGIN

		if( g_GameDATA.m_dwSeq && g_GameDATA.m_szSessionKey )
		{
			static bool OneTimeProc = true;
			if(OneTimeProc)
			{	
				OneTimeProc = false;
				CTDialog* pDlg = g_EUILobby.GetEUI( EUI_LOGIN ) ;
				assert( pDlg );
				if( pDlg )
				{
					CLogin*	pLoginDlg = (CLogin*)pDlg;
					pLoginDlg->ConnectLoginServer();
				}
			}
		}

#endif
	}
	
}

void CGameStateLogin::LoopFadeInOut()
{
	if(LoginState == 2)
	{
		float totalTime, currentTime;
		::getAnimatableMotionTime( hCameraNode, &currentTime, &totalTime);

		if(totalTime > 0.0f)
		{
			if( (totalTime - currentTime) < 1.5f)
			{
				::ScreenFadeInStart( 1.5f, 0.0f, 1.5f, 255, 255, 255);

			}

		}

	}

}


void CGameStateLogin::DrawLOGO()
{
	D3DXMATRIX mat, matScale, matTrans;	
	int iWidth, iHeight;


	::getTextureSize( m_hTitleTexture[LoginState], iWidth, iHeight );
	D3DXVECTOR3 vCenter( (float)(iWidth/2), (float)(iHeight/2), 0);

	float fScaleWidth  = (float)(g_pCApp->GetWIDTH() / 1024.);
	float fScaleHeight = (float)(g_pCApp->GetHEIGHT() / 768.);

	if(LoginState == 0)
		D3DXMatrixScaling( &matScale, fScaleWidth, fScaleHeight, 0.0f );
	else
		D3DXMatrixScaling( &matScale, 2.0f * fScaleWidth,  2.0f * fScaleHeight, 0.0f );
	D3DXMatrixTranslation( &matTrans, (float)(g_pCApp->GetWIDTH()/2), (float)(g_pCApp->GetHEIGHT()/2), 0 );
	D3DXMatrixMultiply( &mat, &matScale, &matTrans );
		
	::setTransformSprite( mat );
	::drawSprite( m_hTitleTexture[LoginState], NULL, &vCenter, NULL, D3DCOLOR_RGBA( 255, 255, 255, 255 ) );			
	
}
