#include "stdafx.h"

#include <STDLIB.H>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <map>
#include <string>
#include <vector>
#include <fcntl.h>
#include <list>
#include <cassert>
#include "crc.h"
#include "__File.h"
#include "TriggerVFS.h"
#include "VFS.h"
#include "VFS_Manager.h"
#include "ErrorEtc.h"
#include "..\..\..\server\util\classHASH.h"

char UseEncryption;
/***********************************************************
 *
 * Global Variable and Global Function
 *
 */
short __stdcall VGetError (void )
{
	short nCurError = g_nLastErr;
	g_nLastErr = VERR_NONE;
	return nCurError ;
}


/*********************************************************************************
 *
 *
 *                                   Interface
 *
 *
 */


/*********************************************************************************
 * Index Files and Index Files open files that are managed by the Pack
 * @param FileName Index Filename
 * @param Mode Properties. "r" read only, "w" write-only (not Use), "w +" create + write + read, "r +" read + write (generated sequence)
 */
VHANDLE __stdcall OpenVFS (const char * FileName, const char * Mode)
{
 	CVFS_Manager * pVFS = NULL;
	//if(FileName[0]=='_')
	//{
	//	UseEncryption = 1;
	//	return NULL;
	//}
   	if(pVFS = new CVFS_Manager)
	{ 
		if(pVFS->Open (FileName, Mode)) 
			return (VHANDLE)pVFS;
		
		delete pVFS;
		pVFS = NULL;
	}
	
	return (VHANDLE)pVFS;
}


/*********************************************************************************
 * Index VFS File and close the file
 * @param hVFS Open the handle
 */
void __stdcall  CloseVFS (VHANDLE hVFS)
{
	CVFS_Manager * pVFS = (CVFS_Manager *)hVFS;
	if(pVFS) 
	{ 
		pVFS->Close (); 
		delete pVFS;
		pVFS = NULL;
	}
}


/*********************************************************************************
 * Information about the file is imported
 */
void __stdcall VGetFileInfo (VHANDLE hVFS, const char *FileName, VFileInfo * FileInfo, bool bCalCrc)
{
	CVFS_Manager * pVFS = (CVFS_Manager *)hVFS;

	if(pVFS && FileInfo)
		pVFS->GetFileInfo (FileName, FileInfo, bCalCrc);
}


/*********************************************************************************
 * Update information for files as
 */
bool __stdcall VSetFileInfo (VHANDLE hVFS, const char *FileName, VFileInfo * FileInfo)
{
	CVFS_Manager * pVFS = (CVFS_Manager *)hVFS;
	if(pVFS) 
	{ 
		return pVFS->SetFileInfo (FileName, FileInfo);
	}

	return false;
}


/*********************************************************************************
 * Index file, the file is added Pack.
 * The internally whatnot.
 */
bool __stdcall VAddVfs (VHANDLE hVFS, const char * VfsName)
{
	CVFS_Manager * pVFS = (CVFS_Manager *)hVFS;
	if(pVFS) 
	{ 
		return pVFS->AddVFS (VfsName);
	}

	return false;
}


/*********************************************************************************
 * Add a file
 * @param hVFS			VFS opened handle
 * @param VfsName		Pack a file name
 * @param FileName		Add the name of the file
 * @param TargetName	Filename register
 * @param shVersion 	Since
 * @param dwCRC			CRC value
 * @param btEncType 	Encript type. 0 = no. 1 =?
 * @param btCompress	Compressed form. 0 = None. 1 = Zip
 * @param bUseDel		Whether to use a deleted block. true = use. false = Disable
 */
