
//ClientLog.h
//PurpleYouko 3/3/14
//added a logging system to help debug the client

#include <windows.h>
#include "ClientLOG.h"
#include <cstdio>
#include <time.h>

void CLientLOG::Client_Log (WORD wLogMODE, char *fmt, ...)
{
	// Timestamp
    time_t rtime;
    time(&rtime);
    char *timestamp = ctime(&rtime);
    timestamp[ strlen(timestamp)-1 ] = ' ';   

	va_list ap;	      // For arguments
	va_start( ap, fmt );

	
	FILE *fh;
    fh = fopen(LOG_DIRECTORY "ClientLog.log", "a+" );

	if ( fh != NULL )
    {
		fprintf( fh, "%s- ", timestamp );
        vfprintf( fh, fmt, ap );
		fputc( '\n', fh );
		fclose( fh );
	}
	va_end(ap);

}
