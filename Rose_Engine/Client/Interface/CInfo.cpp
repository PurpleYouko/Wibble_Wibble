#include "stdafx.h"
#include ".\cinfo.h"
#include "IO_ImageRes.h"
#include "CTDrawImpl.h"
#include "../../TGameCtrl/ResourceMgr.h"
CInfo::CInfo(void)
{
	Clear();
}

CInfo::~CInfo(void)
{
}

void CInfo::Draw()
{
	///배경 이미지 그리기 
	///Width는 g_DrawImpl시에 Size를 주고
	///Height는 Text그리기 전에 그려주자.

	

	std::vector< CTString >::iterator iter;
	
	int iPosX = m_ptPosition.x;
	int iPosY = m_ptPosition.y;

	switch( m_iType )
	{
	case INFOTYPE_LINEIMG:		
		{
			RECT rcDraw = { 2, 2, m_iWidth, 17 };

			for( iter = m_listString.begin(); iter != m_listString.end(); ++iter )
			{
				DrawLineImage( iPosX , iPosY , m_iWidth );
				iter->Draw( rcDraw );
				iPosY += 17;
			}
		}
		break;

	case INFOTYPE_ONEIMG:
		{
			RECT rcDraw = { m_ptPositionText.x, m_ptPositionText.y,
							-m_ptPositionText.x+m_iWidth, m_ptPositionText.y+m_iHeight };

			DrawOneImage( iPosX , iPosY );
			for( iter = m_listString.begin(); iter != m_listString.end(); ++iter )
			{				
				iter->Draw( rcDraw );			
				rcDraw.top += 17;
				rcDraw.bottom += 17;
			}
		}
		break;
	}
	
}

bool CInfo::IsEmpty()
{
	return m_listString.empty();
}

void CInfo::DrawLineImage( int x, int y, int iWidth )
{	
	g_DrawImpl.Draw( x, y, iWidth, m_iMouduleNo, m_iLineImageNo , D3DCOLOR_ARGB( 125,255,255,255) );
}

void CInfo::DrawOneImage( int x, int y )
{	
	g_DrawImpl.Draw( x, y, m_iMouduleNo, m_iLineImageNo , D3DCOLOR_ARGB( 255,255,255,255) );
}

void CInfo::SetImage( int iModuleID, const char* szSID )
{
	m_iMouduleNo = iModuleID;
	m_iLineImageNo = CResourceMgr::GetInstance()->GetImageNID( iModuleID, szSID );
	assert( m_iLineImageNo!=-1 && "CInfo::SetImage()");

	CImageRes* pRes = CImageResManager::GetSingleton().GetImageRes( iModuleID );
	RECT rc = pRes->GetSprite( m_iLineImageNo )->m_Rect;

	m_iWidth = rc.right - rc.left;
	m_iHeight = rc.bottom - rc.top;	
}

void CInfo::Clear()
{
	m_iMouduleNo = UI_IMAGE_RES_ID;
	m_iLineImageNo = CResourceMgr::GetInstance()->GetImageNID(UI_IMAGE_RES_ID, "ID_BLACK_PANEL"  );
	m_listString.clear();
//	m_strTitle.clear();
	
	m_iHeight = 0;
	
	m_uMaxSizeString	= 0;
	m_iWidth			= 0;

	m_ptPosition.x		= 0;
	m_ptPosition.y		= 0;

	m_iType = INFOTYPE_LINEIMG;
}

void CInfo::SetPosition( POINT pt )
{ 
	m_ptPosition = pt; 
	AdjustPosition();
}

void CInfo::SetPositionText( POINT pt )
{ 
    m_ptPositionText = pt;	
}

void CInfo::SetPositionText( int x, int y )
{ 
	m_ptPositionText.x = x;
	m_ptPositionText.y = y;
}

int	 CInfo::GetWidth(){ return m_iWidth; }
int  CInfo::GetHeight(){ return m_iHeight; }
int	 CInfo::GetMaxSizeString(){ return m_uMaxSizeString; }
void CInfo::AddString( CTString& TString )
{
	AddString( TString.m_strText.c_str(), TString.m_dwColor, TString.m_uFormat );
}

void CInfo::AddString( const char* pszTxt, DWORD color, HNODE hFont, UINT uFormat )
{
	if( pszTxt == NULL )
	{
		Clear();		
		return;
	}

	SIZE sizeString = getFontTextExtent( hFont, pszTxt );

	m_iHeight += 18;//sizeString.cy + 5;

	if( m_iWidth < sizeString.cx + 5)
		m_iWidth = sizeString.cx + 5;

	if( strlen( pszTxt ) > m_uMaxSizeString )
		m_uMaxSizeString = strlen( pszTxt );


	CTString String;
	String.SetColor( color );
	String.SetString( pszTxt );
	String.SetFormat( uFormat );
	String.SetFont( hFont );
	m_listString.push_back( String );	
	AdjustPosition();
}

void CInfo::AdjustPosition()
{
	if( m_ptPosition.x < 0 )
		m_ptPosition.x = 0;
	if( m_ptPosition.y < 0 )
		m_ptPosition.y = 0;

	if( m_ptPosition.x + m_iWidth > g_pCApp->GetWIDTH() )
		m_ptPosition.x = g_pCApp->GetWIDTH() - m_iWidth;

	if( m_ptPosition.y + m_iHeight  > g_pCApp->GetHEIGHT() )
		m_ptPosition.y = g_pCApp->GetHEIGHT() - m_iHeight ;

}


void CInfo::SetInfoType(int iType)
{
	m_iType = iType;
}