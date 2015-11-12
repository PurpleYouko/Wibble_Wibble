#include "stdafx.h"
#include ".\ccommdlg.h"
#include "../it_mgr.h"
#include "../../Network/CNetwork.h"
#include "subclass/CFriendListItem.h"
#include "subclass/CMailBoxListItem.h"
#include "subclass/CChatRoomListItem.h"
#include "../Command/UICommand.h"
#include "../../SqliteDB.h"
#include "../../Country.h"
#include "../../System/CGame.h"
#include "TRadioButton.h"
#include "TabbedPane.h"
#include "JContainer.h"
#include "ZListBox.h"
#include "TButton.h"
#include "CAddFriendDlg.h"
#include "../JCommandState.h"

CCommDlg::CCommDlg( int iDlgType )
{
	SetDialogType( iDlgType );
}

CCommDlg::~CCommDlg(void)
{
}


void CCommDlg::Show()
{
	CZListBox* pZlist = GetZListBox( TAB_FRIEND, IID_ZLIST_FRIEND );
#ifdef _DEBUG

	std::list<CFriendListItem*> listFriendListItem;
		

	CWinCtrl*	pCtrl = NULL;	
	CFriendListItem* pFriendListItemBuf = NULL;



	assert( pZlist );
	if( pZlist == NULL ) return;


	//프랜드리스트 복사해서 Dlg생성후 다쉬 넣는담.
	CFriendListItem* pItem = NULL;
	for( int i = 0; i < pZlist->GetSize(); ++i )
	{
		pCtrl = pZlist->GetItem( i );
		
		if( pCtrl )
		{
			pItem = ( CFriendListItem* )pCtrl;
			pFriendListItemBuf = pItem->Clone();
			listFriendListItem.push_back( pFriendListItemBuf );
		}
	}
	

	POINT pt = { GetPosition().x, GetPosition().y };
	Clear();
	Create("DlgComm");		
	SetInterfacePos_After();
	MoveWindow(pt);
	

	pZlist = GetZListBox( TAB_FRIEND, IID_ZLIST_FRIEND );
	std::list<CFriendListItem*>::iterator itor;
	itor = listFriendListItem.begin();
	while( itor != listFriendListItem.end() )
	{
		CFriendListItem * pFI = *itor;

		pZlist->Add( pFI );
		
		itor++;
	}	

#endif


	CTDialog::Show();

	LoadMemoFromDB();

	ClearChatRoomList();
	g_pNet->Send_cli_CHAT_ROOM_LIST( 0, 0 );

	//홍근 : 일본 빌링 수정
#ifdef __JAPAN_BILL
	if( !(CGame::GetInstance().GetPayType() & PLAY_FLAG_COMMUNITY) )
	{
		CWinCtrl* pCtrl = Find( IID_TABBEDPANE );

		assert( pCtrl );
		if( pCtrl == NULL ) return;

		assert( pCtrl->GetControlType() == CTRL_TABBEDPANE );
		if( pCtrl->GetControlType() != CTRL_TABBEDPANE ) return;

		CTabbedPane* pTabbedPane = (CTabbedPane*)pCtrl;

		pTabbedPane->SetSelectedIndex( TAB_MAILBOX );
	}
#endif


	pZlist = GetZListBox( TAB_FRIEND, IID_ZLIST_FRIEND );	
	SetRemoveFriendBtn( pZlist );
}

void CCommDlg::Hide()
{
	CTDialog::Hide();
}