short __stdcall VAddFile (VHANDLE hVFS
						 , const char * VfsName
						 , const char *FileName
						 , const char * TargetName
						 , DWORD dwVersion
						 , DWORD dwCrc
						 , BYTE btEncType
						 , BYTE btCompress
						 , bool bUseDel)
{
	CVFS_Manager * pVFS = (CVFS_Manager *)hVFS;
	if(pVFS) 
	{ 
		// ODS ("Add all files : %s\n", FileName );
		short nRes = pVFS->AddFile (VfsName
								, FileName
								, TargetName
								, dwVersion
								, dwCrc
								, btEncType
								, btCompress
								, bUseDel);
		// ODS ("After adding files : %s\n", FileName);
		
		if(nRes == VADDFILE_CANTWRITE || nRes == VADDFILE_CANTFLUSH )
		{
			DWORD dwErrCode = 0;
			switch ( nRes )
			{
			case VADDFILE_CANTWRITE:
				dwErrCode = CErrorInfo::GetError () + VADDFILE_FIXEVALUE_CANTWRITE ;
				break;

			case VADDFILE_CANTFLUSH:
				dwErrCode = CErrorInfo::GetError () + VADDFILE_FIXEVALUE_CANTFLUSH ;
				break;

			}
			
			if(dwErrCode <= 32767 && dwErrCode > 0)
				nRes = (short)dwErrCode ;
			else
				nRes = VADDFILE_FIOERROR_ETC;
			
		}
		return nRes;
	}

	return VADDFILE_INVALIDHANDLE ;
}


/**********************************************************************************************
 * Single file is removed. Since Im a white space handling. In order to clean up the file, call VClearBlank halgeot
 * Clear Pack without specifying a file
 * @param FileName Clear the file name
 */
short __stdcall VRemoveFile (VHANDLE hVFS, const char *FileName)
{
	CVFS_Manager * pVFS = (CVFS_Manager *)hVFS;

	if(pVFS) 
	{
		// ODS ( "Deleting all files : %s\n", FileName);
		short bRet = pVFS->RemoveFile (FileName);
		// ODS ( "After deleting files : %s\n", FileName);

		return bRet ;
	}

	
	return VRMVFILE_INVALIDHANDLE;
}


/**********************************************************************************************
 * Pack contains all the index file to remove space
 */
void __stdcall  VClearBlankAll (VHANDLE hVFS, VCALLBACK_CLEARBLANKALL CallBackProc)
{
	CVFS_Manager * pVFS = (CVFS_Manager *)hVFS;
	if(pVFS) 
		pVFS->ClearBlankAll ( CallBackProc );
}


/**********************************************************************************
 * Imported standard version
 */
DWORD __stdcall VGetStdVersion (VHANDLE hVFS)
{
	CVFS_Manager * pVFS = (CVFS_Manager *)hVFS;
	if(pVFS)
	{
		return pVFS->GetStdVersion ();
	}

	return 0;
}


/**********************************************************************************
 * Setting the standard version
 */
void __stdcall VSetStdVersion (VHANDLE hVFS, DWORD dwVersion)
{
	CVFS_Manager * pVFS = (CVFS_Manager *)hVFS;
	if(pVFS)
	{
		return pVFS->SetStdVersion (dwVersion);
	}
}


/**********************************************************************************
 * Bringing the current version of
 */
DWORD __stdcall VGetCurVersion (VHANDLE hVFS)
{
	CVFS_Manager * pVFS = (CVFS_Manager *)hVFS;
	if(pVFS)
	{
		return pVFS->GetCurVersion ();
	}

	return 0;
}


/**********************************************************************************
 * Set the current version
 */
void __stdcall VSetCurVersion (VHANDLE hVFS, DWORD dwVersion)
{
	CVFS_Manager * pVFS = (CVFS_Manager *)hVFS;
	if(pVFS)
	{
		return pVFS->SetCurVersion (dwVersion);
	}
}


/*********************************************************************************
 * Pack files within the index file to find out the number of
 */
int __stdcall VGetVfsCount (VHANDLE hVFS)
{
	CVFS_Manager * pVFS = (CVFS_Manager *)hVFS;
	if(pVFS)
	{
		return pVFS->GetVFSCount ();
	}

	return 0;
}


/*********************************************************************************
 * Pack brings the list of files in the file index
 * @param ppFiles
 * @param dwNum			Get the name of a number of
 * @param dwMaxPath		The maximum length that can be stored (NULL included)
 * @return				Find out the actual number of the file name
 */
