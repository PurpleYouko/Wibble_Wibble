// FV_Select.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
//#include "VFSTool.h"
#include "FV_Select.h"
#include "Dlg_Main.h"
#include "TriggerVFS.h"
#include "Dlg_InuptBox_Number.h"
#include "Mmsystem.h"
#include "fcntl.h"
#include "VFS_Manager.h"
// CFV_Select

IMPLEMENT_DYNCREATE(CFV_Select, CFormView)


CFV_Select * g_pFV_Select= NULL;


CFV_Select::CFV_Select()
	: CFormView(CFV_Select::IDD)
	, m_bInit(false)
	, m_iSelStart(0)
	, m_iSelEnd(0)
{
	g_pFV_Select = this;
}

CFV_Select::~CFV_Select()
{
	g_pFV_Select = NULL;
}

void CFV_Select::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDCLIST, m_list);
}

BEGIN_MESSAGE_MAP(CFV_Select, CFormView)
	ON_WM_SIZE()
	ON_COMMAND(ID_SELECTLIST_REMOVEFILES, OnMenu_RemoveFiles)
	ON_WM_CONTEXTMENU()
	ON_WM_KEYUP()
	ON_COMMAND(ID_SELECTLIST_SELECTALL, OnMenu_SelectAll)
	ON_COMMAND(IDM_EXTRACT, OnMenu_Extract)
	ON_COMMAND(IDM_RANDOMSELECT, OnMenu_Randomselect)
	ON_COMMAND(IDM_REMOVEFIELS_B, OnMenu_RemoveFilesB)
END_MESSAGE_MAP()


// CFV_Select �����Դϴ�.

#ifdef _DEBUG
void CFV_Select::AssertValid() const
{
	CFormView::AssertValid();
}

void CFV_Select::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG


// CFV_Select �޽��� ó�����Դϴ�.

void CFV_Select::OnSize(UINT nType, int cx, int cy)
{
	CFormView::OnSize(nType, cx, cy);

	RECT rect;
	GetClientRect (&rect);

	if(m_bInit) { m_list.MoveWindow (10, 32, rect.right - 20, rect.bottom - 52); }
	
}

void CFV_Select::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	m_bInit = true;
}

CListBox * CFV_Select::GetListBox(void)
{
	return &m_list;
}


/** 
 * ���� select �ڽ����� ���� ���� �޴��� ��������
 */
void CFV_Select::OnMenu_RemoveFiles()
{
	//int *			pIndecies = NULL;	/// ���õ� �����ε����� ���⿡ ����
	//CListBox *		plistSel = NULL;	/// ����Ʈ�ڽ��� ���� �ν��Ͻ�
	//int				nSelCnt = 0;		/// ���õ� ����
	//CString			strText;			/// ���õ� �ؽ�Ʈ
	//CVFS_Manager *	pVFS = ::g_pDlg_Main->GetVFS (); /// VFile Instance

	///// ���õ� ������ 0���� ũ��, ����Ʈ�ڽ� �ν��Ͻ��� ��ȿ�ϸ�
	//if(m_list.GetSelCount () > 0 && (plistSel = GetListBox ()))
	//{
	//	/// �ε����� �����ϱ� ���ؼ� �޸𸮸� �Ҵ�
	//	if((pIndecies = new int[ m_list.GetSelCount () ]))
	//	{
	//		/// ���õ� �ε����� �����´�
	//		m_list.GetSelItems (m_list.GetSelCount (), pIndecies);
	//		/// ���õ� ����
	//		nSelCnt = m_list.GetSelCount ();
	//		if(nSelCnt == 1 && pVFile)
	//		{
	//			plistSel->GetText (pIndecies[ 0 ], strText);
	//			/// ���õ� ������ �ϳ��� �׳� �����
	//			pVFile->RemoveFile ((char *)strText.GetString ());
	//		}
	//		else if(nSelCnt > 1 && pVFile)
	//		{
	//			/// �Ѱ� �ֱ� ���ؼ�
	//			char ** pFiles = new char *[ nSelCnt ];
	//			if(pFiles)
	//			{
	//				for(int i = 0; i < nSelCnt; i++)
	//				{
	//					/// �ؽ�Ʈ�� �����´�
	//					plistSel->GetText (pIndecies[ i ], strText);
	//					/// std::string�� �����Ѵ�. new�� �޸� �Ҵ��ϴ� ��ſ� �� ���
	//					pFiles[ i ] = new char[ strText.GetLength () + 1];
	//					strcpy (pFiles[ i ], strText.GetString ());
	//				}

	//				char buff [ 255 ];
	//				/// �����ϱ�
	//				DWORD dwOldTime = timeGetTime ();
	//				bool bRemove= pVFile->RemoveFiles ((const char **)pFiles, nSelCnt);
	//				DWORD dwNewTime = timeGetTime ();
	//				DWORD dwSec = (dwNewTime - dwOldTime) / 1000;
	//				
	//				if(bRemove)
	//				{
	//					/// ���� �޽���
	//					sprintf (buff, "%d�ʵ��� %d���� �����Ǿ����ϴ�", dwSec, nSelCnt);
	//					MessageBox (buff, "���� ����", MB_OK | MB_ICONINFORMATION);
	//				}
	//				else
	//				{
	//					/// ���� �޽���
	//					sprintf (buff, "%d�ʵ��� %d���� �����ϴµ� �����߽��ϴ�.", dwSec, nSelCnt);
	//					MessageBox (buff, "���� ����", MB_OK | MB_ICONINFORMATION);
	//				}

	//				/// �޸� ����
	//				delete pFiles;
	//			}
	//			else
	//			{
	//				/// �޸� �Ҵ� ���������� ���� �޽��� ���
	//				MessageBox ("�޸𸮸� �Ҵ��ϴµ� �����߽��ϴ�", "����", MB_OK | MB_ICONWARNING);
	//			}
	//		}

	//		/// �ؽ�Ʈ�� �����. �� �������� ������ �Ѵ�
	//		for(int i = nSelCnt - 1; i >= 0 ; i--)
	//		{
	//			plistSel->DeleteString (pIndecies[ i ]);
	//		}
	//		delete pIndecies;
	//	}
	//}
}


