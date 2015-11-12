// vfs_open.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include <iostream>
#include "TriggerVFS.h"

using namespace std;

int _tmain(int argc, _TCHAR* argv[])
{
	

	char szCurDir[ 1024 ];
	GetCurrentDirectory (1024, szCurDir);

	char szIdxName[ 1024 ];
	char * szMode = "r";

	if(argc >= 2)
		strcpy (szIdxName, argv[ 1 ]);
	else
		strcpy (szIdxName, "data.idx");

	if(_access (szIdxName, 0) == 0)
	{
		VHANDLE hVFS = OpenVFS (szIdxName, szMode);

		if(hVFS)
		{
			CloseVFS (hVFS);
			hVFS = NULL;
		}
		else
		{
			cout << "Openning failed" << endl;
			return 0;
		}

		cout << "Opened Successfully and closed" << endl;
	}
	else
		cout << "Can't find file" << endl;

	return 0;
}

