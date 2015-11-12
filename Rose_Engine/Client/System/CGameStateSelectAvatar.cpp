#include "stdafx.h"
#include "CGameStateSelectAvatar.h"
#include "CGame.h"
#include "SystemProcScript.h"
#include "../Interface/ExternalUI/ExternalUILobby.h"
#include "../Interface/IO_ImageRes.h"
#include "../interface/CUIMediator.h"
#include "../interface/ExternalUI/CSelectAvata.h"
#include "../CClientStorage.h"
#include "../IO_Terrain.h"
#include "../CCamera.h"
#include "../CSkyDOME.H"
#include "../GameData/CGameDataCreateAvatar.h"
#include "../CJustModelAVT.h"
#include "../country.h"


//조성현 2005 12 - 12 Test 카메라...
int currentIndex = 5;
bool zoomInOnOff = false; 


CGameStateSelectAvatar::CGameStateSelectAvatar(int iID)
{
	m_iStateID = iID;

	InitStateSelectAvatarElement();

	for(int ii = 0; ii < 5; ii += 1)
	{
		for(int jj = 0; jj < 6; jj += 1)
		{
			m_SelectAvatarBox[ii].MotionIDX[jj] = 360 + (6*ii) + jj;
		}
	}

	TestElement[0] = 0.3f;
	TestElement[1] = 0.9f;
	TestElement[2] = 0.15f;
}

CGameStateSelectAvatar::~CGameStateSelectAvatar(void)
{
}

int	CGameStateSelectAvatar::Update( bool bLostFocus )
{
	
	g_EUILobby.Update();

	g_pCamera->Update ();

	D3DVECTOR PosENZIN = g_pCamera->Get_Position ();	
	g_pTerrain->SetCenterPosition( PosENZIN.x, PosENZIN.y );
 
	//	CGameDataCreateAvatar::GetInstance().InputSceneAvatar();

	Pick_POSITION(); //조성현 2005 12 - 14 //캐릭터 테스트...
	::updateScene ();


	RevisionCulledModel(); //조성현 Culling 된 모델의 애니메이션 재생..

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
		///카메라 모션이 멈춰있을경우에만 보이게 한다.
		if( getAnimatableState( g_pCamera->GetZHANDLE() ) == 0 )
		{
			g_EUILobby.Draw();
			D3DVECTOR vec = g_pCamera->Get_Position ();
		}

		CGameDataCreateAvatar::GetInstance().Update(); //조성현 2005 11 - 21 위로 올림..
		//		g_GameDATA.DrawFontCameraPosition(); // 조성현 10 - 19
		
	
		
		::endSprite();		


		::endScene ();
		::swapBuffers();
	} 

	return 0;
}

int CGameStateSelectAvatar::Enter( int iPrevStateID )
{
	CGameDataCreateAvatar::GetInstance().ClearSelectedAvatar();
	CSystemProcScript::GetSingleton().CallLuaFunction( "EnterSelectAvatar", ZZ_PARAM_END );

	g_pSoundLIST->SetVolume( -10000 );
	g_EUILobby.CreateAvataListDlg();

	//박지호::아로아 데이터 초기화 
	if(IsApplyNewVersion())
	{
		goddessMgr.Release();
	} 

	//조성현 2005 12- 14;
	ReConstructSelectAvatar(iPrevStateID);

	m_iPrevStateID = iPrevStateID;

	m_bMotionblendOption = ::GetMotionInterpolation();
	::useMotionInterpolation(false);
    ::setAlphaFogRange( 15000 * 1.5f, 20000 * 1.5f );

	m_iCurrentPickingMotionIndex = -1;

	return 0;
}

int CGameStateSelectAvatar::Leave( int iNextStateID )
{
	if(iNextStateID == 9)
		OnLeaveSelectAvatar(); //조성현 2005 12 - 19 
	else if(iNextStateID == 8)
	{
		ClearSelectedAvatar();
		SaveCurrentSelectAvatar(); 
	}

	CSystemProcScript::GetSingleton().CallLuaFunction( "LeaveSelectAvatar", ZZ_PARAM_END );

	::useMotionInterpolation(m_bMotionblendOption);

	return 0;
}


