#include "stdafx.h"

#include <vector>
#include <string>
#include <cassert>
#include <fcntl.h>
#include <stdlib.h>
#include "TriggerVFS.h"
#include "g_patch.h"
#include "crc.h"


char * OPSTR[ 3 ] = {"ADD", "DEL", "EXCH"};

/********************************************************************************
 * ��ġ����Ʈ�� ������ ����
 */
std::vector<PatchEntry *>		g_vecStd;		/// ���ع���
std::vector<PatchEntry *>		g_vecCur;		/// �������
std::vector<PatchEntry *>		g_vecNext;		/// ��������


/********************************************************************************
 * PatchEntry�� ���Ͽ� �����Ѵ�
 */
void WritePatchEntry (PatchEntry *PE, FILE * fp)
{
	short shLength = 0; 

	/// ���� ����
	fwrite (&PE->dwStdVersion,	sizeof (DWORD), 1, fp);
	/// �����ϸ�
	shLength = (short)(PE->PackFileName.size () + 1);	/// Pack���� �̸��� ũ��
	fwrite (&shLength, sizeof (short), 1, fp);
	fwrite (PE->PackFileName.c_str (), sizeof (char), PE->PackFileName.size (), fp);
	fwrite ("\0", sizeof (char), 1, fp);
	/// ���ϸ�
	shLength = (short)(PE->FileName.size () + 1);		/// 
	fwrite (&shLength, sizeof (short), 1, fp);
	fwrite (PE->FileName.c_str (), sizeof (char), PE->FileName.size (), fp);
	fwrite ("\0", sizeof (char), 1, fp);
	/// �ٿ�ε� ���ϸ�
	shLength = (short)(PE->DownName.size () + 1);		/// 
	fwrite (&shLength, sizeof (short), 1, fp);
	fwrite (PE->DownName.c_str (), sizeof (char), PE->DownName.size (), fp);
	fwrite ("\0", sizeof (char), 1, fp);

	fwrite (&PE->btOP, sizeof (BYTE), 1, fp);
	fwrite (&PE->dwVersion, sizeof (DWORD), 1, fp);
	fwrite (&PE->dwCRC,	sizeof (DWORD), 1, fp);
}


/********************************************************************************
 * PatchEntry�� ���Ͽ��� �д´�
 */
void ReadPatchEntry (PatchEntry *PE, FILE * fp)
{
	char * buff = NULL;
	short shLength = 0;

	fread (&PE->dwStdVersion	, sizeof (DWORD), 1, fp);

	fread (&shLength			, sizeof (short), 1, fp);	/// Pack���� �б�
	buff = new char[ shLength ];							
	fread (buff, sizeof (char)	, shLength, fp);			
	PE->PackFileName = buff;								
	delete buff;	

	fread (&shLength			, sizeof (short), 1, fp);	/// ������ �̸��� ����
	buff = new char[ shLength ];							/// �׸�ŭ �޸� �Ҵ�
	fread (buff, sizeof (char)	, shLength, fp);			/// �����̸� �д´�
	PE->FileName = buff;									/// �ٽ� string�� ����
	delete buff;											/// �޸� ����

	fread (&shLength, sizeof (short), 1, fp);				/// �ٿ� ���� �̸� �б�
	buff = new char[ shLength ];							
	fread (buff, sizeof (char), shLength, fp);				
	PE->DownName = buff;									
	delete buff;											

	fread (&PE->btOP, sizeof (BYTE)	, 1, fp);	///
	fread (&PE->dwVersion, sizeof (DWORD), 1, fp);	///
	fread (&PE->dwCRC, sizeof (DWORD), 1, fp);	///
}


/********************************************************************************
 * Patch List �� �����Ѵ�
 */
