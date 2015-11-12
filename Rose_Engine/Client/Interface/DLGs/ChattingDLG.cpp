#include "StdAfx.h"
#include "ChattingDLG.h"
#include "CTargetMenu.h"
#include "ExchangeDlg.h"
#include "CPartyDlg.h"
#include "CCommDlg.h"
#include "CItemDlg.h"
#include "../it_mgr.h"
#include "../CToolTipMgr.h"
#include "../../GameData/CExchange.h"
#include "../../Network/CNetwork.h"
#include "../../GameCommon/CFilterWord.h"
#include "../../GameCommon/Skill.h"
#include "../Command/CTCmdHotExec.h"

#include "../../GameData/CParty.h"
#include "../../Util/Localizing.h"
#include "../../GameProc/CaptureChatList.h"
#include "TCaption.h"
#include "TEditBox.h"
#include "TListBox.h"
#include "ZListBox.h"
#include "TRadioBox.h"
#include "TScrollBar.h"
#include "ActionEvent.h"
#include "TControlMgr.h"
#include "TImage.h"
#include "TRadioButton.h"
#include <algorithm>
#include ".\cclientstorage.h"
#include "system/CGAME.h"


int CChatDLG::m_iCaptureCount = 0;

CChatDLG::CChatDLG()
{

	m_bChatBlock			= false;
	m_dwChatBlockStartTime	= 0;
	m_dwChatBlockTime		= 0;

	m_bCheckShoutRestrict	= true;
	m_dwShoutRestrictTime   = 20 * 1000;
	m_dwPrevUseShoutTime	= 0;

	m_iSelectedTab			= TAB_ALL;

	ChatFilter filter;
	filter.Filters[0] = 1;///NORMAL
	filter.Filters[1] = 1;///SYSTEM
	filter.Filters[2] = 1;///TRADE
	filter.Filters[3] = 1;///PARTY
	filter.Filters[4] = 1;///CLAN
	filter.Filters[5] = 1;///ALLIED,whisper
	filter.Filters[6] = 1;///shout

	m_Filters.push_back( filter );///All

	filter.Filters[0] = 0;
	filter.Filters[1] = 1;
	filter.Filters[2] = 0;
	filter.Filters[3] = 0;
	filter.Filters[4] = 0;
	filter.Filters[5] = 0;
	filter.Filters[6] = 0;
	m_Filters.push_back( filter );///Whisper


	filter.Filters[0] = 0;
	filter.Filters[1] = 1;
	filter.Filters[2] = 2;
	filter.Filters[3] = 0;
	filter.Filters[4] = 0;
	filter.Filters[5] = 0;
	filter.Filters[6] = 0;
	m_Filters.push_back( filter );///Trade

	filter.Filters[0] = 0;
	filter.Filters[1] = 1;
	filter.Filters[2] = 0;
	filter.Filters[3] = 2;
	filter.Filters[4] = 0;
	filter.Filters[5] = 0;
	filter.Filters[6] = 0;
	m_Filters.push_back( filter );///Party

	filter.Filters[0] = 0;
	filter.Filters[1] = 1;
	filter.Filters[2] = 0;
	filter.Filters[3] = 0;
	filter.Filters[4] = 2;
	filter.Filters[5] = 0;
	filter.Filters[6] = 0;
	m_Filters.push_back( filter );///Clan


	filter.Filters[0] = 0;
	filter.Filters[1] = 1;
	filter.Filters[2] = 0;
	filter.Filters[3] = 0;
	filter.Filters[4] = 0;
	filter.Filters[5] = 2;
	filter.Filters[6] = 0;
	m_Filters.push_back( filter );///Allied


	CFilterWord& Util = CFilterWord::GetInstance();
	std::wstring	wstrTemp;


	///ASCii
	Util.MulityByte2WideString("!", wstrTemp ); m_ShoutCommands.push_back( wstrTemp );
	Util.MulityByte2WideString("~", wstrTemp ); m_AlliedCommands.push_back( wstrTemp );

	Util.MulityByte2WideString("$", wstrTemp ); m_TradeCommands.push_back( wstrTemp );	
	Util.MulityByte2WideString("@", wstrTemp ); m_ClanCommands.push_back( wstrTemp );
	Util.MulityByte2WideString("#", wstrTemp ); m_PartyCommands.push_back( wstrTemp );
	
	Util.MulityByte2WideString("\"", wstrTemp ); m_WhisperCommands.push_back( wstrTemp );
		
	Util.MulityByte2WideString( CStr::Printf("/%s", STR_HELP), wstrTemp); m_HelpCommands.push_back( wstrTemp );
	Util.MulityByte2WideString( " ", wstrTemp); m_Spaces.push_back( wstrTemp );

	/// 2바이트 특문/ 일본어 전각시에도 처리가능하도록 추가한다.
	wchar_t wchar[2] = { 0, 0 };
	wchar[0] = 0xFF01; m_ShoutCommands.push_back( wchar );
	wchar[0] = 0xFF04; m_TradeCommands.push_back( wchar );
	wchar[0] = 0xFF20; m_WhisperCommands.push_back( wchar );
	wchar[0] = 0xFF06; m_ClanCommands.push_back( wchar );
	wchar[0] = 0xFF03; m_PartyCommands.push_back( wchar );
	wchar[0] = 0xFF5E; m_AlliedCommands.push_back( wchar );
	wchar[0] = 0x3000; m_Spaces.push_back( wchar );

	wchar[0] = 0xFF0F;
	std::wstring wstrHelp = wchar;
	Util.MulityByte2WideString( STR_HELP, wstrTemp); 
	wstrHelp.append( wstrTemp );
	m_HelpCommands.push_back( wstrHelp );



	/// 5초안에 3번만 보낼수 있다
	m_iCheckChatCount	= 3;///3
	m_dwCheckChatTime	= 5 * 1000;///

	m_iChatSizeMode = CHATSIZE_MODE_MIDDLE;
	m_iShowSystemChat = 1;

}

CChatDLG::~CChatDLG()
{
}


void CChatDLG::Update( POINT ptMouse )
{
	CTDialog::Update( ptMouse );

	///이하 툴팁 표시
	CTDialog* pDlg = CTDialog::GetProcessMouseOverDialog();

	if( pDlg && pDlg != this ){		return ;	}


	//문자열 추가
	//채팅창크기조절 
	std::string strBuf = LIST_STRING(886);
	CWinCtrl * pCtrl = NULL;
	if( pCtrl = Find(IID_BTN_CHANGE_SIZE) )
	{
		if( pCtrl->IsInside(ptMouse.x, ptMouse.y) )
		{
			CToolTipMgr::GetInstance().RegToolTip( ptMouse.x-20, ptMouse.y-20, strBuf.c_str() );
		}
	}	
}

unsigned int CChatDLG::Process( UINT uiMsg,WPARAM wParam,LPARAM lParam )
{
	unsigned iProcID = 0;

	CWinCtrl * pCtrl = NULL;
	if( pCtrl = Find("MOVE_TAB") )
	{
		pCtrl->Hide();
	}

	if( iProcID = CTDialog::Process(uiMsg,wParam,lParam)) 
	{
		switch(uiMsg)
		{
		case WM_MOUSEWHEEL:			
			return uiMsg;
			break;

		case WM_LBUTTONUP:
			{
				if(On_LButtonUP( iProcID ))
					return uiMsg;
			}
			break;
		case WM_LBUTTONDOWN:
			{
				OnLButtonDown( iProcID, wParam , lParam );
				
				if(IsInside4Move( LOWORD(lParam), HIWORD(lParam ) ) )
				{
					return true;
				}

				if( iProcID == 1 )///Dialog를 Click했다면  이동가능하도록
					return 0;
			}
		case WM_MOUSEMOVE:
			{				
				SetMoveTab( lParam );
			}
			break;
		}	
		return uiMsg;

	}

	switch(uiMsg)
	{
	case WM_KEYDOWN:

		switch( wParam)
		{
		case VK_SPACE:
			{
				//20050902 홍근 2인승 카트 : 스페이스바로 내가 보조석에 탔을 때 내리기.
				if( CTControlMgr::GetInstance()->GetKeyboardInputType() == CTControlMgr::INPUTTYPE_NORMAL 
					&& NULL == CTEditBox::s_pFocusEdit 
					&& g_pAVATAR->IsRideUser() )
				{
					g_pNet->Send_cli_TOGGLE( TOGGLE_TYPE_DRIVE );
				}
			}
			break;
		}
	}

	return 0;
}

