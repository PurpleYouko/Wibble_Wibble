#include "stdafx.h"
#include ".\cchatroomdlg.h"
#include "../../Game.h"
#include "../../GameData/CChatRoom.h"
#include "../../GameCommon/CFilterWord.h"
#include "../../Network/CNetwork.h"
#include "../it_mgr.h"

#include "../CTDrawImpl.h"
#include "../IO_ImageRes.h"

#include "TImage.h"
#include "TScrollBar.h"
#include "TEditBox.h"
#include "TListBox.h"
#include "ZListBox.h"
#include "JContainer.h"
#include "TabbedPane.h"
#include "TComboBox.h"
#include "TPane.h"
#include "ActionEvent.h"
#include "TButton.h"
#include "JStringParser.h"
#include "CAddFriendDlg.h"

#include "ResourceMgr.h"


const POINT c_ptDrawMemberName[] = 
{
	{  8, 45 }, { 8, 60 } ,{ 8, 75 }, { 8, 90 },
	{ 105, 45 }, {105, 60 }, {105, 75 }, {105, 90 }
};


CChatRoomDlg::CChatRoomDlg( int iDlgType )
{
	SetDialogType( iDlgType );
	m_iTab	= TAB_MEMBER;
	m_bMinimize = false;
}

CChatRoomDlg::~CChatRoomDlg(void)
{
}



unsigned CChatRoomDlg::Process( unsigned uiMsg, WPARAM wParam, LPARAM lParam )
{
	if( !IsVision())  return 0;

#ifdef __PRIVATECHAT2

	CWinCtrl * pCtrl = NULL;	
	switch(uiMsg)
	{
	case WM_LBUTTONDBLCLK:
		{	
			if( m_iTab == TAB_MEMBER && !m_bMinimize)
			{
				int i=0;
				std::list< CChatMember >::iterator iter;
				for( iter = m_Members.begin(); iter != m_Members.end(); ++iter, i++ )
				{						
					if( pCtrl = Find( CStr::Printf("NAME_%02d", i) ) )
					{
						if( pCtrl->IsInside( lParam ) && strcmp((*iter).GetName(), g_pAVATAR->Get_NAME()) )
						{
							g_itMGR.OpenPrivateChatDlg( (*iter).GetUserTag(), FRIEND_STATUS_NORMAL, (*iter).GetName() );
						}
					}
				}				
			}			
		}
		break;
	case WM_KEYDOWN:
		{
			switch( wParam )
			{

			case VK_TAB:
				{
					CTEditBox* pEditBoxID  = NULL;
					CTEditBox* pEditBoxPwd = NULL;
					pCtrl = Find( IID_EDITBOX_TITLE );
					if( pCtrl && pCtrl->GetControlType() == CTRL_EDITBOX )
						pEditBoxID = (CTEditBox*)pCtrl;

					pCtrl = Find( IID_EDITBOX_PWD );
					if( pCtrl && pCtrl->GetControlType() == CTRL_EDITBOX )
						pEditBoxPwd = ( CTEditBox*) pCtrl;

					if( pEditBoxID && pEditBoxPwd )
					{
						if( pEditBoxID->IsFocus() )
							pEditBoxPwd->SetFocus(true);
						else
							pEditBoxID->SetFocus(true);
					}
					return uiMsg;					
				}
				break;			
			}
		}
	}

#endif //__PRIVATECHAT2

	if( unsigned uiProcID = CTDialog::Process( uiMsg, wParam,lParam ) )
	{
		switch( uiMsg )
		{
#ifdef __PRIVATECHAT2
		case WM_KEYDOWN:
#endif
		case WM_LBUTTONUP:
			OnLButtonUp( uiProcID, wParam, lParam );
			break;
		case WM_LBUTTONDOWN:
			OnLButtonDown( uiProcID, wParam, lParam );
		default:
			break;
		}
		return uiMsg;
	}
	return 0;
}