bool SavePatchList (const char * FileName, std::vector<PatchEntry *> *vecEntry)
{
	FILE * fp = NULL;
	DWORD dwNum = 0;
	std::vector<PatchEntry *>::iterator iv;
	std::vector<std::string> vecPack;
	std::vector<std::string>::iterator il;
	std::vector<std::string>::iterator im;
	bool bMatch = true;							/// Pack ���� ����Ʈ�� ������ pack���� �̸� ������ ���⿡ false

	/// ����Ʈ���� Pack������ �̾Ƴ���
	iv = vecEntry->begin ();
	for(; iv != vecEntry->end (); iv++)
	{
		bMatch = false;
		il = vecPack.begin ();
		for(; il != vecPack.end (); il++)
		{
			if((*iv)->PackFileName == *il) 
			{ bMatch = true; break; }
		}
		/// ���� �� �����ٸ� �� �������� if�� �ѹ��� �������� ���߰�, bMatch�� false�̴�
		if(bMatch == false) { vecPack.push_back ((*iv)->PackFileName); }
	}

	_fmode = _O_BINARY;
	if((fp = fopen (FileName, "w")))
	{
		dwNum = (DWORD)vecEntry->size ();
		fwrite (&dwNum, sizeof (DWORD), 1, fp);	/// ��ü ������ �����Ѵ�
		
		il = vecPack.begin ();
		for(; il != vecPack.end (); il++)
		{
			iv = vecEntry->begin ();
			for(; iv != vecEntry->end (); iv++)
			{
				/// PackFile ���� �� ���� �� �����ϱ� ���ؼ�
				if((*iv)->PackFileName == *il)
				{
					WritePatchEntry (*iv, fp);
				}
			}
		}
		fclose (fp);

		return true;
	}

	return false;
}


/********************************************************************************
 * Patch List �� �����Ѵ�
 */
bool OpenPatchList (const char * FileName, std::vector<PatchEntry *> *vecEntry)
{
	FILE * fp = NULL;
	DWORD dwNum = 0;
	std::vector<PatchEntry *>::iterator iv;
	PatchEntry * pFE = NULL;

	_fmode = _O_BINARY;
	if((fp = fopen (FileName, "r")))
	{
		fread (&dwNum, sizeof (DWORD), 1, fp);

		for(DWORD i = 0; i < dwNum; i++)
		{
			if((pFE = new PatchEntry))
			{
				ReadPatchEntry (pFE , fp);
				vecEntry->push_back (pFE);
			}
			else 
			{ return false; }
		}
		
		fclose (fp);

		return true;
	}

	return false;
}


/********************************************************************************
 * Patch List �� �����
 */
void DeletePatchList (std::vector<PatchEntry *> *vecEntry)
{
	std::vector<PatchEntry *>::reverse_iterator iv ; 

	iv = vecEntry->rbegin ();
	for(; iv != vecEntry->rend (); iv++)
	{
		delete *iv;
	}

	vecEntry->clear ();
}


/********************************************************************************
 * Patch List �� ī���Ѵ� vecEntry1 ==> vecEntry2
 */
bool CopyPatchList (std::vector<PatchEntry *> *vecEntry1, std::vector<PatchEntry *> *vecEntry2)
{
	PatchEntry * pFE = NULL;
	std::vector<PatchEntry *>::iterator iv ; 

	iv = vecEntry1->begin ();
	for(; iv != vecEntry1->end (); iv++)
	{
		if((pFE = new PatchEntry))
		{
			pFE->btOP			= (*iv)->btOP;				/// OP��
			pFE->dwCRC			= (*iv)->dwCRC;				/// CRC��
			pFE->PackFileName	= (*iv)->PackFileName;		/// PackFile ��
			pFE->FileName		= (*iv)->FileName;			/// ���ϸ�
			pFE->dwStdVersion	= (*iv)->dwStdVersion;		/// ���� ����
			pFE->dwVersion		= (*iv)->dwVersion;			/// �� ������ ����

			vecEntry2->push_back (pFE);
		}
		else
		{ return false;}
	}

	return true;
}


/********************************************************************************
 * Patch List ���� �־��� ���Ͽ� ���� ��Ʈ���� ã�´�
 */
PatchEntry * FindPatchEntry (std::vector<PatchEntry *> *vecEntry1, const char *FileName)
{
	std::vector<PatchEntry *>::iterator iv ; 

	iv = vecEntry1->begin ();
	for(; iv != vecEntry1->end (); iv++)
	{
		if((*iv)->FileName == FileName) { return *iv; }
	}

	return NULL;
}


