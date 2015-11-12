#ifndef __VFS_H__
#define __VFS_H__


#include <windows.h>
#include <io.h>
#include <map>
#include <string>
#include <vector>
#include "TriggerVFS.h"

/**
 * Virstual File System. 
 * File Extention : vfs
 * File entry: the beginning of the file, see struct FileEntry
 */


#define		VFSMODE_NOTUSEIDX	0					/// Do not use the IDX file
#define		VFSMODE_USEIDX		1					/// Do use the IDX file

#define		VERSION_STR			"00000"				/// File version string
#define		VERSION_DEF_DWVALUE	0x00000000
#define		VERSION_DEF_WDVALUE	0x0000
#define		VERSION_ARRARY		{'0','0','0','0'}	/// File version array
#define		VERSION_16ARRAY		{'0','0','0','0'}	/// Hexadecimal file version
#define		VERSION_C1			'0','0'				/// The first character of the file version
#define		VERSION_C2			'0','0'				/// The second character of the file version
#define		VERSION_POS_STD		0					/// Standard versions location
#define		VERSION_POS_CUR		sizeof (DWORD)		/// Current version location

/**
 **
 ** vfs file format
 ** 2 BYTE			: File version ==> Patch version 			(added at 2003.11.19)
 ** 2 BYTE			: File version 2 ==> Patch Version 			(added at 2003.12.9)
 ** DWORD			: File an entry of the table number 		(offset?)
 **
 ** Pascal String	: vfs file name 				(1)
 ** long	lIndexStart : Entry table starting position	(2)
 ** The number of,{(1)(2)}
 **
 ** **************************  파일 엔트리 ***********************
 ** DWORD			: The number of files
 ** DWORD			: The number of deleted files				(added at 2003.11.19)
 ** long			: File Offset
 ** File Entry		: [File number]
 **
 **/


#define VFILE_MAX_PATH			164
/// 0 when the file is the start of the start File Offset
/// #define	SIZEOF_VFILEHEADER		(sizeof(BYTE) * 4 + sizeof(DWORD))		/// Version here
#define	SIZEOF_VFILEHEADER		(sizeof(DWORD) * 2 + sizeof(DWORD))		/// Version here

#define	SIZEOF_VENTRYHEADER		(sizeof(DWORD) * 2 + sizeof (long))	
#define SIZEOF_FILEENTRY_HEADER (sizeof(DWORD) * 2 + sizeof (long))	 /// The size of the file entry table in front of header


#define	SIZEOF_FILEENTRY_EXCPTSTRING (sizeof (short) + sizeof (long) * 3 + sizeof (BYTE) * 3 + 2 * sizeof (DWORD))
#define	SIZEOF_FILEENTRY_V ( s )		(SIZEOF_FILEENTRY_EXCPTSTRING + (s).size () + 1)


/// when you open vfs file structures to keep information
struct VFile 
{
	std::string		sVFSName;		/// vfs file name
	FILE *			fp;				/// Open vfs pointer
	FILE *			fp_FAT;			/// write vfs pointer
	DWORD			dwNumOfFile;	/// the number of files stored in the vfs
	long			lStartOfData;	/// vfs data offset
	BYTE			cMode;			/// IDX open mode (rb, wb)
	std::map<std::string, FileEntry *> mapFileEntry;	/// Saving file entry

	VFile () { fp = fp_FAT = NULL; dwNumOfFile = 0; lStartOfData = 0; }
};



/****************************************************************************
 * 이 클래스는 파일엔트리를 순차적으로 접근하고 수정하기 위한 코드이다
 ****************************************************************************
 */
class CVFS
{
private:
	std::string					m_sFileName;	/// The name of the file. idx, except for the extension of vfs
	std::map<int, std::string>	m_is;			/// Key number value: the name of the file. Number is stored in the order that
	std::map<std::string, int> 	m_si;			/// Key: the file name, value: Number
	std::vector<FileEntry *>	m_ve;			/// put into vector

	std::map<int, FileEntry *>	m_mapDel;		/// Delete the archived file entry and map
	FILE *						m_fp;			/// File Handle
	