/**
 * Context Menu�� ��������
 */
void CFV_Select::OnContextMenu (CWnd* /*pWnd*/, CPoint point)
{
	CMenu menu , *menuSub ;
	//int iSelected ;

	//if( (iSelected = m_listEvent.GetCurSel())==LB_ERR) return ;
	menu.LoadMenu( IDR_MENU1 ) ;

	menuSub = menu.GetSubMenu(2);
	
	if(menuSub && IsSelected () ) 
	{
		//menuSub->EnableMenuItem(IDMENU_EL_COPY,MF_DISABLED | MF_GRAYED ) ;
		//menuSub->EnableMenuItem(IDMENU_EL_CUT,MF_DISABLED | MF_GRAYED ) ;
		//menuSub->EnableMenuItem(IDMENU_EL_DELETE,MF_DISABLED | MF_GRAYED ) ;
		//menuSub->EnableMenuItem(IDMENU_EL_EDIT,MF_DISABLED | MF_GRAYED ) ;

		menuSub->TrackPopupMenu(TPM_LEFTALIGN |TPM_RIGHTBUTTON, point.x, point.y, this);
	}
}

bool CFV_Select::IsSelected(void)
{
	return (m_list.GetSelCount () > 0);
}

void CFV_Select::OnKeyUp (UINT nChar, UINT nRepCnt, UINT nFlags)
{
	bool bCtrlPressed = GetKeyState (VK_RCONTROL) & 0x8000 || GetKeyState (VK_LCONTROL) & 0x8000;
	
	switch (nChar)
	{
	case 'A':
	case 'a':
		if(bCtrlPressed)
		{
		}
		break;

	}

	CFormView::OnKeyUp(nChar, nRepCnt, nFlags);
}

void CFV_Select::OnMenu_SelectAll()
{
	for(short i = 0; i < m_list.GetCount (); i++)
	{ m_list.SetSel (i); }
}


CString strTmpPath;

int CALLBACK BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
{
	TCHAR szDir[MAX_PATH];

	switch(uMsg)
	{
	case BFFM_INITIALIZED:
		if (lpData)
		{
			strcpy (szDir, strTmpPath.GetBuffer (strTmpPath.GetLength()));
			SendMessage (hwnd, BFFM_SETSELECTION, TRUE, (LPARAM)szDir);
		}
		break;

	case BFFM_SELCHANGED: 
	{
		if (SHGetPathFromIDList ((LPITEMIDLIST) lParam, szDir))
		{ SendMessage(hwnd,BFFM_SETSTATUSTEXT,0,(LPARAM)szDir); }
		break;
	}
	default:
	   break;
	}
         
	return 0;
}

