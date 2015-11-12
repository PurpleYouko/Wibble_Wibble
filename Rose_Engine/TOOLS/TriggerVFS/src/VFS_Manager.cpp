#include "stdafx.h"
#include <stdlib.h>
#include "VFS.h"
#include "VFS_Manager.h"
#include "libFile.h"
#include <io.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "libString.h"
#include "libCrc.h"
#include "BlankInfo.h"
#include "Macro.h"

/************************************************************************************
 *
 *
 * CVFS_Manager
 *
 *
 */

///bool CVFS_Manager::s_bLock = false;


CVFS_Manager::CVFS_Manager ()
{
	m_fpIDX			= NULL;
	m_sIdxFileName	= "";
	// strcpy ((char *)m_wStdVersion, VERSION_STR);	
	m_wStdVersion[ 0 ] = VERSION_DEF_WDVALUE; /// Since the initialization
	m_wStdVersion[ 1 ] = VERSION_DEF_WDVALUE;
	m_wCurVersion[ 0 ] = VERSION_DEF_WDVALUE;
	m_wCurVersion[ 1 ] = VERSION_DEF_WDVALUE;

	m_vecVFS.clear ();

	///InitializeCriticalSection (&m_CS);
}

CVFS_Manager::~CVFS_Manager ()
{
	if( m_fpIDX != NULL )
		this->Close ();

	m_fpIDX			= NULL;
	m_sIdxFileName	= "";
}


/******************************************************************************************
 * Make empty index file header for the File,
 */
bool CVFS_Manager::__WriteBlankIndexFile (void)
{
	if( m_fpIDX != NULL )
	{
		// Initialize member variable
		
		this->m_dwNumOfEntry = 0;
		m_wStdVersion[ 0 ] = VERSION_DEF_WDVALUE;
		m_wStdVersion[ 1 ] = VERSION_DEF_WDVALUE;
		m_wCurVersion[ 0 ] = VERSION_DEF_WDVALUE;
		m_wCurVersion[ 1 ] = VERSION_DEF_WDVALUE;

		fseek (m_fpIDX, 0, SEEK_SET);
		fwrite (m_wStdVersion	, sizeof (WORD)	, 2, m_fpIDX);	/// Since standard file write
		fwrite (m_wCurVersion	, sizeof (WORD)	, 2, m_fpIDX);	/// Write the current file version
		fwrite (&m_dwNumOfEntry	, sizeof (DWORD), 1, m_fpIDX);	/// The number of VEntry = 0 Writing to a file

		return ( fflush (m_fpIDX) == 0 );
	}

	return false;
}


/******************************************************************************************
 * Index File Header Read
 */
bool CVFS_Manager::__ReadVEntry (void)
{
	char * buff = NULL;
	VEntry *pVE = NULL;
	short sLength = 0;

	fseek (m_fpIDX, 0, SEEK_SET);									/// Best move forward
	fread ((void *)m_wStdVersion	, sizeof (WORD)	, 2, m_fpIDX);	/// Read the standard version
	fread ((void *)m_wCurVersion	, sizeof (WORD)	, 2, m_fpIDX);	/// Read the current version
	fread ((void *)&m_dwNumOfEntry	, sizeof (DWORD), 1, m_fpIDX);	/// Read the number of VEntry

	for(DWORD i = 0; i < m_dwNumOfEntry; i++)
	{
		if((pVE = new VEntry))
		{
			fread ((void *)&sLength, sizeof (short), 1, m_fpIDX);
			if((buff = new char[ sLength ]))
			{
				/// Read the file name
				ZeroMemory (buff, sLength);
				fread ((void *)buff, sizeof (char), sLength, m_fpIDX);	/// sLength = Including the number of NULL until after
				pVE->sVFSName = buff;
			
				delete [] buff; /// ==>  The original release, but does not need to be turned off. If you are just using one string seems to heap.

				/// Read the offset of the start of the index
				// fread ((void *)&pVE->dwNum,			sizeof (DWORD)	, 1, m_fpIDX);
				fread ((void *)&pVE->lStartOfEntry, sizeof (long)	, 1, m_fpIDX);

				/// CVFS vfs file and creates an instance of the entry to open the
				long lCurPos = ftell (m_fpIDX);
				pVE->pVFS = new CVFS ();
				if(pVE->pVFS)
				{
					bool bPackOpened = pVE->pVFS->Open (
							m_fpIDX
							, pVE->lStartOfEntry
							, pVE->sVFSName.c_str ()
							, m_sBasePath.c_str()
							, m_strIdxOpenMode.c_str ()
							);

					if(bPackOpened) /// If successful, tucked Vector neotgo
						m_vecVFS.push_back (pVE); 
					else if( pVE->sVFSName != "ROOT.VFS" )
						return false;
				}
				
				fseek (m_fpIDX, lCurPos, SEEK_SET);
			}
			else
			{
				delete pVE;
				return false;
			}
		}
		else
		{ return false; }
	}

	return true;
}


bool CVFS_Manager::__TestMapIO (const char * szFileName)
{
	return false;

	HANDLE fpTest	= NULL;
	HANDLE fm		= NULL;
	char * src		= NULL;

	OSVERSIONINFO OsVer;
	GetVersionEx (&OsVer);

	if(OsVer.dwMajorVersion >= 5)
	{
		fpTest = CreateFile (szFileName
			, GENERIC_READ
			, FILE_SHARE_READ
			, NULL
			, OPEN_EXISTING
			, FILE_ATTRIBUTE_READONLY
			, NULL);


		if (fpTest == INVALID_HANDLE_VALUE) 
			return false;

		fm = CreateFileMapping ( fpTest, NULL, PAGE_READONLY, 0, 0, NULL );

		if (!fm)
		{
			CloseHandle( fpTest );
			return false;
		}

		src = reinterpret_cast<char *>(MapViewOfFile( fm, FILE_MAP_READ, 0, 0, 0 ));	
		if (src == NULL) 
		{
			CloseHandle (fm);
			CloseHandle (fpTest);
			return false;
		}

		UnmapViewOfFile( src );
		CloseHandle( fm );
		CloseHandle( fpTest );

		return true;
	}

	return false;
}