	HANDLE m_hFile; // File Handle
	HANDLE m_hMap;	// Memory Mapped IO mapping handle

	FILE *						m_fpFAT;		/// File Handle of Index 파일

	DWORD						m_dwNum;		/// The number of file entries
	DWORD						m_dwDelCnt; 	/// The number of deleted items
	long						m_lStartOffset; /// The file entry data offset
	BYTE						m_cMode;		/// Using VFSMODE_USEIDX: index, VFSMODE_NOTUSEIDX: does not use index
	size_t						m_BUFSIZ;		/// Buffer Size
	char *						m_Buffer;		/// File Buffer

	long						m_lEntryStart;	/// Start Address. FileEntry of If there are a number of vfs in the index file, each file entry table has a starting position

private:
	/// Reads the file header and the entry
	bool		__ReadFileEntry (void);

	/// deletes entries from the map and vector
	void		__EraseMemEntry (const char *FileName, int iIndex);
	/// Adds entries to the map and vector
	void		__AddMemEntry (FileEntry *FE);
	/// Erase the file name in order to sort gives
	bool		__SortFiles (const char **Files, int iNum);
	/// Of the erased file entry looks go in size beulreuk bRestore == true if recovery should
	FileEntry * __FindBlock (std::string sFileName, long lSize, int *iIndex, bool bRestore = true);
	/// index file, write the entry number and the starting offset in the front
	void		__VWriteVFSHeader (DWORD dwNum, DWORD dwDelCnt, long lStartOffset);
	

	int index (const char * sKey);
	std::string index (int iKey);
	int operator [] (std::string sKey);
	std::string operator [] (int iKey);

public :
	CVFS ();
	CVFS (FILE * fpFAT, long lOffset, DWORD dwNum, const char *VfsName, char * Mode);
	~CVFS ();

	long SizeOfEntryTable (void);		/// to calculate the size of File entry table
	
	/// Calls the CVFS_Manager
	bool Open (FILE * fpFAT, long lOffset, const char *VfsName, const char *Dir, const char * Mode);

	/// vfs file closes
	void Close (void);

	
	short RemoveFilesB (const char **Files, int iNum);
	/// Additional files. Its own value is calculated as CRC

	short AddFile (const char * FileName, const char * TargetName, DWORD dwVersion, DWORD dwCRC
		, BYTE btEncType, BYTE btCompress, bool bUseDel = false);
	/// Open The file
	VFileHandle * OpenFile (const char * FileName);
	/// Close The file
	void CloseFile (VFileHandle * FH);

	/// Get the file length
	long GetFileLength (const char * FileName);
	
	/// Gets the total number of files
	DWORD GetFileCount (void);
	/// Memory allocation the caller gets the file name.
	DWORD GetFileNames (char **pFiles, DWORD nFiles, int nMaxPath);
	/// Deletes spaces
	bool ClearBlank (void);
	/// Number of deleted blocks
	DWORD GetDelCnt (void);
	/// Number of reused blocks
	DWORD GetReUsedCnt (void);
	/// 
	int GetSizeofBlankArea ();
	/// Investigation file, but the block size and the size of the file does not match the number of entries. (Who was temping block)
	DWORD GetFileCntWithHole (void);
	
	/// update the starting offset of the File entry
	void SetStartOfEntry (long lStartOfEntry);
	
	///Number of files: total number - the number of erased
	DWORD GetEntryCount (void);
	/// Checks to see if the file exists
	bool FileExists (const char *FileName);

	/// Gets the totaal amount of vfs files
	DWORD GetVFSCount (void);
	/// Gets the names of the VFS´s
	DWORD GetVfsNames (char **ppFiles, DWORD dwNum, short dwMaxPath);

	/// Find information about the file
	void GetFileInfo (const char * FileName, VFileInfo * pFileInfo);
	///
	bool SetFileInfo (const char * FileName, VFileInfo * pFileInfo);

	FILE * GetFP ( void );

	long GetStartOff (void );
};


/// Writes the file entry
bool VWriteFileEntry (FileEntry *fe, FILE * fp, bool bFlush = true);


#endif