void CChatDLG::SetMoveTab( LPARAM lParam )
{
	CWinCtrl * pCtrl = NULL;

	if( pCtrl = Find("CHAT_BG") )
	{
		if( pCtrl->IsInside( LOWORD(lParam), HIWORD(lParam ) ) )
		{
			if( pCtrl = Find("MOVE_TAB") )
			{
				pCtrl->Show();						
				return;
			}
		}
	}		
	if( pCtrl = Find("CHAT_BG_S") )
	{
		if( pCtrl->IsInside( LOWORD(lParam), HIWORD(lParam ) ) )
		{
			if( pCtrl = Find("MOVE_TAB") )
			{
				pCtrl->Show();						
				return;
			}
		}
	}
	if( pCtrl = Find("MOVE_TAB") )
	{
		pCtrl->Hide();
	}
}

void CChatDLG::SetChangeSize(bool bChange)
{
	CWinCtrl * pCtrl = NULL;
	CWinCtrl * pCtrl2 = NULL;

	int iPreChatSizeMode = m_iChatSizeMode; 

	if( bChange )
	{
		m_iChatSizeMode++;
	}
	

	if( m_iChatSizeMode >= CHATSIZE_MODE_SIZE )
	{
		m_iChatSizeMode = CHATSIZE_MODE_SMALL;
	}

	if( pCtrl = Find("CHAT_BG") )
	{
		pCtrl->SetSizeFit(true);
		pCtrl->SetWidth(m_ChatSize[0][m_iChatSizeMode].width);
		pCtrl->SetHeight(m_ChatSize[0][m_iChatSizeMode].height);

		if( m_iChatSizeMode == CHATSIZE_MODE_SMALL)
		{
			pCtrl->Hide();
		}
		else
		{
			pCtrl->Show();
		}
	}

	if(pCtrl = Find("SYSTEM_POS"))
	{
		if( pCtrl2 = Find("CHAT_BG_S") )
		{
			pCtrl2->SetSizeFit(true);
			pCtrl2->SetWidth( pCtrl->GetWidth() );
			pCtrl2->SetHeight( pCtrl->GetHeight() );
		}	
	}
	if(pCtrl = Find("CHAT_BOTTOM_PANE"))
	{
		pCtrl->SetOffset(0, m_ChatSize[0][m_iChatSizeMode].height + m_ChatSize[0][m_iChatSizeMode].pt.y );
	}


	for(int i=0 ; i<6; i++)
	{
		if( pCtrl = Find( CStr::Printf( "SCROLLBAR%02d", i )) )
		{
            pCtrl->SetHeight( m_ChatSize[0][m_iChatSizeMode].height - pCtrl->GetOffset().y );			
		}		
		if( pCtrl = Find( CStr::Printf( "LISTBOX%02d", i )) )
		{
			pCtrl->SetHeight( m_ChatSize[0][m_iChatSizeMode].height  );
			((CTListBox*)pCtrl)->SetExtent( m_ChatSize[0][m_iChatSizeMode].height / ((CTListBox*)pCtrl)->GetLineHeight() );
		}
	}
	
	ActiveListBoxMoveEnd();

	MoveWindow( GetPosition().x,
				GetPosition().y - (m_ChatSize[0][m_iChatSizeMode].height - m_ChatSize[0][iPreChatSizeMode].height) );	
	

}

bool CChatDLG::On_LButtonUP( unsigned iProcID )
{
	switch( iProcID )
	{
	case IID_BTN_CHANGE_SIZE:
		{
			SetChangeSize();
			SetSystemChat();
		}
		break;
	case IID_BTN_FILTER:
		g_itMGR.OpenDialog( DLG_TYPE_CHATFILTER );

		{
			CWinCtrl * pCtrl = NULL;
			CTDialog * pDlg = g_itMGR.FindDlg( DLG_TYPE_CHATFILTER );
			if(pDlg)
			{
				if(pCtrl = Find(iProcID))
				{
					pDlg->MoveWindow(	GetPosition().x,
										GetPosition().y - pDlg->GetHeight() );
				}
			}			
		}

		break;
	default:
		break;
	}

	return true;
}

void CChatDLG::Show()
{
	CTDialog::Show();

	if( it_GetKeyboardInputType() == CTControlMgr::INPUTTYPE_AUTOENTER )
	{
		CWinCtrl* pCtrl = Find( IID_EDITBOX );
		if( pCtrl && pCtrl->GetControlType() == CTRL_EDITBOX )
			pCtrl->SetFocus( true );
	}
	TabSelected( TAB_ALL  );

	m_iChatSizeMode = g_ClientStorage.GetChatDlgType();
	SetChangeSize(false);
	SetSystemChat();
}