int __stdcall VGetVfsNames (VHANDLE hVFS, char **ppFiles, DWORD dwNum, short dwMaxPath)
{
	CVFS_Manager * pVFS = (CVFS_Manager *)hVFS;
	if(pVFS)
	{
		return pVFS->GetVfsNames (ppFiles, dwNum, dwMaxPath);
	}

	return 0;
}


/*********************************************************************************
 * Vfs file and find out the number of files within
 */
int __stdcall VGetFileCount (VHANDLE hVFS, const char *VfsName)
{
	CVFS_Manager * pVFS = (CVFS_Manager *)hVFS;
	if(pVFS)
	{
		return pVFS->GetFileCount (VfsName);
	}

	return 0;
}


/*********************************************************************************
 * Vfs file and find out the number of files within
 */
int __stdcall VGetTotFileCount (VHANDLE hVFS)
{
	CVFS_Manager * pVFS = (CVFS_Manager *)hVFS;
	if(pVFS)
	{
		return pVFS->GetFileTotCount ();
	}

	return 0;
}


/************************************************************************************
 * Pack brings the file name in the file
 */
int __stdcall VGetFileNames (VHANDLE hVFS, const char *VfsName, char **FileName, int nNum, int nMax)
{
	CVFS_Manager * pVFS = (CVFS_Manager *)hVFS;
	if(pVFS)
	{
		return pVFS->GetFileNames (VfsName, FileName, nNum, nMax);
	}

	return 0;
}


/// ----------------------- Pack Work-related files in the file ---------------------------


/*********************************************************************************
 * Open the file
 */
VFileHandle * __stdcall VOpenFile (const char * FileName, VHANDLE hVFS)
{
	CVFS_Manager * pVFS = (CVFS_Manager *)hVFS;

	if(pVFS) 
	{ 
		return pVFS->OpenFile (FileName); 
	}

	return NULL;
}

/*********************************************************************************
 * Close the file
 */
void __stdcall VCloseFile (VFileHandle *hVFH)
{
	if(hVFH) 
	{ 
		if(hVFH->btFileType) { fclose (hVFH->fp); }
		
		if (hVFH->hCVFS) {
			CVFS * pVFS = reinterpret_cast<CVFS *>(hVFH->hCVFS);
			pVFS->CloseFile(hVFH); // includes delete pVFH
		}
		else {
	       delete hVFH;
		}
		hVFH = NULL;
	}	
}


/*********************************************************************************
 * Returns the size of the file (1)
 */
size_t __stdcall vfgetsize (VFileHandle *pVFH)
{
	if(pVFH) { return (size_t)(pVFH->lEndOff - pVFH->lStartOff); }

	return 0;
}


/*********************************************************************************
 * Returns the size of the file (2)
 */
size_t __stdcall VGetFileLength (VHANDLE hVFS, const char *FileName)
{
	/// long lSize = 0;

	CVFS_Manager * pVFS = (CVFS_Manager *)hVFS;
	if(pVFS)
	{
		return pVFS->GetFileLength (FileName);
		/// if(lSize > 0) { return lSize; }
	}

	return 0;
}


/*********************************************************************************
 * To check whether a file exists
 */
bool __stdcall VFileExists (VHANDLE hVFS, const char *FileName)
{
	CVFS_Manager * pVFS = (CVFS_Manager *)hVFS;
	if(pVFS)
	{
		return pVFS->FileExists (FileName);
	}
	return false;
}


/*********************************************************************************
 * Pack files to check whether a file exists only in
 */
bool __stdcall VFileExistsInVfs (VHANDLE hVFS, const char * FileName)
{
	CVFS_Manager * pVFS = (CVFS_Manager *)hVFS;
	if(pVFS)
	{
		return pVFS->FileExistsInVfs (FileName);
	}

	return false;
}


/*********************************************************************************
 * vfs file to read certain files
 * @param buffer Buffer to store the data read
 * @param size The size of the item
 * @param count The number of read entries
 */