int CGameStateSelectAvatar::ProcMouseInput( UINT uiMsg, WPARAM wParam, LPARAM lParam )
{
	if( getAnimatableState( g_pCamera->GetZHANDLE() ) != 0 )
		return 0;

	if( g_EUILobby.MsgProc( uiMsg ,wParam, lParam) )
		return 1;
	/// 아바타 모델에 대한 마우스 오버및 LButtonDown처리

	static bool startOnOff = false;

	POINT ptMouse = { LOWORD( lParam ), HIWORD( lParam ) };;

	m_ScreenPOS.m_nX = (short)ptMouse.x;
	m_ScreenPOS.m_nY = (short)ptMouse.y;

	short zDelta = GET_WHEEL_DELTA_WPARAM(wParam);

	switch( uiMsg )
	{
	case WM_LBUTTONDOWN:
		{
			PickUp_POSITION();

			const char* pszName =  PickingSelectAvatar();  //조성현 2005 12 - 17 /* CGameDataCreateAvatar::GetInstance().Pick_AVATAR( LOWORD(lParam), HIWORD(lParam) )*/;

			if( pszName )
			{
				const char* pszSelectedName = CGameDataCreateAvatar::GetInstance().GetSelectedAvatarName();

				///이전 선택되어진 아바타와 같은 아바타라면 무시
				if( pszSelectedName && strcmp( pszName, pszSelectedName ) == 0 )
					break;

				CJustModelAVT* pPrevAvt = CGameDataCreateAvatar::GetInstance().FindAvatar( pszSelectedName );
				CJustModelAVT* pNowAvt = CGameDataCreateAvatar::GetInstance().FindAvatar( pszName );
				int iPrevAvtDeleteState = 0;
				if( pPrevAvt && pPrevAvt->GetDeleteRemainSEC() )
					iPrevAvtDeleteState = 1;

				int iSex = 0;
				if( pNowAvt && pNowAvt->IsFemale() )
					iSex = 1;

				OnSelectAvatar(pPrevAvt, pNowAvt);


				CGameDataCreateAvatar::GetInstance().SelectAvatar( pszName );

			}
			break;
		}
	case WM_LBUTTONDBLCLK:
		{
			const char* pszName = PickingSelectAvatar();/*CGameDataCreateAvatar::GetInstance().Pick_AVATAR( LOWORD(lParam), HIWORD(lParam) );*/
			if( pszName )
			{
				CGameDataCreateAvatar::GetInstance().SelectAvatar( pszName );
				CSelectAvata* pDlg = (CSelectAvata*)g_EUILobby.GetEUI(EUI_SELECT_AVATA);
				pDlg->SendSelectAvataReq();
			}
			break;
		}
		break;

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
		
#if defined(_DEBUG ) || defined(_D_RELEASE)   

		if( g_GameDATA.m_bObserverCameraMode)
		{
			if((wParam & MK_RBUTTON))
			{
				ObserverCameraTransform( (short)xx, (short)yy);
			}
		}
#endif
		break;

	case WM_MOUSEWHEEL:


#if defined(_DEBUG ) || defined(_D_RELEASE)   

		if(g_GameDATA.m_bObserverCameraMode)
		{
			ObserverCameraZoomInOut( (float)-(zDelta) );
		}

#endif 
		break;

	default:
		break;
	}
	return 0;
}


int CGameStateSelectAvatar::ProcKeyboardInput( UINT uiMsg, WPARAM wParam, LPARAM lParam )
{
	switch(uiMsg)
	{
		case WM_KEYDOWN :
	
			switch ( wParam ) 
			{
			case 'Q':

				if(TestElement[0]+0.02f < 1.0f)
					TestElement[0] += 0.02f;
				else
					TestElement[0] = 1.0f;

				break;

			case 'A':

				
				if(TestElement[0]-0.02f > 0.0f)
					TestElement[0] -= 0.02f;
				else
					TestElement[0] = 0.0f;
				
				break;


			case 'W':

				if(TestElement[1]+0.02f < 1.0f)
					TestElement[1] += 0.02f;
				else
					TestElement[1] = 1.0f;

				break;


			case 'S':
					
				if(TestElement[1]-0.02f > 0.0f)
					TestElement[1] -= 0.02f;
				else
					TestElement[1] = 0.0f;


				break;


			case 'E':

				if(TestElement[2]+0.02f < 1.0f)
					TestElement[2] += 0.02f;
				else
					TestElement[2] = 1.0f;
				
				
				break;

			case 'D':
	
				if(TestElement[2]-0.02f > 0.0f)
					TestElement[2] -= 0.02f;
				else
					TestElement[2] = 0.0f;
				
				
				break;


			}

			break;
    }
	
	if( getAnimatableState( g_pCamera->GetZHANDLE() ) != 0 )
		return 0;

	if( g_EUILobby.MsgProc( uiMsg ,wParam, lParam) )
		return 1;

	return 0;
}