void CChatDLG::SendChatMsg( char* szMsg )
{
	if( szMsg == NULL )
		return;

	if( strlen( szMsg ) < 1  )
		return;

	m_strLastSendMsg = szMsg;


	ActiveListBoxMoveEnd();


	if( IsChatBlock() )
	{
		g_itMGR.AppendChatMsg(STR_STATE_BLOCK_CHAT,IT_MGR::CHAT_TYPE_SYSTEM);	
		return ;
	}
	string stMsg = szMsg;
	string stTargetID;
	string stRealMsg;

	int iChatType = ChatParser( stMsg, stRealMsg, stTargetID );

	///위치에 주의할껏 szMsg의 포인터가 파괴될수 있다.
	CWinCtrl* pCtrl = Find(IID_EDITBOX);
	if( pCtrl == NULL ) return;
	if( pCtrl->GetControlType() != CTRL_EDITBOX ) return;

	CTEditBox* pEditBox = (CTEditBox*) pCtrl;
	pEditBox->clear_text();


	if( !CFilterWord::GetInstance().CheckString( (char*)stRealMsg.c_str() ) )
		g_itMGR.AppendChatMsg(STR_FILTER_BADWORD,IT_MGR::CHAT_TYPE_SYSTEM );

	stRealMsg = CFilterWord::GetInstance().GetChangedString();



	DWORD dwCurrentTime = g_GameDATA.GetTime();

	if( m_bCheckShoutRestrict )//&& ( dwCurrentTime - m_dwPrevSendChatTime) < m_dwChatRestrictTime )
	{
		if( m_PrevSendMessageTimes.size() >= (unsigned int)m_iCheckChatCount )
		{
			DWORD prev_time = m_PrevSendMessageTimes.front();//
			if( dwCurrentTime - prev_time < m_dwCheckChatTime )
			{
				g_itMGR.AppendChatMsg( STR_CHAT_RESTRICT, IT_MGR::CHAT_TYPE_SYSTEM );
#ifdef _DEBUG
#else
				return;
#endif

			}
			m_PrevSendMessageTimes.pop();
		}
		m_PrevSendMessageTimes.push( dwCurrentTime );
	}

	//m_dwPrevSendChatTime = dwCurrentTime;

	CFilterWord& Util = CFilterWord::GetInstance();

	switch( iChatType )
	{
	case CHAT_ALLIED:
		if( g_pAVATAR && g_pAVATAR->Get_TeamNO() != 2 )///Defalt Team No일경우에는 동맹쳇을 보내지 않는다
			g_pNet->Send_cli_ALLIED_SHOUT( (char*)stRealMsg.c_str() );
		break;
	case CHAT_OPEN_HELP:
		break;
	case CHAT_CLAN:
		g_pNet->Send_cli_CLAN_CHAT( (char*)stRealMsg.c_str() );
		break;
	case CHAT_PARTY:
		g_pNet->Send_cli_PARTY_CHAT((char*)stRealMsg.c_str());
		break;
	case CHAT_NORMAL:
		g_pNet->Send_cli_CHAT( (char*)stRealMsg.c_str()  );
		break;
	case CHAT_SHOUT:
		if( m_bCheckShoutRestrict && ( dwCurrentTime - m_dwPrevUseShoutTime) < m_dwShoutRestrictTime )
		{
			g_itMGR.AppendChatMsg(
				CStr::Printf(F_STR_RESTRICT_SHOUT, m_dwShoutRestrictTime / 1000), IT_MGR::CHAT_TYPE_SYSTEM );
			break;			
		}
		g_pNet->Send_cli_SHOUT( (char*)stRealMsg.c_str() );
		m_dwPrevUseShoutTime = g_GameDATA.GetTime();
		break;
	case CHAT_WHISPER:
		{
			///서버에 날리고
			g_pNet->Send_cli_WHISPER(  (char*)stTargetID.c_str(), (char*)stRealMsg.c_str() );

			///자신의 채팅창에 메세지 뿌리고
			string Temp = g_pAVATAR->Get_NAME();
			Temp.append(">");
			Temp.append( stMsg );
			g_itMGR.AppendChatMsg( Temp.c_str(), IT_MGR::CHAT_TYPE_WHISPER);
			///에디트박스에 @ID를 추가한다.
			pEditBox = (CTEditBox*)pCtrl;

			std::string stTemp;
			Util.Wide2MultyByteString( m_WhisperCommands[0], stTemp );			

			stTemp.append( stTargetID );
			pEditBox->AppendText( (char*)stTemp.c_str() );
			pEditBox->AppendText(" ");

			break;
		}
	case CHAT_EXCHANGE:
		{
			CObjCHAR* pObjChar = g_pObjMGR->Get_ClientCharOBJByName( stTargetID.c_str() );
			if( pObjChar )
			{
				if( CExchange::GetInstance().SendCliTradeReq( g_pObjMGR->Get_ServerObjectIndex(pObjChar->Get_INDEX()) ) )
				{
					///자신의 채팅창에 메세지 뿌리고
					string Temp = stTargetID;
					Temp.append(STR_REQ_TRADE);
					g_itMGR.AppendChatMsg( Temp.c_str(), IT_MGR::CHAT_TYPE_SYSTEM);
				}

			}
			break;
		}
	default:
		g_pNet->Send_cli_CHAT( (char*)stRealMsg.c_str() );
		break;
	}
}

int	 CChatDLG::ChatParser( string stMsg, string& stRealMsg ,string& stTargetID )
{
	const char*	pszMsg = stMsg.c_str();

	CFilterWord& Util = CFilterWord::GetInstance();
	std::wstring wstrMsg;
	Util.MulityByte2WideString( pszMsg, wstrMsg );


	std::vector< std::wstring >::iterator iter;
	std::wstring::size_type index;


	for( iter = m_HelpCommands.begin(); iter != m_HelpCommands.end(); ++iter )
	{
		index = wstrMsg.find( *iter );
		if( index == 0 )
			return CHAT_OPEN_HELP;
	}

	if( wstrMsg.length() < 2 )
	{
		stRealMsg = stMsg;	
		return CHAT_NORMAL;
	}


	for( iter = m_ShoutCommands.begin(); iter != m_ShoutCommands.end(); ++iter )
	{
		index = wstrMsg.find( *iter );
		if( index == 0 )
		{
			std::wstring wstrRealMsg = wstrMsg.substr( 1, wstrMsg.size() - 1 );
			Util.Wide2MultyByteString( wstrRealMsg, stRealMsg );
			return CHAT_SHOUT;
		}
	}

	for( iter = m_AlliedCommands.begin(); iter != m_AlliedCommands.end(); ++iter )
	{
		index = wstrMsg.find( *iter );
		if( index == 0 )
		{
			std::wstring wstrRealMsg = wstrMsg.substr( 1, wstrMsg.size() - 1 );
			Util.Wide2MultyByteString( wstrRealMsg, stRealMsg );
			return CHAT_ALLIED;
		}
	}

	for( iter = m_TradeCommands.begin(); iter != m_TradeCommands.end(); ++iter )
	{
		index = wstrMsg.find( *iter );
		if( index == 0 )
		{
			std::wstring wstrRealMsg = wstrMsg.substr( 1, wstrMsg.size() - 1 );
			Util.Wide2MultyByteString( wstrRealMsg, stRealMsg );
			return CHAT_TRADE;
		}
	}


	for( iter = m_ClanCommands.begin(); iter != m_ClanCommands.end(); ++iter )
	{
		index = wstrMsg.find( *iter );		
		if( index == 0 )
		{
			std::wstring wstrRealMsg = wstrMsg.substr( 1, wstrMsg.size() - 1 );
			Util.Wide2MultyByteString( wstrRealMsg, stRealMsg );
			return CHAT_CLAN;
		}
	}



	if( CParty::GetInstance().HasParty() )
	{
		for( iter = m_PartyCommands.begin(); iter != m_PartyCommands.end(); ++iter )
		{
			index = wstrMsg.find( *iter );
			if( index == 0 )
			{
				std::wstring wstrRealMsg = wstrMsg.substr( 1, wstrMsg.size() - 1 );
				Util.Wide2MultyByteString( wstrRealMsg, stRealMsg );
				return CHAT_PARTY;
			}
		}
	}


	for( iter = m_WhisperCommands.begin(); iter != m_WhisperCommands.end(); ++iter )
	{
		wstring wstrBuf = *iter;
		
		index = wstrMsg.find( *iter );
		if( index == 0 )
		{


			for( iter = m_Spaces.begin(); iter != m_Spaces.end(); ++iter )
			{
				index = wstrMsg.find( *iter );
				if( index != std::wstring::npos )
				{
					if( index >= 2 && index < wstrMsg.length() )
					{
						std::wstring wstrTargetID = wstrMsg.substr( 1, index - 1 );
						Util.Wide2MultyByteString( wstrTargetID, stTargetID );

						std::wstring wstrRealMsg = wstrMsg.substr( index, wstrMsg.length() - index );
						Util.Wide2MultyByteString( wstrRealMsg, stRealMsg );
						return CHAT_WHISPER;
					}
					else
					{
						break;
					}
				}
			}
			break;
		}
	}

	stRealMsg = stMsg;	
	return CHAT_NORMAL;
}

void CChatDLG::SetChatBlock( DWORD BlockTime )
{
	m_bChatBlock = true;
	m_dwChatBlockStartTime	= g_GameDATA.GetGameTime();
	m_dwChatBlockTime		= BlockTime;
}

void CChatDLG::SetChatUnBlock()
{
	m_bChatBlock			= false;
	m_dwChatBlockStartTime	= 0;
	m_dwChatBlockTime		= 0;

}

bool CChatDLG::IsChatBlock()
{
	if( m_bChatBlock && timeGetTime() - m_dwChatBlockStartTime >= m_dwChatBlockTime )
		SetChatUnBlock();

	return m_bChatBlock;
}

