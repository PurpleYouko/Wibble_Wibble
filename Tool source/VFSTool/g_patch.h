#ifndef __G_PATCH__
#define __G_PATCH__

#include <string>
#include <vector>
#include "TriggerVFS.h"

/********************************************************************************
 * Op 값 
 * 추가(0), 삭제(1), 교체(2)
 */
#define		OP_ADD		0			/// 추가
#define		OP_DEL		1			/// 삭제
#define		OP_EXCH		2			/// 교체


/********************************************************************************
 * 패치리스트 파일 헤더
 * DWORD 갯수 패치 엔트리 갯수
 */


/********************************************************************************
 * PatchEntry
 * 파일에 대한 패치 정보
 * 일반적으로 Pack파일을 만들때 사용해도 되는 구조임
 */
struct PatchEntry
{
	DWORD 		dwStdVersion;		/// 기준 버젼
	std::string PackFileName;		/// Pack 파일이름.			크기(short):문자열(NULL포함)로 저장
	std::string	FileName;			/// 파일 이름. 저장할때는	크기(short):문자열(NULL포함)로 저장
	std::string DownName;			/// 다운로드용 파일명		크기(short):문자열(NULL포함)로 저장
	BYTE		btOP;				/// Operation. 추가(0), 삭제(1), 교체(2)
	DWORD		dwVersion;			/// 이 파일의 버젼. 상위비트:상위버젼. 하위비트:하위버젼
	DWORD		dwCRC;				/// CRC값. DWORD (32bit)
};


/********************************************************************************
 * VfsEntry
 * Vfs 파일에 대한 Entry. ( 현재 사용하지 않음 )
 */
struct VfsEntry
{
	std::string VfsName;					/// Vfs 파일명
	std::vector<PatchEntry *> PatchList;	/// 패치리스트
};


/// PatchEntry를 파일에 저장한다
void WritePatchEntry (PatchEntry *PE, FILE * fp);

/// PatchEntry를 파일에어 읽는다
void ReadPatchEntry (PatchEntry *PE, FILE * fp);

/// Patch List 를 저장한다
bool SavePatchList (const char * FileName, std::vector<PatchEntry *> *vecEntry);

/// Patch List 를 오픈한다
bool OpenPatchList (const char * FileName, std::vector<PatchEntry *> *vecEntry);

/// Patch List 를 지운다
void DeletePatchList (std::vector<PatchEntry *> *vecEntry);

/// Patch List 를 카피한다 vecEntry1 ==> vecEntry2
bool CopyPatchList (std::vector<PatchEntry *> *vecEntry1, std::vector<PatchEntry *> *vecEntry2);

/// Patch List 에서 주어진 파일에 대한 엔트리를 찾는다
PatchEntry * FindPatchEntry (std::vector<PatchEntry *> *vecEntry1, const char *FileName);

/// 패치리스트를 가지고 VFS파일을 만든다
bool MakeVFS (const char * FileName, std::vector<PatchEntry *> *vecEntry, DWORD, DWORD);

/// Vfs 파일을 연다
bool OpenIDX (const char * FileName, std::vector<PatchEntry *> *vecEntry, DWORD *shStdVersion, DWORD *shCurVersion, bool bClose = true, VHANDLE * hVFS = NULL);

///
DWORD ver_atosh (const char *ver);

///
char * ver_shtoa (DWORD dwVersion, char * buff);

/// 패치리스트를 다음에 저장
extern std::vector<PatchEntry *>	g_vecStd;		/// 기준버젼
extern std::vector<PatchEntry *>	g_vecCur;		/// 현재버젼
extern std::vector<PatchEntry *>	g_vecNext;		/// 다음버젼

extern char * OPSTR[ 3 ] ;


#endif