void CGameStateSelectAvatar::ServerDisconnected()
{
	g_EUILobby.ServerDisconnected();
}

//조성현 2005 12 - 14 캐릭터 선택화면...추가..


void CGameStateSelectAvatar::Pick_POSITION ()
{
	D3DXVECTOR3 RayOrig;
	D3DXVECTOR3 RayDir;

	::getRay (m_ScreenPOS.m_nX, m_ScreenPOS.m_nY, &RayOrig.x, &RayOrig.y,	&RayOrig.z, &RayDir.x, &RayDir.y,	&RayDir.z );
	//	::getRay (CGame::GetInstance().Get_XPos(),CGame::GetInstance().Get_YPos(), &RayOrig.x, &RayOrig.y,	&RayOrig.z, &RayDir.x, &RayDir.y,	&RayDir.z );

	CGame::GetInstance().SetRayDir( RayDir );
	CGame::GetInstance().SetRayOrig( RayOrig );

	float Origin[3], Direction[3];

	Origin[0] = RayOrig.x; Origin[1] = RayOrig.y; Origin[2] = RayOrig.z;
	Direction[0] = RayDir.x; Direction[1] = RayDir.y; Direction[2] = RayDir.z;

	CJustModelAVT* pAvt;

	for(int ii = 0; ii < 5; ii += 1 )
	{
		if(::intersectRay2OBB(Origin, Direction, m_SelectAvatarBox[ii].CenterPosition, m_SelectAvatarBox[ii].Rotation,m_SelectAvatarBox[ii].xMinMax[1], m_SelectAvatarBox[ii].yMinMax[1], m_SelectAvatarBox[ii].zMinMaxEx))
		{
			if(m_SelectAvatarBox[ii].hNode != NULL)
			{
				pAvt = CGameDataCreateAvatar::GetInstance().FindAvatarCount(ii);
				assert(pAvt);

				if(pAvt->m_iCurrentMotionState == 1 && (m_iCurrentPickingMotionIndex != ii))
				{
					pAvt->m_iCurrentMotionState = 2;
					pAvt->SetMotion(pAvt->m_iMotionIDX[2], 1);

				}

			}
			m_iCurrentPickingMotionIndex = ii;
			break;

		}
	}

	if(!::IsForcedMovingCameraState())
	{
		if(zoomInOnOff)
		{
			if(!::intersectRay2OBB(Origin, Direction, m_MovingCameraZoomInBox.CenterPosition, m_MovingCameraZoomInBox.Rotation,m_MovingCameraZoomInBox.xMinMax[1], m_MovingCameraZoomInBox.yMinMax[1], m_MovingCameraZoomInBox.zMinMaxEx))
			{
				::InputForcedMovingCameraElement(m_MovingCameraState[5].Position, m_MovingCameraState[5].Rotation, 1.0f);
               	
				//::InputForcedMovingCameraSystemElement(m_MovingCameraState[5].Position, m_MovingCameraState[5].Rotation, TestElement, 1.0f);

			    currentIndex = 5;
				zoomInOnOff = false;
			}

		}
	}

}


