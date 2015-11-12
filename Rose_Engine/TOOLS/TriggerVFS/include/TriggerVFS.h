#ifndef __TRIGGERVFS_H__
#define __TRIGGERVFS_H__

#include <windows.h>
#include <io.h>
#include <map>
#include <string>
#include <vector>
#include "DllMain.h"
#include <stdio.h>

#pragma warning (disable:4251)


#ifdef __DLL

	#ifdef TRIGGERVFS_EXPORTS
		#define TRIGGERVFS_API __declspec(dllexport)
	#else
		#define TRIGGERVFS_API __declspec(dllimport)
	#endif

#else
	#define TRIGGERVFS_API
#endif




#define		VHANDLE		void *
typedef		 void   (__stdcall *  VCALLBACK_CLEARBLANKALL) ( int );

/// Save the file entry. In the beginning of the file vfs file
struct FileEntry 
{
	std::string		sFileName;			/// File name; length: string (NULL included) Saved form
	long			lFileOffset;		/// File offset
	long			lFileLength;		/// The actual length of the file occupies
	long			lBlockSize;			/// Beulreuk occupy the size (more 19.11.2003)
	BYTE			cDeleted;			/// Delete Status: 0 = Not Delete. 1 = Delete (11/19/2003 added)
	BYTE			btCompressType;		/// Compressed form. 0 = no compression, 1 = ZIP ...
	BYTE			btEncType;			/// Encryption type. 0 = Ecryption Disable. 1 = Encryption Type (1) ...
	DWORD			dwVersion;			/// Version of the file
	DWORD			dwCRC;				/// CRC value
};

/// #define	SIZEOF_FILEENTRY_EXCPTSTRING	(sizeof (short) + sizeof (long) * 3 + sizeof (BYTE) * 3 + sizeof (short) + sizeof (DWORD))

/********************************************************************************
 * VFileInfo CRC value for a file to get information and clearance by Version
 */
struct VFileInfo
{
	DWORD dwVersion;				/// Since the value
	DWORD dwCRC;					/// CRC Value
};



/// vfs file pointer for file operations in
struct VFileHandle
{
	FILE	*		fp;				/// vfs file handle of the file. Copy the VFile Stay
	char *			pData;			// Memory Mapped IO data
	int				iAllocOffset;	// pData + iAllocOffset = Read Data Address
	std::string		sFileName;		/// The file name of the currently open. vfs file name in the file.
	long			lStartOff;		/// Offset the start of the current file
	long			lEndOff;		/// The end of the current file Offset. dwEndOff + 1 is the End Of File.
	long			lCurrentOffset;	/// Current File Indicator Offset. Start offset by his
	BYTE			btFileType;		/// 0 = Packed File , 1 = Normal File outside
	BYTE			btEncrypted;	// 0 = normal data , 1 = data encryption used
	VHANDLE			hVFS;
	VHANDLE			hCVFS;			// the pointer to CVFS by which this file handle was opened.
} ;


/**********************************************************************************************
 *                                      VFS Interface
 **********************************************************************************************
 */