/******************************************************************************************
 *
 */
void CVFS_Manager::__CheckOpenMode ( const char * InputMode, char ModifiedMode[ 16 ] )
{
#ifdef __SUPPORT_MEMORY_MAPPED_IO__
	if( (Mode[ 0 ] == 'm') && __TestMapIO ("Logo.dds") == false )
		strcpy ( ModifiedMode, InputMode + 1);
	else
		strcpy ( ModifiedMode, InputMode );
#else
	if( InputMode[ 0 ] == 'm' )
	{
		strcpy ( ModifiedMode, InputMode + 1 );
	}
	else
		strcpy ( ModifiedMode, InputMode );
#endif
}


/******************************************************************************************
 * Index Open the file
 * @param IndexFile Filename column index
 * @param Mode Properties. "r" read only, "w" write-only (not Use), "w +" create + write + read,
 *                  "r +" read + write (generated sequence)
 */
bool CVFS_Manager::Open (const char *IndexFile, const char * __Mode)
{
	m_sIdxFileName = IndexFile;
	m_sBasePath = GetDirectory (m_sIdxFileName.c_str ()); /// "\" Character, including
    /// Binary Mode Made
	_fmode = _O_BINARY;
	/// If you open in write mode

	char Mode[ 16 ];
	__CheckOpenMode ( __Mode, Mode );

	m_strIdxOpenMode = Mode;

	if(strcmp (Mode, "w+") == 0)
	{
		CFileMode::CheckMode (IndexFile, CFileMode::MODE_READWRITE, true) ;
		if( (m_fpIDX = fopen (IndexFile, Mode)) )	/// "w +" entry is always modified to open
		{
			m_vecVFS.clear ();

			return __WriteBlankIndexFile (); // Just make an empty one Ventry
		}
	}
	else if(strcmp (Mode, "mr") == 0 || strcmp (Mode, "mr+") == 0 )
	{
		/// If you do not read to reading to examine and change the permissions
		CFileMode::CheckMode (IndexFile, CFileMode::MODE_READ, true) ;

		if( CFileMode::CheckMode (IndexFile, CFileMode::MODE_EXISTS)  
			&& (m_fpIDX = fopen (IndexFile, Mode + 1)))
		{
			return __ReadVEntry ();
		}
	}
	else if(strcmp (Mode, "r+") == 0)
	{
		CFileMode::CheckMode (IndexFile, CFileMode::MODE_READWRITE, true) ;

		if( CFileMode::CheckMode (IndexFile, CFileMode::MODE_EXISTS) 
			&& (m_fpIDX = fopen (IndexFile, Mode)) )
		{
			return __ReadVEntry ();
		}
	}
	else if(strcmp (Mode, "r") == 0)
	{
		if( !CFileMode::CheckMode (IndexFile, CFileMode::MODE_EXISTS) )
			return false;

		/// f you do not read to reading to examine and change the permissions
		CFileMode::CheckMode (IndexFile, CFileMode::MODE_READ, true) ;

		if( (m_fpIDX = fopen (IndexFile, Mode)) )
		{
			return __ReadVEntry ();
		}
	}
	
		
	return false;
}


/******************************************************************************************
 * VFS file and close the file Index
 */
void CVFS_Manager::Close (void)
{
	VEntry * m_pVEntry = NULL;
	std::vector<VEntry *>::reverse_iterator ir = m_vecVFS.rbegin ();

	for(; ir != m_vecVFS.rend (); ir++)
	{
		m_pVEntry = *ir;
		if(m_pVEntry)
		{
			m_pVEntry->pVFS->Close ();
			delete m_pVEntry->pVFS;
			delete m_pVEntry;
		}
	}
	m_vecVFS.clear ();

	if(m_fpIDX)
	{
		fclose (m_fpIDX);
		m_fpIDX = NULL;
	}
}


/******************************************************************************************
 * Write VEntry
 */
void CVFS_Manager::__WriteVEntry (VEntry * pVE)
{
	short sLength = (short)pVE->sVFSName.size () + 1;
	fwrite ((void *)&sLength				, sizeof (short), 1, m_fpIDX);
	fwrite ((void *)pVE->sVFSName.c_str ()	, sizeof (char)	, pVE->sVFSName.size (), m_fpIDX);
	fwrite ((void *)"\0"					, sizeof (char) , 1, m_fpIDX);
	fwrite (&pVE->lStartOfEntry				, sizeof (long) , 1, m_fpIDX);
}


/******************************************************************************************
 * The size of the file to write to VEntry
 */
long CVFS_Manager::__SizeOfVEntry (VEntry *VE)
{
	return (long)(VE->sVFSName.size () + 1 + SIZE_VENTRY_EXCEPTSTRING);
}


/******************************************************************************************
 * Seukip VEntry to the
 */
void CVFS_Manager::__SkipVEntry (VEntry *VE)
{
	/// It calculates the size seukip
	long lSkipSize = (long)VE->sVFSName.size () + 1 + SIZE_VENTRY_EXCEPTSTRING;
	fseek (m_fpIDX, lSkipSize, SEEK_CUR);
}


/******************************************************************************************
 * Add the VFS file
 * @param VfsName Use uppercase and fix it later ***
 */
