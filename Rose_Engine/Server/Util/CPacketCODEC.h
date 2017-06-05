/*
	$Header: /OnlineRPG/LIB_Util/CPacketCODEC.h 2     05-11-28 5:41p Young $
*/
#ifndef	__CPACKET_CODEC_H
#define	__CPACKET_CODEC_H
#include "LIB_Util.h"
#include "classCRC.h"
#include "CRandom.h"
//-------------------------------------------------------------------------------------------------

#define	CPacketCODEC					CpC

#define	Encode_SendClientPACKET			ESCP
#define	Encode_SendServerPACKET			ESSP

#define	Decode_RecvClientHEADER			DRCH
#define	Decode_RecvClientBODY			DRCB
#define	Decode_RecvServerHEADER			DRSH
#define	Decode_RecvServerBODY			DRSB
#define	GetDecodedPacketLength			GDPL

struct t_PACKET;
struct t_PACKETHEADER;

class CpC
{
private:
	CR001  *m_pCR;
	int	  **m_ppRT;
	WORD   *m_pRI;
	DWORD m_dwMaxPacketSize;
	bool Find_DupTABLE (int iValue, int iTableIDX, int iRange);
	bool Find_DupINDEX (WORD wValue, int iRange);

	// CRC °è»ê 
	static WORD CalTAG (WORD wCRC, BYTE btVALUE)
	{
		return CRC08(wCRC,btVALUE);
		// return CRC16(wCRC,btVALUE);
	}

public :
	CpC();
	CpC (DWORD dwPacketSize);
	~CpC ();
	void	Default ();
	bool	Init( DWORD dwInitCODE );
	WORD	GDPL(t_PACKETHEADER *pPacket);					// GetDecodedPacketLength

	WORD	ESCP(t_PACKETHEADER *pPacket, int &iSendSEQ);	// Encode_SendClientPACKET
	WORD	DRSH(t_PACKETHEADER *pPacket);					// Decode_RecvServerHEADER
	short	DRSB(t_PACKETHEADER *pPacket);					// Decode_RecvServerBODY

	WORD	ESSP(t_PACKETHEADER *pPacket);					// Encode_SendServerPACKET
	WORD	DRCH(t_PACKETHEADER *pPacket, int &iRecvSEQ);	// Decode_RecvClientHEADER
	short	DRCB(t_PACKETHEADER *pPacket);					// Decode_RecvClientBODY
} ;

//-------------------------------------------------------------------------------------------------
#endif