extern char UseEncryption;
#ifdef	__cplusplus
extern "C" {
#endif

/**
 * Constant for vfseek
 */
#define VFSEEK_CUR SEEK_CUR /// You are here
#define VFSEEK_END SEEK_END /// End of file
#define VFSEEK_SET SEEK_SET	/// Front of the file

TRIGGERVFS_API bool __stdcall __ConvertPath (const char * path , char * path2 );

/// VFS Index File and open the file. Possible modes : "w+", "r+", "r"
TRIGGERVFS_API VHANDLE __stdcall OpenVFS (const char * FileName, const char * Mode = "r");

/// VFS IIndex File and close the file
TRIGGERVFS_API void __stdcall CloseVFS (VHANDLE hVFS);

/// Index file, the file is added Pack.
TRIGGERVFS_API bool __stdcall VAddVfs (VHANDLE hVFS, const char * VfsName);

/// Add a file
TRIGGERVFS_API short __stdcall VAddFile (VHANDLE hVFS, const char * VfsName, const char *FileName, const char * TargetName, DWORD dwVersion, DWORD dwCrc, 
			   BYTE btEncType, BYTE btCompress, bool bUseDel);

/// Removes the file. Since Im a white space handling. In order to clean up the file, call VClearBlankAll halgeot
/// Clear Pack without specifying a file
TRIGGERVFS_API short __stdcall VRemoveFile (VHANDLE hVFS, const char *FileName);

/// Pack contains all the index file to remove space
TRIGGERVFS_API void __stdcall VClearBlankAll (VHANDLE hVFS, VCALLBACK_CLEARBLANKALL CallBackProc = NULL);

/// Brings the information about the file
TRIGGERVFS_API void __stdcall VGetFileInfo (VHANDLE hVFS, const char *FileName, VFileInfo * FileInfo, bool bCalCrc);

/// And updates the file information
TRIGGERVFS_API bool __stdcall VSetFileInfo (VHANDLE hVFS, const char *FileName, VFileInfo * FileInfo);

/// Pack files within the index file to find out the number of
TRIGGERVFS_API int __stdcall VGetVfsCount (VHANDLE hVFS);

/// Pack brings the list of files in the file index
TRIGGERVFS_API int __stdcall VGetVfsNames (VHANDLE hVFS, char **ppFiles, DWORD dwNum, short dwMaxPath);

/// Vfs file and find out the number of files within
TRIGGERVFS_API int __stdcall VGetFileCount (VHANDLE hVFS, const char *VfsName);
/// In the index file and find out the total number of files.
TRIGGERVFS_API int __stdcall VGetTotFileCount (VHANDLE hVFS);

/// Pack brings the file name in the file
TRIGGERVFS_API int __stdcall VGetFileNames (VHANDLE hVFS, const char *VfsName, char **FileName, int nNum, int nMax);

/// Imported standard version
TRIGGERVFS_API DWORD __stdcall VGetStdVersion (VHANDLE hVFS);

/// Setting the standard version of the index file
TRIGGERVFS_API void __stdcall VSetStdVersion (VHANDLE hVFS, DWORD dwVersion);

/// Bringing the current version of
TRIGGERVFS_API DWORD __stdcall VGetCurVersion (VHANDLE hVFS);

/// Set the current version of the index file
TRIGGERVFS_API void __stdcall VSetCurVersion (VHANDLE hVFS, DWORD dwVersion);




/// Open the file
TRIGGERVFS_API VFileHandle * __stdcall VOpenFile (const char * FileName, VHANDLE hVFS);

/// Close the file
TRIGGERVFS_API void __stdcall VCloseFile (VFileHandle *hVFH);

/// When reading a file
TRIGGERVFS_API size_t __stdcall vfread (void *buffer, size_t size, size_t count, VFileHandle *pVFH);

/// Acquiring the entire contents of the file pointer
TRIGGERVFS_API void * __stdcall vfgetdata (size_t * psize, VFileHandle *pVFH);

/// File indicator of the location of mobile
TRIGGERVFS_API int __stdcall vfseek (VFileHandle *pVFH, long offset, int origin );

/// Open the file position indicator value is returned. In fact, the File Offset value is maybe vftell be returned.
TRIGGERVFS_API long __stdcall vftell (VFileHandle *pVFH);

/// Confirmation came at the end of the file
TRIGGERVFS_API int __stdcall vfeof (VFileHandle *pVFH);

/// Getting the size of the file
TRIGGERVFS_API size_t __stdcall vfgetsize (VFileHandle *pVFH);
TRIGGERVFS_API size_t __stdcall VGetFileLength (VHANDLE hVFS, const char *FileName);


/// Examine the file exists
TRIGGERVFS_API bool __stdcall VFileExists (VHANDLE hVFS, const char * FileName);

/// Pack files to check whether a file exists only in
TRIGGERVFS_API bool __stdcall VFileExistsInVfs (VHANDLE hVFS, const char * FileName);

/***************************************************************
 * 
 * Error Dealing
 *
 */
// Brings the last error state
TRIGGERVFS_API short __stdcall VGetError (void );

/// FileName VFS for index information and the actual data matches the check
TRIGGERVFS_API short __stdcall VTestFile (VHANDLE hVFS, const char * FileName);

/// VFS file by reading the calculated value Crc
TRIGGERVFS_API DWORD __stdcall ComputeCrc (VHANDLE hVFS, const char * FileName);

#ifdef	__cplusplus
}
#endif