bool CVFS_Manager::AddVFS (const char *VfsName)
{
	long		lSize = 0;		/// File size
	long		lVET_Size = 0;	/// The size of the current VEntry Table
	long		lNewSize = 0;	/// The new file size
	VEntry *	pVE = NULL;		/// Add to VEntry
	std::vector<VEntry *>::iterator iv;
	char		uprVfsName[ 1024 ];

	if(VfsName == NULL) 
		return false;
	if(VfsExists (VfsName)) 
		return false;	/// Vfs same name already exists, returns false if

	__ConvertPath (VfsName, uprVfsName);

	if((pVE = new VEntry))
	{
		lSize = __vfseek (m_fpIDX, 0, SEEK_END);
		/// Make VEntry
		pVE->sVFSName		= uprVfsName;							/// vfs file name. Puts the uppercase
		pVE->lStartOfEntry	= lSize + __SizeOfVEntry (pVE);			/// Starting offset of the new entry table

		/// The total size of the index file
		lSize = ::__vfseek (m_fpIDX, 0, SEEK_END);
		/// That is because you need to modify VEntry moves forward
		fseek (m_fpIDX, SIZEOF_VFILEHEADER, SEEK_SET);
		lVET_Size = SIZEOF_VFILEHEADER;
		/// For adding VEntry calculating position from
		iv = m_vecVFS.begin ();
		/// CVFS VEntry front of each instance is modified and corrected
		for(; iv != m_vecVFS.end (); iv++) 
		{ 
			(*iv)->lStartOfEntry += __SizeOfVEntry (pVE);
			lVET_Size += __SizeOfVEntry (*iv); 
			__WriteVEntry (*iv);
			(*iv)->pVFS->SetStartOfEntry ((*iv)->lStartOfEntry);		/// It should be also modified in CVFS
		}
		long lInsertedPos = ftell (m_fpIDX);							/// Where you want to add VEntry
		::__MakeFileHole (lInsertedPos, __SizeOfVEntry (pVE), m_fpIDX, true);	/// To make room for the additional
		fseek (m_fpIDX, lInsertedPos, SEEK_SET);						/// Position to write the file header VEntry
		__WriteVEntry (pVE);											/// Write to file
		fflush (m_fpIDX);
		/// Create an instance VFS
		if((pVE->pVFS = new CVFS))
		{ 
			if((pVE->pVFS->Open (m_fpIDX, pVE->lStartOfEntry, pVE->sVFSName.c_str (), m_sBasePath.c_str (), "w+")))
			{ 
				m_vecVFS.push_back (pVE); 
				m_dwNumOfEntry++;
				__WriteIndexHeader (VERSION_STR, m_dwNumOfEntry);
			}
			else
			{
				delete pVE->pVFS;
				delete pVE;
				return false;
			}
		}
		else
		{ 
			delete pVE;
			return false;
		}
	}
	else
	{ return false; }

	return true;
}


/******************************************************************************************
 * RemoveVFS : Remove the VFS
 * @param VfsName Vfs file name to be removed
 */
bool CVFS_Manager::RemoveVFS (const char *VfsName)
{
	DWORD		iDelIndex = -1;
	VEntry *	pVE = NULL;
	long		lDelSize = 0;	/// The size of the area to disappear
	std::vector<VEntry *>::iterator iv;

	fseek (m_fpIDX, SIZEOF_VFILEHEADER, SEEK_SET);
	if((iDelIndex = __FindEntryIndex (VfsName)) >= 0)
	{
		pVE = *(m_vecVFS.begin () + iDelIndex);
		lDelSize = __SizeOfVEntry (pVE);										/// Subtracting all these, but the front has
		for(; iv != m_vecVFS.begin () + iDelIndex; iv++)
		{
			(*iv)->lStartOfEntry -= lDelSize;
			(*iv)->pVFS->SetStartOfEntry ((*iv)->lStartOfEntry);
			__WriteVEntry (*iv);
		}
		/// Overwrite the deleted region
		lDelSize += pVE->pVFS->SizeOfEntryTable ();								/// Ttaenggyeo at the back should be the jeomankeum
		pVE->pVFS->Close ();													/// Close CVFS
		delete pVE->pVFS;														/// Release the instance CVFS
		delete pVE;																/// File entry is removed from memory
		/// Advances come again cleared
		iv++;
		/// VEntry is updated after the
		for(; iv != m_vecVFS.end (); iv++)
		{
			(*iv)->lStartOfEntry -= __SizeOfVEntry (pVE);
			(*iv)->pVFS->SetStartOfEntry ((*iv)->lStartOfEntry);
			__WriteVEntry (*iv);
		}
		m_vecVFS.erase (m_vecVFS.begin () + iDelIndex);							/// Erase vector
		m_dwNumOfEntry--;														/// Reduce the number one
		__WriteIndexHeader (VERSION_STR, m_dwNumOfEntry);						/// Index rewrites the header of the file
		::__ftruncate (::__vfseek (m_fpIDX, 0, SEEK_END) - lDelSize, m_fpIDX);	/// Index to adjust the size of the file
	}

	return false;
}


/******************************************************************************************
 * Write the file header
 */
void CVFS_Manager::__WriteIndexHeader (char * Version, DWORD dwNum)
{
	DWORD dwStdVersion;
	dwStdVersion = atoi (Version);
	fseek (m_fpIDX, 0, SEEK_SET);
	fwrite ((void *)&dwStdVersion, sizeof (WORD), 2, m_fpIDX);		/// Standard version
	fwrite ((void *)m_wCurVersion, sizeof (WORD), 2, m_fpIDX);	/// The current version
	fwrite ((void *)&dwNum,  sizeof (DWORD), 1, m_fpIDX);		/// The number of Ventry
	fflush (m_fpIDX);
}


/******************************************************************************************
 * VFS file searches for the presence of an entry for
 * @param FileName VFS find the file name
 */
VEntry * CVFS_Manager::__FindEntry (const char *FileName)
{
	std::vector<VEntry *>::iterator iv = m_vecVFS.begin ();

	for(; iv != m_vecVFS.end (); iv++)
	{
		/// VEntry * Returns the file name are the same,
		if((*iv)->sVFSName == FileName) 
			return *iv;
	}

	return NULL;
}


/******************************************************************************************
 * VFS provides a file entry is returned in a search the index. Entry information in the header information useful
 * @param FileName VFS find the file name
 */
