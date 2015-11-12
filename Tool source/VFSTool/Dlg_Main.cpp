// Dlg_Main.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
//#include "VFSTool.h"
#include "Dlg_Main.h"
#include "DirTreeCtrl.h"
#include "TriggerVFS.h"
#include "FV_Select.h"
#include <string>
#include <vector>
#include <Mmsystem.h>
#include "libString.h"
#include "libfile.h"
// CDlg_Main

IMPLEMENT_DYNCREATE(CDlg_Main, CFormView)

CDlg_Main * g_pDlg_Main		= NULL;
CVFS_Manager * g_pVFS		= NULL;


CDlg_Main::CDlg_Main()
	: CFormView(CDlg_Main::IDD)
	, m_strWorkingDir(_T(""))
	, m_strCurVfsName(_T(""))
{
	g_pDlg_Main = this;
}

CDlg_Main::~CDlg_Main()
{
	g_pDlg_Main = NULL;
	/// ������ ������ �ʾ����� �ݴ´�
	if(::g_pVFS) 
	{ 
		g_pVFS->Close ();
		delete g_pVFS;
		g_pVFS = NULL;
	}
}

void CDlg_Main::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDCSTATIC_FOLDER	, m_stAbsPath);
	DDX_Control(pDX, IDCSTATIC_FILENAME	, m_FileName);
	DDX_Control(pDX, IDCEDIT_EXCPREFIX	, m_editExcPrefix);
	DDX_Control(pDX, IDCEDIT_EXCEXT		, m_editExcExt);
	DDX_Control(pDX, IDCSTATIC_NUM		, m_staticFileNum);
	DDX_Control(pDX, IDC_LIST1			, m_listVFS);
	DDX_Control(pDX, IDCEDIT_VERSION, m_editVersion);
}

BEGIN_MESSAGE_MAP(CDlg_Main, CFormView)
	ON_BN_CLICKED(IDCBUTTON_SAVEFILE,	OnBnClick_Savefile)
	ON_BN_CLICKED(IDCBUTTON_OPEN,		OnBnClick_OpenFile)
	ON_BN_CLICKED(IDCBUTTON_OPEN2,		OnBnClickedOpen2)
	ON_BN_CLICKED(IDCBT_CLEARBLANK,		OnBnClick_ClearBlank)
	ON_LBN_SELCHANGE(IDC_LIST1, 		OnlistVFS_SelChange)
	ON_BN_CLICKED(IDCBT_ADDVFS, 		OnBnClick_AddVFS)
END_MESSAGE_MAP()


// CDlg_Main �����Դϴ�.

#ifdef _DEBUG
void CDlg_Main::AssertValid() const
{
	CFormView::AssertValid();
}

void CDlg_Main::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG


// CDlg_Main �޽��� ó�����Դϴ�.


/**
 * ���ص��丮�� ����
 */
void CDlg_Main::SetAbsPath(CString path)
{
	m_stAbsPath.SetWindowText (path);
}


/**
 * ���ص��丮�� �˾Ƴ�
 */
CString CDlg_Main::GetAbsPath(void)
{	
	CString strPath;

	m_stAbsPath.GetWindowText (strPath);

	return strPath;
}

void CDlg_Main::OnMenu_SetAbsPath(void)
{
	SetAbsPath (::g_pCtrl_DirTree->GetSelectedPath ());
}


/// ���ܽ�ų �������� ������ �´�
CString CDlg_Main::GetExcPrefix(void)
{
	CString retStr;

	m_editExcPrefix.GetWindowText (retStr);

	return retStr;
}


/// ���ܽ�ų Ȯ���ڸ� ������ �´�
CString CDlg_Main::GetExcExt(void)
{
	CString retStr;

	m_editExcExt.GetWindowText (retStr);

	return retStr;
}

void CDlg_Main::OnBnClick_Savefile()
{
	CVFS_Manager *	VF = NULL;
	CListBox *		List = NULL;
	CString			strText;

	if(GetAbsPath ().GetLength () <= 0) 
	{ 
		MessageBox ("���� ���丮�� �������� �ʾҽ��ϴ�"); 
		return ; 
	}

	CFileDialog pDlg (TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, 
						NULL);

	if(pDlg.DoModal () == IDOK) 
	{
		CString s  = pDlg.GetPathName ();
		std::string sDir = GetDirectory (s.GetString ());
		m_strWorkingDir = sDir.c_str ();					/// ���� �ε��������� ����Ǵ� ���丮�� ������ ���´�

		VF = new CVFS_Manager;
		if((VF->Open((char *)s.GetString (), "w+")))
		{
			SetCurrentDirectory (GetAbsPath ());
			CString VfsName = GetSelectedVfsName ();
			if(VfsName.GetLength () <= 0) { MessageBox ("vfs������ �������� �ʾҽ��ϴ�"); delete VF; return; }
			if(!VF->VfsExists (VfsName.GetString ()))
			{
				if(!VF->AddVFS (VfsName.GetString ()))
				{
					delete VF;
					return ;
				}
			}
			/// ���� ���õ� vfsname�� ������ ������ �߰��� �� ����
			if(VfsName.GetLength () > 0)
			{
				if((List = ::g_pFV_Select->GetListBox ()))
				{
					for(short i = 0; i < List->GetCount (); i++)
					{
						List->GetText (i, strText);
						short shVersion = 0;
						VF->AddFile (VfsName.GetString (), strText.GetString (), strText.GetString (), shVersion, 0, 0, true);
					}
				}
				SetFileName (s);
				::g_pVFS = VF;
			}
			else
			{ delete VF; }
		}
		else
		{ delete VF; }
	}
}


