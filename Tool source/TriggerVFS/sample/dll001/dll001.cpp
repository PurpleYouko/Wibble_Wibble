// dll001.cpp : DLL ���� ���α׷��� ���� �������� �����մϴ�.
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

// ������ ������ �����Դϴ�.
DLL001_API int ndll001=0;

// ������ �Լ��� �����Դϴ�.
DLL001_API int fndll001(void)
{
	return 42;
}

// ������ Ŭ������ �������Դϴ�.
// Ŭ���� ���Ǹ� ������ dll001.h�� �����Ͻʽÿ�.
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