void CGameStateSelectAvatar::PickUp_POSITION ()
{
	D3DXVECTOR3 RayOrig;
	D3DXVECTOR3 RayDir;

	::getRay (m_ScreenPOS.m_nX, m_ScreenPOS.m_nY, &RayOrig.x, &RayOrig.y,	&RayOrig.z, &RayDir.x, &RayDir.y,	&RayDir.z );
	//	::getRay (CGame::GetInstance().Get_XPos(),CGame::GetInstance().Get_YPos(), &RayOrig.x, &RayOrig.y,	&RayOrig.z, &RayDir.x, &RayDir.y,	&RayDir.z );

	CGame::GetInstance().SetRayDir( RayDir );
	CGame::GetInstance().SetRayOrig( RayOrig );

	float Origin[3], Direction[3];

	Origin[0] = RayOrig.x; Origin[1] = RayOrig.y; Origin[2] = RayOrig.z;
	Direction[0] = RayDir.x; Direction[1] = RayDir.y; Direction[2] = RayDir.z;


	CJustModelAVT* pAvt;

	if(!::IsForcedMovingCameraState())
	{	
		for(int ii = 0; ii < 5; ii += 1 )
		{
			if(::intersectRay2OBB(Origin, Direction, m_SelectAvatarBox[ii].CenterPosition, m_SelectAvatarBox[ii].Rotation,m_SelectAvatarBox[ii].xMinMax[1], m_SelectAvatarBox[ii].yMinMax[1], m_SelectAvatarBox[ii].zMinMaxEx))
			{

				//::InputSceneCylinder(m_SelectAvatarBox[ii].Position[0], m_SelectAvatarBox[ii].Position[1], m_SelectAvatarBox[ii].Position[2], 400.0f, 50.0f);

				if(currentIndex == 5)
					::InputForcedMovingCameraElement(m_MovingCameraState[ii].Position, m_MovingCameraState[ii].Rotation, 1.0f);
				else
					::InputForcedMovingCameraElement(m_MovingCameraState[ii].Position, m_MovingCameraState[ii].Rotation, 0.5f * fabsf(ii - currentIndex));
                

				/*if(currentIndex == 5)
					::InputForcedMovingCameraSystemElement(m_MovingCameraState[ii].Position, m_MovingCameraState[ii].Rotation, TestElement, 1.0f);
				else
					::InputForcedMovingCameraSystemElement(m_MovingCameraState[ii].Position, m_MovingCameraState[ii].Rotation, TestElement, 0.5f * fabsf(ii - currentIndex));

				*/
				currentIndex =  ii;
				zoomInOnOff = true;
				break;
			}
		}


	}
}

void CGameStateSelectAvatar::ReConstructSelectAvatar(int iPrevStateID)
{

	if(iPrevStateID == 5 || iPrevStateID == 13)
	{
		CJustModelAVT* pAvt;

		for(int ii = 0; ii < 5; ii += 1)
		{
			m_SelectAvatarBox[ii].hNode = NULL;
			pAvt = CGameDataCreateAvatar::GetInstance().FindAvatarCount(ii);
		
			if(pAvt != NULL)
			{

				::setPosition(pAvt->GetModelNode(),m_SelectAvatarBox[ii].Position[0], m_SelectAvatarBox[ii].Position[1], m_SelectAvatarBox[ii].Position[2]);
				::setRotationQuat2(pAvt->GetModelNode(),m_SelectAvatarBox[ii].Rotation);
				m_SelectAvatarBox[ii].hNode = pAvt->GetModelNode();
				::setModelDirectionByPosition( pAvt->GetModelNode(), m_SelectAvatarBox[ii].DirectionXY[0], m_SelectAvatarBox[ii].DirectionXY[1]);

				for(int jj = 0; jj < 6; jj += 1)
				{
					pAvt->m_iMotionIDX[jj] = m_SelectAvatarBox[ii].MotionIDX[jj];
				}

				if(!pAvt->GetDeleteRemainSEC())
					pAvt->m_iCurrentMotionState = 1;
				else
					pAvt->m_iCurrentMotionState = 5; 

				pAvt->SetMotion(pAvt->m_iMotionIDX[ pAvt->m_iCurrentMotionState], 0);
			}
		}
	}
	else if( true/*iPrevStateID == 6*/)
	{
		LoadCurrentSelectAvatar();
	}
}