/**
 * vfs ������ �����Ѵ�
 */
void CDlg_Main::OnBnClick_OpenFile ()
{
	CListBox *		List = NULL;
	CVFS_Manager *	pVFS = NULL;
	char **			ppVfsNames = NULL;

	CFileDialog pDlg (TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, NULL);

	if(pDlg.DoModal () == IDOK) 
	{
		std::string sDir;						/// index ������ ����Ǵ� ��θ� �����Ѵ�
		CString s  = pDlg.GetPathName ();
		sDir = GetDirectory (s.GetString ());	/// ���丮�� ������ ���´�
		m_strCurVfsName = sDir.c_str ();		/// ��� ������ �ű��

		pVFS = new CVFS_Manager;
		if(pVFS->Open((char *)s.GetString (), "r+"))
		{
			SetCurrentDirectory (GetAbsPath ());
			if((List = ::g_pFV_Select->GetListBox ()))
			{
				List->ResetContent ();
				m_listVFS.ResetContent ();

				/// VFS ���ϸ��� �����´�
				short sVFSCnt = (short)pVFS->GetVFSCount ();
				if((ppVfsNames = new char *[ sVFSCnt ]))
				{
					for(short sI = 0; sI < sVFSCnt; sI++)
					{
						if(!(ppVfsNames [ sI ] = new char[ 255 ])) 
						{ 
							delete ppVfsNames;
							return;
						}
						else
						{
							ZeroMemory (ppVfsNames [ sI ], 255);
						}
					}
				}
				else { return ; }
				pVFS->GetVfsNames (ppVfsNames, sVFSCnt, 255);
				for(int j = 0; j < sVFSCnt; j++) 
				{
					m_listVFS.AddString (ppVfsNames[ j ]);
					delete ppVfsNames[ j ];
				}
				delete ppVfsNames;

				m_listVFS.SetSel (0);

				m_listVFS.GetText (0, m_strCurVfsName);

				if(m_strCurVfsName.GetLength () > 0)
				{
					/// ���������� ������ �����´�
					DWORD dwFileCnt = pVFS->GetFileCount (m_strCurVfsName.GetString());
					char ** pFiles = new char *[ dwFileCnt ];
					if(pFiles)
					{
						DWORD i = 0;
						for(i = 0; i < dwFileCnt; i++)
						{
							pFiles[ i ] = new char[ 255 ];
							if(!pFiles[ i ]) { return; }
							ZeroMemory (pFiles[ i ], 255);
						}
						pVFS->GetFileNames (m_strCurVfsName.GetString (), pFiles, dwFileCnt, 255);
						for(i = 0; i < dwFileCnt; i++)
						{
							List->AddString (pFiles[ i ]);
							delete pFiles[ i ];
						}
						delete pFiles;
						SetFileNum (dwFileCnt);
					}
					//for(short i = 0; i < pVFile->mapFileEntry.size (); i++)
					//{
					//	FileEntry * pEntry = pVFile->mapFileEntry[ i ];
					//	CString strr = pVFile->mapFileEntry[ i ].sFileName.c_str ();
					//	List->AddString (pVFile->mapFileEntry[ i ].sFileName.c_str ());
					//}
				}
			}
			SetFileName (s);
			/// ������ �� �ݰ�
			if(::g_pVFS) { ::g_pVFS->Close (); }
			::g_pVFS = pVFS;

		}
		else
		{
			delete pVFS;
			pVFS = NULL;
		}
	}

}


/**
 * �����̸��� �����Ѵ�
 */
void CDlg_Main::SetFileName(CString File)
{
	m_FileName.SetWindowText (File);
}


/**
 *  �����̸��� �����´�
 */
CString CDlg_Main::GetFileName(void)
{
	CString str;
	m_FileName.GetWindowText (str);

	return str;
}

CVFS_Manager * CDlg_Main::GetVFS (void)
{
	return g_pVFS;
}

/**
 * vfs ������ ������
 */
void CDlg_Main::OnBnClickedOpen2()
{
	if(::g_pVFS)
	{
		/// ���⼭�� vfclose�� ������� ����
		g_pVFS->Close ();
		g_pVFS = NULL;
		SetFileName ("");
		::g_pFV_Select->GetListBox ()->ResetContent ();
		m_listVFS.ResetContent ();
	}
}


/**
 * �������� ���� ����
 */
void CDlg_Main::SetFileNum(DWORD dwNum)
{
	char buff [ 255] ;
	sprintf (buff, "%d", dwNum);
	m_staticFileNum.SetWindowText (buff);
}



