// sam001.cpp : �ܼ� ���� ���α׷��� ���� �������� �����մϴ�.
//
/****************************************************************************
 * 2 ������ ���ÿ� �ε��� ������ �����ؼ� �۾� �� ���.
 */

#include "stdafx.h"
#include "TriggerVFS.h"
#include "dll001.h"
#include <cassert>

/*
bool Dll_OpenVFS (const char * FileName, const char * Mode);
bool Dll_ReadFile (const char * FileName, char * Buff, int iMaxBuffSize);
void Dll_CloseVFS (void);
*/

int _tmain(int argc, _TCHAR* argv[])
{
	
	char * IdxFile = "data.idx";
	char * Mode = "r";
	char * pBuff1 = NULL;
	char * pBuff2 = NULL;

	VFileHandle * pVF = NULL;
	VFileHandle * pVF2 = NULL;

	size_t stFileSize = 0;
	VHANDLE hVFS = OpenVFS (IdxFile, Mode);

	

	return 0;
	
}

