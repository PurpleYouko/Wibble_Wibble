/*
	$Header: /Client/Network/CNetwork.cpp 200   07-03-20 3:03p Raning $
*/
#include "stdAFX.h"
#include "CNetwork.h"
#include "Game.h"
#include "IO_Terrain.h"
#include "../Interface/ExternalUI/ExternalUILobby.h"
#include "../Interface/it_mgr.h"
#include "../System/CGame.h"
#include "../util/CSocketWND.h"
#include "../gameproc/LiveCheck.h"
#include "Debug.h"


CNetwork *g_pNet;

CNetwork *CNetwork::m_pInstance = NULL;

CRITICAL_SECTION	g_csThread;

#define	WM_WORLD_SOCKET_NOTIFY		( WM_SOCKETWND_MSG+0 )
#define	WM_ZONE_SOCKET_NOTIFY		( WM_SOCKETWND_MSG+1 )

/*
void	(*fpCMDProc )	(t_unit *pUnit);
typedef	unsigned int UINT;
*/

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
CNetwork::CNetwork (HINSTANCE hInstance)
{
	m_btZoneSocketSTATUS = 0;
	m_nProcLEVEL = NS_NULL;

	CSocketWND *pSockWND = CSocketWND::InitInstance( hInstance, 2 );
	if ( pSockWND ) 
	{
		pSockWND->AddSocket( &this->m_WorldSOCKET, WM_WORLD_SOCKET_NOTIFY );
		pSockWND->AddSocket( &this->m_ZoneSOCKET, WM_ZONE_SOCKET_NOTIFY );
	}

	::InitializeCriticalSection( &g_csThread );

}
CNetwork::~CNetwork ()
{
	CSOCKET::Free ();

	if ( CSocketWND::GetInstance() )
		CSocketWND::GetInstance()->Destroy();

	
}

CNetwork *CNetwork::Instance (HINSTANCE hInstance)
{
	if ( m_pInstance == NULL ) {
		if ( CSOCKET::Init () ) {
			m_pInstance = new CNetwork( hInstance );
		}
	}

	return m_pInstance;
}

void CNetwork::Destroy ()
{
	::DeleteCriticalSection( &g_csThread );

	m_WorldSOCKET.Close ();
	m_ZoneSOCKET.Close ();
	SAFE_DELETE( m_pInstance );
}