/// 상황에 따라 List와 ScrollBar의 Scroll을 구분한다.
void CChatDLG::AppendMsg( const char* pszMsg, DWORD color, int iType )
{
	if( iType == FILTER_SYSTEM )
	{
		AppendMsg2System( pszMsg, color);
		return;
	}

	AppendMsg2All( pszMsg, color , iType );
	AppendMsg2Whisper( pszMsg, color , iType );
	AppendMsg2Trade( pszMsg, color , iType );
	AppendMsg2Party( pszMsg, color , iType );
	AppendMsg2Clan( pszMsg, color , iType );
	AppendMsg2Allied( pszMsg, color , iType );

}



void CChatDLG::SetEnableShoutRestrict()
{
	m_bCheckShoutRestrict = true;
}

void CChatDLG::SetDisableShoutRestrict()
{
	m_bCheckShoutRestrict = false;
}

void CChatDLG::SendChatMsgRepeat()
{
	if( !m_strLastSendMsg.empty() )
		SendChatMsg( (char*)m_strLastSendMsg.c_str() );
}

void CChatDLG::TabSelected( int iTab )
{
	HideChild( "SELECTED_ALL" );
	HideChild( "SELECTED_TRADE" );
	HideChild( "SELECTED_CLAN" );
	HideChild( "SELECTED_PARTY" );
	HideChild( "SELECTED_WHISPER" );

	m_iSelectedTab = iTab;
	switch( iTab )
	{
	case TAB_ALL:
		{
			ShowChild( IID_LISTBOX_ALL );
			ShowChild( IID_SCROLLBAR_ALL );
			HideChild( IID_LISTBOX_WHISPER );
			HideChild( IID_SCROLLBAR_WHISPER );
			HideChild( IID_LISTBOX_TRADE );
			HideChild( IID_SCROLLBAR_TRADE );
			HideChild( IID_LISTBOX_PARTY );
			HideChild( IID_SCROLLBAR_PARTY );
			HideChild( IID_LISTBOX_CLAN );
			HideChild( IID_SCROLLBAR_CLAN );
			HideChild( IID_LISTBOX_ALLIED );
			HideChild( IID_SCROLLBAR_ALLIED );
			CWinCtrl* pCtrl = NULL;
			if( pCtrl = Find( IID_LISTBOX_ALL ) )
			{
				CTListBox* pListBox = (CTListBox*)pCtrl;
				pListBox->SetValue( pListBox->GetMaximum() );
			}
			ShowChild( "SELECTED_ALL" );
			
		}
		break;
	case TAB_WHISPER:
		{
			ShowChild( IID_LISTBOX_WHISPER );
			ShowChild( IID_SCROLLBAR_WHISPER );
			HideChild( IID_LISTBOX_ALL );
			HideChild( IID_SCROLLBAR_ALL );
			HideChild( IID_LISTBOX_TRADE );
			HideChild( IID_SCROLLBAR_TRADE );
			HideChild( IID_LISTBOX_PARTY );
			HideChild( IID_SCROLLBAR_PARTY );
			HideChild( IID_LISTBOX_CLAN );
			HideChild( IID_SCROLLBAR_CLAN );
			HideChild( IID_LISTBOX_ALLIED );
			HideChild( IID_SCROLLBAR_ALLIED );
			CWinCtrl* pCtrl = NULL;
			if( pCtrl = Find( IID_LISTBOX_WHISPER ) )
			{
				CTListBox* pListBox = (CTListBox*)pCtrl;
				pListBox->SetValue( pListBox->GetMaximum() );
			}			
			ShowChild( "SELECTED_WHISPER" );
		}
		break;
	case TAB_TRADE:
		{
			ShowChild( IID_LISTBOX_TRADE );
			ShowChild( IID_SCROLLBAR_TRADE );
			HideChild( IID_LISTBOX_ALL );
			HideChild( IID_SCROLLBAR_ALL );
			HideChild( IID_LISTBOX_WHISPER );
			HideChild( IID_SCROLLBAR_WHISPER );
			HideChild( IID_LISTBOX_PARTY );
			HideChild( IID_SCROLLBAR_PARTY );
			HideChild( IID_LISTBOX_CLAN );
			HideChild( IID_SCROLLBAR_CLAN );
			HideChild( IID_LISTBOX_ALLIED );
			HideChild( IID_SCROLLBAR_ALLIED );
			CWinCtrl* pCtrl = NULL;
			if( pCtrl = Find( IID_LISTBOX_TRADE ) )
			{
				CTListBox* pListBox = (CTListBox*)pCtrl;
				pListBox->SetValue( pListBox->GetMaximum() );
			}
			
			ShowChild( "SELECTED_TRADE" );
			
		}
		break;
	case TAB_PARTY:
		{
			ShowChild( IID_LISTBOX_PARTY );
			ShowChild( IID_SCROLLBAR_PARTY );
			HideChild( IID_LISTBOX_ALL );
			HideChild( IID_SCROLLBAR_ALL );
			HideChild( IID_LISTBOX_WHISPER );
			HideChild( IID_SCROLLBAR_WHISPER );
			HideChild( IID_LISTBOX_TRADE );
			HideChild( IID_SCROLLBAR_TRADE );
			HideChild( IID_LISTBOX_CLAN );
			HideChild( IID_SCROLLBAR_CLAN );
			HideChild( IID_LISTBOX_ALLIED );
			HideChild( IID_SCROLLBAR_ALLIED );
			CWinCtrl* pCtrl = NULL;
			if( pCtrl = Find( IID_LISTBOX_PARTY ) )
			{
				CTListBox* pListBox = (CTListBox*)pCtrl;
				pListBox->SetValue( pListBox->GetMaximum() );
			}
			ShowChild( "SELECTED_PARTY" );
		}
		break;
	case TAB_CLAN:
		{
			ShowChild( IID_LISTBOX_CLAN );
			ShowChild( IID_SCROLLBAR_CLAN );

			HideChild( IID_LISTBOX_ALL );
			HideChild( IID_SCROLLBAR_ALL );
			HideChild( IID_LISTBOX_WHISPER );
			HideChild( IID_SCROLLBAR_WHISPER );
			HideChild( IID_LISTBOX_TRADE );
			HideChild( IID_SCROLLBAR_TRADE );
			HideChild( IID_LISTBOX_PARTY );
			HideChild( IID_SCROLLBAR_PARTY );
			HideChild( IID_LISTBOX_ALLIED );
			HideChild( IID_SCROLLBAR_ALLIED );
			CWinCtrl* pCtrl = NULL;
			if( pCtrl = Find( IID_LISTBOX_CLAN ) )
			{
				CTListBox* pListBox = (CTListBox*)pCtrl;
				pListBox->SetValue( pListBox->GetMaximum() );
			}
			ShowChild( "SELECTED_CLAN" );
		}
		break;
	case TAB_ALLIED:
		{
			HideChild( IID_LISTBOX_CLAN );
			HideChild( IID_SCROLLBAR_CLAN );
			HideChild( IID_LISTBOX_ALL );
			HideChild( IID_SCROLLBAR_ALL );
			HideChild( IID_LISTBOX_WHISPER );
			HideChild( IID_SCROLLBAR_WHISPER );
			HideChild( IID_LISTBOX_TRADE );
			HideChild( IID_SCROLLBAR_TRADE );
			HideChild( IID_LISTBOX_PARTY );
			HideChild( IID_SCROLLBAR_PARTY );
			ShowChild( IID_LISTBOX_ALLIED );
			ShowChild( IID_SCROLLBAR_ALLIED );
			CWinCtrl* pCtrl = NULL;
			if( pCtrl = Find( IID_LISTBOX_ALLIED ) )
			{
				CTListBox* pListBox = (CTListBox*)pCtrl;
				pListBox->SetValue( pListBox->GetMaximum() );
			}
		}
		break;
	default:
		break;
	}
}