/**
 * ���� ��������
*/
BOOL GetFolder(CString* strSelectedFolder, const char* lpszTitle, const HWND hwndOwner, 
				   const char* strRootFolder, const char* strStartFolder)
{
	char			pszDisplayName[ MAX_PATH ];
	LPITEMIDLIST	lpID;
	BROWSEINFOA		bi;
	
	bi.hwndOwner = hwndOwner;
	
	if (strRootFolder == NULL)
	{ bi.pidlRoot = NULL; }
	else
	{
	   LPITEMIDLIST  pIdl = NULL;
	   IShellFolder* pDesktopFolder;
	   char          szPath[ MAX_PATH ];
	   OLECHAR       olePath[ MAX_PATH ];
	   ULONG         chEaten;
	   ULONG         dwAttributes;

	   strcpy(szPath, (LPCTSTR)strRootFolder);
	   if (SUCCEEDED (SHGetDesktopFolder (&pDesktopFolder)))
	   {
		   MultiByteToWideChar (CP_ACP, MB_PRECOMPOSED, szPath, -1, olePath, MAX_PATH);
		   pDesktopFolder->ParseDisplayName (NULL, NULL, olePath, &chEaten, &pIdl, &dwAttributes);
		   pDesktopFolder->Release ();
	   }
	   bi.pidlRoot = pIdl;
	}
	bi.pszDisplayName	= pszDisplayName;
	bi.lpszTitle		= lpszTitle;
	bi.ulFlags			= BIF_RETURNONLYFSDIRS | BIF_STATUSTEXT;
	bi.lpfn				= BrowseCallbackProc;

	if (strStartFolder == NULL)
	{ bi.lParam = FALSE; }
	else
	{
		strTmpPath.Format ("%s", strStartFolder);
		bi.lParam = TRUE;
	}
	bi.iImage = NULL;
	lpID = SHBrowseForFolderA(&bi);
	if (lpID != NULL)
	{
		BOOL b = SHGetPathFromIDList (lpID, pszDisplayName);
		if (b == TRUE)
		{
			strSelectedFolder->Format ("%s",pszDisplayName);
			return TRUE;
		}
	}
	else
	{ strSelectedFolder->Empty (); }

	return FALSE;
}


/**
 * "���� Ǯ��"�� �������� ������ �ϳ� Ǫ�� �Լ�
 */
void CFV_Select::OnMenu_Extract ()
{
	int * pIndecies			= NULL;	/// ���õ� �ε����� ����
	CListBox * plistSel		= NULL; /// ����Ʈ �ڽ���
	int nSelCnt				= 0;	/// ���õ� ����
	BYTE * btBuff			= NULL;	/// �ӽù���
	CVFS_Manager * pVFS		= NULL;	/// vfs���� �ڵ�
	FILE * fp				= NULL;	/// �ӽ� ����������
	VFileHandle *pVFH		= NULL; /// vfs �����ڵ�
	CString strText = "", strFileName = "", strDir = ""; /// ��Ʈ��, �����̸�, ����


	if(m_list.GetSelCount () > 0 && (plistSel = GetListBox ()) && (pVFS = ::g_pDlg_Main->GetVFS()) 
		&&::g_pDlg_Main->GetSelectedVfsName ().GetLength () > 0)
	{
		/// ������ Ǯ ���丮�� ���´�
		if(GetFolder (&strDir, "������ Ǯ ������ �����ϼ���", GetSafeHwnd (), NULL, NULL))
		{
			/// �ε����� �����ϱ� ���ؼ� �޸𸮸� �Ҵ�
			if((pIndecies = new int[m_list.GetSelCount ()]))
			{
				/// ���õ� �ε������� ������
				m_list.GetSelItems (m_list.GetSelCount (), pIndecies);
				nSelCnt = m_list.GetSelCount ();
				for(short i = 0; i < nSelCnt; i++)
				{
					strText = "";
					/// ����Ʈ�ڽ����� ���ڿ��� ������
					plistSel->GetText (pIndecies[ nSelCnt - 1 - i ], strText);
					/// base file�� �Ҹ��ؼ� ���⿡ �ٽ� �����ϰ� �׸��� ���� ������ �� ����Ѵ�
					strFileName = strText;
					if(strFileName.ReverseFind ('\\') >= 0)
					{ 
						strFileName = strFileName.Right (strFileName.GetLength () - strFileName.ReverseFind ('\\') - 1); 
					}
					long lFileLength = pVFS->GetFileLength (strText.GetString ());
					if(lFileLength >= 0 && (btBuff = new BYTE[ lFileLength ]))
					{
						if((pVFH = pVFS->OpenFile (strText.GetString ())))
						{
							vfread (btBuff, sizeof (BYTE), (size_t)lFileLength, pVFH);
							_fmode = _O_BINARY;
							if((fp = fopen (strDir + "\\" + strFileName, "w")))
							{
								fwrite (btBuff, sizeof(BYTE), (size_t)lFileLength, fp);
								fclose (fp);
							}
							delete btBuff;
							pVFS->CloseFile (pVFH);
						}
					}
				}
				delete pIndecies;
			}
		}
	}
}