DWORD CVFS_Manager::__FindEntryIndexWithFile (const char *FileName)
{
	DWORD dwRet = 0;
	std::vector<VEntry *>::iterator iv = m_vecVFS.begin ();

	for(; iv != m_vecVFS.end (); iv++)
	{
		/// VEntry * Returns the file name are the same
		if((*iv)->pVFS->FileExists (FileName))
			return dwRet;

		dwRet++;
	}

	return -1;
}


/******************************************************************************************
 * VFS provides a file entry is returned in a search the index. Entry information in the header information useful
 * @param FileName VFS find the file name
 */
long CVFS_Manager::__FindEntryIndex (const char *FileName)
{
	DWORD dwRet = 0;

	std::vector<VEntry *>::iterator iv = m_vecVFS.begin ();
	for(; iv != m_vecVFS.end (); iv++)
	{
		/// VEntry * Returns the file name are the same,
		if((*iv)->sVFSName == FileName) { return dwRet; }
		dwRet++;
	}

	return -1;
}


/******************************************************************************************
 * Add the file to the vfs
 * @param dwNum			The number of files to be added to
 * @param TargetName	Registered in the name.
 */
short CVFS_Manager::AddFile (const char *VfsName
							, const char *FileName
							, const char * TargetName
							, DWORD dwVersion
							, DWORD dwCrc
							, BYTE btEncType
							, BYTE btCompres
							, bool bUseDel)
{
	long		lOldSize = 0, lNewSize = 0;
	VEntry *	pVE = NULL;
	std::vector<VEntry *>::iterator iv;
	char uprVfsName[ 1024 ];
	char uprTargetName[ 1024 ];
	
	/// Path of capital letters, spaces before and after removal
	__ConvertPath (VfsName, uprVfsName);
	__ConvertPath (TargetName, uprTargetName);

	
	/// If there is an entry corresponding to VfsName add
	if((pVE = __FindEntry (uprVfsName))) 
	{ 
		lOldSize = pVE->pVFS->SizeOfEntryTable ();						/// Table entries before the size of the file
		short nAddResult = pVE->pVFS->AddFile (FileName
								, uprTargetName
								, dwVersion
								, dwCrc
								, btEncType
								, btCompres
								, bUseDel);		/// Add a file

		if(nAddResult != VADDFILE_SUCCESS)						
			return nAddResult;

		lNewSize = pVE->pVFS->SizeOfEntryTable ();						/// After adding the size of the file table entry
		/// All you have to update the table VEntry
		int iIndex = __FindEntryIndex (uprVfsName);
		/// File Indicator VEntry Table to Table to modify moves forward.
		fseek (m_fpIDX, SIZEOF_VFILEHEADER, SEEK_SET);
		/// There is no change off the front. 
		for(iv = m_vecVFS.begin (); iv <= m_vecVFS.begin () + iIndex; iv++)
		{
			__SkipVEntry (*iv);
		}
		/// VEntry an entry corresponding to the rear of the table offset is pushed
		for(; iv != m_vecVFS.end (); iv++)
		{
			(*iv)->lStartOfEntry += lNewSize - lOldSize;
			(*iv)->pVFS->SetStartOfEntry ((*iv)->lStartOfEntry);
			__WriteVEntry (*iv);
		}

		__FlushIdxFile ();

		return VADDFILE_SUCCESS;
	}

	return VADDFILE_INVALIDVFS;
}


/******************************************************************************************
 * VEntry pack look for the file name in the
 */
VEntry * CVFS_Manager::__FindVEntryWithFile (const char *FileName)
{
	VEntry * pVE = NULL;

	std::vector<VEntry *>::iterator iv = m_vecVFS.begin ();
	for(; iv != m_vecVFS.end (); iv++)
	{
		/// VEntry * Returns the file name are the same,
		if((*iv)->pVFS->FileExists (FileName))
		{  
			return *iv;
		}
	}

	return NULL;
}


/******************************************************************************************
 * Pack locate files, one file is removed.
 * @param File	pack File in the File Name
 * @return VRMVFILE_XXXXX , If successful, returns VRMVFILE_SUCCESS
 */
short CVFS_Manager::RemoveFile (const char *FileName)
{
	const char *szNEW = NULL;
	char uprTargetName[1024];

	VEntry *	pVE = NULL;
	long		lOldSize = 0, lNewSize = 0;
	DWORD		iIndex = -1;
	short		i = 0;
	
	/// Change the name to the correct path to the target
	__ConvertPath (FileName, uprTargetName);

	szNEW = uprTargetName;

	/// If you have to find and delete the entry
	pVE = __FindVEntryWithFile (uprTargetName);
	if(pVE)
	{
		iIndex = __FindEntryIndexWithFile (uprTargetName);
		if(iIndex >= 0)
		{
			CVFS * pVFS = pVE->pVFS;
			lOldSize = pVFS->SizeOfEntryTable ();

			short bRet;
			if( (bRet = pVFS->RemoveFilesB (&szNEW, 1)) == VRMVFILE_SUCCESS )
			{
				lNewSize = pVFS->SizeOfEntryTable ();
				fseek (m_fpIDX, SIZEOF_VFILEHEADER, SEEK_SET);

				for(i = 0; i <= (signed)iIndex; i++)
					__SkipVEntry (*(m_vecVFS.begin () + i)); 

				for(; i < (signed)m_dwNumOfEntry; i++)
				{
					std::vector<VEntry *>::iterator iv = m_vecVFS.begin () + i;
					if(iv != m_vecVFS.end ())	/// If you do not have a vfs file. vfs file is written to the file number and the number is different from the
					{
						VEntry * pVEtoModify = *iv;
						/// VEntry modify the entry table and re-write the Start Offset
						pVEtoModify->lStartOfEntry -= (lOldSize - lNewSize);
						__WriteVEntry (pVEtoModify);
					}
				}

				fflush ( m_fpIDX );

				return VRMVFILE_SUCCESS;
			}
			
			return bRet; // Which returns false if the delete right
		}
	}

	return VRMVFILE_INVALIDVFS; // If you do not try to delete the file and returns true
}


