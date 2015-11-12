/*
	$Header: /Client/Util/CSocket.h 3     03-09-26 1:26p Icarus $
*/
#ifndef	__CSOCKET_H
#define	__CSOCKET_H
#include <winSock.h>
//-------------------------------------------------------------------------------------------------

class CSOCKET {
private :
public  :
	static bool Init ();
	static void Free ();
} ;

int Socket_Error (char *szMsg);

//-------------------------------------------------------------------------------------------------
#endif