int CChatDLG::GetActiveListBox()
{
	switch( m_iSelectedTab )
	{
	case TAB_ALL:
		return IID_LISTBOX_ALL;
	case TAB_WHISPER:
		return IID_LISTBOX_WHISPER;
	case TAB_TRADE:
		return IID_LISTBOX_TRADE;
	case TAB_PARTY:
		return IID_LISTBOX_PARTY;
	case TAB_CLAN:
		return IID_LISTBOX_CLAN;
	case TAB_ALLIED:
		return IID_LISTBOX_ALLIED;
	default:
		break;
	}
	return 0;
}

int CChatDLG::GetActiveScrollBar()
{
	switch( m_iSelectedTab )
	{
	case TAB_ALL:
		return IID_SCROLLBAR_ALL;
	case TAB_WHISPER:
		return IID_SCROLLBAR_WHISPER;
	case TAB_TRADE:
		return IID_SCROLLBAR_TRADE;
	case TAB_PARTY:
		return IID_SCROLLBAR_PARTY;
	case TAB_CLAN:
		return IID_SCROLLBAR_CLAN;
	case TAB_ALLIED:
		return IID_SCROLLBAR_ALLIED;
	default:
		break;
	}
	return 0;
}

void CChatDLG::AppendMsg2System( const char* pszMsg, DWORD dwColor )
{
	AppendMsg2ListBox( IID_LISTBOX_SYSTEM, IID_SCROLLBAR_SYSTEM, pszMsg, dwColor );
}

void CChatDLG::AppendMsg2All( const char* pszMsg, DWORD dwColor , int iFilterType )
{
	if( iFilterType == FILTER_WHISPER )///무조건 넣는다.
	{
		AppendMsg2ListBox( IID_LISTBOX_ALL, IID_SCROLLBAR_ALL, pszMsg, dwColor );
	}
	else///필터 타입에 따라 넣거나안넣는다.
	{
		if( m_Filters[0].Filters[iFilterType] > 0 )
			AppendMsg2ListBox( IID_LISTBOX_ALL, IID_SCROLLBAR_ALL, pszMsg, dwColor );
	}
}

void CChatDLG::AppendMsg2Whisper( const char* pszMsg, DWORD dwColor , int iFilterType )
{
	if( iFilterType == FILTER_WHISPER )///무조건 넣는다.
	{
		AppendMsg2ListBox( IID_LISTBOX_WHISPER, IID_SCROLLBAR_WHISPER, pszMsg, dwColor );
	}
	else///필터 타입에 따라 넣거나안넣는다.
	{
		if( m_Filters[1].Filters[iFilterType] > 0 )
			AppendMsg2ListBox( IID_LISTBOX_WHISPER, IID_SCROLLBAR_WHISPER, pszMsg, dwColor );
	}
}

void CChatDLG::AppendMsg2Trade( const char* pszMsg, DWORD dwColor , int iFilterType )
{
	if( iFilterType == FILTER_WHISPER )///무조건 넣는다.
	{
		AppendMsg2ListBox( IID_LISTBOX_TRADE, IID_SCROLLBAR_TRADE, pszMsg, dwColor );
	}
	else///필터 타입에 따라 넣거나안넣는다.
	{
		if( m_Filters[2].Filters[iFilterType] > 0 )
			AppendMsg2ListBox( IID_LISTBOX_TRADE, IID_SCROLLBAR_TRADE, pszMsg, dwColor );
	}
}

void CChatDLG::AppendMsg2Party( const char* pszMsg, DWORD dwColor , int iFilterType )
{
	if( iFilterType == FILTER_WHISPER )///무조건 넣는다.
	{
		AppendMsg2ListBox( IID_LISTBOX_PARTY, IID_SCROLLBAR_PARTY, pszMsg, dwColor );
	}
	else///필터 타입에 따라 넣거나안넣는다.
	{
		if( m_Filters[3].Filters[iFilterType] > 0 )
			AppendMsg2ListBox( IID_LISTBOX_PARTY, IID_SCROLLBAR_PARTY, pszMsg, dwColor );
	}
}

void CChatDLG::AppendMsg2Clan( const char* pszMsg, DWORD dwColor , int iFilterType )
{
	if( iFilterType == FILTER_WHISPER )///무조건 넣는다.
	{
		AppendMsg2ListBox( IID_LISTBOX_CLAN, IID_SCROLLBAR_CLAN, pszMsg, dwColor );
	}
	else///필터 타입에 따라 넣거나안넣는다.
	{
		if( m_Filters[4].Filters[iFilterType] > 0 )
			AppendMsg2ListBox( IID_LISTBOX_CLAN, IID_SCROLLBAR_CLAN, pszMsg, dwColor );
	}
}

void CChatDLG::	AppendMsg2Allied( const char* pszMsg, DWORD dwColor , int iFilterType )
{
	if( iFilterType == FILTER_WHISPER )///무조건 넣는다.
	{
		AppendMsg2ListBox( IID_LISTBOX_ALLIED, IID_SCROLLBAR_ALLIED, pszMsg, dwColor );
	}
	else///필터 타입에 따라 넣거나안넣는다.
	{
		if( m_Filters[5].Filters[iFilterType] > 0 )
			AppendMsg2ListBox( IID_LISTBOX_ALLIED, IID_SCROLLBAR_ALLIED, pszMsg, dwColor );
	}
}

void CChatDLG::AppendMsg2ListBox( int iListBoxID, int iScrollBarID , const char* pszMsg, DWORD dwColor )
{
	CWinCtrl* pCtrl = Find( iListBoxID );
	assert( pCtrl );
	if( pCtrl && pCtrl->GetControlType() == CTRL_LISTBOX )
	{
		CTListBox* pListBox = (CTListBox*)pCtrl;

		if( pListBox->GetValue()  >= pListBox->GetMaximum() - pListBox->GetExtent() )
		{
			pListBox->AppendText( pszMsg, dwColor , true );
			pListBox->SetValue( pListBox->GetMaximum() );

			pCtrl = Find( iScrollBarID );
			assert( pCtrl );
			if( pCtrl && pCtrl->GetControlType() == CTRL_SCROLLBAR )
			{
				CTScrollBar* pScrollBar = (CTScrollBar*)pCtrl;
				pScrollBar->SetValue( pListBox->GetMaximum() );
			}
		}
		else
		{
			pListBox->AppendText( pszMsg, dwColor , false );
		}
	}
}

void CChatDLG::AppendMsg2ZListBox( int iListBoxID, int iScrollBarID , const char* pszMsg, DWORD dwColor )
{
	CWinCtrl* pCtrl = Find( iListBoxID );
	assert( pCtrl );

	if( pCtrl && pCtrl->GetControlType() == CTRL_ZLISTBOX)
	{
		CZListBox* pListBox = (CZListBox*)pCtrl;

		int iItemCount = pListBox->GetSize();

		int iWidth	 = pListBox->GetWidth();

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
				
				CZLBAppendText* pAppText = new CZLBAppendText(m_szStirng, iWidth, dwColor );	

				pListBox->Add( pAppText );
				pListBox->SetValue( pListBox->GetMaximum() );

				pListBox->SetWidth( iWidth );

				SetHeight( GetHeight() + pAppText->GetHeight() );

				pCtrl = Find( iScrollBarID );
				if( pCtrl && pCtrl->GetControlType() == CTRL_SCROLLBAR )
				{
					pScrollBar = (CTScrollBar*)pCtrl;
					pScrollBar->SetValue( pListBox->GetMaximum() );
				}
			}
		}
		else
		{
			CZLBAppendText* pAppText = new CZLBAppendText( pszMsg, iWidth, dwColor);			

			pListBox->Add( pAppText );

			pListBox->SetValue( pListBox->GetMaximum() );

			pListBox->SetWidth( iWidth );

			SetHeight( GetHeight() + pAppText->GetHeight() );

			pCtrl = Find( iScrollBarID );
			if( pCtrl && pCtrl->GetControlType() == CTRL_SCROLLBAR )
			{
				CTScrollBar* pScrollBar = (CTScrollBar*)pCtrl;
				pScrollBar->SetValue( pListBox->GetMaximum() );
			}
		}
		
	}
}