/*********************************************************************************
 * ��ġ����Ʈ�� ������ VFS������ �����
 */
bool MakeVFS (const char * FileName, std::vector<PatchEntry *> *vecEntry, DWORD dwStdVersion, DWORD dwCurVersion)
{
	VHANDLE hVFS = NULL;

	DWORD dwNum = 0;
	DWORD dwCRC = 0;								/// CRC��
	std::vector<PatchEntry *>::iterator iv;
	std::vector<std::string> vecPack;
	std::vector<std::string>::iterator il;
	bool bMatch = true;								/// Pack ���� ����Ʈ�� ������ pack���� �̸� ������ ���⿡ false

	const char * IdxName = FileName;				/// IDX���ϸ�

	if(FileName == NULL) { return false; }			/// ���� �̸��� ������ �� �����
	if(vecEntry->size () <= 0) { return false; }	/// ������� �� �����

	/// ����Ʈ���� Pack������ �̾Ƴ���
	iv = vecEntry->begin ();
	for(; iv != vecEntry->end (); iv++)
	{
		bMatch = false;
		il = vecPack.begin ();
		for(; il != vecPack.end (); il++)
		{
			if((*iv)->PackFileName == *il) 
			{ bMatch = true; break; }
		}
		/// ���� �� �����ٸ� �� �������� if�� �ѹ��� �������� ���߰�, bMatch�� false�̴�
		if(bMatch == false) { vecPack.push_back ((*iv)->PackFileName); }
	}


	if((hVFS = OpenVFS (IdxName, "w+")))
	{
		dwNum = (DWORD)vecEntry->size ();
		
		il = vecPack.begin ();
		for(; il != vecPack.end (); il++)
		{
			iv = vecEntry->begin ();
			if(VAddVfs (hVFS, (*il).c_str ())) /// Pack������ �߰��Ѵ�
			{
				for(; iv != vecEntry->end (); iv++)
				{
					/// PackFile ���� �� ���� �� �����ϱ� ���ؼ�
					if((*iv)->PackFileName == *il)
					{
						/// CRC���� ���Ѵ�
						dwCRC = getcrc_32_file ((*iv)->FileName.c_str ());
						/// �߰��Ѵ�
						if(!VAddFile (hVFS, (*il).c_str (), (*iv)->FileName.c_str (), (*iv)->FileName.c_str (), (*iv)->dwVersion,
									dwCRC, 0, 0, true))
						{
							return false;	/// �����ϸ� �ٷ� false ������ ������
						}
					}
				}
			}
			else
			{ return false; }	/// VAddVfs�� �����ϸ� �ٷ� fasle�� ������ ������
		}
		::VSetStdVersion (hVFS, dwStdVersion);
		::VSetCurVersion (hVFS, dwCurVersion);
		CloseVFS (hVFS);

		return true;
	}

	return false;
}


/// nNum���� nSizeũ���� ���ڿ��� �����Ѵ�
bool __AllocString (char **ppString , int nNum, int nSize)
{
	for(int i = 0; i < nNum; i++)
	{
		ppString[ i ] = NULL;
	}

	for(int i = 0; i < nNum; i++)
	{
		if(!(ppString[ i ] = new char[ nSize ])) { return false; }
	}

	return true;
}


/// nNum���� nSizeũ���� ���ڿ��� �����Ѵ�
void __FreeString (char **ppString , int nNum)
{
	for(int i = 0; i < nNum; i++)
	{
		delete ppString[ i ];
		ppString[ i ] = NULL;
	}
}


/*********************************************************************************
 * VFS������ �����Ѵ�
 */
