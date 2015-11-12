// vfs_autorun.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include <time.h>
#include <stdio.h>
#include <iostream>
#include <io.h>

using namespace std;

int _tmain(int argc, _TCHAR* argv[])
{
	char szTargetName[ 1024 ];

	struct tm *newtime;
	time_t aclock;


	time( &aclock );   // Get time in seconds
	newtime = localtime( &aclock );   // Convert time to struct tm form 

	/* Print local time as a string */
	if(argc >= 3 && _access (argv[ 2 ], 0) == 0)
	{
		sprintf(szTargetName, "%s_%02d_%02d_%02d_%02d_%02d.txt"
				,argv[ 2 ]
				,newtime->tm_year + 1900
				,newtime->tm_mon + 1
				,newtime->tm_mday 
				,newtime->tm_hour
				,newtime->tm_min
				);

		if(rename (argv[ 1 ], szTargetName) == 0)
			cout << "rename from " << argv[ 1 ] << " to " << szTargetName << endl;
	}
	else
		cout << "Wrong Paramteter \n usage vfs_autorun filename prefix" <<endl;

	return 0;
}

