#include "stdafx.h"
#include "CUIMediator.h"
#include "../Game.h"

#include "System/CGame.h"
#include "TriggerInfo.h"

CUIMediator	g_UIMed;

char *s_pLangStr[] = {
	"en",		// 0; English
	"ar",		// 1; Arabic
	"bg",		// 2; BULGARIAN
	"nl",		// 3; DUTCH
	"fr",		// 4; FRENCH
	"de",		// 5; GERMAN
	"hu",		// 6; HUNGARIAN
	"id",		// 7; INDONESIAN
	"it",		// 8; ITALIAN
	"ja",		// 9; JAPANESE
	"ko",		// 10; KOREAN
	"ms",		// 11; MALAY
	NULL
};

CUIMediator::CUIMediator()
{
	m_iLogoWidth = 0;
	m_iLogoHeight = 0;

	// Construct
}

CUIMediator::~CUIMediator()
{
	// Destruct
}

//----------------------------------------------------------------------------------------------------
/// @param
/// @brief  UI관리자 초기화..
//----------------------------------------------------------------------------------------------------

bool CUIMediator::InitMediator()
{
	setDelayedLoad(0);

	if( m_ChatBoxMgr.Init() == false )
		return false;

	//if( m_NameBox.Init( "3DData\\Control\\Res\\NameBox.tga" ) == false )
	//	return false;

	if( m_DigitEffect.Init() == false )
		return false;

	if( m_PersonalStoreManager.Init() == false )
		return false;
	
	/// For load..( temp )
	m_LogoTex = loadTexture ( "Logo.dds", "Logo.dds", 1, 0 );

	setDelayedLoad(1);

	if( m_LogoTex == NULL )
	{
#ifdef _DEBUG
			g_pCApp->ErrorBOX( "LogoTex load failed !!", "File open error" );
#endif
			//실패한 이유를 적어준다..
			return false;
	}	
	
	getTextureSize( m_LogoTex, m_iLogoWidth, m_iLogoHeight );

	return true;
}

//----------------------------------------------------------------------------------------------------
/// @param
/// @brief 모든 리소스 해제
//----------------------------------------------------------------------------------------------------

void CUIMediator::FreeResource()
{
	m_ChatBoxMgr.FreeResource();
//	m_NameBox.FreeResource();

	m_DigitEffect.ClearAll();
	m_PersonalStoreManager.FreeResource();
}


//----------------------------------------------------------------------------------------------------
/// @param
/// @brief 아바타 이름 출력
//----------------------------------------------------------------------------------------------------

void CUIMediator::DrawAvataName( CObjCHAR* pCharOBJ )
{
	if( pCharOBJ == NULL )
		return;

	D3DVECTOR   PosSCR;
	
	int iType = pCharOBJ->Get_TYPE();
	if( iType == OBJ_MOB )
	{
		int a = 10;
	}
	pCharOBJ->GetScreenPOS (PosSCR);	

	if ( PosSCR.x < 0 || PosSCR.y < 0 )
		return;

	if ( PosSCR.x > g_pCApp->GetWIDTH() ||
		PosSCR.y > g_pCApp->GetHEIGHT() )
		return;
	
	
	m_NameBox.Draw( pCharOBJ, PosSCR.x, PosSCR.y, PosSCR.z ); 	
}

//----------------------------------------------------------------------------------------------------
/// @param
/// @brief 타격수치 출력
//----------------------------------------------------------------------------------------------------

void CUIMediator::CreateDamageDigit(int iPoint,float x,float y,float z, bool bIsUSER )
{
	m_DigitEffect.CreateDigitEffect( iPoint, x, y, z, bIsUSER );
}

//----------------------------------------------------------------------------------------------------
/// @param
/// @brief 
//----------------------------------------------------------------------------------------------------

void CUIMediator::Update()
{
	m_DigitEffect.Refresh();
}

//----------------------------------------------------------------------------------------------------
/// @param
/// @brief 랜더링..
//----------------------------------------------------------------------------------------------------

void CUIMediator::Draw()
{	
	DrawChatBox();	
	//DrawZoneName();

	m_PersonalStoreManager.Draw();
}

//----------------------------------------------------------------------------------------------------
/// @param
/// @brief Chat msg 추가
//----------------------------------------------------------------------------------------------------

void CUIMediator::AddChatMsg( int iCharIndex, const char* pMsg, DWORD Color )
{
	if(g_GameDATA.m_bTranslate)
	{
		std::string strMsgInfo;
		strMsgInfo = "dest=" + std::string(s_pLangStr[g_GameDATA.m_iLangIDX]);
		strMsgInfo += "&msg=" + std::string(pMsg);
		TI_SendHttpPostData("aeongames.net", "/~support/translate.php", strMsgInfo.c_str()/*, (char*)&pMsg*/ );
	}
	m_ChatBoxMgr.AddChat( iCharIndex, pMsg, Color );
}

//----------------------------------------------------------------------------------------------------
/// @param
/// @brief 존이름 세팅
//----------------------------------------------------------------------------------------------------
//
//void CUIMediator::SetZoneName( const char* pszName )
//{
//	if( pszName == NULL )
//		m_strZoneName.erase( m_strZoneName.begin(), m_strZoneName.end() );
//	else
//		m_strZoneName = pszName;
//}

//----------------------------------------------------------------------------------------------------
/// @param
/// @brief 존 이름 랜더링
//----------------------------------------------------------------------------------------------------
//
//void CUIMediator::DrawZoneName()
//{
//	if( !m_strZoneName.empty() )
//	{
//		SIZE size = getFontTextExtent ( g_GameDATA.m_hFONT[ FONT_TITLE ], m_strZoneName.c_str() );
//		D3DXMATRIX mat;	
//		D3DXMatrixTranslation( &mat, (float)(g_pCApp->GetWIDTH() / 2 - size.cx / 2),(float)( g_pCApp->GetHEIGHT() / 3 - size.cy / 2 ) ,0.0f);
//		::setTransformSprite( mat );
//
//		::drawFontf( g_GameDATA.m_hFONT[ FONT_TITLE ], true, 0,0, g_dwWHITE, m_strZoneName.c_str() );
//	}
//}


//----------------------------------------------------------------------------------------------------	
/// @brief personal store
//----------------------------------------------------------------------------------------------------

void CUIMediator::AddPersonalStoreIndex( int iClientObjIDX )
{
	m_PersonalStoreManager.AddStoreList( iClientObjIDX );
}

//----------------------------------------------------------------------------------------------------
/// @param
/// @brief 
//----------------------------------------------------------------------------------------------------

void CUIMediator::SubPersonalStoreIndex( int iClientObjIDX )
{
	m_PersonalStoreManager.SubStoreList( iClientObjIDX );
}

//----------------------------------------------------------------------------------------------------
/// @param
/// @brief
//----------------------------------------------------------------------------------------------------

void CUIMediator::ResetPersonalStore()
{
	m_PersonalStoreManager.ClearList();
}