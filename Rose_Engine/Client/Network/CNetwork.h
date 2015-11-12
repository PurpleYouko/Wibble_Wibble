/*
	$Header: /Client/Network/CNetwork.h 15    06-03-28 6:10p Raning $
*/
#ifndef	__CNETWORK_H
#define	__CNETWORK_H
#include "NET_Prototype.h"
#include "..\util\CshoSOCKET.H"
#include "Object.h"
#include "RecvPACKET.h"
#include "SendPACKET.h"
//-------------------------------------------------------------------------------------------------

enum {
	NS_NULL=0,
	NS_CON_TO_LSV,
	NS_DIS_FORM_LSV,
	NS_CON_TO_WSV
} ;

class CNetwork : public CRecvPACKET, public CSendPACKET 
{
private :
	bool	bAllInONE;		/// 월드/존서버가 같은 서버냐?
	void Send_PACKET(t_PACKET *pSendPacket, bool bSendToWorld=false);

	short	m_nProcLEVEL;
	bool	m_bMoveingZONE;
	BYTE	m_btZoneSocketSTATUS;

	static CNetwork *m_pInstance;
	CNetwork (HINSTANCE hInstance);
	~CNetwork ();

	void Proc_WorldPacket ();
	void Proc_ZonePacket ();

	void MoveZoneServer ();

public  :
#ifdef	__VIRTUAL_SERVER
	CClientSOCKET	m_SOCKET;
#else
	CshoClientSOCK	m_WorldSOCKET;
	CshoClientSOCK	m_ZoneSOCKET;
#endif
	bool m_bWarping;	// 존 워프 중인가...

	static CNetwork *Instance (HINSTANCE hInstance);
	void Destroy ();

	bool ConnectToServer (char *szServerIP, WORD wTcpPORT, short nProcLEVEL=0);
	void DisconnectFromServer (short nProcLEVEL=0);
	// 박 지호 
	void Send_AuthMsg (void);

	// Playing packet ..
//	void Send_Damage (int iServerObjectIndex, short nDamage);
//	void Recv_Damage (t_PACKET *pPacket);

	void Proc ();
} ;
extern CNetwork	*g_pNet;
//-------------------------------------------------------------------------------------------------
#endif