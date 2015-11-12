// ���� ifdef ����� DLL���� ���������ϴ� �۾��� ���� �� �ִ� ��ũ�θ� ����� 
// ǥ�� ����Դϴ�. �� DLL�� ��� �ִ� ������ ��� ����ٿ� ���ǵ� DLL001_EXPORTS ��ȣ��
// �����ϵǸ�, ������ DLL�� ����ϴ� �ٸ� ������Ʈ������ �� ��ȣ�� ������ �� �����ϴ�.
// �̷��� �ϸ� �ҽ� ���Ͽ� �� ������ ��� �ִ� �ٸ� ��� ������Ʈ������ 
// DLL001_API �Լ��� DLL���� �������� ������ ����,
// �� DLL�� �ش� ��ũ�η� ���ǵ� ��ȣ�� ���������� ������ ���ϴ�.
#ifdef DLL001_EXPORTS
#define DLL001_API __declspec(dllexport)
#else
#define DLL001_API __declspec(dllimport)
#endif

#include "TriggerVFS.h"

// �� Ŭ������ dll001.dll���� ������ ���Դϴ�.
class DLL001_API Cdll001 {
public:
	Cdll001(void);
	// TODO: ���⿡ �޼��带 �߰��մϴ�.
};

extern DLL001_API int ndll001;

DLL001_API int fndll001(void);

DLL001_API bool Dll_OpenVFS (const char * FileName, const char * Mode);
DLL001_API void Dll_CloseFile (VFileHandle *pVF);
DLL001_API bool Dll_ReadFile (VFileHandle * pVF, char * Buff, int iReadSize);
DLL001_API void Dll_CloseVFS (void);
DLL001_API VFileHandle * Dll_OpenFile (const char * FileName);