void CGameStateSelectAvatar::InitStateSelectAvatarElement()
{

	m_SelectAvatarBox[0].Position[0] = -4890.37f + 520000.0f; m_SelectAvatarBox[0].Position[1] = 135.333f + 520000.0f; m_SelectAvatarBox[0].Position[2] = 5497.79f;
	m_SelectAvatarBox[0].Rotation[0] = 0.0f; m_SelectAvatarBox[0].Rotation[1] = 0.0f; m_SelectAvatarBox[0].Rotation[2] = 0.67353f; m_SelectAvatarBox[0].Rotation[3] = 0.73916f; 

	m_SelectAvatarBox[1].Position[0] = -4841.4f + 520000.0f; m_SelectAvatarBox[1].Position[1] = 261.975f + 520000.0f; m_SelectAvatarBox[1].Position[2] = 5497.79f;
	m_SelectAvatarBox[1].Rotation[0] = 0.0f; m_SelectAvatarBox[1].Rotation[1] = 0.0f; m_SelectAvatarBox[1].Rotation[2] = 0.455356f; m_SelectAvatarBox[1].Rotation[3] = 0.89031f; 

	m_SelectAvatarBox[2].Position[0] = -4735.52f + 520000.0f; m_SelectAvatarBox[2].Position[1] = 347.595f + 520000.0f; m_SelectAvatarBox[2].Position[2] = 5497.79f;
	m_SelectAvatarBox[2].Rotation[0] = 0.0f; m_SelectAvatarBox[2].Rotation[1] = 0.0f; m_SelectAvatarBox[2].Rotation[2] = 0.21768f; m_SelectAvatarBox[2].Rotation[3] = 0.97602f; 

	m_SelectAvatarBox[3].Position[0] = -4602.94f + 520000.0f; m_SelectAvatarBox[3].Position[1] = 368.719f + 520000.0f; m_SelectAvatarBox[3].Position[2] = 5497.79f;
	m_SelectAvatarBox[3].Rotation[0] = 0.0f; m_SelectAvatarBox[3].Rotation[1] = 0.0f; m_SelectAvatarBox[3].Rotation[2] = -0.0587484f; m_SelectAvatarBox[3].Rotation[3] = 0.998273f; 


	m_SelectAvatarBox[4].Position[0] = -4475.84f + 520000.0f; m_SelectAvatarBox[4].Position[1] = 319.976f + 520000.0f; m_SelectAvatarBox[4].Position[2] = 5497.79f;
	m_SelectAvatarBox[4].Rotation[0] = 0.0f; m_SelectAvatarBox[4].Rotation[1] = 0.0f; m_SelectAvatarBox[4].Rotation[2] = -0.303677f; m_SelectAvatarBox[4].Rotation[3] = 0.952775f; 


	D3DXQUATERNION bufferQuaternion;
	D3DXMATRIX bufferMatrix;


	for(int ii = 0; ii < 5; ii += 1)
	{
		m_SelectAvatarBox[ii].xMinMax[0] = -30.0f; m_SelectAvatarBox[ii].xMinMax[1] = 30.0f;
		m_SelectAvatarBox[ii].yMinMax[0] = -30.0f; m_SelectAvatarBox[ii].yMinMax[1] = 30.0f;
		m_SelectAvatarBox[ii].zMinMax[0] = 0.0f; m_SelectAvatarBox[ii].zMinMax[1] = 200.0f;
		m_SelectAvatarBox[ii].zMinMaxEx = 0.5f * m_SelectAvatarBox[ii].zMinMax[1];


		m_SelectAvatarBox[ii].CenterPosition[0] = m_SelectAvatarBox[ii].Position[0];
		m_SelectAvatarBox[ii].CenterPosition[1] = m_SelectAvatarBox[ii].Position[1];
		m_SelectAvatarBox[ii].CenterPosition[2] = m_SelectAvatarBox[ii].Position[2] + 0.5f * m_SelectAvatarBox[ii].zMinMax[1];

		m_SelectAvatarBox[ii].hNode = NULL;

		bufferQuaternion.x = m_SelectAvatarBox[ii].Rotation[0];
		bufferQuaternion.y = m_SelectAvatarBox[ii].Rotation[1];
		bufferQuaternion.z = m_SelectAvatarBox[ii].Rotation[2];
		bufferQuaternion.w = m_SelectAvatarBox[ii].Rotation[3];

		D3DXMatrixRotationQuaternion(&bufferMatrix, &bufferQuaternion);

		m_SelectAvatarBox[ii].DirectionXY[0] = -bufferMatrix._21 + m_SelectAvatarBox[ii].Position[0];
		m_SelectAvatarBox[ii].DirectionXY[1] = -bufferMatrix._22 + m_SelectAvatarBox[ii].Position[1];


	}

	m_MovingCameraZoomInBox.Position[0] = -4699.67f + 520000.0f; m_MovingCameraZoomInBox.Position[1] = 261.291f + 520000.0f; m_MovingCameraZoomInBox.Position[2] = 5471.66f;
	m_MovingCameraZoomInBox.Rotation[0] = 0.0f; m_MovingCameraZoomInBox.Rotation[1] = 0.0f; m_MovingCameraZoomInBox.Rotation[2] = 0.244403f; m_MovingCameraZoomInBox.Rotation[3] = 0.969674f; 

	m_MovingCameraZoomInBox.xMinMax[0] = -260.0f; m_MovingCameraZoomInBox.xMinMax[1] = 260.0f;
	m_MovingCameraZoomInBox.yMinMax[0] = -60.0f; m_MovingCameraZoomInBox.yMinMax[1] = 60.0f;
	m_MovingCameraZoomInBox.zMinMax[0] = 0.0f; m_MovingCameraZoomInBox.zMinMax[1] = 170.0f;
	m_MovingCameraZoomInBox.zMinMaxEx = 0.5f * m_MovingCameraZoomInBox.zMinMax[1];

	m_MovingCameraZoomInBox.CenterPosition[0] = m_MovingCameraZoomInBox.Position[0];
	m_MovingCameraZoomInBox.CenterPosition[1] = m_MovingCameraZoomInBox.Position[1];
	m_MovingCameraZoomInBox.CenterPosition[2] = m_MovingCameraZoomInBox.Position[2] + 0.5f * m_MovingCameraZoomInBox.zMinMax[1];

	m_MovingCameraState[0].Position[0] = -4413.25f + 520000.0f; m_MovingCameraState[0].Position[1] = -121.743f + 520000.0f; m_MovingCameraState[0].Position[2] = 5758.9f;
	m_MovingCameraState[0].Rotation[0] = 0.543959f; m_MovingCameraState[0].Rotation[1] = 0.308789f; m_MovingCameraState[0].Rotation[2] = 0.385177f; m_MovingCameraState[0].Rotation[3] = 0.678525f; 

	m_MovingCameraState[1].Position[0] = -4398.13f + 520000.0f; m_MovingCameraState[1].Position[1] = -99.3186f + 520000.0f; m_MovingCameraState[1].Position[2] = 5760.65f;
	m_MovingCameraState[1].Rotation[0] = 0.567169f; m_MovingCameraState[1].Rotation[1] = 0.271911f; m_MovingCameraState[1].Rotation[2] = 0.336083f; m_MovingCameraState[1].Rotation[3] = 0.701023f; 


	m_MovingCameraState[2].Position[0] = -4371.77f + 520000.0f; m_MovingCameraState[2].Position[1] = -72.6093f + 520000.0f; m_MovingCameraState[2].Position[2] = 5759.6f;
	m_MovingCameraState[2].Rotation[0] = 0.588678f; m_MovingCameraState[2].Rotation[1] = 0.215489f; m_MovingCameraState[2].Rotation[2] = 0.267821f; m_MovingCameraState[2].Rotation[3] = 0.731638f; 


	m_MovingCameraState[3].Position[0] = -4344.54f + 520000.0f; m_MovingCameraState[3].Position[1] = -54.3433f + 520000.0f; m_MovingCameraState[3].Position[2] = 5755.77f;
	m_MovingCameraState[3].Rotation[0] = 0.597303f; m_MovingCameraState[3].Rotation[1] = 0.163217f; m_MovingCameraState[3].Rotation[2] = 0.206981f; m_MovingCameraState[3].Rotation[3] = 0.757461f; 


	m_MovingCameraState[4].Position[0] =-4319.03f + 520000.0f; m_MovingCameraState[4].Position[1] = -43.8005f + 520000.0f; m_MovingCameraState[4].Position[2] =5748.29f;
	m_MovingCameraState[4].Rotation[0] = 0.592399f; m_MovingCameraState[4].Rotation[1] = 0.117329f; m_MovingCameraState[4].Rotation[2] = 0.154854f; m_MovingCameraState[4].Rotation[3] = 0.781868f; 

	m_MovingCameraState[5].Position[0] = -4086.34f + 520000.0f; m_MovingCameraState[5].Position[1] = -288.088f + 520000.0f; m_MovingCameraState[5].Position[2] = 5962.44f;
	m_MovingCameraState[5].Rotation[0] = 0.54276f; m_MovingCameraState[5].Rotation[1] = 0.209241f; m_MovingCameraState[5].Rotation[2] = 0.292589f; m_MovingCameraState[5].Rotation[3] =0.758961f; 
}