size_t __stdcall vfread (void *buffer, size_t size, size_t count, VFileHandle *pVFH)
{
	size_t stReadSize	= size * count; /// The number of bytes to read
	size_t stReaded		= 0; 			/// Read one byte at a time was the number of bytes actually read
	size_t stItems		= 0; 			/// You can read this item

	long lOffset = pVFH->lStartOff + pVFH->lCurrentOffset;

	CVFS_Manager * pVFS = (CVFS_Manager *)pVFH->hVFS;

	/// Make sure the end of the file exceeds the
	long lToWhere = pVFH->lStartOff + pVFH->lCurrentOffset + stReadSize;
	if(lToWhere >= (signed)(pVFH->lEndOff))
		stReadSize = pVFH->lEndOff - (pVFH->lStartOff + pVFH->lCurrentOffset);
	if(pVFH->pData) 
	{
		stReaded = sizeof(BYTE)*stReadSize;
		memcpy(buffer, pVFH->pData + pVFH->iAllocOffset + pVFH->lCurrentOffset, stReaded);
	}
	else
	{
		fseek (pVFH->fp, lOffset, SEEK_SET);
		stReaded = fread (buffer, sizeof (BYTE), stReadSize, pVFH->fp);
	}
//#define KEYSIZE 0x1000
//	long Mpointer;
	/// Reads data
	//if(pVFH->pData) 
	//{
	//	stReaded = sizeof(BYTE)*stReadSize;
	//	memcpy(buffer, pVFH->pData + pVFH->iAllocOffset + pVFH->lCurrentOffset, stReaded);
	//	
	//	if(pVFH->btEncrypted && UseEncryption==1)
	//	{
	//	//	MessageBox( NULL, "decrypting", "TRose", MB_OK | MB_ICONERROR);
	//		const char *hashstring = pVFH->sFileName.c_str();
	//		unsigned long key = StrToHashKey(hashstring);
	//		char crypttable[KEYSIZE];
	//		DWORD *EAX=reinterpret_cast<DWORD*>(crypttable);
	//		DWORD EDI=0;
	//		DWORD ECX = key;
	//		DWORD EDX = key+1;
	//		ECX = ECX*4+1;
	//		for(int i = 0; i < 0x400; i++)
	//		{
	//			EDI=ECX*EDX;
	//			*((DWORD*)EAX) = EDI;
	//			EAX++;
	//			ECX+=4;
	//			EDX++;
	//		}

	//		Mpointer = (pVFH->lCurrentOffset) % KEYSIZE;
	//		for(size_t k=0;k<stReaded;k++)
	//		{
	//			*((char*)buffer+k)^= crypttable[Mpointer];
	//			Mpointer++; Mpointer %= KEYSIZE;
	//		}
	//	}
	//}
	//else
	//{
	//	fseek (pVFH->fp, lOffset, SEEK_SET);
	//	stReaded = fread (buffer, sizeof (BYTE), stReadSize, pVFH->fp);
	//	if (!pVFH->btFileType)
	//	{
	//		if(pVFH->btEncrypted && UseEncryption==1)
	//		{
	//	//		MessageBox( NULL, "decrypting", "TRose", MB_OK | MB_ICONERROR);
	//			const char *hashstring = pVFH->sFileName.c_str();
	//			unsigned long key = StrToHashKey(hashstring);

	//			char crypttable[KEYSIZE];
	//			DWORD *EAX=reinterpret_cast<DWORD*>(crypttable);
	//			DWORD EDI=0;
	//			DWORD ECX = key;
	//			DWORD EDX = key+1;
	//			ECX = ECX*4+1;
	//			for(int i = 0; i < 0x400; i++)
	//			{
	//				EDI=ECX*EDX;
	//				*((DWORD*)EAX) = EDI;
	//				EAX++;
	//				ECX+=4;
	//				EDX++;
	//			}

	//			Mpointer = (pVFH->lCurrentOffset) % KEYSIZE;
	//			for(size_t k=0;k<stReaded;k++)
	//			{
	//				*((char*)buffer+k)^= crypttable[Mpointer];
	//				Mpointer++; Mpointer %= KEYSIZE;
	//			}
	//		}
	//	}
	//}

	/// Move the current file offset to read as sets
	pVFH->lCurrentOffset += (long)stReaded;
	/// = The number of bytes read can read this item / size of the item
	stItems = stReaded / size;

	return stItems;
}

