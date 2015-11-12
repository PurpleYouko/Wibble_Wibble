// vfs_clearblank.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include "TriggerVFS.h"
#include <iostream>

using namespace std;

void __stdcall ClearCallBack (int iCur)
{
	cout << "Current : " << iCur << endl;
}

int _tmain(int argc, _TCHAR* argv[])
{
	char szCurDir[ 1024 ];
	GetCurrentDirectory (1024, szCurDir);

	char * Mode = "r+";
	char szIdxFile[ 1024 ];
	
	if(argc >= 2)
		strcpy (szIdxFile, argv[ 1 ]);
	else
		strcpy (szIdxFile, "data.idx");


	VHANDLE hVFS = OpenVFS (szIdxFile, Mode);
	if(hVFS)
	{
		VClearBlankAll (hVFS, ClearCallBack);

		CloseVFS (hVFS);

		cout << "Finshined Clearing Blanks of " << szIdxFile <<endl ;
	}
	else
		cout << "Failed in openning Index file (" << szIdxFile << ")" << endl;

	return 0;
}