void CDlg_Main::OnBnClick_ClearBlank()
{
	HINSTANCE	hInst = (HINSTANCE)::GetModuleHandle (NULL);
	HCURSOR		hc = NULL, hOldCursor = NULL;
	DWORD		dwOldTime = 0, dwTimeDiff = 0;
	DWORD		dwDelCnt = 0, dwFileH = 0;
	CString		strSelVfs;

	strSelVfs = GetSelectedVfsName ();
	if(::g_pVFS && strSelVfs.GetLength () > 0) 
	{ 
		hc = LoadCursor (hInst, /*MAKEINTRESOURCE(IDC_CURSOR1)*/ IDC_WAIT); 
		hOldCursor	= SetCursor (hc);

		dwDelCnt	= g_pVFS->GetDelCnt (strSelVfs.GetString ());
		dwFileH		= g_pVFS->GetFileCntWithHole (strSelVfs.GetString ());
		dwOldTime	= timeGetTime ();
		::g_pVFS->ClearBlank (strSelVfs.GetString ());
		dwTimeDiff	= ( timeGetTime () - dwOldTime ) / 1000;
		
		if(hOldCursor)
		{
			DestroyCursor (hc);
			SetCursor (hOldCursor);
		}

		char buff [255];
		sprintf (buff, "%d�ʵ��� %d���� ������ ���ŵǾ���, %d�� ������ ������ �����Ǿ����ϴ�", 
			dwTimeDiff, dwDelCnt, dwFileH);

		MessageBox (buff, "���� ���", MB_OK | MB_ICONINFORMATION);
	}
}


/**
 * ListBox���� ���ϸ��� �������� ��
 */
void CDlg_Main::OnlistVFS_SelChange()
{
	CString strOldName = m_strCurVfsName;
	CListBox * List = NULL;
	CVFS_Manager * pVFS = ::g_pVFS;

	/// ���õ� ���ϸ��� ����
	m_listVFS.GetText (m_listVFS.GetCurSel (), m_strCurVfsName);
	if(m_listVFS.GetCurSel () != LB_ERR && (List = ::g_pFV_Select->GetListBox ()) && pVFS)
	{ 
		if(m_strCurVfsName.GetLength () > 0)
		{
			List->ResetContent ();
			/// ���������� ������ �����´�
			DWORD dwFileCnt = pVFS->GetFileCount (m_strCurVfsName.GetString());
			char ** pFiles = new char *[ dwFileCnt ];
			if(pFiles)
			{
				DWORD i = 0;
				for(i = 0; i < dwFileCnt; i++)
				{
					pFiles[ i ] = new char[ 255 ];
					if(!pFiles[ i ]) { return; }
					ZeroMemory (pFiles[ i ], 255);
				}
				pVFS->GetFileNames (m_strCurVfsName.GetString (), pFiles, dwFileCnt, 255);
				for(i = 0; i < dwFileCnt; i++)
				{
					List->AddString (pFiles[ i ]);
					delete pFiles[ i ];
				}
				delete pFiles;
				SetFileNum (dwFileCnt);
			}	
		}
		else
		{
			m_strCurVfsName = strOldName;
		}
	}
}


/**
 * VFS File�� �߰��Ѵ�
 */
void CDlg_Main::OnBnClick_AddVFS ()
{
	CFileDialog pDlg (TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, NULL);

	if(pDlg.DoModal ()== IDOK)
	{
		CString sPath		= pDlg.GetPathName ();
		CString sFileName	= GetBaseName (sPath.GetString ());
		std::string sDir	= GetDirectory (sPath.GetString ());

		if(sFileName.GetLength () > 0 && ::g_pVFS && !::g_pVFS->VfsExists (sFileName.GetString ()))
		{
			if(g_pVFS->AddVFS (sFileName.GetString ()))
			{ m_listVFS.AddString (sFileName); }
		}
		else if(sFileName.GetLength () > 0)
		{ m_listVFS.AddString (sFileName); }
	}
}

CString CDlg_Main::GetSelectedVfsName (void)
{
	return m_strCurVfsName;
}


CString CDlg_Main::GetSelVfsName (void)
{
	return m_strCurVfsName;
}

CString CDlg_Main::GetWorkDir(void)
{
	return m_strWorkingDir;
}

short CDlg_Main::GetVersion(void)
{
	CString strVersion;
	short	shVersion = 0;
	char *	buff = NULL;
	int		iDotPos = 0, i;

	m_editVersion.GetWindowText (strVersion);
	buff = new char[ strVersion.GetLength () + 1 ];
	if(!buff) { MessageBox ("�޸� �Ҵ� ����"); return -1; }

	for(i = 0; i < strlen (buff); i++)
	{
		if(buff[ i ] == '.') { iDotPos = i; break; }
	}

	if(i >= strlen(buff)) { MessageBox ("������ x.x�������� �Է��� �ּ���"); return -1; }

	((char *)shVersion)[ 0 ] = atoi (buff + iDotPos + 1);	/// ���������� ������Ʈ��
	buff[ iDotPos ] = 0;
	((char *)shVersion)[ 1 ] = atoi (buff);					/// ���� ������ ������Ʈ��
	
	return shVersion;
}