//홍근 일본 빌링 수정.
unsigned CCommDlg::Process( unsigned uiMsg, WPARAM wParam, LPARAM lParam )
{
	if( !IsVision() ) return 0;

	int iCurrSelectTab = 0;
	CWinCtrl* pCtrl = Find( IID_TABBEDPANE );
	if( pCtrl && pCtrl->GetControlType() == CTRL_TABBEDPANE )
	{
		CTabbedPane* pTabbedPane = (CTabbedPane*)pCtrl;
		iCurrSelectTab = pTabbedPane->GetSelectedIndex();
	}

	if( unsigned uiProcID = CTDialog::Process( uiMsg, wParam, lParam ) )
	{
		switch( uiMsg )
		{
		case WM_LBUTTONUP:
			OnLButtonUp( uiProcID, wParam, lParam );
			break;
		case WM_LBUTTONDOWN:
			{
				if( uiProcID == IID_TABBUTTON_FRIEND )
				{
#ifdef __JAPAN_BILL
					if( !(CGame::GetInstance().GetPayType() & PLAY_FLAG_COMMUNITY) )
					{
						CWinCtrl* pCtrl = Find( IID_TABBEDPANE );

						assert( pCtrl );
						if( pCtrl == NULL ) return 0;

						assert( pCtrl->GetControlType() == CTRL_TABBEDPANE );
						if( pCtrl->GetControlType() != CTRL_TABBEDPANE ) return 0;

						CTabbedPane* pTabbedPane = (CTabbedPane*)pCtrl;

						pTabbedPane->SetSelectedIndex( iCurrSelectTab );

						g_itMGR.OpenMsgBox( STR_JP_BILL_CANT_USE_FREINDLIST, CMsgBox::BT_OK, false );
					}
#endif						
				}						
				else if( uiProcID == IID_TABBUTTON_CHATROOM )
				{
#ifdef __JAPAN_BILL
					if( !(CGame::GetInstance().GetPayType() & PLAY_FLAG_COMMUNITY) )
					{
						CWinCtrl* pCtrl = Find( IID_TABBEDPANE );

						assert( pCtrl );
						if( pCtrl == NULL ) return 0;

						assert( pCtrl->GetControlType() == CTRL_TABBEDPANE );
						if( pCtrl->GetControlType() != CTRL_TABBEDPANE ) return 0;

						CTabbedPane* pTabbedPane = (CTabbedPane*)pCtrl;

						pTabbedPane->SetSelectedIndex( iCurrSelectTab );

						g_itMGR.OpenMsgBox( STR_JP_BILL_CANT_USE_CHATROOM, CMsgBox::BT_OK, false );

						break;
					}
#endif				
					ClearChatRoomList();
					g_pNet->Send_cli_CHAT_ROOM_LIST( 0, 0 );					

				}
			}
			break;
		default:

			break;
		}
		return uiMsg;
	}
	return 0;
}