extern short g_nLastErr;

#define VERR_NONE					0
#define VERR_DELETE_CANTFIND		1
#define VERR_MEM_ALLOCFAIL			2


/***************************************************************
 * VTestFile The return value for
 */
#define	VTEST_SUCCESS				0
#define	VTEST_INVALIDHANDLE			1	// VFS Handle is not valid.
#define	VTEST_CANTOPEN				2	// Can not open file
#define	VTEST_NOTENOUGHMEM			3	// Failure to secure the memory for test
#define	VTEST_LENGTHNOTMATCH		4	// Reading the file size and the actual size of the index information does not match the
#define	VTEST_CRCNOTMATCH			5	// CRC values ??do not match.
#define	VTEST_FILENOTEXISTS			6	// Index file does not exist within or outside
#define	VTEST_INVALIDRANGE			7	// VFS is the file location of the file size is greater than
#define VTEST_OUTFILE				8	// Exists as an external file,
#define VTEST_CANTKNOWVFSINFO		9	// Naelsu no information about VFS file
#define VTEST_ZEROLENGTH			10	// In the index file is a zero length file

struct VfsInfo 
{
	long lStartOff;	// May not be zero.
	long lEndOff;	
};


/******************************************************************
 * VAddFile  Error Code
 */
#define	VADDFILE_SUCCESS				0	// Success
#define	VADDFILE_INVALIDHANDLE			1	// Handle the return value is not valid OpenVFS did not take effect.
#define	VADDFILE_INVALIDVFS				2	// VFS can not find the file
#define	VADDFILE_CANTOPENFILE			3	// Can not open source file
#define	VADDFILE_EXISTSALREADY			4	// VFS files that already exist in the target fails VRemoveFile
#define	VADDFILE_DONTEXIST				5	// There is no source file
#define	VADDFILE_MEMALLOCFAIL			6	// Memory allocation failure
#define	VADDFILE_CANTWRITE				7	// Can or can not fwrite fflush the VFS
#define	VADDFILE_CANTFLUSH				8	// Can or can not fwrite fflush the VFS
#define	VADDFILE_CVFS_AddFile_FAILURE	9	// Can or can not fwrite fflush the VFS
#define	VADDFILE_ZEROFILESIZE			10	// File size 0
#define	VADDFILE_FIOERROR_ETC			11	// error which occurred in or fflush fwrite CErrorInfo :: GetLastError value beyond the value of the short lt
#define VADDFILE_CVFS_AddFile_NONOE		12

#define VADDFILE_FIXEVALUE_CANTWRITE		15000	// fwrite Fails 
#define VADDFILE_FIXEVALUE_CANTFLUSH		20000	// fflush Fails


/******************************************************************
 * VRemoveFile  Error Code
 */
#define VRMVFILE_SUCCESS			0	// Success
#define VRMVFILE_INVALIDVFS			1	// VFS can not find the file
#define VRMVFILE_MEMALLOCFAIL		2	// Memory allocation failure
#define VRMVFILE_DONTEXIST			3	// I can not find the file you want to delete
#define VRMVFILE_INVALIDHANDLE		4	// VHANDLE is not valid


#endif