void CChatDLG::OnLButtonDown( unsigned uiProcID, WPARAM wParam , LPARAM lParam )
{
	CFilterWord& Util = CFilterWord::GetInstance();

	switch( uiProcID )
	{
	case IID_BTN_ALL:
		TabSelected( TAB_ALL );
		if( CWinCtrl* pCtrl = Find( IID_EDITBOX ) )
		{
			CTEditBox* pBox = (CTEditBox*)pCtrl;
			pBox->clear_text();
		}
		ActiveListBoxMoveEnd();
		break;
	case IID_BTN_WHISPER:
		TabSelected( TAB_WHISPER );
		if( CWinCtrl* pCtrl = Find( IID_EDITBOX ) )
		{
			CTEditBox* pBox = (CTEditBox*)pCtrl;
			pBox->clear_text();

			std::string stTemp;
			Util.Wide2MultyByteString( m_WhisperCommands[0], stTemp );			
			pBox->AppendText( stTemp.c_str() );
		}
		ActiveListBoxMoveEnd();
		break;
	case IID_BTN_TRADE:
		TabSelected( TAB_TRADE );
		if( CWinCtrl* pCtrl = Find( IID_EDITBOX ) )
		{
			CTEditBox* pBox = (CTEditBox*)pCtrl;
			pBox->clear_text();
		}
		ActiveListBoxMoveEnd();
		break;
	case IID_BTN_PARTY:
		TabSelected( TAB_PARTY );
		if( CWinCtrl* pCtrl = Find( IID_EDITBOX ) )
		{
			CTEditBox* pBox = (CTEditBox*)pCtrl;
			pBox->clear_text();
			pBox->AppendText("#");
		}
		ActiveListBoxMoveEnd();
		break;
	case IID_BTN_CLAN:
		TabSelected( TAB_CLAN );
		if( CWinCtrl* pCtrl = Find( IID_EDITBOX ) )
		{
			CTEditBox* pBox = (CTEditBox*)pCtrl;
			pBox->clear_text();

			std::string stTemp;
			Util.Wide2MultyByteString( m_ClanCommands[0], stTemp );			
			pBox->AppendText( stTemp.c_str() );
		}
		ActiveListBoxMoveEnd();
		break;
	case IID_BTN_ALLIED:
		{
			TabSelected( TAB_ALLIED );
			if( CWinCtrl* pCtrl = Find( IID_EDITBOX ) )
			{
				CTEditBox* pBox = (CTEditBox*)pCtrl;
				pBox->clear_text();
				pBox->AppendText("~");
			}
		}
		ActiveListBoxMoveEnd();
		break;
	default:
		break;
	}

	if( uiProcID != IID_BTN_FILTER )
		g_itMGR.CloseDialog( DLG_TYPE_CHATFILTER );
}


bool CChatDLG::Create( const char* IDD )
{
	if( CTDialog::Create( IDD ) )
	{
		CWinCtrl* pCtrl = Find( IID_RADIOBOX );
		if( pCtrl && pCtrl->GetControlType() == CTRL_RADIOBOX )
		{
			CTRadioBox* pBox = (CTRadioBox*)pCtrl;
			pBox->SetPressedButton( IID_BTN_ALL );
		}

		pCtrl = Find( IID_EDITBOX );
		assert( pCtrl );
		if( pCtrl )
		{
			CTEditBox* pEditBox = (CTEditBox*) pCtrl;
			pEditBox->SetLimitText(150);
			pCtrl->AddActionListener( this );
		}

		return true;
	}
	return false;

}

ChatFilter& CChatDLG::GetCurrentTabFilter( )
{
	return m_Filters[ m_iSelectedTab - 1];
}

void CChatDLG::SetCurrentTabFilter( ChatFilter& filter )
{
	m_Filters[ m_iSelectedTab - 1] = filter;

	SetSystemChat();
}

void CChatDLG::SetSystemChat()
{
	CWinCtrl * pCtrl = NULL;
	CWinCtrl * pCtrl2 = NULL;

	int		iBefore_ShowSystemChat = m_iShowSystemChat;
	bool	bChange_ShowSystemChat = false;

	if( pCtrl = Find("PAN_SYSTEM_CHAT") )
	{
		if( m_Filters[ m_iSelectedTab - 1].Filters[FILTER_SYSTEM] <= 0 
			|| m_iChatSizeMode == CHATSIZE_MODE_SMALL )
		{        
			pCtrl->Hide();
			m_iShowSystemChat = 0;
		}
		else
		{
			pCtrl->Show();
			m_iShowSystemChat = 1;
		}
	}	

	if( iBefore_ShowSystemChat != m_iShowSystemChat )
	{
		bChange_ShowSystemChat = true;
	}

	if(m_iShowSystemChat==0)
	{
		if( pCtrl = Find("MOVE_TAB") )
		{
            pCtrl->SetOffset( pCtrl->GetOffset().x, m_ChatSize[1][CHATSIZE_MODE_SMALL].height );	
			GetCaption()->SetOffset( pCtrl->GetOffset() );
		}
		if( pCtrl = Find("PAN_INPUT") )
		{
			pCtrl->SetOffset( 0, 0 );			
		}	
		if( pCtrl = Find("PAN_CHANNEL_TAB") )
		{
			pCtrl->Hide();
		}
	}
	else
	{
		if( pCtrl = Find("MOVE_TAB") )
		{
			pCtrl->SetOffset( pCtrl->GetOffset().x, 0 );
			GetCaption()->SetOffset( pCtrl->GetOffset() );
		}
		if( pCtrl = Find("PAN_INPUT") )
		{			
			if( pCtrl2 = Find("PAN_INPUT_POS") )
			{
				pCtrl->SetOffset( pCtrl2->GetOffset() );
			}
		}
		if( pCtrl = Find("PAN_CHANNEL_TAB") )
		{
			pCtrl->Show();
		}
	}

	if( bChange_ShowSystemChat == true )
	{
		if( pCtrl = Find("PAN_INPUT_POS") )
		{
			if(m_iShowSystemChat==0)
			{
				MoveWindow( GetPosition().x, GetPosition().y + pCtrl->GetOffset().y );
			}
			else
			{
				MoveWindow( GetPosition().x, GetPosition().y - pCtrl->GetOffset().y );
			}			
		}		
	}
	else
	{
		MoveWindow(GetPosition());
	}
	
}