/******************************************************************************************
 * Remove multiple files. Just leave a blank space
 */
bool CVFS_Manager::RemoveFiles (const char *VfsName, const char **Files, int iNum)
{
	VEntry *	pVE = NULL;
	long		lOldSize = 0, lNewSize = 0;
	DWORD		iIndex = -1;
	short		i = 0;

	/// If you have to find and delete the entry
	if((pVE = __FindEntry (VfsName)) && (iIndex = __FindEntryIndex (VfsName)) >= 0)
	{
		lOldSize = pVE->pVFS->SizeOfEntryTable ();
		if(pVE->pVFS->RemoveFilesB (Files, iNum))
		{
			lNewSize = pVE->pVFS->SizeOfEntryTable ();
			fseek (m_fpIDX, SIZEOF_VFILEHEADER, SEEK_SET);
			for(i = 0; i <= (signed)iIndex; i++)
			{ __SkipVEntry (*(m_vecVFS.begin () + i)); }

			for(; i < (signed)m_dwNumOfEntry; i++)
			{
				/// VEntry modify the entry table and re-write the Start Offset
				(*(m_vecVFS.begin () + i))->lStartOfEntry -= (lOldSize - lNewSize);
				__WriteVEntry (*(m_vecVFS.begin () + i));
			}

			return true;
		}
	}
	
	return false;
}


/// Fflush the index file.
void CVFS_Manager::__FlushIdxFile (void)
{
	if(m_fpIDX)
		fflush (m_fpIDX);
}


/******************************************************************************************
 * Open the file in the vfs file
 * When I open the index in a different directory may be a problem. ==> Modify
 */
VFileHandle * CVFS_Manager::OpenFile (const char *FileName)
{
	VFileHandle *pVF = NULL;
	std::vector<VEntry *>::iterator iv = m_vecVFS.begin ();

	if(FileName == NULL) { return NULL; }

	
	if( FileExistsInVfs( FileName ) ) // If you open it first in the vfs. And the existence of an external file search
	{
		char rightName[1024];
		::__ConvertPath (FileName, rightName);
		for(; iv != m_vecVFS.end (); iv++)
		{
			if((pVF = (*iv)->pVFS->OpenFile (rightName)))
			{
				pVF->hVFS = (VHANDLE)this;
				return pVF;
			}
		}
	}
	else if(_access (FileName, 0) == 0 && _access (FileName, 4) == 0)
	{
		if((pVF = new VFileHandle))
		{
			_fmode = _O_BINARY;
			if((pVF->fp = fopen (FileName, "r")))
			{
				pVF->lCurrentOffset	= 0;
				pVF->lStartOff		= 0;
				pVF->lEndOff		= __vfseek (pVF->fp, 0, SEEK_END);
				pVF->sFileName		= FileName;
				pVF->btFileType		= 1;									/// If only one file
				pVF->hVFS			= NULL;
				pVF->pData			= NULL;

				fseek (pVF->fp, 0, SEEK_SET);

				return pVF;
			}
		}
	}

	return NULL; /// Not found, NULL is returned on pVF
}


/******************************************************************************************
 * Close the file opened by OpenFile
 * Are simply a function to release memory, but ...
 */
void CVFS_Manager::CloseFile (VFileHandle *pVFH)
{
	/// In general, if you call VCloseFile VCloseFile and close the file, make sure
	if(pVFH->btFileType) { fclose (pVFH->fp); }
	/// Now just simply put a function to release memory
	delete pVFH;
}


/******************************************************************************************
 * VFS Searches for the file in the file name
 */
DWORD CVFS_Manager::GetFileNames (const char *VfsName, char **pFiles, DWORD nFiles, int nMaxPath)
{
	VEntry * pVE = NULL;
	if(!(pVE = __FindEntry (VfsName))) { return 0; }

	return pVE->pVFS->GetFileNames (pFiles, nFiles, nMaxPath);
}


/******************************************************************************************
 * To find out the file size
 * @return If you can not find the file and return 0. (The actual file size is 0 degrees)
 */
long CVFS_Manager::GetFileLength (const char *FileName)
{
	VFileHandle * pVF	= NULL;
	long lFileSize		= 0;

	/// Even if there is only one file, the file size returned.
	struct _stat file_stat;	
	if (_stat(FileName, &file_stat) == 0) 
		lFileSize = (long)file_stat.st_size;
	else
	{
		std::vector<VEntry *>::iterator iv = m_vecVFS.begin ();
		/// Vfs file is not much in the index file, so it is acceptable to just shall saenggakdoem for loop
		for(; iv != m_vecVFS.end (); iv++)
		{
			lFileSize = (*iv)->pVFS->GetFileLength (FileName);	
			if(lFileSize >= 0) /// Returns the size of the discovery. Find out by using the map size clearance
				break;
		}
	}

	return lFileSize; /// Returns -1 if not found
}


/******************************************************************************************
 * To find out the number of files
 */
DWORD CVFS_Manager::GetFileCount (const char *VfsName)
{
	VEntry * pVE = NULL;
	if(pVE = __FindEntry (VfsName))			/// If you search for files
	{
		return pVE->pVFS->GetFileCount ();	/// Returns the number
	}

	return 0;
}


/******************************************************************************************
 * The total number of files in the index file to find out.
 */
DWORD CVFS_Manager::GetFileTotCount (void)
{
	if(m_vecVFS.size () <= 0) 
		return 0;

	std::vector<VEntry *>::iterator iv = m_vecVFS.begin ();
	int iCnt = 0;
	for(; iv != m_vecVFS.end (); iv++)
	{
		if(*iv) /// Enter the appropriate value iterator malgeot think that.
		{
			iCnt += (*iv)->pVFS->GetFileCount ();
		}
	}

	return iCnt;
}