void CChatRoomDlg::Show()
{

#ifdef _DEBUG

	POINT pt = { GetPosition().x, GetPosition().y };
	Clear();
	Create("DlgChatRoom");		
	SetInterfacePos_After();
	MoveWindow(pt);

#endif



	CTDialog::Show();
	CWinCtrl* pCtrl = FindChildInPane( IID_PANE_TOP, IID_TABBEDPANE );

	if( pCtrl && pCtrl->GetControlType() == CTRL_TABBEDPANE )
	{
		CTabbedPane* pPane = (CTabbedPane*)pCtrl;
		pPane->SetSelectedIndex( TAB_CONFIG );
		m_iTab = TAB_CONFIG;
	}

	if( m_bMinimize )
	{
		if( pCtrl = FindChildInPane( IID_PANE_TOP, IID_BTN_MINIMIZE ) )
			pCtrl->Hide();
	}
	else
	{
		if( pCtrl = FindChildInPane( IID_PANE_TOP, IID_BTN_MAXIMIZE ) )
			pCtrl->Hide();
	}
		
#ifdef __PRIVATECHAT2
	SetReturnID( IID_BTN_MAKE );
#endif

}

void CChatRoomDlg::AddChatMsg( const char* pszMsg, DWORD dwColor )
{
	CWinCtrl* pCtrl = FindChildInPane( IID_PANE_BOTTOM, IID_CHAT_LIST );

	//ZListBox
	if( pCtrl && pCtrl->GetControlType() == CTRL_ZLISTBOX)
	{
		AddChatMsg_ZList( pszMsg, dwColor);
	}
	//TListBox
	else
	{
		AddChatMsg_TList( pszMsg, dwColor);
	}
}


void CChatRoomDlg::AddChatMsg_ZList( const char* pszMsg, DWORD dwColor )
{
	CWinCtrl* pCtrl = FindChildInPane( IID_PANE_BOTTOM, IID_CHAT_LIST );

	if( pCtrl && pCtrl->GetControlType() == CTRL_ZLISTBOX)
	{
		CZListBox* pListBox = (CZListBox*)pCtrl;

			int iItemCount = pListBox->GetSize();

			int iWidth	 = pListBox->GetWidth();

			//06.07.27 - 김주현
			//pszMsg가 2라인 이상일 경우에 리스트박스에 한줄씩 뜯어 넣기 위해서 테스트용으로 스트링을 세팅한다.
			
			CJStringParser	m_jString;

			m_jString.SetSplitType( CJStringParser::SPLIT_WORD );
			m_jString.SetDefaultColor( dwColor );
			m_jString.SetString( CStr::Printf( "%s", pszMsg), iWidth - 10 );

			
			CTScrollBar* pScrollBar;

			if(m_jString.GetStringCount() > 1)
			{
				for(int i = 0; i < m_jString.GetStringCount(); i++)
				{
					m_szStirng = m_jString.GetJString(i);

					CZLBAppendText* pAppText = new CZLBAppendText(m_szStirng, iWidth);	

					pListBox->Add( pAppText );

					//GetExtent 주의..
					pListBox->SetExtent( pListBox->GetExtent() );
					pListBox->SetValue( 0 );
					pListBox->SetHeight( pListBox->GetHeight() + pAppText->GetHeight());
					pListBox->SetWidth( iWidth );

					SetHeight( GetHeight() + pAppText->GetHeight() );

					pCtrl = FindChildInPane( IID_PANE_BOTTOM, IID_CHAT_SCROLLBAR );
					if( pCtrl && pCtrl->GetControlType() == CTRL_SCROLLBAR )
					{
						pScrollBar = (CTScrollBar*)pCtrl;
						pScrollBar->SetValue( pListBox->GetMaximum() );
					}
				}
			}
			else
			{
				CZLBAppendText* pAppText = new CZLBAppendText( pszMsg, iWidth);			

				pListBox->Add( pAppText );
				pListBox->SetExtent( pListBox->GetExtent() );
				pListBox->SetValue( 0 );
				pListBox->SetHeight( pListBox->GetHeight() + pAppText->GetHeight());
				pListBox->SetWidth( iWidth );

				SetHeight( GetHeight() + pAppText->GetHeight() );

				pCtrl = FindChildInPane( IID_PANE_BOTTOM, IID_CHAT_SCROLLBAR );
				if( pCtrl && pCtrl->GetControlType() == CTRL_SCROLLBAR )
				{
					CTScrollBar* pScrollBar = (CTScrollBar*)pCtrl;
					pScrollBar->SetValue( pListBox->GetMaximum() );
				}
			}
	}
}