unsigned CChatDLG::ActionPerformed( CActionEvent* e )
{
	assert( e );
	if( e == NULL ) return 0;

	CFilterWord& Util = CFilterWord::GetInstance();

	switch( e->GetID() )
	{
	case WM_KEYDOWN:
		{
			if( e->GetWParam() == VK_RETURN )
			{
				CWinCtrl* pCtrl = e->GetSource();
				assert( pCtrl );
				if( pCtrl && pCtrl->GetControlType() == CTRL_EDITBOX )
				{
					CTEditBox* pEditBox = (CTEditBox*)pCtrl;
					if( CTEditBox::s_pFocusEdit && CTEditBox::s_pFocusEdit == pEditBox && pEditBox->get_text() )
					{
						char* msgtext = pEditBox->get_text();
						if (strlen(msgtext) > 0)
						{
							bool sendMsg = true;

							if (msgtext[0] == '/')
							{
								int skillIdx = 0;
								if (strncmp(msgtext, "/sit", 4) == 0) {
									skillIdx = 11;
								} else if (strncmp(msgtext, "/stand", 6) == 0) {
									assert( g_pAVATAR );
									if ( g_pAVATAR->Get_STATE() == CS_SIT ) {
										skillIdx = 11;
									}
								} else if (strncmp(msgtext, "/pick", 5) == 0) {
									skillIdx = 12;
								} else if (strncmp(msgtext, "/atk", 4) == 0 || strncmp(msgtext, "/attack", 7) == 0) {
									skillIdx = 13;
								} else if (strncmp(msgtext, "/party", 6) == 0) {
									skillIdx = 14;
								} else if (strncmp(msgtext, "/trade", 6) == 0) {
									skillIdx = 15;
								} else if (strncmp(msgtext, "/vend", 5) == 0 || strncmp(msgtext, "/vending", 8) == 0) {
									skillIdx = 16;
								} else if (strncmp(msgtext, "/self", 5) == 0) {
									skillIdx = 17;
								} else if (strncmp(msgtext, "/tgt", 4) == 0 || strncmp(msgtext, "/target", 7) == 0) {
									skillIdx = 20;
								} else if (strncmp(msgtext, "/leave", 6) == 0) {
									CTMacroCommand* pMacroCmd = new CTMacroCommand;

									CTCmdSendPacketPartyReq*	pSubCmd1 = new CTCmdSendPacketPartyReq(PARTY_REQ_LEFT, g_pObjMGR->Get_ServerObjectIndex(g_pAVATAR->Get_INDEX() ) );
									CTCmdLeaveParty*			pSubCmd2 = new CTCmdLeaveParty;

									pMacroCmd->AddSubCommand( pSubCmd1 );
									pMacroCmd->AddSubCommand( pSubCmd2 );

									g_itMGR.OpenMsgBox( STR_PARTY_QUERY_LEAVE,CMsgBox::BT_OK | CMsgBox::BT_CANCEL, true, 0 ,pMacroCmd, NULL );
									sendMsg = false;
								} else if (strncmp(msgtext, "/friend", 7) == 0) {
									if (msgtext[7] == ' ') {
										char* paramStr = msgtext + 8;
										if( paramStr && *paramStr )
										{
											if( _strcmpi( paramStr, g_pAVATAR->Get_NAME() ) != 0)
											{
												CTDialog* pDlg = g_itMGR.FindDlg( DLG_TYPE_COMMUNITY );
												assert( pDlg );
												if( pDlg )
												{
													CCommDlg* pCommDlg = (CCommDlg*)pDlg;
													if( pCommDlg->FindFriendByName( paramStr ) == NULL )
														g_pNet->Send_cli_MCMD_APPEND_REQ( paramStr );
													else
														g_itMGR.OpenMsgBox(STR_DUPLICATED_FRIENDNAME);
												}
											}
										}
									}
									sendMsg = false;
								} else if (strncmp(msgtext, "/sort", 5) == 0) {
									CTDialog* pDlg = g_itMGR.FindDlg( DLG_TYPE_ITEM );
									if( pDlg )
									{
										CItemDlg* pItemDlg = ( CItemDlg* ) pDlg;
										pItemDlg->SortItems();
									}
								} else if (strncmp(msgtext, "/logout", 7) == 0) {
									g_itMGR.SetWaitDisconnectType( 1 );
									g_pNet->Send_cli_CHAR_CHANGE();
									sendMsg = false;
								} else if (strncmp(msgtext, "/quit", 5) == 0) {
									g_pCApp->SetExitGame();
									sendMsg = false;
								} else if (strncmp(msgtext, "/help", 5) == 0) {
									AppendMsg2System("Available commands:", 0xffffffff);
									AppendMsg2System("/sit - Sets character to sit state", 0xffffffff);
									AppendMsg2System("/stand - Stands up from sit state", 0xffffffff);
								//	AppendMsg2System("/party - Asks someone to join party", 0xffffffff);
									AppendMsg2System("/leave - Leaves a party", 0xffffffff);
									AppendMsg2System("/trade - Requests a trade", 0xffffffff);
									AppendMsg2System("/vending - Opens a store", 0xffffffff);
									AppendMsg2System("/self - Targets your own character", 0xffffffff);
									AppendMsg2System("/target - Targets nearby enemy", 0xffffffff);
									AppendMsg2System("/logout - Returns to character select", 0xffffffff);
									AppendMsg2System("/quit - Exits the game", 0xffffffff);
								}
								

								if (skillIdx != 0)
								{
									assert( g_pAVATAR );
									CSkillSlot* pSkillSlot = g_pAVATAR->GetSkillSlot();
									assert( pSkillSlot );
									CSkill* pSkill = pSkillSlot->GetSkillBySkillIDX( skillIdx );
									assert( pSkill );
									if( pSkill )
										pSkill->Execute();
									sendMsg = false;
								}
							}


							if (sendMsg) {
								SendChatMsg(msgtext);
							}

							switch( m_iSelectedTab )
							{
							case TAB_PARTY:
								if( pEditBox->get_text() == NULL )
									pEditBox->AppendText("#");
								else if( strlen(pEditBox->get_text()) < 1 )
									pEditBox->AppendText("#");

								break;
							case TAB_CLAN:
								{
									std::string stTemp;
									Util.Wide2MultyByteString( m_ClanCommands[0], stTemp );			
									

									if( pEditBox->get_text() == NULL )
										pEditBox->AppendText( stTemp.c_str() );
									else if( strlen(pEditBox->get_text()) < 1 )
										pEditBox->AppendText( stTemp.c_str() );
								}
								
								break;
							case TAB_ALLIED:
								if( pEditBox->get_text() == NULL )
									pEditBox->AppendText("~");
								else if( strlen(pEditBox->get_text()) < 1 )
									pEditBox->AppendText("~");
								break;

							case TAB_WHISPER:
								{
									std::string stTemp;
									Util.Wide2MultyByteString( m_WhisperCommands[0], stTemp );			


									if( pEditBox->get_text() == NULL )
										pEditBox->AppendText( stTemp.c_str() );
									else if( strlen(pEditBox->get_text()) < 1 )
										pEditBox->AppendText( stTemp.c_str() );
								}
								break;

							default:
								break;
							}
						}
					}
					return pEditBox->GetControlID();
				}
			}
		}
		break;
	default:
		break;
	}
	return 0;
}

//*-------------------------------------------------------------------------------------*/
/// @brief 현재 활성화된(선택된탭) 리스트박스의 item count를 리턴한다.
//*-------------------------------------------------------------------------------------*/
int CChatDLG::GetLineCount()
{
	int listbox_id = GetActiveListBox();
	if( CWinCtrl* pCtrl = Find( listbox_id ) )
	{
		CTListBox* pListBox = (CTListBox*)pCtrl;
		return pListBox->GetMaximum();
	}
	return 0;
}

