/*
$Header: /Client/Network/RecvPACKET.cpp 822   07-05-14 1:48p Raning $
*/
#include "stdAFX.h"
#include "CNetwork.h"
#include "Game.h"
#include "CCamera.h"
#include "IO_Terrain.h"
#include "Game_FUNC.h"
#include "JCommandState.h"	

#include "../CClientStorage.h"
#include "../CJustModelAVT.h"
#include "../SqliteDB.h"
#include "../Country.h"

#include "../System/CGame.h"
#include "../GameData/ServerList.h"
#include "../GameData/CClan.h"
#include "../GameData/CBank.h"
#include "../GameData/CParty.h"
#include "../GameData/CSeparate.h"
#include "../GameData/CGameDataCreateAvatar.h"
#include "../GameData/CUpgrade.h"
#include "../GameCommon/Skill.h"
#include "../Common/IO_STB.h"
#include "../Common/IO_Quest.h"
#include "../common/Calculation.h"
#include "../Util/CCheat.h"
#include "../GameProc/CDayNNightProc.h"

#include "../Interface/it_MGR.h"
#include "../Interface/TypeResource.h"	
#include "../Interface/CHelpMgr.h"
#include "../Interface/CUIMediator.h"
#include "../Interface/ExternalUI/CExternalUI.h"
#include "../Interface/ExternalUI/CLogin.h"
#include "../Interface/ExternalUI/ExternalUILobby.h"
#include "../Interface/ExternalUI/CSelectChannel.h"
#include "../Interface/ClanMarkTransfer.h"
#include "../Interface/ClanMarkManager.h"

#include "../Interface/Dlgs/DeliveryStoreDlg.h"
#include "../Interface/Dlgs/ChattingDlg.h"
#include "../interface/dlgs/CQuestDlg.h"
#include "../interface/dlgs/CPartyDlg.h"
#include "../interface/dlgs/DealDlg.h"
#include "../interface/dlgs/MakeDlg.h"
#include "../interface/dlgs/CSystemMsgDlg.h"
#include "../interface/dlgs/ExchangeDlg.h"
#include "../interface/dlgs/CCommDlg.h"
#include "../interface/dlgs/CSeparateDlg.h"
#include "../interface/DLGs/ClanWar_BossDlg.h"

#include "../interface/icon/ciconitem.h"

#include "../interface/command/CTCmdHotExec.h"
#include "../interface/command/CTCmdNumberInput.h"
#include "../interface/command/UICommand.h"

#include "../Util/LogWnd.h"

#include "../Event/QuestRewardQueue.h"
#include "../ObjFixedEvent.h"

#include "../Event/Quest_Func.h"
#include "GameProc/LiveCheck.h"
#include "GameProc/DelayedExp.h"

#include "../Tutorial/TutorialEventManager.h"
#include "../GameProc/UseItemDelay.h"
#include "../Util/SystemUtil.h"

#include "../interface/dlgs/CMsgBox.h"
WORD myid = 0;
WORD pickupitem = 0;
// Count in the global settings from the server ...
void SetServerVAR( tagVAR_GLOBAL *pVAR )
{
	// short	m_nWorld_PRODUCT;					// Manufacturing
	// DWORD	m_dwUpdateTIME;						// Updated time.
	// short	m_nWorld_RATE;						// Economic world prices ::80~140
	// BYTE		m_btTOWN_RATE;						// Waterfront Village					80~140
	// BYTE		m_btItemRATE[ MAX_PRICE_TYPE ];		// Another item prices				1~127

	::Set_WorldPROD( pVAR->m_nWorld_PRODUCT );
	::Set_WorldRATE( pVAR->m_nWorld_RATE );

	if ( g_pTerrain ) {
		g_pTerrain->m_Economy.m_dwUpdateTIME = pVAR->m_dwUpdateTIME;
		g_pTerrain->m_Economy.m_btTOWN_RATE = pVAR->m_btTOWN_RATE;
		::CopyMemory( g_pTerrain->m_Economy.m_btItemRATE, pVAR->m_btItemRATE, sizeof(BYTE)*MAX_PRICE_TYPE);
	}

	g_GameDATA.m_iPvPState  = pVAR->m_dwGlobalFLAGS & ZONE_FLAG_PK_ALLOWED;
}



//-------------------------------------------------------------------------------------------------
CRecvPACKET::CRecvPACKET ()
{
	m_pRecvPacket = (t_PACKET*) new char[ MAX_PACKET_SIZE ];
}
CRecvPACKET::~CRecvPACKET()
{
	SAFE_DELETE_ARRAY( m_pRecvPacket );
}

//-------------------------------------------------------------------------------------------------
void CRecvPACKET::Recv_srv_ERROR ()
{
	switch( m_pRecvPacket->m_lsv_ERROR.m_wErrorCODE ) 
	{
	case 0:
		break;
	default :
		;
	} 
}

//-------------------------------------------------------------------------------------------------
void CRecvPACKET::Recv_gsv_GM_COMMAND ()
{
	switch( m_pRecvPacket->m_gsv_GM_COMMAND.m_btCMD ) 
	{
	case GM_CMD_SHUT   :
		{
			short nOffset=sizeof( gsv_GM_COMMAND );

			DWORD dwBlockTime = m_pRecvPacket->m_gsv_GM_COMMAND.m_wBlockTIME;
			DWORD dwBlockTimeMilSecond = dwBlockTime * 60 * 1000;

			char *szName = Packet_GetStringPtr ( m_pRecvPacket, nOffset );
			char szTemp[128];
			///Chat block off
			if( dwBlockTime <= 0 )
			{
				if( szName )
					sprintf( szTemp,FORMAT_STR_CANCEL_BLOCK_CHAT, szName);
				else
					sprintf( szTemp,STR_CANCEL_BLOCK_CHAT);

				g_itMGR.OpenMsgBox(szTemp);
				CChatDLG* pDlg = g_itMGR.GetChatDLG();
				if( pDlg )
					pDlg->SetChatUnBlock();

			}
			else///Chat Block
			{
				if( szName )
					sprintf( szTemp,F_STR2_CHAT_BLOCKED_BY_GM, szName ,dwBlockTime );
				else
					sprintf( szTemp,FORMAT_STR_BLOCK_CHAT,dwBlockTime );

				g_itMGR.OpenMsgBox(szTemp);
				CChatDLG* pDlg = g_itMGR.GetChatDLG();
				if( pDlg )
					pDlg->SetChatBlock( dwBlockTimeMilSecond );
			}					
			break;
		}
	case GM_CMD_LOGOUT :
		{
			CTCmdExit Command;
			Command.Exec( NULL );
			//CTCommand* pCmd = new CTCmdExit;
			//short nOffset=sizeof( gsv_GM_COMMAND );
			//char *szName = Packet_GetStringPtr ( m_pRecvPacket, nOffset );
			//char szTemp[128];
			//if( szName )
			//	sprintf( szTemp,FORMAT_STR_GM_BAN_USER, szName );
			//else
			//	sprintf( szTemp,STR_GM_BAN_USER);
			//
			//g_itMGR.OpenMsgBox(szTemp,CMsgBox::BT_OK, true, 0, pCmd ,NULL);
			break;
		}
	}
}

//-------------------------------------------------------------------------------------------------
void CRecvPACKET::Recv_gsv_SET_GLOBAL_VAR ()
{
	SetServerVAR( (tagVAR_GLOBAL*)&m_pRecvPacket->m_pDATA[ sizeof(t_PACKETHEADER) ] );

	g_itMGR.AppendChatMsg(STR_CHANGE_PRICES, IT_MGR::CHAT_TYPE_SYSTEM );

	if( CDealData::GetInstance().GetTradeItemCnt() )
	{
		CTCmdCloseStore* pCmd = new CTCmdCloseStore;
		char szTemp[256];
		sprintf( szTemp,"%s %s",STR_CHANGE_PRICES,STR_QUERY_STOP_TRADE );
		g_itMGR.OpenMsgBox(szTemp , CMsgBox::BT_OK | CMsgBox::BT_CANCEL, true, 0, pCmd , NULL );
	}

}

//-------------------------------------------------------------------------------------------------
void CRecvPACKET::Recv_gsv_SET_GLOVAL_FLAG ()
{
	g_GameDATA.m_iPvPState = m_pRecvPacket->m_gsv_SET_GLOBAL_FLAG.m_dwGlobalFLAGS & ZONE_FLAG_PK_ALLOWED;
}

//-------------------------------------------------------------------------------------------------
void CRecvPACKET::Recv_srv_ANNOUNCE_TEXT ()
{
#pragma message ( "TODO:: Notice received treatment..." __FILE__ )
}

//-------------------------------------------------------------------------------------------------
void CRecvPACKET::Recv_gsv_ANNOUNCE_CHAT ()
{
	short nOffset=sizeof( t_PACKETHEADER );
	char *szMSG = NULL;
	szMSG = CStr::Printf("%s", Packet_GetStringPtr( m_pRecvPacket, nOffset ) );
	char *szName = Packet_GetStringPtr( m_pRecvPacket, nOffset );			

	if( g_Cheat.DoSpecialCheat( szMSG ) )
		return;

	if( szMSG[ 0 ] == '/' )
		return;

	CTDialog* pDlg = g_itMGR.FindDlg( DLG_TYPE_SYSTEMMSG );
	if( pDlg )
	{
		std::string Title = STR_NOTIFY_03;
		if( szName )
		{
			Title.append(":");				
			Title.append( szName );
		}
		CSystemMsgDlg* pSysMsgDlg = (CSystemMsgDlg*) pDlg;
		pSysMsgDlg->SetMessage( Title.c_str(),szMSG, CSystemMsgDlg::MSG_TYPE_NOTICE, g_dwWHITE, 15000 , g_GameDATA.m_hFONT[ FONT_NORMAL_BOLD ]);

		std::string ChatMsg = STR_NOTIFY_03;
		if( szName )///More
		{
			ChatMsg.append(":");
			ChatMsg.append( szName );
			ChatMsg.append(" ");
		}			

		ChatMsg.append( szMSG );
		g_itMGR.AppendChatMsg( ChatMsg.c_str(), IT_MGR::CHAT_TYPE_NOTICE );
	}
}

//-------------------------------------------------------------------------------------------------
bool CRecvPACKET::Recv_lsv_LOGIN_REPLY ()
{
	CLogin* pLogin = (CLogin*)g_EUILobby.GetEUI( EUI_LOGIN );

	if( pLogin == NULL )
		return false;

	g_EUILobby.HideMsgBox();


	CServerList& ServerList = CServerList::GetInstance();
	
	///Channel show, hide, nine minutes
//	if( m_pRecvPacket->m_srv_LOGIN_REPLY.m_btResult & 0x80 )
//		ServerList.HideChannel();
//	else
//		ServerList.ShowChannel();

	//Honggeun: The channel is set to always show.
	ServerList.ShowChannel();

	///Channel show, hide, remove state information
	BYTE btResult = m_pRecvPacket->m_srv_LOGIN_REPLY.m_btResult & ~0x80;

	if( RESULT_LOGIN_REPLY_OK != btResult 
		&& RESULT_LOGIN_REPLY_TAIWAN_OK != btResult
		&& RESULT_LOGIN_REPLY_JAPAN_OK != btResult )
	{

		CTCommand* pCmd = NULL;

#ifdef __AUTOLOGIN	

		/*
		CTDialog* pDlg = g_EUILobby.GetEUI( EUI_LOGIN ) ;
		assert( pDlg );
		if( pDlg && pDlg->IsVision()==false )
		{
			pDlg->Show();
		}*/

		//Automatic login fails, the client end.
		if( g_GameDATA.m_dwSeq && g_GameDATA.m_szSessionKey )
		{
			pCmd = new CTCmdExit;
		}
#endif
		switch( btResult ) 
		{
		case RESULT_LOGIN_REPLY_NO_RIGHT_TO_CONNECT:
			g_EUILobby.ShowMsgBox(STR_LOGIN_REPLY_NO_RIGHT_TO_CONNECT, CTMsgBox::BT_OK ,true , pLogin->GetDialogType(), pCmd );
			return false;
		case RESULT_LOGIN_REPLY_FAILED               :// Error
			g_EUILobby.ShowMsgBox(STR_COMMON_ERROR,CTMsgBox::BT_OK ,true , pLogin->GetDialogType(), pCmd );
			return false;
		case RESULT_LOGIN_REPLY_NOT_FOUND_ACCOUNT    :// Account no.			
			g_EUILobby.ShowMsgBox(STR_NOT_FOUND_ACCOUNT, CTMsgBox::BT_OK, true, pLogin->GetDialogType(), pCmd );
			return false;
		case RESULT_LOGIN_REPLY_INVALID_PASSWORD     :// Off duty error
			g_EUILobby.ShowMsgBox(STR_INVALID_PASSWORD,CTMsgBox::BT_OK ,true ,pLogin->GetDialogType(), pCmd );
			return false;
		case RESULT_LOGIN_REPLY_ALREADY_LOGGEDIN     :// Being already logged in
			g_EUILobby.ShowMsgBox(STR_ALREADY_LOGGEDIN,CTMsgBox::BT_OK ,true , pLogin->GetDialogType(), pCmd );
			return false;
		case RESULT_LOGIN_REPLY_REFUSED_ACCOUNT      :// The account is rejected by the server., Or block the account.
			g_EUILobby.ShowMsgBox(STR_REFUSED_ACCOUNT,CTMsgBox::BT_OK ,true , pLogin->GetDialogType(), pCmd );
			return false;
		case RESULT_LOGIN_REPLY_NEED_CHARGE          :// Need to charge
			g_EUILobby.ShowMsgBox( STR_BILL_AT_ROSEONLINE_HOMEPAGE	,CTMsgBox::BT_OK ,true ,pLogin->GetDialogType(), pCmd ); //Please apply online website payment Rose ~
			return false;		   			
		case RESULT_LOGIN_REPLY_TOO_MANY_USER:
			g_EUILobby.ShowMsgBox(STR_LOGIN_REPLY_TOO_MANY_USER,CTMsgBox::BT_OK ,true ,pLogin->GetDialogType(), pCmd );
			return false;
		case RESULT_LOGIN_REPLY_NO_REAL_NAME:
			g_EUILobby.ShowMsgBox(STR_RESULT_LOGIN_REPLY_NO_REAL_NAME, CTMsgBox::BT_OK ,true ,pLogin->GetDialogType(), pCmd );
			return false;
		case RESULT_LOGIN_REPLY_OUT_OF_IP:
			g_EUILobby.ShowMsgBox( STR_INSUFFICIENCY_IP, CTMsgBox::BT_OK ,true ,pLogin->GetDialogType(), pCmd); //"Ip access has been exceeded for"
			return false;
		case RESULT_LOGIN_REPLY_TAIWAN_FAILED:
			switch( m_pRecvPacket->m_srv_LOGIN_REPLY.m_wPayType )
			{
			case 7:
				g_EUILobby.ShowMsgBox(STR_NOT_FOUND_ACCOUNT,CTMsgBox::BT_OK ,true , pLogin->GetDialogType(), pCmd );
				break;
			default:
				g_EUILobby.ShowMsgBox(STR_COMMON_ERROR,CTMsgBox::BT_OK ,true , pLogin->GetDialogType(), pCmd );
				break;
			}

			return false;
		default:
			g_EUILobby.ShowMsgBox("Login Failed", CTMsgBox::BT_OK ,true ,pLogin->GetDialogType(), pCmd );
			return false;
		}
	}
	short nOffset=sizeof(srv_LOGIN_REPLY);
	char *szServerName;
	DWORD *pServerID;

	szServerName = Packet_GetStringPtr( m_pRecvPacket, nOffset);
	pServerID = (DWORD*)Packet_GetDataPtr( m_pRecvPacket, nOffset, sizeof(DWORD) );

	if ( !szServerName || !pServerID ) 
	{
		CTCommand* pCmd = new CTCmdExit;
		g_EUILobby.ShowMsgBox(STR_INSPECT_ALL_SEVER,CTMsgBox::BT_OK ,true , pLogin->GetDialogType() ,pCmd);
		return false;
	}


	///
	/// Get server list
	///

	CExternalUIManager& refEUIManager = g_EUILobby.GetExternalUIManager();

	//	bool	bSetFirstServerID = false;
	//	int		iFirstServerID  = 0;

	DWORD dwServerID = 0;
	short nServerID=0;
	DWORD dwRight = m_pRecvPacket->m_srv_LOGIN_REPLY.m_wRight ;
	CGame::GetInstance().SetRight( dwRight );
	CGame::GetInstance().SetPayType( m_pRecvPacket->m_srv_LOGIN_REPLY.m_wPayType );

	std::map< BYTE, pair< DWORD, std::string> > TempServerList;




	///If you have previously used the interface window is removed from the Observer.
	if( CTDialog* pUI = refEUIManager.GetEUI( EUI_SELECT_SERVER ) )
	{
		CSelectServer* pSelectServer = (CSelectServer*)pUI;
		ServerList.DeleteObserver( (IObserver*)pSelectServer );
	}


	//EUI_SELECT_SERVER,
	CSelectServer* pSelectServer = new CSelectServer;
	refEUIManager.AddEUI( EUI_SELECT_SERVER, pSelectServer );
	pSelectServer->SetEUIObserver( &refEUIManager );

	if( CServerList::GetInstance().IsShowChannel() )
		pSelectServer->Create("DlgSelSvr");
	else
		pSelectServer->Create("DlgSelOnlySvr");///hiding channels.

	POINT	ptNew;
	ptNew.x = g_pCApp->GetWIDTH() / 2 - pSelectServer->GetWidth()/2;
	ptNew.y = g_pCApp->GetHEIGHT() / 3;
	pSelectServer->MoveWindow( ptNew );

	ServerList.AddObserver( pSelectServer );
	ServerList.ClearWorldServerList();

	//CTDialog* pUI = refEUIManager.GetEUI( EUI_SELECT_SERVER );
	//CSelectServer* pSelectServer = (CSelectServer*)pUI;
	//
	//pSelectServer->ClearServerList();
	//pSelectServer->ClearChannelList();

	while( szServerName && pServerID ) {
		if ( !dwServerID )
			dwServerID = *pServerID;

		nServerID ++;

		LogString (LOG_DEBUG, "\n\n[[ Server: %s, ID: %d ]]\n\n\n", szServerName, *pServerID );

		///Attached to the server name in the @ list shows only the developer permission.
		if( g_GameDATA.m_bForOpenTestServer )
		{
			if( szServerName[0] == '@' && strlen(szServerName) >= 2 )
				TempServerList.insert( std::map< BYTE, pair< DWORD, std::string> >::value_type( (BYTE)( 128 + (*pServerID) ), make_pair( *pServerID, &szServerName[1] ) ) );
		}
		else
		{
			if( szServerName[0] == '@')
			{  
				if( dwRight >= CHEAT_MM) 
				{
					if( strlen(szServerName) >= 2 )
						TempServerList.insert( std::map< BYTE, pair< DWORD, std::string> >::value_type( (BYTE)( 128 + (*pServerID) ) , make_pair( *pServerID, &szServerName[0] ) ) );
				}
			}
			else
			{
				if( strlen(szServerName) >= 2 )
					TempServerList.insert( std::map< BYTE, pair< DWORD, std::string> >::value_type( szServerName[0], make_pair( *pServerID, &szServerName[1] ) ) );

			}
		}

		szServerName = Packet_GetStringPtr( m_pRecvPacket, nOffset);
		pServerID = (DWORD*)Packet_GetDataPtr( m_pRecvPacket, nOffset, sizeof(DWORD) );
	}

	std::map< BYTE, pair< DWORD, std::string> >::iterator iter;
	int iCount = 0;
	for( iter = TempServerList.begin(); iter != TempServerList.end(); ++iter, ++iCount )
		ServerList.AddWorldServerList( iCount, iter->second.first, (char*)(iter->second.second.c_str()) );


	g_EUILobby.HideMsgBox();

	///Need to draw a frame Loading Comes out.
	g_EUILobby.ShowMsgBox( "Loading...",CTMsgBox::BT_NONE, true, pSelectServer->GetDialogType() );
	//CGame::GetInstance().UpdateCurrentState();
	//->

	CGame::GetInstance().ChangeState( CGame::GS_SELECTSVR );
	g_EUILobby.HideMsgBox();

	g_EUILobby.CreateServerListDlg();
	g_EUILobby.SendReqFirstServerChannelList();

	///Succeeded in connecting

	if( CTDialog* pDlg = g_EUILobby.GetEUI( EUI_LOGIN ) )
	{
		CLogin* p = (CLogin*)pDlg;
		if( g_ClientStorage.IsSaveLastConnectID() )
			g_ClientStorage.SaveLastConnectID( p->GetID() );
		else
			g_ClientStorage.SaveLastConnectID( NULL );

		g_ClientStorage.SaveOptionLastConnectID();
	}


	return true;
}

//-------------------------------------------------------------------------------------------------
int CRecvPACKET::Recv_lsv_SELECT_SERVER ()
{
	ClientLog (LOG_DEBUG, "Recv_lsv_SELECT_SERVER:: Result: %d ", m_pRecvPacket->m_lsv_SELECT_SERVER.m_btResult);

	///
	/// Recv proc
	/// Check inside the normal operation ...
	///
	CSelectServer* pSelectServer = (CSelectServer*)g_EUILobby.GetEUI( EUI_SELECT_SERVER );

	if( pSelectServer == NULL )
		return 0;

	pSelectServer->RecvSelectServer( m_pRecvPacket );

	if ( m_pRecvPacket->m_lsv_SELECT_SERVER.m_btResult != RESULT_SELECT_SERVER_OK	) {
		return 0;
	}

	short nOffset = sizeof( lsv_SELECT_SERVER );
	char *szServerIP;
	WORD *pServerPort;

	szServerIP = Packet_GetStringPtr( m_pRecvPacket, nOffset);
	pServerPort= (WORD*)Packet_GetDataPtr  ( m_pRecvPacket, nOffset, sizeof(WORD) );

	// Socket address or port not change during operation.
	this->m_WSV_IP.Set( szServerIP );
	this->m_wWSV_PORT  = *pServerPort;
	this->m_dwWSV_ID   = m_pRecvPacket->m_lsv_SELECT_SERVER.m_dwIDs[0];

	ClientLog (LOG_DEBUG, "Recv_lsv_SELECT_SERVER:: Result: %d, IP: %s, Port: %d ", m_pRecvPacket->m_lsv_SELECT_SERVER.m_btResult, szServerIP, *pServerPort);

	this->DisconnectFromServer( NS_DIS_FORM_LSV );	

	// Temporary
	//DestroyWaitDlg();

	return m_pRecvPacket->m_lsv_SELECT_SERVER.m_dwIDs[1];
}

//-------------------------------------------------------------------------------------------------
int CRecvPACKET::Recv_srv_JOIN_SERVER_REPLY ()
{
	char *szResult[] = {
		"RESULT_JOIN_SERVER_OK",
			"RESULT_JOIN_SERVER_FAILED",
			"RESULT_JOIN_SERVER_TIME_OUT",
			"RESULT_JOIN_SERVER_INVALID_PASSWORD",
			"RESULT_JOIN_SERVER_ALREADY_LOGGEDIN"
	} ;

	LogString (LOG_DEBUG, "Recv_srv_JOIN_SERVER_REPLY:: Result: %d [ %s ]", m_pRecvPacket->m_srv_JOIN_SERVER_REPLY.m_btResult, szResult[ m_pRecvPacket->m_srv_JOIN_SERVER_REPLY.m_btResult ]);

	if ( RESULT_JOIN_SERVER_OK != m_pRecvPacket->m_srv_JOIN_SERVER_REPLY.m_btResult ) {
		return 0;
	}

	return m_pRecvPacket->m_srv_JOIN_SERVER_REPLY.m_dwID;
}

//-------------------------------------------------------------------------------------------------
void CRecvPACKET::Recv_gsv_INIT_DATA ()
{
	m_pRecvPacket->m_gsv_INIT_DATA.m_iRandomSEED;
	m_pRecvPacket->m_gsv_INIT_DATA.m_wRandomINDEX;
}

//-------------------------------------------------------------------------------------------------
void CRecvPACKET::Recv_wsv_CHAR_LIST ()
{
	CSelectAvata* pSelectAvata = (CSelectAvata*)g_EUILobby.GetEUI( EUI_SELECT_AVATA );

	pSelectAvata->RecvAvataList( m_pRecvPacket );

	g_EUILobby.CloseWaitAvataListDlg();	

	if( CGame::GetInstance().GetCurrStateID() == CGame::GS_SELECTSVR )
		CGame::GetInstance().ChangeState( CGame::GS_PREPARESELECTAVATAR );
	else
		CGame::GetInstance().ChangeState( CGame::GS_SELECTAVATAR );
}

bool CRecvPACKET::Recv_wsv_CREATE_CHAR ()
{

	CCreateAvata* pCreateAvata = (CCreateAvata*)g_EUILobby.GetEUI( EUI_CREATE_AVATA );
	if( pCreateAvata == NULL )
		return false;

	if( pCreateAvata->RecvCreateAvata( m_pRecvPacket ) )
	{
		g_EUILobby.CloseWaitJoinServerDlg();		
		g_pNet->Send_cli_CHAR_LIST();
		return true;
	}

	return false;
}

void CRecvPACKET::Recv_wsv_MOVE_SERVER ()
{
	short nOffset = sizeof(wsv_MOVE_SERVER);
	char *szServerIP  =Packet_GetStringPtr( m_pRecvPacket, nOffset);

	this->m_GSV_IP.Set( szServerIP );
	this->m_wGSV_PORT    = m_pRecvPacket->m_wsv_MOVE_SERVER.m_wPortNO;
	this->m_dwGSV_IDs[0] = m_pRecvPacket->m_wsv_MOVE_SERVER.m_dwIDs[0];
	this->m_dwGSV_IDs[1] = m_pRecvPacket->m_wsv_MOVE_SERVER.m_dwIDs[1];
}

//-------------------------------------------------------------------------------------------------
void CRecvPACKET::Recv_gsv_JOIN_ZONE ()
{
	if( !g_pAVATAR )
		return;
myid=m_pRecvPacket->m_gsv_JOIN_ZONE.m_wServerObjectIndex;
	// The index of my avatar ...
	g_pObjMGR->Set_ServerObjectIndex (g_pAVATAR->Get_INDEX(), m_pRecvPacket->m_gsv_JOIN_ZONE.m_wServerObjectIndex );

	/// If dead set .. Current HP and EXP penalty.
	/// Current Experience sETTING
	g_pAVATAR->SetCur_EXP( m_pRecvPacket->m_gsv_JOIN_ZONE.m_lCurEXP );
	g_pAVATAR->Set_HP( m_pRecvPacket->m_gsv_JOIN_ZONE.m_nCurHP );
	g_pAVATAR->Set_MP( m_pRecvPacket->m_gsv_JOIN_ZONE.m_nCurMP );
	g_pAVATAR->m_GrowAbility.m_lPenalEXP = m_pRecvPacket->m_gsv_JOIN_ZONE.m_lPenalEXP;

	/// Reset time for recovery.
	g_pAVATAR->ClearTimer();

	// set server var Eoham created function ...
	SetServerVAR( &m_pRecvPacket->m_gsv_JOIN_ZONE.m_VAR );

	g_pObjMGR->ResetTime();
	//	g_pCMouse->SetEnable();
	//	g_pCKeybd->SetEnable();

	g_DayNNightProc.SetWorldTime( g_pTerrain->GetZoneNO(), m_pRecvPacket->m_gsv_JOIN_ZONE.m_dwAccWorldTIME );	

	/// List sohwanmop clear.
	g_pAVATAR->ClearSummonedMob();



	//----------------------------------------------------------------------------------------------------	
	/// @brief Existing data set trigger execution
	//----------------------------------------------------------------------------------------------------
	g_pAVATAR->SetTeamInfo( m_pRecvPacket->m_gsv_JOIN_ZONE.m_iTeamNO );
	/// Running quest rewards.	
	/// Rather than running on the client. Quest_Reply Modification is brought 7/28
	/*char* pTriggerName = ZONE_JOIN_TRIGGER( g_pTerrain->GetZoneNO() );
	t_HASHKEY HashKEY = ::StrToHashKey( pTriggerName );
	eQST_RESULT bResult = g_QuestList.CheckQUEST( g_pAVATAR, HashKEY, true );*/

	//----------------------------------------------------------------------------------------------------	
	/// @brief Effects appear to connect the zone
	//----------------------------------------------------------------------------------------------------
	SE_CharJoinZone( g_pAVATAR->Get_INDEX() );

	//----------------------------------------------------------------------------------------
	/// Weather Effects
	//----------------------------------------------------------------------------------------
	int iZoneWeather = ZONE_WEATHER_TYPE( g_pTerrain->GetZoneNO() );
	SE_WeatherEffect( g_pAVATAR->Get_INDEX(), iZoneWeather );


	//----------------------------------------------------------------------------------------------------	
	/// @brief If the party being SvrIndex Gives the change.
	//----------------------------------------------------------------------------------------------------
	if( CParty::GetInstance().HasParty() )
	{
		tag_PARTY_MEMBER MemberInfo;
		MemberInfo.m_dwUserTAG  = g_pAVATAR->GetUniqueTag();
		MemberInfo.m_wObjectIDX = m_pRecvPacket->m_gsv_JOIN_ZONE.m_wServerObjectIndex	;
		/* If you ignore the Nile.
		MemberInfo.m_nHP		= g_pAVATAR->Get_HP();
		MemberInfo.m_nMaxHP		= g_pAVATAR->Get_MaxHP();
		MemberInfo.m_btRecoverHP = g_pAVATAR->Get_RecoverHP();
		MemberInfo.m_btRecoverMP = g_pAVATAR->Get_RecoverMP();
		MemberInfo.m_btCON		 = g_pAVATAR->Get_CON();
		MemberInfo.m_dwStatusFALG = g_pAVATAR->m_EndurancePack.GetStateFlag()
		*/
		CParty::GetInstance().ChangeMemberInfoByUserTag( MemberInfo );
	}


	//	CLiveCheck::GetSingleton().ResetTime();		// 2004.11.11, icarus: SRV_JOIN_SERVER_REPLY Sometimes got moving.

	g_GameDATA.m_iReceivedAvatarEXP = g_pAVATAR->Get_EXP();

	::setModelBlinkCloseMode( g_pAVATAR->GetZMODEL(), false );

	//------------------------------------------------------------------------------------
	///Night Ji Ho: Goddess summoning effect being summoned to the state of the user sets. 
	//------------------------------------------------------------------------------------
	goddessMgr.SetProcess(goddessMgr.IsAvataState(),m_pRecvPacket->m_gsv_JOIN_ZONE.m_wServerObjectIndex,TRUE);

}


//-------------------------------------------------------------------------------------------------
void CRecvPACKET::Recv_gsv_REVIVE_REPLY ()
{
	if ( m_pRecvPacket->m_gsv_REVIVE_REPLY.m_nZoneNO > 0 )
	{
		if( g_pAVATAR )
			g_pAVATAR->m_nReviveZoneNO = m_pRecvPacket->m_gsv_REVIVE_REPLY.m_nZoneNO;
	}
}
void CRecvPACKET::Recv_gsv_SET_VAR_REPLY ()
{
	if( !g_pAVATAR )
		return;

	if ( m_pRecvPacket->m_gsv_SET_VAR_REPLY.m_btVarTYPE & REPLY_GSV_SET_VAR_FAIL_BIT ) {
		// TODO:: Parameter setting error !!!
		return;
	}

	int iValue = m_pRecvPacket->m_gsv_SET_VAR_REPLY.m_iValue;
	switch( m_pRecvPacket->m_gsv_SET_VAR_REPLY.m_btVarTYPE ) 
	{
	case SV_SEX		:	return g_pAVATAR->Set_SEX ( iValue );		
	case SV_CLASS	:	return g_pAVATAR->Set_JOB ( iValue );
	case SV_UNION	:	return g_pAVATAR->Set_UNION ( iValue );
	case SV_RANK	:	return g_pAVATAR->Set_RANK ( iValue );
	case SV_FAME	:	return g_pAVATAR->Set_FAME ( iValue );

	case SV_STR		:	return g_pAVATAR->Set_STR ( iValue );
	case SV_DEX		:	return g_pAVATAR->Set_DEX ( iValue );
	case SV_INT		:	return g_pAVATAR->Set_INT ( iValue );
	case SV_CON		:	return g_pAVATAR->Set_CON ( iValue );
	case SV_CHA		:	return g_pAVATAR->Set_CHARM ( iValue );
	case SV_SEN		:	return g_pAVATAR->Set_SENSE ( iValue );

	case SV_LEVEL	:	return g_pAVATAR->Set_LEVEL ( iValue );
	case SV_EXP		:	return g_pAVATAR->Set_EXP ( iValue );

	}

	//Learn the skills that change character attributes updated.
	CTCmdUpdateLearnAbleSkill Cmd;
	Cmd.Exec(NULL);
}

//-------------------------------------------------------------------------------------------------
void CRecvPACKET::Recv_gsv_SELECT_CHAR ()
{
	short nOffset = sizeof( gsv_SELECT_CHAR );	//PY this seems to be out by 4 bytes after I added two shorts to tagGrowAbility added 4 bytes for maxHP and maxMP to the packet at server side
	char *szName;
	szName = Packet_GetStringPtr( m_pRecvPacket, nOffset);

	LogString (LOG_NORMAL, "\n\n\n>>> AVATER( %s ) : Zone: %d, Pos: %f, %f <<<\n\n\n\n",
		szName, 
		m_pRecvPacket->m_gsv_SELECT_CHAR.m_nZoneNO,
		m_pRecvPacket->m_gsv_SELECT_CHAR.m_PosSTART.x, m_pRecvPacket->m_gsv_SELECT_CHAR.m_PosSTART.y);
	ClientLog (LOG_NORMAL, "Client logged in with character: %s with offset value %i", szName, nOffset);

	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Data set selected avatar.
	CGame& refGame = CGame::GetInstance();
	refGame.m_strAvataName = std::string( ( szName == NULL )? "NULL":szName );

	::CopyMemory( refGame.m_SelectedAvataInfo.m_PartITEM,			m_pRecvPacket->m_gsv_SELECT_CHAR.m_PartITEM,		sizeof( refGame.m_SelectedAvataInfo.m_PartITEM ) );
	::CopyMemory ( &refGame.m_SelectedAvataInfo.m_BasicINFO,		&m_pRecvPacket->m_gsv_SELECT_CHAR.m_BasicINFO,		sizeof(tagBasicINFO)	);
	::CopyMemory ( &refGame.m_SelectedAvataInfo.m_BasicAbility,		&m_pRecvPacket->m_gsv_SELECT_CHAR.m_BasicAbility,	sizeof(tagBasicAbility)	);
	::CopyMemory ( &refGame.m_SelectedAvataInfo.m_BasicAbility,		&m_pRecvPacket->m_gsv_SELECT_CHAR.m_BasicAbility,	sizeof(tagBasicAbility)	);
	::CopyMemory ( &refGame.m_SelectedAvataInfo.m_GrowAbility,		&m_pRecvPacket->m_gsv_SELECT_CHAR.m_GrowAbility,	sizeof(tagGrowAbility)	);
	::CopyMemory ( &refGame.m_SelectedAvataInfo.m_Skill,			&m_pRecvPacket->m_gsv_SELECT_CHAR.m_Skill,			sizeof(tagSkillAbility)	);
	::CopyMemory ( &refGame.m_SelectedAvataInfo.m_HotICONS,			&m_pRecvPacket->m_gsv_SELECT_CHAR.m_HotICONS,		sizeof(CHotICONS)		);
#ifdef __KRRE_NEW_AVATAR_DB
	::CopyMemory ( &refGame.m_SelectedAvataInfo.m_dwCoolTime,		&m_pRecvPacket->m_gsv_SELECT_CHAR.m_dwCoolTime,		sizeof(refGame.m_SelectedAvataInfo.m_dwCoolTime)		);
#endif	
	
	refGame.m_SelectedAvataInfo.m_btCharRACE     = m_pRecvPacket->m_gsv_SELECT_CHAR.m_btCharRACE;
	refGame.m_SelectedAvataInfo.m_nZoneNO		= m_pRecvPacket->m_gsv_SELECT_CHAR.m_nZoneNO;
	refGame.m_SelectedAvataInfo.m_PosSTART.x		= m_pRecvPacket->m_gsv_SELECT_CHAR.m_PosSTART.x;
	refGame.m_SelectedAvataInfo.m_PosSTART.y		= m_pRecvPacket->m_gsv_SELECT_CHAR.m_PosSTART.y;
	refGame.m_SelectedAvataInfo.m_nReviveZoneNO	= m_pRecvPacket->m_gsv_SELECT_CHAR.m_nReviveZoneNO;
	refGame.m_SelectedAvataInfo.m_dwUniqueTAG	= m_pRecvPacket->m_gsv_SELECT_CHAR.m_dwUniqueTAG;		//charid from server
	//ClientLog (LOG_NORMAL, "Character came in with race: %i map: %i", refGame.m_SelectedAvataInfo.m_btCharRACE, refGame.m_SelectedAvataInfo.m_nZoneNO);
	//ClientLog (LOG_NORMAL, "Character came in with MaxHP: %i level: %i stamina: %i", refGame.m_SelectedAvataInfo.m_GrowAbility.m_MaxHP, refGame.m_SelectedAvataInfo.m_GrowAbility.m_nLevel,refGame.m_SelectedAvataInfo.m_GrowAbility.m_nSTAMINA);
	//ClientLog (LOG_NORMAL, "Character came in with Clientid: %i", refGame.m_SelectedAvataInfo.m_dwUniqueTAG);
}

//-------------------------------------------------------------------------------------------------
void CRecvPACKET::Recv_gsv_INVENTORY_DATA ()
{
	CGame& refGame = CGame::GetInstance();
	refGame.m_SelectedAvataINV.m_INV.m_i64Money = m_pRecvPacket->m_gsv_INVENTORY_DATA.m_INV.m_i64Money;

	memcpy( &refGame.m_SelectedAvataINV.m_INV.m_ItemLIST, 
			&m_pRecvPacket->m_gsv_INVENTORY_DATA.m_INV.m_ItemLIST, 
			sizeof( m_pRecvPacket->m_gsv_INVENTORY_DATA.m_INV.m_ItemLIST ) );
}

//-------------------------------------------------------------------------------------------------
void CRecvPACKET::Recv_gsv_QUEST_DATA()
{ 
	ClientLog(LOG_DEBUG,"Receive quest data packet received");
	CGame& refGame = CGame::GetInstance();

	memcpy( &refGame.m_QuestData,
		&m_pRecvPacket->m_gsv_QUEST_DATA,
		sizeof( m_pRecvPacket->m_gsv_QUEST_DATA ) );

	g_EUILobby.CloseAvataListDlg();	

	CGame::GetInstance().CreateSelectedAvata();


	gsv_TELEPORT_REPLY data;
	data.m_nZoneNO = refGame.m_SelectedAvataInfo.m_nZoneNO;
	data.m_PosWARP.x = refGame.m_SelectedAvataInfo.m_PosSTART.x;
	data.m_PosWARP.y = refGame.m_SelectedAvataInfo.m_PosSTART.y;


	refGame.SetLoadingData(  data );
	ClientLog(LOG_DEBUG,"Running GS_MOVE_MAIN. Could this be the problem?");
	refGame.ChangeState( CGame::GS_MOVEMAIN );

}

//20060811 Honggeun
void CRecvPACKET::Recv_gsv_QUEST_ONLY()
{ 
	if( !g_pAVATAR )
		return;

	g_pAVATAR->m_bQuestUpdate = true;

	::CopyMemory( &g_pAVATAR->m_Quests, &m_pRecvPacket->m_gsv_QUEST_ONLY.m_Quests, sizeof( tagQuestData ) );

	//Now, comment out the clan uses only the variables
/*
	///Quest List Update
	CTDialog* pDlg = g_itMGR.FindDlg( DLG_TYPE_QUEST );
	if( pDlg && pDlg->IsVision() )
	{
		CQuestDlg* pQuestDlg = ( CQuestDlg* ) pDlg;
		pQuestDlg->UpdateQuestList();
	}
*/
}

//20060811 Honggeun
void CRecvPACKET::Recv_gsv_WISH_LIST()
{ 
	//Unused for now.
	/*
	if( !g_pAVATAR )	return;

	g_pAVATAR->m_bQuestUpdate = true;

	::CopyMemory ( &g_pAVATAR->m_WishLIST,		&m_pRecvPacket->m_gsv_WISH_LIST,				sizeof(tagWishLIST)		);

	///Quest List Update
	CTDialog* pDlg = g_itMGR.FindDlg( DLG_TYPE_QUEST );
	if( pDlg && pDlg->IsVision() )
	{
		CQuestDlg* pQuestDlg = ( CQuestDlg* ) pDlg;
		pQuestDlg->UpdateQuestList();
	}
	*/
}

//-------------------------------------------------------------------------------------------------
void CRecvPACKET::Recv_gsv_CHEAT_CODE ()
{
	char *szCheatCode = m_pRecvPacket->m_gsv_CHEAT_CODE.m_szCheatCODE;
	g_Cheat.DoCheat( szCheatCode );
}


//-------------------------------------------------------------------------------------------------
/// Only works once stopped.
void CRecvPACKET::Recv_gsv_SET_MOTION ()
{
	CObjCHAR *pCHAR = g_pObjMGR->Get_ClientCharOBJ( m_pRecvPacket->m_gsv_SET_MOTION.m_wObjectIDX, false );
	if ( pCHAR ) 
	{
		if( m_pRecvPacket->m_gsv_SET_MOTION.m_wIsSTOP )
		{
			pCHAR->SetCMD_STOP();
		}

		// Keep the current moving speed (?) Do not know ... Annie speed while moving ever default ...
		pCHAR->Set_MOTION( m_pRecvPacket->m_gsv_SET_MOTION.m_nMotionNO, pCHAR->m_fCurMoveSpeed );
		::setRepeatCount( pCHAR->GetZMODEL(), 1 );

		pCHAR->SetUpdateMotionFlag( true );
	}
}

//-------------------------------------------------------------------------------------------------
void CRecvPACKET::Recv_gsv_TOGGLE ()
{
	CObjCHAR *pCHAR = g_pObjMGR->Get_ClientCharOBJ( m_pRecvPacket->m_gsv_TOGGLE.m_wObjectIDX, false );
	if ( pCHAR ) 
	{
		pCHAR->SetCMD_TOGGLE( m_pRecvPacket->m_gsv_TOGGLE.m_btTYPE );				

		/// Set speed has changed the face velocity
		if ( m_pRecvPacket->m_HEADER.m_nSize == ( sizeof( gsv_TOGGLE ) + sizeof(short) ) )
		{
			if( pCHAR->IsUSER() )
			{
				((CObjAVT*)pCHAR)->SetOri_RunSPEED( m_pRecvPacket->m_gsv_TOGGLE.m_nRunSPEED[0] );

				//Recalculate movement and attack animation speed
				((CObjAVT*)pCHAR)->Update_SPEED();
			}
		}	
	}
}

//-------------------------------------------------------------------------------------------------
void CRecvPACKET::Recv_gsv_CHAT ()
{
	short nOffset = sizeof( gsv_CHAT );
	char *szMsg;

	CObjAVT *pCHAR = g_pObjMGR->Get_ClientCharAVT( m_pRecvPacket->m_gsv_CHAT.m_wObjectIDX, false );
	if ( pCHAR ) 
	{
		szMsg = Packet_GetStringPtr( m_pRecvPacket, nOffset);

		if( szMsg[ 0 ] == '/' )
			return;

		g_itMGR.AppendChatMsg( CStr::Printf("%s>%s", pCHAR->Get_NAME(), szMsg), IT_MGR::CHAT_TYPE_ALL );

		g_UIMed.AddChatMsg( m_pRecvPacket->m_gsv_CHAT.m_wObjectIDX, szMsg );

#ifdef __VIRTUAL_SERVER
		g_Cheat.DoCheat( szMsg );
#endif

	}
}

void CRecvPACKET::Recv_gsv_PARTY_CHAT()
{
	_RPT1( _CRT_WARN,"Recv_gsv_PARTY_CHAT_1(%d)\n" ,g_GameDATA.GetGameTime() );
	short nOffset = sizeof( gsv_CHAT );
	char *szMsg;

	PartyMember member;
	if( CParty::GetInstance().GetMemberInfoByObjSvrIdx( m_pRecvPacket->m_gsv_CHAT.m_wObjectIDX, member ))
	{
		szMsg = Packet_GetStringPtr( m_pRecvPacket, nOffset);
		g_itMGR.AppendChatMsg( CStr::Printf("%s>%s", member.m_strName.c_str(), szMsg), IT_MGR::CHAT_TYPE_PARTY );
		g_UIMed.AddChatMsg( m_pRecvPacket->m_gsv_CHAT.m_wObjectIDX, szMsg );
	}
	_RPT1( _CRT_WARN,"Recv_gsv_PARTY_CHAT_2(%d)\n" ,g_GameDATA.GetGameTime() );
}

void CRecvPACKET::Recv_gsv_WHISPER ()
{
	short nOffset = sizeof( gsv_WHISPER );
	char *szAccount, *szMsg;
	szAccount = Packet_GetStringPtr( m_pRecvPacket, nOffset);
	szMsg     = Packet_GetStringPtr( m_pRecvPacket, nOffset);

	if( szMsg[ 0 ] == '/' )
		return;
	char buffer[1024];

	if ( szMsg && szMsg[ 0 ] ) 
	{
		///Always shows the server sends a whisper.
		///GM And the system would have to do so in the case of a whisper ...
		if( _strcmpi( szAccount, "<SERVER>::" ) == 0 )
		{
			sprintf( buffer, "[%s]%s>%s",STR_WHISPER,szAccount, szMsg );
			g_itMGR.AppendChatMsg ( buffer, IT_MGR::CHAT_TYPE_WHISPER );
		}
		else
		{
			if( g_ClientStorage.IsApproveWhisper() )
			{
				sprintf( buffer,"[%s]%s>%s", STR_WHISPER,szAccount, szMsg );
				g_itMGR.AppendChatMsg ( buffer, IT_MGR::CHAT_TYPE_WHISPER );
			}
			else
			{
				g_pNet->Send_cli_WHISPER(  szAccount, STR_OTHER_WHISPER_REJECT_STATE );
			}
		}
	}
	else
		g_itMGR.AppendChatMsg ( CStr::Printf( FORMAT_STR_FAIL_WHISPER, szAccount ), IT_MGR::CHAT_TYPE_WHISPER );
}

void CRecvPACKET::Recv_gsv_SHOUT ()
{
	short nOffset = sizeof( gsv_SHOUT );
	char *szAccount, *szMsg;
	szAccount = Packet_GetStringPtr( m_pRecvPacket, nOffset);
	szMsg     = Packet_GetStringPtr( m_pRecvPacket, nOffset);

	g_itMGR.AppendChatMsg( CStr::Printf("[%s]%s>%s",STR_SHOUT, szAccount, szMsg), IT_MGR::CHAT_TYPE_SHOUT);
}

//-------------------------------------------------------------------------------------------------
bool CRecvPACKET::Recv_tag_ADD_CHAR (short nCliObjIDX, tag_ADD_CHAR* tagAddChar )
{
	//struct tag_ADD_CHAR : public t_PACKETHEADER {
	//    WORD		m_wObjectIDX;
	//	tPOINTF		m_PosCUR;
	//	tPOINTF		m_PosTO;
	//	WORD		m_wCommand;
	//	WORD		m_wTargetOBJ;
	//	BYTE		m_btMoveMODE;					// 0:Walk 1: Run, 2: My passenger riding on the item, 3: m_wTargetOBJ the passenger riding on the item.
	//	int			m_iHP;
	//#ifdef	__APPLY_04_10_15_TEAMNO
	//	int			m_iTeamNO;						//	char m_cTeamNO => int 15/10/04 Change the Modify ...
	//#else
	//	char		m_cTeamNO;
	//#endif
	//	DWORD		m_dwStatusFALG;			
	//} ;


	CObjCHAR *pSourCHAR = g_pObjMGR->Get_CharOBJ( nCliObjIDX, false );
	if ( pSourCHAR ) 
	{
		switch( tagAddChar->m_btMoveMODE )
		{
		case 0: // Walking
			pSourCHAR->m_bRunMODE = false;
			break;
		case 1: // Jump
			pSourCHAR->m_bRunMODE = true;
			break;
		case 2: // Tan cart status
			{
				//pSourCHAR->m_bRunMODE = false;
				if( pSourCHAR->Is_AVATAR() )
				{
					CObjAVT* pAVT = ( CObjAVT* )pSourCHAR;
					pAVT->CreateCartFromMyData();
				}
			}
			break;
		}		

		pSourCHAR->m_btMoveMODE = tagAddChar->m_btMoveMODE;

		pSourCHAR->Set_HP( tagAddChar->m_iHP );

		/// Team info
		pSourCHAR->SetTeamInfo( tagAddChar->m_iTeamNO );

		if ( pSourCHAR->Get_HP() > 0 ) 
		{
			// pSourCHAR->m_PosGOTO.x = m_pRecvPacket->m_tag_ADD_CHAR.m_PosTO.x;
			// pSourCHAR->m_PosGOTO.y = m_pRecvPacket->m_tag_ADD_CHAR.m_PosTO.y;
			// ;
			switch( tagAddChar->m_wCommand ) {
		case CMD_DIE		:
			pSourCHAR->SetCMD_DIE();
			return false;
		case CMD_SIT		:
			pSourCHAR->SetCMD_SIT ();
			break;

		case CMD_STOP		:
			break;

		case CMD_ATTACK		:
			{
				WORD wServeDist = CD3DUtil::distance ((int)tagAddChar->m_PosCUR.x, (int)tagAddChar->m_PosCUR.y, 
					(int)tagAddChar->m_PosTO.x, (int)tagAddChar->m_PosTO.y);

				D3DVECTOR PosTO;
				PosTO.x = tagAddChar->m_PosTO.x;
				PosTO.y = tagAddChar->m_PosTO.y;
				PosTO.z = 0; // For guests arriving to a new height because it does not know the current one set to the height of his avatar

				pSourCHAR->SetCMD_ATTACK( 
					tagAddChar->m_wTargetOBJ, 
					wServeDist, 
					PosTO );						
			}
			break;

		case CMD_RUNAWAY    :
		case CMD_MOVE		:
		case CMD_PICK_ITEM	:
			{
				WORD wServeDist = CD3DUtil::distance ((int)tagAddChar->m_PosCUR.x, (int)tagAddChar->m_PosCUR.y, 
					(int)tagAddChar->m_PosTO.x, (int)tagAddChar->m_PosTO.y);
				D3DVECTOR PosTO;
				PosTO.x = tagAddChar->m_PosTO.x;
				PosTO.y = tagAddChar->m_PosTO.y;
				PosTO.z = 0; // For guests arriving to a new height because it does not know the current one set to the height of his avatar

				pSourCHAR->SetCMD_MOVE ( 
					wServeDist, 
					PosTO, 
					tagAddChar->m_wTargetOBJ );						
			}
			break;

		case CMD_SKILL2SELF :
			// TODO:: // pSourCHAR->SetCMD_Skill2SELF
			break;
		case CMD_SKILL2OBJ :
			// TODO:: // pSourCHAR->SetCMD_Skill2OBJ
			// And if you can not find the target and the target to keep the command
			// Go to the end coordinates.
			break;
		case CMD_SKILL2POS :
			// TODO:: // pSourCHAR->SetCMD_Skill2POS
			break;
			}

			return true;
		}else
		{
			pSourCHAR->Set_HP( DEAD_HP + 1 );
			pSourCHAR->Dead();
		}
	}
	return false;
}

void CRecvPACKET::Recv_tag_ADJ_STATUS ( short& nPacketOffset, tag_ADD_CHAR* ptagAddChar )
{
	int	  iStateSTBIDX = 0;
	DWORD dwTemp = 0;
	DWORD dwResult = ptagAddChar->m_dwStatusFALG;

	CObjCHAR* pChar = g_pObjMGR->Get_ClientCharOBJ( ptagAddChar->m_wObjectIDX, true );
	if( pChar == NULL )
		return;

	/// Check all status bits.
	for( int i = 0; i < sizeof( DWORD )*8; i++ )
	{
		dwTemp = 0x00000001 & ( dwResult >> i );

		if( dwTemp == 0 )
			continue;

		int iIngType = 0;
		if( CEndurancePack::m_StateFlagTable.find( ( dwTemp << i ) ) != CEndurancePack::m_StateFlagTable.end() )
		{
			iIngType = CEndurancePack::m_StateFlagTable[ ( dwTemp << i ) ];
		}

		if( CEndurancePack::m_StateSTBIDXTable.find( iIngType ) != CEndurancePack::m_StateSTBIDXTable.end() )
		{
			iStateSTBIDX = CEndurancePack::m_StateSTBIDXTable[ iIngType ];

			/// Sustained due to the use of the item is not ...
			if( ING_INC_HP != iStateSTBIDX && ING_INC_MP != iStateSTBIDX )
			{
				if( iStateSTBIDX == ING_DEC_LIFE_TIME )
					pChar->AddEnduranceEntity( 0, 43, 100, ENDURANCE_TYPE_SKILL );
				else
					pChar->AddEnduranceEntity( 0, iStateSTBIDX, 100, ENDURANCE_TYPE_SKILL );
			}
		}/*else
		 {
		 AddMsgToChatWND( STR_UNKNOWN_STATE_FLAG, g_dwRED ,CChatDLG::CHAT_TYPE_SYSTEM);
		 }*/
	}


	if ( ptagAddChar->m_dwStatusFALG & ( FLAG_ING_MAX_HP | FLAG_ING_INC_MOV_SPEED | FLAG_ING_DEC_MOV_SPEED | FLAG_ING_INC_ATK_SPEED | FLAG_ING_DEC_ATK_SPEED ) ) 
	{
		///pChar->m_EndurancePack.ClearEntityPack();
		BYTE* pDATA = reinterpret_cast< BYTE* >( ptagAddChar );
		short *nSTATUS = ( short* )( &(pDATA[ nPacketOffset ]) );

		/// As long as the data is put in the flags set.
		int iIndex = 0;
		if( ptagAddChar->m_dwStatusFALG & FLAG_ING_MAX_HP )		
			pChar->m_EndurancePack.SetStateValue( ING_INC_MAX_HP, nSTATUS[ iIndex++ ] );

		if( ptagAddChar->m_dwStatusFALG & FLAG_ING_INC_MOV_SPEED )		
			pChar->m_EndurancePack.SetStateValue( ING_INC_MOV_SPD, nSTATUS[ iIndex++ ] );

		if( ptagAddChar->m_dwStatusFALG & FLAG_ING_DEC_MOV_SPEED )		
			pChar->m_EndurancePack.SetStateValue( ING_DEC_MOV_SPD, nSTATUS[ iIndex++ ] );

		if( ptagAddChar->m_dwStatusFALG & FLAG_ING_INC_ATK_SPEED )		
			pChar->m_EndurancePack.SetStateValue( ING_INC_ATK_SPD, nSTATUS[ iIndex++ ] );

		if( ptagAddChar->m_dwStatusFALG & FLAG_ING_DEC_ATK_SPEED )		
			pChar->m_EndurancePack.SetStateValue( ING_DEC_ATK_SPD, nSTATUS[ iIndex++ ] );		

		nPacketOffset += iIndex * sizeof( short );

	}
	//else if( ptagAddChar->m_dwStatusFALG & FLAG_ING_STORE_MODE )
	//{
	//	if( pChar->IsUSER() )///If Avatar
	//	{
	//		CObjAVT* pAvt=( CObjAVT* )pChar;		


	//		char *pszTitle = Packet_GetStringPtr ( m_pRecvPacket, nPacketOffset );

	//		pAvt->SetPersonalStoreTitle( pszTitle );
	//		g_UIMed.AddPersonalStoreIndex( pAvt->Get_INDEX() );
	//	}
	//}
}

// MOB & NPC Add to.
void CRecvPACKET::Recv_gsv_SET_NPC_SHOW ()
{
	CObjCHAR *pCHAR = (CObjCHAR*)g_pObjMGR->Get_ClientOBJECT( m_pRecvPacket->m_gsv_SET_NPC_SHOW.m_wObjectIDX );

	if ( pCHAR && pCHAR->IsA(OBJ_NPC ) ) {
		if ( m_pRecvPacket->m_gsv_SET_NPC_SHOW.m_bShow )
			pCHAR->SHOW ();
		else
			pCHAR->HIDE ();
	}
}

void CRecvPACKET::Recv_gsv_NPC_CHAR ()
{
	D3DVECTOR PosCUR;

	PosCUR.x = m_pRecvPacket->m_gsv_NPC_CHAR.m_PosCUR.x;
	PosCUR.y = m_pRecvPacket->m_gsv_NPC_CHAR.m_PosCUR.y;
	PosCUR.z = 0.0f;

	short nCObj, nCharIdx = abs( m_pRecvPacket->m_gsv_NPC_CHAR.m_nCharIdx );

	if ( NPC_TYPE( nCharIdx ) == 999 ) 
	{

		nCObj = g_pObjMGR->Add_NpcCHAR( m_pRecvPacket->m_gsv_NPC_CHAR.m_wObjectIDX, nCharIdx, PosCUR, m_pRecvPacket->m_gsv_NPC_CHAR.m_nQuestIDX, m_pRecvPacket->m_gsv_NPC_CHAR.m_fModelDIR);


		if ( this->Recv_tag_ADD_CHAR( nCObj, &(m_pRecvPacket->m_gsv_NPC_CHAR) ) ) 
		{
			short nOffset = sizeof( gsv_NPC_CHAR );
			this->Recv_tag_ADJ_STATUS( nOffset, &(m_pRecvPacket->m_gsv_NPC_CHAR) );			
		}

		CObjCHAR *pSourCHAR = g_pObjMGR->Get_CharOBJ( nCObj, false );
		if( pSourCHAR )///2004/2/27: More nAvy
		{
			if ( m_pRecvPacket->m_gsv_NPC_CHAR.m_nCharIdx < 0 ) {
				( (CObjNPC*)pSourCHAR )->HIDE ();
			}

			/// 04/5/18 Add an event variable specific NPC - jeddli
			((CObjNPC*)pSourCHAR)->SetEventValue( m_pRecvPacket->m_gsv_NPC_CHAR.m_nEventSTATUS );
			/*char Buf[255];
			sprintf( Buf, " By event parameters from the server %d\n", m_pRecvPacket->m_gsv_NPC_CHAR.m_nEventSTATUS );
			MessageBox( NULL, Buf, "...", MB_OK );*/

			LogString (LOG_NORMAL, "Add NPC : [%s] CObj: %d, SObj: %d \n", 
				pSourCHAR->Get_NAME(),
				nCObj, g_pObjMGR->Get_ServerObjectIndex( nCObj ) );
		}
		else
		{
			LogString (LOG_NORMAL, "Fail Add NPC : CharIdx[%d], QuestIdx[%d]\n", 
				m_pRecvPacket->m_gsv_NPC_CHAR.m_nCharIdx,
				m_pRecvPacket->m_gsv_NPC_CHAR.m_nQuestIDX
				);


		}
	} else {
		_ASSERT( 0 );
	}
}

void CRecvPACKET::Recv_gsv_MOB_CHAR ()
{
	CObjCHAR* pChar = NULL;
	int iSkillOwner = 0;
	int iDoingSkillIDX = 0;

	/// Is your pet?
	if( m_pRecvPacket->m_gsv_MOB_CHAR.m_dwStatusFALG & FLAG_ING_DEC_LIFE_TIME )
	{
		short nOffset = sizeof( gsv_MOB_CHAR );

		short *nSTATUS = ( short* )( &( m_pRecvPacket->m_pDATA[ nOffset ] ) );

		/// As long as the data is put in the flags set.
		int iIndex = 0;
		if( m_pRecvPacket->m_gsv_MOB_CHAR.m_dwStatusFALG & FLAG_ING_MAX_HP )		
			iIndex++;

		if( m_pRecvPacket->m_gsv_MOB_CHAR.m_dwStatusFALG & FLAG_ING_INC_MOV_SPEED )		
			iIndex++;

		if( m_pRecvPacket->m_gsv_MOB_CHAR.m_dwStatusFALG & FLAG_ING_DEC_MOV_SPEED )		
			iIndex++;

		if( m_pRecvPacket->m_gsv_MOB_CHAR.m_dwStatusFALG & FLAG_ING_INC_ATK_SPEED )		
			iIndex++;

		if( m_pRecvPacket->m_gsv_MOB_CHAR.m_dwStatusFALG & FLAG_ING_DEC_ATK_SPEED )		
			iIndex++;

		iSkillOwner = nSTATUS[ iIndex++ ];
		pChar = g_pObjMGR->Get_ClientCharOBJ( iSkillOwner, true );

		/// If you are not currently casting immediately summoned	
		if( pChar && pChar->m_nActiveSkillIDX && !iDoingSkillIDX )
			iDoingSkillIDX = pChar->m_nActiveSkillIDX;

		if( pChar && pChar->m_nToDoSkillIDX && !iDoingSkillIDX )
			iDoingSkillIDX = pChar->m_nToDoSkillIDX;

		if( pChar && pChar->m_nDoingSkillIDX && iDoingSkillIDX )
			iDoingSkillIDX = pChar->m_nDoingSkillIDX;



		/// One immediately summoned
		//if( ( pChar != NULL ) && 		
		//	/// It is currently not active. Casting action is already underway. Went packets arriving too late.
		//	iDoingSkillIDX )		
		//{
		//	pChar->SetSummonMobInfo( m_pRecvPacket->m_gsv_MOB_CHAR );
		//	return;
		//}

		/// Not just the caster summons.
	}

	/// Appeared normal mobs.
	{

		D3DVECTOR PosCUR;

		PosCUR.x = m_pRecvPacket->m_gsv_MOB_CHAR.m_PosCUR.x;
		PosCUR.y = m_pRecvPacket->m_gsv_MOB_CHAR.m_PosCUR.y;
		PosCUR.z = 0.0f;


		short nCObj;

		if ( NPC_TYPE( m_pRecvPacket->m_gsv_MOB_CHAR.m_nCharIdx ) != 999 ) 
		{
			nCObj = g_pObjMGR->Add_MobCHAR( m_pRecvPacket->m_gsv_MOB_CHAR.m_wObjectIDX, 
				m_pRecvPacket->m_gsv_MOB_CHAR.m_nCharIdx, 
				PosCUR, 
				m_pRecvPacket->m_gsv_MOB_CHAR.m_nQuestIDX, 
				(m_pRecvPacket->m_gsv_MOB_CHAR.m_btMoveMODE) ? true:false, 
				m_pRecvPacket->m_gsv_MOB_CHAR.m_nMonLevel,
				m_pRecvPacket->m_gsv_MOB_CHAR.m_nMonSize );
			if ( this->Recv_tag_ADD_CHAR( nCObj, &(m_pRecvPacket->m_gsv_MOB_CHAR) ) ) 
			{
				short nOffset = sizeof( gsv_MOB_CHAR );
				this->Recv_tag_ADJ_STATUS( nOffset, &(m_pRecvPacket->m_gsv_MOB_CHAR) );
			}

			/*LogString( LOG_NORMAL, " =====ADD MOB[ %s, %f, %f, %f ]=====\n", 
			g_pObjMGR->Get_CharOBJ( nCObj, false )->Get_NAME(),
			g_pObjMGR->Get_CharOBJ( nCObj, false )->Get_CurPOS().x,
			g_pObjMGR->Get_CharOBJ( nCObj, false )->Get_CurPOS().y,
			g_pObjMGR->Get_CharOBJ( nCObj, false )->Get_CurPOS().z );*/

			//-------------------------------------------------------------------------------------
			/// Displays that if sohwanmop. Sohwanmop.		
			//-------------------------------------------------------------------------------------
			if( pChar )
			{
				;
			}

			//-------------------------------------------------------------------------------------
			/// Sohwanmop sohwanmop if I added to the list of
			//-------------------------------------------------------------------------------------
			if( (pChar != NULL) && ( pChar->IsA( OBJ_USER ) ) )
			{
				SummonMobInfo mobInfo;
				bool bIsThere = ((CObjUSER*)pChar)->GetSummonedMobInfo( m_pRecvPacket->m_gsv_MOB_CHAR.m_wObjectIDX, mobInfo );
				if( bIsThere )
					iDoingSkillIDX = mobInfo.iSkillIDX;

				((CObjUSER*)pChar)->AddSummonedMob( m_pRecvPacket->m_gsv_MOB_CHAR.m_wObjectIDX, iDoingSkillIDX );

				if( iDoingSkillIDX )
				{					

					int iSkillLevel = SKILL_LEVEL( iDoingSkillIDX );
					int iMaxHP = 0;

					///2005.06.29 Taiwan applied to expressions separated
					if( IsApplyNewVersion() )
					{
						/// Depending on the type of pet HP Separate calculation 2005/8/26 Choejongjin
						switch( NPC_SUMMONMOB_TYPE( m_pRecvPacket->m_gsv_MOB_CHAR.m_nCharIdx ) )
						{
						case 0:
							iMaxHP = (int)( NPC_HP( m_pRecvPacket->m_gsv_MOB_CHAR.m_nCharIdx	) + g_pAVATAR->Get_MaxHP() * 8 / 10 );
							break;
						case 1:
							iMaxHP = (int)( NPC_HP( m_pRecvPacket->m_gsv_MOB_CHAR.m_nCharIdx	));
							break;
						default:
							_RPTF0( _CRT_ASSERT,"Unknown type (pet HP Formula nine minutes)-LIST_NPC.STB the 29th column" );
							break;
						}
					}
					else
					{
						iMaxHP = (int)( NPC_HP( m_pRecvPacket->m_gsv_MOB_CHAR.m_nCharIdx	) * ( iSkillLevel + 16 ) 
							* ( g_pAVATAR->Get_LEVEL() + 85  ) / 2600.f );
					}

					CObjCHAR* pMobChar = g_pObjMGR->Get_CharOBJ( nCObj, true );
					if( pMobChar )
					{
						((CObjMOB*)pMobChar)->Set_MaxHP( iMaxHP );
					}
				}else
				{
					assert( 0 && "This is summoned mob, but don't have owner" );
				}
			}
		} else 
		{
			Recv_gsv_NPC_CHAR ();
			return;
			//		_ASSERT( 0 );
		}
	}
}

//-------------------------------------------------------------------------------------------------
/// Add a User 
/// When a party receives and stores the characters in advance if you were party members Data By taking Setting allows
/// Eoke one state? Now Recv_gsv_AVT_CHAR The state does not require any special treatment beyond bunched in the Object List is simply put.
void CRecvPACKET::Recv_gsv_AVT_CHAR ()
{
	/*
	WORD		m_wObjectIDX;
	tPOINTF		m_PosCUR;
	tPOINTF		m_PosTO;
	WORD		m_wCommand;
	WORD		m_wTargetOBJ;
	short		m_nHP;

	bool		m_bIsFemale;
	short		m_nPartItemIDX[ MAX_BODY_PART ];
	// char		szUserID[]
	*/
	short nOffset = sizeof( gsv_AVT_CHAR );
	char *szName = Packet_GetStringPtr ( m_pRecvPacket, nOffset );

	CObjAVT *pNewAVT = g_pObjMGR->New_AvtCHAR( m_pRecvPacket->m_gsv_AVT_CHAR.m_wObjectIDX, szName ); 
	if ( !pNewAVT ) 
	{
		LogString (LOG_NORMAL, "ADD_USER[ %s ] !!!!!!!!FAILED!!!!!!!!!!! \n", szName );
		return;
	}

	/// Bullet Information storage.
	for( int i = 0;  i < MAX_SHOT_TYPE; ++i )
		pNewAVT->SetShotData( i, m_pRecvPacket->m_gsv_AVT_CHAR.m_sShotItem[i].m_nItemNo );
	//------------------------------------------------------------------------------------------


	// Note the following three lines in the order
	//	pNewAVT->SetAllPARTS    ( m_pRecvPacket->m_gsv_AVT_CHAR.m_nPartItemIDX );
	pNewAVT->SetAllPARTS    ( m_pRecvPacket->m_gsv_AVT_CHAR.m_PartITEM );
	//	pNewAVT->SetAllPetPARTS ( m_pRecvPacket->m_gsv_AVT_CHAR.m_nRidingITEM );
	pNewAVT->SetAllPetPARTS ( m_pRecvPacket->m_gsv_AVT_CHAR.m_RidingITEM );
	pNewAVT->SetOri_RunSPEED( m_pRecvPacket->m_gsv_AVT_CHAR.m_nRunSpeed );
	pNewAVT->SetPsv_AtkSPEED( m_pRecvPacket->m_gsv_AVT_CHAR.m_nPsvAtkSpeed );

	pNewAVT->Set_HP( m_pRecvPacket->m_gsv_AVT_CHAR.m_iHP );
	//	assert( m_pRecvPacket->m_gsv_AVT_CHAR.m_nHP > 0 );

	pNewAVT->SetTeamInfo( m_pRecvPacket->m_gsv_AVT_CHAR.m_iTeamNO );

	pNewAVT->SetAvtLevel( m_pRecvPacket->m_gsv_AVT_CHAR.m_btLEVEL );
	/// Job .. more..

	//------------------------------------------------------------------------------------------
	///If the party
	///CParty Retrieve information from.
	PartyMember member;
	if( CParty::GetInstance().GetMemberInfoByObjSvrIdx( m_pRecvPacket->m_gsv_AVT_CHAR.m_wObjectIDX , member ) )
	{
		pNewAVT->Set_MaxHP( member.m_Info.m_nMaxHP );
		pNewAVT->Set_CON( member.m_Info.m_nCON );
		pNewAVT->Set_AddRecoverHP( member.m_Info.m_btRecoverHP );
		pNewAVT->Set_AddRecoverMP( member.m_Info.m_btRecoverMP );
		pNewAVT->SetStamina( member.m_Info.m_nSTAMINA );
	}

	//------------------------------------------------------------------------------------------
	/// TODO:: Weight ratio of the run / walk to walk as limited ...
	m_pRecvPacket->m_gsv_AVT_CHAR.m_btWeightRate;


	pNewAVT->Update_SPEED ();	//PY: animation speed only

	LogString (LOG_NORMAL, "ADD_USER[ %s ], MoveSpeed: %f \n", szName, pNewAVT->GetOri_RunSPEED() );

	D3DVECTOR PosCUR;
	PosCUR.x = m_pRecvPacket->m_gsv_AVT_CHAR.m_PosCUR.x;
	PosCUR.y = m_pRecvPacket->m_gsv_AVT_CHAR.m_PosCUR.y;
	PosCUR.z = 0.0f;

	if ( g_pObjMGR->Add_AvtCHAR( pNewAVT, PosCUR, m_pRecvPacket->m_gsv_AVT_CHAR.m_btCharRACE) ) 
	{
		if ( this->Recv_tag_ADD_CHAR( pNewAVT->Get_INDEX(), &(m_pRecvPacket->m_gsv_AVT_CHAR) ) ) 
		{
			this->Recv_tag_ADJ_STATUS( nOffset, &(m_pRecvPacket->m_gsv_AVT_CHAR) );
		}
	} 
	else 
	{
		g_pObjMGR->Del_Object( pNewAVT );
		return;
	}

	// Check individual stores for moving... 2004. 11. 25
	// Process For Ment string... 
	// 06. 11. 23 - Kim, Joo - Hyun: flag to check if statement in a switch statement to change;;
	if( m_pRecvPacket->m_gsv_AVT_CHAR.m_dwSubFLAG & FLAG_SUB_STORE_MODE)
	{
		short* nType = (short*)Packet_GetDataPtr( m_pRecvPacket, nOffset, sizeof( short ) );
		char* pszTitle = (char*)Packet_GetStringPtr( m_pRecvPacket, nOffset );			

		pNewAVT->SetPersonalStoreTitle( pszTitle, *nType );
		g_UIMed.AddPersonalStoreIndex( pNewAVT->Get_INDEX() );
	}

	if( m_pRecvPacket->m_gsv_AVT_CHAR.m_dwSubFLAG & FLAG_SUB_INTRO_CHAT)
	{
		char *pszMent = Packet_GetStringPtr ( m_pRecvPacket, nOffset );
	}



	/// clan information
	tag_CLAN_ID* pClanInfo = (tag_CLAN_ID*)Packet_GetDataPtr( m_pRecvPacket, nOffset, sizeof(tag_CLAN_ID) ) ;
	if( pClanInfo && pNewAVT )
	{
		pNewAVT->SetClan( 
			pClanInfo->m_dwClanID, 
			pClanInfo->m_wClanMARK[0],
			pClanInfo->m_wClanMARK[1],
			(char*)Packet_GetStringPtr( m_pRecvPacket, nOffset ),
			pClanInfo->m_btClanLEVEL,
			pClanInfo->m_btClanPOS
			);
	}

	//------------------------------------------------------------------------------------
	/// Special status flag
	//------------------------------------------------------------------------------------
	pNewAVT->ChangeSpecialState( m_pRecvPacket->m_gsv_AVT_CHAR.m_dwSubFLAG );

	//------------------------------------------------------------------------------------
	///Night Ji Ho: Goddess Summon Summon effects of state to set the user.
	//------------------------------------------------------------------------------------
	goddessMgr.SetProcess((m_pRecvPacket->m_gsv_AVT_CHAR.m_dwSubFLAG & FLAG_SUB_ARUA_FAIRY),
		m_pRecvPacket->m_gsv_AVT_CHAR.m_wObjectIDX,TRUE);
}

//-------------------------------------------------------------------------------------------------
void CRecvPACKET::Recv_gsv_SUB_OBJECT ()
{
	short nObjCNT= ( m_pRecvPacket->m_HEADER.m_nSize - sizeof( t_PACKETHEADER ) ) / 2;

	CGameOBJ *pObj;
	for (short nI=0; nI<nObjCNT; nI++) 
	{
		pObj = g_pObjMGR->Get_ClientOBJECT( m_pRecvPacket->m_gsv_SUB_OBJECT.m_wObjectIDX[ nI ] );
		if ( pObj ) 
		{			 
			switch( pObj->Get_TYPE() )
			{
			case OBJ_EVENTOBJECT:					
				break; 

			case OBJ_ITEM:					
				/*	LogString( LOG_NORMAL, "SubItem [ %d ] \n", iDropItem );
				iDropItem--;
				*/	 

			default:  
				{
					//---------------------------------------------------------------------------
					//2 seater off bakjiho ::
					CObjAVT* pAVT = g_pObjMGR->Get_ClientCharAVT( m_pRecvPacket->m_gsv_SUB_OBJECT.m_wObjectIDX[ nI ], false );
					if(pAVT)
					{	pAVT->Process_JOIN_RIDEUSER(); }
					//---------------------------------------------------------------------------

					g_UIMed.SubPersonalStoreIndex( pObj->Get_INDEX() );

					LogString ( LOG_NORMAL, "Del_Object: CObj: %s, SObj: %d \n", pObj->Get_NAME(), m_pRecvPacket->m_gsv_SUB_OBJECT.m_wObjectIDX[ nI ] );
					g_pObjMGR->Del_Object( pObj );		
				}
				break;
			} 
		}
	}
}

//-------------------------------------------------------------------------------------------------
void CRecvPACKET::Recv_gsv_SET_WEIGHT_RATE ()
{
	// Weight ratio of the run, walking restrictions apply.
	CObjAVT* pAVT = g_pObjMGR->Get_ClientCharAVT( m_pRecvPacket->m_gsv_SET_WEIGHT_RATE.m_wObjectIDX, false );
	if( pAVT )
	{
		pAVT->SetWeightRate( m_pRecvPacket->m_gsv_SET_WEIGHT_RATE.m_btWeightRate );

		if( m_pRecvPacket->m_gsv_SET_WEIGHT_RATE.m_btWeightRate >= 110 )
		{
			if( pAVT->GetPetMode() < 0 )
				pAVT->SetCMD_TOGGLE( MOVE_MODE_WALK + TOGGLE_TYPE_DRIVE );

			if( pAVT->IsA(OBJ_USER) )
				g_itMGR.AppendChatMsg( CStr::Printf("%s >= 110%%, %s",STR_WEIGHT_RATE,STR_CANT_ATTACK), IT_MGR::CHAT_TYPE_SYSTEM );
		}
		else if( m_pRecvPacket->m_gsv_SET_WEIGHT_RATE.m_btWeightRate >= 100  )
		{
			if( pAVT->GetPetMode() < 0 )
				pAVT->SetCMD_TOGGLE( MOVE_MODE_WALK + TOGGLE_TYPE_DRIVE );

			if( pAVT->IsA( OBJ_USER ) )
				g_itMGR.AppendChatMsg(CStr::Printf("100%% <= %s < 110%%, %s", STR_WEIGHT_RATE, STR_CANT_RUN ), IT_MGR::CHAT_TYPE_SYSTEM );
		}
		else
		{					
			if( pAVT->IsA( OBJ_USER ) )
				g_itMGR.AppendChatMsg( CStr::Printf("%s < 100%%, %s", STR_WEIGHT_RATE ,STR_NO_PENALTY), IT_MGR::CHAT_TYPE_SYSTEM );
		}
	}
}

//-------------------------------------------------------------------------------------------------
void CRecvPACKET::Recv_gsv_ADJUST_POS ()
{
	m_pRecvPacket->m_gsv_ADJUST_POS.m_wObjectIDX;
	CObjCHAR *pSourCHAR = g_pObjMGR->Get_ClientCharOBJ( m_pRecvPacket->m_gsv_ADJUST_POS.m_wObjectIDX, true );
	if ( pSourCHAR ) 
	{
		// Where the coordinates of the user to compensate for !!!
		// Starting position is the same as the final position may change...
		// If the distance between the starting position than the lf
		// In other cases the final position of the less between the main surface doeldeut...

		// TODO:: pSourCHAR The coordinates of the m_pRecvPacket->m_gsv_ADJUST_POS.m_PosCUR To quickly move to a final position is corrected.
		// One force.
		D3DVECTOR PosCUR;
		PosCUR.x = m_pRecvPacket->m_gsv_ADJUST_POS.m_PosCUR.x;
		PosCUR.y = m_pRecvPacket->m_gsv_ADJUST_POS.m_PosCUR.y;
		PosCUR.z = m_pRecvPacket->m_gsv_ADJUST_POS.m_nPosZ;

		pSourCHAR->SetCMD_MOVE(	PosCUR, pSourCHAR->m_bRunMODE );

		// Stop command..
		//pSourCHAR->SetCMD_STOP ();
	}
}

//-------------------------------------------------------------------------------------------------
void CRecvPACKET::Recv_gsv_STOP ()
{
	CObjCHAR *pCHAR = g_pObjMGR->Get_ClientCharOBJ( m_pRecvPacket->m_gsv_STOP.m_wObjectIDX, true );
	if ( pCHAR ) {
		pCHAR->SetCMD_STOP ();
	}
}

//-------------------------------------------------------------------------------------------------
void CRecvPACKET::Recv_gsv_ATTACK ()
{


#ifdef _DEBUG	

	//Sending a message to the chat window
	//--------------------------------------------------------------
	//char buffer[1024];
	//sprintf( buffer, "*******************");
	//g_itMGR.AppendChatMsg ( buffer, IT_MGR::CHAT_TYPE_WHISPER );

	//sprintf( buffer, "Receive Attack");
	//g_itMGR.AppendChatMsg ( buffer, IT_MGR::CHAT_TYPE_WHISPER );
	//-------------------------------------------------------------
#endif	

	CObjCHAR *pSourCHAR = g_pObjMGR->Get_ClientCharOBJ( m_pRecvPacket->m_gsv_ATTACK.m_wAtkObjIDX, true );

	if ( pSourCHAR ) 
	{
		if( pSourCHAR->IsA( OBJ_MOB ) )
			pSourCHAR->m_bRunMODE = true;

		D3DVECTOR PosTO;
		PosTO.x = m_pRecvPacket->m_gsv_ATTACK.m_PosTO.x;
		PosTO.y = m_pRecvPacket->m_gsv_ATTACK.m_PosTO.y;
		PosTO.z = 0;

		pSourCHAR->SetCMD_ATTACK( m_pRecvPacket->m_gsv_ATTACK.m_wDefObjIDX, 
			m_pRecvPacket->m_gsv_ATTACK.m_wSrvDIST, 
			PosTO );

#ifdef	_DEBUG
		if ( g_pObjMGR->Get_ServerObjectIndex( g_pAVATAR->m_nIndex ) == m_pRecvPacket->m_gsv_ATTACK.m_wDefObjIDX ) {
			ClientLog (LOG_NORMAL, "Attack command:: %d:%s[ %d ]  ==> %s[ %f, %f, %f ] \n", 
				m_pRecvPacket->m_gsv_ATTACK.m_wAtkObjIDX,
				pSourCHAR->Get_NAME(),

				m_pRecvPacket->m_gsv_ATTACK.m_wSrvDIST, 					

				g_pAVATAR->Get_NAME(),

				m_pRecvPacket->m_gsv_ATTACK.m_PosTO.x, 
				m_pRecvPacket->m_gsv_ATTACK.m_PosTO.y,
				PosTO.z );
		}
#endif
	}
}

void CRecvPACKET::Recv_gsv_ATTACK_START ()
{
	_ASSERT( 0 );
}

//----------------------------------------------------------------------------------------------------
/// @param
/// @brief Added value of z.
//----------------------------------------------------------------------------------------------------

void CRecvPACKET::Recv_gsv_MOUSECMD ()
{
	/*
	struct gsv_MOUSECMD : public t_PACKETHEADER {
	WORD	m_wSourObjIDX;
	WORD	m_wDestObjIDX;
	tPOINTF	m_PosCUR;
	tPOINTF m_PosTO;
	} ;
	*/

#ifdef _DEBUG	
	//Cho, Sung - Hyun 10 - 27 chatting 
	//-----------------------------------------------------------------
	char buffer[1024];
	sprintf( buffer, "*******************");	
	CStr::DebugString(buffer);
	sprintf( buffer, "Receive Mouse Command");	
	CStr::DebugString(buffer);
	//------------------------------------------------------------------
#endif	

	CObjCHAR *pSourCHAR = g_pObjMGR->Get_ClientCharOBJ( m_pRecvPacket->m_gsv_MOUSECMD.m_wSourObjIDX, true );

	if ( !pSourCHAR )
		return;

	D3DVECTOR PosTO;

	PosTO.x = m_pRecvPacket->m_gsv_MOUSECMD.m_PosTO.x;
	PosTO.y = m_pRecvPacket->m_gsv_MOUSECMD.m_PosTO.y;
	PosTO.z = 0;

	if (0 == pSourCHAR->Get_ModelSPEED()) 
	{
		// Was in place only if the standstill current height correction.
		// Update_Height Set the height value, update the hint.
		pSourCHAR->SetResetPosZ( true, m_pRecvPacket->m_gsv_MOUSECMD.m_nPosZ );
	}

	pSourCHAR->SetCMD_MOVE ( m_pRecvPacket->m_gsv_MOUSECMD.m_wSrvDIST,
		PosTO, 
		m_pRecvPacket->m_gsv_MOUSECMD.m_wDestObjIDX );

}

//----------------------------------------------------------------------------------------------------
/// @param
/// @brief 
/// @bug Move the monsters only come in the packet. So m_pRecvPacket-> m_gsv_MOVE.m_nPosZ this garbage
///		 Do not corrected for height.
//----------------------------------------------------------------------------------------------------

void CRecvPACKET::Recv_gsv_MOVE ()
{
	/// Add to Recv_gsv_MOUSECMD movement speed packet ...
	CObjCHAR *pSourCHAR = g_pObjMGR->Get_ClientCharOBJ( m_pRecvPacket->m_gsv_MOVE.m_wSourObjIDX, true );
	if ( !pSourCHAR )
		return;	

	pSourCHAR->m_bRunMODE = m_pRecvPacket->m_gsv_MOVE.m_btMoveMODE;
	pSourCHAR->m_btMoveMODE = m_pRecvPacket->m_gsv_MOVE.m_btMoveMODE;

	D3DVECTOR PosTO;
	PosTO.x = m_pRecvPacket->m_gsv_MOVE.m_PosTO.x;
	PosTO.y = m_pRecvPacket->m_gsv_MOVE.m_PosTO.y;	
	PosTO.z = 0;

	/*if( pSourCHAR->Is_AVATAR() )
	{
	D3DXVECTOR3 PosCur = pSourCHAR->Get_CurPOS();
	if( abs( PosCur.z - m_pRecvPacket->m_gsv_MOVE.m_nPosZ )  )
	{
	PosCur.z = m_pRecvPacket->m_gsv_MOVE.m_nPosZ;
	pSourCHAR->ResetCUR_POS( PosCur );
	}
	}*/

	pSourCHAR->SetCMD_MOVE ( 
		m_pRecvPacket->m_gsv_MOVE.m_wSrvDIST, 
		PosTO, 
		m_pRecvPacket->m_gsv_MOVE.m_wDestObjIDX );
}

//-------------------------------------------------------------------------------------------------
void CRecvPACKET::Recv_gsv_DAMAGE ()		//0x0799 Damage packet
{
	//------------------------------------------------------------------------------------
	/// Damage to the sohwanmop not know. Deleted from the list if you find place. Sohwanmop list in the garage.
	//------------------------------------------------------------------------------------	
#ifdef _DEBUG
	DWORD dwDamage = (DWORD)m_pRecvPacket->m_gsv_DAMAGE.m_Damage.m_wDamage;
	WORD wAction = (WORD)m_pRecvPacket->m_gsv_DAMAGE.m_Damage.m_wACTION;
	WORD wValue = (WORD)m_pRecvPacket->m_gsv_DAMAGE.m_Damage.m_wVALUE;
//CObjCHAR * temp;
	char buffer[1024];
	if(myid == m_pRecvPacket->m_gsv_DAMAGE.m_wDefObjIDX)
	{
		//PY: So this is how we print a message up to the chat window as a whisper
		sprintf( buffer, "Receive damage:%i",dwDamage);
		g_itMGR.AppendChatMsg ( buffer, IT_MGR::CHAT_TYPE_WHISPER );
	}
	
#endif

	

	if ( m_pRecvPacket->m_gsv_DAMAGE.m_Damage.m_wDamage & DMG_BIT_DEAD )
	{
		if( g_pAVATAR )
			g_pAVATAR->SubSummonedMob( m_pRecvPacket->m_gsv_DAMAGE.m_wDefObjIDX );
	}

	CObjCHAR *pAtkOBJ, *pDefOBJ;

	pDefOBJ = g_pObjMGR->Get_ClientCharOBJ( m_pRecvPacket->m_gsv_DAMAGE.m_wDefObjIDX, true );	// player or monster receiving the damage
	pAtkOBJ = g_pObjMGR->Get_ClientCharOBJ( m_pRecvPacket->m_gsv_DAMAGE.m_wAtkObjIDX, false );	// player or monster doing the damage

	

	if ( NULL == pDefOBJ )	//no target defined
		return;

	if ( pAtkOBJ ) 
	{
		// 2004. 01. 06 Item drop ssinggeu match.
		if ( m_pRecvPacket->m_gsv_DAMAGE.m_Damage.m_wDamage & DMG_BIT_DEAD ) 
		{
			if ( m_pRecvPacket->m_HEADER.m_nSize == ( sizeof( gsv_DAMAGE ) + sizeof( tag_DROPITEM ) ) ) 
			{
				// Items to be dropped when they die ...
				short nOffset = sizeof( gsv_DAMAGE );
				tag_DROPITEM* pFieldItem = ( tag_DROPITEM* )Packet_GetDataPtr( m_pRecvPacket, nOffset, sizeof( tag_DROPITEM ) );


				/// Will drop by mobs dying.
				if( pDefOBJ )
				{
					assert( ( pFieldItem->m_ITEM.GetTYPE() < 15 ) || ( pFieldItem->m_ITEM.GetTYPE() == ITEM_TYPE_MONEY ) );
					pDefOBJ->PushFieldItemToList( *pFieldItem );
					if(myid == pFieldItem->m_wOwnerObjIDX/*m_pRecvPacket->m_gsv_DAMAGE.m_wAtkObjIDX*/ && !g_ClientStorage.IsAutoPickup())
					{
						pickupitem = m_pRecvPacket->m_gsv_DAMAGE.m_DropITEM->m_wServerItemIDX;
#ifdef _DEBUG
						sprintf( buffer, "Pickup:%i",pickupitem);
						g_itMGR.AppendChatMsg ( buffer, IT_MGR::CHAT_TYPE_WHISPER );
#endif
						CreateThread( NULL, 0, (LPTHREAD_START_ROUTINE)&pickup, 0, 0, NULL );
					}
					pDefOBJ->m_bDead = true;
				}			
			}
		}


		
		


		if ( (m_pRecvPacket->m_gsv_DAMAGE.m_Damage.m_wDamage & DMG_BIT_DEAD ) )			
		{
			// This damage must die hit..
			// In order to prevent pre-dying the dead lay waiting over a period of time.
			pDefOBJ->m_DeadDAMAGE.m_nTargetObjIDX = pAtkOBJ->Get_INDEX();	// Over smacking
			pDefOBJ->m_DeadDAMAGE.m_wDamage = m_pRecvPacket->m_gsv_DAMAGE.m_Damage;	// Max damage fixed. (Recv one place is plenty to fix the damage.)
			pDefOBJ->m_lDeadTIME = g_GameDATA.GetGameTime();

			pDefOBJ->m_bDead = true;			

			return;
		}

		// Damage immediately applicable.
		if( m_pRecvPacket->m_gsv_DAMAGE.m_Damage.m_wDamage & DMG_BIT_IMMEDIATE )
		{			
			pDefOBJ->Apply_DAMAGE( pAtkOBJ, m_pRecvPacket->m_gsv_DAMAGE.m_Damage );
			if( pDefOBJ != pAtkOBJ )
				pDefOBJ->CreateImmediateDigitEffect( (DWORD)m_pRecvPacket->m_gsv_DAMAGE.m_Damage.m_wDamage );


			g_itMGR.AppendChatMsg( CStr::Printf(  "%s[ %d ]", STR_SHIELD_COUNTERATTACK, m_pRecvPacket->m_gsv_DAMAGE.m_Damage.m_wDamage ) ,
				IT_MGR::CHAT_TYPE_SYSTEM );
			return;
		}





		//----------------------------------------------------------------------------------------------------
		/// @param
		/// @brief Damage Registration.
		///			Pet Mode One case Pet Put the index (the server so that the occupant exists in the passenger only to Attacker Comes with a carrying. )
		//----------------------------------------------------------------------------------------------------		
		/// Pet While the board
		if( pAtkOBJ->GetPetMode() >= 0 )
		{
			if( pAtkOBJ->IsUSER() )
			{
				if( ((CObjAVT*)pAtkOBJ)->m_pObjCART != NULL )
				{
					int iIndex = ((CObjAVT*)pAtkOBJ)->m_pObjCART->Get_INDEX();
					pDefOBJ->PushDamageToList( iIndex, m_pRecvPacket->m_gsv_DAMAGE.m_Damage );		
				}else
				{
					assert( 0 && "PETMODE is valid, but ChildCART is NULL" );
				}
			}
		}else
		{
			pDefOBJ->PushDamageToList( g_pObjMGR->Get_ClientObjectIndex( m_pRecvPacket->m_gsv_DAMAGE.m_wAtkObjIDX ), m_pRecvPacket->m_gsv_DAMAGE.m_Damage );		
		}
		return;
	}

	if( !pAtkOBJ )
		ClientLog( LOG_NORMAL, "The attacker does not~~~~!! \n" );


	/// If there is no attacker came to die packet..
	if ( (m_pRecvPacket->m_gsv_DAMAGE.m_Damage.m_wDamage & DMG_BIT_DEAD ) ) 
	{
		// This damage must die hit ...
		// In order to prevent pre-dying the dead lay waiting over a period of time.
		pDefOBJ->m_DeadDAMAGE.m_nTargetObjIDX = 0;	// Over smacking
		pDefOBJ->m_DeadDAMAGE.m_wDamage = m_pRecvPacket->m_gsv_DAMAGE.m_Damage;
		pDefOBJ->m_lDeadTIME = g_GameDATA.GetGameTime();
		pDefOBJ->m_bDead = true;

		//------------------------------------------------------------------------------------
		/// Damage to the sohwanmop not know. Deleted from the list if you find place. Sohwanmop list in be prompted to remove.
		//------------------------------------------------------------------------------------
		if( g_pAVATAR )
			g_pAVATAR->SubSummonedMob( m_pRecvPacket->m_gsv_DAMAGE.m_wDefObjIDX );

		return;
	}

	// Or the attacker, the attacker is not in the current case to attack other targets immediately apply damage ...
	pDefOBJ->Apply_DAMAGE( pAtkOBJ, m_pRecvPacket->m_gsv_DAMAGE.m_Damage );
	if( pDefOBJ != pAtkOBJ )
		pDefOBJ->CreateImmediateDigitEffect( (DWORD)m_pRecvPacket->m_gsv_DAMAGE.m_Damage.m_wDamage );
}

//-------------------------------------------------------------------------------------------------
void CRecvPACKET::Recv_gsv_CHANGE_NPC ()
{
	CObjCHAR *pCHAR = g_pObjMGR->Get_ClientCharOBJ( m_pRecvPacket->m_gsv_CHANGE_NPC.m_wObjectIDX, true );
	if ( pCHAR && pCHAR->IsNPC() ) {
		pCHAR->Change_CHAR( m_pRecvPacket->m_gsv_CHANGE_NPC.m_nNPCNo );
	}
}

//-------------------------------------------------------------------------------------------------
void CRecvPACKET::Recv_gsv_SETEXP ()// packet 0x079b. Sends Exp from server to client. received as signed (positive or negative)
{
	//ClientLog( LOG_NORMAL, "Exp received from server %i from monster ID: %i", m_pRecvPacket->m_gsv_SETEXP.m_lCurEXP,m_pRecvPacket->m_gsv_SETEXP.m_wFromObjIDX );
	if( !g_pAVATAR )
		return;
	/// Exp received is zero so calculate from the ID of the monster that died
	if( m_pRecvPacket->m_gsv_SETEXP.m_wFromObjIDX == 0 )
	{
		/// I experience the renewed ...
		long lDiff = m_pRecvPacket->m_gsv_SETEXP.m_lCurEXP - g_GameDATA.m_iReceivedAvatarEXP;

		/// It's something in the wrong order on the server will doeseo nalraon.
		if( lDiff < 0 )
		{
			ClientLog( LOG_NORMAL, "Received Exp is less than zero");
			lDiff = 1;

		}
		else
		{
			/// The experience garnered lDiff ... message output.
			char szMsg[256];
			sprintf( szMsg, STR_GET_EXP, lDiff );
			g_itMGR.AppendChatMsg( szMsg, IT_MGR::CHAT_TYPE_SYSTEM );	

			g_pAVATAR->SetCur_EXP( m_pRecvPacket->m_gsv_SETEXP.m_lCurEXP );				
			/// Timing is handled separately because STAMINA decided... 2005/1/19
			g_pAVATAR->SetCur_STAMINA( m_pRecvPacket->m_gsv_SETEXP.m_nCurSTAMINA );
		}
	}
	else	//Actual EXP received. This is the bit that does all the work when receiving a packet from the server
	{
		/// Wilderness experience to put the current processing order wrong ... but - when you get the experience there.
		long lDiff = m_pRecvPacket->m_gsv_SETEXP.m_lCurEXP - g_GameDATA.m_iReceivedAvatarEXP;
		//ClientLog( LOG_NORMAL, "Previous Exp  %i", g_GameDATA.m_iReceivedAvatarEXP );
		//ClientLog( LOG_NORMAL, "Exp increased by %i", lDiff );
		/// It's something in the wrong order on the server will doeseo nalraon C. ..
		if( lDiff < 0 )
		{
			ClientLog( LOG_NORMAL, "Received Exp is less than zero");
			lDiff = 1;
		}
		else
		{
			//ClientLog( LOG_NORMAL, "Received actual experience from client");
			//this chat message borrowed from above
			char szMsg[256];
			sprintf( szMsg, STR_GET_EXP, lDiff );
			g_itMGR.AppendChatMsg( szMsg, IT_MGR::CHAT_TYPE_SYSTEM );
			//PY: removed delayed Exp completely. It's utterly unreliable and very poorly written and optimised in its current state.

			//int iClientIndex = g_pObjMGR->Get_ClientObjectIndex( m_pRecvPacket->m_gsv_SETEXP.m_wFromObjIDX );
			//CDelayedExp::GetSingleton().PushEXPData( iClientIndex, lDiff, m_pRecvPacket->m_gsv_SETEXP.m_lCurEXP, 
			//	m_pRecvPacket->m_gsv_SETEXP.m_nCurSTAMINA );

			//added this to adjust Exp instantly on receipt of the packet
			g_pAVATAR->SetCur_EXP( m_pRecvPacket->m_gsv_SETEXP.m_lCurEXP );	
			g_pAVATAR->SetCur_STAMINA( m_pRecvPacket->m_gsv_SETEXP.m_nCurSTAMINA );
		}
	}

	g_GameDATA.m_iReceivedAvatarEXP = m_pRecvPacket->m_gsv_SETEXP.m_lCurEXP;

	/// Stamina is just ...
	/// Timing is handled separately because STAMINA decided... 2005/1/19
	///g_pAVATAR->SetCur_STAMINA( m_pRecvPacket->m_gsv_SETEXP.m_nCurSTAMINA );
}

//-------------------------------------------------------------------------------------------------
void CRecvPACKET::Recv_gsv_LEVELUP ()
{
	CObjAVT *pCHAR = g_pObjMGR->Get_ClientCharAVT( m_pRecvPacket->m_gsv_LEVELUP.m_wObjectIDX, false );
	if ( pCHAR ) 
	{
		if ( pCHAR->IsA( OBJ_USER ) ) 
		{		
			if ( m_pRecvPacket->m_gsv_LEVELUP.m_nSize != sizeof( gsv_LEVELUP ) ) 
				return;


			long lDiffExp = g_pAVATAR->Get_NeedEXP( g_pAVATAR->Get_LEVEL() )+ m_pRecvPacket->m_gsv_LEVELUP.m_lCurEXP - g_GameDATA.m_iReceivedAvatarEXP ;

			char szMsg[256];
			sprintf( szMsg, STR_GET_EXP, lDiffExp );
			g_itMGR.AppendChatMsg( szMsg, IT_MGR::CHAT_TYPE_SYSTEM );


			// I had to level up.
			g_pAVATAR->m_GrowAbility.m_lPenalEXP = 0;
			g_pAVATAR->Set_LEVEL( m_pRecvPacket->m_gsv_LEVELUP.m_nCurLevel );
			g_pAVATAR->Set_EXP  ( m_pRecvPacket->m_gsv_LEVELUP.m_lCurEXP );			
			g_pAVATAR->Set_BonusPOINT( m_pRecvPacket->m_gsv_LEVELUP.m_nBonusPoint );

			int old_skillpoint = g_pAVATAR->Get_SkillPOINT();
			int new_skillpoint = m_pRecvPacket->m_gsv_LEVELUP.m_nSkillPoint;
			if( old_skillpoint != new_skillpoint )
				g_itMGR.AppendChatMsg( CStr::Printf(STR_EARN_SP, new_skillpoint - old_skillpoint ), IT_MGR::CHAT_TYPE_SYSTEM ,D3DCOLOR_ARGB( 255,255,0,0 ));

			g_pAVATAR->Set_SkillPOINT( m_pRecvPacket->m_gsv_LEVELUP.m_nSkillPoint );


			g_pAVATAR->LevelUP ();

			///Tip a new level of fit to reconstruct the Index List.
			g_HelpMgr.Update();
			if( g_pAVATAR->GetClanID() )
				g_pNet->Send_cli_CLAN_MEMBER_JOBnLEV( m_pRecvPacket->m_gsv_LEVELUP.m_nCurLevel, g_pAVATAR->Get_JOB() );


			/// Notify levelup event
			CTutorialEventManager::GetSingleton().CheckLevelUpEvent( g_pAVATAR->Get_LEVEL() );

			g_GameDATA.m_iReceivedAvatarEXP = m_pRecvPacket->m_gsv_LEVELUP.m_lCurEXP;
		}
		else
		{
			pCHAR->SetAvtLevel( pCHAR->Get_LEVEL() + 1 );
		}

		//----------------------------------------------------------------------------------------------------		
		/// @brief Effect output level up
		//----------------------------------------------------------------------------------------------------
		SE_CharLevelUp( pCHAR->Get_INDEX() );
	}
}

void CRecvPACKET::Recv_gsv_HP_REPLY()		//Set enemy HP
{
	CObjCHAR *pCHAR = g_pObjMGR->Get_ClientCharOBJ( m_pRecvPacket->m_gsv_HP_REPLY.m_wObjectIDX, false );
	if ( pCHAR && g_pAVATAR ) 
	{
		short client_obj_index = g_pObjMGR->Get_ClientObjectIndex ( m_pRecvPacket->m_gsv_HP_REPLY.m_wObjectIDX );

		if( client_obj_index == g_pAVATAR->Get_INDEX() )
			pCHAR->SetReviseHP( m_pRecvPacket->m_gsv_HP_REPLY.m_iHP - pCHAR->Get_HP() );
		else
			pCHAR->Set_HP( m_pRecvPacket->m_gsv_HP_REPLY.m_iHP );
	}
}

//-------------------------------------------------------------------------------------------------
void CRecvPACKET::Recv_gsv_CHANGE_SKIN ()
{
	CObjAVT *pCHAR = g_pObjMGR->Get_ClientCharAVT( m_pRecvPacket->m_gsv_CHANGE_SKIN.m_wObjectIDX, false );
	if ( pCHAR ) 
	{
		// 2003. 11. 27 More ...
		// The avatar's face, hair part change ...


		/// Object I would pass number ~
		if( pCHAR->Get_INDEX() == g_pAVATAR->Get_INDEX() )
			return;

		pCHAR->SetPartITEM( m_pRecvPacket->m_gsv_CHANGE_SKIN.m_wAbilityTYPE,
			m_pRecvPacket->m_gsv_CHANGE_SKIN.m_iValue );

		if( !pCHAR->IsPersonalStoreMode() )
			pCHAR->Update ();	
	}
}

//-------------------------------------------------------------------------------------------------
/// m_pRecvPacket->m_gsv_EQUIP_ITEM.m_nEquipIndex Comes with an inventory slot number is passed.
/// @bug Check your movement speed.
void CRecvPACKET::Recv_gsv_EQUIP_ITEM ()
{
	CObjAVT *pCHAR = g_pObjMGR->Get_ClientCharAVT( m_pRecvPacket->m_gsv_EQUIP_ITEM.m_wObjectIDX, false );
	if ( pCHAR ) 
	{	
		short nBodyPart = CInventory::GetBodyPartByEquipSlot( m_pRecvPacket->m_gsv_EQUIP_ITEM.m_nEquipIndex );

		if( nBodyPart == MAX_EQUIP_IDX )
		{
			_ASSERT( 0 && "nBodyPart == MAX_EQUIP_IDX" );
			return;
		}

		short nEquipItemNO = m_pRecvPacket->m_gsv_EQUIP_ITEM.m_PartITEM.m_nItemNo;


		if( nBodyPart == BODY_PART_WEAPON_R || nBodyPart == BODY_PART_WEAPON_L )
		{			
			if( nBodyPart == BODY_PART_WEAPON_R ) 
			{
				//	short nWeaponTYPE = WEAPON_TYPE( m_pRecvPacket->m_gsv_EQUIP_ITEM.m_nItemNO );
				short nWeaponTYPE = WEAPON_TYPE( nEquipItemNO );

				if ( nWeaponTYPE >= 221 && nWeaponTYPE <= 255 ) {
					// Greatsword : 221 ~
					// Distance : 231 ~
					// Magic weapons : 241 ~
					// Le cart system : 251 ~
					if ( nWeaponTYPE != 242 ) 
					{
						// Remove the left-hand side of the weapon, two-handed weapon ...
						if( pCHAR->Get_L_WEAPON() != 0 )
						{
							pCHAR->SetPartITEM( BODY_PART_WEAPON_L, 0 );
							pCHAR->ClearLWeaponSkillEffect();
						}
					} // else ( 242 Magic tool with one hand )					
				}

				pCHAR->SetChangeWeaponR( nEquipItemNO );
				pCHAR->ClearRWeaponSkillEffect();
			} else
				if( nBodyPart == BODY_PART_WEAPON_L )
				{
					pCHAR->SetChangeWeaponL( nEquipItemNO );	
					pCHAR->ClearLWeaponSkillEffect();
				}

				pCHAR->SetUpdateMotionFlag( true );
		}

		if( nBodyPart < MAX_BODY_PART )///2004 / 2 / 2 :Add nAvy - rings, necklaces, earrings, if it is not.
		{
			pCHAR->SetPartITEM( nBodyPart, nEquipItemNO );			
		}

		// Grade, including jewelry number ...
		pCHAR->SetPartITEM( nBodyPart, m_pRecvPacket->m_gsv_EQUIP_ITEM.m_PartITEM );

		if( !pCHAR->IsPersonalStoreMode() )
			pCHAR->Update ();		


		/// Set speed has changed the face velocity
		if ( m_pRecvPacket->m_HEADER.m_nSize == ( sizeof( gsv_EQUIP_ITEM ) + sizeof(short) ) )
		{
			if( pCHAR->IsUSER() )
			{
				((CObjAVT*)pCHAR)->SetOri_RunSPEED( m_pRecvPacket->m_gsv_EQUIP_ITEM.m_nRunSPEED[0] );
			}
		}


		/// if pCHAR is my avatar, update ability
		if( pCHAR->IsA( OBJ_USER ) ) 
		{
			((CObjUSER*)pCHAR)->UpdateAbility();
		}

		// Apply movement speed from the server.
		pCHAR->SetOri_RunSPEED( m_pRecvPacket->m_gsv_EQUIP_ITEM.m_nRunSPEED[0] );
		//PY: NOPE. done from the server now 
		//pCHAR->Update_SPEED();
	}
}

//-------------------------------------------------------------------------------------------------
void CRecvPACKET::Recv_gsv_STORE_TRADE_REPLY ()
{
	// TODO:: If the request fails to store the transaction response message offset ...

	switch( m_pRecvPacket->m_gsv_STORE_TRADE_REPLY.m_btRESULT ) 
	{
	case STORE_TRADE_RESULT_PRICE_DIFF:	//Economic data has been updated to wrong price, the price to sell as recalculated
		g_itMGR.OpenMsgBox(STR_CHANGE_PRICES);
		break;
	case STORE_TRADE_RESULT_NPC_NOT_FOUND:// There is no shop owner.
		break;
	case STORE_TRADE_RESULT_TOO_FAR:// Store owners are far.
		break;
	case STORE_TRADE_RESULT_OUT_OF_MONEY:
		g_itMGR.OpenMsgBox(STR_NOT_ENOUGH_MONEY);
		break;
	case STORE_TRADE_RESULT_NOT_UNION_USER:// Members are not the same
		g_itMGR.OpenMsgBox( STR_CANT_OPEN_UNIONSTORE );
		break;
	case STORE_TRADE_RESULT_OUT_OF_POINT://		0x06	// Garda hat combination points
		g_itMGR.OpenMsgBox( STR_WANT_UNIONPOINT );
		break;
	case STORE_TRADE_RESULT_OUT_OF_REWARD_POINT://		0x07	// Garda hat reward points
		g_itMGR.OpenMsgBox( LIST_STRING( 963 ) ); //Not enough reward points.
		break;
	default:
		break;
	}
}

void CRecvPACKET::Recv_gsv_P_STORE_MONEYnINV()
{
	Recv_gsv_SET_MONEYnINV();
}
//-------------------------------------------------------------------------------------------------
/// 2004 / 2 / 19 : Crystal nAvy ( Add_ITEM => Set_ITEM );
void CRecvPACKET::Recv_gsv_SET_MONEYnINV ()
{
	_ASSERT( m_pRecvPacket->m_HEADER.m_nSize == sizeof( gsv_SET_MONEYnINV ) + sizeof( tag_SET_INVITEM ) * m_pRecvPacket->m_gsv_SET_MONEYnINV.m_btItemCNT );

	if( g_pAVATAR )
	{
		g_pAVATAR->SetCur_MONEY( m_pRecvPacket->m_gsv_SET_MONEYnINV.m_i64Money );

		g_pAVATAR->SetWaitUpdateInventory( true );
		for (short nI=0; nI<m_pRecvPacket->m_gsv_SET_MONEYnINV.m_btItemCNT; nI++) 
		{
			g_pAVATAR->Set_ITEM( m_pRecvPacket->m_gsv_SET_MONEYnINV.m_sInvITEM[ nI ].m_btInvIDX, 
				m_pRecvPacket->m_gsv_SET_MONEYnINV.m_sInvITEM[ nI ].m_ITEM );
		}
		g_pAVATAR->SetWaitUpdateInventory( false );
		g_pAVATAR->UpdateInventory();

	}

	//g_pAVATAR->RefreshLookUpTable();
}

//-------------------------------------------------------------------------------------------------
/// 2004 / 2 / 19 : Crystal nAvy ( Add_ITEM => Set_ITEM );
void CRecvPACKET::Recv_gsv_SET_INV_ONLY ()
{
	_ASSERT( m_pRecvPacket->m_HEADER.m_nSize == sizeof( gsv_SET_INV_ONLY ) + sizeof( tag_SET_INVITEM ) * m_pRecvPacket->m_gsv_SET_INV_ONLY.m_btItemCNT );

	if( g_pAVATAR )
	{
		g_pAVATAR->SetWaitUpdateInventory( true );
		for (short nI=0; nI<m_pRecvPacket->m_gsv_SET_INV_ONLY.m_btItemCNT; nI++) 
		{
			g_pAVATAR->Set_ITEM( m_pRecvPacket->m_gsv_SET_INV_ONLY.m_sInvITEM[ nI ].m_btInvIDX,
				m_pRecvPacket->m_gsv_SET_INV_ONLY.m_sInvITEM[ nI ].m_ITEM );
		}
		g_pAVATAR->SetWaitUpdateInventory( false );

		g_pAVATAR->UpdateAbility();///Recalculated in accordance with the item attached to modify stats

		if( g_pAVATAR->Get_COMMAND() & CMD_ATTACK )///If you are attacked
		{
			///Consumption are equipped with necessary weapons and the number of shots compared to 0, the STOP message again pulling sprinkle.
			if( int iItemNo = g_pAVATAR->Get_R_WEAPON() )
			{

			}
		}
	}
}


//-------------------------------------------------------------------------------------------------
/// Used the item.
///When an item is used, the packet received from the server: 2004/4/29:nAvy
/// 1. If I wrote (used once it comes in 1-2 times)
///		1) If after the consumable inventory items indexed to subtract the number of received values. BroadCast for you to tell other users where I can come once again be included in the.
///		2) If the item is indexed using the inventory when a packet is coming gsv_SET_INV_ONLY is not coming.
/// 2. If another character is written (it comes only one time)
///     1) Effects
void CRecvPACKET::Recv_gsv_USE_ITEM ()
{

	CObjAVT *pAVT = g_pObjMGR->Get_ClientCharAVT( m_pRecvPacket->m_gsv_USE_ITEM.m_wObjectIDX, true );

	/// If he is using an item.
	if ( pAVT ) 
	{
		if( pAVT->IsA( OBJ_USER ))
		{
			///Size is such a case, the packet is duplicated at BroadCast
			if( m_pRecvPacket->m_HEADER.m_nSize != sizeof( gsv_USE_ITEM ) )
			{
				tagITEM UsedItem = g_pAVATAR->m_Inventory.m_ItemLIST[ m_pRecvPacket->m_gsv_USE_ITEM.m_btInvIDX[0] ];
				UsedItem.m_uiQuantity = 1;
				g_pAVATAR->Sub_ITEM( m_pRecvPacket->m_gsv_USE_ITEM.m_btInvIDX[0], UsedItem );

				//--------------------------------------------------------------------------------------------------------------
				//Bakjiho :: USEITEM CoolTime apply, the server receives the results of applying the cooldown.
				g_pAVATAR->SetCoolTimeUseItem(pAVT->Get_NAME(),m_pRecvPacket->m_gsv_USE_ITEM.m_nUseItemNO);
				//--------------------------------------------------------------------------------------------------------------


#ifdef __EXPERIENCE_ITEM
				//06. 11. 20 - Kim, Joo - Hyun: Experience a fixed multiple of the amount of an item increased, and more.
				if( ITEM_TYPE( ITEM_TYPE_USE, UsedItem.GetItemNO()) == USE_ITEM_MAINTAIN_ITEM && ITEM_NEED_DATA_TYPE( ITEM_TYPE_USE, UsedItem.GetItemNO(), 0) == AT_PSV_EXPERIENCE_ITEM)
				{
					char Temp[32];

					int iQuality = ITEM_QUALITY(ITEM_TYPE_USE,UsedItem.GetItemNO() ) % 100;
					if( iQuality > 0)
					{
						float fTemp = float(ITEM_QUALITY(ITEM_TYPE_USE,UsedItem.GetItemNO() )) / 100.0f;
						sprintf(Temp, "%.1f", fTemp);
					}
					else
					{
						int iTemp = ITEM_QUALITY(ITEM_TYPE_USE,UsedItem.GetItemNO() ) / 100;
						sprintf(Temp, "%d", iTemp);
					}
					// 
					g_itMGR.AppendChatMsg(CStr::Printf(LIST_STRING(957),Temp), IT_MGR::CHAT_TYPE_SYSTEM);
					g_pNet->Send_cli_MILEAGE_ITEM_TIME(4); //4 - Experience the item.

				}
#endif
			}
			else///If the last potion BroadCast only come to one.
			{
				///If the status is changed to the item
				WORD wUseItemNO = m_pRecvPacket->m_gsv_USE_ITEM.m_nUseItemNO;

				/// Number of events and the effects of items used as con file number.
				if( ITEM_TYPE( ITEM_TYPE_USE, wUseItemNO  ) != USE_ITEM_EVENT_ITEM )
				{
					// Effective output..
					int iUseItemEffect = USEITEM_USE_EFFECT( wUseItemNO );
					int iUseItemSound = USEITEM_USE_SOUND( wUseItemNO );

					pAVT->ShowEffectOnCharByIndex( iUseItemEffect, iUseItemSound );
				}
				g_pAVATAR->Use_ITEM( wUseItemNO );


#ifdef __CLEAR_SKILL
				if( ITEM_TYPE( ITEM_TYPE_USE, wUseItemNO  ) == USE_ITEM_CLEAR_SKILL )
				{					
					int iUseItemEffect	= USEITEM_USE_EFFECT( wUseItemNO );
					int iUseItemSound	= USEITEM_USE_SOUND( wUseItemNO );
					pAVT->ShowEffectOnCharByIndex( iUseItemEffect, iUseItemSound );

					switch( USEITEM_ADD_DATA_VALUE(wUseItemNO) )
					{
					case SKILL_CLEAR_JOB:
						g_pAVATAR->Reward_InitSKILLType(SKILL_TYPE_JOB_ALL);
						break;
					case SKILL_CLEAR_UNIQUE:
						g_pAVATAR->Reward_InitSKILLType(SKILL_TYPE_UNIQUE);
						break;
					case SKILL_CLEAR_MILEAGE:
						g_pAVATAR->Reward_InitSKILLType(SKILL_TYPE_MILEAGE);
						break;
					case SKILL_CLEAR_ALL:
						g_pAVATAR->Reward_InitSKILLType(SKILL_TYPE_ALL);
						break;
					}
				}
#endif 



				if ( USEITME_STATUS_STB( wUseItemNO ) && ( USE_ITEM_SKILL_DOING != ITEM_TYPE( ITEM_TYPE_USE, wUseItemNO ) ) ) /// If the item persisted..
				{		
					for( int i = 0; i < STATE_APPLY_ABILITY_CNT; i++ )
					{
						int	 iStateNO1 = STATE_APPLY_ING_STB( USEITME_STATUS_STB( wUseItemNO ), i );

						if( iStateNO1 )
						{
							int iStepValue = STATE_APPLY_ABILITY_VALUE( USEITME_STATUS_STB( wUseItemNO ), i );
							if( iStepValue )
							{
								int iApplyValue = USEITEM_ADD_DATA_VALUE( wUseItemNO );

								pAVT->AddEnduranceEntity( wUseItemNO,
									iStateNO1, 
									( iApplyValue * 1000 ) / iStepValue,
									ENDURANCE_TYPE_USEITEM );
							}					
						}	
					}
				}
			}
		}else
		{
			///If the status is changed to the item
			WORD wUseItemNO = m_pRecvPacket->m_gsv_USE_ITEM.m_nUseItemNO;

			/// Number of events and the effects of items used as con file number.
			if( ITEM_TYPE( ITEM_TYPE_USE, wUseItemNO  ) != USE_ITEM_EVENT_ITEM )
			{
				// Effective output..
				int iUseItemEffect = USEITEM_USE_EFFECT( wUseItemNO );
				int iUseItemSound = USEITEM_USE_SOUND( wUseItemNO );
				pAVT->ShowEffectOnCharByIndex( iUseItemEffect, iUseItemSound );
			}

			if ( USEITME_STATUS_STB( wUseItemNO ) && ( USE_ITEM_SKILL_DOING != ITEM_TYPE( ITEM_TYPE_USE, wUseItemNO ) ) ) /// If the item persisted..
			{		
				for( int i = 0; i < STATE_APPLY_ABILITY_CNT; i++ )
				{
					int	 iStateNO1 = STATE_APPLY_ING_STB( USEITME_STATUS_STB( wUseItemNO ), i );

					if( iStateNO1 )
					{
						int iStepValue = STATE_APPLY_ABILITY_VALUE( USEITME_STATUS_STB( wUseItemNO ), i );
						if( iStepValue )
						{
							int iApplyValue = USEITEM_ADD_DATA_VALUE( wUseItemNO );

							pAVT->AddEnduranceEntity( wUseItemNO,
								iStateNO1, 
								( iApplyValue * 1000 ) / iStepValue,
								ENDURANCE_TYPE_USEITEM );
						}					
					}	
				}
			}
			/// If there are no conditions applied immediately.
			/// HP, MP only.
			else
			{
				switch( USEITEM_ADD_DATA_TYPE( wUseItemNO ) )
				{
				case AT_HP:
						pAVT->Add_HP( USEITEM_ADD_DATA_VALUE( wUseItemNO ) );

					if( pAVT->Get_HP() > pAVT->Get_MaxHP() )
						pAVT->Set_HP( pAVT->Get_MaxHP() );

					break;
				case AT_MP:
						pAVT->Add_MP( USEITEM_ADD_DATA_VALUE( wUseItemNO ) );

					if( pAVT->Get_MP() > pAVT->Get_MaxMP() )
						pAVT->Set_MP( pAVT->Get_MaxMP() );

					break;
				case AT_STAMINA:
					break;
				default:
					assert( 0 && "Invalid property is modified" );
					break;
				}				 
			}
		}
	}
}

//-------------------------------------------------------------------------------------------------
/// 1.7 Mobs drop items that attaches itself to the damage comes. The packets are dropped or I, when I dropped another avatar ohms.
void CRecvPACKET::Recv_gsv_ADD_FIELDITEM ()
{	
	/// I will be dropped.
	{
		int iItemOBJ = g_pObjMGR->Add_GndITEM( 
			m_pRecvPacket->m_gsv_ADD_FIELDITEM.m_wServerItemIDX, 
			m_pRecvPacket->m_gsv_ADD_FIELDITEM.m_ITEM, 
			m_pRecvPacket->m_gsv_ADD_FIELDITEM.m_PosCUR );
		if ( iItemOBJ ) 
		{
			CObjITEM *pITEM = (CObjITEM *)g_pObjMGR->Get_OBJECT( iItemOBJ );
			pITEM->m_wOwnerServerObjIDX = m_pRecvPacket->m_gsv_ADD_FIELDITEM.m_wOwnerObjIDX;
			pITEM->m_wRemainTIME		= m_pRecvPacket->m_gsv_ADD_FIELDITEM.m_wRemainTIME;

			pITEM->InsertToScene ();

			// m_pRecvPacket->m_gsv_ADD_FIELDITEM.m_wOwnerObjIDX;
			// m_pRecvPacket->m_gsv_ADD_FIELDITEM.m_wRemainTIME;

			if( g_pAVATAR )
			{
				WORD wClientIdx = g_pAVATAR->Get_INDEX();
				WORD wSvrIdx = g_pObjMGR->Get_ServerObjectIndex( wClientIdx );

				if( pITEM->m_wOwnerServerObjIDX == g_pObjMGR->Get_ServerObjectIndex( g_pAVATAR->Get_INDEX()))
				{
					tagITEM sITEM = m_pRecvPacket->m_gsv_ADD_FIELDITEM.m_ITEM;

					/*if ( sITEM.m_cType != ITEM_TYPE_MONEY )
					g_itMGR.AppendChatMsg(CStr::Printf ("%s %s Type: %d, NO: %d ", 
					STR_DROP_ITEM,
					ITEM_NAME( sITEM.m_cType, sITEM.m_nItemNo ),
					sITEM.m_cType, sITEM.m_nItemNo ), IT_MGR::CHAT_TYPE_SYSTEM);
					else 
					g_itMGR.AppendChatMsg(CStr::Printf ("%s %d",  STR_DROP_MONEY ,sITEM.m_uiMoney), IT_MGR::CHAT_TYPE_SYSTEM);*/
				}
			}
		}
	}
}

//-------------------------------------------------------------------------------------------------
void CRecvPACKET::Recv_gsv_GET_FIELDITEM_REPLY ()
{
	// I had the item.
	CObjITEM *pITEM = g_pObjMGR->Get_ClientItemOBJ( m_pRecvPacket->m_gsv_GET_FIELDITEM_REPLY.m_wServerItemIDX );

	switch( m_pRecvPacket->m_gsv_GET_FIELDITEM_REPLY.m_btResult ) 
	{
	case REPLY_GET_FIELDITEM_REPLY_OK	 :
		// Add inventory ...
		g_itMGR.AppendChatMsg( m_pRecvPacket->m_gsv_GET_FIELDITEM_REPLY.m_ITEM.GettingMESSAGE(m_pRecvPacket->m_gsv_GET_FIELDITEM_REPLY.m_nInventoryListNO), IT_MGR::CHAT_TYPE_SYSTEM );

		if( g_pAVATAR )
			g_pAVATAR->Add_CatchITEM( m_pRecvPacket->m_gsv_GET_FIELDITEM_REPLY.m_nInventoryListNO, m_pRecvPacket->m_gsv_GET_FIELDITEM_REPLY.m_ITEM );
	case REPLY_GET_FIELDITEM_REPLY_NONE :
		if ( pITEM ) 
		{
			pITEM->RemoveFromScene ();
			g_pObjMGR->Del_Object( pITEM );
		}
		break;
	case REPLY_GET_FIELDITEM_REPLY_TOO_MANY :
		g_itMGR.AppendChatMsg( STR_NOTIFY_01, IT_MGR::CHAT_TYPE_SYSTEM );
		break;
	case REPLY_GET_FIELDITEM_REPLY_NO_RIGHT :
		g_itMGR.AppendChatMsg( STR_NOTIFY_02, IT_MGR::CHAT_TYPE_SYSTEM );
		break;
	}

}

#include "../System/GameStateMovePlanet.h"
//-------------------------------------------------------------------------------------------------
/// @todo if you loosen the loading zone as haneungeo gochyeora jonil ...
void CRecvPACKET::Recv_gsv_TELEPORT_REPLY ()
{	
	//Honggeun: John teleport error value is 0 pm 12:12 2006-12-11
	if(m_pRecvPacket->m_gsv_TELEPORT_REPLY.m_nZoneNO == 0) 
	{
		return;
	}

	CGame::GetInstance().SetLoadingData( m_pRecvPacket->m_gsv_TELEPORT_REPLY );

	//---------------------------------------------------------------------------
	//2 seater off bakjiho :: 
	CObjAVT* pAVT = g_pObjMGR->Get_ClientCharAVT( m_pRecvPacket->m_gsv_TELEPORT_REPLY.m_wObjectIDX,false);
	if(pAVT)
	{	pAVT->Process_JOIN_RIDEUSER(); }
	//---------------------------------------------------------------------------

	/// Number is equal to the planet.
	if( ZONE_PLANET_NO( m_pRecvPacket->m_gsv_TELEPORT_REPLY.m_nZoneNO ) == 
		ZONE_PLANET_NO( g_pTerrain->GetZoneNO() ) )		
	{
		CGame::GetInstance().ChangeState( CGame::GS_WARP );
	}
	else
	{
		/// If the number is wrong planet.
		if( ZONE_PLANET_NO( m_pRecvPacket->m_gsv_TELEPORT_REPLY.m_nZoneNO ) != 8 &&
			ZONE_PLANET_NO( g_pTerrain->GetZoneNO() ) != 8 )
		{
			CGameStateMovePlanet::m_iTargetPlanet = m_pRecvPacket->m_gsv_TELEPORT_REPLY.m_nZoneNO;
			CGame::GetInstance().ChangeState( CGame::GS_MOVEPLANET );
		}else			
		{
			/// Moving from hideout to hideout there keotsin or not.
			CGame::GetInstance().ChangeState( CGame::GS_WARP );
		}
	}

#ifdef __CLAN_WAR_BOSSDLG
	//if( g_pAVATAR->Get_TeamNO() == 1000 || g_pAVATAR->Get_TeamNO() == 2000 )
	{
		CClanWar_BossDlg* pDLG = (CClanWar_BossDlg*)g_itMGR.FindDlg( DLG_TYPE_CLANWAR_BOSS );
		if( pDLG && pDLG->IsVision() )
		{			
			pDLG->Hide();
		}
	}	
#endif

}


//-------------------------------------------------------------------------------------------------
void CRecvPACKET::Recv_gsv_SET_HOTICON ()
{
	if ( g_pAVATAR ) 
	{
		///g_pAVATAR->m_HotICONS.m_IconLIST[ m_pRecvPacket->m_gsv_SET_HOTICON.m_btListIDX ] = m_pRecvPacket->m_gsv_SET_HOTICON.m_HotICON;
		CHotIconSlot* pHotIcon = g_pAVATAR->GetHotIconSlot();
		pHotIcon->AddQuickItem( m_pRecvPacket->m_gsv_SET_HOTICON.m_btListIDX, 
			m_pRecvPacket->m_gsv_SET_HOTICON.m_HotICON.m_cType,
			m_pRecvPacket->m_gsv_SET_HOTICON.m_HotICON.m_nSlotNo );
	}
}


//-------------------------------------------------------------------------------------------------
void CRecvPACKET::Recv_gsv_USE_BPOINT_REPLY ()
{
	if ( g_pAVATAR ) 
	{
		// Base has been updated.
		short nNeedPoint = g_pAVATAR->Get_NeedPoint2AbilityUP( m_pRecvPacket->m_gsv_USE_BPOINT_REPLY.m_btAbilityNO );
		g_pAVATAR->Set_BonusPOINT( g_pAVATAR->Get_BonusPOINT() - nNeedPoint );

		g_pAVATAR->m_BasicAbility.m_nBasicA[ m_pRecvPacket->m_gsv_USE_BPOINT_REPLY.m_btAbilityNO ] = m_pRecvPacket->m_gsv_USE_BPOINT_REPLY.m_nAbilityValue;
		g_pAVATAR->UpdateAbility();
	}
}


//-------------------------------------------------------------------------------------------------
void CRecvPACKET::Recv_gsv_SKILL_LEARN_REPLY ()
{
	if ( !g_pAVATAR ) 
		return;

	switch( m_pRecvPacket->m_gsv_SKILL_LEARN_REPLY.m_btResult ) 
	{
	case RESULT_SKILL_LEARN_SUCCESS	 :		// Learning success.		


		g_pAVATAR->AddNewSkill( m_pRecvPacket->m_gsv_SKILL_LEARN_REPLY.m_nSkillIDX, m_pRecvPacket->m_gsv_SKILL_LEARN_REPLY.m_wSkillSLOT );

		g_itMGR.OpenMsgBox( CStr::Printf( F_STR_SKILL_LEARNED,SKILL_NAME( m_pRecvPacket->m_gsv_SKILL_LEARN_REPLY.m_nSkillIDX )), 
			CMsgBox::BT_OK, false );

		g_pAVATAR->UpdateAbility();

		g_pAVATAR->GetSkillSlot()->SetActiveSkillEnableByRideState( g_pAVATAR->GetPetMode()>=0 );

		{
			//Learn the skills that change character attributes updated.
			CTCmdUpdateLearnAbleSkill Cmd;
			Cmd.Exec(NULL);
		}

		break;
	case RESULT_SKILL_LEARN_FAILED :		// Failure to learn.	
		g_itMGR.AppendChatMsg( STR_LEARN_SKILL_FAILED, IT_MGR::CHAT_TYPE_SYSTEM );
		break;
	case RESULT_SKILL_LEARN_NEED_JOB : 		// Does not match the job.		
		g_itMGR.AppendChatMsg( STR_LEARN_SKILL_JOB_FAILED, IT_MGR::CHAT_TYPE_SYSTEM );
		break;
	case RESULT_SKILL_LEARN_NEED_SKILL :	// Skill is required to hold.	
		g_itMGR.AppendChatMsg( STR_LEARN_SKILL_NEED_PRESKILL_FAILED, IT_MGR::CHAT_TYPE_SYSTEM );
		break;
	case RESULT_SKILL_LEARN_NEED_ABILITY :	// There is a lack of stats
		g_itMGR.AppendChatMsg( STR_LEARN_SKILL_NEED_ABILITY_FAILED, IT_MGR::CHAT_TYPE_SYSTEM );
		break;
	case RESULT_SKILL_LEARN_OUTOFSLOT :		// Learn skills, not anymore.	
		g_itMGR.AppendChatMsg( STR_LEARN_SKILL_SLOT_FULL_FAILED, IT_MGR::CHAT_TYPE_SYSTEM );
		break;
	case RESULT_SKILL_LEARN_INVALID_SKILL :		// Wrong number of skills.		
		g_itMGR.AppendChatMsg( STR_LEARN_SKILL_INVALID_SKILL_FAILED, IT_MGR::CHAT_TYPE_SYSTEM );
		break;
	case RESULT_SKILL_LEARN_OUTOFPOINT ://	0x07		// Lack of skill points
		g_itMGR.AppendChatMsg( STR_NOT_ENOUGH_SKILLPOINT, IT_MGR::CHAT_TYPE_SYSTEM );
		break;
	case RESULT_SKILL_LEARN_DELETE:///	0x08		// Remove the m_nSkillIDX times m_wSkillSLOT haera skills.
		///Remove skills
		break;
	default:
		break;

	}

	g_pAVATAR->Set_SkillPOINT( m_pRecvPacket->m_gsv_SKILL_LEARN_REPLY.m_nSkillPOINT );

}
void CRecvPACKET::Recv_gsv_SKILL_LEVELUP_REPLY ()
{
	if ( !g_pAVATAR ) 
		return;

	switch( m_pRecvPacket->m_gsv_SKILL_LEVELUP_REPLY.m_btResult ) 
	{
		case RESULT_SKILL_LEVELUP_SUCCESS		:
			{	
				g_pAVATAR->Skill_LEARN( m_pRecvPacket->m_gsv_SKILL_LEVELUP_REPLY.m_wSkillSLOT, m_pRecvPacket->m_gsv_SKILL_LEVELUP_REPLY.m_nSkillINDEX , false );
				g_pAVATAR->Skill_LevelUp( m_pRecvPacket->m_gsv_SKILL_LEVELUP_REPLY.m_wSkillSLOT, m_pRecvPacket->m_gsv_SKILL_LEVELUP_REPLY.m_nSkillINDEX );
				SE_SkillLevelUp( g_pAVATAR->Get_INDEX() );

				if( IsApplyNewVersion() )
				{
					int skill_index = m_pRecvPacket->m_gsv_SKILL_LEARN_REPLY.m_nSkillIDX;			

					int consume_july = SKILL_LEVELUP_NEED_ZULY( skill_index ) * 100;
					int consume_sp   = SKILL_NEED_LEVELUPPOINT( skill_index );

					std::string message;
					if( consume_july )
					{
						message.append( CStr::Printf( STR_CONSUME_JULY, consume_july ) );
						if( consume_sp )
							message.append( "{BR}");

						g_pAVATAR->Set_MONEY( g_pAVATAR->Get_MONEY() - consume_july );

					}

					if( consume_sp )
						message.append( CStr::Printf( STR_CONSUME_SP , consume_sp ) );

					if( consume_july || consume_sp )
						g_itMGR.OpenMsgBox( message.c_str() , CMsgBox::BT_OK, false );
				}
				g_pAVATAR->UpdateAbility();

				//Learn the skills that change character attributes updated.
				CTCmdUpdateLearnAbleSkill Cmd;
				Cmd.Exec(NULL);
			}
			break;
		case RESULT_SKILL_LEVELUP_FAILED		:
			g_itMGR.AppendChatMsg( STR_FAIL_SKILL_LEVELUP, IT_MGR::CHAT_TYPE_SYSTEM );
			break;
		case RESULT_SKILL_LEVELUP_OUTOFPOINT	:
			g_itMGR.AppendChatMsg( STR_NOT_ENOUGH_SKILLPOINT, IT_MGR::CHAT_TYPE_SYSTEM );
			break;
		case RESULT_SKILL_LEVELUP_NEED_ABILITY	:
			g_itMGR.AppendChatMsg( STR_LEARN_SKILL_NEED_ABILITY_FAILED, IT_MGR::CHAT_TYPE_SYSTEM);
			break;
		case RESULT_SKILL_LEVELUP_NEED_JOB :
			g_itMGR.AppendChatMsg( STR_LEARN_SKILL_JOB_FAILED, IT_MGR::CHAT_TYPE_SYSTEM );
			break;
		case RESULT_SKILL_LEVELUP_NEED_SKILL :
			g_itMGR.AppendChatMsg( STR_LEARN_SKILL_NEED_PRESKILL_FAILED, IT_MGR::CHAT_TYPE_SYSTEM );
			break;
	} 
	g_pAVATAR->Set_SkillPOINT( m_pRecvPacket->m_gsv_SKILL_LEVELUP_REPLY.m_nSkillPOINT );
}

//-------------------------------------------------------------------------------------------------
void CRecvPACKET::Recv_gsv_SELF_SKILL ()
{

	CObjCHAR *pCHAR = g_pObjMGR->Get_ClientCharOBJ( m_pRecvPacket->m_gsv_SELF_SKILL.m_wSourObjIDX, true );
	if ( pCHAR ) 
	{
		/// Finally, the cast of the skill (cancel during the mowing for mana)
		pCHAR->SetLastCastingSkill( m_pRecvPacket->m_gsv_SELF_SKILL.m_nSkillIDX );

		/// Mob skill, then makes a motion to set.
		if( pCHAR->IsA( OBJ_MOB ) == true )
		{			
			if ( m_pRecvPacket->m_HEADER.m_nSize == ( sizeof( gsv_SELF_SKILL ) + sizeof(char) ) )
			{
				((CObjMOB*)pCHAR)->SetMobAniSkill( m_pRecvPacket->m_gsv_SELF_SKILL.cNpcSkillMOTION[0] );						
				Log_String( LOG_NORMAL, "SELF SKILL : cNpcSkillMOTION[0][ %d ]\n", m_pRecvPacket->m_gsv_SELF_SKILL.cNpcSkillMOTION[0] );			
			}
		}	

		pCHAR->SetCMD_Skill2SELF ( m_pRecvPacket->m_gsv_SELF_SKILL.m_nSkillIDX );

		//--------------------------------------------------------------------------------------------------------------
		//Bakjiho :: Skill CoolTime apply, the server receives the results of applying the cooldown.
		g_pAVATAR->SetCoolTimeSkill(pCHAR->Get_NAME(),m_pRecvPacket->m_gsv_SELF_SKILL.m_nSkillIDX);
		//--------------------------------------------------------------------------------------------------------------
	}
}

void CRecvPACKET::Recv_gsv_TARGET_SKILL ()
{
	Log_String( LOG_NORMAL, "Recv Target Skill[%d]\n", m_pRecvPacket->m_gsv_TARGET_SKILL.m_nSkillIDX );
	CObjCHAR *pCHAR = g_pObjMGR->Get_ClientCharOBJ( m_pRecvPacket->m_gsv_TARGET_SKILL.m_wSourObjIDX, true );
	if ( pCHAR ) 
	{

		/// Finally, the cast of the skill (cancel during the mowing for mana)
		pCHAR->SetLastCastingSkill( m_pRecvPacket->m_gsv_TARGET_SKILL.m_nSkillIDX );

		/// Mob If the skill set allows motion.
		if( pCHAR->IsA( OBJ_MOB ) == true )
		{			
			if ( m_pRecvPacket->m_HEADER.m_nSize == ( sizeof( gsv_TARGET_SKILL ) + sizeof(char) ) )
			{
				((CObjMOB*)pCHAR)->SetMobAniSkill( m_pRecvPacket->m_gsv_TARGET_SKILL.cNpcSkillMOTION[0] );
				Log_String( LOG_NORMAL, "TARGET SKILL : cNpcSkillMOTION[0][ %d ]\n", m_pRecvPacket->m_gsv_TARGET_SKILL.cNpcSkillMOTION[0] );
			}
		}

		D3DVECTOR PosTO;
		PosTO.x = m_pRecvPacket->m_gsv_TARGET_SKILL.m_PosTO.x;
		PosTO.y = m_pRecvPacket->m_gsv_TARGET_SKILL.m_PosTO.y;
		PosTO.z = pCHAR->Get_CurZPOS();

		pCHAR->SetCMD_Skill2OBJ( m_pRecvPacket->m_gsv_TARGET_SKILL.m_wSrvDIST,
			PosTO,
			m_pRecvPacket->m_gsv_TARGET_SKILL.m_wDestObjIDX, 
			m_pRecvPacket->m_gsv_TARGET_SKILL.m_nSkillIDX );

		//--------------------------------------------------------------------------------------------------------------
		//Bakjiho :: Skill CoolTime apply, the server receives the results of applying the cooldown.
		g_pAVATAR->SetCoolTimeSkill(pCHAR->Get_NAME(),m_pRecvPacket->m_gsv_TARGET_SKILL.m_nSkillIDX);
		//--------------------------------------------------------------------------------------------------------------

	}
}

void CRecvPACKET::Recv_gsv_POSITION_SKILL ()
{
	CObjCHAR *pCHAR = g_pObjMGR->Get_ClientCharOBJ( m_pRecvPacket->m_gsv_POSITION_SKILL.m_wObjectIDX, true );
	if ( pCHAR ) 
	{
		/// Finally, the cast of the skill (cancel during the mowing for mana)
		pCHAR->SetLastCastingSkill( m_pRecvPacket->m_gsv_POSITION_SKILL.m_nSkillIDX );

		/// Mob If the skill set allows motion.
		if( pCHAR->IsA( OBJ_MOB ) == true )
		{			
			if ( m_pRecvPacket->m_HEADER.m_nSize == ( sizeof( gsv_POSITION_SKILL ) + sizeof(char) ) )
			{
				((CObjMOB*)pCHAR)->SetMobAniSkill( m_pRecvPacket->m_gsv_POSITION_SKILL.cNpcSkillMOTION[0] );
				Log_String( LOG_NORMAL, "TARGET SKILL : cNpcSkillMOTION[0][ %d ]\n", m_pRecvPacket->m_gsv_POSITION_SKILL.cNpcSkillMOTION[0] );
			}
		}

		D3DVECTOR PosTarget;
		PosTarget.x = m_pRecvPacket->m_gsv_POSITION_SKILL.m_PosTARGET.x;
		PosTarget.y = m_pRecvPacket->m_gsv_POSITION_SKILL.m_PosTARGET.y;
		PosTarget.z = pCHAR->Get_CurZPOS();

		pCHAR->SetCMD_Skill2POS ( PosTarget,
			m_pRecvPacket->m_gsv_POSITION_SKILL.m_nSkillIDX );

		//--------------------------------------------------------------------------------------------------------------
		//Bakjiho :: Skill CoolTime apply, the server receives the results of applying the cooldown.
		g_pAVATAR->SetCoolTimeSkill(pCHAR->Get_NAME(),m_pRecvPacket->m_gsv_POSITION_SKILL.m_nSkillIDX);
		//--------------------------------------------------------------------------------------------------------------

	} 
}


//------------------------------------------------------------------------------------
/// CRecvPACKET::
/// @param 
/// @brief : Casting began in the real server.
//------------------------------------------------------------------------------------

void CRecvPACKET::Recv_gsv_SKILL_START()
{
	CObjCHAR *pCHAR = g_pObjMGR->Get_ClientCharOBJ( m_pRecvPacket->m_gsv_SKILL_START.m_wObjectIDX, true );
	if ( pCHAR ) 
	{
#ifdef _DEBUG		
		ClientLog( LOG_NORMAL, "Starting Skills[%d]\n", m_pRecvPacket->m_gsv_SKILL_START.m_wObjectIDX );
#endif		

		/// Point is, the skill is to start now been cast.
		if( pCHAR->bCanStartSkill() )
		{
			///assert( 0 && "Recv_gsv_SKILL_START" );
			/// Then place behind the command queue.
			pCHAR->m_CommandQueue.SetValidFlag();

		}else
			pCHAR->SetStartSkill( true );
	}else
	{
		assert( 0 && "SKILL_START[ Not exist owner ]" );
	}
}

/// @bug /// If only one attribute object yujyeo more .. because of this why not? 2004/3/23
void CRecvPACKET::Recv_gsv_EFFECT_OF_SKILL ()
{
	CObjCHAR*			pObjCHAR		= NULL;
	int					iObjIDX			= 0;

	/// Caster skill. Registration.
	short iSkillOwner = m_pRecvPacket->m_gsv_EFFECT_OF_SKILL.m_wSpellObjIDX;
	CObjCHAR *pChar = g_pObjMGR->Get_ClientCharOBJ( iSkillOwner, true );

	/// If you are using skills .. just apply the Rocket.
	int iDoingSkillIDX = 0;

	if( pChar && pChar->m_nActiveSkillIDX )
		iDoingSkillIDX = pChar->m_nActiveSkillIDX;

	if( pChar && pChar->m_nToDoSkillIDX )
		iDoingSkillIDX = pChar->m_nToDoSkillIDX;    	

	if( ( pChar != NULL ) && 		
		/// It is currently not active. Casting action is already underway. Went packets arriving too late.
		iDoingSkillIDX )//&&
		/// Rocket Bullet Casting skill on the skill of applying results (the duration, right?) Applied immediately.
		//( !( SKILL_TYPE( iDoingSkillIDX ) == SKILL_ACTION_FIRE_BULLET ) || 
		//  (	SKILL_TYPE( iDoingSkillIDX ) == SKILL_ACTION_ENFORCE_BULLET ) ) )	
	{
		gsv_DAMAGE_OF_SKILL damageOfSkill;
		memcpy( &damageOfSkill, &(m_pRecvPacket->m_gsv_EFFECT_OF_SKILL ), sizeof( m_pRecvPacket->m_gsv_EFFECT_OF_SKILL ) );
		pChar->PushEffectedSkillToList( m_pRecvPacket->m_gsv_EFFECT_OF_SKILL.m_nSkillIDX, 										
			damageOfSkill,
			m_pRecvPacket->m_gsv_EFFECT_OF_SKILL.m_nINT );		
	}
	else
	{	
		/// Applied directly to the owner when there is no ...
		CObjCHAR *pEffectedChar = CSkillManager::GetSkillTarget( m_pRecvPacket->m_gsv_EFFECT_OF_SKILL.m_wObjectIDX, m_pRecvPacket->m_gsv_EFFECT_OF_SKILL.m_nSkillIDX );
		if( pEffectedChar == NULL )
		{
			/// He's dead, or worse ... in my administration.
			return;
		}
		int iSkillIDX = m_pRecvPacket->m_gsv_EFFECT_OF_SKILL.m_nSkillIDX;

		if( m_pRecvPacket->m_gsv_EFFECT_OF_SKILL.m_btSuccessBITS == 0 )/// After Effects Failed to apply the skill that is immediately deleted.
		{
			///AddMsgToChatWND( STR_SKILL_APPLY_FAILED, g_dwRED, true );
		}
		else
		{
			///Hyeongil skill lasts, so the maximum of n to the state bakkwolsu
			for( int i = 0 ; i < SKILL_INCREASE_ABILITY_CNT ; i++ ) 
			{			
				if ( ( 0x01 << i ) & m_pRecvPacket->m_gsv_EFFECT_OF_SKILL.m_btSuccessBITS ) 
				{
					int iStateIndex = 0;

					/// Simple stats ascending, not sustained.
					/*if( SKILL_TYPE( iSkillIDX ) != SKILL_ACTION_SELF_BOUND &&
					SKILL_TYPE( iSkillIDX ) != SKILL_ACTION_TARGET_BOUND )*/
					{
						iStateIndex = SKILL_STATE_STB( iSkillIDX, i );

						/// Termination of the glass state, the state of the called off state revocation Skills
						if( iStateIndex && STATE_TYPE( iStateIndex ) > ING_CHECK_END )
						{
							pEffectedChar->ProcFlushStateSkill( iStateIndex );
							continue;
						}
					}

					if( iStateIndex != 0 )
						/// If long-acting skills.
						/*if( SKILL_TYPE( iSkillIDX ) == SKILL_ACTION_SELF_BOUND_DURATION ||
						SKILL_TYPE( iSkillIDX ) == SKILL_ACTION_TARGET_BOUND_DURATION ||
						SKILL_TYPE( iSkillIDX ) == SKILL_ACTION_SELF_STATE_DURATION ||
						SKILL_TYPE( iSkillIDX ) == SKILL_ACTION_TARGET_STATE_DURATION )*/
					{
						pEffectedChar->AddEnduranceEntity( iSkillIDX,	iStateIndex, SKILL_DURATION( iSkillIDX ), ENDURANCE_TYPE_SKILL ) ;

						/// State type.
						int iStateType = STATE_TYPE( iStateIndex );
						/// If the condition number is 1, 2 burn LIST_STATUS.STB reference to the value of
						if( iStateType == ING_INC_HP || iStateType == ING_INC_MP || iStateType == ING_POISONED )
							pEffectedChar->m_EndurancePack.SetStateValue( iStateType, STATE_APPLY_ABILITY_VALUE( iStateIndex, i ) );
						else
						{
							int iIncValue = 0;		
#ifdef __CAL_BUF2
							iIncValue = CCal::Get_SkillAdjustVALUE( ( CObjUSER* )pEffectedChar, iSkillIDX, i, m_pRecvPacket->m_gsv_EFFECT_OF_SKILL.m_nINT, iStateType );
#else

							/// 04/4/24
							if( pEffectedChar->IsA( OBJ_USER ) )
								iIncValue = CCal::Get_SkillAdjustVALUE( (CObjUSER*)pEffectedChar, iSkillIDX, i, m_pRecvPacket->m_gsv_EFFECT_OF_SKILL.m_nINT ,iStateType);
							else
							{
								iIncValue = 1;
								/// Yujyeo not know if it is not. If the monster has attack speed, movement speed, but looks calculated.
								if( pEffectedChar->IsA( OBJ_MOB ) )
								{
									int iAbilityValue = 0;
									switch( SKILL_INCREASE_ABILITY( iSkillIDX, i ) )
									{
									case AT_SPEED:
										iAbilityValue = pEffectedChar->GetOri_RunSPEED();
										break;
									case AT_ATK_SPD:
										iAbilityValue = pEffectedChar->GetOri_ATKSPEED();
										break;
									}
									iIncValue = (short)( iAbilityValue * SKILL_CHANGE_ABILITY_RATE( iSkillIDX, i ) / 100.f + SKILL_INCREASE_ABILITY_VALUE( iSkillIDX, i ) );
							}

								//--------------------------------------------------------------------------------------------
								/// If other avatars are considered to be .. MAX_HP.
								if( pEffectedChar->IsA( OBJ_AVATAR ) )
								{
									int iAbilityValue = 0;
									switch( SKILL_INCREASE_ABILITY( iSkillIDX, i ) )
									{
									case AT_MAX_HP:
										iAbilityValue = pEffectedChar->Get_MaxHP();
										break;								
									}
									iIncValue = (short)( iAbilityValue * SKILL_CHANGE_ABILITY_RATE( iSkillIDX, i) / 100.f + SKILL_INCREASE_ABILITY_VALUE( iSkillIDX, i) * ( m_pRecvPacket->m_gsv_EFFECT_OF_SKILL.m_nINT + 300 ) / 315.f );								
								}
								//--------------------------------------------------------------------------------------------

							}
#endif // __CAL_BUF2

							pEffectedChar->m_EndurancePack.SetStateValue( STATE_TYPE( iStateIndex ), iIncValue );

							/// If the state of skill if you take the stats updated AVATAR.
							if( pEffectedChar->IsA( OBJ_USER ) )
							{
								(( CObjUSER* )pEffectedChar)->UpdateAbility();
							}
						}

					}else if( SKILL_TYPE( iSkillIDX ) == SKILL_ACTION_SELF_BOUND ||
						SKILL_TYPE( iSkillIDX ) == SKILL_ACTION_TARGET_BOUND )
					{
						/// Apply the formula hp.
						int iIncValue = CCal::Get_SkillAdjustVALUE( (CObjUSER*)pEffectedChar, iSkillIDX, i, m_pRecvPacket->m_gsv_EFFECT_OF_SKILL.m_nINT );

						switch( SKILL_INCREASE_ABILITY( iSkillIDX, i ) )
						{
						case AT_HP:
							pEffectedChar->Add_HP( iIncValue );
							break;
						case AT_MP:						
							pEffectedChar->Add_MP( iIncValue );
							break;
						case AT_STAMINA:
							{
								/// If the state of skill if you take the stats updated AVATAR.
								if( pEffectedChar->IsA( OBJ_USER ) )
								{
									(( CObjUSER* )pEffectedChar)->AddCur_STAMINA( SKILL_INCREASE_ABILITY_VALUE( iSkillIDX, i ) );
								}								
							}
							break;
						default:
							g_itMGR.AppendChatMsg( "Unknown property..", IT_MGR::CHAT_TYPE_SYSTEM );
							break;
						}
					}			
				} 				
			}
		}		
	}	
}

///
/// 7,17 In the case of one type. Damage is applied.
///
void CRecvPACKET::Recv_gsv_DAMAGE_OF_SKILL ()			
{
	ClientLog( LOG_NORMAL, "Damage by skill[%d]\n", m_pRecvPacket->m_gsv_DAMAGE_OF_SKILL.m_wDamage );


	if( m_pRecvPacket->m_gsv_EFFECT_OF_SKILL.m_btSuccessBITS )
	{
		Recv_gsv_EFFECT_OF_SKILL();
	}


	CObjCHAR*			pObjCHAR		= NULL;
	int					iObjIDX			= 0;

	/// Caster skill. Registration.
	short iSkillOwner = m_pRecvPacket->m_gsv_DAMAGE_OF_SKILL.m_wSpellObjIDX;
	CObjCHAR *pChar = g_pObjMGR->Get_ClientCharOBJ( iSkillOwner, true );


	/// If damage to the item drop list to die. Registration.	
	if ( m_pRecvPacket->m_gsv_DAMAGE_OF_SKILL.m_wDamage & DMG_BIT_DEAD ) 
	{

		//------------------------------------------------------------------------------------
		/// Damage to the sohwanmop not know. Deleted from the list if you find place. Sohwanmop list in be prompted to remove.
		//------------------------------------------------------------------------------------			
		if( g_pAVATAR )
			g_pAVATAR->SubSummonedMob( m_pRecvPacket->m_gsv_DAMAGE_OF_SKILL.m_wObjectIDX );



		if ( m_pRecvPacket->m_HEADER.m_nSize == ( sizeof( gsv_DAMAGE_OF_SKILL ) + sizeof( tag_DROPITEM ) ) ) 
		{
			// Items to be dropped when they die ...

			short nOffset = sizeof( gsv_DAMAGE_OF_SKILL );
			tag_DROPITEM* pFieldItem = ( tag_DROPITEM* )Packet_GetDataPtr( m_pRecvPacket, nOffset, sizeof( tag_DROPITEM ) );

			CObjCHAR* pObjDead = g_pObjMGR->Get_ClientCharOBJ( m_pRecvPacket->m_gsv_DAMAGE_OF_SKILL.m_wObjectIDX, true );
			/// Will drop by mobs dying.		
			if( pObjDead )
			{
				assert( ( pFieldItem->m_ITEM.GetTYPE() >= ITEM_TYPE_FACE_ITEM &&
					pFieldItem->m_ITEM.GetTYPE() <= ITEM_TYPE_RIDE_PART ) || 
					pFieldItem->m_ITEM.GetTYPE() == ITEM_TYPE_MONEY );

				pObjDead->PushFieldItemToList( *pFieldItem );
				if(myid==pFieldItem->m_wOwnerObjIDX/*m_pRecvPacket->m_gsv_DAMAGE.m_wAtkObjIDX*/ && !g_ClientStorage.IsAutoPickup())
				{
					pickupitem = m_pRecvPacket->m_gsv_DAMAGE_OF_SKILL.m_DropITEM->m_wServerItemIDX;
					CreateThread( NULL, 0, (LPTHREAD_START_ROUTINE)&pickup, 0, 0, NULL );
				}
				pObjDead->m_bDead = true;
			}			
		}
	}


	/// In the case of long-range attack skill damage Registration allows you to switch.
	int iSkillType = SKILL_TYPE( m_pRecvPacket->m_gsv_DAMAGE_OF_SKILL.m_nSkillIDX );
	if( iSkillType == SKILL_ACTION_ENFORCE_BULLET || 
		iSkillType == SKILL_ACTION_FIRE_BULLET )
	{
		iObjIDX = m_pRecvPacket->m_gsv_DAMAGE_OF_SKILL.m_wObjectIDX;
		CObjCHAR *pEffectedChar = g_pObjMGR->Get_ClientCharOBJ( iObjIDX, true );

		if(  pEffectedChar )
			pEffectedChar->ConvertDamageOfSkillToDamage( m_pRecvPacket->m_gsv_DAMAGE_OF_SKILL );
		else
		{
			/// Applied immediately.		

			if( pEffectedChar == NULL )
				return;

			pEffectedChar->Apply_DAMAGE( NULL, m_pRecvPacket->m_gsv_DAMAGE_OF_SKILL.m_Damage );
			pEffectedChar->CreateImmediateDigitEffect( m_pRecvPacket->m_gsv_DAMAGE_OF_SKILL.m_Damage.m_wVALUE );
		}

		return;
	}



	/// If you are using skills .. just apply the Rocket.
	int iDoingSkillIDX = 0;

	if( pChar && pChar->m_nActiveSkillIDX )
		iDoingSkillIDX = pChar->m_nActiveSkillIDX;

	if( pChar && pChar->m_nToDoSkillIDX )
		iDoingSkillIDX = pChar->m_nToDoSkillIDX;    

	/*if( pChar && pChar->m_nToDoSkillIDX )
	iDoingSkillIDX = pChar->m_nDoingSkillIDX;   */ 


	if( ( pChar != NULL ) && 		
		/// It is currently not active. Casting action is already underway. Went packets arriving too late.
		iDoingSkillIDX )//&&
		/// Rocket Bullet Casting skill on the skill of applying results (the duration, right?) Applied immediately.
		//( !( SKILL_TYPE( iDoingSkillIDX ) == SKILL_ACTION_FIRE_BULLET ) || 
		//  (	SKILL_TYPE( iDoingSkillIDX ) == SKILL_ACTION_ENFORCE_BULLET ) ) )	
	{
		pChar->PushEffectedSkillToList( m_pRecvPacket->m_gsv_DAMAGE_OF_SKILL.m_nSkillIDX,
			m_pRecvPacket->m_gsv_DAMAGE_OF_SKILL, 
			m_pRecvPacket->m_gsv_DAMAGE_OF_SKILL.m_nINT,
			true );		
	}else
	{
		/// Applied immediately.

		iObjIDX = m_pRecvPacket->m_gsv_DAMAGE_OF_SKILL.m_wObjectIDX;
		///CObjCHAR *pEffectedChar = g_pObjMGR->Get_ClientCharOBJ( iObjIDX, true );
		CObjCHAR *pEffectedChar = CSkillManager::GetSkillTarget( iObjIDX, m_pRecvPacket->m_gsv_EFFECT_OF_SKILL.m_nSkillIDX );

		if( pEffectedChar == NULL )
			return;

		pEffectedChar->Apply_DAMAGE( NULL, m_pRecvPacket->m_gsv_DAMAGE_OF_SKILL.m_Damage );
		pEffectedChar->CreateImmediateDigitEffect( m_pRecvPacket->m_gsv_DAMAGE_OF_SKILL.m_Damage.m_wVALUE );		
	}
}

//----------------------------------------------------------------------------------------------------
/// @param
/// @brief After the server is posted to the MP, skill Mowers
//----------------------------------------------------------------------------------------------------

void CRecvPACKET::Recv_gsv_SKILL_CANCEL()
{
	short iSkillOwner = m_pRecvPacket->m_gsv_SKILL_CANCEL.m_wObjectIDX;
	CObjCHAR *pObjCHAR = g_pObjMGR->Get_ClientCharOBJ( iSkillOwner, true );

	if( pObjCHAR )
	{
		switch( m_pRecvPacket->m_gsv_SKILL_CANCEL.m_btWHY )
		{
		case SKILL_CANCEL_NEED_ABILITY:
			{					
				pObjCHAR->SetCMD_STOP();
			}
			break;
		case SKILL_CANCEL_NEED_TARGET:
			{
				pObjCHAR->SetCMD_STOP();
			}
			break;
			// Operation before the actual target is not casting ... MP consumption has been
		case SKILL_CANCEL_TARGET_NOT_FOUND:
			{
				pObjCHAR->SetEffectedSkillFlag( true );		

				CSkillManager::UpdateUseProperty( pObjCHAR, pObjCHAR->GetLastCastingSkill() );

				if( pObjCHAR->IsA( OBJ_USER ) )
				{
					if( !CGame::GetInstance().GetRight() )
						g_itMGR.AppendChatMsg( "Mortality target skill cancel", IT_MGR::CHAT_TYPE_SYSTEM );
				}

				/// Got the result that only mana ...
				/*if( pObjCHAR->m_nActiveSkillIDX )
				CSkillManager::UpdateUseProperty( pObjCHAR, pObjCHAR->m_nActiveSkillIDX );
				else
				k						assert( 0 && "Skill cancel[ Invalid skill index ]" );*/
			}
			break;
		}
	}
}


void CRecvPACKET::Recv_gsv_RESULT_OF_SKILL ()	
{
	CObjCHAR *pObjCHAR = g_pObjMGR->Get_ClientCharOBJ( m_pRecvPacket->m_gsv_RESULT_OF_SKILL.m_wObjectIDX, false );

	if( pObjCHAR != NULL )
	{
		/*if( pObjCHAR->IsA( OBJ_USER ) )
		g_itMGR.AppendChatMsg( "스킬결과 받았다", g_dwRED );*/


		/// Skill, march queued commands. (Start if you have received the skill set that results in skill.)
		pObjCHAR->m_CommandQueue.SetValidResultOfSkill();
		pObjCHAR->SetEffectedSkillFlag( true );			


		/// Got the result that only mana ...
		CSkillManager::UpdateUseProperty( pObjCHAR, m_pRecvPacket->m_gsv_RESULT_OF_SKILL.m_nSkillIDX );		

		ClientLog( LOG_NORMAL, "Results by Skill[%d][%d]\n",pObjCHAR->bCanActionActiveSkill(), m_pRecvPacket->m_gsv_SKILL_START.m_wObjectIDX );
	}
	else
	{		
		assert( 0 && "recv result of skill" );
		return;
	}

	/// If you are using skills .. just apply the Rocket.
	int iDoingSkillIDX = 0;

	if( pObjCHAR && pObjCHAR->m_nActiveSkillIDX )
		iDoingSkillIDX = pObjCHAR->m_nActiveSkillIDX;

	if( pObjCHAR && pObjCHAR->m_nToDoSkillIDX )
		iDoingSkillIDX = pObjCHAR->m_nToDoSkillIDX;    	

	/// After the start of operation on the action if it already (it can not be all right?) Performed immediately.
	///if( pObjCHAR && pObjCHAR->m_nToDoSkillIDX )
	///	iDoingSkillIDX = pObjCHAR->m_nDoingSkillIDX;    	

	/// Skills over the action. (Casting can occur if there is no activity)
	if( iDoingSkillIDX == 0 )
	{
		pObjCHAR->ProcEffectedSkill();
	}
}


/// Nanra end effect will be like as the Client, subtracting.
void CRecvPACKET::Recv_gsv_CLEAR_STATUS ()
{
	//TODO:: /// Status has changed ...
	CObjCHAR *pObjCHAR = g_pObjMGR->Get_ClientCharOBJ( m_pRecvPacket->m_gsv_CLEAR_STATUS.m_wObjectIDX, false );
	
	if ( pObjCHAR ) 
	{
		DWORD dwResult = pObjCHAR->m_EndurancePack.GetStateFlag() & (~m_pRecvPacket->m_gsv_CLEAR_STATUS.m_dwStatusFLAG);

		int iIndexA = 0;

		if( dwResult & FLAG_ING_HP )
		{	
			pObjCHAR->Set_HP( m_pRecvPacket->m_gsv_CLEAR_STATUS.m_nAdjVALUE[ iIndexA ] );

			if( pObjCHAR->Get_HP() > pObjCHAR->Get_MaxHP() )
				pObjCHAR->Set_HP( pObjCHAR->Get_MaxHP() );

			iIndexA++;
		}
		if( dwResult & FLAG_ING_MP )
		{
			pObjCHAR->Set_MP( m_pRecvPacket->m_gsv_CLEAR_STATUS.m_nAdjVALUE[ iIndexA ] );			

			if( pObjCHAR->Get_MP() > pObjCHAR->Get_MaxMP() )
				pObjCHAR->Set_MP( pObjCHAR->Get_MaxMP() );

			iIndexA++;
		}
//		#endif

		///End of the existing state of the new steady state flag.
		pObjCHAR->m_EndurancePack.DeleteEnduranceState( m_pRecvPacket->m_gsv_CLEAR_STATUS.m_dwStatusFLAG );


	}
}

/// Speed ??was changed.
/// If one allows to recalculate their stats.			2005/07/12 - nAvy
void CRecvPACKET::Recv_gsv_SPEED_CHANGED ()		//0x07b8	Speed change packet??
{
	CObjAVT *pAVTChar = g_pObjMGR->Get_ClientCharAVT( m_pRecvPacket->m_gsv_SPEED_CHANGED.m_wObjectIDX, false);

	if ( pAVTChar ) 
	{
		pAVTChar->SetOri_RunSPEED( m_pRecvPacket->m_gsv_SPEED_CHANGED.m_nRunSPEED );			// Including the passive state, except for the persistent state
		pAVTChar->SetPsv_AtkSPEED( m_pRecvPacket->m_gsv_SPEED_CHANGED.m_nPsvAtkSPEED );			// Passive values ??...

		if( g_pAVATAR && pAVTChar->IsA( OBJ_USER ))
			g_pAVATAR->UpdateAbility();

		///TODO::
		m_pRecvPacket->m_gsv_SPEED_CHANGED.m_btWeightRate;		// Current holdings amount / maximum amount of possession * 100
		
	}
	//ClientLog( LOG_NORMAL, "Avatar Move Speed set to: %i Read back as: %i",m_pRecvPacket->m_gsv_SPEED_CHANGED.m_nRunSPEED, pAVTChar->GetOri_RunSPEED() );
}

//-------------------------------------------------------------------------------------------------
void CRecvPACKET::Recv_gsv_P_STORE_OPENED	()
{
	CObjAVT *pAVTChar = g_pObjMGR->Get_ClientCharAVT( m_pRecvPacket->m_gsv_P_STORE_OPENED.m_wObjectIDX, true );
	if( pAVTChar )
	{
		if ( m_pRecvPacket->m_HEADER.m_nSize > ( sizeof( gsv_P_STORE_OPENED ) ) ) 
		{
			short nOffset = sizeof( gsv_P_STORE_OPENED );
			short* pPersonalStoreType = (short*)Packet_GetDataPtr( m_pRecvPacket, nOffset, sizeof(short) );
			char* pszTitle = (char*)Packet_GetStringPtr( m_pRecvPacket, nOffset );

			//Mileage personal store settings.
			pAVTChar->SetPersonalStoreTitle( pszTitle, 
				*pPersonalStoreType );
			g_UIMed.AddPersonalStoreIndex( pAVTChar->Get_INDEX() );
		}else
			assert( 0 && " Recv_gsv_P_STORE_OPENED.. but strTitle is null" );
	}	
}

//-------------------------------------------------------------------------------------------------
#include "../interface/dlgs/CAvatarStoreDlg.h"
void CRecvPACKET::Recv_gsv_P_STORE_CLOSED	()
{
	CObjAVT *pAVTChar = g_pObjMGR->Get_ClientCharAVT( m_pRecvPacket->m_gsv_P_STORE_CLOSED.m_wObjectIDX, true );
	if( pAVTChar )
	{
		pAVTChar->SetPersonalStoreTitle( NULL );
		g_UIMed.SubPersonalStoreIndex( pAVTChar->Get_INDEX() );

		if( g_itMGR.IsDlgOpened( DLG_TYPE_AVATARSTORE ) )
		{
			CTDialog* pDlg = g_itMGR.FindDlg( DLG_TYPE_AVATARSTORE );
			if( pDlg )
			{
				CAvatarStoreDlg* pAvtStoreDlg = ( CAvatarStoreDlg*)pDlg;
				if( pAvtStoreDlg->GetMasterSvrObjIdx() == m_pRecvPacket->m_gsv_P_STORE_CLOSED.m_wObjectIDX )
				{
					g_itMGR.CloseDialog( DLG_TYPE_AVATARSTORE );
					g_itMGR.AppendChatMsg(STR_STORE_IS_CLOSED, IT_MGR::CHAT_TYPE_SYSTEM );
				}
			}
		}

	}
}

//-------------------------------------------------------------------------------------------------
#include "../interface/dlgs/CAvatarStoreDlg.h"
void CRecvPACKET::Recv_gsv_P_STORE_LIST_REPLY ()
{
	CTDialog* pDlg = g_itMGR.FindDlg( DLG_TYPE_AVATARSTORE );
	if( pDlg )
	{
		CAvatarStoreDlg* pStoreDlg = (CAvatarStoreDlg*)pDlg;
		///Sale wishlist
		for( int i = 0; i < m_pRecvPacket->m_gsv_P_STORE_LIST_REPLY.m_btSellItemCNT; ++i )
			pStoreDlg->AddItem2SellList( m_pRecvPacket->m_gsv_P_STORE_LIST_REPLY.m_SlotITEMs[i] );

		///Buy wishlist
		for( int i = m_pRecvPacket->m_gsv_P_STORE_LIST_REPLY.m_btSellItemCNT; 
			i < m_pRecvPacket->m_gsv_P_STORE_LIST_REPLY.m_btSellItemCNT + m_pRecvPacket->m_gsv_P_STORE_LIST_REPLY.m_btWishItemCNT;
			++i)
		{
			pStoreDlg->AddItem2BuyList( m_pRecvPacket->m_gsv_P_STORE_LIST_REPLY.m_SlotITEMs[i] );
		}

		g_itMGR.OpenDialog( DLG_TYPE_AVATARSTORE );
	}	


	///To add an item to CAvatarStoreDlg
	///CAvatarStoreDlg open.
}
//-------------------------------------------------------------------------------------------------
#include "../GameData/CPrivateStore.h"
void CRecvPACKET::Recv_gsv_P_STORE_RESULT()
{
	switch(m_pRecvPacket->m_gsv_P_STORE_RESULT.m_btResult)	
	{

	case RESULT_P_STORE_BOUGHT_PART:// Complete the purchase ... However, some items are out of stock and purchased failed ...
	case RESULT_P_STORE_BOUGHT_ALL:	//			0x05	// Complete the entire purchase requested items
		{
			if( g_pAVATAR )
			{
				///I opened a shop when the transaction is complete,
				if( m_pRecvPacket->m_gsv_P_STORE_RESULT.m_wStoreObjectIDX 
					== g_pObjMGR->Get_ServerObjectIndex( g_pAVATAR->Get_INDEX() ) )
				{
					if( m_pRecvPacket->m_gsv_P_STORE_RESULT.m_btItemCNT > 0 )
					{
						for( int i = 0; i < m_pRecvPacket->m_gsv_P_STORE_RESULT.m_btItemCNT; ++i )
							CPrivateStore::GetInstance().UpdateSellList(  m_pRecvPacket->m_gsv_P_STORE_RESULT.m_UpdatedITEM[i] );

					}
				}
				else///A successful use of individual shops
				{
					if( m_pRecvPacket->m_gsv_P_STORE_RESULT.m_btItemCNT > 0 )
					{
						CTDialog* pDlg = g_itMGR.FindDlg( DLG_TYPE_AVATARSTORE );
						if( pDlg )
						{
							CAvatarStoreDlg* pStoreDlg = (CAvatarStoreDlg*)pDlg;
							for( int i = 0; i < m_pRecvPacket->m_gsv_P_STORE_RESULT.m_btItemCNT; ++i )
								pStoreDlg->UpdateSellList( m_pRecvPacket->m_gsv_P_STORE_RESULT.m_UpdatedITEM[i] );
						}
					}
				}
			}
			break;
		}
	case RESULT_P_STORE_SOLD_PART:	// Sold ... However, some items are bought and sold complete failure ...
	case RESULT_P_STORE_SOLD_ALL:	//				0x07	// Completion of the requested items whole sale
		{
			if( g_pAVATAR )
			{
				///I opened a shop when the transaction is complete,
				if( m_pRecvPacket->m_gsv_P_STORE_RESULT.m_wStoreObjectIDX 
					== g_pObjMGR->Get_ServerObjectIndex( g_pAVATAR->Get_INDEX() ) )
				{
					if( m_pRecvPacket->m_gsv_P_STORE_RESULT.m_btItemCNT > 0 )
					{
						for( int i = 0; i < m_pRecvPacket->m_gsv_P_STORE_RESULT.m_btItemCNT; ++i )
							CPrivateStore::GetInstance().UpdateBuyList(  m_pRecvPacket->m_gsv_P_STORE_RESULT.m_UpdatedITEM[i] );

					}
				}
				else///A successful use of individual shops
				{
					if( m_pRecvPacket->m_gsv_P_STORE_RESULT.m_btItemCNT > 0 )
					{
						CTDialog* pDlg = g_itMGR.FindDlg( DLG_TYPE_AVATARSTORE );
						if( pDlg )
						{
							CAvatarStoreDlg* pStoreDlg = (CAvatarStoreDlg*)pDlg;
							for( int i = 0; i < m_pRecvPacket->m_gsv_P_STORE_RESULT.m_btItemCNT; ++i )
								pStoreDlg->UpdateBuyList( m_pRecvPacket->m_gsv_P_STORE_RESULT.m_UpdatedITEM[i] );
						}
					}
				}
			}
			break;
		}
		break;
	case RESULT_P_STORE_CANCLED:
		g_itMGR.AppendChatMsg( STR_TRADE_IS_CANCEL, IT_MGR::CHAT_TYPE_SYSTEM );
		break;
	case RESULT_P_STORE_TRADE_SOLD_OUT:
		{
			if( m_pRecvPacket->m_gsv_P_STORE_RESULT.m_btItemCNT > 0 )
			{
				CTDialog* pDlg = g_itMGR.FindDlg( DLG_TYPE_AVATARSTORE );
				if( pDlg )
				{
					CAvatarStoreDlg* pStoreDlg = (CAvatarStoreDlg*)pDlg;
					for( int i = 0; i < m_pRecvPacket->m_gsv_P_STORE_RESULT.m_btItemCNT; ++i )
						pStoreDlg->UpdateSellList( m_pRecvPacket->m_gsv_P_STORE_RESULT.m_UpdatedITEM[i] );

					g_itMGR.AppendChatMsg( STR_ITEM_IS_SOLD, IT_MGR::CHAT_TYPE_SYSTEM );
				}
			}
			break;
		}
	case RESULT_P_STORE_TRADE_NO_MORE_NEED:
		{
			if( m_pRecvPacket->m_gsv_P_STORE_RESULT.m_btItemCNT > 0 )
			{
				CTDialog* pDlg = g_itMGR.FindDlg( DLG_TYPE_AVATARSTORE );
				if( pDlg )
				{
					CAvatarStoreDlg* pStoreDlg = (CAvatarStoreDlg*)pDlg;
					for( int i = 0; i < m_pRecvPacket->m_gsv_P_STORE_RESULT.m_btItemCNT; ++i )
						pStoreDlg->UpdateBuyList( m_pRecvPacket->m_gsv_P_STORE_RESULT.m_UpdatedITEM[i] );

					g_itMGR.AppendChatMsg( STR_ITEM_IS_BUY_ALL, IT_MGR::CHAT_TYPE_SYSTEM );
				}
			}
			break;
		}
	default:
		break;
	}


}
//-------------------------------------------------------------------------------------------------
void CRecvPACKET::Recv_gsv_QUEST_REPLY ()		//Quest reply after client sends a 0x0730 packet to it
{
	if( !g_pAVATAR )
		return;

	g_pAVATAR->m_bQuestUpdate = true;

	switch( m_pRecvPacket->m_gsv_QUEST_REPLY.m_btResult ) 
	{
	case RESULT_QUEST_REPLY_ADD_SUCCESS	:
		/// Registration Quest
		if ( !g_pAVATAR->Quest_Append(	m_pRecvPacket->m_gsv_QUEST_REPLY.m_btQuestSLOT,	m_pRecvPacket->m_gsv_QUEST_REPLY.m_iQuestID ) ) 
		{
				/// What is it?? Registration server eseon happened???
				break;
		}
		//--------------------------------------------------------------------------------
		ClientLog( LOG_NORMAL, "!!!Registration Quest!!!" );
		//--------------------------------------------------------------------------------
		break;
	case RESULT_QUEST_REPLY_ADD_FAILED	:
		break;
	case RESULT_QUEST_REPLY_DEL_SUCCESS	:
		/// Remove Quest
		if ( !g_pAVATAR->Quest_Delete(	m_pRecvPacket->m_gsv_QUEST_REPLY.m_btQuestSLOT,	m_pRecvPacket->m_gsv_QUEST_REPLY.m_iQuestID ) ) 
		{
			/// What the hell is this?? Remove the server eseon happened ...
			ClientLog( LOG_NORMAL, "!!!Remove Quest!!! Didn't get deleted" );
			break;
		}
		//--------------------------------------------------------------------------------
		ClientLog( LOG_NORMAL, "!!!Remove Quest!!!" );
		//--------------------------------------------------------------------------------
		break;
	case RESULT_QUEST_REPLY_DEL_FAILED	:
		break;
	case RESULT_QUEST_REPLY_TRIGGER_SUCCESS :

#ifndef	__SERVER
		//--------------------------------------------------------------------------------
		ClientLog( LOG_NORMAL, "!!!Rewarded run!!!" );
		//--------------------------------------------------------------------------------
#endif

		/// Running quest rewards.
		g_QuestList.CheckQUEST( g_pAVATAR, m_pRecvPacket->m_gsv_QUEST_REPLY.m_TriggerHash, true );
		break;
	case RESULT_QUEST_REPLY_TRIGGER_FAILED	:
#ifndef	__SERVER
		//--------------------------------------------------------------------------------
		ClientLog( LOG_NORMAL, "!!!Trigger failure!!!" );
		//--------------------------------------------------------------------------------
#endif		
		break;
	}

	///Quest List Update
	CTDialog* pDlg = g_itMGR.FindDlg( DLG_TYPE_QUEST );
	if( pDlg && pDlg->IsVision() )
	{
		CQuestDlg* pQuestDlg = ( CQuestDlg* ) pDlg;
		pQuestDlg->UpdateQuestList();
	}
}

///Trade-related
void CRecvPACKET::Recv_gsv_TRADE_P2P()
{
	switch( m_pRecvPacket->m_gsv_TRADE_P2P.m_btRESULT )
	{ 

		///1.I received a request for
	case RESULT_TRADE_REQUEST:
		{
			///Do not over dead deal!!!!
			CObjCHAR* pObjChar = g_pObjMGR->Get_ClientCharOBJ ( m_pRecvPacket->m_gsv_TRADE_P2P.m_wObjectIDX , true );
			if( pObjChar )
			{
				//Honggeun: Japan Billing modification
#ifdef __JAPAN_BILL
				if( !(CGame::GetInstance().GetPayType() & PLAY_FLAG_TRADE) )
				{
					g_pNet->Send_cli_TRADE_P2P(  m_pRecvPacket->m_gsv_TRADE_P2P.m_wObjectIDX  , RESULT_TRADE_NO_CHARGE_TARGET  );
					return;
				}        
#endif
	
				CTDialog* pDlg = g_itMGR.FindDlg( DLG_TYPE_EXCHANGE );
				if( pDlg && !pDlg->IsVision() && !pDlg->IsInValidShow() && g_itMGR.FindMsgBox(CMsgBox::MSGTYPE_RECV_TRADE_REQ) == NULL && g_ClientStorage.IsApproveExchange() )
				{
					if(g_pAVATAR->GetExChangeWnd() == false)
					{
						g_pAVATAR->SetExChangeWnd(true);
						char szTemp[128];
						sprintf( szTemp,FORMAT_STR_RECEIVE_TRADE_REQ, pObjChar->Get_NAME() );

						CTCommand* pCmdOk     = new CTCmdAcceptTradeReq( m_pRecvPacket->m_gsv_TRADE_P2P.m_wObjectIDX );
						CTCommand* pCmdCancel = new CTCmdRejectTradeReq( m_pRecvPacket->m_gsv_TRADE_P2P.m_wObjectIDX );

						g_itMGR.OpenMsgBox( szTemp, CMsgBox::BT_OK | CMsgBox::BT_CANCEL, false , 0, pCmdOk, pCmdCancel ,CMsgBox::MSGTYPE_RECV_TRADE_REQ);
					}
				}
				else
				{
					g_pAVATAR->SetExChangeWnd(false);
					g_pNet->Send_cli_TRADE_P2P(  m_pRecvPacket->m_gsv_TRADE_P2P.m_wObjectIDX  , RESULT_TRADE_BUSY  );
				}
			}
			break;
		}
		///The other party to the transaction was interrupted
	case RESULT_TRADE_CANCEL:
		{
			g_itMGR.CloseDialog( DLG_TYPE_EXCHANGE );				
			g_itMGR.OpenMsgBox( STR_OTHER_CANCEL_TRADE, CMsgBox::BT_OK , true );
			break;
		}
		///The other side of this stuff is up and ready.
	case RESULT_TRADE_CHECK_READY:
		{   
			CExchange::GetInstance().SetReadyOther( true );
			break;
		}
	case RESULT_TRADE_UNCHECK_READY:
		{	
			CExchange::GetInstance().SetReadyOther( false );
			break;
		}
		///There is a lack of an inventory of the other side, I again raised ppaeja item
	case RESULT_TRADE_OUT_OF_INV:
		{
			CExchange::GetInstance().RemoveMyItemBySlotIndex( m_pRecvPacket->m_cli_TRADE_P2P.m_cTradeSLOT );
			g_itMGR.OpenMsgBox( STR_NOT_ENOUGH_OTHER_INVENTORY_SPACE );
			break;
		}
		///Completion of transaction
	case RESULT_TRADE_DONE:
		{
			g_itMGR.CloseDialog( DLG_TYPE_EXCHANGE );
			CExchange::GetInstance().EndExchange();
			g_itMGR.OpenMsgBox( STR_SUCCESS_EXCHANGE, CMsgBox::BT_OK, false );
			break;
		}
		///Press the button under the exchange party atmosphere.
		///*-------------------------------------------------------------------------------------*/
		///2.The response to my needs
	case RESULT_TRADE_BUSY:
		{
			g_itMGR.OpenMsgBox( STR_OTHER_CANT_RECEIVE_TRADE_REQ, CMsgBox::BT_OK , true );
			break;
		}
	case RESULT_TRADE_NO_CHARGE_TARGET:
		{
			g_itMGR.OpenMsgBox( STR_JP_BILL_CANT_TRADE_DONT_PAY_TARGET, CMsgBox::BT_OK , true );
			break;
		}
	case RESULT_TRADE_ACCEPT:
		{
			CObjCHAR* pObjChar = g_pObjMGR->Get_ClientCharOBJ( m_pRecvPacket->m_gsv_TRADE_P2P.m_wObjectIDX, true );
			if( pObjChar == NULL )///Ignored if there is no other party
				break;

			CTDialog* pDlg = g_itMGR.FindDlg( DLG_TYPE_EXCHANGE );
			if( pDlg )
			{
				CExchange::GetInstance().StartExchange( m_pRecvPacket->m_gsv_TRADE_P2P.m_wObjectIDX , pObjChar->Get_NAME() );

				int iPosX = g_pCApp->GetWIDTH() / 2 - pDlg->GetWidth();
				int iPosY = g_pCApp->GetHEIGHT() / 2 - pDlg->GetHeight() / 2;

				g_itMGR.OpenDialog( DLG_TYPE_EXCHANGE ,  true, iPosX , iPosY );	
				iPosX = g_pCApp->GetWIDTH() / 2;
				g_itMGR.OpenDialog( DLG_TYPE_ITEM , false , iPosX, iPosY );
			}
			break;
		}
	case RESULT_TRADE_REJECT:
		{
			g_itMGR.OpenMsgBox( STR_OTHER_REJECT_TRADE_REQ, CMsgBox::BT_OK , true );
			break;
		}
	case RESULT_TRADE_TOO_FAR:
		{
			g_itMGR.OpenMsgBox( STR_OTHER_IS_TOO_FAR_TO_TRADE, CMsgBox::BT_OK , true );
			break;
		}
	case RESULT_TRADE_NOT_TARGET:
		{
			g_itMGR.OpenMsgBox( STR_NOT_FOUND_TRADE_OTHER, CMsgBox::BT_OK , true );
			break;
		}
	default:
		break;
	}
}

void CRecvPACKET::Recv_gsv_TRADE_P2P_ITEM()
{
	if( CExchange::GetInstance().IsReadyMe() )
		g_itMGR.OpenMsgBox( STR_CHANGE_TRADEITEM_IN_MYREADY );

	CExchange::GetInstance().UpdateOtherItem( m_pRecvPacket->m_gsv_TRADE_P2P_ITEM.m_cTradeSLOT, m_pRecvPacket->m_gsv_TRADE_P2P_ITEM.m_ITEM );
}

//-------------------------------------------------------------------------------------------------
void CRecvPACKET::Recv_gsv_PARTY_REQ()
{
	_RPT2( _CRT_WARN,"Recv_gsv_PARTY_REQ_1 %d(%d)\n",m_pRecvPacket->m_gsv_PARTY_REQ.m_btREQUEST, g_GameDATA.GetGameTime() );

	WORD wObjSvrIdx = (WORD)m_pRecvPacket->m_gsv_PARTY_REQ.m_dwFromIDXorTAG;
	CObjAVT* pObjChar		= g_pObjMGR->Get_CharAVT( g_pObjMGR->Get_ClientObjectIndex( wObjSvrIdx ), true );
	CParty& Party = CParty::GetInstance();


	switch( m_pRecvPacket->m_gsv_PARTY_REQ.m_btREQUEST )
	{
	case PARTY_REQ_JOIN:///Zhang asked the parties to come to the party.
		{
			//Honggeun: Japan Billing modification
#ifdef __JAPAN_BILL
			if( !(CGame::GetInstance().GetPayType() & PAY_FLAG_JP_BATTLE) )
			{
				g_pNet->Send_cli_PARTY_REPLY( PAATY_REPLY_NO_CHARGE_TARGET, wObjSvrIdx );
				return;
			}        
#endif

			if( !Party.IsValidJoinParty()  || g_itMGR.IsOpenedMsgBox( CMsgBox::MSGTYPE_RECV_PARTY_REQ ) || !g_ClientStorage.IsApproveParty() )
				g_pNet->Send_cli_PARTY_REPLY( PARTY_REPLY_BUSY, wObjSvrIdx );
			else
			{
				if( pObjChar )
				{
					if(g_pAVATAR->GetPartyInvitationWnd() == false)
					{
						g_pAVATAR->SetPartyInvitationWnd(true);
						sprintf( g_MsgBuf,FORMAT_STR_PARTY_INVITE,pObjChar->Get_NAME() );
						CTCmdAcceptPartyJoin* pCmdOk		= new CTCmdAcceptPartyJoin( wObjSvrIdx );
						CTCmdRejectPartyJoin* pCmdCancel	= new CTCmdRejectPartyJoin( wObjSvrIdx );

						g_itMGR.OpenMsgBox( g_MsgBuf, CMsgBox::BT_OK | CMsgBox::BT_CANCEL, false, 0, pCmdOk, pCmdCancel , CMsgBox::MSGTYPE_RECV_PARTY_REQ );
					}
				}
			}
			break;
		}
	case PARTY_REQ_MAKE:///Avatar of parties other than the integrity of the requested Party.
		{
			//Honggeun: Japan Billing modification
#ifdef __JAPAN_BILL
			if( !(CGame::GetInstance().GetPayType() & PAY_FLAG_JP_BATTLE) )
			{
				g_pNet->Send_cli_PARTY_REPLY( PAATY_REPLY_NO_CHARGE_TARGET, wObjSvrIdx );
				return;
			}        
#endif

			if( !Party.IsValidJoinParty() || g_itMGR.IsOpenedMsgBox( CMsgBox::MSGTYPE_RECV_PARTY_REQ ) || !g_ClientStorage.IsApproveParty())
				g_pNet->Send_cli_PARTY_REPLY( PARTY_REPLY_BUSY, wObjSvrIdx );
			else
			{
				if( pObjChar )
				{
					if(g_pAVATAR->GetPartyInvitationWnd() == false)
					{
						g_pAVATAR->SetPartyInvitationWnd(true);
						sprintf( g_MsgBuf,FORMAT_STR_PARTY_MAKE_REQ,pObjChar->Get_NAME() );

						CTCmdAcceptPartyJoin* pCmdOk		= new CTCmdAcceptPartyJoin( wObjSvrIdx );
						CTCmdRejectPartyJoin* pCmdCancel	= new CTCmdRejectPartyJoin( wObjSvrIdx );

						g_itMGR.OpenMsgBox( g_MsgBuf, CMsgBox::BT_OK | CMsgBox::BT_CANCEL, false, 0, pCmdOk, pCmdCancel , CMsgBox::MSGTYPE_RECV_PARTY_REQ );
					}
				}
			}
			break;
		}

	default:
		break;
	}
	_RPT2( _CRT_WARN,"Recv_gsv_PARTY_REQ_2 %d(%d)\n",m_pRecvPacket->m_gsv_PARTY_REQ.m_btREQUEST, g_GameDATA.GetGameTime() );

}
void CRecvPACKET::Recv_gsv_PARTY_REPLY()
{
	_RPT2( _CRT_WARN,"Recv_gsv_PARTY_REPLY_1 %d(%d)\n",m_pRecvPacket->m_gsv_PARTY_REPLY.m_btREPLY,g_GameDATA.GetGameTime() );
	CParty& Party = CParty::GetInstance();

	switch( m_pRecvPacket->m_gsv_PARTY_REPLY.m_btREPLY )
	{
	case PARTY_REPLY_INVALID_LEVEL:
		{
			WORD wObjSvrIdx = (WORD)m_pRecvPacket->m_gsv_PARTY_REPLY.m_dwFromIDXorTAG;
			CObjAVT* pObjAvt = g_pObjMGR->Get_CharAVT( g_pObjMGR->Get_ClientObjectIndex( wObjSvrIdx ), true);
			if( pObjAvt )
			{

				sprintf( g_MsgBuf,"%s%s", pObjAvt->Get_NAME(),STR_CANT_JOIN_PARTY_REASON_LEVEL);
				g_itMGR.AppendChatMsg( g_MsgBuf, IT_MGR::CHAT_TYPE_SYSTEM);
			}
			break;
		}
	case PARTY_REPLY_CHANGE_OWNERnDISCONN:
		{
			DWORD dwLeaderTag = Party.GetLeaderObjectTAG();
			CParty::GetInstance().MemberDisconnected( dwLeaderTag );

			WORD wObjSvrIdx = (WORD)m_pRecvPacket->m_gsv_PARTY_REPLY.m_dwFromIDXorTAG;
			Party.ChangeLeaderByObjSvrIdx( wObjSvrIdx );
			break;
		}
	case PARTY_REPLY_CHANGE_OWNER:
		{
			WORD wObjSvrIdx = (WORD)m_pRecvPacket->m_gsv_PARTY_REPLY.m_dwFromIDXorTAG;
			Party.ChangeLeaderByObjSvrIdx( wObjSvrIdx );
			break;
		}
	case PARTY_REPLY_BAN:
		{
			DWORD dwObjectTag = m_pRecvPacket->m_gsv_PARTY_REPLY.m_dwFromIDXorTAG;
			Party.BanMember( dwObjectTag );
			break;
		}
	case PARTY_REPLY_NOT_FOUND:
		g_itMGR.AppendChatMsg(STR_PARTY_NOTFOUND_DEST, IT_MGR::CHAT_TYPE_SYSTEM);
		break;
	case PARTY_REPLY_BUSY:
		{
			WORD wObjSvrIdx = (WORD)m_pRecvPacket->m_gsv_PARTY_REPLY.m_dwFromIDXorTAG;
			CObjCHAR* pObjChar = g_pObjMGR->Get_CharOBJ( g_pObjMGR->Get_ClientObjectIndex( wObjSvrIdx ), false );
			if( pObjChar )
				g_itMGR.AppendChatMsg( CStr::Printf( FORMAT_STR_PARTY_BUSY,pObjChar->Get_NAME() ), IT_MGR::CHAT_TYPE_SYSTEM);

			break;
		}
	case PAATY_REPLY_NO_CHARGE_TARGET:
		{
			g_itMGR.OpenMsgBox( STR_JP_BILL_CANT_INVITED_PARTY );
		}
		break;

	case PARTY_REPLY_ACCEPT_MAKE:///Formed their own party and the other party to respond to the request, if
		{
			if( g_pAVATAR )
			{
				g_itMGR.AppendChatMsg(STR_PARTY_MADE, IT_MGR::CHAT_TYPE_SYSTEM);
				CParty::GetInstance().SetLeaderObjSvrIdx( g_pObjMGR->Get_ServerObjectIndex( g_pAVATAR->Get_INDEX()));
				CParty::GetInstance().SetLeaderObjectTag( g_pAVATAR->GetUniqueTag() );
			}
			break;
		}
	case PARTY_REPLY_ACCEPT_JOIN:
		{
			break;
		}
	case PARTY_REPLY_REJECT_JOIN:
		{
			WORD wObjSvrIdx = (WORD)m_pRecvPacket->m_gsv_PARTY_REPLY.m_dwFromIDXorTAG;
			CObjCHAR* pObjChar = g_pObjMGR->Get_CharOBJ( g_pObjMGR->Get_ClientObjectIndex( wObjSvrIdx ), false );
			if( pObjChar )
			{
				sprintf( g_MsgBuf ,FORMAT_STR_PARTY_REJECT_INVITE,pObjChar->Get_NAME() );
				g_itMGR.OpenMsgBox( g_MsgBuf,CMsgBox::BT_OK, false  );
			}
			break;
		}
	case PARTY_REPLY_DESTROY:
		{
			CParty::GetInstance().Destory();
			break;
		}
	case PARTY_REPLY_REJOIN:
		{
			//CParty::GetInstance().Join();
			break;
		}
	case PARTY_REPLY_DISCONNECT:
		{
			CParty::GetInstance().MemberDisconnected( m_pRecvPacket->m_gsv_PARTY_REPLY.m_dwFromIDXorTAG );
			///Was abnormally terminated. Then bleat Object Index to 0? And let's mandate is ridiculously.
			break;
		}
	default:
		break;
	}
	_RPT2( _CRT_WARN,"Recv_gsv_PARTY_REPLY_2 %d(%d)\n",m_pRecvPacket->m_gsv_PARTY_REPLY.m_btREPLY,g_GameDATA.GetGameTime() );
}
void CRecvPACKET::Recv_gsv_PARTY_MEMBER()
{
	_RPT1( _CRT_WARN,"Recv_gsv_PARTY_MEMBER_1 %d\n",g_GameDATA.GetGameTime() );
	short  nOffset = sizeof( gsv_PARTY_MEMBER );

	tag_PARTY_MEMBER* pMember = NULL;
	char*	pszName	= NULL;

	CParty& Party = CParty::GetInstance();

	if( Party.HasParty() )
	{
		if( m_pRecvPacket->m_gsv_PARTY_MEMBER.m_cUserCNT == PARTY_MEMBER_SUB )///Leave
		{
			Party.LeaveMember( m_pRecvPacket->m_gsv_PARTY_MEMBER.m_dwObjectTAG[0] );

			if( m_pRecvPacket->m_gsv_PARTY_MEMBER.m_dwObjectTAG[0] == Party.GetLeaderObjectTAG() )
				Party.GetInstance().ChangeLeaderByTag( m_pRecvPacket->m_gsv_PARTY_MEMBER.m_dwObjectTAG[1] );

		}
		else///Join
		{
			pMember = (tag_PARTY_MEMBER*)Packet_GetDataPtr( m_pRecvPacket, nOffset, sizeof( tag_PARTY_MEMBER ) );
			pszName = Packet_GetStringPtr( m_pRecvPacket, nOffset );
			assert( pMember && pszName );
			if( pMember && pszName )
				Party.JoinMember( *pMember, pszName );
		}
	}
	else///When the new party was formed
	{
		///Sign up here to create a new party person.
		if( Party.IsPartyLeader() )///I do not come down.
		{
			CParty::GetInstance().Make();
			pMember = (tag_PARTY_MEMBER*)Packet_GetDataPtr( m_pRecvPacket, nOffset, sizeof( tag_PARTY_MEMBER ) );
			pszName = Packet_GetStringPtr( m_pRecvPacket, nOffset );
			assert( pMember && pszName );
			if( pMember && pszName )
				Party.JoinMember( *pMember, pszName );

			Party.RecvPartyRule( m_pRecvPacket->m_gsv_PARTY_MEMBER.m_btPartyRULE );
		}
		else///The party who is dating the first time / / / I'm down to money?
		{
			CParty::GetInstance().Join();
			for( char i = 0; i < m_pRecvPacket->m_gsv_PARTY_MEMBER.m_cUserCNT; ++i )
			{
				pMember = (tag_PARTY_MEMBER*)Packet_GetDataPtr( m_pRecvPacket, nOffset, sizeof( tag_PARTY_MEMBER ) );
				pszName = Packet_GetStringPtr( m_pRecvPacket, nOffset );
				assert( pMember && pszName );
				if( pMember && pszName )
				{
					Party.JoinMember( *pMember, pszName );
					if( i == 0 )///If the party.
					{
						Party.ChangeLeaderByObjSvrIdx( pMember->m_wObjectIDX );
					}
				}
			}
			Party.RecvPartyRule( m_pRecvPacket->m_gsv_PARTY_MEMBER.m_btPartyRULE );
		}
	}
	_RPT1( _CRT_WARN,"Recv_gsv_PARTY_MEMBER_2 %d\n",g_GameDATA.GetGameTime() );
} 

void CRecvPACKET::Recv_gsv_PARTY_LEVnEXP()
{
	_RPT1( _CRT_WARN,"Recv_gsv_PARTY_LEVnEXP_1(%d)\n" ,g_GameDATA.GetGameTime() );

	CParty::GetInstance().SetExp( m_pRecvPacket->m_gsv_PARTY_LEVnEXP.m_iEXP );
	CParty::GetInstance().SetLevel( m_pRecvPacket->m_gsv_PARTY_LEVnEXP.m_btLEVEL );
	///Eopsi level of
	if( m_pRecvPacket->m_gsv_PARTY_LEVnEXP.m_bitLevelUP )
	{
		CParty::GetInstance().LevelUp();

		//----------------------------------------------------------------------------------------------------			
		/// @brief Attach effects to party members.
		//----------------------------------------------------------------------------------------------------
		if( g_pAVATAR )
			SE_PartyLevelUp( g_pAVATAR->Get_INDEX() );	

	}
	_RPT1( _CRT_WARN,"Recv_gsv_PARTY_LEVnEXP_2(%d)\n" ,g_GameDATA.GetGameTime() );
}

///Once you save the Object List in AVATAR If there is Data Put gives. Object List In the AVATAR If there is Data Put gives.
///Maybe, depending on server architecture has changed struggling packet, the server looks like now
///2004/7/15 - Obj Index party in the event of my death have not come byeonhaedo. JOIN_ZONE in processing.
void CRecvPACKET::Recv_gsv_CHANGE_OBJIDX()
{
	_RPT1( _CRT_WARN,"Recv_gsv_CHANGE_OBJIDX-1(%d)\n", g_GameDATA.GetGameTime() );
	if( CParty::GetInstance().ChangeMemberInfoByUserTag( m_pRecvPacket->m_gsv_CHANGE_OBJIDX.m_Member ) )
	{
		CObjAVT *pObjAVT = g_pObjMGR->Get_ClientCharAVT( m_pRecvPacket->m_gsv_CHANGE_OBJIDX.m_Member.m_wObjectIDX , false ); 
		if ( pObjAVT ) 
		{
			tag_PARTY_MEMBER* pMemberInfo = &(m_pRecvPacket->m_gsv_CHANGE_OBJIDX.m_Member);
			if( pMemberInfo )///If the party
			{
				pObjAVT->Set_HP( pMemberInfo->m_nHP );
				pObjAVT->Set_MaxHP( pMemberInfo->m_nMaxHP );
				pObjAVT->Set_CON( pMemberInfo->m_nCON );
				pObjAVT->Set_AddRecoverHP( pMemberInfo->m_btRecoverHP );
				pObjAVT->Set_AddRecoverMP( pMemberInfo->m_btRecoverMP );
				pObjAVT->SetStamina( pMemberInfo->m_nSTAMINA );
			}
		}
	}
	_RPT1( _CRT_WARN,"Recv_gsv_CHANGE_OBJIDX-2(%d)\n", g_GameDATA.GetGameTime() );

}
//-------------------------------------------------------------------------------------------------
void CRecvPACKET::Recv_gsv_CREATE_ITEM_REPLY()
{
	CTDialog* pDlg = g_itMGR.FindDlg( DLG_TYPE_MAKE );
	if( pDlg && pDlg->IsVision() )
	{
		CMakeDLG* pMakeDlg = ( CMakeDLG* ) pDlg;
		pMakeDlg->RecvResult( m_pRecvPacket );
	}
}
//-------------------------------------------------------------------------------------------------
void CRecvPACKET::Recv_gsv_BANK_LIST_REPLY()
{
	if( !g_pAVATAR )
		return;

	switch( m_pRecvPacket->m_gsv_BANK_LIST_REPLY.m_btREPLY )
	{
	case BANK_REPLY_INIT_DATA:
		{
			g_pAVATAR->InitBank();///
			for( BYTE bt = 0; bt < m_pRecvPacket->m_gsv_BANK_LIST_REPLY.m_btItemCNT;++bt )
				g_pAVATAR->SetBankItem(  m_pRecvPacket->m_gsv_BANK_LIST_REPLY.m_sInvITEM[bt].m_btInvIDX, 
				m_pRecvPacket->m_gsv_BANK_LIST_REPLY.m_sInvITEM[bt].m_ITEM);

			///Information is kept in the bank with the money when
			short Not_IncludeMoneyPacketSize = sizeof( gsv_BANK_LIST_REPLY ) + sizeof(tag_SET_INVITEM) * m_pRecvPacket->m_gsv_BANK_LIST_REPLY.m_btItemCNT;

			if( m_pRecvPacket->m_HEADER.m_nSize  == Not_IncludeMoneyPacketSize + 8 )
			{
				__int64* money = (__int64*)Packet_GetDataPtr( m_pRecvPacket, Not_IncludeMoneyPacketSize, sizeof(__int64) );
				CBank::GetInstance().SetMoney( *money );
			}
		}
	case BANK_REPLY_SHOW_DATA:
		g_itMGR.OpenDialog( DLG_TYPE_ITEM, false );
		g_itMGR.OpenDialog( DLG_TYPE_BANK ,false );
		break;
	case BANK_REPLY_PLATINUM:///Must be later than INIT_DATA.
		{
			for( BYTE bt = 0; bt < m_pRecvPacket->m_gsv_BANK_LIST_REPLY.m_btItemCNT;++bt )
				g_pAVATAR->SetBankItem(  m_pRecvPacket->m_gsv_BANK_LIST_REPLY.m_sInvITEM[bt].m_btInvIDX, 
				m_pRecvPacket->m_gsv_BANK_LIST_REPLY.m_sInvITEM[bt].m_ITEM);
		}
		break;
	case BANK_REPLY_NEED_PASSWORD:// I need to shed off-duty.
		g_itMGR.OpenMsgBox(STR_BANK_REPLY_NEED_PASSWORD);
		break;
	case BANK_REPLY_CHANGED_PASSWORD:// Change off-duty warehouse dwaeteum
		g_itMGR.OpenMsgBox(STR_BANK_REPLY_CHANGED_PASSWORD);
		break;
	case BANK_REPLY_INVALID_PASSWORD:// There must shed off duty
		g_itMGR.OpenMsgBox(STR_BANK_REPLY_INVALID_PASSWORD);
		break;
	case BANK_REPLY_CLEARED_PASSWORD:// Deleted off-duty storage
		g_itMGR.OpenMsgBox( STR_BANK_REPLY_CLEARED_PASSWORD);
		break;
	default:
		break;
	}
}

void CRecvPACKET::Recv_gsv_MOVE_ITEM()
{
	if( !g_pAVATAR )
		return;

	if( m_pRecvPacket->m_gsv_MOVE_ITEM.m_nInvIDX >= 0 )
		g_pAVATAR->Set_ITEM( m_pRecvPacket->m_gsv_MOVE_ITEM.m_nInvIDX, m_pRecvPacket->m_gsv_MOVE_ITEM.m_InvItem );

	if( m_pRecvPacket->m_gsv_MOVE_ITEM.m_nBankIDX >= 0 )
		g_pAVATAR->SetBankItem( m_pRecvPacket->m_gsv_MOVE_ITEM.m_nBankIDX, m_pRecvPacket->m_gsv_MOVE_ITEM.m_BankITEM );

	// If the packet size == gsv_MOVE_ITEM storage => Inventory moves
	// Packet size == gsv_MOVE_ITEM + sizeof (__int64) surface inventory => storage, m_iCurMoney [0] contains the money that
	if( m_pRecvPacket->m_HEADER.m_nSize > sizeof( gsv_MOVE_ITEM) )
		g_pAVATAR->Set_MONEY( m_pRecvPacket->m_gsv_MOVE_ITEM.m_iCurMoney[0] );

	if( m_pRecvPacket->m_gsv_MOVE_ITEM.m_nInvIDX < 0 && m_pRecvPacket->m_gsv_MOVE_ITEM.m_nBankIDX < 0 )
		g_itMGR.AppendChatMsg(STR_BANK_MOVEITEM_FAILED, IT_MGR::CHAT_TYPE_SYSTEM );

}
//-------------------------------------------------------------------------------------------------
///2004/ 2 / 18 : Create : nAvy : Shots consumption related
///2004/ 2 / 19 : Modify : nAvy : If Shots consumption is used in the current Inventory allows the deletion.
void CRecvPACKET::Recv_gsv_SET_BULLET()
{
	CObjAVT* pObjAVT = g_pObjMGR->Get_CharAVT( g_pObjMGR->Get_ClientObjectIndex(m_pRecvPacket->m_gsv_SET_BULLET.m_wObjectIDX ), false);
	if( pObjAVT )
	{
		tagShotDATA ShotData = m_pRecvPacket->m_gsv_SET_BULLET.m_sShot;
		pObjAVT->SetShotData( ShotData.m_cType, ShotData.m_nItemNo );
	}
}
//-------------------------------------------------------------------------------------------------
void CRecvPACKET::Recv_gsv_SERVER_DATA()
{
	switch( m_pRecvPacket->m_gsv_SERVER_DATA.m_btDataTYPE )
	{
	case SERVER_DATA_ECONOMY:
		{
			struct EconomyData{
				DWORD	m_nTown_COUNTER;					// Counter decremented by 1 in one minute.		50~100

				short	m_nTown_POP_BASE;					// Based on the population.					100~8000
				short	m_nTown_DEV_BASE;					// Development is based on					10~100
				short	m_nTown_CONSUM[ MAX_PRICE_TYPE ];	// Consumption by Item

				short	m_nTownDEV;							// Village Development
				int		m_iTownPOP;							// Village population.
				int		m_iTownITEM[ MAX_PRICE_TYPE ];		// Reserves by Item			1000~100000
			};

			EconomyData Data;
			memcpy( &Data, m_pRecvPacket->m_gsv_SERVER_DATA.m_pDATA, sizeof( EconomyData ));


			sprintf( g_MsgBuf,"Based on population:%d", Data.m_nTown_POP_BASE );
			g_itMGR.AppendChatMsg(g_MsgBuf, g_dwYELLOW );

			sprintf( g_MsgBuf,"Development is based on:%d", Data.m_nTown_DEV_BASE );
			g_itMGR.AppendChatMsg(g_MsgBuf, g_dwYELLOW );

			for( int i = 1; i < MAX_PRICE_TYPE; ++i )
			{
				sprintf( g_MsgBuf,"Consumption by Item(%d):%d", i,Data.m_nTown_CONSUM[i] );
				g_itMGR.AppendChatMsg(g_MsgBuf, g_dwYELLOW );
			}

			sprintf( g_MsgBuf,"Village Development:%d", Data.m_nTownDEV );
			g_itMGR.AppendChatMsg(g_MsgBuf, g_dwYELLOW );

			sprintf( g_MsgBuf,"Village population:%d", Data.m_iTownPOP );
			g_itMGR.AppendChatMsg(g_MsgBuf, g_dwYELLOW );


			for( int i = 1; i < MAX_PRICE_TYPE; ++i )
			{
				sprintf( g_MsgBuf,"Reserves by Item(%d):%d", i,Data.m_iTownITEM[i] );
				g_itMGR.AppendChatMsg(g_MsgBuf, g_dwYELLOW );
			}


			break;
		}
	case SERVER_DATA_NPC:
		{
#define	MAX_OBJ_VAR_CNT		20
			struct tagObjVAR 
			{
				union 
				{
					struct 
					{
						int			m_iNextCheckTIME;
						t_HASHKEY	m_HashNextTrigger;
						int			m_iPassTIME;
						union 
						{
							short	m_nEventSTATUS;
							short	m_nAI_VAR[ MAX_OBJ_VAR_CNT ];
						} ;
					} ;
					BYTE m_pVAR[ 1 ];
				};		
			};

			tagObjVAR* pObjVAR = (tagObjVAR*)m_pRecvPacket->m_gsv_SERVER_DATA.m_pDATA;

			g_itMGR.AppendChatMsg( CStr::Printf("NextCheckTIME: %d", pObjVAR->m_iNextCheckTIME ),g_dwYELLOW );
			g_itMGR.AppendChatMsg( CStr::Printf("HashNextTrigger: %d", pObjVAR->m_HashNextTrigger ), g_dwYELLOW );
			g_itMGR.AppendChatMsg( CStr::Printf("PassTime: %d", pObjVAR->m_iPassTIME ), g_dwYELLOW );
			g_itMGR.AppendChatMsg( CStr::Printf("EventStatus: %d", pObjVAR->m_nEventSTATUS), g_dwYELLOW );
			for( int i = 0 ; i < MAX_OBJ_VAR_CNT; ++i )
				g_itMGR.AppendChatMsg( CStr::Printf("AI_VAR(%d):%d", i+1, pObjVAR->m_nAI_VAR[ i ] ), g_dwYELLOW );

			break;
		}
	default:

		break;
	}
}

//-------------------------------------------------------------------------------------------------
void CRecvPACKET::Recv_gsv_ASSEMBLE_RIDE_ITEM()
{
	/*struct gsv_ASSEMBLE_RIDE_ITEM : public t_PACKETHEADER 
	{
	WORD	m_wObjectIDX;
	short	m_nRidingPartIDX;
	short	m_nItemNO;
	} ;*/

	CObjAVT *pAVT = g_pObjMGR->Get_ClientCharAVT( m_pRecvPacket->m_gsv_ASSEMBLE_RIDE_ITEM.m_wObjectIDX, false );
	if ( pAVT ) 
	{
		pAVT->SetPetParts( m_pRecvPacket->m_gsv_ASSEMBLE_RIDE_ITEM.m_nRidingPartIDX,
			m_pRecvPacket->m_gsv_ASSEMBLE_RIDE_ITEM.m_RideITEM.m_nItemNo );
		//m_pRecvPacket->m_gsv_ASSEMBLE_RIDE_ITEM.m_nItemNO );

		pAVT->UpdatePet();

		/// Set speed has changed the face velocity
		if ( m_pRecvPacket->m_HEADER.m_nSize == ( sizeof( gsv_ASSEMBLE_RIDE_ITEM ) + sizeof(short) ) )
		{
			pAVT->SetOri_RunSPEED( m_pRecvPacket->m_gsv_ASSEMBLE_RIDE_ITEM.m_nRunSPEED[0] );
		}

	}else
	{
		;
		// ERROR
	}
}


void CRecvPACKET::Recv_GSV_SET_EVENT_STATUS()
{
	CObjCHAR *pCHAR = g_pObjMGR->Get_CharOBJ( g_pObjMGR->Get_ClientObjectIndex( m_pRecvPacket->m_gsv_SET_EVENT_STATUS.m_wObjectIDX ), false );
	if ( pCHAR ) 
	{
		switch( pCHAR->Get_TYPE() )
		{
		case OBJ_NPC:
			{
				((CObjNPC*)pCHAR)->SetEventValue( m_pRecvPacket->m_gsv_SET_EVENT_STATUS.m_nEventSTATUS );
			}
			break;

		case OBJ_EVENTOBJECT:
			{
				CObjFixedEvent* pObj = (CObjFixedEvent*)pCHAR;					
				pObj->ExecEventScript( m_pRecvPacket->m_gsv_SET_EVENT_STATUS.m_nEventSTATUS );
			}
			break;			
		}
	}
}

//----------------------------------------------------------------------------------------------------
/// @param
/// @brief
//----------------------------------------------------------------------------------------------------
void CRecvPACKET::Recv_GSV_SET_ITEM_LIFE()
{	
	if( !g_pAVATAR )
		return;

	CItemSlot* pItemSlot =  g_pAVATAR->GetItemSlot();
	CItem* pItem = pItemSlot->GetItem( m_pRecvPacket->m_gsv_SET_ITEM_LIFE.m_nInventoryIDX );
	if( pItem )
	{
		pItem->SetLife( m_pRecvPacket->m_gsv_SET_ITEM_LIFE.m_nLife );

		if( pItem->GetItem().GetLife() <= 0 &&
			(ITEM_TYPE( pItem->GetItem().GetTYPE(), pItem->GetItem().GetItemNO() ) == TUNING_PART_ENGINE_CART ||
			ITEM_TYPE( pItem->GetItem().GetTYPE(), pItem->GetItem().GetItemNO() ) == TUNING_PART_ENGINE_CASTLEGEAR) )
		{						
			g_itMGR.AppendChatMsg( STR_CANT_REFAIR_PAT_ENGINE, IT_MGR::CHAT_TYPE_SYSTEM );
		}

		if( pItem->GetItem().GetLife() <= 0  )
		{
			g_pAVATAR->UpdateAbility();
		}	
	}
}


void CRecvPACKET::Recv_lsv_CHANNEL_LIST_REPLY ()
{

	DWORD worldserver_id = m_pRecvPacket->m_lsv_CHANNEL_LIST_REPLY.m_dwServerID;
	CServerList::GetInstance().ClearChannelServerList( worldserver_id );

	short nOffset = sizeof( lsv_CHANNEL_LIST_REPLY );

	if( m_pRecvPacket->m_lsv_CHANNEL_LIST_REPLY.m_btChannelCNT > 0 )
	{
		tagCHANNEL_SRV* pChannel = NULL;
		char* pszChannelName = NULL;
		int iFirstChannelNo = 0;
		for( int i = 0; i < m_pRecvPacket->m_lsv_CHANNEL_LIST_REPLY.m_btChannelCNT; ++i )
		{
			pChannel = (tagCHANNEL_SRV*)Packet_GetDataPtr( m_pRecvPacket, nOffset, sizeof(tagCHANNEL_SRV) );
			pszChannelName = Packet_GetStringPtr( m_pRecvPacket, nOffset);
			if( !pszChannelName )
				pszChannelName = CStr::Printf("Channel-%d", pChannel->m_btChannelNO );

			CServerList::GetInstance().AddChannelServerList( worldserver_id, pChannel->m_btChannelNO, pszChannelName, pChannel->m_nUserPERCENT );

			if( i == 0 )
				iFirstChannelNo = pChannel->m_btChannelNO;

		}
		g_EUILobby.HideMsgBox();
	}
}


//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
#include "../GameData/CChatRoom.h"
void CRecvPACKET::Recv_wsv_CHATROOM ()
{
	switch( m_pRecvPacket->m_tag_CHAT_HEADER.m_btCMD ) {
		case CHAT_REPLY_ROOM_MAKED		:
			{
				short nOffset = sizeof( tag_CHAT_HEADER );
				WORD* pwMyID = (WORD*)Packet_GetDataPtr( m_pRecvPacket, nOffset, sizeof(WORD) );
				//char* szRoomName = Packet_GetStringPtr( m_pRecvPacket, nOffset );

				CChatRoom::GetInstance().Made( *pwMyID );
				CChatRoom::GetInstance().SetMaster( *pwMyID );

				break;
			}
		case CHAT_REPLY_ROOM_JOINED		:
			{
				short nOffset = sizeof( tag_CHAT_HEADER );
				WORD* pwMyID = (WORD*)Packet_GetDataPtr( m_pRecvPacket, nOffset, sizeof(WORD) );
				
				// UserTag				
#ifdef __PRIVATECHAT2
				DWORD * pdwMyTag = (DWORD*)Packet_GetDataPtr(m_pRecvPacket, nOffset, sizeof(DWORD));
#endif
				char* szRoomName = Packet_GetStringPtr( m_pRecvPacket, nOffset );

				CChatRoom::GetInstance().SetTitle( szRoomName );
				CChatRoom::GetInstance().Join();			

				WORD* pwUserID;
				DWORD * pdwUserTag;
				char* szUserName;
				bool bSetMaster = false;

				for( ; nOffset < m_pRecvPacket->m_HEADER.m_nSize ; )
				{
					pwUserID = (WORD*)Packet_GetDataPtr( m_pRecvPacket, nOffset, sizeof( WORD ));
					
					// UserTag				
#ifdef __PRIVATECHAT2
					pdwUserTag = (DWORD*)Packet_GetDataPtr(m_pRecvPacket, nOffset, sizeof(DWORD));
					szUserName = Packet_GetStringPtr(m_pRecvPacket, nOffset);
					CChatRoom::GetInstance().JoinMember( *pwUserID, szUserName, *pdwUserTag );
#else
					szUserName = Packet_GetStringPtr(m_pRecvPacket, nOffset);
					CChatRoom::GetInstance().JoinMember( *pwUserID, szUserName );
#endif
					if( !bSetMaster )
					{
						CChatRoom::GetInstance().SetMaster( *pwUserID );
						bSetMaster = true;
					}
				}

#ifdef __PRIVATECHAT2
				if( g_pAVATAR )
				{
					CChatRoom::GetInstance().JoinMember( 
						*pwMyID,
						g_pAVATAR->Get_NAME(),
						*pdwMyTag );
				}
#else
				if( g_pAVATAR )
				{
					CChatRoom::GetInstance().JoinMember( 
						*pwMyID,
						g_pAVATAR->Get_NAME() );
				}
#endif
				

				break;
			}

		case CHAT_REPLY_MAKE_FAILED		:	// Making room failed ...
			g_itMGR.OpenMsgBox(STR_CHAT_REPLY_MAKE_FAILED);
			CChatRoom::GetInstance().SetState( CChatRoom::STATE_DEACTIVATED );

			CChatRoom::GetInstance().SetPassword(NULL);
			CChatRoom::GetInstance().SetTitle(NULL);
			break;

		case CHAT_REPLY_MAKE_TITLE_DUP		:	//...Failure to make a duplicate control room
			g_itMGR.OpenMsgBox(LIST_STRING(952));
			CChatRoom::GetInstance().SetState( CChatRoom::STATE_DEACTIVATED );

			CChatRoom::GetInstance().SetPassword(NULL);
			CChatRoom::GetInstance().SetTitle(NULL);
			break;

		case CHAT_REPLY_INVALIED_PASS	:	// The requested number of participants must be off-duty.
			g_itMGR.OpenMsgBox( STR_CHAT_REPLY_INVALIED_PASS );
			CChatRoom::GetInstance().SetState( CChatRoom::STATE_DEACTIVATED );
			break;
		case CHAT_REPLY_ROOM_NOT_FOUND	:	// Jyeoteum ppogae requested room.
			g_itMGR.OpenMsgBox(STR_CHAT_REPLY_ROOM_NOT_FOUND);
			CChatRoom::GetInstance().SetState( CChatRoom::STATE_DEACTIVATED );
			///Chat will be deleted from the list
			break;
		case CHAT_REPLY_FULL_USERS		:	// Im requested the unlimited room.
			g_itMGR.OpenMsgBox(STR_CHAT_REPLY_FULL_USERS);
			CChatRoom::GetInstance().SetState( CChatRoom::STATE_DEACTIVATED );
			break;

		case CHAT_REPLY_KICKED			:	
			{
				if( g_pAVATAR )
				{
					// Jjot gyeonan CHAT_REPLY_USER_LEFT-like structure ... someone who comes ... but the only person he'd sent to the CHAT_REPLY_USER_LEFT
					if( g_pObjMGR->Get_ServerObjectIndex(g_pAVATAR->Get_INDEX()) 
						== m_pRecvPacket->m_wsv_CHAT_ROOM_USER.m_wUserID )
					{
						CChatRoom::GetInstance().Leave();
						g_itMGR.AppendChatMsg(STR_CHAT_REPLY_KICKED,IT_MGR::CHAT_TYPE_SYSTEM );
					}
				}
				break;
			}
		case CHAT_REPLY_USER_LEFT		:
			{
				CChatRoom::GetInstance().LeaveMember( m_pRecvPacket->m_wsv_CHAT_ROOM_USER.m_wUserID );
				break;
			}
		case CHAT_REPLY_USER_LEFT_NAME:
			{
				short nOffset = sizeof( tag_CHAT_HEADER );
				char* pszName = Packet_GetStringPtr( m_pRecvPacket, nOffset );
				CChatRoom::GetInstance().LeaveMember( pszName );
				break;
			}
		case CHAT_REPLY_USER_ADD		:	
			{
				char* pszUserName;
				short nOffset = sizeof( wsv_CHAT_ROOM_USER );	

				pszUserName = Packet_GetStringPtr(m_pRecvPacket, nOffset);

				// UserTag				
#ifdef __PRIVATECHAT2
				CChatRoom::GetInstance().JoinMember( 
					m_pRecvPacket->m_wsv_CHAT_ROOM_USER.m_wUserID,
					pszUserName,
					m_pRecvPacket->m_wsv_CHAT_ROOM_USER.m_dwDBID );
#else
				CChatRoom::GetInstance().JoinMember( 
					m_pRecvPacket->m_wsv_CHAT_ROOM_USER.m_wUserID,
					pszUserName );
#endif

				
				break;
			}

		case CHAT_REPLY_ROOM_LIST_END:
		case CHAT_REPLY_ROOM_LIST:
			{
				tag_CHAT_ROOM *pRoom;
				short nOffset = sizeof( wsv_CHAT_ROOM_LIST );
				char *szRoomName;

				CTDialog* pDlg = g_itMGR.FindDlg( DLG_TYPE_COMMUNITY );
				assert( pDlg );
				if( pDlg == NULL ) break;

				CCommDlg* pCommDlg = (CCommDlg*)pDlg;
				for (short nI=0; nI<m_pRecvPacket->m_wsv_CHAT_ROOM_LIST.m_cRoomCNT; nI++) 
				{
					pRoom = (tag_CHAT_ROOM*)Packet_GetDataPtr( m_pRecvPacket, nOffset, sizeof(tag_CHAT_ROOM) );
					szRoomName = Packet_GetStringPtr( m_pRecvPacket, nOffset );

					pCommDlg->AddChatRoom( szRoomName, pRoom->m_btRoomTYPE, pRoom->m_nRoomIDX, pRoom->m_cUserCNT );
				}
				break;
			}
	}
}
#include "../interface/dlgs/subclass/CFriendListItem.h"
void CRecvPACKET::Recv_tag_MCMD_HEADER ()		//0x07e1
{
	switch( m_pRecvPacket->m_tag_MCMD_HEADER.m_btCMD ) 
	{
	case MSGR_CMD_APPEND_REQ	:
		{
				if( g_ClientStorage.IsApproveAddFriend() )
				{
					if(g_pAVATAR->GetAddFriendWnd() == false)
					{
						g_pAVATAR->SetAddFriendWnd(true);
					CTCmdAcceptAddFriend* pAcceptCmd = new CTCmdAcceptAddFriend( m_pRecvPacket->m_wsv_MCMD_APPEND_REQ.m_wUserIDX,
						0,
						m_pRecvPacket->m_wsv_MCMD_APPEND_REQ.m_szName);

					CTCmdRejectAddFriend* pRejectCmd = new CTCmdRejectAddFriend( m_pRecvPacket->m_wsv_MCMD_APPEND_REQ.m_wUserIDX ,
#ifdef __PRIVATECHAT2
						g_pAVATAR->Get_NAME() );
#else
						m_pRecvPacket->m_wsv_MCMD_APPEND_REQ.m_szName );
#endif					

					g_itMGR.OpenMsgBox( CStr::Printf( F_STR_QUERY_APPEND_FRIEND_REQ, m_pRecvPacket->m_wsv_MCMD_APPEND_REQ.m_szName ),
						CMsgBox::BT_OK | CMsgBox::BT_CANCEL, 
						false,
						0,
						pAcceptCmd,
						pRejectCmd );
					}
                }
				else
				{
					CTCmdRejectAddFriend Command(	m_pRecvPacket->m_wsv_MCMD_APPEND_REQ.m_wUserIDX ,
#ifdef __PRIVATECHAT2
													g_pAVATAR->Get_NAME() );
#else
													m_pRecvPacket->m_wsv_MCMD_APPEND_REQ.m_szName );
#endif					
													
					Command.Exec( NULL );
				}
			break;
		}
	case MSGR_CMD_APPEND_ACCEPT	:
		{
			CTDialog* pDlg = g_itMGR.FindDlg( DLG_TYPE_COMMUNITY );
			assert( pDlg );
			if( pDlg )
			{
				CCommDlg* pCommDlg = (CCommDlg*)pDlg;
				//Existing 
				if( CFriendListItem* pItem = pCommDlg->FindFriend( m_pRecvPacket->m_wsv_MCMD_APPEND_ACCEPT.m_dwUserTAG ) )
				{
					pItem->SetStatus( m_pRecvPacket->m_wsv_MCMD_APPEND_ACCEPT.m_btStatus );
				}
				else
				{
					pCommDlg->AddFriend( m_pRecvPacket->m_wsv_MCMD_APPEND_ACCEPT.m_dwUserTAG,
						m_pRecvPacket->m_wsv_MCMD_APPEND_ACCEPT.m_btStatus,
						m_pRecvPacket->m_wsv_MCMD_APPEND_ACCEPT.m_szName );
				}

			}
			break;
		}
	case MSGR_CMD_APPEND_REJECT	:
		{
			// For more friend requests (szName) is refused.
			short nOffset = sizeof( tag_MCMD_HEADER );
			char *szName = Packet_GetStringPtr( m_pRecvPacket, nOffset );
			g_itMGR.OpenMsgBox(CStr::Printf(F_STR_REJECT_APPEND_FRIEND_REQ, szName) );
			break;
		}
	case MSGR_CMD_NOT_FOUND		:
		{
			// For more friend requests (szName) is not
			short nOffset = sizeof( tag_MCMD_HEADER );
			char *szName = Packet_GetStringPtr( m_pRecvPacket, nOffset );
			g_itMGR.OpenMsgBox(CStr::Printf(F_STR_NOT_FOUNDORCONNECT_FRIEND, szName) );
			break;
		}
	case MSGR_CMD_CHANGE_STATUS	:
		{
			// Users m_pRecvPacket-> m_wsv_MCMD_STATUS_REPLY.m_dwUserTAG the state
			// m_pRecvPacket-> m_wsv_MCMD_STATUS_REPLY.m_btStatus had to change.
			CTDialog* pDlg = g_itMGR.FindDlg( DLG_TYPE_COMMUNITY );
			assert( pDlg );
			if( pDlg )
			{
				CCommDlg* pCommDlg = (CCommDlg*)pDlg;
				pCommDlg->ChangeFriendStatus( m_pRecvPacket->m_wsv_MCMD_STATUS_REPLY.m_dwUserTAG,
					m_pRecvPacket->m_wsv_MCMD_STATUS_REPLY.m_btStatus );
			}		
			break;
		}
	case MSGR_CMD_LIST:
		{
			// Hotlist ...
			//ClientLog(LOG_DEBUG,"Received 0x07e1 packet with a 0x006 command");
#pragma pack (push, 1)
			struct tagFriend {
				DWORD	m_dwUserTAG;
				BYTE	m_btStatus;
			} *pFR;
#pragma pack (pop)
			char *szName;
			CTDialog* pDlg = g_itMGR.FindDlg( DLG_TYPE_COMMUNITY );
			assert( pDlg );
			if( pDlg )
			{
				CCommDlg* pCommDlg = (CCommDlg*)pDlg;
				pCommDlg->ClearFriendList();

				short nOffset = sizeof( wsv_MCMD_LIST );
				for (short nI=0; nI<m_pRecvPacket->m_wsv_MCMD_LIST.m_btFriendCNT; nI++) 
				{
					pFR = (tagFriend*)Packet_GetDataPtr( m_pRecvPacket, nOffset, sizeof(tagFriend ) );
					szName = Packet_GetStringPtr( m_pRecvPacket, nOffset );

					pCommDlg->AddFriend( pFR->m_dwUserTAG, pFR->m_btStatus, szName );
				}
			}
			//ClientLog(LOG_DEBUG,"Finished processing 0x07e1 packet with a 0x006 command");
			break;
		}
	default:
		break;
		/*
		case MSGR_CMD_DELETE		:	
		{
		break;
		}
		case MSGR_CMD_REFUSE		:	
		{
		break;
		}
		*/
	}
}

//----------------------------------------------------------------------------------------------------
#include "../interface/dlgs/subclass/CFriendListItem.h"
#include "../interface/dlgs/CPrivateChatDlg.h"
void CRecvPACKET::Recv_wsv_MESSENGER_CHAT ()
{
	CTDialog* pDlg = g_itMGR.FindDlg( DLG_TYPE_COMMUNITY );
	assert( pDlg );
	if( pDlg == NULL ) return;


#ifdef __PRIVATECHAT2	
		//Honggeun: Japan Billing modification
	#ifdef __JAPAN_BILL
		if( !(CGame::GetInstance().GetPayType() & PLAY_FLAG_COMMUNITY) )
		{					
			g_pNet->Send_cli_WHISPER(  m_pRecvPacket->m_wsv_MESSENGER_CHAT.m_FromName, STR_JP_BILL_CANT_SEND_MSG );
			return;
		}        
	#endif

		if( g_ClientStorage.IsApproveMessanger() )
		{
			CPrivateChatDlg* pPrivateChatDlg = g_itMGR.GetPrivateChatDlg( m_pRecvPacket->m_wsv_MESSENGER_CHAT.m_dwUserTAG );
			if( pPrivateChatDlg  == NULL )
			{
				g_itMGR.OpenPrivateChatDlg( m_pRecvPacket->m_wsv_MESSENGER_CHAT.m_dwUserTAG,
											FRIEND_STATUS_NORMAL,
											m_pRecvPacket->m_wsv_MESSENGER_CHAT.m_FromName );
			}

			if( pPrivateChatDlg = g_itMGR.GetPrivateChatDlg( m_pRecvPacket->m_wsv_MESSENGER_CHAT.m_dwUserTAG ) )
			{
				pPrivateChatDlg->RecvChatMsg( m_pRecvPacket->m_wsv_MESSENGER_CHAT.m_dwUserTAG ,
					m_pRecvPacket->m_wsv_MESSENGER_CHAT.m_szMSG );
			}
			else
			{
				assert( pPrivateChatDlg );
			}
		}
		else
		{			
			g_pNet->Send_cli_WHISPER(	m_pRecvPacket->m_wsv_MESSENGER_CHAT.m_FromName,
										STR_OTHERSTATE_IS_REJCTMESSANGER );
		}

#else
	CCommDlg* pCommDlg = (CCommDlg*)pDlg;
	CFriendListItem* pItem = pCommDlg->FindFriend( m_pRecvPacket->m_wsv_MESSENGER_CHAT.m_dwUserTAG );

	if( pItem )
	{

		//Honggeun: Japan Billing modification
#ifdef __JAPAN_BILL
		if( !(CGame::GetInstance().GetPayType() & PLAY_FLAG_COMMUNITY) )
		{					
			g_pNet->Send_cli_WHISPER(  (char*)pItem->GetName(), STR_JP_BILL_CANT_SEND_MSG );
			return;
		}        
#endif

		if( g_ClientStorage.IsApproveMessanger() )
		{
			CPrivateChatDlg* pPrivateChatDlg = g_itMGR.GetPrivateChatDlg( m_pRecvPacket->m_wsv_MESSENGER_CHAT.m_dwUserTAG );
			if( pPrivateChatDlg  == NULL )
				g_itMGR.OpenPrivateChatDlg( m_pRecvPacket->m_wsv_MESSENGER_CHAT.m_dwUserTAG, pItem->GetStatus(), pItem->GetName() );

			if( pPrivateChatDlg = g_itMGR.GetPrivateChatDlg( m_pRecvPacket->m_wsv_MESSENGER_CHAT.m_dwUserTAG ) )
			{
				pPrivateChatDlg->RecvChatMsg( m_pRecvPacket->m_wsv_MESSENGER_CHAT.m_dwUserTAG ,
					m_pRecvPacket->m_wsv_MESSENGER_CHAT.m_szMSG );
			}
			else
			{
				assert( pPrivateChatDlg );
			}
		}
		else
		{			
			g_pNet->Send_cli_WHISPER(  (char*)pItem->GetName(), STR_OTHERSTATE_IS_REJCTMESSANGER );
		}
	}
#endif
	
}
//----------------------------------------------------------------------------------------------------
void CRecvPACKET::Recv_wsv_MEMO()
{
	switch( m_pRecvPacket->m_wsv_MEMO.m_btTYPE )
	{
	case MEMO_REPLY_RECEIVED_CNT:
		{
			if( g_pAVATAR )
			{
				g_pAVATAR->SetServerSavedMemoCount( m_pRecvPacket->m_wsv_MEMO.m_nRecvCNT[0] );
				if( g_pAVATAR->GetServerSavedMemoCount() > 0 )
					g_pNet->Send_cli_MEMO_REQ();
			}
			break;
		}
	case MEMO_REPLY_CONTENTS:
		{
			CTDialog* pDlg = g_itMGR.FindDlg( DLG_TYPE_COMMUNITY );
			assert( pDlg );
			if( pDlg )
			{
				CCommDlg* pCommDlg = (CCommDlg*)pDlg;

				DWORD*	pdwRecvTime;
				char*	pszFromName;
				char*	pszMemo;

				for( short	nOffset = sizeof( wsv_MEMO ) ; nOffset < m_pRecvPacket->m_HEADER.m_nSize ; )
				{
					pdwRecvTime = (DWORD*)Packet_GetDataPtr( m_pRecvPacket, nOffset, sizeof( DWORD ) );
					pszFromName	= Packet_GetStringPtr( m_pRecvPacket, nOffset );
					pszMemo		= Packet_GetStringPtr( m_pRecvPacket, nOffset );

					///File
					//pCommDlg->AddMemo( pszFromName, pszMemo, *pdwRecvTime );
					CSqliteDB::GetInstance().AddMemo( g_pAVATAR->Get_NAME(), pszFromName, pszMemo, *pdwRecvTime );

					if( g_pAVATAR )
						g_pAVATAR->DecreaseServerSavedMemoCount();
				}

				if( g_pAVATAR )
				{
					if( g_pAVATAR->GetServerSavedMemoCount() > 0 )
						g_pNet->Send_cli_MEMO_REQ();
				}
			}
			break;
		}
	case MEMO_REPLY_SEND_OK:
		break;
	case MEMO_REPLY_SEND_INVALID_TARGET:
	case MEMO_REPLY_SEND_NOT_EXIST:
		g_itMGR.OpenMsgBox(STR_MEMO_REPLY_SEND_NOT_EXISTORINVALID);
		break;
	case MEMO_REPLY_SEND_REFUSED:
		g_itMGR.OpenMsgBox(STR_MEMO_REPLY_SEND_REFUSED);
		break;
	case MEMO_REPLY_SEND_FULL_MEMO:
		g_itMGR.OpenMsgBox(STR_MEMO_REPLY_SEND_FULL_MEMO);
		break;
	case MEMO_REPLY_SEND_INVALID_CONTENT:
		g_itMGR.OpenMsgBox(STR_MEMO_REPLY_SEND_INVALID_CONTENT);
		break;
	default:
		break;
	}
}
#include "../interface/dlgs/CChatRoomDlg.h"
void CRecvPACKET::Recv_wsv_CHATROOM_MSG ()
{
	CTDialog* pDlg = g_itMGR.FindDlg( DLG_TYPE_CHATROOM );
	assert( pDlg );
	if( pDlg )
	{
		CChatRoomDlg* pChatRoomDlg = ( CChatRoomDlg*) pDlg;
		pChatRoomDlg->RecvChatMsg( m_pRecvPacket->m_wsv_CHATROOM_MSG.m_wObjectID,
			m_pRecvPacket->m_wsv_CHATROOM_MSG.m_szMSG
			);
	}
}


//----------------------------------------------------------------------------------------------------
/// @param
/// @brief
//----------------------------------------------------------------------------------------------------
void CRecvPACKET::Recv_gsv_CRAFT_ITEM_REPLY()
{
	//struct gsv_CRAFT_ITEM_REPLY : public t_PACKETHEADER {
	//BYTE			m_btRESULT;
	//BYTE			m_btOutCNT;			// Item number changed
	//tag_SET_INVITEM	m_sInvITEM[ 0 ];	// For as long as the number has been changed ... 
	//									// Exception) CRAFT_UPGRADE_SUCCESS, CRAFT_UPGRADE_FAILED If the
	//									// m_sInvITEM [m_btOutCNT-1]. m_uiQuantity example of the success that the calculated value is
	//} ;

	switch( m_pRecvPacket->m_gsv_CRAFT_ITEM_REPLY.m_btRESULT )
	{
	case	CRAFT_GEMMING_SUCCESS:		//	0x01
		{
			//PY: Fix gemming later
			/*
			g_pAVATAR->SetWaitUpdateInventory( true );
			for( int i = 0; i < m_pRecvPacket->m_gsv_CRAFT_ITEM_REPLY.m_btOutCNT; i++ )
			{
				int iPartIdx = CInventory::GetBodyPartByEquipSlot( m_pRecvPacket->m_gsv_CRAFT_ITEM_REPLY.m_sInvITEM[ i ].m_btInvIDX );

				if( g_pAVATAR )
				{
					g_pAVATAR->SetPartITEM( iPartIdx,
						m_pRecvPacket->m_gsv_CRAFT_ITEM_REPLY.m_sInvITEM[ i ].m_ITEM );
					g_pAVATAR->Set_ITEM( m_pRecvPacket->m_gsv_CRAFT_ITEM_REPLY.m_sInvITEM[ i ].m_btInvIDX,
						m_pRecvPacket->m_gsv_CRAFT_ITEM_REPLY.m_sInvITEM[ i ].m_ITEM );
					/// Because packets are carried Equip Property seems unnecessary.
					/// Doeldeut updated only when the item slot.
					g_pAVATAR->Update();
				}
			}
			g_pAVATAR->SetWaitUpdateInventory( false );
			g_pAVATAR->UpdateInventory();


			g_itMGR.AppendChatMsg( STR_GEMMING_SUCCESS, IT_MGR::CHAT_TYPE_SYSTEM );					
			*/
		}
		break;
	case	CRAFT_GEMMING_NEED_SOCKET:	//	0x02	// No sockets.
		{
			g_itMGR.AppendChatMsg( STR_GEMMING_NEED_SOCKET, IT_MGR::CHAT_TYPE_SYSTEM );					
		}
		break;
	case	CRAFT_GEMMING_USED_SOCKET:	//	0x03	// If jammed, and more ....
		break;

	case	CRAFT_BREAKUP_SUCCESS_GEM:	//	0x04	// Jewelry separate success
		{
			//PY: fix later
			/*
			if( g_pAVATAR )
			{
				g_pAVATAR->SetWaitUpdateInventory( true );
				for( int i = 0; i < m_pRecvPacket->m_gsv_CRAFT_ITEM_REPLY.m_btOutCNT; i++ )
				{
					g_pAVATAR->Set_ITEM( m_pRecvPacket->m_gsv_CRAFT_ITEM_REPLY.m_sInvITEM[ i ].m_btInvIDX,
						m_pRecvPacket->m_gsv_CRAFT_ITEM_REPLY.m_sInvITEM[ i ].m_ITEM );
				}
				g_pAVATAR->SetWaitUpdateInventory( false );
				g_pAVATAR->UpdateInventory();

				g_itMGR.OpenMsgBox(STR_CRAFT_BREAKUP_SUCCESS_GEM);

				CSeparate& Separate = CSeparate::GetInstance();
				switch( Separate.GetType() )
				{
				case CSeparate::TYPE_SKILL:
					g_pAVATAR->Sub_MP( Separate.GetRequireMp() );
					break;
				case CSeparate::TYPE_NPC:
					g_pAVATAR->Set_MONEY( g_pAVATAR->Get_MONEY() - Separate.GetRequireMoney() );
					break;
				default:
					break;
				}			

				SE_SuccessSeparate( g_pAVATAR->Get_INDEX() );
			}
			*/

			break;
		}
	case	CRAFT_BREAKUP_DEGRADE_GEM:	//	0x05	// Jewelry separate success, jewelery rating downgrade
		{
			//PY: fix later
			/*
			if( g_pAVATAR )
			{
				g_pAVATAR->SetWaitUpdateInventory( true );
				for( int i = 0; i < m_pRecvPacket->m_gsv_CRAFT_ITEM_REPLY.m_btOutCNT; i++ )
				{
					g_pAVATAR->Set_ITEM( m_pRecvPacket->m_gsv_CRAFT_ITEM_REPLY.m_sInvITEM[ i ].m_btInvIDX,
						m_pRecvPacket->m_gsv_CRAFT_ITEM_REPLY.m_sInvITEM[ i ].m_ITEM );
				}
				g_pAVATAR->SetWaitUpdateInventory( false );
				g_pAVATAR->UpdateInventory();

				CSeparate& Separate = CSeparate::GetInstance();
				switch( Separate.GetType() )
				{
				case CSeparate::TYPE_SKILL:
					g_pAVATAR->Sub_MP( Separate.GetRequireMp() );
					break;
				case CSeparate::TYPE_NPC:
					g_pAVATAR->Set_MONEY( g_pAVATAR->Get_MONEY() - Separate.GetRequireMoney() );
					break;
				default:
					break;
				}

				SE_SuccessSeparate( g_pAVATAR->Get_INDEX() );
				g_itMGR.OpenMsgBox(STR_CRAFT_BREAKUP_DEGRADE_GEM);
			}
			*/
			break;
		}
	case	CRAFT_BREAKUP_CLEARED_GEM:	//	0x06	// Jewelry separate successful, jewelry deleted
		{
			//PY: Fix later
			/*
			if( g_pAVATAR )
			{
				g_pAVATAR->SetWaitUpdateInventory( true );
				for( int i = 0; i < m_pRecvPacket->m_gsv_CRAFT_ITEM_REPLY.m_btOutCNT; i++ )
				{
					g_pAVATAR->Set_ITEM( m_pRecvPacket->m_gsv_CRAFT_ITEM_REPLY.m_sInvITEM[ i ].m_btInvIDX,
						m_pRecvPacket->m_gsv_CRAFT_ITEM_REPLY.m_sInvITEM[ i ].m_ITEM );
				}
				g_pAVATAR->SetWaitUpdateInventory( false );
				g_pAVATAR->UpdateInventory();


				CSeparate& Separate = CSeparate::GetInstance();
				switch( Separate.GetType() )
				{
				case CSeparate::TYPE_SKILL:
					g_pAVATAR->Sub_MP( Separate.GetRequireMp() );
					break;
				case CSeparate::TYPE_NPC:
					g_pAVATAR->Set_MONEY( g_pAVATAR->Get_MONEY() - Separate.GetRequireMoney() );
					break;
				default:
					break;
				}

				g_itMGR.OpenMsgBox(STR_CRAFT_BREAKUP_CLEARED_GEM);
				SE_SuccessSeparate( g_pAVATAR->Get_INDEX() );
			}
			*/
			break;
		}

	case	CRAFE_BREAKUP_SUCCESS:		//	0x07	// Items disassembly success
		{

			if( g_pAVATAR )
			{
				ClientLog (LOG_NORMAL, "Breaking the item. Giving %i items ", m_pRecvPacket->m_gsv_CRAFT_ITEM_REPLY.m_btOutCNT );

				CSeparate& Separate = CSeparate::GetInstance();

				switch( Separate.GetType() )
				{
				case CSeparate::TYPE_SKILL:
					//06. 01. 1 :Kim, Joo - Hyun - New version of the disassembly, so the skill MP consumption by not getting the basic skills.
					break;

				case CSeparate::TYPE_NPC:
					g_pAVATAR->Set_MONEY( g_pAVATAR->Get_MONEY() - Separate.GetRequireMoney() );
					break;

				default:
					break;
				}

				Separate.ClearResultItemSet();

				// 05.11.30 - 김주현
				// Note m_sInvITEM last m_btInvIDX trying to disassembly the actual index of the item that contains a clamped.
				//		m_sInvITEM last m_ITEM be zero filled. attention.
				for( int i = 0; i < m_pRecvPacket->m_gsv_CRAFT_ITEM_REPLY.m_btOutCNT; i++ )
				{
					
					tag_SET_INVITEM tmpItem;
					tmpItem.m_btInvIDX = m_pRecvPacket->m_gsv_CRAFT_ITEM_REPLY.m_sInvITEM[i].py_Slot;
					short ThisType = m_pRecvPacket->m_gsv_CRAFT_ITEM_REPLY.m_sInvITEM[ i ].py_ItemType;
					short ThisNo = m_pRecvPacket->m_gsv_CRAFT_ITEM_REPLY.m_sInvITEM[ i ].py_ItemNum;
					byte ItemCnt = m_pRecvPacket->m_gsv_CRAFT_ITEM_REPLY.m_sInvITEM [i ].py_ItemCnt;
					ClientLog (LOG_NORMAL, "Item %i: type: %i: number:  %i: Slot: %i: Count: %i", i + 1, ThisType, ThisNo, tmpItem.m_btInvIDX, ItemCnt );
					if(ThisType >= ITEM_TYPE_USE && ThisType < ITEM_TYPE_RIDE_PART)	//It's stackable Type 10, 11, 12, 13.  
					{
						tmpItem.m_ITEM.SetItemNo(ThisNo);
						tmpItem.m_ITEM.SetType1(ThisType);
						tmpItem.m_ITEM.SetQuantity1(ItemCnt);
					}
					else	//It's an equip
					{
						tmpItem.m_ITEM.SetItemNo(ThisNo);
						tmpItem.m_ITEM.SetType(ThisType);
						ItemCnt = 1;
					}
					//Maybe we need to send it this function a number of times equal to the item count
					//for( int j=0; j< ItemCnt; j++)
					//{
						//ClientLog (LOG_NORMAL, "Sending item %i of %i",j,ItemCnt);
						Separate.AddResultItemSet( tmpItem );
					//}
					//Separate.AddResultItemSet( m_pRecvPacket->m_gsv_CRAFT_ITEM_REPLY.m_sInvITEM[ i ] );
				}
				ClientLog (LOG_NORMAL, "Finished adding the items");
				Separate.SetResult(m_pRecvPacket->m_gsv_CRAFT_ITEM_REPLY.m_btRESULT );
				ClientLog (LOG_NORMAL, "Finished SetResult");

			}
			break;

		}
	case	CRAFE_INVALID_ITEM:  // Something went wrong!
		{
#ifdef _NEWINTERFACEKJH
			g_itMGR.OpenMsgBox( "CRAFT_INVALID_ITEM_ERROR" );
#endif
		}
		break;
	case	CRAFT_UPGRADE_SUCCESS:		//	0x10	// Jaeryeon success
		{
			//PY: fix later
			/*
			if( g_pAVATAR )
			{
				CUpgrade& Upgrade = CUpgrade::GetInstance();
				Upgrade.ClearResultItemSet();

				for( int i = 0; i < m_pRecvPacket->m_gsv_CRAFT_ITEM_REPLY.m_btOutCNT - 1; i++ )
					Upgrade.AddResultItemSet( m_pRecvPacket->m_gsv_CRAFT_ITEM_REPLY.m_sInvITEM[ i ] );

				Upgrade.SetResultSuccessProb( m_pRecvPacket->m_gsv_CRAFT_ITEM_REPLY.m_sInvITEM[ m_pRecvPacket->m_gsv_CRAFT_ITEM_REPLY.m_btOutCNT - 1].m_ITEM.m_uiQuantity );
				Upgrade.SetResult( m_pRecvPacket->m_gsv_CRAFT_ITEM_REPLY.m_btRESULT  );

				switch( Upgrade.GetType() )
				{
				case CUpgrade::TYPE_SKILL:
					g_pAVATAR->Sub_MP( Upgrade.GetRequireMp() );
					break;
				case CUpgrade::TYPE_NPC:
					g_pAVATAR->Set_MONEY( g_pAVATAR->Get_MONEY() - Upgrade.GetRequireMoney() );
					break;
				default:
					break;
				}
			}
			*/
			break;
		}
	case	CRAFT_UPGRADE_FAILED:		//	0x11	// Failure jaeryeon
		{
			//PY: fix later
			/*
			if( g_pAVATAR )
			{
				CUpgrade& Upgrade = CUpgrade::GetInstance();
				Upgrade.ClearResultItemSet();
				for( int i = 0; i < m_pRecvPacket->m_gsv_CRAFT_ITEM_REPLY.m_btOutCNT - 1; i++ )
					Upgrade.AddResultItemSet( m_pRecvPacket->m_gsv_CRAFT_ITEM_REPLY.m_sInvITEM[ i ] );

				Upgrade.SetResultSuccessProb( m_pRecvPacket->m_gsv_CRAFT_ITEM_REPLY.m_sInvITEM[ m_pRecvPacket->m_gsv_CRAFT_ITEM_REPLY.m_btOutCNT - 1].m_ITEM.m_uiQuantity );

				// 07. 03. 08 - Kim, Joo - Hyun: jaeryeon If an item's ratings will fall m_sInvITEM [1] in place of the information that comes with the item. CUpgradeDlgStateResult set to 0x99 then output the string to the rating downgrade.
				if(m_pRecvPacket->m_gsv_CRAFT_ITEM_REPLY.m_sInvITEM[1].m_ITEM.GetItemNO() != 0)
					Upgrade.SetResult( 0x99 );
				else
					Upgrade.SetResult( m_pRecvPacket->m_gsv_CRAFT_ITEM_REPLY.m_btRESULT  );

				switch( Upgrade.GetType() )
				{
				case CUpgrade::TYPE_SKILL:
					g_pAVATAR->Sub_MP( Upgrade.GetRequireMp() );
					break;
				case CUpgrade::TYPE_NPC:
					g_pAVATAR->Set_MONEY( g_pAVATAR->Get_MONEY() - Upgrade.GetRequireMoney() );
					break;
				default:
					break;
				}
			}
			*/
			break;
		}
		break;		//PY: why is this break here and not above where i added one?
	case	CRAFT_UPGRADE_INVALID_MAT:	//	0x12	// Material items are wrong.
		{
			CUpgrade::GetInstance().SetResult( m_pRecvPacket->m_gsv_CRAFT_ITEM_REPLY.m_btRESULT  );
			break;
		}
	
	case	CRAFT_GIFTBOX_SUCCESS:		// 0x13		// Gift box disassembly
		{
			//PY: fix later
			/*
			if( g_pAVATAR )
			{
				tagITEM		ItemData;
				tagITEM*	pItem = NULL;
				//CIcon*		pIcon = NULL;
				CreateMsgBoxData MsgBoxData;
				MsgBoxData.strMsg = STR_GETTING_GIFTBOX;
				CIconItem* pItemIcon = NULL;

				g_pAVATAR->SetWaitUpdateInventory( true );
				for( int i = 0; i < m_pRecvPacket->m_gsv_CRAFT_ITEM_REPLY.m_btOutCNT; i++ )
				{
					pItem = &m_pRecvPacket->m_gsv_CRAFT_ITEM_REPLY.m_sInvITEM[ i ].m_ITEM;
					if( !pItem->IsEmpty() )
					{
						if( !pItem->IsEnableDupCNT() )
						{
							pItemIcon = new CIconItem(pItem);		
							MsgBoxData.m_Icons.push_back( pItemIcon );
						}
						else
						{
							ItemData = *pItem;
							if( ItemData.m_uiQuantity > g_pAVATAR->m_Inventory.m_ItemLIST[ m_pRecvPacket->m_gsv_CRAFT_ITEM_REPLY.m_sInvITEM[ i ].m_btInvIDX ].m_uiQuantity )
							{
								ItemData.m_uiQuantity -= g_pAVATAR->m_Inventory.m_ItemLIST[ m_pRecvPacket->m_gsv_CRAFT_ITEM_REPLY.m_sInvITEM[ i ].m_btInvIDX ].m_uiQuantity;
								pItemIcon = new CIconItem(&ItemData);
								MsgBoxData.m_Icons.push_back( pItemIcon );
							}
						}
					}

					g_pAVATAR->Set_ITEM( m_pRecvPacket->m_gsv_CRAFT_ITEM_REPLY.m_sInvITEM[ i ].m_btInvIDX,
						m_pRecvPacket->m_gsv_CRAFT_ITEM_REPLY.m_sInvITEM[ i ].m_ITEM );
				}
				g_pAVATAR->SetWaitUpdateInventory( false );
				g_pAVATAR->UpdateInventory();

				g_itMGR.OpenMsgBox2(MsgBoxData);
				SE_SuccessGiftBox( g_pAVATAR->Get_INDEX() );
			}
			*/
			break;
		}
	case  CRAFT_DRILL_SOCKET_SUCCESS	:    //Socket create success
		{
			//fix later
			/*
			g_pAVATAR->SetWaitUpdateInventory( true );
			for( int i = 0; i < m_pRecvPacket->m_gsv_CRAFT_ITEM_REPLY.m_btOutCNT; i++ )
			{
				g_pAVATAR->Set_ITEM( m_pRecvPacket->m_gsv_CRAFT_ITEM_REPLY.m_sInvITEM[ i ].m_btInvIDX,
					m_pRecvPacket->m_gsv_CRAFT_ITEM_REPLY.m_sInvITEM[ i ].m_ITEM );
			}
			g_pAVATAR->SetWaitUpdateInventory( false );
			g_pAVATAR->UpdateInventory();
			CGame::GetInstance().EndMakeSocket(); 
			g_itMGR.OpenMsgBox( STR_MAKESOCKET_SUCCESS );
			*/
		}
		SE_SuccessUpgrade( g_pAVATAR->Get_INDEX() );
		break;
	case  CRAFT_DRILL_CLEAR_USEITME      :    // Only items, we disappeared.
		{
			/*
			g_pAVATAR->SetWaitUpdateInventory( true );
			for( int i = 0; i < m_pRecvPacket->m_gsv_CRAFT_ITEM_REPLY.m_btOutCNT; i++ )
			{
				g_pAVATAR->Set_ITEM( m_pRecvPacket->m_gsv_CRAFT_ITEM_REPLY.m_sInvITEM[ i ].m_btInvIDX,
					m_pRecvPacket->m_gsv_CRAFT_ITEM_REPLY.m_sInvITEM[ i ].m_ITEM );
			}
			g_pAVATAR->SetWaitUpdateInventory( false );
			g_pAVATAR->UpdateInventory();
			CGame::GetInstance().EndMakeSocket();
			g_itMGR.OpenMsgBox( STR_MAKESOCKET_FAILED );
			*/
		}
		SE_FailUpgrade( g_pAVATAR->Get_INDEX() );
		break;
	case CRAFE_DRILL_INVALID_USEITEM   :    //Item is not a drill.
		{
			int k=0;
		}
		break;
	case	CRAFT_DRILL_EXIST_SOCKET	        :	  // The socket is already.
		{
			int k=0;

		}
		break;
	case	CRAFT_DRILL_INVALID_TITEM	    :	  //Items that can not penetrate the socket ...
		{
			int k=0;

		}
		break;
	case CRAFE_NOT_ENOUGH_MONEY:
		{
			g_itMGR.OpenMsgBox( STR_NOT_ENOUGH_MONEY );
			break;
		}
	case CRAFE_NOT_ENOUGH_MP:
		{
			g_itMGR.OpenMsgBox( STR_NOT_ENOUGH_MANA);
			break;
		}
	default:
		break;
	}
}

void CRecvPACKET::Recv_gsv_USED_ITEM_TO_REPAIR()
{
	Recv_gsv_SET_INV_ONLY();

	if( g_pAVATAR )
		g_pAVATAR->UpdateAbility();
}
void CRecvPACKET::Recv_gsv_REPAIRED_FROM_NPC()
{
	Recv_gsv_SET_MONEYnINV();

	if( g_pAVATAR )
		g_pAVATAR->UpdateAbility();
}

void CRecvPACKET::Recv_gsv_SET_MONEY_ONLY()
{
	__int64 i64Diff = m_pRecvPacket->m_gsv_SET_MONEY_ONLY.m_i64Money - g_pAVATAR->Get_MONEY();

	if( i64Diff > 0 )
		g_itMGR.AppendChatMsg( CStr::Printf(F_STR_SET_MONEY_INC, i64Diff) ,IT_MGR::CHAT_TYPE_SYSTEM );
	else
		g_itMGR.AppendChatMsg( CStr::Printf(F_STR_SET_MONEY_DEC, -i64Diff),IT_MGR::CHAT_TYPE_SYSTEM );

	if( g_pAVATAR )
		g_pAVATAR->Set_MONEY( m_pRecvPacket->m_gsv_SET_MONEY_ONLY.m_i64Money );
}

void CRecvPACKET::Recv_gsv_REWARD_MONEY()
{
	__int64 i64Diff = m_pRecvPacket->m_gsv_SET_MONEY_ONLY.m_i64Money - g_pAVATAR->Get_MONEY();

	if( i64Diff > 0 )
		g_itMGR.AppendChatMsg( CStr::Printf(F_STR_REWARD_MONEY_INC, i64Diff) , IT_MGR::CHAT_TYPE_QUESTREWARD );
	else
		g_itMGR.AppendChatMsg( CStr::Printf(F_STR_REWARD_MONEY_DEC, -i64Diff), IT_MGR::CHAT_TYPE_QUESTREWARD );

	if( g_pAVATAR )
		g_pAVATAR->Set_MONEY( m_pRecvPacket->m_gsv_SET_MONEY_ONLY.m_i64Money );
}


//----------------------------------------------------------------------------------------------------	
///
/// @brief Quest reward-related
///
//----------------------------------------------------------------------------------------------------
void CRecvPACKET::Recv_gsv_REWARD_ITEM()
{
	if( !g_pAVATAR )
		return;

	for ( int i = 0; i < m_pRecvPacket->m_gsv_SET_INV_ONLY.m_btItemCNT ; i++ ) 
	{
		char *szMsg = NULL;

		tagITEM *pInvITEM = NULL;
		tagITEM *pSrvITEM = NULL;

		pSrvITEM = &m_pRecvPacket->m_gsv_SET_INV_ONLY.m_sInvITEM[i].m_ITEM;
		if( !pSrvITEM->IsEmpty() )
		{
			if ( pSrvITEM->IsEnableDupCNT() ) 
			{
				pInvITEM = &g_pAVATAR->m_Inventory.m_ItemLIST[ m_pRecvPacket->m_gsv_SET_INV_ONLY.m_sInvITEM[i].m_btInvIDX ];
				szMsg = CStr::Printf(F_STR_REWARD_ITEMS, ITEM_NAME( pSrvITEM->GetTYPE(), pSrvITEM->GetItemNO() ), pSrvITEM->GetQuantity() - pInvITEM->GetQuantity() );
			} 
			else 
			{
				szMsg = CStr::Printf(F_STR_REWARD_ITEM, ITEM_NAME( pSrvITEM->GetTYPE(), pSrvITEM->GetItemNO() ) );
			}

			g_itMGR.AppendChatMsg( szMsg, IT_MGR::CHAT_TYPE_QUESTREWARD );
		}
	}
}

void CRecvPACKET::Recv_gsv_REWARD_ADD_ABILITY()
{
	g_QuestRewardQueue.PushAddAbility( m_pRecvPacket->m_gsv_SET_ABILITY );
}

void CRecvPACKET::Recv_gsv_REWARD_SET_ABILITY()
{
	g_QuestRewardQueue.PushSetAbility( m_pRecvPacket->m_gsv_SET_ABILITY );
}

void CRecvPACKET::Recv_gsv_PARTY_ITEM()
{
	_RPT1( _CRT_WARN,"Recv_gsv_PARTY_ITEM_1%d\n",g_GameDATA.GetGameTime() );
	CParty& refParty = CParty::GetInstance();
	assert( refParty.HasParty() );
	if( refParty.HasParty() )
	{
		PartyMember Member;
		if( refParty.GetMemberInfoByObjSvrIdx( m_pRecvPacket->m_gsv_PARTY_ITEM.m_wObjectIDX , Member ) )
		{						
			g_itMGR.AppendChatMsg( 				
				CStr::Printf( m_pRecvPacket->m_gsv_PARTY_ITEM.m_ITEM.GettingMESSAGE_Party( Member.m_strName.c_str() ) ),
				IT_MGR::CHAT_TYPE_SYSTEM );			
		}		
	}
	_RPT1( _CRT_WARN,"Recv_gsv_PARTY_ITEM_2%d\n",g_GameDATA.GetGameTime() );
}

void CRecvPACKET::Recv_gsv_PARTY_RULE()
{
	_RPT1( _CRT_WARN,"Recv_gsv_PARTY_RULE_1(%d)\n",g_GameDATA.GetGameTime() );
	CParty::GetInstance().RecvPartyRule( m_pRecvPacket->m_gsv_PARTY_RULE.m_btPartyRULE );
	_RPT1( _CRT_WARN,"Recv_gsv_PARTY_RULE_2(%d)\n",g_GameDATA.GetGameTime() );
}

void CRecvPACKET::Recv_wsv_CHAR_CHANGE()
{
	CGame::GetInstance().ChangeState( CGame::GS_EXITMAIN );
}

void CRecvPACKET::Recv_gsv_LOGOUT_REPLY()
{
	g_itMGR.SetWaitDisconnectTime( m_pRecvPacket->m_gsv_LOGOUT_REPLY.m_wWaitSec );
	g_itMGR.ChangeState( IT_MGR::STATE_WAITDISCONNECT );
}

void CRecvPACKET::Recv_wsv_DELETE_CHAR ()
{
	short nOffset = (short)sizeof( wsv_DELETE_CHAR);
	char *szName = Packet_GetStringPtr( m_pRecvPacket, nOffset );

	CJustModelAVT* pAVT = CGameDataCreateAvatar::GetInstance().FindAvatar( szName );
	assert( pAVT );
	if( pAVT )
	{
		if( m_pRecvPacket->m_wsv_DELETE_CHAR.m_dwDelRemainTIME == 0xFFFFFFFF )
		{
			g_EUILobby.ShowMsgBox( STR_CLAN_CANT_DELETE_CLANMASTER, CTMsgBox::BT_OK ,true , EUI_SELECT_AVATA );
		}
		else
		{
			pAVT->SetDeleteRemainSEC( m_pRecvPacket->m_wsv_DELETE_CHAR.m_dwDelRemainTIME );
			if( m_pRecvPacket->m_wsv_DELETE_CHAR.m_dwDelRemainTIME )
			{
				pAVT->SwitchState( CJustModelAVT::STATE_SITDOWN );

				pAVT->m_iCurrentMotionState = 5;
				pAVT->SetMotion( pAVT->m_iMotionIDX[pAVT->m_iCurrentMotionState], 0);
			}
			else
			{
				pAVT->SwitchState( CJustModelAVT::STATE_STANDUP );
				pAVT->m_iCurrentMotionState = 3;
				pAVT->SetMotion( pAVT->m_iMotionIDX[pAVT->m_iCurrentMotionState], 0);
			}



		}
	}
}

//----------------------------------------------------------------------------------------------------	
///
/// @brief Events related to object processing
///
//----------------------------------------------------------------------------------------------------	
void CRecvPACKET::Recv_gsv_ADD_EVENTOBJ()
{
	/*struct gsv_ADD_EVENTOBJ : public t_PACKETHEADER {
	WORD	m_wObjectIDX;
	char	m_cMapX;
	char	m_cMapY;
	WORD	m_wMapEventID;
	short	m_nEventSTATUS;
	} ;*/

	CObjFixedEvent* pObj = g_pObjMGR->GetEventObject( m_pRecvPacket->m_gsv_ADD_EVENTOBJ.m_wMapEventID );
	if( pObj )
	{		
		g_pObjMGR->Set_ServerObjectIndex( pObj->Get_INDEX(), m_pRecvPacket->m_gsv_ADD_EVENTOBJ.m_wObjectIDX );

		pObj->ExecEventScript( m_pRecvPacket->m_gsv_ADD_EVENTOBJ.m_nEventSTATUS, true, false, true );
	}else
	{
		assert( 0 && "Recv invalid event object" );
	}


}

void CRecvPACKET::Recv_gsv_APPRAISAL_REPLY()
{
	if( m_pRecvPacket->m_gsv_APPRAISAL_REPLY.m_btResult == RESULT_APPRAISAL_REPLY_OK )
	{
		if( g_pAVATAR )
		{
			g_pAVATAR->m_Inventory.m_ItemLIST[ m_pRecvPacket->m_gsv_APPRAISAL_REPLY.m_wInventoryIndex ].m_bIsAppraisal = 1;
			g_pAVATAR->Set_MONEY( g_pAVATAR->Get_MONEY() - CGame::GetInstance().GetAppraisalCost() );


		}
	}
	else
	{
		g_itMGR.OpenMsgBox(STR_FAIL_IDENTIFY );
	}
	CGame::GetInstance().EndAppraisal();
}


//----------------------------------------------------------------------------------------------------	
///
/// @brief HP, MP Potion end packet.
///			The contents of this packet updates the current state, which was also previously the damage is cumulative and must be exhausted.
//----------------------------------------------------------------------------------------------------	
void CRecvPACKET::Recv_gsv_SET_HPnMP()
{
	short iTarget = m_pRecvPacket->m_gsv_SET_HPnMP.m_wObjectIDX;
	CObjCHAR *pChar = g_pObjMGR->Get_ClientCharOBJ( iTarget, true );

	if( pChar && g_pAVATAR )
	{
		short client_obj_index = g_pObjMGR->Get_ClientObjectIndex ( m_pRecvPacket->m_gsv_SET_HPnMP.m_wObjectIDX );

		if( m_pRecvPacket->m_gsv_SET_HPnMP.m_nHP >= 0 )
		{
			/// Clear all the damage.
			pChar->ClearAllDamage();
				///Only when their own Revise HP (slow correction) applies.
				if( client_obj_index == g_pAVATAR->Get_INDEX() )
					pChar->SetReviseHP( m_pRecvPacket->m_gsv_SET_HPnMP.m_nHP - pChar->Get_HP() );
				else
					pChar->Set_HP( m_pRecvPacket->m_gsv_SET_HPnMP.m_nHP );
		}

		if( m_pRecvPacket->m_gsv_SET_HPnMP.m_nMP >= 0 )
		{
			//06. 02. 09 - Kim, Joo - Hyun: Potion MP compensation on termination if not getting washed over.
			
				///Only when their own Revise MP (slow correction) applies.
				if( client_obj_index == g_pAVATAR->Get_INDEX() )
					pChar->SetReviseMP( m_pRecvPacket->m_gsv_SET_HPnMP.m_nMP - pChar->Get_MP() );
				else
					pChar->Set_MP( m_pRecvPacket->m_gsv_SET_HPnMP.m_nMP );
			
		}
	}	
}
//----------------------------------------------------------------------------------------------------	
///
/// HP and MP sent from Server every second. (PY: soon will receive other stats too)
///			
//----------------------------------------------------------------------------------------------------	
void CRecvPACKET::Recv_gsv_CHAR_HPMP_INFO()
{ 
#ifdef _NoRecover
	if( g_pAVATAR )
	{
		int NewHP = m_pRecvPacket->m_gsv_CHAR_HPMP_INFO.m_CurHP;
		int NewMaxHP = m_pRecvPacket->m_gsv_CHAR_HPMP_INFO.m_MaxHP;
		if(m_pRecvPacket->m_gsv_CHAR_HPMP_INFO.m_CurHP > 0)
		{
			if(m_pRecvPacket->m_gsv_CHAR_HPMP_INFO.m_CurHP >= g_pAVATAR->Get_MaxHP())
				g_pAVATAR->Set_HP(g_pAVATAR->Get_MaxHP());
			else 
			{
				//g_pAVATAR->SetReviseHP( m_pRecvPacket->m_gsv_CHAR_HPMP_INFO.m_CurHP - g_pAVATAR->Get_HP());
				g_pAVATAR->Set_HP(m_pRecvPacket->m_gsv_CHAR_HPMP_INFO.m_CurHP);		//PY modification
			}
		}
		if(m_pRecvPacket->m_gsv_CHAR_HPMP_INFO.m_CurMP > 0)
		{
			if(m_pRecvPacket->m_gsv_CHAR_HPMP_INFO.m_CurMP >= g_pAVATAR->Get_MaxMP())
				g_pAVATAR->Set_MP(g_pAVATAR->Get_MaxMP());
			else 
			{
				//g_pAVATAR->SetReviseMP( m_pRecvPacket->m_gsv_CHAR_HPMP_INFO.m_CurMP - g_pAVATAR->Get_MP());
				g_pAVATAR->Set_MP( m_pRecvPacket->m_gsv_CHAR_HPMP_INFO.m_CurMP);	//PY modification. Just set the MP to the value in the packet and be done with it. leave recovery rates to the server
			}
		}
		//PY: adding MaxHP and MaxMP to see if it reads them from the packet
		if(m_pRecvPacket->m_gsv_CHAR_HPMP_INFO.m_MaxHP > 0)
		{
			g_pAVATAR->m_Battle.m_nMaxHP = m_pRecvPacket->m_gsv_CHAR_HPMP_INFO.m_MaxHP;				//PY: trying a new approache here. Yup that did it
			g_pAVATAR->Set_MaxHP(m_pRecvPacket->m_gsv_CHAR_HPMP_INFO.m_MaxHP);
		}
		if(m_pRecvPacket->m_gsv_CHAR_HPMP_INFO.m_MaxMP > 0)
		{
			g_pAVATAR->m_Battle.m_nMaxMP = m_pRecvPacket->m_gsv_CHAR_HPMP_INFO.m_MaxMP;
			g_pAVATAR->Set_MaxMP(m_pRecvPacket->m_gsv_CHAR_HPMP_INFO.m_MaxMP);
		}
		//ClientLog(LOG_DEBUG,"RecvPACKET 0x07ec. HP: %i, MaxHP %i",NewHP, NewMaxHP);
	}
#endif
}

//PY: Adding a new packet structure to receive stat information beyond HP and MP	0x07ed
void CRecvPACKET::Recv_gsv_CHAR_STAT_INFO()
{
	if( g_pAVATAR )
	{
		g_pAVATAR->m_Battle.m_nATT = m_pRecvPacket->m_gsv_CHAR_STAT_INFO.m_CurAP;
		g_pAVATAR->m_Battle.m_nDEF = m_pRecvPacket->m_gsv_CHAR_STAT_INFO.m_CurDef;
		g_pAVATAR->m_Battle.m_nHIT = m_pRecvPacket->m_gsv_CHAR_STAT_INFO.m_CurAccuracy;
		g_pAVATAR->m_Battle.m_nAVOID = m_pRecvPacket->m_gsv_CHAR_STAT_INFO.m_CurDodge;
		g_pAVATAR->m_Battle.m_nRES = m_pRecvPacket->m_gsv_CHAR_STAT_INFO.m_CurMDef;
		g_pAVATAR->m_Battle.m_iCritical = m_pRecvPacket->m_gsv_CHAR_STAT_INFO.m_CurCrit;
		g_pAVATAR->m_Battle.m_nMaxWEIGHT = m_pRecvPacket->m_gsv_CHAR_STAT_INFO.m_CurMaxWeight;

		//To Do Add move speed modification here
		//Possibly add things such as attack speed also. not sure if it's needed yet
	}
}


//----------------------------------------------------------------------------------------------------	
///
/// @brief Do_DeadEvent Was called in to handle the request from the server change.
///
//----------------------------------------------------------------------------------------------------	
void CRecvPACKET::Recv_gsv_CHECK_NPC_EVENT()
{	
	//LOGOUT( " A tree standing from the server by running the command[ %s ] ", NPC_DESC( m_pRecvPacket->m_gsv_CHECK_NPC_EVENT.m_nNpcIDX ) );
	ClientLog( LOG_NORMAL, "Recieved a quest trigger command %s \n", NPC_DESC( m_pRecvPacket->m_gsv_CHECK_NPC_EVENT.m_nNpcIDX ));
	QF_doQuestTrigger( NPC_DESC( m_pRecvPacket->m_gsv_CHECK_NPC_EVENT.m_nNpcIDX ) );
}

void CRecvPACKET::Recv_wsv_CLAN_COMMAND()
{
	switch( m_pRecvPacket->m_wsv_CLAN_COMMAND.m_btRESULT )
	{
	case RESULT_CLAN_MEMBER_JOBnLEV:
		{
			short nOffset = sizeof( wsv_CLAN_MEMBER_JOBnLEV );
			if( char* pszName = (char*)Packet_GetStringPtr( m_pRecvPacket, nOffset ) )
			{
				CClan::GetInstance().SetMemberJob( pszName, m_pRecvPacket->m_wsv_CLAN_MEMBER_JOBnLEV.m_nJOB );
				CClan::GetInstance().SetMemberLevel( pszName, m_pRecvPacket->m_wsv_CLAN_MEMBER_JOBnLEV.m_nLEVEL );
			}
		}
		break;
	case RESULT_CLAN_CREATE_NO_CONDITION:
		g_itMGR.OpenMsgBox( STR_CLAN_RESULT_CLAN_CREATE_NO_CONDITION );
		break;
	case RESULT_CLAN_CREATE_OK:/// A user comes to the creation.
		{			
			g_pAVATAR->Set_MONEY( g_pAVATAR->Get_MONEY() - 1000000 );
		
			g_itMGR.CloseDialog( DLG_TYPE_CLAN_ORGANIZE );
			g_itMGR.OpenMsgBox(STR_CLAN_ORGANIZED );
			short nOffset = sizeof( wsv_CLAN_COMMAND );

			if( tag_MY_CLAN* pInfo = (tag_MY_CLAN*)Packet_GetDataPtr( m_pRecvPacket, nOffset, sizeof(tag_MY_CLAN ) ) )
			{
				if( char* pszName = (char*)Packet_GetStringPtr( m_pRecvPacket, nOffset ) )
				{
					CClan& Clan = CClan::GetInstance();

					Clan.SetName( pszName );
					Clan.SetClanNo( pInfo->m_dwClanID );
					Clan.SetMarkBack( pInfo->m_wClanMARK[0] );
					Clan.SetMarkCenter( pInfo->m_wClanMARK[1] );
					Clan.SetPoint( pInfo->m_iClanSCORE );
					Clan.SetClass( pInfo->m_btClanPOS );
					Clan.SetMoney( pInfo->m_biClanMONEY );
					Clan.SetStoreRate( pInfo->m_iClanRATE );
					Clan.SetLevel( pInfo->m_btClanLEVEL );
					Clan.SetSkill( pInfo->m_ClanBIN.m_SKILL, MAX_CLAN_SKILL_SLOT );

#ifdef __EDIT_CLANDLG_CLANWAR
					Clan.SetRewardPoint(pInfo->m_iRewardPoint);
#else 
					Clan.SetRewardPoint(0);
#endif
#ifdef __CLAN_INFO_ADD
					Clan.SetRankPoint(pInfo->m_iRankPoint);
#endif
					if( char* pszSlogan = (char*)Packet_GetStringPtr( m_pRecvPacket, nOffset ) )
						Clan.SetSlogan( pszSlogan );

					Clan.AddMember( g_pAVATAR->Get_NAME(), CClan::CLAN_MASTER , pInfo->m_iClanCONT, CGame::GetInstance().GetJoinChannelNo() , g_pAVATAR->Get_LEVEL(), g_pAVATAR->Get_JOB() );

					g_pAVATAR->SetClan( pInfo->m_dwClanID, pInfo->m_wClanMARK[0], pInfo->m_wClanMARK[1], pszName , 1, CClan::CLAN_MASTER );

					Clan.SetMemberJob( g_pAVATAR->Get_NAME(), g_pAVATAR->Get_JOB() );
					Clan.SetMemberLevel( g_pAVATAR->Get_NAME(), g_pAVATAR->Get_LEVEL() );

					g_pNet->Send_cli_CLAN_MEMBER_JOBnLEV(  g_pAVATAR->Get_LEVEL() , g_pAVATAR->Get_JOB());
				}
			}
		}
		break;
	case RESULT_CLAN_JOIN_OK:///The user only new member comes in a clan.
		{
			g_itMGR.OpenMsgBox(STR_CLAN_RESULT_CLAN_JOIN_OK);
			short nOffset = sizeof( wsv_CLAN_COMMAND );
			if( tag_MY_CLAN* pInfo = (tag_MY_CLAN*)Packet_GetDataPtr( m_pRecvPacket, nOffset, sizeof(tag_MY_CLAN ) ) )
			{
				if( char* pszName = (char*)Packet_GetStringPtr( m_pRecvPacket, nOffset ) )
				{
					CClan& Clan = CClan::GetInstance();

					Clan.SetName( pszName );
					Clan.SetClanNo( pInfo->m_dwClanID );
					Clan.SetMarkBack( pInfo->m_wClanMARK[0] );
					Clan.SetMarkCenter( pInfo->m_wClanMARK[1] );
					Clan.SetPoint( pInfo->m_iClanSCORE );
					Clan.SetClass( pInfo->m_btClanPOS );
					Clan.SetMoney( pInfo->m_biClanMONEY );
					Clan.SetStoreRate( pInfo->m_iClanRATE );
					Clan.SetLevel( pInfo->m_btClanLEVEL );
					Clan.SetSkill( pInfo->m_ClanBIN.m_SKILL, MAX_CLAN_SKILL_SLOT );
#ifdef __EDIT_CLANDLG_CLANWAR
					Clan.SetRewardPoint(pInfo->m_iRewardPoint);
#else 
					Clan.SetRewardPoint(0);
#endif

#ifdef __CLAN_INFO_ADD
					Clan.SetRankPoint(pInfo->m_iRankPoint);
#endif
					if( char* pszSlogan = (char*)Packet_GetStringPtr( m_pRecvPacket, nOffset ) )
						Clan.SetSlogan( pszSlogan );

					if( char* pszNotice = (char*)Packet_GetStringPtr( m_pRecvPacket, nOffset ) )
						Clan.SetNotice( pszNotice );

					g_pAVATAR->SetClan( pInfo->m_dwClanID, pInfo->m_wClanMARK[0], pInfo->m_wClanMARK[1], pszName , pInfo->m_btClanLEVEL, CClan::CLAN_JUNIOR );

					Clan.RequestMemberList();
					//Clan.RequestClanInfo();

					g_pNet->Send_cli_CLAN_MEMBER_JOBnLEV(  g_pAVATAR->Get_LEVEL() , g_pAVATAR->Get_JOB());
				}
			}
		}
		break;
	case RESULT_CLAN_DESTROY_OK://Not used for packet
		g_itMGR.OpenMsgBox(STR_CLAN_RESULT_CLAN_DESTROY_OK);
		break;
	case RESULT_CLAN_CREATE_FAILED://		
		g_itMGR.OpenMsgBox(STR_CLAN_RESULT_CLAN_CREATE_FAILED );
		break;
	case RESULT_CLAN_CREATE_DUP_NAME://		
		g_itMGR.OpenMsgBox(STR_CLAN_RESULT_CLAN_CREATE_DUP_NAME);
		break;
	case RESULT_CLAN_CREATE_NO_RIGHT://			
		g_itMGR.CloseDialog( DLG_TYPE_CLAN_ORGANIZE );
		g_itMGR.OpenMsgBox(STR_CLAN_RESULT_CLAN_CREATE_NO_RIGHT);
		break;
	case RESULT_CLAN_DESTROYED://			0x051		// Clancy was ppogae.
		g_itMGR.OpenMsgBox(STR_CLAN_RESULT_CLAN_DESTROYED);
		CClan::GetInstance().Clear();
		if( g_pAVATAR )
			g_pAVATAR->ResetClan();
		break;
	case RESULT_CLAN_DESTROY_FAILED://		
		g_itMGR.OpenMsgBox(STR_CLAN_RESULT_CLAN_DESTROY_FAILED);
		break;
	case RESULT_CLAN_DESTROY_NO_RIGHT://			// Ppogael not authorized.
		g_itMGR.OpenMsgBox(STR_CLAN_RESULT_CLAN_DESTROY_NO_RIGHT);
		break;
	case RESULT_CLAN_JOIN_MEMBER:/// When a new member comes to the Clan.
		{
			/*strujct {
			char *m_szMember;	New subscribers
			char *m_szMaster;	Join Now let
			*/
			short nOffset = sizeof( wsv_CLAN_COMMAND );
			char* pszMember = (char*)Packet_GetStringPtr( m_pRecvPacket, nOffset );
			char* pszMaster = (char*)Packet_GetStringPtr( m_pRecvPacket, nOffset );
			assert( pszMember && pszMaster );
			if( pszMember && pszMaster )
			{
				if( _strcmpi( g_pAVATAR->Get_NAME(), pszMember ) )///I showed him not only when.
				{
					g_itMGR.OpenMsgBox( CStr::Printf(STR_CLAN_RESULT_CLAN_JOIN, pszMember, pszMaster));

					CObjCHAR* pChar = g_pObjMGR->Get_ClientCharOBJByName( pszMember );
					if( pChar && pChar->Is_AVATAR() )
					{
						CObjAVT* pAvt = (CObjAVT*)pChar;				
						pAvt->SetClan( g_pAVATAR->GetClanID(),
							g_pAVATAR->GetClanMarkBack(),
							g_pAVATAR->GetClanMarkCenter(),
							g_pAVATAR->GetClanName() ,
							g_pAVATAR->GetClanLevel() , CClan::CLAN_JUNIOR );
					}
					CClan::GetInstance().AddMember( pszMember, CClan::CLAN_JUNIOR , 0, 0 ,0, 0);
				}
			}
		}
		break;
	case RESULT_CLAN_JOIN_FAILED:			
		g_itMGR.OpenMsgBox(STR_CLAN_RESULT_CLAN_JOIN_FAILED);
		break;
	case RESULT_CLAN_JOIN_NO_RIGHT://				// Not join the authority.
		g_itMGR.OpenMsgBox(STR_CLAN_RESULT_CLAN_JOIN_NO_RIGHT);
		break;
	case RESULT_CLAN_JOIN_HAS_CLAN://				// I have joined another guild.
		g_itMGR.OpenMsgBox(STR_CLAN_RESULT_CLAN_JOIN_HAS_CLAN);
		break;
	case RESULT_CLAN_MY_DATA:///Clan information when logging in for the first time
		{
			short nOffset = sizeof( wsv_CLAN_COMMAND );
			if( tag_MY_CLAN* pInfo = (tag_MY_CLAN*)Packet_GetDataPtr( m_pRecvPacket, nOffset, sizeof(tag_MY_CLAN ) ) )
			{
				if( char* pszName = (char*)Packet_GetStringPtr( m_pRecvPacket, nOffset ) )
				{
					CClan& Clan = CClan::GetInstance();
					Clan.SetName( pszName );
					Clan.SetClanNo( pInfo->m_dwClanID );
					Clan.SetMarkBack( pInfo->m_wClanMARK[0] );
					Clan.SetMarkCenter( pInfo->m_wClanMARK[1] );
					Clan.SetPoint( pInfo->m_iClanSCORE );
					Clan.SetClass( pInfo->m_btClanPOS );
					Clan.SetLevel( pInfo->m_btClanLEVEL );
					Clan.SetMoney( pInfo->m_biClanMONEY );
					Clan.SetStoreRate( pInfo->m_iClanRATE );
					Clan.SetSkill( pInfo->m_ClanBIN.m_SKILL, MAX_CLAN_SKILL_SLOT );
#ifdef __EDIT_CLANDLG_CLANWAR
					Clan.SetRewardPoint(pInfo->m_iRewardPoint);
#else 
					Clan.SetRewardPoint(0);
#endif

#ifdef __CLAN_INFO_ADD
					Clan.SetRankPoint(pInfo->m_iRankPoint);
#endif
					if( char* pszSlogan = (char*)Packet_GetStringPtr( m_pRecvPacket, nOffset ) )
						Clan.SetSlogan( pszSlogan );

					if( char* pszNotice = (char*)Packet_GetStringPtr( m_pRecvPacket, nOffset ) )
						Clan.SetNotice( pszNotice );

					if( g_pAVATAR )///After the avatar ever made??'re done.
					{
						g_pAVATAR->SetClan( pInfo->m_dwClanID, pInfo->m_wClanMARK[0], pInfo->m_wClanMARK[1], pszName , pInfo->m_btClanLEVEL , pInfo->m_btClanPOS );
					}

				}
			}
		}
		break;
	case RESULT_CLAN_INFO://				0x031		// Guild information / / / 2004/11/23 deleted
		{
			short nOffset = sizeof( wsv_CLAN_COMMAND );
			tag_MY_CLAN* pInfo = (tag_MY_CLAN*)Packet_GetDataPtr( m_pRecvPacket, nOffset, sizeof(tag_MY_CLAN) );

			assert( pInfo );
			if( pInfo )
			{
				CClan& Clan = CClan::GetInstance();

				if( Clan.GetLevel() < pInfo->m_btClanLEVEL )
					g_itMGR.OpenMsgBox( STR_CLAN_LEVELUP,CMsgBox::BT_OK, false );

				Clan.SetClanNo( pInfo->m_dwClanID );
				Clan.SetMarkBack( pInfo->m_wClanMARK[0] );
				Clan.SetMarkCenter( pInfo->m_wClanMARK[1] );
				Clan.SetPoint( pInfo->m_iClanSCORE );
				//Clan.SetClass( pInfo->m_btClanPOS );
				Clan.SetLevel( pInfo->m_btClanLEVEL );
				Clan.SetMoney( pInfo->m_biClanMONEY );
				Clan.SetStoreRate( pInfo->m_iClanRATE );
				Clan.SetSkill( pInfo->m_ClanBIN.m_SKILL, MAX_CLAN_SKILL_SLOT );

#ifdef __EDIT_CLANDLG_CLANWAR
				Clan.SetRewardPoint(pInfo->m_iRewardPoint);
#else 
				Clan.SetRewardPoint(0);
#endif

#ifdef __CLAN_INFO_ADD
				Clan.SetRankPoint(pInfo->m_iRankPoint);
#endif
			}
		}
		break;
	case RESULT_CLAN_ROSTER://				0x032		// Guild List
		{
			CClan& Clan = CClan::GetInstance();
			Clan.ClearMemberlist();

			short nOffset = sizeof( wsv_CLAN_COMMAND );
			tag_CLAN_MEMBER* pMember = NULL;
			char*  pszCharName	= NULL;


			//CStringManager& StringMgr = CStringManager::GetSingleton();
			while( pMember = (tag_CLAN_MEMBER*)Packet_GetDataPtr( m_pRecvPacket, nOffset, sizeof( tag_CLAN_MEMBER ) ) )
			{
				if( pszCharName = (char*)Packet_GetStringPtr( m_pRecvPacket, nOffset ) )
					Clan.AddMember( pszCharName, pMember->m_btClanPOS, pMember->m_iClanCONTRIBUTE, pMember->m_btChannelNO , pMember->m_nLEVEL ,pMember->m_nJOB);

			}


		}
		break;
	case RESULT_CLAN_KICK://				0x081		// Has expelled from the clan
		{
			//char *m_szMember;	Taltoeja
			//char *m_szMaster;	Now let Withdrawal

			short nOffset = sizeof( wsv_CLAN_COMMAND );
			char* pszMember = (char*)Packet_GetStringPtr( m_pRecvPacket, nOffset );
			char* pszMaster = (char*)Packet_GetStringPtr( m_pRecvPacket, nOffset );

			assert( pszMember && pszMaster );
			if( pszMember && pszMaster )
			{
				if( _strcmpi( pszMember, g_pAVATAR->Get_NAME() ) )
				{
					g_itMGR.OpenMsgBox( CStr::Printf(STR_CLAN_RESULT_CLAN_KICK, pszMember, pszMaster ) );
					CClan::GetInstance().RemoveMember( pszMember );
				}
				else
				{
					g_itMGR.OpenMsgBox( CStr::Printf(STR_CLAN_RESULT_CLAN_KICK_ME, pszMaster ));
					CClan::GetInstance().Clear();
					if( g_pAVATAR )
						g_pAVATAR->ResetClan();
				}
			}
		}
		break;
	case RESULT_CLAN_QUIT://				0x082		// Deleted from the clan member (Clan variable initialization)
		{
			short nOffset = sizeof( wsv_CLAN_COMMAND );
			char* pszMember = (char*)Packet_GetStringPtr( m_pRecvPacket, nOffset );
			assert( pszMember );
			if( pszMember )
			{
				if( _strcmpi( pszMember, g_pAVATAR->Get_NAME() )== 0)			
				{
					CClan::GetInstance().Clear();
					if( g_pAVATAR )
						g_pAVATAR->ResetClan();
				}
				else
				{
					CClan::GetInstance().RemoveMember( pszMember );
					g_itMGR.OpenMsgBox( CStr::Printf(STR_CLAN_RESULT_CLAN_QUIT,  pszMember ) );
				}
			}
		}
		break;
	case GCMD_INVITE_REQ://I was asked to join the clan. <Requestor name>
		{
			short nOffset = sizeof( wsv_CLAN_COMMAND );
			char* pszMaster = (char*)Packet_GetStringPtr( m_pRecvPacket, nOffset );
			assert( pszMaster );
			if( pszMaster )
			{

//Honggeun: Japan Billing modification
#ifdef __JAPAN_BILL
				if( !(CGame::GetInstance().GetPayType() & PLAY_FLAG_BATTLE) )
				{
					g_itMGR.OpenMsgBox( STR_JP_BILL_CANT_JOIN_CLAN );

					CTCmdRejectReqJoinClan pCmdNo( pszMaster );
					pCmdNo.Exec(NULL);

					return;
				}        
#endif

				//MSGTYPE_RECV_CLANJOIN_REQ
				if( g_itMGR.FindMsgBox( CMsgBox::MSGTYPE_RECV_CLANJOIN_REQ ) == NULL )
				{
					CTCommand* pCmdYes  = new CTCmdAcceptReqJoinClan( pszMaster );
					CTCommand* pCmdNo	= new CTCmdRejectReqJoinClan( pszMaster );
					g_itMGR.OpenMsgBox(
						CStr::Printf( STR_CLAN_GCMD_INVITE_REQ, pszMaster ),
						CMsgBox::BT_OK | CMsgBox::BT_CANCEL, false, 0,
						pCmdYes, pCmdNo ,CMsgBox::MSGTYPE_RECV_CLANJOIN_REQ);
				}
			}
		}
		break;
		//#define	GCMD_INVITE_REPLY_YES	0x00c		//Join Clan approval <requestor name>
	case GCMD_INVITE_REPLY_NO://	0x00d		//Join a clan refused <requestor name>
		{
			short nOffset = sizeof( wsv_CLAN_COMMAND );
			char* pszTargetName = (char*)Packet_GetStringPtr( m_pRecvPacket, nOffset );

			g_itMGR.OpenMsgBox( CStr::Printf(STR_CLAN_GCMD_INVITE_REPLY_NO, pszTargetName ) );
		}
		break;
	case RESULT_CLAN_MOTD:
		{
			short nOffset = sizeof( wsv_CLAN_COMMAND );
			CClan::GetInstance().SetNotice( (const char*)Packet_GetStringPtr( m_pRecvPacket, nOffset ) );
		}
		break;
	case RESULT_CLAN_SLOGAN:
		{
			short nOffset = sizeof( wsv_CLAN_COMMAND );
			CClan::GetInstance().SetSlogan( (const char*)Packet_GetStringPtr( m_pRecvPacket, nOffset ) );
		}
		break;
	case RESULT_CLAN_POSITION://			0x083		// The adjustment of the clan when my class
		{
			short nOffset = sizeof( wsv_CLAN_COMMAND );
			char* pszMaster = (char*)Packet_GetStringPtr( m_pRecvPacket, nOffset );
			BYTE* pbtClass  = (BYTE*)Packet_GetDataPtr( m_pRecvPacket, nOffset, sizeof( BYTE ) );
			CClan::GetInstance().SetClass( *pbtClass );
			if( g_pAVATAR && g_pAVATAR->GetClanMarkBack() == 0 && *pbtClass > CClan::CLAN_MASTER )
				g_pNet->Send_cli_CLANMARK_REG_TIME();


			if( pszMaster )
			{
				std::string strMsg = CStr::Printf(STR_CLAN_RESULT_CLAN_POSITION,CStringManager::GetSingleton().GetClanClass( *pbtClass ) );
				strMsg.append(":");
				strMsg.append( pszMaster );

				g_itMGR.OpenMsgBox( strMsg.c_str() ); 
			}
		}
		break;
	case RESULT_CLAN_SET:
		{
			short nOffset = sizeof( wsv_CLAN_COMMAND );
			WORD* pwObjIDX			= (WORD*)Packet_GetDataPtr( m_pRecvPacket, nOffset, sizeof( WORD ));
			tag_CLAN_ID* pClanID	= (tag_CLAN_ID*)Packet_GetDataPtr( m_pRecvPacket, nOffset, sizeof( tag_CLAN_ID) );
			assert( pwObjIDX && pClanID );
			if( pwObjIDX && pClanID )
			{
				CObjAVT* pAvt = g_pObjMGR->Get_ClientCharAVT ( *pwObjIDX, false );
				if( pAvt )
				{
					if( pClanID->m_dwClanID == 0 )
					{
						pAvt->ResetClan();		
					}
					else
					{
						if( char* pszClanName = (char*)Packet_GetStringPtr( m_pRecvPacket, nOffset ) )
						{
							if( pClanID->m_wClanMARK[0] == 0 && pAvt->GetClanMarkCenter() != pClanID->m_wClanMARK[1] )
								g_pNet->Send_cli_CLANMARK_REQ( pClanID->m_dwClanID );

							pAvt->SetClan( pClanID->m_dwClanID, pClanID->m_wClanMARK[0], pClanID->m_wClanMARK[1], pszClanName , pClanID->m_btClanLEVEL , pClanID->m_btClanPOS );
						}
					}
				}
			}
		}
		break;
	case RESULT_CLAN_MEMBER_LOGIN:// Sent to all of the members login
	case RESULT_CLAN_MEMBER_LOGOUT:// Sent to all members log out
		{
			short nOffset = sizeof( wsv_CLAN_COMMAND );
			if( tag_CLAN_MEMBER* pMember = (tag_CLAN_MEMBER*)Packet_GetDataPtr( m_pRecvPacket, nOffset, sizeof( tag_CLAN_MEMBER ) ) )
			{
				if( char* pszCharName = (char*)Packet_GetStringPtr( m_pRecvPacket, nOffset ) )
					CClan::GetInstance().ChangeConnectStatus( pszCharName, pMember->m_btChannelNO );
			}
		}
		break;
	case RESULT_CLAN_MEMBER_POSITION://		0x075		// Position adjustment is sent to all of the members
		{
			short nOffset = sizeof( wsv_CLAN_COMMAND );
			if( tag_CLAN_MEMBER* pMember = (tag_CLAN_MEMBER*)Packet_GetDataPtr( m_pRecvPacket, nOffset, sizeof( tag_CLAN_MEMBER ) ) )
			{
				if( char* pszCharName = (char*)Packet_GetStringPtr( m_pRecvPacket, nOffset ) )
					CClan::GetInstance().ChangeClass(pszCharName, pMember->m_btClanPOS );
			}
		}
		break;
	default:
		break;
	}
}

void CRecvPACKET::Recv_wsv_CLAN_CHAT()
{
	short nOffset = sizeof( gsv_SHOUT );
	char *szAccount, *szMsg;
	szAccount = Packet_GetStringPtr( m_pRecvPacket, nOffset);
	szMsg     = Packet_GetStringPtr( m_pRecvPacket, nOffset);

	g_itMGR.AppendChatMsg( CStr::Printf("[%s]%s>%s","CLAN", szAccount, szMsg), IT_MGR::CHAT_TYPE_CLAN);
}

/// Clan marks received from the server storage.
void CRecvPACKET::Recv_wsv_CLANMARK_REPLY()
{
	int iClanID = (int)( m_pRecvPacket->m_wsv_CLANMARK_REPLY.m_dwClanID );
	if( iClanID )//Change was successful and
	{
		WORD wCRC16 = m_pRecvPacket->m_wsv_CLANMARK_REPLY.m_wMarkCRC16;	

		int iSize = m_pRecvPacket->m_wsv_CLANMARK_REPLY.m_nSize - sizeof( wsv_CLANMARK_REPLY );	
		short nOffset = sizeof( wsv_CLANMARK_REPLY );
		BYTE* pDATA = (BYTE*)Packet_GetStringPtr( m_pRecvPacket, nOffset );

		///Save as file
		CClanMarkTransfer::GetSingleton().ReceiveMarkFromServer( iClanID, wCRC16, pDATA, iSize );

		///Clan currently registered mark has been changed so that if you are using to change the Texture.
		///If not in use does not do any operation.

		std::string file_name;
		CClanMarkUserDefined::GetClanMarkFileName( CGame::GetInstance().GetSelectedServerID(), iClanID, file_name );

		CClanMarkManager::GetSingleton().ReloadTexture( file_name.c_str(), wCRC16 );

		if( g_pAVATAR && g_pAVATAR->GetClanID() == iClanID && g_pAVATAR->GetClanPos() >= CClan::CLAN_MASTER )
		{
			///Comes down to two times now.
			if( g_pAVATAR->GetClanMarkCenter() != wCRC16 )
			{
				g_pNet->Send_cli_CLANMARK_REG_TIME();
				g_itMGR.OpenMsgBox( STR_CLANMARK_REGISTER_SUCCESS );
			}
		}

		/// Clan in the covered (and including) the information is updated every clan haejueoya the mark.
		/// If you are a newly registered mark clan CNameBox will find new CClanMarkUserDefined
		/// Mark was registered on the Texture is changed when changing jobs, so there is no give separately.
		g_pObjMGR->ResetClanMarkInfo( iClanID, wCRC16 );
		//g_pAVATAR->SetClanMark( 0, wCRC16 );
	}
	else//Failure
	{
		switch( m_pRecvPacket->m_wsv_CLANMARK_REPLY.m_wFailedReason )
		{
		case RESULT_CLANMARK_TOO_MANY_UPDATE:// The update of the mark after a certain period of time must ...
			g_itMGR.OpenMsgBox( STR_CLANMARK_UPDATE_ERROR );
			break;
		case RESULT_CLANMARK_DB_ERROR:// DB update error
			g_itMGR.OpenMsgBox("Register ClanMark Error:DB");
			break;
		case RESULT_CLANMAKR_SP_ERROR://			0x0003	// SP DB error
			g_itMGR.OpenMsgBox("Register ClanMark Error:SP");
			break;
		default:
			g_itMGR.OpenMsgBox("Register ClanMark Error");
			break;
		}
	}
}

void CRecvPACKET::Recv_gsv_ALLIED_CHAT()
{

	short nOffset = sizeof( gsv_ALLIED_CHAT );

	CObjAVT *pCHAR = g_pObjMGR->Get_ClientCharAVT( m_pRecvPacket->m_gsv_ALLIED_CHAT.m_wObjectIDX, false );
	if ( pCHAR && g_pAVATAR->Get_TeamNO() == m_pRecvPacket->m_gsv_ALLIED_CHAT.m_iTeamNO ) 
	{
		char* pszMsg = Packet_GetStringPtr( m_pRecvPacket, nOffset);
		g_itMGR.AppendChatMsg( CStr::Printf("[%s]%s>%s", STR_ALLIED, pCHAR->Get_NAME(), pszMsg), IT_MGR::CHAT_TYPE_ALLIED );
	}
}

void CRecvPACKET::Recv_gsv_ALLIED_SHOUT()
{
	short nOffset = sizeof( gsv_ALLIED_SHOUT );
	char* pszAccount = Packet_GetStringPtr( m_pRecvPacket, nOffset);
	char* szMsg		 = Packet_GetStringPtr( m_pRecvPacket, nOffset);
	if ( g_pAVATAR && pszAccount && szMsg && g_pAVATAR->Get_TeamNO() == m_pRecvPacket->m_gsv_ALLIED_SHOUT.m_iTeamNO ) 
		g_itMGR.AppendChatMsg ( CStr::Printf("[%s]%s>%s", STR_ALLIED, pszAccount, szMsg), IT_MGR::CHAT_TYPE_ALLIED );
}

void CRecvPACKET::Recv_wsv_CLANMARK_REG_TIME()
{
	SYSTEMTIME clanmark_regtime;

	clanmark_regtime.wYear  = m_pRecvPacket->m_wsv_CLANMARK_REG_TIME.m_wYear;
	clanmark_regtime.wMonth = m_pRecvPacket->m_wsv_CLANMARK_REG_TIME.m_btMon;
	clanmark_regtime.wDay   = m_pRecvPacket->m_wsv_CLANMARK_REG_TIME.m_btDay;
	clanmark_regtime.wHour  = m_pRecvPacket->m_wsv_CLANMARK_REG_TIME.m_btHour;
	clanmark_regtime.wMinute = m_pRecvPacket->m_wsv_CLANMARK_REG_TIME.m_btMin;
	clanmark_regtime.wSecond = m_pRecvPacket->m_wsv_CLANMARK_REG_TIME.m_btSec;

	CClan::GetInstance().SetClanMarkRegTime( clanmark_regtime );
}


//=======================================================================================
//
//  Begin - Keulraenjeon related
//
//=======================================================================================
#ifdef __CLAN_WAR_SET

//---------------------------------------------------------------------------------------
//  Name  : Recv_gsv_CLANWAR_JOIN_REQ()
//  Desc  : Keulraenjeon request entry
//  Author: Antonio - 2006-07-24  Pm 12:29:53
//---------------------------------------------------------------------------------------
void CRecvPACKET::Recv_gsv_CLANWAR_JOIN_REQ()
{
	char psz[_MAX_PATH];
	sprintf( psz, STR_CLANWAR_JOIN_REQ );

	gsv_cli_CLAN_WAR_OK *p = &m_pRecvPacket->m_gsv_cli_ClanWar_OK;
	CTCmdClanWarJoinAck *pCmdOk		= new CTCmdClanWarJoinAck( p, true  );
	CTCmdClanWarJoinAck *pCmdCancel	= new CTCmdClanWarJoinAck( p, false );
	short btType = CMsgBox::BT_OK | CMsgBox::BT_CANCEL;

	//200700503 Honggeun: break-modal.
	g_itMGR.OpenMsgBox( psz,
						btType,
						true,
						0,
						pCmdOk, pCmdCancel,
						CMsgBox::MSGTYPE_RECV_CLANWAR_JOIN_REQ,
						"MsgBox",
						true,
						CTCmdClanWarJoinAck::time,
						CMsgBox::CHECKCOMMAND_NONE );
}

//---------------------------------------------------------------------------------------
//  Name  : Recv_gsv_CLANWAR_TIME()
//  Desc  : Processes related to the remaining time keulraenjeon
//  Author: Antonio - 2006-07-27  Pm 6:19:06
//---------------------------------------------------------------------------------------
void CRecvPACKET::Recv_gsv_CLANWAR_TIME()
{
	gsv_cli_CLAN_WAR_TIME *p = &m_pRecvPacket->m_gsv_cli_ClanWar_Time;
	SetSpriteTime( p->m_Play_Count );
}

//---------------------------------------------------------------------------------------
//  Name  : Recv_gsv_CLANWAR_ERR()
//  Desc  : Keulraenjeon related error handling
//  Author: Antonio - 2006-07-27  Pm 6:19:06
//---------------------------------------------------------------------------------------
//Keulraenjeon modified message.
void CRecvPACKET::Recv_gsv_CLANWAR_ERR()
{
	char psz[_MAX_PATH] = { '\0' };

	gsv_cli_CLAN_WAR_ERR *p = &m_pRecvPacket->m_gsv_cli_ClanWar_Err;

	switch ( p->m_CW_Err_w )
	{
	case CWAR_OPPONENT_MASTER_ALREADY_GIVEUP:	// Zhang clan already abandoned when the relative
		{
			if ( CTCmdClanWarJoinAck::isPresent )
			{
				CTCmdClanWarJoinAck::err = CWAR_OPPONENT_MASTER_ALREADY_GIVEUP;
			}
			else
			{
				//[Keulraenjeon] clan master the other abstained September
				sprintf( psz, STR_CLANWAR_ABSTENTION );
				g_itMGR.OpenMsgBox( psz );
			}
			break;
		}
	case CWAR_QUEST_LIST_FULL:	// When applying keulraenjeon the quest log is full
		{
			if ( CTCmdClanWarJoinAck::isPresent )
			{
				CTCmdClanWarJoinAck::err = CWAR_QUEST_LIST_FULL;
			}
			else
			{
				//[Keulraenjeon] holds more than 10 quests, so you can not participated keulraenjeon.
				sprintf( psz, STR_CLANWAR_QUEST_LIST_FULL );
				g_itMGR.OpenMsgBox( psz );
			}
			break;
		}

	case CWAR_CLAN_MASTER_LEVEL_RESTRICTED:	// Level sacred clan chapter keulraenjeon not apply when the limit
		{
			if ( CTCmdClanWarJoinAck::isPresent )
			{
				CTCmdClanWarJoinAck::err = CWAR_CLAN_MASTER_LEVEL_RESTRICTED;
			}
			else
			{
				//[Keulraenjeon] You can not choose your level of participation amount.
				sprintf( psz, STR_CLANWAR_OFFER_MISLEVEL );
				g_itMGR.OpenMsgBox( psz );
			}
			break;
		}
#if 0
	case 4:	// Level restrictions allowed when keulraenjeon
		{
			//[Keulraenjeon] Clan master is that you can not apply to join the keulraenjeon level.
			sprintf( psz, STR_CLANWAR_ENTER_MISLEVEL );
			g_itMGR.OpenMsgBox( psz );
			break;
		}
#endif
	case CWAR_TOO_MANY_CWAR_PROGRESS:       
		{            
			//  Limited number of participants allowed to keulraenjeon message.
			//[Keulraenjeon] beyond the person can not participate in more keulraenjeon.
			sprintf( psz, LIST_STRING(953) );
			g_itMGR.OpenMsgBox( psz );
			break;
		}
	case CWAR_TIME_OUT:
		{
			//Keulraenjeon accept timeout. Keulraenjeon end.	
			CMsgBox * pMsgBox = NULL;
			if( pMsgBox = g_itMGR.FindMsgBox(CMsgBox::MSGTYPE_RECV_CLANWAR_JOIN_REQ) )
			{
				pMsgBox->Hide();
			}
			if( pMsgBox = g_itMGR.FindMsgBox(CMsgBox::MSGTYPE_RECV_CLANWAR_JOIN_REQ_RECONFIRM) )
			{
				pMsgBox->Hide();
			}
		}
		break;
	case CWAR_OPPONENT_MASTER_GIVEUP:
		{
			// 974 : [Keulraenjeon] Clan master is waived by the other party to win in November keulraenjeon.
			sprintf( psz, LIST_STRING(974) );
			g_itMGR.OpenMsgBox( psz );
		}
		break;
	case CWAR_MASTER_GIVEUP:
		{
			// 975 : [Keulraenjeon] keulraenjeon to defeat the clan master has waived September
			sprintf( psz, LIST_STRING(975) );
			g_itMGR.OpenMsgBox( psz );
		}
		break;
	}
}

//---------------------------------------------------------------------------------------
//  Name  : Recv_gsv_CLANWAR_PROGRESS()
//  Desc  : Keulraenjeon message processing proceeds
//  Author: Antonio - 2006-08-21  pm 5:35:48
//---------------------------------------------------------------------------------------
void CRecvPACKET::Recv_gsv_CLANWAR_PROGRESS()
{
	gsv_cli_CLAN_WAR_PROGRESS *p = &m_pRecvPacket->m_gsv_cli_ClanWar_Progress;
	SetClanWarProgressMsg( p->m_CW_Progress );
}


void CRecvPACKET::Recv_gsv_CLANWAR_BOSS_HP()
{
#ifdef __CLAN_WAR_BOSSDLG
	gsv_cli_CLAN_WAR_BOSS_HP * pRecvPacket = &m_pRecvPacket->m_gsv_cli_ClanWar_BOSS_HP;	

	CClanWar_BossDlg* pDLG = (CClanWar_BossDlg*)g_itMGR.FindDlg( DLG_TYPE_CLANWAR_BOSS );
	if( !pDLG )//If you do not create a paste.
	{	
		pDLG = new CClanWar_BossDlg;
		pDLG->Show();
	}	
	pDLG->Set_Boss( pRecvPacket );
#endif
}

#endif
//=======================================================================================
//
//  End - Keulraenjeon related
//
//=======================================================================================


void CRecvPACKET::Recv_gsv_ITEM_RESULT_REPORT()
{
	WORD wObjectIdx = m_pRecvPacket->m_gsv_ITEM_RESULT_REPORT.m_wObjectIDX;
	if( CObjAVT* pAvt = g_pObjMGR->Get_ClientCharAVT( wObjectIdx, false ) )
	{
		switch( m_pRecvPacket->m_gsv_ITEM_RESULT_REPORT.m_btREPORT )
		{
		case REPORT_ITEM_CREATE_START://		0x00	// Started manufacturing items
			SE_StartMake ( pAvt->Get_INDEX() );
			break;
		case REPORT_ITEM_CREATE_SUCCESS://		0x01	// Items manufactured success
			SE_SuccessMake( pAvt->Get_INDEX() );
			break;
		case REPORT_ITEM_CREATE_FAILED://		0x02	// Items manufactured failure
			SE_FailMake( pAvt->Get_INDEX() );
			break;
		case REPORT_ITEM_UPGRADE_START://		0x03	// Items smelting start
			SE_StartUpgrade( pAvt->Get_INDEX() );
			break;
		case REPORT_ITEM_UPGRADE_SUCCESS://		0x04	// Items smelting success
			SE_SuccessUpgrade( pAvt->Get_INDEX() );
			break;
		case REPORT_ITEM_UPGRADE_FAILED://		0x05	// Items smelting failure
			SE_FailUpgrade( pAvt->Get_INDEX() );
			break;
		default:
			break;
		}
	}
}

void CRecvPACKET::Recv_gsv_MALL_ITEM_REPLY()
{
	switch( m_pRecvPacket->m_gsv_MALL_ITEM_REPLY.m_btReplyTYPE )
	{
	case REPLY_MALL_ITEM_CHECK_CHAR_FOUND:
		///Present the target was found.
		/// Dueotdeon save by importing a character name and the item name will be assigned to the message box OK button to Open a Command to create.
		if( CTDialog* pDlg = g_itMGR.FindDlg( DLG_TYPE_DELIVERYSTORE ) )
		{

			CDeliveryStoreDlg* p = (CDeliveryStoreDlg*)pDlg;
			assert( p->get_selecteditem_slotindex() >= 1 );
			if( p->get_selecteditem_slotindex() >= 1 )
			{
				CTCommand* pCmdOk = new CTCmdGiftMallItem( (BYTE)(p->get_selecteditem_slotindex() - 1 ), p->get_receiver_name() );
				g_itMGR.OpenMsgBox( CStr::Printf("%s:%s{BR}%s:%s", STR_RECEIVER, p->get_receiver_name(), STR_SENDING_ITEM ,p->GetSelectedItemName() ), CMsgBox::BT_OK | CMsgBox::BT_CANCEL, true, 0, pCmdOk );
			}
		}	

		break;
	case REPLY_MALL_ITEM_CHECK_CHAR_NONE:
		g_itMGR.OpenMsgBox(STR_REPLY_MALL_ITEM_CHECK_CHAR_NONE);
		break;
	case REPLY_MALL_ITEM_CHECK_CHAR_INVALID:
		g_itMGR.OpenMsgBox(STR_REPLY_MALL_ITEM_CHECK_CHAR_INVALID);
		break;
	case REPLY_MALL_ITEM_BRING_SUCCESS:
		if( CTDialog* pDlg = g_itMGR.FindDlg( DLG_TYPE_DELIVERYSTORE ) )
		{
			g_pAVATAR->Set_ITEM( m_pRecvPacket->m_gsv_MALL_ITEM_REPLY.m_BringITEM[0].m_btInvIDX,
				m_pRecvPacket->m_gsv_MALL_ITEM_REPLY.m_BringITEM[0].m_ITEM );

			CDeliveryStoreDlg* p = (CDeliveryStoreDlg*)pDlg;
			p->SetItem( m_pRecvPacket->m_gsv_MALL_ITEM_REPLY.m_BringITEM[1].m_btInvIDX,
				m_pRecvPacket->m_gsv_MALL_ITEM_REPLY.m_BringITEM[1].m_ITEM	);///DeliveryStore raiding.
		}
		break;
	case REPLY_MALL_ITEM_BRING_FAILED:
		g_itMGR.OpenMsgBox(STR_REPLY_MALL_ITEM_BRING_FAILED);
		break;
	case REPLY_MALL_ITME_GIVE_SUCCESS:
		if( CTDialog* pDlg = g_itMGR.FindDlg( DLG_TYPE_DELIVERYSTORE ) )
		{
			CDeliveryStoreDlg* p = (CDeliveryStoreDlg*)pDlg;
			p->SetItem( m_pRecvPacket->m_gsv_MALL_ITEM_REPLY.m_BringITEM[0].m_btInvIDX,
				m_pRecvPacket->m_gsv_MALL_ITEM_REPLY.m_BringITEM[0].m_ITEM );

			g_itMGR.OpenMsgBox(STR_REPLY_MALL_ITME_GIVE_SUCCESS);
		}
		break;
	case REPLY_MALL_ITEM_GIVE_FAILED:
		g_itMGR.OpenMsgBox(STR_REPLY_MALL_ITEM_GIVE_FAILED);
		break;
	case REPLY_MALL_ITEM_LIST_START:
		if( CTDialog* pDlg = g_itMGR.FindDlg( DLG_TYPE_DELIVERYSTORE ) )
		{
			CDeliveryStoreDlg* p = (CDeliveryStoreDlg*)pDlg;
			p->ClearItem();
		}
		break;
	case REPLY_MALL_ITEM_LIST_DATA:
		if( CTDialog* pDlg = g_itMGR.FindDlg( DLG_TYPE_DELIVERYSTORE ) )
		{
			CDeliveryStoreDlg* p = (CDeliveryStoreDlg*)pDlg;

			short nOffset = sizeof( gsv_MALL_ITEM_REPLY );

			tagITEM* pItem = NULL;
			char* pszFrom	= NULL;
			char* pszDesc	= NULL;
			char* pszTo		= NULL;

			for( int count = 0; count < m_pRecvPacket->m_gsv_MALL_ITEM_REPLY.m_btCntOrIdx; ++count )
			{
				pItem = (tagITEM*)Packet_GetDataPtr( m_pRecvPacket, nOffset, sizeof(tagITEM) );
				pszFrom = Packet_GetStringPtr( m_pRecvPacket, nOffset );                

				if( pszFrom && pszFrom[0] != '\0' )
				{
					pszDesc = Packet_GetStringPtr( m_pRecvPacket, nOffset );
					pszTo   = Packet_GetStringPtr( m_pRecvPacket, nOffset );
				}
								
				p->AddItem( pItem , pszFrom , pszDesc, pszTo );							

				pszFrom = NULL;
				pszDesc = NULL;
				pszTo   = NULL;
			}
		}
		break;
	case REPLY_MALL_ITEM_LIST_END:
		///?????
		break;
	default:
		break;
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////////
//	Japan end billing process
///////////////////////////////////////////////////////////////////////////////////////////////////////
void CRecvPACKET::Set_BILLING_MESSAGE_EXT_JPN()
{
	CGame::GetInstance().SetPayType( 0 );

	g_itMGR.AppendChatMsg( STR_JP_PAY_TYPE, IT_MGR::CHAT_TYPE_SYSTEM );				

	if(m_pRecvPacket->m_gsv_BILLING_MESSAGE_EXT.m_dwPlayingFlag[0])
	{
		if( m_pRecvPacket->m_gsv_BILLING_MESSAGE_EXT.m_dwPlayingFlag[0] & PAY_FLAG_JP_BATTLE )
		{
			g_itMGR.AppendChatMsg( STR_JP_PAY_ENTRY, IT_MGR::CHAT_TYPE_SYSTEM );				
		}
		if( m_pRecvPacket->m_gsv_BILLING_MESSAGE_EXT.m_dwPlayingFlag[0] & PAY_FLAG_JP_COMMUNITY )
		{
			g_itMGR.AppendChatMsg( STR_JP_PAY_COMMUNITY, IT_MGR::CHAT_TYPE_SYSTEM );			
		}
		if( m_pRecvPacket->m_gsv_BILLING_MESSAGE_EXT.m_dwPlayingFlag[0] & PAY_FLAG_JP_TRADE )
		{
			g_itMGR.AppendChatMsg( STR_JP_PAY_TRADE, IT_MGR::CHAT_TYPE_SYSTEM );			
		}
		if( m_pRecvPacket->m_gsv_BILLING_MESSAGE_EXT.m_dwPlayingFlag[0] & PAY_FLAG_JP_STOCK_SPACE )
		{
			g_itMGR.AppendChatMsg( STR_JP_PAY_STOCKSPACE, IT_MGR::CHAT_TYPE_SYSTEM );			
		}
		if( m_pRecvPacket->m_gsv_BILLING_MESSAGE_EXT.m_dwPlayingFlag[0] & PAY_FLAG_JP_EXTRA_STOCK )
		{
			g_itMGR.AppendChatMsg( STR_JP_PAY_EXTRASTOCK, IT_MGR::CHAT_TYPE_SYSTEM );			
		}
		if( m_pRecvPacket->m_gsv_BILLING_MESSAGE_EXT.m_dwPlayingFlag[0] & PAY_FLAG_JP_STARSHIP_PASS )
		{
			g_itMGR.AppendChatMsg( STR_JP_PAY_STARSHIPPASS, IT_MGR::CHAT_TYPE_SYSTEM );			
		}
		if( m_pRecvPacket->m_gsv_BILLING_MESSAGE_EXT.m_dwPlayingFlag[0] & PAY_FLAG_JP_DUNGEON_ADV )
		{
			g_itMGR.AppendChatMsg( STR_JP_PAY_DUNGEONADVENTURE, IT_MGR::CHAT_TYPE_SYSTEM );			
		}
		if( m_pRecvPacket->m_gsv_BILLING_MESSAGE_EXT.m_dwPlayingFlag[0] & PAY_FLAG_JP_EXTRA_CHAR)
		{
			g_itMGR.AppendChatMsg( STR_JP_PAY_EXTRACHARACTER, IT_MGR::CHAT_TYPE_SYSTEM );			
		}		
	}	
	else
	{
		g_itMGR.AppendChatMsg( STR_JP_PAY_FREEPLAY, IT_MGR::CHAT_TYPE_SYSTEM );
	}

	CGame::GetInstance().SetPayType( (WORD)m_pRecvPacket->m_gsv_BILLING_MESSAGE_EXT.m_dwPlayingFlag[0] );
}


//First, I do TW ... yet does not leave.
void CRecvPACKET::Set_BILLING_MESSAGE_EXT_TW()
{
	switch(m_pRecvPacket->m_gsv_BILLING_MESSAGE_EXT.m_dwPayType)
	{
	case 0:	//Prohibition
		break;
	case 1: //Hour, every 1 hour 06 point reduction, 1 hour to 1 hour If you fail fill calculations.
		g_itMGR.AppendChatMsg( STR_TW_BILL_TYPE_1HOUR, IT_MGR::CHAT_TYPE_SYSTEM );
		break;
	case 2:	//May Learn, 30, 350 point reduction.
		g_itMGR.AppendChatMsg( STR_TW_BILL_TYPE_MONTH, IT_MGR::CHAT_TYPE_SYSTEM );
		break;
	case 3:	//Subscribe Hour, 20 basis point reduction to 12 hours.
		g_itMGR.AppendChatMsg( STR_TW_BILL_TYPE_12HOUR, IT_MGR::CHAT_TYPE_SYSTEM );
		break;
	case 4:	//Learn 3 months, 90 days and 888 basis point reduction.
		g_itMGR.AppendChatMsg( STR_TW_BILL_TYPE_3MONTH, IT_MGR::CHAT_TYPE_SYSTEM );
		break;
	case 5:	//Hong Kong Monthly Subscription.
		g_itMGR.AppendChatMsg( STR_TW_BILL_TYPE_HONGKONG_MONTH, IT_MGR::CHAT_TYPE_SYSTEM );
		break;
	case 6:	//3 months subscription in Hong Kong.
		g_itMGR.AppendChatMsg( STR_TW_BILL_TYPE_HONGKONG_3MONTH, IT_MGR::CHAT_TYPE_SYSTEM );
		break;
	case 7:	//Hong Kong time
		g_itMGR.AppendChatMsg( STR_TW_BILL_TYPE_HONGKONG_HOUR, IT_MGR::CHAT_TYPE_SYSTEM );
		break;
	case 8:	//Learn Hong Kong time units.
		g_itMGR.AppendChatMsg( STR_TW_BILL_TYPE_HONGKONG_12HOUR, IT_MGR::CHAT_TYPE_SYSTEM );
		break;
	case 9:	//One image every 0.25 point reduction in minutes.
		g_itMGR.AppendChatMsg( STR_TW_BILL_TYPE_MINUTE, IT_MGR::CHAT_TYPE_SYSTEM );
		break;
	case 10:	//2 Hour Learn, every two hours, the 12-point cut.
		g_itMGR.AppendChatMsg( STR_TW_BILL_TYPE_2HOUR, IT_MGR::CHAT_TYPE_SYSTEM );
		break;
	case 11:	//10 days Subscription, 150 basis point reduction every 10 days.
		g_itMGR.AppendChatMsg( STR_TW_BILL_TYPE_10DAY, IT_MGR::CHAT_TYPE_SYSTEM );
		break;
	case 12:	//Second trial
		g_itMGR.AppendChatMsg( STR_TW_BILL_TYPE_FREE, IT_MGR::CHAT_TYPE_SYSTEM );
		break;
	case 101:	//Block 3 days
		g_itMGR.AppendChatMsg( STR_TW_BILL_TYPE_BLOCK_3DAY, IT_MGR::CHAT_TYPE_SYSTEM );
		break;
	case 102:	//Block 7 days
		g_itMGR.AppendChatMsg( STR_TW_BILL_TYPE_BLOCK_7DAY, IT_MGR::CHAT_TYPE_SYSTEM );
		break;
	case 103:	//Blocks 10 days
		g_itMGR.AppendChatMsg( STR_TW_BILL_TYPE_BLOCK_10DAY, IT_MGR::CHAT_TYPE_SYSTEM );
		break;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
//	Korea, the Philippines end billing process
//
//	Individual sperm (Platinum), all the non-premium.
///////////////////////////////////////////////////////////////////////////////////////////////////////
void CRecvPACKET::Set_BILLING_MESSAGE_EXT_KOR_And_PH()
{
#ifdef __KOREA
	switch(m_pRecvPacket->m_gsv_BILLING_MESSAGE_EXT.m_dwPayType)
	{
	case BILLING_MSG_PAY_FU:
		g_itMGR.AppendChatMsg( STR_FREE_ACCOUNT_USER, IT_MGR::CHAT_TYPE_SYSTEM );		
		break;
	case BILLING_MSG_PAY_FA:
		if( CGame::GetInstance().GetDetailPayType() != BILLING_MSG_FREE_USER )
		{
			g_itMGR.AppendChatMsg( STR_PAID_DAYBASED_PLAN, IT_MGR::CHAT_TYPE_SYSTEM );			
		}
		else
		{
			g_itMGR.AppendChatMsg( STR_BILLING_MSG_FREE_USER, IT_MGR::CHAT_TYPE_SYSTEM );			
		}
		break;
	case BILLING_MSG_PAY_FAP:
		g_itMGR.AppendChatMsg( STR_PAID_DAYBASED_PLAN_PLATINUM, IT_MGR::CHAT_TYPE_SYSTEM );		
		break;

	case BILLING_MSG_PAY_FQ:
		g_itMGR.AppendChatMsg( STR_PAID_TIMEBASED_PLAN, IT_MGR::CHAT_TYPE_SYSTEM );		
		break;

	case BILLING_MSG_PAY_GU:
		g_itMGR.AppendChatMsg( STR_PAID_INTERNET_CAFE, IT_MGR::CHAT_TYPE_SYSTEM );		
		break;

	case BILLING_MSG_PAY_GQ:
		g_itMGR.AppendChatMsg( STR_PAID_USER_PLAN_IC_TIME_END, IT_MGR::CHAT_TYPE_SYSTEM );
		break;

	case BILLING_MSG_PAY_IQ:
		g_itMGR.AppendChatMsg( STR_PAID_USER_PLAN_IC_IP_FULL, IT_MGR::CHAT_TYPE_SYSTEM );
		break;
	}
	
#else
	switch(m_pRecvPacket->m_gsv_BILLING_MESSAGE_EXT.m_dwPayType)
	{
	case PAY_FLAG_KR_FU:
		g_itMGR.AppendChatMsg( STR_FREE_ACCOUNT_USER, IT_MGR::CHAT_TYPE_SYSTEM );			
		CGame::GetInstance().SetPayType( PLAY_FLAG_KOREA_DEFAULT | PLAY_FLAG_EXTRA_CHAR | PLAY_FLAG_EXTRA_STOCK );
		break;

	case PAY_FLAG_KR_FA:
		{
			g_itMGR.AppendChatMsg( STR_PAID_DAYBASED_PLAN, IT_MGR::CHAT_TYPE_SYSTEM );				
			CGame::GetInstance().SetPayType( PLAY_FLAG_KOREA_DEFAULT );
			break;
		}			

	case PAY_FLAG_KR_FAP:
		g_itMGR.AppendChatMsg( STR_PAID_DAYBASED_PLAN_PLATINUM, IT_MGR::CHAT_TYPE_SYSTEM );			
		CGame::GetInstance().SetPayType( PLAY_FLAG_KOREA_DEFAULT | PLAY_FLAG_EXTRA_CHAR | PLAY_FLAG_EXTRA_STOCK );
		break;
	case PAY_FLAG_KR_FQ:
		g_itMGR.AppendChatMsg( STR_PAID_TIMEBASED_PLAN, IT_MGR::CHAT_TYPE_SYSTEM );
		CGame::GetInstance().SetPayType( PLAY_FLAG_KOREA_DEFAULT );
		break;
	case PAY_FLAG_KR_GU:
		g_itMGR.AppendChatMsg( STR_PAID_INTERNET_CAFE, IT_MGR::CHAT_TYPE_SYSTEM );
		CGame::GetInstance().SetPayType( PLAY_FLAG_KOREA_DEFAULT );
		break;
	case PAY_FLAG_KR_GQ:
		g_itMGR.AppendChatMsg( STR_PAID_USER_PLAN_IC_TIME_END, IT_MGR::CHAT_TYPE_SYSTEM );
		CGame::GetInstance().SetPayType( PLAY_FLAG_KOREA_DEFAULT );
		break;
	case PAY_FLAG_KR_IQ:
		g_itMGR.AppendChatMsg( STR_PAID_USER_PLAN_IC_IP_FULL, IT_MGR::CHAT_TYPE_SYSTEM );
		CGame::GetInstance().SetPayType( PLAY_FLAG_KOREA_DEFAULT );
		break;		
	}	
#endif	
}	

///////////////////////////////////////////////////////////////////////////////////////////////////////
// BILLING_MESSAGE Southern part of me in the process.
///////////////////////////////////////////////////////////////////////////////////////////////////////
void CRecvPACKET::Set_BILLING_MESSAGE_EXT_Other()
{
	switch( m_pRecvPacket->m_gsv_BILLING_MESSAGE_EXT.m_wMsgTYPE )
	{
	case BILLING_MSG_KICKOUT_CHEAT_DETECT:
		g_itMGR.AppendChatMsg( STR_BILLING_MSG_KICKOUT_CHEAT_DETECT ,IT_MGR::CHAT_TYPE_SYSTEM);
		break;
	case BILLING_MSG_KICKOUT_ALREADY_LOGIN:
		g_itMGR.AppendChatMsg( STR_BILLING_MSG_KICKOUT_ALREADY_LOGIN ,IT_MGR::CHAT_TYPE_SYSTEM);
		break;
	case BILLING_MSG_KICKOUT_TIME_EXPIRED:
		g_itMGR.AppendChatMsg( STR_BILLING_MSG_KICKOUT_TIME_EXPIRED ,IT_MGR::CHAT_TYPE_SYSTEM);
		break;
	case BILLING_MSG_KICKOUT_DUP_LOGIN:
		g_itMGR.AppendChatMsg( STR_BILLING_MSG_KICKOUT_DUP_LOGIN ,IT_MGR::CHAT_TYPE_SYSTEM);
		break;
	case BILLING_MSG_KICKOUT_NO_RIGHT:
		g_itMGR.AppendChatMsg( STR_BILLING_MSG_KICKOUT_NO_RIGHT ,IT_MGR::CHAT_TYPE_SYSTEM);
		break;
	case BILLING_MSG_KICKOUT_OUT_OF_IP:
		g_itMGR.AppendChatMsg( STR_BILLING_MSG_KICKOUT_OUT_OF_IP ,IT_MGR::CHAT_TYPE_SYSTEM);
		break;
	case BILLING_MSG_KICKOUT_EXIST_ACCOUNT:
		g_itMGR.AppendChatMsg( STR_BILLING_MSG_KICKOUT_EXIST_ACCOUNT ,IT_MGR::CHAT_TYPE_SYSTEM);
		break;
	case BILLING_MSG_TYPE_SET_DATE:///Expiration date
		{
			if( CGame::GetInstance().GetDetailPayType() != PAY_FLAG_KR_FU )
			{
				short nOffset = sizeof( gsv_BILLING_MESSAGE_EXT );

				if( char* pszRemainTime = Packet_GetStringPtr( m_pRecvPacket, nOffset ) )
				{
					std::string strTime = pszRemainTime;
					CGame::GetInstance().SetExpireTime( pszRemainTime );
					CGame::GetInstance().SetExpireTimeType( 1 );

					if( strTime.size() >= 12 )
					{
						int year  = atoi( strTime.substr( 0, 4 ).c_str() );
						int month = atoi( strTime.substr( 4, 2 ).c_str() );
						int day   = atoi( strTime.substr( 6, 2 ).c_str() );
						int hour  = atoi( strTime.substr( 8, 2 ).c_str() );
						int min   = atoi( strTime.substr( 10, 2 ).c_str() );

						g_itMGR.AppendChatMsg( CStr::Printf("%s:%d/%d/%d %d:%02d", STR_EXPIRED_TIME, year, month, day, hour, min ), IT_MGR::CHAT_TYPE_SYSTEM );
					}
				}
			}
		}
		break;
	case BILLING_MSG_TYPE_SET_TIME:///The remaining minutes
		{
			short nOffset = sizeof( gsv_BILLING_MESSAGE_EXT );
			if( char* pszRemainTime = Packet_GetStringPtr( m_pRecvPacket, nOffset ) )
			{
				CGame::GetInstance().SetExpireTime( pszRemainTime );
				CGame::GetInstance().SetExpireTimeType( 2 );
				DWORD time = atoi( pszRemainTime );
				g_itMGR.AppendChatMsg( CStr::Printf(STR_GAME_TIME_LEFT_MINS, time ), IT_MGR::CHAT_TYPE_SYSTEM );
			}
		}
		break;
	case BILLING_MSG_TYPE_TIME_ALERT:///The remaining minutes
		{
			short nOffset = sizeof( gsv_BILLING_MESSAGE_EXT );
			if( char* pszRemainTime = Packet_GetStringPtr( m_pRecvPacket, nOffset ) )
			{
				CGame::GetInstance().SetExpireTime( pszRemainTime );
				CGame::GetInstance().SetExpireTimeType( 2 );

				DWORD time = atoi( pszRemainTime );
				g_itMGR.AppendChatMsg( CStr::Printf( STR_GAME_TIME_LEFT_MINS, time ), IT_MGR::CHAT_TYPE_ERROR );
			}
		}
		break;
	case BILLING_MSG_TYPE_TAIWAN:
		{
			short nOffset = sizeof( gsv_BILLING_MESSAGE_EXT );
			if( char* pszMsg = Packet_GetStringPtr( m_pRecvPacket, nOffset ) )
				g_itMGR.AppendChatMsg( pszMsg, IT_MGR::CHAT_TYPE_SYSTEM );
		}
		break;
	case BILLING_MSG_KICKOUT_TAIWAN:
		{
			//g_itMGR.AppendChatMsg( "Disconnect From Billing Server", IT_MGR::CHAT_TYPE_SYSTEM );
		}
		break;

	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Extended billing process.
/////////////////////////////////////////////////////////////////////////////////////////////////////////
void CRecvPACKET::Recv_gsv_BILLING_MESSAGE_EXT()
{
	///Appropriate message output
	//CGame::GetInstance().SetExpireTimeType( 0 );
	//CGame::GetInstance().SetPayType( 0 );

	std::string strBuf;

	switch( m_pRecvPacket->m_gsv_BILLING_MESSAGE_EXT.m_wMsgTYPE )
	{
	case EXT_BILLING_MSG_PAY_KOREA:	//korea, Philippines.
		Set_BILLING_MESSAGE_EXT_KOR_And_PH();
		break;

	case EXT_BILLING_MSG_PAY_TAIWAN:
		Set_BILLING_MESSAGE_EXT_TW();
		break;

	case EXT_BILLING_MSG_PAY_JAPAN:		
		Set_BILLING_MESSAGE_EXT_JPN();					
		break;

	case EXT_BILLING_MSG_TYPE_SET_DATE: //Expiration date

		if( m_pRecvPacket->m_gsv_BILLING_MESSAGE_EXT.m_dwPayType==0 
			&& m_pRecvPacket->m_gsv_BILLING_MESSAGE_EXT.m_dwPlayingFlag[0] == 0)
		{
			short nOffset = sizeof( gsv_BILLING_MESSAGE_EXT );

			if( char* pszRemainTime = Packet_GetStringPtr( m_pRecvPacket, nOffset ) )
			{

				//ExpireTime
				g_itMGR.AppendChatMsg( CStr::Printf("%s", STR_JP_PAY_EXPIRE_DATE), IT_MGR::CHAT_TYPE_SYSTEM );

				char* p = strtok( pszRemainTime, "\t" );
				while( p != NULL )
				{
					//type
					std::string strType = p;

					//time
					p = strtok( NULL,"\t" );					

					std::string strTime;
					if(p)
					{
						strTime = p;
					}

					//함수 작성.

					int iType = atoi( strType.c_str() );

					CGame::GetInstance().SetExpireTimeExt( iType-1, strTime.c_str() );
					CGame::GetInstance().SetExpireTimeTypeExt( iType-1, 1 );

					if( strTime.size() >= 12 )
					{
						int year  = atoi( strTime.substr( 0, 4 ).c_str() );
						int month = atoi( strTime.substr( 4, 2 ).c_str() );
						int day   = atoi( strTime.substr( 6, 2 ).c_str() );
						int hour  = atoi( strTime.substr( 8, 2 ).c_str() );
						int min   = atoi( strTime.substr( 10, 2 ).c_str() );

						switch(iType-1)
						{
						case BILL_FLAG_JP_BATTLE:
							g_itMGR.AppendChatMsg( CStr::Printf("%s:%d/%d/%d %d:%02d", STR_JP_PAY_ENTRY, year, month, day, hour, min ), IT_MGR::CHAT_TYPE_SYSTEM );
							break;
						case BILL_FLAG_JP_COMMUNITY:
							g_itMGR.AppendChatMsg( CStr::Printf("%s:%d/%d/%d %d:%02d", STR_JP_PAY_COMMUNITY, year, month, day, hour, min ), IT_MGR::CHAT_TYPE_SYSTEM );
							break;
						case BILL_FLAG_JP_TRADE:
							g_itMGR.AppendChatMsg( CStr::Printf("%s:%d/%d/%d %d:%02d", STR_JP_PAY_TRADE, year, month, day, hour, min ), IT_MGR::CHAT_TYPE_SYSTEM );
							break;
						case BILL_FLAG_JP_STOCK_SPACE:
							g_itMGR.AppendChatMsg( CStr::Printf("%s:%d/%d/%d %d:%02d", STR_JP_PAY_STOCKSPACE, year, month, day, hour, min ), IT_MGR::CHAT_TYPE_SYSTEM );
							break;
						case BILL_FLAG_JP_EXTRA_STOCK:
							g_itMGR.AppendChatMsg( CStr::Printf("%s:%d/%d/%d %d:%02d", STR_JP_PAY_EXTRASTOCK, year, month, day, hour, min ), IT_MGR::CHAT_TYPE_SYSTEM );
							break;
						case BILL_FLAG_JP_STARSHIP_PA:
							g_itMGR.AppendChatMsg( CStr::Printf("%s:%d/%d/%d %d:%02d", STR_JP_PAY_STARSHIPPASS, year, month, day, hour, min ), IT_MGR::CHAT_TYPE_SYSTEM );
							break;
						case BILL_FLAG_JP_DUNGEON_ADV:
							g_itMGR.AppendChatMsg( CStr::Printf("%s:%d/%d/%d %d:%02d", STR_JP_PAY_DUNGEONADVENTURE, year, month, day, hour, min ), IT_MGR::CHAT_TYPE_SYSTEM );
							break;
						case BILL_FLAG_JP_EXTRA_CHAR:
							g_itMGR.AppendChatMsg( CStr::Printf("%s:%d/%d/%d %d:%02d", STR_JP_PAY_EXTRACHARACTER, year, month, day, hour, min ), IT_MGR::CHAT_TYPE_SYSTEM );
							break;
						}											
					}
					p = strtok( NULL,"\t" );
				}
			}
		}

		break;

	case EXT_BILLING_MSG_TYPE_SET_TIME:	//The remaining minutes

		if( m_pRecvPacket->m_gsv_BILLING_MESSAGE_EXT.m_dwPayType==0 
			&& m_pRecvPacket->m_gsv_BILLING_MESSAGE_EXT.m_dwPlayingFlag[0] == 0)
		{
			short nOffset = sizeof( GSV_BILLING_MESSAGE_EXT );
			if( char* pszRemainTime = Packet_GetStringPtr( m_pRecvPacket, nOffset ) )
			{

				//Remaining time
				g_itMGR.AppendChatMsg( CStr::Printf("%s", STR_JP_PAY_REMAIN_TIME), IT_MGR::CHAT_TYPE_SYSTEM );


				char* p = strtok( pszRemainTime, "\t" );
				while( p != NULL )
				{
					//type
					std::string strType = p;

					//time
					p = strtok( NULL,"\t" );
					std::string strTime = p;

					int iType = atoi( strType.c_str() );

					CGame::GetInstance().SetExpireTimeExt( iType-1, strTime.c_str() );					
					CGame::GetInstance().SetExpireTimeTypeExt( iType-1, 2 );

					DWORD time = atoi( strTime.c_str() );					

					switch(iType-1)
					{
					case BILL_FLAG_JP_BATTLE:
						g_itMGR.AppendChatMsg( CStr::Printf(STR_JP_PAY_REMAIN_TIME_MESSAGE, STR_JP_PAY_ENTRY , strTime.c_str() ), IT_MGR::CHAT_TYPE_SYSTEM );
						break;
					case BILL_FLAG_JP_COMMUNITY:
						g_itMGR.AppendChatMsg( CStr::Printf(STR_JP_PAY_REMAIN_TIME_MESSAGE, STR_JP_PAY_COMMUNITY, strTime.c_str() ), IT_MGR::CHAT_TYPE_SYSTEM );
						break;
					case BILL_FLAG_JP_TRADE:
						g_itMGR.AppendChatMsg( CStr::Printf(STR_JP_PAY_REMAIN_TIME_MESSAGE, STR_JP_PAY_TRADE, strTime.c_str() ), IT_MGR::CHAT_TYPE_SYSTEM );
						break;
					case BILL_FLAG_JP_STOCK_SPACE:
						g_itMGR.AppendChatMsg( CStr::Printf(STR_JP_PAY_REMAIN_TIME_MESSAGE, STR_JP_PAY_STOCKSPACE, strTime.c_str() ), IT_MGR::CHAT_TYPE_SYSTEM );
						break;
					case BILL_FLAG_JP_EXTRA_STOCK:
						g_itMGR.AppendChatMsg( CStr::Printf(STR_JP_PAY_REMAIN_TIME_MESSAGE, STR_JP_PAY_EXTRASTOCK, strTime.c_str() ), IT_MGR::CHAT_TYPE_SYSTEM );
						break;
					case BILL_FLAG_JP_STARSHIP_PA:
						g_itMGR.AppendChatMsg( CStr::Printf(STR_JP_PAY_REMAIN_TIME_MESSAGE, STR_JP_PAY_STARSHIPPASS, strTime.c_str() ), IT_MGR::CHAT_TYPE_SYSTEM );
						break;
					case BILL_FLAG_JP_DUNGEON_ADV:
						g_itMGR.AppendChatMsg( CStr::Printf(STR_JP_PAY_REMAIN_TIME_MESSAGE, STR_JP_PAY_DUNGEONADVENTURE, strTime.c_str() ), IT_MGR::CHAT_TYPE_SYSTEM );
						break;
					case BILL_FLAG_JP_EXTRA_CHAR:
						g_itMGR.AppendChatMsg( CStr::Printf(STR_JP_PAY_REMAIN_TIME_MESSAGE, STR_JP_PAY_EXTRACHARACTER, strTime.c_str() ), IT_MGR::CHAT_TYPE_SYSTEM );
						break;
					}	


					p = strtok( NULL,"\t" );
				}			
			}
		}

		break;

	case EXT_BILLING_MSG_TYPE_TIME_ALERT:
	case EXT_BILLING_MSG_TYPE_TIME_EXPIRED:

		if( m_pRecvPacket->m_gsv_BILLING_MESSAGE_EXT.m_wMsgTYPE == EXT_BILLING_MSG_TYPE_TIME_ALERT )
		{
			//Expired Alert
			strBuf = STR_JP_PAY_EXPIRE_ALERT;
		}
		else
		{
			//Expired.
			strBuf = STR_JP_PAY_EXPIRE;			
		}

		if( m_pRecvPacket->m_gsv_BILLING_MESSAGE_EXT.m_dwPayType==0 )
		{
			//Japan
			if( m_pRecvPacket->m_gsv_BILLING_MESSAGE_EXT.m_dwPlayingFlag[0] )
			{                
				if( m_pRecvPacket->m_gsv_BILLING_MESSAGE_EXT.m_dwPlayingFlag[0] & PAY_FLAG_JP_BATTLE )
				{
					g_itMGR.AppendChatMsg( CStr::Printf("%s : %s", strBuf.c_str(), STR_JP_PAY_ENTRY), IT_MGR::CHAT_TYPE_SYSTEM );
				}
				if( m_pRecvPacket->m_gsv_BILLING_MESSAGE_EXT.m_dwPlayingFlag[0] & PAY_FLAG_JP_COMMUNITY )
				{
					g_itMGR.AppendChatMsg( CStr::Printf("%s : %s", strBuf.c_str(), STR_JP_PAY_COMMUNITY), IT_MGR::CHAT_TYPE_SYSTEM );
				}
				if( m_pRecvPacket->m_gsv_BILLING_MESSAGE_EXT.m_dwPlayingFlag[0] & PAY_FLAG_JP_TRADE )
				{
					g_itMGR.AppendChatMsg( CStr::Printf("%s : %s", strBuf.c_str(), STR_JP_PAY_TRADE), IT_MGR::CHAT_TYPE_SYSTEM );
				}
				if( m_pRecvPacket->m_gsv_BILLING_MESSAGE_EXT.m_dwPlayingFlag[0] & PAY_FLAG_JP_STOCK_SPACE )
				{
					g_itMGR.AppendChatMsg( CStr::Printf("%s : %s", strBuf.c_str(), STR_JP_PAY_STOCKSPACE), IT_MGR::CHAT_TYPE_SYSTEM );					
				}
				if( m_pRecvPacket->m_gsv_BILLING_MESSAGE_EXT.m_dwPlayingFlag[0] & PAY_FLAG_JP_EXTRA_STOCK )
				{
					g_itMGR.AppendChatMsg( CStr::Printf("%s : %s", strBuf.c_str(), STR_JP_PAY_EXTRASTOCK), IT_MGR::CHAT_TYPE_SYSTEM );
				}
				if( m_pRecvPacket->m_gsv_BILLING_MESSAGE_EXT.m_dwPlayingFlag[0] & PAY_FLAG_JP_STARSHIP_PASS )
				{
					g_itMGR.AppendChatMsg( CStr::Printf("%s : %s", strBuf.c_str(), STR_JP_PAY_STARSHIPPASS), IT_MGR::CHAT_TYPE_SYSTEM );
				}
				if( m_pRecvPacket->m_gsv_BILLING_MESSAGE_EXT.m_dwPlayingFlag[0] & PAY_FLAG_JP_DUNGEON_ADV )
				{
					g_itMGR.AppendChatMsg( CStr::Printf("%s : %s", strBuf.c_str(), STR_JP_PAY_DUNGEONADVENTURE), IT_MGR::CHAT_TYPE_SYSTEM );
				}
				if( m_pRecvPacket->m_gsv_BILLING_MESSAGE_EXT.m_dwPlayingFlag[0] & PAY_FLAG_JP_EXTRA_CHAR)
				{
					g_itMGR.AppendChatMsg( CStr::Printf("%s : %s", strBuf.c_str(), STR_JP_PAY_EXTRACHARACTER), IT_MGR::CHAT_TYPE_SYSTEM );
				}

				if(  m_pRecvPacket->m_gsv_BILLING_MESSAGE_EXT.m_wMsgTYPE == EXT_BILLING_MSG_TYPE_TIME_EXPIRED )
				{
					DWORD dwCurPayType = CGame::GetInstance().GetPayType();

					dwCurPayType &= ~m_pRecvPacket->m_gsv_BILLING_MESSAGE_EXT.m_dwPlayingFlag[0];

					CGame::GetInstance().SetPayType( (WORD)dwCurPayType );
				}				
			}			
		}
		break;

	default:
		//BILLING_MSG 've Put too do not wanna wear a helmet seems like coming over.
		Set_BILLING_MESSAGE_EXT_Other();

	}	   
}

void CRecvPACKET::Recv_gsv_BILLING_MESSAGE()
{
	///Appropriate message output

	CGame::GetInstance().SetExpireTimeType( 0 );

	switch( m_pRecvPacket->m_gsv_BILLING_MESSAGE.m_btTYPE )
	{
		//case BILLING_MSG_FREE_USER:
		//	break;

	case BILLING_MSG_JPN_NEED_CHARGE:

		switch( m_pRecvPacket->m_gsv_BILLING_MESSAGE2.m_cFunctionTYPE )
		{
		case 'H':	//clan skill				
			g_itMGR.OpenMsgBox( STR_JP_BILL_CANT_USE_CLANSKILL );
			break;

		case 'I':	//basic field		
			g_itMGR.OpenMsgBox( STR_JP_BILL_CANT_MOVE_PLANET );
			break;

		case 'J':	//Friends List
			g_itMGR.OpenMsgBox( LIST_STRING(917) );	//Destination using the user does not pay, you can not send messages.
			break;

		case 'P':	// space taravel
			g_itMGR.OpenMsgBox( STR_JP_BILL_CANT_MOVE_PLANET2 );
			break;

		case 'Q':	// Dungeon Adventure
			g_itMGR.OpenMsgBox( STR_JP_BILL_CANT_ENTER_DUNGEON );
			break;
		}

		break;

	case BILLING_MSG_PAY_FU:
		g_itMGR.AppendChatMsg( "Lifetime free users.", IT_MGR::CHAT_TYPE_SYSTEM );
		CGame::GetInstance().SetPayType( m_pRecvPacket->m_gsv_BILLING_MESSAGE.m_btTYPE );
		break;
	case BILLING_MSG_PAY_FA:
		if( CGame::GetInstance().GetDetailPayType() != BILLING_MSG_FREE_USER )
		{
			g_itMGR.AppendChatMsg( "Individual user. Semen", IT_MGR::CHAT_TYPE_SYSTEM );
			CGame::GetInstance().SetPayType( m_pRecvPacket->m_gsv_BILLING_MESSAGE.m_btTYPE );
		}
		else
		{
			g_itMGR.AppendChatMsg( "Free users.", IT_MGR::CHAT_TYPE_SYSTEM );
			CGame::GetInstance().SetPayType( m_pRecvPacket->m_gsv_BILLING_MESSAGE.m_btTYPE );
		}
		break;
	case BILLING_MSG_PAY_FAP:
		g_itMGR.AppendChatMsg( "Individual sperm (Platinum) users.", IT_MGR::CHAT_TYPE_SYSTEM );
		CGame::GetInstance().SetPayType( m_pRecvPacket->m_gsv_BILLING_MESSAGE.m_btTYPE );
		break;
	case BILLING_MSG_PAY_FQ:
		g_itMGR.AppendChatMsg( "Quantification of individual users",IT_MGR::CHAT_TYPE_SYSTEM );
		CGame::GetInstance().SetPayType( m_pRecvPacket->m_gsv_BILLING_MESSAGE.m_btTYPE );
		break;
	case BILLING_MSG_PAY_GU:
		g_itMGR.AppendChatMsg( "Games have been connected as", IT_MGR::CHAT_TYPE_SYSTEM );
		CGame::GetInstance().SetPayType( m_pRecvPacket->m_gsv_BILLING_MESSAGE.m_btTYPE );
		break;
	case BILLING_MSG_PAY_GQ:
		g_itMGR.AppendChatMsg( "Games of the billing period (time) has expired, the person was connected to quantify", IT_MGR::CHAT_TYPE_SYSTEM );
		CGame::GetInstance().SetPayType( m_pRecvPacket->m_gsv_BILLING_MESSAGE.m_btTYPE );
		break;
	case BILLING_MSG_PAY_IQ:
		g_itMGR.AppendChatMsg( "Games beyond the individual's access to IP numbers were quantified by connecting",IT_MGR::CHAT_TYPE_SYSTEM );
		CGame::GetInstance().SetPayType( m_pRecvPacket->m_gsv_BILLING_MESSAGE.m_btTYPE );
		break;
	case BILLING_MSG_KICKOUT_CHEAT_DETECT:
		g_itMGR.AppendChatMsg( STR_BILLING_MSG_KICKOUT_CHEAT_DETECT ,IT_MGR::CHAT_TYPE_SYSTEM);
		break;
	case BILLING_MSG_KICKOUT_ALREADY_LOGIN:
		g_itMGR.AppendChatMsg( STR_BILLING_MSG_KICKOUT_ALREADY_LOGIN ,IT_MGR::CHAT_TYPE_SYSTEM);
		break;
	case BILLING_MSG_KICKOUT_TIME_EXPIRED:
		g_itMGR.AppendChatMsg( STR_BILLING_MSG_KICKOUT_TIME_EXPIRED ,IT_MGR::CHAT_TYPE_SYSTEM);
		break;
	case BILLING_MSG_KICKOUT_DUP_LOGIN:
		g_itMGR.AppendChatMsg( STR_BILLING_MSG_KICKOUT_DUP_LOGIN ,IT_MGR::CHAT_TYPE_SYSTEM);
		break;
	case BILLING_MSG_KICKOUT_NO_RIGHT:
		g_itMGR.AppendChatMsg( STR_BILLING_MSG_KICKOUT_NO_RIGHT ,IT_MGR::CHAT_TYPE_SYSTEM);
		break;
	case BILLING_MSG_KICKOUT_OUT_OF_IP:
		g_itMGR.AppendChatMsg( STR_BILLING_MSG_KICKOUT_OUT_OF_IP ,IT_MGR::CHAT_TYPE_SYSTEM);
		break;
	case BILLING_MSG_KICKOUT_EXIST_ACCOUNT:
		g_itMGR.AppendChatMsg( STR_BILLING_MSG_KICKOUT_EXIST_ACCOUNT ,IT_MGR::CHAT_TYPE_SYSTEM);
		break;
	case BILLING_MSG_TYPE_SET_DATE:///Expiration date
		{
			if( CGame::GetInstance().GetDetailPayType() != BILLING_MSG_PAY_FU )
			{
				short nOffset = sizeof( gsv_BILLING_MESSAGE );

				if( char* pszRemainTime = Packet_GetStringPtr( m_pRecvPacket, nOffset ) )
				{
					std::string strTime = pszRemainTime;
					

					if( strTime.size() >= 12 )
					{
						int year  = atoi( strTime.substr( 0, 4 ).c_str() );
						int month = atoi( strTime.substr( 4, 2 ).c_str() );
						int day   = atoi( strTime.substr( 6, 2 ).c_str() );
						int hour  = atoi( strTime.substr( 8, 2 ).c_str() );
						int min   = atoi( strTime.substr( 10, 2 ).c_str() );

						//Honggeun lifetime free users.
						if(year <= 1970)
						{
							break;
						}

						g_itMGR.AppendChatMsg( CStr::Printf("%s:%d/%d/%d %d:%02d", STR_EXPIRED_TIME, year, month, day, hour, min ), IT_MGR::CHAT_TYPE_SYSTEM );
					}

					CGame::GetInstance().SetExpireTime( pszRemainTime );
					CGame::GetInstance().SetExpireTimeType( 1 );
				}
			}
		}
		break;
	case BILLING_MSG_TYPE_SET_TIME:///The remaining minutes
		{
			short nOffset = sizeof( gsv_BILLING_MESSAGE );
			if( char* pszRemainTime = Packet_GetStringPtr( m_pRecvPacket, nOffset ) )
			{
				CGame::GetInstance().SetExpireTime( pszRemainTime );
				CGame::GetInstance().SetExpireTimeType( 2 );
				DWORD time = atoi( pszRemainTime );
				g_itMGR.AppendChatMsg( CStr::Printf(STR_GAME_TIME_LEFT_MINS, time ), IT_MGR::CHAT_TYPE_SYSTEM );
			}
		}
		break;
	case BILLING_MSG_TYPE_TIME_ALERT:///The remaining minutes
		{
			short nOffset = sizeof( gsv_BILLING_MESSAGE );
			if( char* pszRemainTime = Packet_GetStringPtr( m_pRecvPacket, nOffset ) )
			{
				CGame::GetInstance().SetExpireTime( pszRemainTime );
				CGame::GetInstance().SetExpireTimeType( 2 );

				DWORD time = atoi( pszRemainTime );
				g_itMGR.AppendChatMsg( CStr::Printf( STR_GAME_TIME_LEFT_MINS, time ), IT_MGR::CHAT_TYPE_ERROR );
			}
		}
		break;
	case BILLING_MSG_TYPE_TAIWAN:
		{
			short nOffset = sizeof( gsv_BILLING_MESSAGE );
			if( char* pszMsg = Packet_GetStringPtr( m_pRecvPacket, nOffset ) )
				g_itMGR.AppendChatMsg( pszMsg, IT_MGR::CHAT_TYPE_SYSTEM );
		}
		break;
	case BILLING_MSG_KICKOUT_TAIWAN:
		{
			//g_itMGR.AppendChatMsg( "Disconnect From Billing Server", IT_MGR::CHAT_TYPE_SYSTEM );
		}
		break;
	default:
		break;
	}
}

void CRecvPACKET::Recv_gsv_MOVE_ZULY()
{
	if( g_pAVATAR )
	{
		CBank::GetInstance().SetMoney( m_pRecvPacket->m_gsv_MOVE_ZULY.m_i64BankZuly );
		g_pAVATAR->Set_MONEY( m_pRecvPacket->m_gsv_MOVE_ZULY.m_i64InvZuly );
	}	
}


//----------------------------------------------------------------------------------------------------	
///Ji Ho :: Goddess Summon Night packet processing
///Choi Axial :: - stats financial postpartum save changes: 2005/7/12: nAvy
///           - New version of the packets are coming home only
void CRecvPACKET::Recv_gsv_GODDNESS_MODE()
{

	LOGOUT( "Goddess summoned by the command On/Off [ %d ]",m_pRecvPacket->m_gsv_GODDNESS_MODE.m_btOnOff);
	goddessMgr.SetProcess(m_pRecvPacket->m_gsv_GODDNESS_MODE.m_btOnOff,
		m_pRecvPacket->m_gsv_GODDNESS_MODE.m_wObjectIDX);


	//short	client_object_index = g_pObjMGR->Get_ClientObjectIndex( m_pRecvPacket->m_gsv_GODDNESS_MODE.m_wObjectIDX);
	//if( client_object_index == g_pAVATAR->Get_INDEX() )
	//	g_pAVATAR->Calc_AruaAddAbility();


}

void CRecvPACKET::Recv_gsv_PATSTATE_CHANGE()
{
	_RPT1( _CRT_WARN,"m_gsv_PATSTATE_CHANGE.m_btOnOff is %d\n", m_pRecvPacket->m_gsv_PATSTATE_CHANGE.m_btOnOff );

	if( g_pAVATAR && g_pObjMGR && g_pObjMGR->Get_ServerObjectIndex( g_pAVATAR->Get_INDEX() ) == m_pRecvPacket->m_gsv_PATSTATE_CHANGE.m_wObjectIDX )
	{
		g_pAVATAR->SetCur_PatHP( m_pRecvPacket->m_gsv_PATSTATE_CHANGE.m_nMaxPatHP );
		g_pAVATAR->SetCur_PatCoolTIME( m_pRecvPacket->m_gsv_PATSTATE_CHANGE.m_dwCoolTIME );

		///Bakjiho :: boarding skills on the set time.	
		if( 0 == m_pRecvPacket->m_gsv_PATSTATE_CHANGE.m_btOnOff )
		{
			if( g_SkillList.Get_Cart_SkillIndex()>0 )
			{
				g_SoloSkillDelayTick.SetUseItemDelay(	g_SkillList.Get_Cart_SkillIndex(),
														g_SkillList.GetDelayTickCount( g_SkillList.Get_Cart_SkillIndex() ) );				
			}			
		}

	}
}


void CRecvPACKET::Recv_gsv_CHARSTATE_CHANGE()
{
	CObjAVT *pNewAVT = g_pObjMGR->Get_ClientCharAVT( m_pRecvPacket->m_gsv_CHARSTATE_CHANGE.m_wObjectIDX, true ); 
	if ( pNewAVT ) 
	{
		//------------------------------------------------------------------------------------
		/// Special status flag
		//------------------------------------------------------------------------------------
		pNewAVT->ChangeSpecialState( m_pRecvPacket->m_gsv_CHARSTATE_CHANGE.m_dwFLAG );		
	}
}

// 2 seater cart.
void CRecvPACKET::Recv_gsv_CART_RIDE()
{
//06. 04. 18 - Kim, Joo - Hyun: overflow-related bug fixes invitation to cart.

if( g_pAVATAR->m_bPersonalStoreMode == false)
{
	switch( m_pRecvPacket->m_gsv_CART_RIDE.m_btType )
	{
	case CART_RIDE_REQ:				// Do you want a ride? only sent m_wGuestObjIDX
		{ 
			if( g_pAVATAR->GetCartInvitationWnd() == false )
			{
				g_pAVATAR->SetCartInvitationWnd(true);

				//Find Index Owner.
				
				int iOwnerObjClientIndex = g_pObjMGR->Get_ClientObjectIndex( m_pRecvPacket->m_gsv_CART_RIDE.m_wOwnerObjIDX );
				CObjAVT* pAVT = g_pObjMGR->Get_CharAVT( iOwnerObjClientIndex, true );

				if( pAVT == NULL)
				{
					return;
				}	

				// % s has required a ride. Would you like to ride?
				sprintf( g_MsgBuf, STR_WANT_BOARDS, pAVT->Get_NAME() );

				CTCmdAcceptCartRide * pCmdOk	= new CTCmdAcceptCartRide( m_pRecvPacket->m_gsv_CART_RIDE.m_wOwnerObjIDX, m_pRecvPacket->m_gsv_CART_RIDE.m_wGuestObjIDX );
				CTCmdRejectCartRide * pCmdCancel	= new CTCmdRejectCartRide( m_pRecvPacket->m_gsv_CART_RIDE.m_wOwnerObjIDX, m_pRecvPacket->m_gsv_CART_RIDE.m_wGuestObjIDX );

				g_itMGR.OpenMsgBox_CartRide( g_MsgBuf, CMsgBox::BT_OK | CMsgBox::BT_CANCEL, false, 0, pCmdOk, pCmdCancel , CMsgBox::MSGTYPE_RECV_CART_RIDE_REQ,
					m_pRecvPacket->m_gsv_CART_RIDE.m_wOwnerObjIDX,
					m_pRecvPacket->m_gsv_CART_RIDE.m_wGuestObjIDX );
			}
		}
		break;	

	case CART_RIDE_ACCEPT:			// Carrick on talkke sent all around
		{
			g_pAVATAR->SetCartInvitationWnd(false);
			// If you can not use the boarding place.
			// STR_BOARDING_CANT_USE

			// You can not ride.
			// STR_CANT_BOARD
			//-------------------------------------------------------------------------------------------------------------------
			//박지호::2인승 탑승 
			CObjAVT *oSrc = g_pObjMGR->Get_ClientCharAVT( m_pRecvPacket->m_gsv_CART_RIDE.m_wOwnerObjIDX, true ); 
			if(oSrc == NULL)
				return;

			oSrc->SetRideUser(m_pRecvPacket->m_gsv_CART_RIDE.m_wGuestObjIDX);
			//-------------------------------------------------------------------------------------------------------------------

			//20050901 Honggeun only rider in the passenger seat two-seater cart.
			int iGuestObjClientIndex = g_pObjMGR->Get_ClientObjectIndex( m_pRecvPacket->m_gsv_CART_RIDE.m_wGuestObjIDX );
			CObjAVT* pAVT = g_pObjMGR->Get_CharAVT( iGuestObjClientIndex, true );
			if( pAVT && pAVT->Get_INDEX() == g_pAVATAR->Get_INDEX() )
			{				
				g_pAVATAR->Set_Block_CartRide( true );
			}
		}

		break;

	case CART_RIDE_REFUSE:			// Not only sent you getting m_wOwnerObjIDX
		{
			g_pAVATAR->SetCartInvitationWnd(false);
			//Find Index Owner.
			int iGuestObjClientIndex = g_pObjMGR->Get_ClientObjectIndex( m_pRecvPacket->m_gsv_CART_RIDE.m_wGuestObjIDX );
			int iOwnerObjClientIndex = g_pObjMGR->Get_ClientObjectIndex( m_pRecvPacket->m_gsv_CART_RIDE.m_wOwnerObjIDX );

			CObjAVT* pAVT = g_pObjMGR->Get_CharAVT( iGuestObjClientIndex, true );

			if( pAVT == NULL )
			{
				return;
			}	

			// Owner only to send a message.
			if( iOwnerObjClientIndex == g_pAVATAR->Get_INDEX() )
			{
				// % s has been refused boarding.	
				g_itMGR.AppendChatMsg(	CStr::Printf( STR_REFUSE_BOARD, pAVT->Get_NAME() ),
					IT_MGR::CHAT_TYPE_SYSTEM, D3DCOLOR_ARGB( 255, 206, 223, 136) );
			}

			//-------------------------------------------------------------------------------------------------------------------
			//Denied boarding animation bakjiho ::
			pAVT->Ride_Cansel_Motion();	
			//-------------------------------------------------------------------------------------------------------------------
		}
		break;

	case CART_RIDE_OWNER_NOT_FOUND:// Dawn gives a ride Carrick disappeared
		// You can not ride.
		g_itMGR.AppendChatMsg(STR_CANT_BOARD, IT_MGR::CHAT_TYPE_SYSTEM, D3DCOLOR_ARGB( 255, 206, 223, 136) );
		break;

	case CART_RIDE_GUEST_NOT_FOUND:// Carrick tried burning disappeared
		// You can not ride.
		g_itMGR.AppendChatMsg(STR_CANT_BOARD, IT_MGR::CHAT_TYPE_SYSTEM, D3DCOLOR_ARGB( 255, 206, 223, 136) );
		break;	
	}
}
}

void  CRecvPACKET::Recv_gsv_SCREEN_SHOT_TIME()
{
	SYSTEMTIME SysTime;
	SysTime.wYear = m_pRecvPacket->m_gsv_SCREEN_SHOT_TIME.wYear;
	SysTime.wMonth = m_pRecvPacket->m_gsv_SCREEN_SHOT_TIME.btMonth;
	SysTime.wDay = m_pRecvPacket->m_gsv_SCREEN_SHOT_TIME.btDay;
	SysTime.wHour = m_pRecvPacket->m_gsv_SCREEN_SHOT_TIME.btHour;
	SysTime.wMinute = m_pRecvPacket->m_gsv_SCREEN_SHOT_TIME.btMin;

	CGame::GetInstance().ScreenCAPTURE(SysTime);

}

void  CRecvPACKET::Recv_gsv_WARNING_MSGCODE()
{
	switch(m_pRecvPacket->m_gsv_WARNING_MSGCODE.m_dwWarningCode)
	{
	case EGSV_PARTY_EXP_LIMIT:
		{
			// Due to differences in the level of the player and the monster failed to acquire experience.
			g_itMGR.AppendChatMsg( LIST_STRING(455), IT_MGR::CHAT_TYPE_SYSTEM, D3DCOLOR_ARGB( 255, 206, 223, 136) );
		}
		break;
	case EGSV_DONT_DUPLICATE_EXPERIENCE_MILEAGE_ITEM_USE:
		{
			// "Can not be combined." 
			g_itMGR.AppendChatMsg( LIST_STRING(249), IT_MGR::CHAT_TYPE_SYSTEM, D3DCOLOR_ARGB( 255, 206, 223, 136) );
#ifdef __EXPERIENCE_ITEM
			g_pNet->Send_cli_MILEAGE_ITEM_TIME(4); //4 - Experience the item.
#endif
		}
		break;
	case EGSV_EXPERIENCE_MILEAGE_ITEM_EXPIRED:
		{
			// "Experience the item has expired." 
			g_itMGR.AppendChatMsg( LIST_STRING(956), IT_MGR::CHAT_TYPE_SYSTEM, D3DCOLOR_ARGB( 255, 206, 223, 136) );
		}
		break;
	}	
}

void CRecvPACKET::Recv_gsv_Cli_USER_MSG_1LV()
{
	switch(m_pRecvPacket->m_gsv_User_MSG_1LV.m_Sub_Type)
	{
	case 1:
		{
			g_pAVATAR->m_bDead = true;
			uniDAMAGE sDamage;
			sDamage.m_wDamage = g_pAVATAR->Get_MaxHP();
			sDamage.m_wACTION = DMG_ACT_DEAD;			
			g_pAVATAR->Apply_DAMAGE (NULL,  sDamage);	
			g_pAVATAR->m_lDeadTIME = g_GameDATA.GetGameTime();
		}
		break;
	
	case 100:
		{
#ifdef __NPC_COLLISION

			if(m_pRecvPacket->m_gsv_User_MSG_1LV.m_Data == 0)
			{
				//Closed state
				g_pObjMGR->ClosedCollisionNPCMotion();				

#ifdef _DEBUG
				g_itMGR.AppendChatMsg( "The door is closed while the", IT_MGR::CHAT_TYPE_SYSTEM, D3DCOLOR_ARGB( 255, 206, 223, 136) );
				ClientLog(LOG_DEBUG,"The door is closed while the", IT_MGR::CHAT_TYPE_SYSTEM, D3DCOLOR_ARGB( 255, 206, 223, 136) );
#endif				
			
			}
			else if(m_pRecvPacket->m_gsv_User_MSG_1LV.m_Data == 1)
			{
				//Held in
				g_pObjMGR->OpeningCollisionNPCMotion();				

#ifdef _DEBUG
				g_itMGR.AppendChatMsg( "Of the door is open", IT_MGR::CHAT_TYPE_SYSTEM, D3DCOLOR_ARGB( 255, 206, 223, 136) );
				ClientLog(LOG_DEBUG,"Of the door is open", IT_MGR::CHAT_TYPE_SYSTEM, D3DCOLOR_ARGB( 255, 206, 223, 136) );
#endif				
			}
			else if(m_pRecvPacket->m_gsv_User_MSG_1LV.m_Data == 2)
			{
				//Open state.
				g_pObjMGR->OpenedCollisionNPCMotion();		

#ifdef _DEBUG
				g_itMGR.AppendChatMsg( "Door is closed in", IT_MGR::CHAT_TYPE_SYSTEM, D3DCOLOR_ARGB( 255, 206, 223, 136) );
				ClientLog(LOG_DEBUG,"Door is closed in", IT_MGR::CHAT_TYPE_SYSTEM, D3DCOLOR_ARGB( 255, 206, 223, 136) );
#endif			
			}
#endif	
		}
	}
	
}
#ifdef __EXPERIENCE_ITEM
void CRecvPACKET::Recv_gsv_MALL_ITEM_TIME()
{
	DWORD dwIndex = m_pRecvPacket->m_gsv_MIELAGE_ITEM_TIME.dwIndex;

	SYSTEMTIME SysTime;
	SysTime.wYear = m_pRecvPacket->m_gsv_MIELAGE_ITEM_TIME.wYear;
	SysTime.wMonth = m_pRecvPacket->m_gsv_MIELAGE_ITEM_TIME.btMonth;
	SysTime.wDay = m_pRecvPacket->m_gsv_MIELAGE_ITEM_TIME.btDay;
	SysTime.wHour = m_pRecvPacket->m_gsv_MIELAGE_ITEM_TIME.btHour;
	SysTime.wMinute = m_pRecvPacket->m_gsv_MIELAGE_ITEM_TIME.btMin;

	if(dwIndex < 0)
	{
#ifdef _DEBUG
		g_itMGR.AppendChatMsg( CStr::Printf("Debug : Mall Item Index is less than zero! dwIndex =  %d",dwIndex), IT_MGR::CHAT_TYPE_SYSTEM, D3DCOLOR_ARGB( 255, 206, 223, 136) );
#endif
		return;
	}

	// 06. 12. 05 - Kim, Joo - Hyun: +35 4 changes to the index is calculated. Note from t_AbilityINDEX 94 times.
	if(dwIndex > 3)
	{
		dwIndex += 35;
	}

	char* pszName =  CStringManager::GetSingleton().GetAbility( dwIndex +AT_MAINTAIN_ABILITY );

	if(SysTime.wYear == 0)
	{
#ifdef _DEBUG
		g_itMGR.AppendChatMsg( CStr::Printf("Debug : %s Items are items that have already expired. Remaining time 0!!",pszName), IT_MGR::CHAT_TYPE_SYSTEM, D3DCOLOR_ARGB( 255, 206, 223, 136) );
#endif
		return;
	}
	
	g_itMGR.AppendChatMsg( CStr::Printf(STR_ITEM_TIME_OUT, pszName, SysTime.wYear, SysTime.wMonth, SysTime.wDay, SysTime.wHour, SysTime.wMinute ), IT_MGR::CHAT_TYPE_SYSTEM, D3DCOLOR_ARGB( 255, 206, 223, 136));
}

#endif



void CRecvPACKET::Recv_gsv_Server_Time()
{	
	__int64 i64ServerTime = m_pRecvPacket->m_gsv_Server_Time.m_ServerTime;
	__int64 i64ClientTime;
	_time64( &i64ClientTime );

	int iServerGap = i64ServerTime - i64ClientTime;

	struct tm tmServerLocalTime;
	tmServerLocalTime.tm_year	= m_pRecvPacket->m_gsv_Server_Time.wYear;
	tmServerLocalTime.tm_mon	= m_pRecvPacket->m_gsv_Server_Time.wMonth;
	tmServerLocalTime.tm_mday	= m_pRecvPacket->m_gsv_Server_Time.wDay;
	tmServerLocalTime.tm_hour	= m_pRecvPacket->m_gsv_Server_Time.wHour;
	tmServerLocalTime.tm_min	= m_pRecvPacket->m_gsv_Server_Time.wMin;
	tmServerLocalTime.tm_sec	= m_pRecvPacket->m_gsv_Server_Time.wSec;
		
	time_t ServerLocalTime_t = mktime( &tmServerLocalTime );

	g_GameDATA.m_dwServerTimeGap = (ServerLocalTime_t - i64ClientTime);

}

void CRecvPACKET::Recv_srv_LOGIN_REPLY2()
{
	//char sztxtAccount[36];
	//char szMD5PW[33];
	g_GameDATA.m_Account.Set(m_pRecvPacket->m_srv_LOGIN_REPLY2.sztxtAccount);
	ZeroMemory( g_GameDATA.m_PasswordMD5, sizeof(g_GameDATA.m_PasswordMD5) );
	CopyMemory( g_GameDATA.m_PasswordMD5, m_pRecvPacket->m_srv_LOGIN_REPLY2.szMD5PW, sizeof(g_GameDATA.m_PasswordMD5) );


}

void pickup(){
	WORD wFieldItem = pickupitem;
	Sleep(4000);
	g_pNet->Send_cli_GET_FIELDITEM_REQ( wFieldItem );
}

