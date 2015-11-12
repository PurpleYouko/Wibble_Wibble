// dll001.cpp : DLL 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include "dll001.h"
#include "TriggerVFS.h"


BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
    return TRUE;
}

// 내보낸 변수의 예제입니다.
DLL001_API int ndll001=0;

// 내보낸 함수의 예제입니다.
DLL001_API int fndll001(void)
{
	return 42;
}

// 내보낸 클래스의 생성자입니다.
// 클래스 정의를 보려면 dll001.h를 참조하십시오.
Cdll001::Cdll001()
{ 
	return; 
}



VHANDLE g_hVFS = NULL;

bool Dll_OpenVFS (const char * FileName, const char * Mode)
{
	g_hVFS = OpenVFS (FileName, Mode);

	return (g_hVFS != NULL); 
}

VFileHandle * Dll_OpenFile (const char * FileName)
{
	VFileHandle * pVF = VOpenFile (FileName, g_hVFS);

	return pVF;
}


void Dll_CloseFile (VFileHandle *pVF)
{
	if(pVF)
		VCloseFile (pVF);
}


bool Dll_ReadFile (VFileHandle * pVF, char * Buff, int iReadSize)
{
	size_t stReaded = 0;

	if(g_hVFS)
	{
		if(pVF)
			stReaded = vfread (Buff, 1, iReadSize, pVF);
	}

	return ((int)stReaded == iReadSize);
}

void Dll_CloseVFS (void)
{
	if(g_hVFS)
	{
		CloseVFS (g_hVFS);
		g_hVFS = NULL;
	}
}