void CChatRoomDlg::AddChatMsg_TList( const char* pszMsg, DWORD dwColor )
{

	CWinCtrl* pCtrl = FindChildInPane( IID_PANE_BOTTOM, IID_CHAT_LIST );
	if( pCtrl )
	{
		CTListBox* pListBox = (CTListBox*)pCtrl;
		if( pListBox->GetValue() >= pListBox->GetMaximum() - pListBox->GetExtent() )
		{
			pListBox->AppendText( pszMsg, dwColor );
			pCtrl = FindChildInPane( IID_PANE_BOTTOM, IID_CHAT_SCROLLBAR );
			if( pCtrl && pCtrl->GetControlType() == CTRL_SCROLLBAR )
			{
				CTScrollBar* pScrollBar = (CTScrollBar*)pCtrl;
				pScrollBar->SetValue( pListBox->GetMaximum() );
			}
		}
		else
		{
			pListBox->AppendText( pszMsg, dwColor, false );
		}
	}
}




void CChatRoomDlg::OnLButtonUp( unsigned uiProcID, WPARAM wParam, LPARAM lParam )
{
	switch( uiProcID )
	{
	case IID_BTN_MINIMIZE:
		Minimize();
		break;
	case IID_BTN_MAXIMIZE:
		Maximize();
		break;
	case IID_BTN_CLOSE:
		Hide();
		break;
	case IID_BTN_MAKE:
		{
			CWinCtrl* pCtrl = FindChildInPane( IID_PANE_TOP, IID_TABBEDPANE );

			assert( pCtrl );
			if( pCtrl == NULL ) return;
			
			assert( pCtrl->GetControlType() == CTRL_TABBEDPANE );
			if( pCtrl->GetControlType() != CTRL_TABBEDPANE ) return;

			CTabbedPane* pTabbedPane = (CTabbedPane*)pCtrl;

			CJContainer* pContainer = pTabbedPane->GetTabContainer( TAB_CONFIG );
			assert( pContainer );
			if( pContainer == NULL ) return;
			
			pCtrl = pContainer->Find( IID_EDITBOX_TITLE );
			assert( pCtrl );
			if( pCtrl == NULL ) return;
			
			assert( pCtrl->GetControlType() == CTRL_EDITBOX );
			if( pCtrl->GetControlType() != CTRL_EDITBOX ) return;
			
			CTEditBox* pTitleEditBox = (CTEditBox*) pCtrl;
			char* pszTitle = pTitleEditBox->get_text();

			if( pszTitle && strlen( pszTitle ) > 0 )
			{
				pCtrl = pContainer->Find( IID_EDITBOX_PWD );
				assert( pCtrl );
				if( pCtrl == NULL ) return;
				
				assert( pCtrl->GetControlType() == CTRL_EDITBOX );
				if( pCtrl->GetControlType() != CTRL_EDITBOX ) return;
				
				CTEditBox* pTitleEditBox = (CTEditBox*) pCtrl;
				char* pszPwd = pTitleEditBox->get_text();
				if( pszPwd && strlen( pszPwd ) < 1 )
					pszPwd = NULL;

				BYTE btRoomType = CHATROOM_NORMAL;				
				//홍근 오후 3:54 2006-10-26 : 커뮤니티 기능 추가 작업.
#ifdef __PRIVATECHAT2				
				if( pszPwd  )
				{
					btRoomType = CHATROOM_PRIVATE;
				}
#else
#pragma message( "현재 모든 채팅방은 공개방으로한다.")				
				pszPwd = NULL;///현재 모든 채팅방은 공개방
#endif

				pCtrl = pContainer->Find( IID_COMBOBOX_MAXUSER );
				assert( pCtrl );
				if( pCtrl == NULL ) return;
				assert( pCtrl->GetControlType() == CTRL_COMBOBOX );
				if( pCtrl->GetControlType() != CTRL_COMBOBOX ) return;

				CTComboBox* pComboBox = (CTComboBox*)pCtrl;
				const char* pszValue = pComboBox->GetSelectedItem();
				int iMaxUserCount = 8;
				if( pszValue )
					iMaxUserCount = atoi( pszValue );

				CChatRoom::GetInstance().SendReqMakeRoom( btRoomType, iMaxUserCount, pszTitle, pszPwd );
			}
			break;
		}
	default:
		break;
	}
}
bool CChatRoomDlg::Create( const char* IDD )
{
	if( !CTDialog::Create( IDD )) return false;
		
	CWinCtrl* pCtrl = FindChildInPane( IID_PANE_TOP,  IID_TABBEDPANE );
	if( pCtrl && pCtrl->GetControlType() == CTRL_TABBEDPANE )
	{
		CTabbedPane* pPane = (CTabbedPane*)pCtrl;
		
		///VIDEO
		CJContainer* pContainer = pPane->GetTabContainer( TAB_CONFIG );
		assert( pContainer );
		pCtrl = pContainer->Find( IID_COMBOBOX_MAXUSER );
		if( pCtrl && pCtrl->GetControlType() == CTRL_COMBOBOX )
		{
			CTComboBox* pComboBox = (CTComboBox*)pCtrl;
			for( int i = 2; i <= 8; ++i )
				pComboBox->AddItem( CStr::Printf("%d",i ));
		}


		pCtrl = pContainer->Find( IID_COMBOBOX_TYPE );
		if( pCtrl && pCtrl->GetControlType() == CTRL_COMBOBOX )
		{
			CTComboBox* pComboBox = (CTComboBox*)pCtrl;
			for( int i = 1; i <= 2; ++i )
				pComboBox->AddItem( CStr::Printf("%d",i ));
		}
	}

	pCtrl = FindChildInPane( IID_PANE_BOTTOM, IID_CHAT_EDIT );
	if( pCtrl->GetControlType() == CTRL_EDITBOX )
	{
		CTEditBox* pEditBox = (CTEditBox*) pCtrl;
		pEditBox->SetLimitText(150);
		pCtrl->AddActionListener( this );	
	}


	CChatRoom::GetInstance().AddObserver( this );
	return true;
}

