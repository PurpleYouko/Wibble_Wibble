
#include <windows.h>
#include <tchar.h>
#include <crtdbg.h>
#include "CPacketCODEC.h"

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
struct tPOINTF {
	float	x;
	float	y;
} ;

#define	__POINTF
#define	__SKIP_SRV_PROTOTYPE

#include "CshoSOCKET.h"

CshoClientSOCK::CshoClientSOCK ()
{
	m_hEncSOCK = (HANDLE)new CPacketCODEC(MAX_PACKET_SIZE);

	m_iSendSEQ = 0;
}
CshoClientSOCK::~CshoClientSOCK ()
{
	delete (CPacketCODEC*)m_hEncSOCK;
}

//-------------------------------------------------------------------------------------------------
void CshoClientSOCK::Set_NetSTATUS (BYTE btStatus)
{
	if ( NETWORK_STATUS_CONNECT == btStatus ) {
		// 초기화...
		m_iSendSEQ = 0;
	}

	CClientSOCKET::Set_NetSTATUS( btStatus );
}

void CshoClientSOCK::OnAccepted(int *pSendSEQ)
{
	m_iSendSEQ = pSendSEQ[ 0 ];
}

//-------------------------------------
/*
void CshoClientSOCK::mF_Init (DWORD dwInitCODE)
{	
//	CPacketCODEC::Instance ();
	if ( dwInitCODE )
		((CPacketCODEC*)m_hEncSOCK)->Init( dwInitCODE );
	else
		((CPacketCODEC*)m_hEncSOCK)->Default ();
}

WORD CshoClientSOCK::mF_ESP (t_PACKETHEADER *pPacket)
{
	// 서버에 보낼 패킷 암호화....
	return ((CPacketCODEC*)m_hEncSOCK)->Encode_SendClientPACKET( pPacket, m_iSendSEQ );
}
WORD CshoClientSOCK::mF_DRH (t_PACKETHEADER *pPacket)
{
	// 서버에서 받은 패킷헤더 복호화...
	return ((CPacketCODEC*)m_hEncSOCK)->Decode_RecvServerHEADER( pPacket );
}
short CshoClientSOCK::mF_DRB (t_PACKETHEADER *pPacket)
{
	// 서버에서 받은 패킷몸체 복호화...
	return ((CPacketCODEC*)m_hEncSOCK)->Decode_RecvServerBODY( pPacket );
}
*/
// 2006.03.24/김대성/삭제 - CClientSOCKET 의 함수사용 ( 아무것도 안함 )
//-------------------------------------

//-------------------------------------------------------------------------------------------------
bool CshoClientSOCK::WndPROC (WPARAM wParam, LPARAM lParam)
{
	int nErrorCode = WSAGETSELECTERROR(lParam);
	switch ( WSAGETSELECTEVENT(lParam) ) {
		case FD_READ:
		{
			this->OnReceive( nErrorCode );
			break;
		}
		case FD_WRITE:
			this->OnSend ( nErrorCode );
			break;
		case FD_CONNECT: 
		{
			this->OnConnect ( nErrorCode );

			if ( !nErrorCode ) 
				this->Set_NetSTATUS( NETWORK_STATUS_CONNECT );
			else 
				this->Set_NetSTATUS( NETWORK_STATUS_DERVERDEAD );
			break;
		}
		case FD_CLOSE:		// Close()함수를 호출해서 종료될때는 발생 안한다.
		{
			this->OnClose ( nErrorCode );
			this->Set_NetSTATUS( NETWORK_STATUS_DISCONNECT );
		}
	}

	return true;
}

//-------------------------------------------------------------------------------------------------

//-------------------------------------
// 2006.03.23/김대성/추가 - Stream Cipher Encryption Algorithm "Arcfour"
void CshoClientSOCK::cipher_init(const unsigned char *key, int key_len)
{
	cipher.init(key, key_len);
}

void CshoClientSOCK::cipher_encrypt(void *src, int len)
{
	cipher.encrypt(src, len);
}

void CshoClientSOCK::cipher_decrypt(void *src, int len)
{
	cipher.decrypt(src, len);
}
//-------------------------------------

//-------------------------------------------------------------------------------------------------