void CCommDlg::OnLButtonUp( unsigned uiProcID, WPARAM wParam, LPARAM lParam )
{
	switch( uiProcID )
	{
	case IID_BTN_ICONIZE:
		g_itMGR.AddDialogIcon( 	GetDialogType() );
		break;
	case IID_BTN_REMOVEFRIEND:
	case IID_BTN_REMOVEFRIEND01:
	case IID_BTN_REMOVEFRIEND02:
	case IID_BTN_REMOVEFRIEND03:
	case IID_BTN_REMOVEFRIEND04:
	case IID_BTN_REMOVEFRIEND05:
	case IID_BTN_REMOVEFRIEND06:
	case IID_BTN_REMOVEFRIEND07:
	case IID_BTN_REMOVEFRIEND08:
	case IID_BTN_REMOVEFRIEND09:
		{
			CZListBox* pZListBox = GetZListBox( TAB_FRIEND, IID_ZLIST_FRIEND );
			assert( pZListBox );
			
			if( (uiProcID - IID_BTN_REMOVEFRIEND)< pZListBox->GetSize() )
			{
				//06. 11. 23 - 김주현 : 10번째 초과한 목록에 대한 삭제가 안되는 버그 수정을 위해서 GetValue 추가.
				CWinCtrl* pCtrl = pZListBox->GetItem( uiProcID - IID_BTN_REMOVEFRIEND + pZListBox->GetValue());
				if( pCtrl )
				{
					CFriendListItem* pItem = (CFriendListItem*)pCtrl;
					CTCmdRemoveFriend* pCmd = new CTCmdRemoveFriend( pItem->GetUserTag() );

					g_itMGR.OpenMsgBox( CStr::Printf(F_STR_QUERY_DELETE_FRIEND, pItem->GetName() ),
						CMsgBox::BT_OK | CMsgBox::BT_CANCEL, true, 0, pCmd, NULL );
				}
			}

			break;
		}
	case IID_BTN_ADDFRIEND:
		{
			CAddFriendDlg * pDlg = (CAddFriendDlg*)g_itMGR.OpenDialog( DLG_TYPE_ADDFRIEND );	
			CObjCHAR *pObj = (CObjCHAR*)g_pObjMGR->Get_CharOBJ( g_UserInputSystem.GetCurrentTarget(), true );			
			if( pObj )
			{	
				pDlg->SetFriendName( pObj->Get_NAME() );				
			}			
		}
		break;
	case IID_BTN_CREATECHATROOM:
		if( !g_itMGR.IsDlgOpened( DLG_TYPE_CHATROOM ) )
			g_itMGR.OpenDialog( DLG_TYPE_CHATROOM );
		break;
	case IID_BTN_CLOSE:
		Hide();
		break;
	case IID_BTN_CLEAR_MAIL:
		ClearMemoList();
		break;
	default:
		break;
	}
}
void CCommDlg::AddFriend( DWORD dwUserTag, BYTE btStatus, const char* pszName )
{
	CZListBox* pZlist = GetZListBox( TAB_FRIEND, IID_ZLIST_FRIEND );
	assert( pZlist );
	if( pZlist == NULL ) return;

	CFriendListItem* pItem = new CFriendListItem( dwUserTag, btStatus, pszName );

	CWinCtrl * pCtrl = NULL;
	if( pCtrl = Find("FRIEND_LIST_ITEM") )
	{	
		pItem->SetWidth( pCtrl->GetWidth() );
		pItem->SetHeight( pCtrl->GetHeight() );
	}	

	pZlist->Add( pItem );

	SetRemoveFriendBtn( pZlist );
}

void CCommDlg::ClearFriendList()
{
	CZListBox* pZlist = GetZListBox( TAB_FRIEND, IID_ZLIST_FRIEND );
	assert( pZlist );
	if( pZlist == NULL ) return;

	pZlist->Clear();

}
void CCommDlg::ChangeFriendStatus( DWORD dwUserTag, BYTE btStatus )
{
	CWinCtrl*	pCtrl = NULL;	
	CZListBox* pZlist = GetZListBox( TAB_FRIEND, IID_ZLIST_FRIEND );
	assert( pZlist );
	if( pZlist == NULL ) return;

	CFriendListItem* pItem = NULL;
	for( int i = 0; i < pZlist->GetSize(); ++i )
	{
		pCtrl = pZlist->GetItem( i );
		assert( pCtrl );
		if( pCtrl )
		{
			pItem = ( CFriendListItem* )pCtrl;
			if( pItem->GetUserTag() == dwUserTag )
			{
				pItem->SetStatus( btStatus );
				break;
			}
		}
	}
}

CZListBox* CCommDlg::GetZListBox( int iTab, int iListID )
{
	CWinCtrl* pCtrl = Find( IID_TABBEDPANE );

	assert( pCtrl );
	if( pCtrl == NULL ) return NULL;
	
	assert( pCtrl->GetControlType() == CTRL_TABBEDPANE );
	if( pCtrl->GetControlType() != CTRL_TABBEDPANE ) return NULL;

	CTabbedPane* pTabbedPane = (CTabbedPane*)pCtrl;

	CJContainer* pContainer = pTabbedPane->GetTabContainer( iTab );
	assert( pContainer );
	if( pContainer == NULL ) return NULL;
	
	pCtrl = pContainer->Find( iListID );
	assert( pCtrl );
	if( pCtrl == NULL ) return NULL;
	
	assert( pCtrl->GetControlType() == CTRL_ZLISTBOX );
	if( pCtrl->GetControlType() != CTRL_ZLISTBOX ) return NULL;
	
	return (CZListBox*)pCtrl;
}

