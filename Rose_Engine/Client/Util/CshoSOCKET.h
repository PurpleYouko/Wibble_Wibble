/*
	$Header: /Client/Util/CshoSOCKET.h 8     06-06-02 5:46p Kjhpower $
*/
#ifndef	__CSHO_SOCK_H
#define	__CSHO_SOCK_H
#include "CClientSOCKET.h"

//-------------------------------------
// 2006.03.23/김대성/추가 - Stream Cipher Encryption Algorithm "Arcfour"
#include "Arcfour/Arcfour.h"
//-------------------------------------

//-------------------------------------------------------------------------------------------------

class CshoClientSOCK : public CClientSOCKET
{
private:
	HANDLE	m_hEncSOCK;
	int		m_iSendSEQ;

//-------------------------------------
#ifndef __ARCFOUR
	WORD	mF_ESP (t_PACKETHEADER *pPacket);
	WORD	mF_DRH (t_PACKETHEADER *pPacket);
	short	mF_DRB (t_PACKETHEADER *pPacket);
#endif
// 2006.03.24/김대성/삭제 - CClientSOCKET 의 함수사용 ( 아무것도 안함 )
//-------------------------------------

public :
	CshoClientSOCK ();
	virtual ~CshoClientSOCK ();

	void Set_NetSTATUS (BYTE btStatus);
	void OnAccepted(int *pSendSEQ);
	bool WndPROC ( WPARAM wParam, LPARAM lParam );


#ifdef __ARCFOUR
//-------------------------------------
// 2006.03.23/김대성/추가 - Stream Cipher Encryption Algorithm "Arcfour"
	void cipher_init(const unsigned char *key, int key_len);
	void cipher_encrypt(void *src, int len);
	void cipher_decrypt(void *src, int len);
#else
	void	mF_Init(DWORD dwInit);
#endif

private:
	Arcfour cipher;
//-------------------------------------
} ;

//-------------------------------------------------------------------------------------------------
#endif