const char* CGameStateSelectAvatar::PickingSelectAvatar()
{
	D3DXVECTOR3 RayOrig;
	D3DXVECTOR3 RayDir;

	::getRay (m_ScreenPOS.m_nX, m_ScreenPOS.m_nY, &RayOrig.x, &RayOrig.y,	&RayOrig.z, &RayDir.x, &RayDir.y,	&RayDir.z );

	CGame::GetInstance().SetRayDir( RayDir );
	CGame::GetInstance().SetRayOrig( RayOrig );

	float Origin[3], Direction[3];

	Origin[0] = RayOrig.x; Origin[1] = RayOrig.y; Origin[2] = RayOrig.z;
	Direction[0] = RayDir.x; Direction[1] = RayDir.y; Direction[2] = RayDir.z;

	CJustModelAVT* pAvt;
	for(int ii = 0; ii < 5; ii += 1 )
	{
		if(::intersectRay2OBB(Origin, Direction, m_SelectAvatarBox[ii].CenterPosition, m_SelectAvatarBox[ii].Rotation,m_SelectAvatarBox[ii].xMinMax[1], m_SelectAvatarBox[ii].yMinMax[1], m_SelectAvatarBox[ii].zMinMaxEx))
		{
			if(m_SelectAvatarBox[ii].hNode != NULL)
			{
				pAvt = CGameDataCreateAvatar::GetInstance().FindAvatarCount(ii);
				if(pAvt != NULL)
					return pAvt->GetName().c_str();
				else
					return NULL;
			}

		}
	}

	return NULL;
}