void CCommDlg::RemoveFriend( DWORD dwUserTag )
{
	CWinCtrl*	pCtrl = NULL;	
	CZListBox* pZlist = GetZListBox( TAB_FRIEND, IID_ZLIST_FRIEND );
	assert( pZlist );
	if( pZlist == NULL ) return;

	CFriendListItem* pItem = NULL;
	for( int i = 0; i < pZlist->GetSize(); ++i )
	{
		pCtrl = pZlist->GetItem( i );
		assert( pCtrl );
		if( pCtrl )
		{
			pItem = ( CFriendListItem* )pCtrl;
			if( pItem->GetUserTag() == dwUserTag )
			{
				pZlist->DelItem( i );
				break;
			}
		}
	}

	SetRemoveFriendBtn( pZlist );	
}

CFriendListItem* CCommDlg::FindFriend( DWORD dwUserTag )
{
	CWinCtrl*	pCtrl = NULL;	
	CZListBox* pZlist = GetZListBox( TAB_FRIEND, IID_ZLIST_FRIEND );
	assert( pZlist );
	if( pZlist == NULL ) return NULL;

	CFriendListItem* pItem;
	for( int i = 0; i < pZlist->GetSize(); ++i )
	{
		pCtrl = pZlist->GetItem( i );
		assert( pCtrl );
		if( pCtrl )
		{
			pItem = (CFriendListItem*)pCtrl;
			if( pItem->GetUserTag() == dwUserTag )
				return pItem;
		}
	}
	return NULL;
}
CFriendListItem* CCommDlg::FindFriendByName( const char* pszName )
{
	assert( pszName );
	if( pszName == NULL ) return NULL;

	CWinCtrl*	pCtrl = NULL;	
	CZListBox* pZlist = GetZListBox( TAB_FRIEND, IID_ZLIST_FRIEND );
	assert( pZlist );
	if( pZlist == NULL ) return NULL;

	CFriendListItem* pItem;
	for( int i = 0; i < pZlist->GetSize(); ++i )
	{
		pCtrl = pZlist->GetItem( i );
		assert( pCtrl );
		if( pCtrl )
		{
			pItem = (CFriendListItem*)pCtrl;
			if( _strcmpi( pszName, pItem->GetName() ) == 0 )
				return pItem;
		}
	}
	return NULL;
}



void CCommDlg::ClearMemoList()
{
	CZListBox* pZlist = GetZListBox( TAB_MAILBOX, IID_ZLIST_MAILBOX );
	assert( pZlist );
	if( pZlist == NULL ) return;

	pZlist->Clear();
}

void CCommDlg::ClearChatRoomList()
{
	CZListBox* pZlist = GetZListBox( TAB_CHATROOM, IID_ZLIST_CHATROOM );
	assert( pZlist );
	if( pZlist == NULL ) return;

	pZlist->Clear();
}

void CCommDlg::AddMemo( int iRowid, const char* pszFromName, const char* pszMemo , DWORD dwRecvTime )
{
	CZListBox* pZlist = GetZListBox( TAB_MAILBOX, IID_ZLIST_MAILBOX );
	assert( pZlist );
	if( pZlist == NULL ) return;


	CMailBoxListItem* pItem = new CMailBoxListItem( pszFromName, pszMemo, dwRecvTime  );
	pZlist->Add( pItem );
	pItem->SetControlID( iRowid );
}

void CCommDlg::DeleteMemo( int iIndex )
{
	CZListBox* pZlist = GetZListBox( TAB_MAILBOX, IID_ZLIST_MAILBOX );
	assert( pZlist );
	if( pZlist == NULL ) return;

	pZlist->DelItemByControlID( iIndex );
}
void CCommDlg::AddChatRoom( const char* pszTitle, BYTE btRoomType, short nRoomID, BYTE btUserCount )
{
	CZListBox* pZList = GetZListBox( TAB_CHATROOM, IID_ZLIST_CHATROOM );
	assert( pZList );
	if( pZList == NULL ) return;

	CChatRoomListItem* pItem = NULL;
	pItem = new CChatRoomListItem( nRoomID, btRoomType, btUserCount, pszTitle );
	
	pZList->Add( pItem );

}

