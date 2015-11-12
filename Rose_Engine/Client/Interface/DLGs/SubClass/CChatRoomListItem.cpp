#include "stdafx.h"
#include ".\cchatroomlistitem.h"
#include "../../../Game.h"
#include "../../it_mgr.h"
#include "../../../GameData/CChatRoom.h"
#include "ZListBox.h"
#include "ResourceMgr.h"
#include "../../CTDrawImpl.h"
#include "../../IO_ImageRes.h"
#include "CChattingRoomPasswordDlg.h"

CChatRoomListItem::CChatRoomListItem( WORD wID, BYTE btRoomType, BYTE btUserCount, const char* pszTitle )
{
	SetHeight( 20 );
	SetWidth( 174 );
	m_wID = wID;
	m_btRoomType = btRoomType;
	m_btUserCount = btUserCount;
	assert( pszTitle );
	if( pszTitle )
		m_strTitle = pszTitle;
}

CChatRoomListItem::~CChatRoomListItem(void)
{

}

void CChatRoomListItem::Draw()
{
#ifdef __PRIVATECHAT2
	//홍근 오후 3:03 2006-10-25
	//비번방 아이콘.	
	if(m_btRoomType)
	{		
		SIZE size = getFontTextExtent( g_GameDATA.m_hFONT[ FONT_NORMAL_BOLD ], m_strTitle.c_str() );
		int iImageID    = CResourceMgr::GetInstance()->GetImageNID( UI_IMAGE_RES_ID, "CHATROOM_LOCK" );
		g_DrawImpl.Draw( m_sPosition.x+size.cx+4, m_sPosition.y, UI_IMAGE_RES_ID, iImageID );	
	}	
	D3DXMATRIX mat;	
	D3DXMatrixTranslation( &mat, (float)m_sPosition.x, (float)m_sPosition.y,0.0f);
	::setTransformSprite( mat );	
	::drawFont( g_GameDATA.m_hFONT[ FONT_NORMAL_BOLD ], 
		true, 
		0, 0, 
		(IsSelected()) ? g_dwYELLOW : g_dwWHITE, 
		m_strTitle.c_str() );

	

#else
	D3DXMATRIX mat;	
	D3DXMatrixTranslation( &mat, (float)m_sPosition.x, (float)m_sPosition.y,0.0f);
	::setTransformSprite( mat );	
	::drawFont( g_GameDATA.m_hFONT[ FONT_NORMAL_BOLD ], 
		   true, 
		   0, 0, 
		   (IsSelected()) ? g_dwYELLOW : g_dwWHITE, 
		   m_strTitle.c_str() );
#endif	//__PRIVATECHAT2
}

unsigned CChatRoomListItem::Process( unsigned uiMsg, WPARAM wParam, LPARAM lParam )
{
	if( !IsInside( LOWORD(lParam), HIWORD( lParam )) ) return 0;

	switch( uiMsg )
	{
	case WM_LBUTTONDOWN:
		SetSelected();
		return GetControlID();
	case WM_LBUTTONDBLCLK:
		if( !g_itMGR.IsDlgOpened( DLG_TYPE_CHATROOM ) )
		{
			if( m_btRoomType )
			{
				CChattingRoomPasswordDlg * pDlg = new CChattingRoomPasswordDlg;
				pDlg->Show();
				pDlg->SetID(m_wID);
				pDlg->MoveWindowCenter();
			}
			else
			{
				CChatRoom::GetInstance().SendReqJoinRoom( m_btRoomType, m_wID, NULL );
			}			
		}
		break;
	default:
		break;
	}
	
	return 0;
}
void CChatRoomListItem::SetSelected()
{
	if( m_pParent )
	{
		if( m_pParent->GetControlType() == CTRL_ZLISTBOX )
		{
			CZListBox* pList = ( CZListBox*)m_pParent;
			pList->DeselectAll();
		}
	}
	CWinCtrl::SetSelected();
}

void CChatRoomListItem::SetRoomOwnerName( const char * pName )
{
	m_strRoomOwnerName = pName;
}

void CChatRoomListItem::Update( POINT ptMouse )
{	
}

