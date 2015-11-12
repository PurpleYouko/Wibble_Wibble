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
 * 패치리스트를 다음에 저장
 */
std::vector<PatchEntry *>		g_vecStd;		/// 기준버젼
std::vector<PatchEntry *>		g_vecCur;		/// 현재버젼
std::vector<PatchEntry *>		g_vecNext;		/// 다음버젼


/********************************************************************************
 * PatchEntry를 파일에 저장한다
 */
void WritePatchEntry (PatchEntry *PE, FILE * fp)
{
	short shLength = 0; 

	/// 기준 버젼
	fwrite (&PE->dwStdVersion,	sizeof (DWORD), 1, fp);
	/// 팩파일명
	shLength = (short)(PE->PackFileName.size () + 1);	/// Pack파일 이름의 크기
	fwrite (&shLength, sizeof (short), 1, fp);
	fwrite (PE->PackFileName.c_str (), sizeof (char), PE->PackFileName.size (), fp);
	fwrite ("\0", sizeof (char), 1, fp);
	/// 파일명
	shLength = (short)(PE->FileName.size () + 1);		/// 
	fwrite (&shLength, sizeof (short), 1, fp);
	fwrite (PE->FileName.c_str (), sizeof (char), PE->FileName.size (), fp);
	fwrite ("\0", sizeof (char), 1, fp);
	/// 다운로드 파일명
	shLength = (short)(PE->DownName.size () + 1);		/// 
	fwrite (&shLength, sizeof (short), 1, fp);
	fwrite (PE->DownName.c_str (), sizeof (char), PE->DownName.size (), fp);
	fwrite ("\0", sizeof (char), 1, fp);

	fwrite (&PE->btOP, sizeof (BYTE), 1, fp);
	fwrite (&PE->dwVersion, sizeof (DWORD), 1, fp);
	fwrite (&PE->dwCRC,	sizeof (DWORD), 1, fp);
}


/********************************************************************************
 * PatchEntry를 파일에어 읽는다
 */
void ReadPatchEntry (PatchEntry *PE, FILE * fp)
{
	char * buff = NULL;
	short shLength = 0;

	fread (&PE->dwStdVersion	, sizeof (DWORD), 1, fp);

	fread (&shLength			, sizeof (short), 1, fp);	/// Pack파일 읽기
	buff = new char[ shLength ];							
	fread (buff, sizeof (char)	, shLength, fp);			
	PE->PackFileName = buff;								
	delete buff;	

	fread (&shLength			, sizeof (short), 1, fp);	/// 파일의 이름의 길이
	buff = new char[ shLength ];							/// 그만큼 메모리 할당
	fread (buff, sizeof (char)	, shLength, fp);			/// 파일이름 읽는다
	PE->FileName = buff;									/// 다시 string에 저장
	delete buff;											/// 메모리 해제

	fread (&shLength, sizeof (short), 1, fp);				/// 다운 파일 이름 읽기
	buff = new char[ shLength ];							
	fread (buff, sizeof (char), shLength, fp);				
	PE->DownName = buff;									
	delete buff;											

	fread (&PE->btOP, sizeof (BYTE)	, 1, fp);	///
	fread (&PE->dwVersion, sizeof (DWORD), 1, fp);	///
	fread (&PE->dwCRC, sizeof (DWORD), 1, fp);	///
}


/********************************************************************************
 * Patch List 를 저장한다
 */
bool SavePatchList (const char * FileName, std::vector<PatchEntry *> *vecEntry)
{
	FILE * fp = NULL;
	DWORD dwNum = 0;
	std::vector<PatchEntry *>::iterator iv;
	std::vector<std::string> vecPack;
	std::vector<std::string>::iterator il;
	std::vector<std::string>::iterator im;
	bool bMatch = true;							/// Pack 파일 리스트에 동일한 pack파일 이름 없으면 여기에 false

	/// 리스트내에 Pack파일을 뽑아낸다
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
		/// 같은 게 없었다면 위 루프에서 if를 한번도 만족하지 못했고, bMatch는 false이다
		if(bMatch == false) { vecPack.push_back ((*iv)->PackFileName); }
	}

	_fmode = _O_BINARY;
	if((fp = fopen (FileName, "w")))
	{
		dwNum = (DWORD)vecEntry->size ();
		fwrite (&dwNum, sizeof (DWORD), 1, fp);	/// 전체 갯수를 저장한다
		
		il = vecPack.begin ();
		for(; il != vecPack.end (); il++)
		{
			iv = vecEntry->begin ();
			for(; iv != vecEntry->end (); iv++)
			{
				/// PackFile 같은 거 끼리 쭉 저장하기 위해서
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
 * Patch List 를 오픈한다
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
 * Patch List 를 지운다
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
 * Patch List 를 카피한다 vecEntry1 ==> vecEntry2
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
			pFE->btOP			= (*iv)->btOP;				/// OP값
			pFE->dwCRC			= (*iv)->dwCRC;				/// CRC값
			pFE->PackFileName	= (*iv)->PackFileName;		/// PackFile 명
			pFE->FileName		= (*iv)->FileName;			/// 파일명
			pFE->dwStdVersion	= (*iv)->dwStdVersion;		/// 기준 버젼
			pFE->dwVersion		= (*iv)->dwVersion;			/// 이 파일의 버젼

			vecEntry2->push_back (pFE);
		}
		else
		{ return false;}
	}

	return true;
}