/******************************************************************************************
 * The index number in a file bundle file is examined
 */
DWORD CVFS_Manager::GetVFSCount (void)
{
	return m_dwNumOfEntry;
}


/******************************************************************************************
 * GetVfsNames : Index File in the survey as the name of the bundle file
 * @param ppFiles Buffer to store the file name
 * @param dwNum number of strings stored in ppFiles
 * @param dwMaxPath Maximum length of the string
 */
DWORD CVFS_Manager::GetVfsNames (char **ppFiles, DWORD dwNum, short dwMaxPath)
{
	int i = 0;
	std::vector<VEntry *>::iterator iv = m_vecVFS.begin ();

	for(i = 0; i < dwMaxPath && iv != m_vecVFS.end (); i++)
	{
		strncpy (ppFiles[ i ], (*iv)->sVFSName.c_str (), dwMaxPath - 1);
		ppFiles[ i ][ dwMaxPath - 1 ] = 0;
		iv++;
	}

	return i;
}


/******************************************************************************************
 * Pack Jyeoteuna removed from the file the number of files that are not clean yet investigated
 */
DWORD CVFS_Manager::GetDelCnt (const char *VfsName)
{
	VEntry * pVE = NULL;

	if(pVE = __FindEntry (VfsName))			/// If you search for files
	{
		return pVE->pVFS->GetDelCnt ();		/// Returns the number
	}

	return 0;
}


/******************************************************************************************
 * 
 */
DWORD CVFS_Manager::GetFileCntWithHole (const char *VfsName)
{
	VEntry * pVE = NULL;

	if(pVE = __FindEntry (VfsName))					/// If you search for files
	{
		return pVE->pVFS->GetFileCntWithHole ();	/// Returns the number
	}

	return 0;
}


/// Clear the space
bool CVFS_Manager::ClearBlank (const char * VfsName )
{
	long		lOldSize = 0 , lNewSize = 0;
	VEntry *	pVE = NULL;
	int			iIndex = 0;
	std::vector<VEntry *>::iterator iv;
	long		lFileSize = 0;

	if(pVE = __FindEntry (VfsName))						/// If you search for files
	{
		lFileSize = __vfseek (m_fpIDX, 0, SEEK_END);	/// File Size
		lOldSize = pVE->pVFS->SizeOfEntryTable ();		/// Changes in the size table to find out the size change
		if(pVE->pVFS->ClearBlank ())
		{
			lNewSize = pVE->pVFS->SizeOfEntryTable ();
			/// Pull back of
			::__MoveFileBlock (pVE->lStartOfEntry + lOldSize
				, lFileSize - (pVE->lStartOfEntry + lOldSize)
				, pVE->lStartOfEntry + lNewSize, 1000000, m_fpIDX, false);

			fflush (m_fpIDX);
			/// Adjust the size of the file
			::__ftruncate (lFileSize - (lOldSize - lNewSize), m_fpIDX);

			iIndex = __FindEntryIndex (VfsName);
			iv = m_vecVFS.begin ();
			/// VEntry Table moved forward in order to modify the
			fseek (m_fpIDX, SIZEOF_VFILEHEADER, SEEK_SET);
			/// Prior to just seukip
			for(; iv <= m_vecVFS.begin () + iIndex; iv++)
			{
				__SkipVEntry (*iv);
			}
			/// This skips
			/// iv++;   /// <== Not because it is not skipping delete
			/// Subtracted from the back again as deleted
			for(; iv != m_vecVFS.end (); iv++)
			{
				(*iv)->lStartOfEntry -= lOldSize - lNewSize;
				(*iv)->pVFS->SetStartOfEntry ((*iv)->lStartOfEntry);
				__WriteVEntry (*iv);
			}

			return true;
		}
	}

	return false;
}


/// Pack deletes all spaces in file
bool CVFS_Manager::ClearBlankAll (VCALLBACK_CLEARBLANKALL CallBackProc)
{

	long			lOldSize = 0 , lNewSize = 0;
	VEntry *		pVE = NULL;
	int				iIndex = 0;
	std::vector<VEntry *>::iterator iv;
	std::vector<VEntry *>::iterator il;
	long			lFileSize = 0;
	const char *	VfsName;

	std::vector<int> vecStepPos;

	CBlankInfo::procCallBack = CallBackProc;

	il = m_vecVFS.begin ();
	for(; il != m_vecVFS.end (); il++)
	{
		if( *il )
		{
			VEntry * pVE = *il;
			CBlankInfo::iMaxCount += ( pVE->pVFS->GetEntryCount () + pVE->pVFS->GetDelCnt () 
									+ pVE->pVFS->GetReUsedCnt () + 1 ) ;
			CBlankInfo::iMaxCount += 2;

			vecStepPos.push_back ( CBlankInfo::iMaxCount );
		}
	}

	std::vector<int>::iterator itStepPos = vecStepPos.begin ();

	for( il = m_vecVFS.begin (); il != m_vecVFS.end (); il++ )
	{
		VfsName = (*il)->sVFSName.c_str ();

		if(pVE = __FindEntry (VfsName))						/// If you search for files
		{
			lFileSize = __vfseek (m_fpIDX, 0, SEEK_END);	/// File Size
			lOldSize = pVE->pVFS->SizeOfEntryTable ();		/// Changes in the size table to find out the size change
			if(pVE->pVFS->ClearBlank ())
			{
				lNewSize = pVE->pVFS->SizeOfEntryTable ();
				/// Pull back of
				::__MoveFileBlock (pVE->lStartOfEntry + lOldSize
					, lFileSize - (pVE->lStartOfEntry + lOldSize)
					, pVE->lStartOfEntry + lNewSize, 1000000, m_fpIDX, false);
				CBlankInfo::DoStep ();
				fflush (m_fpIDX);
				/// Adjust the size of the file
				::__ftruncate (lFileSize - (lOldSize - lNewSize), m_fpIDX);

				iIndex = __FindEntryIndex (VfsName);
				iv = m_vecVFS.begin ();
				/// VEntry Table moved forward in order to modify the
				fseek (m_fpIDX, SIZEOF_VFILEHEADER, SEEK_SET);
				/// Prior to just seukip
				for(; iv <= m_vecVFS.begin () + iIndex; iv++)
				{
					__SkipVEntry (*iv);
				}
				/// This skips
				/// iv++;   /// <== Not because it is not skipping delete
				/// Subtracted from the back again as deleted
				for(; iv != m_vecVFS.end (); iv++)
				{
					(*iv)->lStartOfEntry -= lOldSize - lNewSize;
					(*iv)->pVFS->SetStartOfEntry ((*iv)->lStartOfEntry);
					__WriteVEntry (*iv);
				}
				CBlankInfo::DoStep ();
				if( itStepPos != vecStepPos.end () )
				{
					CBlankInfo::SetStep ( *itStepPos );
					itStepPos++;
				}
			}
		}
	}

	CBlankInfo::iMaxCount		= 0;
	CBlankInfo::iDealedCount	= 0;
	CBlankInfo::DoStep ();
	CBlankInfo::procCallBack	= NULL;
	
	return true;

}