void CChatRoomDlg::Draw()
{
	if( !IsVision() ) return;
 	CTDialog::Draw();

	D3DXMATRIX mat;	
	D3DXMatrixTranslation( &mat, (float)m_sPosition.x, (float)m_sPosition.y,0.0f);
	::setTransformSprite( mat );
	//멤버 아이디 그리기


	const char* pszTitle = NULL;
	if(CChatRoom::GetInstance().GetTitle())
	{
		pszTitle = CChatRoom::GetInstance().GetTitle();
		if( pszTitle )
			::drawFont( g_GameDATA.m_hFONT[ FONT_NORMAL ], 
			true, 
			30,
			6,
			g_dwWHITE,  
			pszTitle );		
	}	

	if( m_iTab == TAB_MEMBER && !m_bMinimize)
	{
		std::list< CChatMember >::iterator iter;
		int i = 0;
		for( iter = m_Members.begin(); iter != m_Members.end(); ++iter,++i )
		{
			::drawFont( g_GameDATA.m_hFONT[ FONT_NORMAL ], 
				true, 
				c_ptDrawMemberName[i].x,
				c_ptDrawMemberName[i].y,
				g_dwWHITE,  
				iter->GetName() );		
		}
	}

#ifdef __PRIVATECHAT2
	if( strlen(CChatRoom::GetInstance().GetTitle()) && CChatRoom::GetInstance().GetPassword() )
	{		
		SIZE size = getFontTextExtent( g_GameDATA.m_hFONT[ FONT_NORMAL ], pszTitle );
		int iImageID    = CResourceMgr::GetInstance()->GetImageNID( UI_IMAGE_RES_ID, "CHATROOM_LOCK" );
		g_DrawImpl.Draw( m_sPosition.x+size.cx+4+30, m_sPosition.y+6, UI_IMAGE_RES_ID, iImageID );	
	}
#endif
}

void CChatRoomDlg::OnLButtonDown( unsigned uiProcID, WPARAM wParam, LPARAM lParam )
{
	switch( uiProcID )
	{
	case IID_TABBUTTON_MEMBER:
		m_iTab = TAB_MEMBER;
		break;
	case IID_TABBUTTON_CONFIG:
		m_iTab = TAB_CONFIG;
		break;
	default:
		break;
	}
}