void CNetwork::Send_PACKET( t_PACKET *pSendPacket, bool bSendToWorld )
{

#ifdef	__VIRTUAL_SERVER

	m_ZoneSOCKET.Packet_Register2RecvQ( pSendPacket );

#else
	//PY.   for no encryption at all. We just #define __USE_WITHIN_COMPANY_TEST. Could also be done in StdAfx.h
#define __USE_WITHIN_COMPANY_TEST
	
	
	//PY packet logging
	time_t rtime;
    time(&rtime);
    char *timestamp = ctime(&rtime);
    timestamp[ strlen(timestamp)-1 ] = ' ';
	FILE *fh = fopen(  "log/InOutpackets.log", "a+" );

	if ( fh != NULL) 
	{
		fprintf( fh, "%s- OUT %04x: ", timestamp, pSendPacket->m_HEADER.m_wType );
		for ( int i=0; i<pSendPacket->m_HEADER.m_nSize; ++i ) 
		{
			fprintf( fh, "%02x ", (unsigned char)pSendPacket->m_pDATA[i] );
			if(i == 5) fprintf( fh, ":");	// add a colon after header data
		}
		fprintf( fh, "\n" );
		fclose( fh );
	}
	//PY end
	


	if ( bSendToWorld || bAllInONE )
	{
#ifdef __ARCFOUR		

#ifdef __ARCFOURLOG
		FILE * pFile = fopen("ArcfourLog.txt", "at");
		if(pFile)	fprintf(pFile, "Encrypt before = size:%d, type:0x%04x\n", pSendPacket->m_HEADER.m_nSize, pSendPacket->m_HEADER.m_wType);
#endif		

		unsigned char * p = (unsigned char*)pSendPacket;

		

		//_PACKETRESERVED Should contain an encrypted packet
		//pSendPacket->m_HEADER.m_wReserved = _PACKETRESERVED;

		::EnterCriticalSection( &g_csThread );

#ifdef __ARCFOURLOG
		for (int iip = 0; iip < pSendPacket->m_HEADER.m_nSize; ++iip)
		{
			fprintf(pFile, "%02x ", ((unsigned char*)p)[iip]);
		}
		fprintf(pFile, "\n");
#endif		


// Use in-house or not.
#ifndef __USE_WITHIN_COMPANY_TEST
#ifdef __ENCRYPT2
		//for(int i=2; i < (pSendPacket->m_HEADER.m_nSize - 2); i++)		p[i] = p[i] ^ 'a';
		for(int i=2; i < (pSendPacket->m_HEADER.m_nSize - 2); i++)		p[i] = p[i] ^ 'a' ^ 0x13;
#else
		m_WorldSOCKET.cipher_encrypt( p+2, pSendPacket->m_HEADER.m_nSize - sizeof(short) );
#endif
#endif


#ifdef __ARCFOURLOG
		for (int iip = 0; iip < pSendPacket->m_HEADER.m_nSize; ++iip)
		{
			fprintf(pFile, "%02x ", ((unsigned char*)p)[iip]);
		}
		fprintf(pFile, "\n");

			if(pFile)	fprintf(pFile, "Encrypt after = size:%d, type:0x%04x\n", pSendPacket->m_HEADER.m_nSize, pSendPacket->m_HEADER.m_wType);
			if(pFile)	fprintf(pFile, "===================================================================\n");

			fclose(pFile);
#endif

#endif

		//send the packet here
		m_WorldSOCKET.Packet_Register2SendQ( pSendPacket );		
			
		::LeaveCriticalSection( &g_csThread );
	}
	else
	{
#ifdef __ARCFOUR		


#ifdef __ARCFOURLOG
		FILE * pFile = fopen("ArcfourLog.txt", "at");
		if(pFile)	fprintf(pFile, "Encrypt before = size:%d, type:0x%04x\n", pSendPacket->m_HEADER.m_nSize, pSendPacket->m_HEADER.m_wType);
#endif		

		unsigned char * p = (unsigned char*)pSendPacket;

		// _PACKETRESERVED Should contain an encrypted packet
		pSendPacket->m_HEADER.m_wReserved = _PACKETRESERVED;

#ifdef __ARCFOURLOG
		for (int iip = 0; iip < pSendPacket->m_HEADER.m_nSize; ++iip)
		{
			fprintf(pFile, "%02x ", ((unsigned char*)p)[iip]);
		}
		fprintf(pFile, "\n");
#endif

		::EnterCriticalSection( &g_csThread );	

#ifndef __USE_WITHIN_COMPANY_TEST
#ifdef __ENCRYPT2
		//for(int i=2; i < (pSendPacket->m_HEADER.m_nSize - 2); i++)		p[i] = p[i] ^ 'a';
		for(int i=2; i < (pSendPacket->m_HEADER.m_nSize - 2); i++)		p[i] = p[i] ^ 'a' ^ 0x13;
		
#else
		m_ZoneSOCKET.cipher_encrypt( p+2, pSendPacket->m_HEADER.m_nSize - sizeof(short) );	
#endif
#endif

#ifdef __ARCFOURLOG
		for (int iip = 0; iip < pSendPacket->m_HEADER.m_nSize; ++iip)
		{
			fprintf(pFile, "%02x ", ((unsigned char*)p)[iip]);
		}
		fprintf(pFile, "\n");

		if(pFile)	fprintf(pFile, "Encrypt after = size:%d, type:0x%04x\n", pSendPacket->m_HEADER.m_nSize, pSendPacket->m_HEADER.m_wType);
		if(pFile)	fprintf(pFile, "===================================================================\n");

		fclose(pFile);
#endif


		
#endif

		m_ZoneSOCKET.Packet_Register2SendQ( pSendPacket );

		::LeaveCriticalSection( &g_csThread );
	}
		
#endif
}

//-------------------------------------------------------------------------------------------------
bool CNetwork::ConnectToServer (char *szServerIP, WORD wTcpPORT, short nProcLEVEL)
{
	// World Socket ...
	if( m_nProcLEVEL == nProcLEVEL )
		return true;	

	m_nProcLEVEL = nProcLEVEL;


#ifdef __ARCFOUR
	
#ifdef __ARCFOURLOG
	FILE * pFile = fopen("ArcfourLog.txt", "at");
	if(pFile)	fprintf(pFile, "m_WorldSOCKET Cipher Init\n\n");
	fclose(pFile);
#endif

//	m_WorldSOCKET.cipher_init( g_pCApp->GetArcfourKey(), g_pCApp->GetArcfourKeySize() );

#endif

	return m_WorldSOCKET.Connect( CSocketWND::GetInstance()->GetWindowHandle(), szServerIP, wTcpPORT, WM_WORLD_SOCKET_NOTIFY );
}

//-------------------------------------------------------------------------------------------------

