#include "stdafx.h"
#include "cgamestatetitle.h"
#include <process.h>
#include "CGame.h"
#include "../IO_Terrain.h"
#include "../CCamera.h"
#include "System_FUNC.h"
#include "SystemProcScript.h"
#include "../interface/IO_ImageRes.h"
#include "../interface/it_mgr.h"


int CGameStateTitle::m_iBackGroundZone  = 1;

CGameStateTitle::CGameStateTitle( int iID )
{
	m_iStateID  = iID;

	m_iBackGroundZone = 1;
	m_hTitleTexture	  = NULL;
}

CGameStateTitle::~CGameStateTitle(void)
{
}


int	CGameStateTitle::Update( bool bLostFocus )
{	
	return 0;
}
int CGameStateTitle::Enter( int iPrevStateID )
{ 
	CGame::GetInstance().Load_DataNotUseThread();


	///
	/// ������� ����� �� ��ȣ�� ���´�.
	///
	m_iBackGroundZone = SC_GetBGZoneNO();
	
	m_hTitleTexture = loadTexture ( "3DData\\Control\\Res\\ExUI1.dds", 
								"3DData\\Control\\Res\\ExUI1.dds",
								1, 
								0 );
	
//	m_hThread = (HANDLE)_beginthreadex( NULL, 0, &ThreadFunc, NULL, CREATE_SUSPENDED, NULL );
////	SetThreadPriority( m_hThread,THREAD_PRIORITY_HIGHEST  );
//
//	if( m_hThread )
//	{
//		ResumeThread( m_hThread );
//	}
//	else///Thread ���� ���н� ���ξ����忡�� �ε��ϰ� State�� �ٲپ� �ش�.
	{
		Draw();
		ThreadFunc(NULL);

		if (g_GameDATA.m_SkipIntroNote)
			CGame::GetInstance().ChangeState( CGame::GS_LOGIN );
		else
			CGame::GetInstance().ChangeState( CGame::GS_WARNING );
	

	}
	
	return 0; 
}

int CGameStateTitle::Leave( int iNextStateID )
{ 
	if( m_hTitleTexture )
	{
		unloadTexture( m_hTitleTexture );
		m_hTitleTexture = NULL;
	}
	return 0; 
}

unsigned __stdcall CGameStateTitle::ThreadFunc( void* pArguments )
{
	setDelayedLoad(0);
	CImageResManager::GetSingletonPtr()->LoadImageResources();

	g_itMGR.Init();


	CGame::GetInstance().Load_BasicDATA();


//	g_pTerrain->SetLoadingMode( true );
	g_pTerrain->LoadZONE( CGameStateTitle::m_iBackGroundZone , false );

	///
	/// ī�޶� ����� 32_32 �������� ���������.. ��������� ���ؼ� �����Ѵ�.
	///
	D3DVECTOR PosENZIN;
	PosENZIN.x = 520000.0f;
	PosENZIN.y = 520000.0f;
	PosENZIN.z = 0.0f;

	g_pCamera->Set_Position ( PosENZIN );
	::setDelayedLoad( 2 );	
	::setDelayedLoad( 0 );	
	return 0;
}

void CGameStateTitle::Draw()
{
	::setClearColor( 1, 1, 1 );
	if( g_pCApp->IsActive() )
	{
		if ( !::beginScene() ) //  ����̽��� �սǵ� ���¶�� 0�� �����ϹǷ�, ��� ������ ��ŵ
		{
			return;
		}
		
		::clearScreen();

		::beginSprite( D3DXSPRITE_ALPHABLEND );	

		D3DXMATRIX mat, matScale, matTrans;	
		int iWidth, iHeight;


		::getTextureSize( m_hTitleTexture, iWidth, iHeight );
		D3DXVECTOR3 vCenter( (float)(iWidth/2), (float)(iHeight/2), 0);

		float fScaleWidth  = (float)(g_pCApp->GetWIDTH() / 1024.);
		float fScaleHeight = (float)(g_pCApp->GetHEIGHT() / 768.);

		D3DXMatrixScaling( &matScale, fScaleWidth, fScaleHeight, 0.0f );
		D3DXMatrixTranslation( &matTrans, (float)(g_pCApp->GetWIDTH()/2), (float)(g_pCApp->GetHEIGHT()/2), 0 );
		D3DXMatrixMultiply( &mat, &matScale, &matTrans );
		
		::setTransformSprite( mat );
		::drawSprite( m_hTitleTexture, 
						NULL,
						&vCenter,
						NULL,								
						D3DCOLOR_RGBA( 255, 255, 255, 255 ) );
		
		
		::endSprite();

		::endScene();
		::swapBuffers();
	}
}