void CGameStateSelectAvatar::RevisionCulledModel()
{

	for(int ii = 0; ii < 5; ii += 1)
	{
		if(m_SelectAvatarBox[ii].hNode != NULL)
		{
			CulledModelAnimationRevision(m_SelectAvatarBox[ii].hNode);
		}
	}
}

void CGameStateSelectAvatar::OnSelectAvatar(CJustModelAVT* pPrevAvt, CJustModelAVT* pNowAvt)
{
	if(pPrevAvt != NULL)
	{
		if(!pPrevAvt->GetDeleteRemainSEC())
		{
			pPrevAvt->m_iCurrentMotionState = 1;
			pPrevAvt->SetMotion(pPrevAvt->m_iMotionIDX[pPrevAvt->m_iCurrentMotionState], 0);
		}
	}

	if(pNowAvt != NULL)
	{
		if(!pNowAvt->GetDeleteRemainSEC())
		{
			pNowAvt->m_iCurrentMotionState = 3;
			pNowAvt->SetMotion(pNowAvt->m_iMotionIDX[pNowAvt->m_iCurrentMotionState], 0);
		}
	}

}

void CGameStateSelectAvatar::OnLeaveSelectAvatar()
{
	const char* SelectedName = CGameDataCreateAvatar::GetInstance().GetSelectedAvatarName();

	CJustModelAVT* pAvt = CGameDataCreateAvatar::GetInstance().FindAvatar( SelectedName );
	pAvt->SetMotion(pAvt->m_iMotionIDX[4], 1);

}

void CGameStateSelectAvatar::ClearSelectedAvatar()
{
	const char* SelectedName = CGameDataCreateAvatar::GetInstance().GetSelectedAvatarName();

	if(SelectedName != NULL)
	{
		CJustModelAVT* pAvt = CGameDataCreateAvatar::GetInstance().FindAvatar( SelectedName );

		if(pAvt == NULL)
			return;


		if(!pAvt->GetDeleteRemainSEC())
		{
			pAvt->m_iCurrentMotionState = 1;
			pAvt->SetMotion(pAvt->m_iMotionIDX[pAvt->m_iCurrentMotionState], 0);
		}
	}

}


