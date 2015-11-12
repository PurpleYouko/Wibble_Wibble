#ifndef __G_PATCH__
#define __G_PATCH__

#include <string>
#include <vector>
#include "TriggerVFS.h"

/********************************************************************************
 * Op �� 
 * �߰�(0), ����(1), ��ü(2)
 */
#define		OP_ADD		0			/// �߰�
#define		OP_DEL		1			/// ����
#define		OP_EXCH		2			/// ��ü


/********************************************************************************
 * ��ġ����Ʈ ���� ���
 * DWORD ���� ��ġ ��Ʈ�� ����
 */


/********************************************************************************
 * PatchEntry
 * ���Ͽ� ���� ��ġ ����
 * �Ϲ������� Pack������ ���鶧 ����ص� �Ǵ� ������
 */
struct PatchEntry
{
	DWORD 		dwStdVersion;		/// ���� ����
	std::string PackFileName;		/// Pack �����̸�.			ũ��(short):���ڿ�(NULL����)�� ����
	std::string	FileName;			/// ���� �̸�. �����Ҷ���	ũ��(short):���ڿ�(NULL����)�� ����
	std::string DownName;			/// �ٿ�ε�� ���ϸ�		ũ��(short):���ڿ�(NULL����)�� ����
	BYTE		btOP;				/// Operation. �߰�(0), ����(1), ��ü(2)
	DWORD		dwVersion;			/// �� ������ ����. ������Ʈ:��������. ������Ʈ:��������
	DWORD		dwCRC;				/// CRC��. DWORD (32bit)
};


/********************************************************************************
 * VfsEntry
 * Vfs ���Ͽ� ���� Entry. ( ���� ������� ���� )
 */
struct VfsEntry
{
	std::string VfsName;					/// Vfs ���ϸ�
	std::vector<PatchEntry *> PatchList;	/// ��ġ����Ʈ
};


/// PatchEntry�� ���Ͽ� �����Ѵ�
void WritePatchEntry (PatchEntry *PE, FILE * fp);

/// PatchEntry�� ���Ͽ��� �д´�
void ReadPatchEntry (PatchEntry *PE, FILE * fp);

/// Patch List �� �����Ѵ�
bool SavePatchList (const char * FileName, std::vector<PatchEntry *> *vecEntry);

/// Patch List �� �����Ѵ�
bool OpenPatchList (const char * FileName, std::vector<PatchEntry *> *vecEntry);

/// Patch List �� �����
void DeletePatchList (std::vector<PatchEntry *> *vecEntry);

/// Patch List �� ī���Ѵ� vecEntry1 ==> vecEntry2
bool CopyPatchList (std::vector<PatchEntry *> *vecEntry1, std::vector<PatchEntry *> *vecEntry2);

/// Patch List ���� �־��� ���Ͽ� ���� ��Ʈ���� ã�´�
PatchEntry * FindPatchEntry (std::vector<PatchEntry *> *vecEntry1, const char *FileName);

/// ��ġ����Ʈ�� ������ VFS������ �����
bool MakeVFS (const char * FileName, std::vector<PatchEntry *> *vecEntry, DWORD, DWORD);

/// Vfs ������ ����
bool OpenIDX (const char * FileName, std::vector<PatchEntry *> *vecEntry, DWORD *shStdVersion, DWORD *shCurVersion, bool bClose = true, VHANDLE * hVFS = NULL);

///
DWORD ver_atosh (const char *ver);

///
char * ver_shtoa (DWORD dwVersion, char * buff);

/// ��ġ����Ʈ�� ������ ����
extern std::vector<PatchEntry *>	g_vecStd;		/// ���ع���
extern std::vector<PatchEntry *>	g_vecCur;		/// �������
extern std::vector<PatchEntry *>	g_vecNext;		/// ��������

extern char * OPSTR[ 3 ] ;


#endif