/**
 * ����Ʈ�ڽ����� ������ �������� �����Ѵ�
 */
void CFV_Select::OnMenu_Randomselect()
{
	CDlg_InuptBox_Number dlg;
	if(dlg.DoModal () == IDOK)
	{
		int iNum = 0, iIndex = -1, iItemCnt = m_list.GetCount ();
		if((iNum = dlg.GetNumber ()) > 0)
		{
			while(m_list.GetSelCount () < iNum)
			{
				iIndex = rand () % iItemCnt;
				m_list.SetSel (iIndex);
			}
		}
	}
}


/**
 * ������ ����µ�, �����ٴ� ��ŷ�� �� ���� ���
 */
void CFV_Select::OnMenu_RemoveFilesB ()
{
	int *			pIndecies = NULL;	/// ���õ� �����ε����� ���⿡ ����
	CListBox *		plistSel = NULL;	/// ����Ʈ�ڽ��� ���� �ν��Ͻ�
	int				nSelCnt = 0;		/// ���õ� ����
	CString			strText;			/// ���õ� �ؽ�Ʈ
	CVFS_Manager *	pVFS = ::g_pDlg_Main->GetVFS (); /// VFile Instance

	/// ���õ� ������ 0���� ũ��, ����Ʈ�ڽ� �ν��Ͻ��� ��ȿ�ϸ�
	if(m_list.GetSelCount () > 0 && (plistSel = GetListBox ()))
	{
		/// ���õ� ����
		nSelCnt = m_list.GetSelCount ();
		/// �ε����� �����ϱ� ���ؼ� �޸𸮸� �Ҵ�
		if((pIndecies = new int[ nSelCnt ]))
		{
			/// ���õ� �ε����� �����´�
			m_list.GetSelItems (m_list.GetSelCount (), pIndecies);
			if(nSelCnt > 1 && pVFS)
			{
				/// �Ѱ� �ֱ� ���ؼ�
				char ** pFiles = new char *[ nSelCnt ];
				if(pFiles)
				{
					for(int i = 0; i < nSelCnt; i++)
					{
						/// �ؽ�Ʈ�� �����´�
						plistSel->GetText (pIndecies[ i ], strText);
						/// std::string�� �����Ѵ�. new�� �޸� �Ҵ��ϴ� ��ſ� �� ���
						pFiles[ i ] = new char[ strText.GetLength () + 1];
						strcpy (pFiles[ i ], strText.GetString ());
					}

					if(pVFS && ::g_pDlg_Main->GetSelectedVfsName ().GetLength () > 0)
					{
						char buff [ 255 ];
						/// �����ϱ�
						DWORD dwOldTime = timeGetTime ();
						bool bRemove= pVFS->RemoveFiles (::g_pDlg_Main->GetSelectedVfsName ().GetString (), 
														(const char **)pFiles, nSelCnt);
						DWORD dwNewTime = timeGetTime ();
						DWORD dwSec = (dwNewTime - dwOldTime) / 1000;
						
						if(bRemove)
						{
							/// ���� �޽���
							sprintf (buff, "%d�ʵ��� %d���� �����Ǿ����ϴ�", dwSec, nSelCnt);
							MessageBox (buff, "���� ����", MB_OK | MB_ICONINFORMATION);
						}
						else
						{
							/// ���� �޽���
							sprintf (buff, "%d�ʵ��� %d���� �����ϴµ� �����߽��ϴ�.", dwSec, nSelCnt);
							MessageBox (buff, "���� ����", MB_OK | MB_ICONINFORMATION);
						}
					}

					/// �޸� ����
					delete pFiles;
				}
				else
				{
					/// �޸� �Ҵ� ���������� ���� �޽��� ���
					MessageBox ("�޸𸮸� �Ҵ��ϴµ� �����߽��ϴ�", "����", MB_OK | MB_ICONWARNING);
				}
			}

			/// �ؽ�Ʈ�� �����. �� �������� ������ �Ѵ�
			for(int i = nSelCnt - 1; i >= 0 ; i--)
			{
				plistSel->DeleteString (pIndecies[ i ]);
			}
			delete pIndecies;
		}
	}
}