//*-------------------------------------------------------------------------------------*/
/// @param index : 0-based
/// @brief 현재 활성화된(선택된탭) 리스트박스의 index의 string을 리턴한다.
//*-------------------------------------------------------------------------------------*/
const char* CChatDLG::GetLineString( int index )
{
	int listbox_id = GetActiveListBox();
	if( CWinCtrl* pCtrl = Find( listbox_id ) )
	{
		CTListBox* pListBox = (CTListBox*)pCtrl;
		return pListBox->GetText( index );
	}
	return NULL;
}
//*-------------------------------------------------------------------------------------*/
/// @brief 현재 활성화된(선택된탭) 리스트박스를 맨밑으로 이동시킨다
//*-------------------------------------------------------------------------------------*/
void CChatDLG::ActiveListBoxMoveEnd()
{
	int listbox_id = GetActiveListBox();
	int scrollbox_id = GetActiveScrollBar();

	CWinCtrl* pCtrl = Find( listbox_id );
	if( pCtrl && pCtrl->GetControlType() == CTRL_LISTBOX )
	{
		CTListBox* pListBox = (CTListBox*)pCtrl;
		pListBox->SetValue( pListBox->GetMaximum() );

		pCtrl = Find( scrollbox_id );
		if( pCtrl && pCtrl->GetControlType() == CTRL_SCROLLBAR )
		{
			CTScrollBar* pScrollBar = (CTScrollBar*)pCtrl;
			pScrollBar->SetValue( pListBox->GetMaximum() );
		}
	}
}

void CChatDLG::RefreshDlg()
{
}


void CChatDLG::SetInterfacePos_After()
{
	CWinCtrl * pCtrl = NULL;
	CWinCtrl * pCtrl2 = NULL;
	

	if( pCtrl = Find("SMALL_POS"))
	{
		m_ChatSize[0][CHATSIZE_MODE_SMALL].pt.x = pCtrl->GetOffset().x;
		m_ChatSize[0][CHATSIZE_MODE_SMALL].pt.y = pCtrl->GetOffset().y;
		m_ChatSize[0][CHATSIZE_MODE_SMALL].width = pCtrl->GetWidth();
		m_ChatSize[0][CHATSIZE_MODE_SMALL].height = pCtrl->GetHeight();		
	}
	if( pCtrl = Find("MIDDLE_POS"))
	{
		m_ChatSize[0][CHATSIZE_MODE_MIDDLE].pt.x = pCtrl->GetOffset().x;
		m_ChatSize[0][CHATSIZE_MODE_MIDDLE].pt.y = pCtrl->GetOffset().y;
		m_ChatSize[0][CHATSIZE_MODE_MIDDLE].width = pCtrl->GetWidth();
		m_ChatSize[0][CHATSIZE_MODE_MIDDLE].height = pCtrl->GetHeight();
	}
	if( pCtrl = Find("LARGE_POS"))
	{	
		m_ChatSize[0][CHATSIZE_MODE_LARGE].pt.x = pCtrl->GetOffset().x;
		m_ChatSize[0][CHATSIZE_MODE_LARGE].pt.y = pCtrl->GetOffset().y;
		m_ChatSize[0][CHATSIZE_MODE_LARGE].width = pCtrl->GetWidth();
		m_ChatSize[0][CHATSIZE_MODE_LARGE].height = pCtrl->GetHeight();
	}
	if( pCtrl = Find("XLARGE_POS"))
	{
		m_ChatSize[0][CHATSIZE_MODE_XLARGE].pt.x = pCtrl->GetOffset().x;
		m_ChatSize[0][CHATSIZE_MODE_XLARGE].pt.y = pCtrl->GetOffset().y;
		m_ChatSize[0][CHATSIZE_MODE_XLARGE].width = pCtrl->GetWidth();
		m_ChatSize[0][CHATSIZE_MODE_XLARGE].height = pCtrl->GetHeight();
	}
	
	if( pCtrl = Find("CHAT_BG") )
	{
		pCtrl->SetSizeFit(true);
		pCtrl->SetWidth(m_ChatSize[0][m_iChatSizeMode].width);
		pCtrl->SetHeight(m_ChatSize[0][m_iChatSizeMode].height);
	}

	if( pCtrl = Find("CHAT_BG_SS") )
	{
		pCtrl->SetSizeFit(true);
		pCtrl->SetWidth(m_ChatSize[0][m_iChatSizeMode].width);		
	}

	if(pCtrl = Find("SYSTEM_POS"))
	{		
		m_ChatSize[1][CHATSIZE_MODE_SMALL].width = pCtrl->GetWidth();
		m_ChatSize[1][CHATSIZE_MODE_SMALL].height = pCtrl->GetHeight();

		if( pCtrl2 = Find("CHAT_BG_S") )
		{
			pCtrl2->SetSizeFit(true);
			pCtrl2->SetWidth( pCtrl->GetWidth() );
			pCtrl2->SetHeight( pCtrl->GetHeight() );
		}	
	}

	//문자열 추가.	
/*
	if( pCtrl = Find( IID_BTN_ALL ) )
	{
		((CTRadioButton*)pCtrl)->SetText( "전체" );
	}
	if( pCtrl = Find( IID_BTN_WHISPER ) )
	{
		((CTRadioButton*)pCtrl)->SetText( "\"귓속말" );
	}
	if( pCtrl = Find( IID_BTN_TRADE ) )
	{
		((CTRadioButton*)pCtrl)->SetText( "$매매" );
	}
	if( pCtrl = Find( IID_BTN_PARTY ) )
	{
		((CTRadioButton*)pCtrl)->SetText( "#파티" );
	}
	if( pCtrl = Find( IID_BTN_CLAN ) )
	{
		((CTRadioButton*)pCtrl)->SetText( "@클랜" );
	}
*/
	if( pCtrl = Find( "SELECTED_ALL"  ) )
	{
		//전체 887
		((CTImage*)pCtrl)->SetText( LIST_STRING(887) );
		((CTImage*)pCtrl)->SetAlign( DT_BOTTOM | DT_CENTER );
	}
	if( pCtrl = Find( "SELECTED_WHISPER" ) )
	{
		//귓속말 779
		((CTImage*)pCtrl)->SetText( LIST_STRING(779) );
		((CTImage*)pCtrl)->SetAlign( DT_BOTTOM | DT_CENTER );
	}
	if( pCtrl = Find( "SELECTED_TRADE" ) )
	{
		//매매 888
		((CTImage*)pCtrl)->SetText( LIST_STRING(888) );
		((CTImage*)pCtrl)->SetAlign( DT_BOTTOM | DT_CENTER );
	}
	if( pCtrl = Find( "SELECTED_PARTY" ) )
	{
		//파티 27
		((CTImage*)pCtrl)->SetText( LIST_STRING(27) );
		((CTImage*)pCtrl)->SetAlign( DT_BOTTOM | DT_CENTER );
	}
	if( pCtrl = Find( "SELECTED_CLAN" ) )
	{
		//클랜 36
		((CTImage*)pCtrl)->SetText( LIST_STRING(36) );
		((CTImage*)pCtrl)->SetAlign( DT_BOTTOM | DT_CENTER );
	}
	
	MoveWindow(GetPosition());
}



bool CChatDLG::IsInside4Move(int x, int y)
{
	CWinCtrl * pCtrl = NULL;

	if(pCtrl = Find("MOVE_TAB"))
	{
		if( pCtrl->IsInside(x, y) )
		{
			return true;
		}
	}

	if(pCtrl = Find("MAIN_LEFT"))
	{
		if( pCtrl->IsInside(x, y) )
		{
			return true;
		}
	}

	if(pCtrl = Find("MAIN_MIDDLE"))
	{
		if( pCtrl->IsInside(x, y) )
		{
			return true;
		}
	}
	

	return false;
}

bool CChatDLG::On_LButtonDN( unsigned iProcID, WPARAM wParam, LPARAM lParam )
{
	return false;
}