void CCommDlg::LoadMemoFromDB()
{
	ClearMemoList();
	CSqliteDB::GetInstance().LoadMemo( g_pAVATAR->Get_NAME(), callback );
}

int CCommDlg::callback(void *NotUsed, int argc, char **argv, char **azColName)
{
	assert( argc == 4 );
	if( argc == 4 )
	{
		if( CTDialog* pDlg = g_itMGR.FindDlg( DLG_TYPE_COMMUNITY ) )
		{
			CCommDlg* pCommDlg = (CCommDlg*)pDlg;
			pCommDlg->AddMemo( atoi(argv[0]), argv[1], argv[2], atoi( argv[3] ) );
		}
	}
	return 0;
}

void CCommDlg::SetInterfacePos_After()
{
	CWinCtrl * pCtrl = NULL;
	if( pCtrl = Find("MAIL_MAIN") )
	{
        pCtrl->SetSizeFit(true);
	}
	if( pCtrl = Find("CHATROOM_MAIN") )
	{
		pCtrl->SetSizeFit(true);
	}
	if( pCtrl = Find("FRIEND_MAIN") )
	{
		pCtrl->SetSizeFit(true);
	}

	//문자열 추가.
	if( pCtrl = Find(IID_TABBUTTON_FRIEND) )
	{	
		//친구 880
		((CTRadioButton*)pCtrl)->SetText( LIST_STRING(880) );
	}
	if( pCtrl = Find(IID_TABBUTTON_CHATROOM) )
	{	
		//대화방 881
		((CTRadioButton*)pCtrl)->SetText( LIST_STRING(881) );
	}
	if( pCtrl = Find(IID_TABBUTTON_MAILBOXM) )
	{	
		//우편함 882
		((CTRadioButton*)pCtrl)->SetText( LIST_STRING(882) );
	}
	if( pCtrl = Find(IID_BTN_ADDFRIEND) )
	{	
		//친구 추가하기 883
		((CTButton*)pCtrl)->SetText( LIST_STRING(883) );
		((CTButton*)pCtrl)->SetTextColor( g_dwWHITE );
		pCtrl->SetFont(FONT_NORMAL);
	}
	if( pCtrl = Find(IID_BTN_CREATECHATROOM) )
	{	
		//대화방 만들기 884
		((CTButton*)pCtrl)->SetText( LIST_STRING(884) );
		((CTButton*)pCtrl)->SetTextColor( g_dwWHITE );
		pCtrl->SetFont(FONT_NORMAL);
	}
	if( pCtrl = Find(IID_BTN_CLEAR_MAIL) )
	{	
		//전체 메시지 삭제 885
		((CTButton*)pCtrl)->SetText( LIST_STRING(885) );
		((CTButton*)pCtrl)->SetTextColor( g_dwWHITE );
		pCtrl->SetFont(FONT_NORMAL);
	}

	//커뮤니티 25
	GetCaption()->SetString( LIST_STRING(25) );
}


void CCommDlg::SetRemoveFriendBtn( CZListBox * pZList )
{	
	if( pZList == NULL ) return;

	CWinCtrl * pCtrl = NULL;
	
	int i = 0;
	
	for( i = 0 ; i < pZList->GetExtent(); ++i )
	{
		if( pCtrl = Find(IID_BTN_REMOVEFRIEND + i) )
		{
			if( i < pZList->GetSize() )
			{
				pCtrl->Show();
			}
			else
			{
				pCtrl->Hide();
			}
		}		
	}	
}


void CCommDlg::Update( POINT ptMouse )
{
	if( !IsVision() ) return;
	CTDialog::Update( ptMouse );
}