/********************************************************************************
 * Patch List 에서 주어진 파일에 대한 엔트리를 찾는다
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
 * 패치리스트를 가지고 VFS파일을 만든다
 */
bool MakeVFS (const char * FileName, std::vector<PatchEntry *> *vecEntry, DWORD dwStdVersion, DWORD dwCurVersion)
{
	VHANDLE hVFS = NULL;

	DWORD dwNum = 0;
	DWORD dwCRC = 0;								/// CRC값
	std::vector<PatchEntry *>::iterator iv;
	std::vector<std::string> vecPack;
	std::vector<std::string>::iterator il;
	bool bMatch = true;								/// Pack 파일 리스트에 동일한 pack파일 이름 없으면 여기에 false

	const char * IdxName = FileName;				/// IDX파일명

	if(FileName == NULL) { return false; }			/// 파일 이름이 없으면 안 만든다
	if(vecEntry->size () <= 0) { return false; }	/// 비었으면 안 만든다

	/// 리스트내에 Pack파일을 뽑아낸다
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
		/// 같은 게 없었다면 위 루프에서 if를 한번도 만족하지 못했고, bMatch는 false이다
		if(bMatch == false) { vecPack.push_back ((*iv)->PackFileName); }
	}


	if((hVFS = OpenVFS (IdxName, "w+")))
	{
		dwNum = (DWORD)vecEntry->size ();
		
		il = vecPack.begin ();
		for(; il != vecPack.end (); il++)
		{
			iv = vecEntry->begin ();
			if(VAddVfs (hVFS, (*il).c_str ())) /// Pack파일을 추가한다
			{
				for(; iv != vecEntry->end (); iv++)
				{
					/// PackFile 같은 거 끼리 쭉 저장하기 위해서
					if((*iv)->PackFileName == *il)
					{
						/// CRC값을 구한다
						dwCRC = getcrc_32_file ((*iv)->FileName.c_str ());
						/// 추가한다
						if(!VAddFile (hVFS, (*il).c_str (), (*iv)->FileName.c_str (), (*iv)->FileName.c_str (), (*iv)->dwVersion,
									dwCRC, 0, 0, true))
						{
							return false;	/// 실패하면 바로 false 리턴해 버린다
						}
					}
				}
			}
			else
			{ return false; }	/// VAddVfs에 실패하면 바로 fasle를 리턴해 버린다
		}
		::VSetStdVersion (hVFS, dwStdVersion);
		::VSetCurVersion (hVFS, dwCurVersion);
		CloseVFS (hVFS);

		return true;
	}

	return false;
}


/// nNum개의 nSize크기의 문자열을 리턴한다
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


/// nNum개의 nSize크기의 문자열을 해제한다
void __FreeString (char **ppString , int nNum)
{
	for(int i = 0; i < nNum; i++)
	{
		delete ppString[ i ];
		ppString[ i ] = NULL;
	}
}


/*********************************************************************************
 * VFS파일을 오픈한다
 */
bool OpenIDX (const char * FileName, std::vector<PatchEntry *> *vecEntry, DWORD *dwStdVersion, DWORD *dwCurVersion, bool bClose, VHANDLE * phVFS)
{
	char **ppVfs = NULL;
	char **ppFile = NULL;	/// Vfs안에 있는 파일이름을 저장한다
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

					/// 파일이름을 끄집어 낸다
					if((ppFile = new char *[nFile]))
					{
						if(!__AllocString (ppFile, nFile, 1024)) { return false; }
						::VGetFileNames (hVFS, ppVfs[ j ], ppFile, nFile, 1024);
					}
					else
					{ return false; } /// 메모리 할당 실패하면 false 리턴

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
		else /// 메모리할당 실패나 비었을 경우
		{ return false; }

		if(bClose)	{ CloseVFS (hVFS); *phVFS = NULL; }
		else		{ *phVFS = hVFS; }
	}

	return true;
}


/************************************************************************************
 * 문자열 버젼을 숫자 버젼으로 바꾸어 줌
 * 최고 15자 까지
 * 버젼 문자열은 xxx.xxx형식으로. major버젼은 상위비트에. minor 버젼은 하위버젼에
 * @return 유효하지 않은 버젼문자열일 경우 . -1을 리턴
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

	dwVersion		|= atoi (buff + iDotPos + 1);	/// 하위버젼은 하위비트에
	buff[ iDotPos ]	= 0;
	dwVersion		|= atoi (buff) << (sizeof(WORD) * 8);	/// 상위 버젼은 상위비트에
	
	return dwVersion;
}


/************************************************************************************
 * 숫자 버젼을 문자열 버젼으로 바꾸어 줌
 * 버젼 문자열은 xx.xx형식으로
 * @param buff 적어도 7자이상이어야 함
 * @return 유효하지 않은 버젼문자열일 경우 . -1을 리턴
 */
char * ver_shtoa (DWORD dwVersion, char * buff)
{
	/// shVersion이 음수이면 NULL을 리턴
	if(dwVersion == 0) { return NULL; }

	sprintf (buff, "%d.%d", ((WORD *)&dwVersion)[ 1 ], ((WORD *)&dwVersion)[ 0 ]);

	return buff;
}