void CNetwork::DisconnectFromServer (short nProcLEVEL)
{
	m_nProcLEVEL = nProcLEVEL;
	m_WorldSOCKET.Close ();
	m_WorldSOCKET.Set_NetSTATUS( NETWORK_STATUS_DISCONNECT );
}

//-------------------------------------------------------------------------------------------------
void CNetwork::MoveZoneServer ()
{
	if ( NETWORK_STATUS_CONNECT == m_btZoneSocketSTATUS ) {
		// John broke new connection to a server socket ...
		m_bMoveingZONE = true;
		m_ZoneSOCKET.Close();
	} else {
		// Directly connected ...
		m_bMoveingZONE = false;

#ifdef __ARCFOUR

#ifdef __ARCFOURLOG
		FILE * pFile = fopen("ArcfourLog.txt", "at");
		if(pFile)	fprintf(pFile, "m_ZoneSOCKET Cipher Init\n\n");
		fclose(pFile);
#endif

//		m_ZoneSOCKET.cipher_init( g_pCApp->GetArcfourKey(), g_pCApp->GetArcfourKeySize() );

#endif

		m_ZoneSOCKET.Connect( CSocketWND::GetInstance()->GetWindowHandle(), m_GSV_IP.Get(), m_wGSV_PORT, WM_ZONE_SOCKET_NOTIFY );
	}
}