bool OpenIDX (const char * FileName, std::vector<PatchEntry *> *vecEntry, DWORD *dwStdVersion, DWORD *dwCurVersion, bool bClose, VHANDLE * phVFS)
{
	char **ppVfs = NULL;
	char **ppFile = NULL;	/// Vfs�ȿ� �ִ� �����̸��� �����Ѵ�
	VHANDLE hVFS = NULL;

	if(_access (FileName, 00) != 0) { return false; }

	if((hVFS = OpenVFS (FileName, "r+")))
	{
		*dwStdVersion = ::VGetStdVersion (hVFS);
		*dwCurVersion = ::VGetCurVersion (hVFS);
		int nVfs = ::VGetVfsCount (hVFS);
		ppVfs = new char * [ nVfs ];
		if(nVfs > 0 && ppVfs)
		{
			if(!__AllocString (ppVfs, nVfs, 1024)) { return false; }
			
			int nGet = 0;
			if((nGet = ::VGetVfsNames (hVFS, ppVfs, nVfs, 1024)) > 0)
			{
				for(int j = 0; j < nGet; j++)
				{
					int nFile = ::VGetFileCount (hVFS, ppVfs[ j ]);

					/// �����̸��� ������ ����
					if((ppFile = new char *[nFile]))
					{
						if(!__AllocString (ppFile, nFile, 1024)) { return false; }
						::VGetFileNames (hVFS, ppVfs[ j ], ppFile, nFile, 1024);
					}
					else
					{ return false; } /// �޸� �Ҵ� �����ϸ� false ����

					for(int k = 0; k < nFile; k++)
					{
                        PatchEntry *pPE = new PatchEntry; assert (pPE);

						VFileInfo VInfo;
						VGetFileInfo (hVFS, ppFile[ k ], &VInfo, false);
						pPE->dwCRC		= VInfo.dwCRC;
						pPE->dwVersion	= VInfo.dwVersion;
						pPE->dwStdVersion = *dwStdVersion;
												
						pPE->FileName = ppFile[ k ];
						pPE->PackFileName = ppVfs[ j ];

						vecEntry->push_back (pPE);
					}

					__FreeString (ppFile, nFile);
					delete ppFile; ppFile = NULL;
				}
			}
			
			__FreeString (ppVfs, nVfs);
			delete ppVfs; ppVfs = NULL;
		}
		else /// �޸��Ҵ� ���г� ����� ���
		{ return false; }

		if(bClose)	{ CloseVFS (hVFS); *phVFS = NULL; }
		else		{ *phVFS = hVFS; }
	}

	return true;
}


/************************************************************************************
 * ���ڿ� ������ ���� �������� �ٲپ� ��
 * �ְ� 15�� ����
 * ���� ���ڿ��� xxx.xxx��������. major������ ������Ʈ��. minor ������ ����������
 * @return ��ȿ���� ���� �������ڿ��� ��� . -1�� ����
 */
DWORD ver_atosh (const char *ver)
{
	char	buff[ 16 ];
	int		dwVersion = 0;
	int		iDotPos = 0;

	strcpy (buff, ver);
	
	for(unsigned int i = 0; i < strlen (buff); i++)
	{
		if(buff[ i ] == '.') { iDotPos = i; break; }
	}

	if(iDotPos >= (signed)strlen(buff)) { return 0; }
	if(strlen ( ver + iDotPos + 1 ) != 3 ) { return 0; }

	dwVersion		|= atoi (buff + iDotPos + 1);	/// ���������� ������Ʈ��
	buff[ iDotPos ]	= 0;
	dwVersion		|= atoi (buff) << (sizeof(WORD) * 8);	/// ���� ������ ������Ʈ��
	
	return dwVersion;
}


/************************************************************************************
 * ���� ������ ���ڿ� �������� �ٲپ� ��
 * ���� ���ڿ��� xx.xx��������
 * @param buff ��� 7���̻��̾�� ��
 * @return ��ȿ���� ���� �������ڿ��� ��� . -1�� ����
 */
char * ver_shtoa (DWORD dwVersion, char * buff)
{
	/// shVersion�� �����̸� NULL�� ����
	if(dwVersion == 0) { return NULL; }

	sprintf (buff, "%d.%d", ((WORD *)&dwVersion)[ 1 ], ((WORD *)&dwVersion)[ 0 ]);

	return buff;
}
