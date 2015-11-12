#ifndef	__CLIENT_LOG_H
#define	__CLIENT_LOG_H
//-------------------------------------------------------------------------------------------------


//#define LOG_NONE            0
//#define LOG_NORMAL			0x01
//#define LOG_VERBOSE			0x02
//#define LOG_DEBUG			0x04

#define LOG_DIRECTORY		"log/"


class CLientLOG {
private :
  static WORD	m_wLogMODE[ 3 ];
	static char m_StrBUFF [ 1024 ];

	CRITICAL_SECTION	m_csLOCK;

public  :
	static void Client_Log (WORD wLogMODE, char *fmt, ...);

    CLientLOG ();
    ~CLientLOG ();

} ;

#define ClientLog  CLientLOG::Client_Log

//-------------------------------------------------------------------------------------------------
#endif