//-------------------------------------------------------------------------------------------------
void CNetwork::Proc_WorldPacket ()
{
	CshoClientSOCK *pSocket = &this->m_WorldSOCKET;

	while( m_WorldSOCKET.Peek_Packet( m_pRecvPacket, true ) ) 
	{

		//PY packet logging. hopeully this is in the right place
		if(m_pRecvPacket->m_HEADER.m_wType != 0x700)
		{
			FILE *fh = fopen(  "log/InOutpackets.log", "a+" );

			time_t rtime;
			time(&rtime);
			char *timestamp = ctime(&rtime);
			timestamp[ strlen(timestamp)-1 ] = ' ';

			if ( fh != NULL) 
			{
				fprintf( fh, "%s- IN  %04x: ", timestamp, m_pRecvPacket->m_HEADER.m_wType );
				for ( int i=0; i<m_pRecvPacket->m_HEADER.m_nSize; ++i ) 
				{
					fprintf( fh, "%02x ", (unsigned char)m_pRecvPacket->m_pDATA[i] );
					if(i == 5) fprintf( fh, ":");	// add a colon after header data
				}
				fprintf( fh, "\n" );
				fclose( fh );
			}
		}
		//PY end

#ifdef __ARCFOUR
		//Dummy packet generation anti hacking
		//Chance to spend a pile.
		if( rand()%100 == 0 )
		{		
			Send_cli_DUMMY_1();		
		}
#endif
		switch( m_pRecvPacket->m_HEADER.m_wType ) 
		{
			case SOCKET_NETWORK_STATUS :
			{
				switch( m_pRecvPacket->m_NetSTATUS.m_btStatus ) {
					case NETWORK_STATUS_ACCEPTED    :
					{
						pSocket->OnAccepted ( (int*)m_pRecvPacket->m_NetSTATUS.m_dwSocketIDs );
						CGame::GetInstance().AcceptedConnectLoginSvr();
						continue;
					}
					case NETWORK_STATUS_CONNECT		:
					{
						// Was the connection with the server ...
						switch( m_nProcLEVEL ) {
							case NS_CON_TO_WSV :	// Was connected to the server on the World.
								Send_cli_JOIN_SERVER_REQ (m_dwWSV_ID, true);
								m_bWarping = false;
								bAllInONE = true;
								break;
							case NS_CON_TO_LSV :	// Attempt to connect to a login server.
								{
									pSocket->mF_Init( 0 );
									Send_cli_HEADER( CLI_ACCEPT_REQ ,true );
#ifdef __ARCFOUR
									//Dummy packet generation anti hacking
									//Chance to spend a pile.
									//if( rand()%2 == 0 )									
									{		
										Send_cli_DUMMY_1();		//sends a stupid 0x808 packet that means nothing at all
									}
#endif
								}
								break;
						}
						if(!CGame::GetInstance().GetCurVersion().empty())
							g_pCApp->SetCaption( CStr::Printf("%s [Ver. %s]", __BUILD_REGION_NAME, CGame::GetInstance().GetCurVersion().c_str()) );
						else
							g_pCApp->SetCaption( __BUILD_REGION_NAME);

					#ifdef	__VIRTUAL_SERVER
						g_pCApp->ErrorBOX( "Virtual server that has been set..", "ERROR !!!", MB_OK);
					#endif
						continue;
					}
					case NETWORK_STATUS_DISCONNECT:
					{
						// g_pCApp->SetCaption ( "Disconnected" );

						if ( NS_DIS_FORM_LSV  == m_nProcLEVEL ) {
							// Reconnected to the game server ...
							this->ConnectToServer( m_WSV_IP.Get(), m_wWSV_PORT, NS_CON_TO_WSV );
							continue;
						}
						CGame::GetInstance().ProcWndMsg( WM_USER_WORLDSERVER_DISCONNECTED,0,0 );
						//SendServerDisconnectMsg( "WorldServer" );
						break;
					}
					case NETWORK_STATUS_DERVERDEAD	:
					{
						g_pCApp->SetCaption ( "Server DEAD" );
					#ifndef	__VIRTUAL_SERVER
						g_pCApp->ErrorBOX( STR_SERVER_EXAMINE, STR_SERVER_INFO, MB_OK);
						g_pCApp->SetExitGame();
					#endif
					break;
					}
				}

				//CGame::GetInstance().ProcWndMsg( WM_USER_SERVER_DISCONNECTED,0,0 );
				// Connection with the server failed.
				ClientLog (LOG_NORMAL,"Failed to connect with the server.\n");
				break;
			}
			case SRV_ERROR :
				Recv_srv_ERROR ();
				break;

			case SRV_JOIN_SERVER_REPLY :	// Was connected to the server on the World.
			{
				DWORD dwRet = Recv_srv_JOIN_SERVER_REPLY ();
				if ( dwRet ) {
					pSocket->OnAccepted( (int*)&dwRet );
					this->Send_cli_CHAR_LIST ();
				} else {
					// TODO:: error
					this->DisconnectFromServer ();
					return;
				}
				break;
			}

			case LSV_LOGIN_REPLY :
				if ( !Recv_lsv_LOGIN_REPLY () ) 
				{
					this->DisconnectFromServer ();
					return;
				}
				break;

			case LSV_LOGIN_REPLY2:
				Recv_srv_LOGIN_REPLY2();
				break;

			case LSV_SELECT_SERVER :
				{
					DWORD dwRet = Recv_lsv_SELECT_SERVER ();
					if ( dwRet )
						pSocket->mF_Init( dwRet );
					break;
				}
			case LSV_CHANNEL_LIST_REPLY :
				Recv_lsv_CHANNEL_LIST_REPLY ();
				break;
			// List of characters received
			case WSV_CHAR_LIST :
				Recv_wsv_CHAR_LIST ();			
				break;
			
			case WSV_DELETE_CHAR :
				Recv_wsv_DELETE_CHAR ();
				break;
			// Result of the character created by request
			case WSV_CREATE_CHAR :
				Recv_wsv_CREATE_CHAR (); 
					
				break;
			case WSV_MESSENGER:
				Recv_tag_MCMD_HEADER ();
				break;
			case WSV_MESSENGER_CHAT: 	
				Recv_wsv_MESSENGER_CHAT ();
				break;
			case WSV_MEMO:
				Recv_wsv_MEMO();
				break;
			case WSV_CHATROOM:
				Recv_wsv_CHATROOM();
				break;
			case WSV_CHATROOM_MSG:
				Recv_wsv_CHATROOM_MSG();
				break;
			case WSV_CHAR_CHANGE:
				Recv_wsv_CHAR_CHANGE();
				break;
			// John server moving, guys ...
			case WSV_MOVE_SERVER :
			{
				bAllInONE = false;
				Recv_wsv_MOVE_SERVER ();
				MoveZoneServer ();
				break;
			}

			case GSV_GODDNESS_MODE:
			{
				Recv_gsv_GODDNESS_MODE();
				break;
			}	

			default :
				Proc_ZonePacket ();
		}
	} 
}