/*********************************************************************************
 * All information on the file to obtain a pointer.
 * Not release the pointer is directly obtained.
 * @psize Used to obtain the size.
 * @pVFH File handle
 * @return Pointer
 */
void * __stdcall vfgetdata (size_t * psize, VFileHandle *pVFH)
{
	long lOffset = pVFH->lStartOff + pVFH->lCurrentOffset;

	CVFS_Manager * pVFS = (CVFS_Manager *)pVFH->hVFS;

	/// Reads data
	if (!pVFS) {
		return NULL;
	}

	if (psize) {
		*psize = pVFH->lEndOff - pVFH->lStartOff;
	}
	return pVFH->pData + pVFH->iAllocOffset;
}

/*********************************************************************************
 * File indicator is moved. The file fseek occupies the same as in the range.
 * @return fseek same
 */
int __stdcall vfseek (VFileHandle *pVFH, long offset, int origin )
{
	int iRet = 0;
	long lOffset = 0;

	/// Never exceed the scope of this file should be let.
	switch (origin)
	{
	case VFSEEK_SET:
		/// Lower than the upper limit value is set to the newly prevent
		lOffset = pVFH->lStartOff + offset;
		if(lOffset < pVFH->lStartOff)	{ lOffset = pVFH->lStartOff; }
		if(lOffset > pVFH->lEndOff)		{ lOffset = pVFH->lEndOff; }
		pVFH->lCurrentOffset = lOffset - pVFH->lStartOff;
		break;

	case VFSEEK_CUR:
		lOffset = pVFH->lStartOff + pVFH->lCurrentOffset + offset;
		if(lOffset < pVFH->lStartOff)	{ lOffset = pVFH->lStartOff; }
		if(lOffset > pVFH->lEndOff)		{ lOffset = pVFH->lEndOff; }
		pVFH->lCurrentOffset = lOffset - pVFH->lStartOff;
		break;

	case VFSEEK_END:
		lOffset = pVFH->lEndOff + offset;
		if(lOffset < pVFH->lStartOff)	{ lOffset = pVFH->lStartOff; }
		if(lOffset > pVFH->lEndOff)		{ lOffset = pVFH->lEndOff; }
		pVFH->lCurrentOffset = lOffset - pVFH->lStartOff;
		break;
	}

	CVFS_Manager * pVFS = (CVFS_Manager *)pVFH->hVFS;

	if (!pVFS) { // If a regular file
		iRet = fseek (pVFH->fp, lOffset, SEEK_SET); /// Actually perform fseek
	}

	return iRet;
}


/*********************************************************************************
 * Ftell returns a file with the same indicator value, but within the range of the file occupies.
 */
long __stdcall vftell (VFileHandle *pVFH)
{
	/// Indicator that the current position of the opened file is transferred
	vfseek (pVFH, 0, VFSEEK_CUR);
	/// And return. Location indicator of the relative position of the current value Im
	return pVFH->lCurrentOffset;
}


/*********************************************************************************
 * feof same. 1 if the end of file, and returns 0 otherwise
 */
int __stdcall vfeof (VFileHandle *pVFH)
{
	return (pVFH->lStartOff + pVFH->lCurrentOffset == pVFH->lEndOff ? 1 : 0);
}


/********************************************************************************
 * FileName VFS for index information and the actual data matches the check
 */
short __stdcall VTestFile (VHANDLE hVFS, const char * FileName)
{
	CVFS_Manager * pVFS = (CVFS_Manager *)hVFS;
	if(pVFS)
		return pVFS->TestFile (FileName);

	return VTEST_INVALIDHANDLE;
}

/********************************************************************************
 * FileName VFS for index information and the actual data matches the check
 */
DWORD __stdcall ComputeCrc (VHANDLE hVFS, const char * FileName)
{
	CVFS_Manager * pVFS = (CVFS_Manager *)hVFS;
	if(pVFS)
		return pVFS->ComputeCrc (FileName);

	return 0;
}