void CChatRoomDlg::Update( CObservable* pObservable, CTObject* pObj )
{
	assert( pObj );
	if( pObj == NULL ) return;

	assert( strcmp( pObj->toString(),"ChatRoom" ) == 0 );
	if( strcmp( pObj->toString(),"ChatRoom" ) ) return;

	CTEventChatRoom* pEvent = (CTEventChatRoom*)pObj;
	
	int iServerIdx = pEvent->GetServerIdx();
	switch( pEvent->GetID() )
	{
	case CTEventChatRoom::EID_JOIN_MEMBER:
		{
			CChatMember Member;
			Member.SetName( pEvent->GetName() );
			Member.SetUserTag(pEvent->GetUserTag());
			Member.SetServerIdx( iServerIdx );
			m_Members.push_back( Member );
			break;
		}
	case CTEventChatRoom::EID_LEAVE_MEMBER:
		{
			std::list< CChatMember >::iterator iter;
			for( iter = m_Members.begin(); iter != m_Members.end(); ++iter )
			{
				if( iter->GetServerIdx() == iServerIdx )
				{
					m_Members.erase( iter );
					break;
				}
			}
			break;
		}
	case CTEventChatRoom::EID_SET_MASTER:
		{
			std::list< CChatMember >::iterator iter;
			for( iter = m_Members.begin(); iter != m_Members.end(); ++iter )
			{
				if( iter->GetServerIdx() == iServerIdx )
				{
					CChatMember Member = *iter;
					m_Members.erase( iter );
					m_Members.push_front( Member );
					break;
				}
			}
			break;
		}
	case CTEventChatRoom::EID_STATE_DEACTIVATED:
		break;
	case CTEventChatRoom::EID_STATE_ACTIVATED:
		{
			Show();
			CWinCtrl* pCtrl = FindChildInPane( IID_PANE_TOP, IID_TABBEDPANE );
			if( pCtrl && pCtrl->GetControlType() == CTRL_TABBEDPANE )
			{
				CTabbedPane* pPane = (CTabbedPane*)pCtrl;
				pPane->SetSelectedIndex( TAB_MEMBER );
				m_iTab = TAB_MEMBER;
			}		

			pCtrl = FindChildInPane( IID_PANE_BOTTOM, IID_CHAT_EDIT );
			if( pCtrl->GetControlType() == CTRL_EDITBOX )
			{
				pCtrl->SetFocus(true);
			}
			break;
		}
	default:
		break;
	}
			
}

void CChatRoomDlg::Hide()
{
	CTDialog::Hide();

	CWinCtrl* pCtrl = FindChildInPane( IID_PANE_BOTTOM, IID_CHAT_LIST );
	if( pCtrl )
	{
		if( pCtrl->GetControlType() == CTRL_ZLISTBOX )
		{
			CZListBox* pListBox = (CZListBox*)pCtrl;
			pListBox->Clear();			
		}
		else
		{
			CTListBox* pListBox = (CTListBox*)pCtrl;
			pListBox->ClearText();
		}
	}
	
	CChatRoom::GetInstance().Leave();
}

void CChatRoomDlg::SendChatMsg()
{
	CWinCtrl* pCtrl = FindChildInPane( IID_PANE_BOTTOM, IID_CHAT_EDIT );
	
	if( pCtrl->GetControlType() != CTRL_EDITBOX )
		return;

	CTEditBox* pEditBox = (CTEditBox*)pCtrl;

	if( pEditBox->IsFocus() )
	{
		if( CChatRoom::GetInstance().GetState() == CChatRoom::STATE_ACTIVATED )
		{
			char* pszMsg = pEditBox->get_text();
			if( pszMsg && strlen( pszMsg ) >= 1 )
				g_pNet->Send_cli_CHATROOM_MSG( pszMsg );
		}
		else
		{
			AddChatMsg(pEditBox->get_text(), g_dwWHITE );	
		}
		pEditBox->clear_text();

		///내가 채팅 메세지를 보내면 항상 리스트는 맨밑으로 이동
		CWinCtrl* pCtrl = FindChildInPane( IID_PANE_BOTTOM, IID_CHAT_LIST);
		if( pCtrl )
		{
			if( pCtrl->GetControlType() == CTRL_ZLISTBOX)
			{
				CZListBox* pListBox = (CZListBox*)pCtrl;
				pCtrl = FindChildInPane( IID_PANE_BOTTOM, IID_CHAT_SCROLLBAR );
				if( pCtrl && pCtrl->GetControlType() == CTRL_SCROLLBAR )
				{
					CTScrollBar* pScrollBar = (CTScrollBar*)pCtrl;
					pScrollBar->SetValue( pListBox->GetMaximum() );
				}
			}
			else
			{
				CTListBox* pListBox = (CTListBox*)pCtrl;
				pCtrl = FindChildInPane( IID_PANE_BOTTOM, IID_CHAT_SCROLLBAR );
				if( pCtrl && pCtrl->GetControlType() == CTRL_SCROLLBAR )
				{
					CTScrollBar* pScrollBar = (CTScrollBar*)pCtrl;
					pScrollBar->SetValue( pListBox->GetMaximum() );
				}
			}			
		}
	}
}


