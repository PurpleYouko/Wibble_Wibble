// 다음 ifdef 블록은 DLL에서 내보내기하는 작업을 쉽게 해 주는 매크로를 만드는 
// 표준 방식입니다. 이 DLL에 들어 있는 파일은 모두 명령줄에 정의된 DLL001_EXPORTS 기호로
// 컴파일되며, 동일한 DLL을 사용하는 다른 프로젝트에서는 이 기호를 정의할 수 없습니다.
// 이렇게 하면 소스 파일에 이 파일이 들어 있는 다른 모든 프로젝트에서는 
// DLL001_API 함수를 DLL에서 가져오는 것으로 보고,
// 이 DLL은 해당 매크로로 정의된 기호가 내보내지는 것으로 봅니다.
#ifdef DLL001_EXPORTS
#define DLL001_API __declspec(dllexport)
#else
#define DLL001_API __declspec(dllimport)
#endif

#include "TriggerVFS.h"

// 이 클래스는 dll001.dll에서 내보낸 것입니다.
class DLL001_API Cdll001 {
public:
	Cdll001(void);
	// TODO: 여기에 메서드를 추가합니다.
};

extern DLL001_API int ndll001;

DLL001_API int fndll001(void);

DLL001_API bool Dll_OpenVFS (const char * FileName, const char * Mode);
DLL001_API void Dll_CloseFile (VFileHandle *pVF);
DLL001_API bool Dll_ReadFile (VFileHandle * pVF, char * Buff, int iReadSize);
DLL001_API void Dll_CloseVFS (void);
DLL001_API VFileHandle * Dll_OpenFile (const char * FileName);