/****************************************************************************************
 * Vfs Name exists investigates
 */
bool CVFS_Manager::VfsExists (const char *VfsName)
{
	char uprVfsName[ 1024 ];
	__ConvertPath (VfsName, uprVfsName); /// Uppercase and search

	std::vector<VEntry *>::iterator iv = m_vecVFS.begin ();
	for(;iv != m_vecVFS.end (); iv++)
	{
		if((*iv)->sVFSName == uprVfsName) 
			return true;
	}

	return false;
}


/*********************************************************************************
 * Check if a file exists
 * Note: First, check out the files in the. And, Pack file check
 * Pack the file number is not too much to turn the for loop because ignorant
 */
bool CVFS_Manager::FileExists (const char * FileName)
{
	std::vector<VEntry *>::iterator iv;
	char uprFileName[ 1024 ];

	/// FileName is NULL, return false immediately
	if(FileName == NULL) 
		return false;

	/// Even if there is only one file that returns true
	if(_access (FileName, 0) == 0) 
		return true;

	__ConvertPath (FileName, uprFileName); /// Pack Checked file name of the file should be capitalized halttaeneun

	iv = m_vecVFS.begin ();

	for(; iv != m_vecVFS.end (); iv++)
	{
		VEntry * pVE = *iv;

		if(pVE)
		{
			CVFS * pVFS = pVE->pVFS;

			if(pVFS->FileExists (uprFileName))
				return true;
		}
	}

	return false;
}

/// Check if a file exists only in Pack File
bool CVFS_Manager::FileExistsInVfs (const char * FileName)
{
	std::vector<VEntry *>::iterator iv;
	char uprFileName[ 1024 ];
	
	__ConvertPath (FileName, uprFileName); /// Pack Checked file name of the file should be capitalized halttaeneun
	
	iv = m_vecVFS.begin ();
	for(; iv != m_vecVFS.end (); iv++)
	{
		if((*iv)->pVFS->FileExists (uprFileName)) 
			return true;
	}

	return false;
}

/***********************************************************************************
 * Find out information about a file
 */
void CVFS_Manager::GetFileInfo (const char * FileName, VFileInfo * pFileInfo, bool bCalCrc)
{
	std::vector<VEntry *>::iterator iv = m_vecVFS.begin ();

	for(; iv != m_vecVFS.end (); iv++)
	{
		if((*iv)->pVFS->FileExists (FileName)) 
		{
			(*iv)->pVFS->GetFileInfo (FileName, pFileInfo );

			if( bCalCrc )
				pFileInfo->dwCRC = ComputeCrc ( FileName );

			return ;
		}
	}

	pFileInfo->dwCRC		= 0;
	pFileInfo->dwVersion	= 0;
}


/***********************************************************************************
 * Find out information about a file
 */
bool CVFS_Manager::SetFileInfo (const char * FileName, VFileInfo * pFileInfo)
{
	std::vector<VEntry *>::iterator iv = m_vecVFS.begin ();

	for(; iv != m_vecVFS.end (); iv++)
	{
		if((*iv)->pVFS->FileExists (FileName)) 
		{
			return (*iv)->pVFS->SetFileInfo (FileName, pFileInfo);
		}
	}

	return false;
}


/***********************************************************************************
 * Since the index file and find out the criteria for
 */
DWORD CVFS_Manager::GetStdVersion (void)
{
	DWORD dwRet = MAKEDWORD( m_wStdVersion[ 1 ], m_wStdVersion[ 0 ] );
	
	// ((WORD *)&dwRet)[ 0 ] = m_wStdVersion[ 0 ];
	// ((WORD *)&dwRet)[ 1 ] = m_wStdVersion[ 1 ];

	return dwRet;
}


/***********************************************************************************
 * Since the index file to set the criteria for
 */
void CVFS_Manager::SetStdVersion (DWORD dwVersion)
{
	SetStdVersion (HIWORD(dwVersion), LOWORD(dwVersion));
}


void CVFS_Manager::SetStdVersion (WORD wHiVer, WORD wLoVer)
{
	fseek (m_fpIDX, VERSION_POS_STD, SEEK_SET);
	fwrite ((void *)&wLoVer, sizeof (WORD), 1, m_fpIDX);
	fwrite ((void *)&wHiVer, sizeof (WORD), 1, m_fpIDX);
	fflush (m_fpIDX);
}


/***********************************************************************************
 * Applied to get a clearance version
 */
DWORD CVFS_Manager::GetCurVersion (void)
{
	DWORD dwRet = MAKEDWORD( m_wCurVersion[ 1 ] , m_wCurVersion[ 0 ] );
	
	// ((WORD *)&dwRet)[ 0 ] = m_wCurVersion[ 0 ];
	// ((WORD *)&dwRet)[ 1 ] = m_wCurVersion[ 1 ];

	return dwRet;	
}