void CNetwork::Proc_ZonePacket ()
{
	if(m_pRecvPacket->m_HEADER.m_wType != 0x700)
	{
		FILE *fh = fopen(  "log/InOutpackets.log", "a+" );

		time_t rtime;
		time(&rtime);
		char *timestamp = ctime(&rtime);
		timestamp[ strlen(timestamp)-1 ] = ' ';

		if ( fh != NULL) 
		{
			fprintf( fh, "%s- IN  %04x: ", timestamp, m_pRecvPacket->m_HEADER.m_wType );
			for ( int i=0; i<m_pRecvPacket->m_HEADER.m_nSize; ++i ) 
			{
				fprintf( fh, "%02x ", (unsigned char)m_pRecvPacket->m_pDATA[i] );
				if(i == 5) fprintf( fh, ":");	// add a colon after header data
			}
			fprintf( fh, "\n" );
			fclose( fh );
		}
	}
	switch( m_pRecvPacket->m_HEADER.m_wType ) 
	{
			case GSV_CART_RIDE:
				Recv_gsv_CART_RIDE();
				break;

			case GSV_PATSTATE_CHANGE:
				Recv_gsv_PATSTATE_CHANGE();
				break;

			case SRV_CHECK_AUTH :
				break;	
			case GSV_SET_NPC_SHOW :
				Recv_gsv_SET_NPC_SHOW ();
				break;
		
			case GSV_MOVE_ZULY:
				Recv_gsv_MOVE_ZULY();
				break;
			case WSV_CLANMARK_REG_TIME:
				Recv_wsv_CLANMARK_REG_TIME();
				break;
			case GSV_BILLING_MESSAGE:
				Recv_gsv_BILLING_MESSAGE();
				break;
			case GSV_BILLING_MESSAGE_EXT:
				Recv_gsv_BILLING_MESSAGE_EXT();
				break;
			case GSV_MALL_ITEM_REPLY:
				Recv_gsv_MALL_ITEM_REPLY();
				break;
			case GSV_ITEM_RESULT_REPORT:
				Recv_gsv_ITEM_RESULT_REPORT();
				break;
			case GSV_LOGOUT_REPLY:
				Recv_gsv_LOGOUT_REPLY();
				break;


			case WSV_CLAN_COMMAND :
				Recv_wsv_CLAN_COMMAND ();
				break;
			case WSV_CLAN_CHAT:
				Recv_wsv_CLAN_CHAT();
				break;
			case WSV_CLANMARK_REPLY:
				Recv_wsv_CLANMARK_REPLY();
				break;


			case SRV_ERROR :
				Recv_srv_ERROR ();
				break;

			case GSV_RELAY_REQ :
				this->Send_PACKET( m_pRecvPacket );
				break;

			case GSV_SET_GLOBAL_VAR :
				Recv_gsv_SET_GLOBAL_VAR ();
				break;

			case GSV_SET_GLOBAL_FLAG:
				Recv_gsv_SET_GLOVAL_FLAG();
				break;

			case SRV_ANNOUNCE_TEXT :
				Recv_srv_ANNOUNCE_TEXT ();
				break;
			case GSV_ANNOUNCE_CHAT :
				Recv_gsv_ANNOUNCE_CHAT ();
				break;

			case GSV_GM_COMMAND :
				Recv_gsv_GM_COMMAND ();
				break;

			// Result of the character selection by
			case GSV_SELECT_CHAR :
				Recv_gsv_SELECT_CHAR ();

				//Send_cli_JOIN_ZONE ();		// GSV_SELECT_CHAR
				break;

			case GSV_JOIN_ZONE :
				Recv_gsv_JOIN_ZONE ();
				break;


			case GSV_REVIVE_REPLY :
				Recv_gsv_REVIVE_REPLY ();
				break;

			case GSV_SET_VAR_REPLY :
				Recv_gsv_SET_VAR_REPLY ();
				break;

			case GSV_TELEPORT_REPLY :
				this->m_bWarping = false;
				Recv_gsv_TELEPORT_REPLY ();
//				Send_cli_JOIN_ZONE ();		// GSV_TELEPORT_REPLY
				break;

			case GSV_INVENTORY_DATA :
				Recv_gsv_INVENTORY_DATA ();
//				g_pCApp->SetStatus( AS_MAIN_GAME );
				break;
			case GSV_QUEST_DATA :
				Recv_gsv_QUEST_DATA();
				break;

			case GSV_QUEST_ONLY:
				Recv_gsv_QUEST_ONLY();
				break;

			case GSV_WISH_LIST:
				Recv_gsv_WISH_LIST();
				break;

			case GSV_MILEAGE_INV_DATA:                        //Mileage inventory data.
				break;

			case GSV_INIT_DATA :
				Recv_gsv_INIT_DATA ();
				break;

			case GSV_CHEAT_CODE :
				Recv_gsv_CHEAT_CODE ();
				break;

			case GSV_SET_MOTION :
				Recv_gsv_SET_MOTION ();
				break;

			case GSV_TOGGLE :
				Recv_gsv_TOGGLE ();
				break;
			case GSV_PARTY_CHAT:
				Recv_gsv_PARTY_CHAT();
				break;
			case GSV_CHAT :
				Recv_gsv_CHAT ();
				break;
			case GSV_WHISPER :
				Recv_gsv_WHISPER ();
				break;
			case GSV_SHOUT :
				Recv_gsv_SHOUT ();
				break;

			case GSV_NPC_CHAR :
				Recv_gsv_NPC_CHAR ();
				break;
			case GSV_MOB_CHAR :
				Recv_gsv_MOB_CHAR ();
				break;
			case GSV_AVT_CHAR :
				Recv_gsv_AVT_CHAR ();
				break;

			case GSV_SUB_OBJECT :
				Recv_gsv_SUB_OBJECT ();
				break;
			case GSV_STOP :
				Recv_gsv_STOP ();
				break;

			case GSV_MOVE :
				Recv_gsv_MOVE ();
				break;
			case GSV_ATTACK :
				Recv_gsv_ATTACK ();
				break;
			//case GSV_ATTACK_START :
			//	Recv_gsv_ATTACK_START ();
			//	break;

			case GSV_CHANGE_NPC :
				Recv_gsv_CHANGE_NPC ();
				break;

			case GSV_DAMAGE :
				Recv_gsv_DAMAGE ();
				break;

			case GSV_SETEXP:
				Recv_gsv_SETEXP();
				break;
			case GSV_LEVELUP :
				Recv_gsv_LEVELUP ();
				break;
	
			case GSV_HP_REPLY:
				Recv_gsv_HP_REPLY();
				break;

			case GSV_CHANGE_SKIN :
				Recv_gsv_CHANGE_SKIN ();
				break;

			case GSV_EQUIP_ITEM :
				Recv_gsv_EQUIP_ITEM ();
				break;

			case GSV_ADD_FIELDITEM :
				Recv_gsv_ADD_FIELDITEM ();
				break;

			case GSV_GET_FIELDITEM_REPLY :
				Recv_gsv_GET_FIELDITEM_REPLY ();
				break;
			case GSV_MOUSECMD :
				Recv_gsv_MOUSECMD ();
				break;

			case GSV_SET_WEIGHT_RATE :
				Recv_gsv_SET_WEIGHT_RATE ();
				break;

			case GSV_ADJUST_POS:
				Recv_gsv_ADJUST_POS();
				break;

			case GSV_SKILL_LEARN_REPLY :
				Recv_gsv_SKILL_LEARN_REPLY ();
				break;

			case GSV_SKILL_LEVELUP_REPLY :
				Recv_gsv_SKILL_LEVELUP_REPLY ();
				break;

			case GSV_SELF_SKILL :
				Recv_gsv_SELF_SKILL ();
				break;
			case GSV_TARGET_SKILL :
				Recv_gsv_TARGET_SKILL ();
				break;
			case GSV_POSITION_SKILL :
				Recv_gsv_POSITION_SKILL ();
				break;


			case GSV_EFFECT_OF_SKILL :
				Recv_gsv_EFFECT_OF_SKILL ();
				break;	
			case GSV_DAMAGE_OF_SKILL:
				Recv_gsv_DAMAGE_OF_SKILL ();
				break;
			case GSV_RESULT_OF_SKILL:
				Recv_gsv_RESULT_OF_SKILL ();
				break;

			case GSV_SKILL_START:
				Recv_gsv_SKILL_START();
				break;

			case GSV_SKILL_CANCEL:
				Recv_gsv_SKILL_CANCEL();
				break;


			case GSV_CLEAR_STATUS:
				Recv_gsv_CLEAR_STATUS();
				break;

			case GSV_SPEED_CHANGED : 
				Recv_gsv_SPEED_CHANGED ();
				break;

			/// Used the item.
			case GSV_USE_ITEM:
				Recv_gsv_USE_ITEM ();
				break;

			case GSV_P_STORE_MONEYnINV:
				Recv_gsv_P_STORE_MONEYnINV();
				break;
			case GSV_SET_MONEYnINV:
				Recv_gsv_SET_MONEYnINV ();
				break;

			case GSV_REWARD_ITEM  :		/// Quest reward items are changed by
			{
				Recv_gsv_REWARD_ITEM();
			}

			case GSV_SET_INV_ONLY :
				Recv_gsv_SET_INV_ONLY ();
				break;

			case GSV_SET_HOTICON :
				Recv_gsv_SET_HOTICON ();
				break;

			case GSV_USE_BPOINT_REPLY :
				Recv_gsv_USE_BPOINT_REPLY ();
				break;

			case GSV_QUEST_REPLY :
				Recv_gsv_QUEST_REPLY ();
				break;
			/*
			case GSV_QUEST_DATA_REPLY :
				Recv_gsv_QUEST_DATA_REPLY ();
				break;
			*/
			case GSV_TRADE_P2P:
				Recv_gsv_TRADE_P2P();
				break;
			case GSV_TRADE_P2P_ITEM:
				Recv_gsv_TRADE_P2P_ITEM();
				break;

			case GSV_PARTY_REQ:
				Recv_gsv_PARTY_REQ();
				break;
			case GSV_PARTY_REPLY:
				Recv_gsv_PARTY_REPLY();
				break;
			case GSV_PARTY_MEMBER:
				Recv_gsv_PARTY_MEMBER();
				break;
			case GSV_PARTY_LEVnEXP :
				Recv_gsv_PARTY_LEVnEXP ();
				break;
			case GSV_PARTY_ITEM:
				Recv_gsv_PARTY_ITEM();
				break;
			case GSV_PARTY_RULE:
				Recv_gsv_PARTY_RULE();
				break;
			case GSV_CHANGE_OBJIDX:
				Recv_gsv_CHANGE_OBJIDX();
				break;
			case GSV_STORE_TRADE_REPLY:
				Recv_gsv_STORE_TRADE_REPLY();
				break;
			case GSV_CREATE_ITEM_REPLY:
				Recv_gsv_CREATE_ITEM_REPLY();
				break;
			case GSV_BANK_LIST_REPLY:
				Recv_gsv_BANK_LIST_REPLY();
				break;
			case GSV_MOVE_ITEM:
				Recv_gsv_MOVE_ITEM();
				break;
			case GSV_SET_BULLET:
				Recv_gsv_SET_BULLET();
				break;
			case GSV_SERVER_DATA:
				Recv_gsv_SERVER_DATA();
				break;
			case GSV_ASSEMBLE_RIDE_ITEM:
				Recv_gsv_ASSEMBLE_RIDE_ITEM();
				break;

			case GSV_SET_EVENT_STATUS:
				Recv_GSV_SET_EVENT_STATUS();
				break;

			case GSV_SET_ITEM_LIFE:
				Recv_GSV_SET_ITEM_LIFE();
				break;

			case GSV_P_STORE_OPENED:
				Recv_gsv_P_STORE_OPENED	();
				break;
			case GSV_P_STORE_CLOSED:
				Recv_gsv_P_STORE_CLOSED	();
				break;
			case GSV_P_STORE_LIST_REPLY:
                Recv_gsv_P_STORE_LIST_REPLY ();
				break;
			case GSV_P_STORE_RESULT:
				Recv_gsv_P_STORE_RESULT();
				break;

			case GSV_USED_ITEM_TO_REPAIR:
				Recv_gsv_USED_ITEM_TO_REPAIR();
				break;
			case GSV_REPAIRED_FROM_NPC:
				Recv_gsv_REPAIRED_FROM_NPC();
				break;

			case GSV_SET_MONEY_ONLY:
				Recv_gsv_SET_MONEY_ONLY();
				break;
			case GSV_REWARD_MONEY:
				Recv_gsv_REWARD_MONEY();
				break;

			case GSV_REWARD_ADD_ABILITY :
			{
				Recv_gsv_REWARD_ADD_ABILITY();
				break;
			}
			case GSV_REWARD_SET_ABILITY :
			{
				Recv_gsv_REWARD_SET_ABILITY();
				break;
			}

			case GSV_GODDNESS_MODE:
			{
				Recv_gsv_GODDNESS_MODE();
				break;
			}

			//----------------------------------------------------------------------------------------------------			
			/// @brief Jamming related items
			//----------------------------------------------------------------------------------------------------
			case GSV_CRAFT_ITEM_REPLY:
				Recv_gsv_CRAFT_ITEM_REPLY();
				break;

			//----------------------------------------------------------------------------------------------------			
			/// @brief Object-related events
			//----------------------------------------------------------------------------------------------------
			case GSV_ADD_EVENTOBJ:
				Recv_gsv_ADD_EVENTOBJ();
				break;

			case GSV_APPRAISAL_REPLY:
				Recv_gsv_APPRAISAL_REPLY();
				break;

			case GSV_SET_HPnMP:
				Recv_gsv_SET_HPnMP();
				break;

			case GSV_CHAR_HPMP_INFO:				// 1 packet per second from server updates HP / MP (PY: soon it will receive other stats too)
				Recv_gsv_CHAR_HPMP_INFO();
				break;

			case GSV_CHECK_NPC_EVENT:
				Recv_gsv_CHECK_NPC_EVENT();
				break;

			case GSV_ALLIED_CHAT:
				Recv_gsv_ALLIED_CHAT();
				break;
			case GSV_ALLIED_SHOUT:
				Recv_gsv_ALLIED_SHOUT();
				break;



			case GSV_CHARSTATE_CHANGE:
				Recv_gsv_CHARSTATE_CHANGE();
				break;


			case GSV_SCREEN_SHOT_TIME:
				 Recv_gsv_SCREEN_SHOT_TIME();
				 break;
			// Begin - Antonio - 2006-07-24  AM 11:16:21
#ifdef __CLAN_WAR_SET
		    // Keulraenjeon request entry
			case GSV_CLANWAR_INTO_OK:
				 Recv_gsv_CLANWAR_JOIN_REQ();
				 break;

			 // Processes related to the remaining time keulraenjeon
			case GSV_CLANWAR_TIME:
				Recv_gsv_CLANWAR_TIME();
				break;

			case GSV_CLANWAR_ERR:
				Recv_gsv_CLANWAR_ERR();
				break;

			case GSV_CLANWAR_PROGRESS:
				Recv_gsv_CLANWAR_PROGRESS();
				break;

			case GSV_CLANWAR_BOSS_HP:
				Recv_gsv_CLANWAR_BOSS_HP();
				break;
#endif
			// End  - Antonio

			case GSV_WARNING_MSGCODE:
				Recv_gsv_WARNING_MSGCODE();
				break;

			case GSV_USER_MSG_1LV:
				Recv_gsv_Cli_USER_MSG_1LV();
				break;

#ifdef __EXPERIENCE_ITEM
			// Remaining time of the items mileage.
			case GSV_MILEAGE_ITEM_TIME:
				Recv_gsv_MALL_ITEM_TIME();
				break;
#endif
			case GSV_SERVER_TIME:
				Recv_gsv_Server_Time();
				break;


			default :
				//_ASSERT(0);
				ClientLog (LOG_NORMAL, "received Invalid packet type ... type: 0x%x , size: %d \n", m_pRecvPacket->m_HEADER.m_wType, m_pRecvPacket->m_HEADER.m_nSize);
	}
}