void CGameStateSelectAvatar::SaveCurrentSelectAvatar()
{	
	CJustModelAVT* pAvt;

	for(int ii = 0; ii < 5; ii += 1)
	{		
		if(m_SelectAvatarBox[ii].hNode != NULL)
		{
			pAvt = CGameDataCreateAvatar::GetInstance().FindAvatarCount(ii);
			m_SelectAvatarBox[ii].BufferAvatarName = pAvt->GetName(); 
		}
	}
}

void CGameStateSelectAvatar::LoadCurrentSelectAvatar()
{
	CJustModelAVT* pAvt;

	int iNextIndex = 0;
	int iPlatinum_Cnt		= 3;
	int iPremium_Cnt		= 0;

	for(int ii = 0; ii < 5; ii += 1)
	{
		//06. 03. 08 - 김주현 : 케릭터 삭제를 한 후에.. 클라를 끄지않고 삭제대기시간을 기다린후
		//케릭터를 생성하면..-_-;; pAvt에 쓰레기 값이 들어가나;; 기존에는 쓰레기값인지 아닌지 체크를 안하고 해서 
		//덤프 에러뜨면서 강종되던 것을 수정.
		pAvt = CGameDataCreateAvatar::GetInstance().FindAvatar(m_SelectAvatarBox[ii].BufferAvatarName.c_str());

		if(m_SelectAvatarBox[ii].hNode != NULL && NULL != pAvt)
		{
			pAvt->m_iSelectPositionIndex = ii;

			//홍근
			if( pAvt->IsPlatinum() )
			{				
				iPlatinum_Cnt++;
			}
			else
			{
				iPremium_Cnt++;
			}

			m_SelectAvatarBox[ii].hNode = pAvt->GetModelNode();

			::setPosition(pAvt->GetModelNode(),m_SelectAvatarBox[ii].Position[0], m_SelectAvatarBox[ii].Position[1], m_SelectAvatarBox[ii].Position[2]);
			::setRotationQuat2(pAvt->GetModelNode(),m_SelectAvatarBox[ii].Rotation);
			::setModelDirectionByPosition( pAvt->GetModelNode(), m_SelectAvatarBox[ii].DirectionXY[0], m_SelectAvatarBox[ii].DirectionXY[1]);

			if(!pAvt->GetDeleteRemainSEC())
				pAvt->m_iCurrentMotionState = 1;
			else
				pAvt->m_iCurrentMotionState = 5;

			for(int jj = 0; jj < 6; jj += 1)
			{
				pAvt->m_iMotionIDX[jj] = m_SelectAvatarBox[ii].MotionIDX[jj];
			}

			pAvt->SetMotion(pAvt->m_iMotionIDX[pAvt->m_iCurrentMotionState], 0);

		}
	}

	pAvt = CGameDataCreateAvatar::GetInstance().FindAvatarCurrentState( -1);

	if(pAvt != NULL)
	{
		//홍근
		if( pAvt->IsPlatinum() )
		{				
			iNextIndex = iPlatinum_Cnt;
		}
		else
		{
			iNextIndex = iPremium_Cnt;
		}

		if(iNextIndex < 6)
		{
			pAvt->m_iSelectPositionIndex = iNextIndex;
			m_SelectAvatarBox[iNextIndex].hNode = pAvt->GetModelNode();

			::setPosition(pAvt->GetModelNode(),m_SelectAvatarBox[iNextIndex].Position[0], m_SelectAvatarBox[iNextIndex].Position[1], m_SelectAvatarBox[iNextIndex].Position[2]);
			::setRotationQuat2(pAvt->GetModelNode(),m_SelectAvatarBox[iNextIndex].Rotation);
			m_SelectAvatarBox[iNextIndex].hNode = pAvt->GetModelNode();
			::setModelDirectionByPosition( pAvt->GetModelNode(), m_SelectAvatarBox[iNextIndex].DirectionXY[0], m_SelectAvatarBox[iNextIndex].DirectionXY[1]);

			pAvt->m_iCurrentMotionState = 0;

			for(int jj = 0; jj < 6; jj += 1)
			{
				pAvt->m_iMotionIDX[jj] = m_SelectAvatarBox[iNextIndex].MotionIDX[jj];
			}

			pAvt->SetMotion(pAvt->m_iMotionIDX[pAvt->m_iCurrentMotionState], 1);


		}
	}

}
