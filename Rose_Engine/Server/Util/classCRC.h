#ifndef	__CLASSCRC_H
#define	__CLASSCRC_H
//-------------------------------------------------------------------------------------------------

class classCRC {
private :
	static BYTE		m_btCRC08;
	static WORD		m_wCRC16;
	static DWORD	m_dwCRC32;

public  :
	static	void	InitCRC08 ()	{	m_btCRC08 = 0;	}
	static	void	InitCRC16 ()	{	m_wCRC16  = 0;	}
	static	void	InitCRC32 ()	{	m_dwCRC32 = 0;	}

	static BYTE		GetCRC08 ()		{	return m_btCRC08;	}
    static WORD		GetCRC16 ()     {   return m_wCRC16;	}
    static DWORD	GetCRC32 ()     {   return m_dwCRC32;   }

	static DWORD	FileCRC32 (char *szFileName);
#ifdef	__USE_STATIC_CRC_VAR
	static DWORD	DataCRC32 (const void *pDATA, int iSize, bool bInitCRC);
#else
	static DWORD	DataCRC32 (const void *pDATA, int iSize, DWORD);
#endif
	static WORD		DataCRC16 (const void *pDATA, int iSize, bool bInitCRC);
#ifdef __USE_STATIC_CRC_VAR
	static BYTE		DataCRC08 (const BYTE *pDATA, int iSize, bool bInitCRC);
#else
	static BYTE		DataCRC08 (const BYTE *pDATA, int iSize);
#endif
	static WORD		DataCRC16 (const void *pDATA, int iSize);
} ;

#define CRC08(CRC,btVALUE)	( g_pcCRC08[ CRC ^ btVALUE ] );
#define	CRC16(CRC,btVALUE)	( (CRC >> 8 ) ^ g_pwCRC16[ ( CRC ^ btVALUE ) & 0x0ff ] );

extern const BYTE  g_pcCRC08[ 256 ];
extern const WORD  g_pwCRC16[ 256 ];
extern const DWORD g_pdwCRC32[ 256 ];

//-------------------------------------------------------------------------------------------------
#endif