/***********************************************************************************
 * Since bn applied
 */
void CVFS_Manager::SetCurVersion (DWORD dwVersion)
{
	SetCurVersion ( HIWORD(dwVersion), LOWORD(dwVersion) );
}


void CVFS_Manager::SetCurVersion (WORD wHiVer, WORD wLoVer)
{
	fseek (m_fpIDX, VERSION_POS_CUR, SEEK_SET);
	fwrite ((void *)&wLoVer, sizeof (WORD), 1, m_fpIDX);
	fwrite ((void *)&wHiVer, sizeof (WORD), 1, m_fpIDX);
	fflush (m_fpIDX);
}


/***********************************************************************************
 * To examine the size of the empty space.
 * Returns: the size of the space at the end of successful research. This file has no Pack also returns 0
 */
DWORD CVFS_Manager::GetSizeOfBlankArea (void)
{
	if(m_vecVFS.size () <= 0) 
		return 0;

	std::vector<VEntry *>::iterator iv = m_vecVFS.begin ();
	int iSum = 0;
	for(; iv != m_vecVFS.end (); iv++)
	{
		if(*iv) /// Enter the appropriate value iterator malgeot think that
		{
			///
			VEntry * pVEntry = *iv;
			iSum += pVEntry->pVFS->GetSizeofBlankArea ();
		}
	}

	return iSum;
}


// FileName The index data matches the real data is checked and
short CVFS_Manager::TestFile (const char * FileName)
{

	if(FileExistsInVfs ( FileName ) )
	{

		VEntry * pVEntry = GetVEntryWF ( FileName );
		VFileHandle * pVF = OpenFile (FileName );
		if(pVF && pVEntry)
		{
			VfsInfo VfsRange ;
			if(GetVfsInfo ( pVEntry->sVFSName.c_str (), &VfsRange) ) // GetVEntryWF Success should also succeeded GetVfsInfo
			{
				if (pVF->lStartOff < VfsRange.lStartOff || pVF->lEndOff > VfsRange.lEndOff )
				{
					::VCloseFile ( pVF );
					pVF = NULL;
					return VTEST_INVALIDRANGE;
				}
			}
			else
			{
				::VCloseFile ( pVF );
				pVF = NULL;
				return VTEST_CANTKNOWVFSINFO;
			}

			long lSize = GetFileLength ( FileName );
			if( lSize == 0 )
			{
				::VCloseFile ( pVF );
				pVF = NULL;
				return VTEST_ZEROLENGTH;
			}

			BYTE * pbtData = new BYTE[ lSize ];
			if( pbtData )
			{
				size_t stReadCnt = ::vfread (pbtData, sizeof (BYTE), lSize, pVF);
				if( stReadCnt != (size_t)lSize )
				{
					::VCloseFile ( pVF );
					pVF = NULL;

					delete [] pbtData;
					pbtData = NULL;

					return  VTEST_LENGTHNOTMATCH;
				}

				::VCloseFile ( pVF );
				pVF = NULL;

				DWORD dwCRC = CLibCrc::GetIcarusCrc ( pbtData , lSize );
				VFileInfo FileInfo;
				this->GetFileInfo ( FileName, &FileInfo , false);
				if( FileInfo.dwCRC != dwCRC )
				{
					delete [] pbtData;
					pbtData = NULL;

					return VTEST_CRCNOTMATCH;
				}

				delete [] pbtData;
				pbtData = NULL;

				return VTEST_SUCCESS;
			}
			return VTEST_NOTENOUGHMEM;
		}

		return VTEST_CANTOPEN;
	}

	if( FileExists ( FileName ) )
		return VTEST_OUTFILE;

	return VTEST_FILENOTEXISTS;
}


/***************************************************************
 *
 * VFS Research on the
 *
 */
// Vfs Examine the file name of the VFS Entry
VEntry * CVFS_Manager::GetVEntry ( const char * VfsName )
{
	std::vector<VEntry *>::iterator iv = m_vecVFS.begin ();

	for(; iv != m_vecVFS.end (); iv++)
	{
		VEntry * pV = *iv;
		if(pV && CLibString::CompareNI ( pV->sVFSName.c_str () , VfsName) )
			return pV;
	}

	return NULL;
}

// VFS Present in the VFS Entry searches
VEntry * CVFS_Manager::GetVEntryWF ( const char * FileName )
{
	std::vector<VEntry *>::iterator iv = m_vecVFS.begin ();

	for(; iv != m_vecVFS.end (); iv++)
	{
		VEntry * pV = *iv;
		if(pV && pV->pVFS->FileExists ( FileName ) )
			return pV;
	}

	return NULL;
}


bool CVFS_Manager::GetVfsInfo (const char * VfsName, VfsInfo * VI )
{
	VEntry * pVE = GetVEntry ( VfsName );
	if(pVE )
	{
		FILE * FP = pVE->pVFS->GetFP ();
		if(FP)
		{
			VI->lEndOff		= __vfseek (FP, 0 , SEEK_END);
			VI->lStartOff	= pVE->pVFS->GetStartOff ();

			return true;
		}
	}

	return false;
}

// Also included in the external file
DWORD CVFS_Manager::ComputeCrc ( const char * FileName)
{
	DWORD dwCrc = 0;
	VFileHandle * pVH = OpenFile ( FileName );
	if(pVH)
	{
		long lSize = GetFileLength ( FileName );
		BYTE * pbtData = new BYTE[ lSize ] ;
		if(pbtData)
		{
			if(::vfread ( pbtData, sizeof (BYTE), lSize, pVH ) == lSize)
				dwCrc = CLibCrc::GetIcarusCrc ( pbtData, lSize );

			delete [] pbtData;
			pbtData = NULL;
		}

		VCloseFile ( pVH );
		pVH = NULL;
	}

	return dwCrc;
}