void CChatRoomDlg::RecvChatMsg( WORD wUserID, const char* pszMsg )
{
	std::list< CChatMember >::iterator iter;
	for( iter = m_Members.begin(); iter != m_Members.end(); ++iter )
	{
		if( iter->GetServerIdx() == wUserID )
		{
			AddChatMsg( CStr::Printf( "%s> %s", iter->GetName(), pszMsg ),
				g_dwWHITE );
			break;
		}
	}

}
CWinCtrl* CChatRoomDlg::FindChildInPane( unsigned uiPaneID, unsigned uiChildID )
{
	CWinCtrl* pCtrl = Find( uiPaneID );
	if( pCtrl && pCtrl->GetControlType() == CTRL_PANE )
	{
		CTPane* pPane = (CTPane*)pCtrl;
		return pPane->FindChild( uiChildID );
	}
	return NULL;
}

void CChatRoomDlg::Minimize()
{
	CWinCtrl* pCtrl = NULL;
	CWinCtrl* pCtrl2 = NULL;
	m_bMinimize = true;

	if( pCtrl = Find( IID_BTN_MINIMIZE ) )
		pCtrl->Hide();

	if( pCtrl = Find( IID_BTN_MAXIMIZE ) )
		pCtrl->Show();

	if( pCtrl = Find( IID_PANE_TOP ))
	{
		pCtrl->Hide();
	}	

	if( pCtrl = Find( IID_PANE_BOTTOM ) )
	{
		if( pCtrl2 = Find( "PAN_MID_POS" ) )
		{
			pCtrl->SetOffset( pCtrl2->GetOffset() );
			SetHeight( pCtrl2->GetHeight() );
		}		
	}
	
	MoveWindow( m_sPosition );
}

void CChatRoomDlg::Maximize()
{
	CWinCtrl* pCtrl = NULL;
	CWinCtrl* pCtrl2 = NULL;

	m_bMinimize = false;

	if( pCtrl = Find( IID_BTN_MINIMIZE ) )
		pCtrl->Show();

	if( pCtrl = Find( IID_BTN_MAXIMIZE ) )
		pCtrl->Hide();

	if( pCtrl = Find( IID_PANE_TOP ))
	{
		pCtrl->Show();
	}	

	if( pCtrl = Find( IID_PANE_BOTTOM ) )
	{
		if( pCtrl2 = Find( "PAN_BOTTOM_POS" ) )
		{
			pCtrl->SetOffset( pCtrl2->GetOffset() );
			SetHeight( pCtrl2->GetHeight() );
		}		
	}
	

	MoveWindow( m_sPosition );
}

unsigned CChatRoomDlg::ActionPerformed( CActionEvent* e )
{
		assert( e );
	if( e == NULL ) return 0;

	switch( e->GetID() )
	{
	case WM_KEYDOWN:
		{
			if( e->GetWParam() == VK_RETURN )
			{
				CWinCtrl* pCtrl = e->GetSource();
				assert( pCtrl );
				if( pCtrl && pCtrl->IsFocus() && pCtrl->GetControlID() == IID_CHAT_EDIT )
				{
					SendChatMsg();
					return pCtrl->GetControlID();
				}
			}
		}
		break;
	default:
		break;
	}
	return 0;
}

void CChatRoomDlg::SetInterfacePos_After()
{
	CWinCtrl * pCtrl = NULL;

	if( pCtrl = Find("CHATROOM_BG") )
	{
		pCtrl->SetSizeFit(true);
	}

	if( pCtrl = Find("ROOM_NAME") )
	{
		((CTImage*)pCtrl)->SetText( CStringManager::GetSingleton().GetString( 752 ) );	//방이름
	}
	if( pCtrl = Find("ROOM_CAPACITY") )
	{
		((CTImage*)pCtrl)->SetText( CStringManager::GetSingleton().GetString( 753 ) );//인원제한
	}
	if( pCtrl = Find("ROOM_PASSWORD") )
	{
		((CTImage*)pCtrl)->SetText( CStringManager::GetSingleton().GetString( 754 ) );//비밀번호
	}	

	if( pCtrl = Find(IID_TABBUTTON_MEMBER) )
	{
		//898 참가자
		((CTButton*)pCtrl)->SetText( LIST_STRING(898) );
	}	
	if( pCtrl = Find(IID_TABBUTTON_CONFIG) )
	{
		//899 방설정
		((CTButton*)pCtrl)->SetText( LIST_STRING(899) );
	}	
	if( pCtrl = Find(IID_BTN_MAKE) )
	{
		//858 확인
		((CTButton*)pCtrl)->SetText( LIST_STRING(858) );
	}	
}
			
	