//-------------------------------------------------------------------------------------------------
void CNetwork::Proc ()
{
	this->Proc_WorldPacket ();

	CshoClientSOCK *pSocket = &this->m_ZoneSOCKET;
	while( m_ZoneSOCKET.Peek_Packet( m_pRecvPacket, true ) ) 
	{
		// LogString (LOG_DEBUG, "Packet_Proc:: Type: 0x%x, Size: %d \n", m_pRecvPacket->m_HEADER.m_wType, m_pRecvPacket->m_HEADER.m_nSize );

#ifdef __ARCFOUR
		//Dummy packet generation anti hacking		
		if( rand()%100 == 0 )
		{		
			Send_cli_DUMMY_1();		
		}

#endif	

		switch( m_pRecvPacket->m_HEADER.m_wType ) {
			case SOCKET_NETWORK_STATUS :
			{
				m_btZoneSocketSTATUS = m_pRecvPacket->m_NetSTATUS.m_btStatus;
				switch( m_pRecvPacket->m_NetSTATUS.m_btStatus ) {
					case NETWORK_STATUS_ACCEPTED    :
					{
						pSocket->OnAccepted ( (int*)m_pRecvPacket->m_NetSTATUS.m_dwSocketIDs );
						_ASSERT( 0 );
						continue;
					}
					case NETWORK_STATUS_CONNECT		:
					{
						// John was the connection with the server ...
						// After selecting the character into a real zone.
						m_bWarping = false;
						pSocket->mF_Init( m_dwGSV_IDs[1] );
						this->Send_cli_JOIN_SERVER_REQ( m_dwGSV_IDs[0] );
						continue;
					}
					case NETWORK_STATUS_DISCONNECT:
					{
						// Will disconnect the connection to the server move? Will disconnected?
						if ( m_bMoveingZONE ) {
							this->MoveZoneServer ();
							continue;
						}
						CGame::GetInstance().ProcWndMsg( WM_USER_SERVER_DISCONNECTED,0,0 );
						//SendServerDisconnectMsg( "ZoneServer" );
						break;
					}
					case NETWORK_STATUS_DERVERDEAD	:
					{
						break;
					}
				}

				ClientLog (LOG_NORMAL,"John failed to connect with the server.\n");
				break;
			}

			case SRV_JOIN_SERVER_REPLY :	// Zone was connected to the server.
			{
				DWORD dwRet = Recv_srv_JOIN_SERVER_REPLY ();
				if ( dwRet ) {
					pSocket->OnAccepted( (int*)&dwRet );
					// John first server connected? If you ...

					CLiveCheck::GetSingleton().ResetTime();
				} else {
					// TODO:: error
					this->DisconnectFromServer ();
					return;
				}
				break;
			}

			default :
				this->Proc_ZonePacket();
		}
	}
}

//-------------------------------------------------------------------------------------------------
//2005. 5. 23 Night Ji Ho
void CNetwork::Send_AuthMsg (void)
{
}

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
