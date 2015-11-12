// FV_Select.cpp : 구현 파일입니다.
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


// CFV_Select 진단입니다.

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


// CFV_Select 메시지 처리기입니다.

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
 * 파일 select 박스에서 파일 제거 메뉴를 눌렀을때
 */
void CFV_Select::OnMenu_RemoveFiles()
{
	//int *			pIndecies = NULL;	/// 선택된 파일인덱스를 여기에 저장
	//CListBox *		plistSel = NULL;	/// 리스트박스에 대한 인스턴스
	//int				nSelCnt = 0;		/// 선택된 갯수
	//CString			strText;			/// 선택된 텍스트
	//CVFS_Manager *	pVFS = ::g_pDlg_Main->GetVFS (); /// VFile Instance

	///// 선택된 갯수가 0보다 크고, 리스트박스 인스턴스가 유효하면
	//if(m_list.GetSelCount () > 0 && (plistSel = GetListBox ()))
	//{
	//	/// 인덱스를 저장하기 위해서 메모리를 할당
	//	if((pIndecies = new int[ m_list.GetSelCount () ]))
	//	{
	//		/// 선택된 인덱스를 가져온다
	//		m_list.GetSelItems (m_list.GetSelCount (), pIndecies);
	//		/// 선택된 갯수
	//		nSelCnt = m_list.GetSelCount ();
	//		if(nSelCnt == 1 && pVFile)
	//		{
	//			plistSel->GetText (pIndecies[ 0 ], strText);
	//			/// 선택된 파일이 하나면 그냥 지운다
	//			pVFile->RemoveFile ((char *)strText.GetString ());
	//		}
	//		else if(nSelCnt > 1 && pVFile)
	//		{
	//			/// 넘겨 주기 위해서
	//			char ** pFiles = new char *[ nSelCnt ];
	//			if(pFiles)
	//			{
	//				for(int i = 0; i < nSelCnt; i++)
	//				{
	//					/// 텍스트를 가져온다
	//					plistSel->GetText (pIndecies[ i ], strText);
	//					/// std::string에 저장한다. new로 메모리 할당하는 대신에 이 방법
	//					pFiles[ i ] = new char[ strText.GetLength () + 1];
	//					strcpy (pFiles[ i ], strText.GetString ());
	//				}

	//				char buff [ 255 ];
	//				/// 삭제하기
	//				DWORD dwOldTime = timeGetTime ();
	//				bool bRemove= pVFile->RemoveFiles ((const char **)pFiles, nSelCnt);
	//				DWORD dwNewTime = timeGetTime ();
	//				DWORD dwSec = (dwNewTime - dwOldTime) / 1000;
	//				
	//				if(bRemove)
	//				{
	//					/// 성공 메시지
	//					sprintf (buff, "%d초동안 %d개가 삭제되었습니다", dwSec, nSelCnt);
	//					MessageBox (buff, "파일 삭제", MB_OK | MB_ICONINFORMATION);
	//				}
	//				else
	//				{
	//					/// 실패 메시지
	//					sprintf (buff, "%d초동안 %d개가 삭제하는데 실패했습니다.", dwSec, nSelCnt);
	//					MessageBox (buff, "파일 삭제", MB_OK | MB_ICONINFORMATION);
	//				}

	//				/// 메모리 해제
	//				delete pFiles;
	//			}
	//			else
	//			{
	//				/// 메모리 할당 실패했을때 에러 메시지 출력
	//				MessageBox ("메모리를 할당하는데 실패했습니다", "에러", MB_OK | MB_ICONWARNING);
	//			}
	//		}

	//		/// 텍스트를 지운다. 꼭 역순으로 지워야 한다
	//		for(int i = nSelCnt - 1; i >= 0 ; i--)
	//		{
	//			plistSel->DeleteString (pIndecies[ i ]);
	//		}
	//		delete pIndecies;
	//	}
	//}
}


/**
 * Context Menu를 눌렀을때
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
 * 폴더 가져오기
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
 * "파일 풀기"를 눌렀을때 파일을 하나 푸는 함수
 */
void CFV_Select::OnMenu_Extract ()
{
	int * pIndecies			= NULL;	/// 선택된 인덱스를 저장
	CListBox * plistSel		= NULL; /// 리스트 박스용
	int nSelCnt				= 0;	/// 선택된 갯수
	BYTE * btBuff			= NULL;	/// 임시버퍼
	CVFS_Manager * pVFS		= NULL;	/// vfs파일 핸들
	FILE * fp				= NULL;	/// 임시 파일포인터
	VFileHandle *pVFH		= NULL; /// vfs 파일핸들
	CString strText = "", strFileName = "", strDir = ""; /// 스트링, 파일이름, 폴더


	if(m_list.GetSelCount () > 0 && (plistSel = GetListBox ()) && (pVFS = ::g_pDlg_Main->GetVFS()) 
		&&::g_pDlg_Main->GetSelectedVfsName ().GetLength () > 0)
	{
		/// 파일을 풀 디렉토리를 묻는다
		if(GetFolder (&strDir, "파일을 풀 폴더를 선택하세요", GetSafeHwnd (), NULL, NULL))
		{
			/// 인덱스를 저장하기 위해서 메모리를 할당
			if((pIndecies = new int[m_list.GetSelCount ()]))
			{
				/// 선택된 인덱스들을 가져옴
				m_list.GetSelItems (m_list.GetSelCount (), pIndecies);
				nSelCnt = m_list.GetSelCount ();
				for(short i = 0; i < nSelCnt; i++)
				{
					strText = "";
					/// 리스트박스에서 문자열을 가져옴
					plistSel->GetText (pIndecies[ nSelCnt - 1 - i ], strText);
					/// base file명만 불리해서 여기에 다시 저장하고 그리고 파일 생성할 때 사용한다
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
 * 리스트박스에서 파일을 랜덤으로 선택한다
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
 * 파일을 지우는데, 지웠다는 마킹만 해 놓는 경우
 */
void CFV_Select::OnMenu_RemoveFilesB ()
{
	int *			pIndecies = NULL;	/// 선택된 파일인덱스를 여기에 저장
	CListBox *		plistSel = NULL;	/// 리스트박스에 대한 인스턴스
	int				nSelCnt = 0;		/// 선택된 갯수
	CString			strText;			/// 선택된 텍스트
	CVFS_Manager *	pVFS = ::g_pDlg_Main->GetVFS (); /// VFile Instance

	/// 선택된 갯수가 0보다 크고, 리스트박스 인스턴스가 유효하면
	if(m_list.GetSelCount () > 0 && (plistSel = GetListBox ()))
	{
		/// 선택된 갯수
		nSelCnt = m_list.GetSelCount ();
		/// 인덱스를 저장하기 위해서 메모리를 할당
		if((pIndecies = new int[ nSelCnt ]))
		{
			/// 선택된 인덱스를 가져온다
			m_list.GetSelItems (m_list.GetSelCount (), pIndecies);
			if(nSelCnt > 1 && pVFS)
			{
				/// 넘겨 주기 위해서
				char ** pFiles = new char *[ nSelCnt ];
				if(pFiles)
				{
					for(int i = 0; i < nSelCnt; i++)
					{
						/// 텍스트를 가져온다
						plistSel->GetText (pIndecies[ i ], strText);
						/// std::string에 저장한다. new로 메모리 할당하는 대신에 이 방법
						pFiles[ i ] = new char[ strText.GetLength () + 1];
						strcpy (pFiles[ i ], strText.GetString ());
					}

					if(pVFS && ::g_pDlg_Main->GetSelectedVfsName ().GetLength () > 0)
					{
						char buff [ 255 ];
						/// 삭제하기
						DWORD dwOldTime = timeGetTime ();
						bool bRemove= pVFS->RemoveFiles (::g_pDlg_Main->GetSelectedVfsName ().GetString (), 
														(const char **)pFiles, nSelCnt);
						DWORD dwNewTime = timeGetTime ();
						DWORD dwSec = (dwNewTime - dwOldTime) / 1000;
						
						if(bRemove)
						{
							/// 성공 메시지
							sprintf (buff, "%d초동안 %d개가 삭제되었습니다", dwSec, nSelCnt);
							MessageBox (buff, "파일 삭제", MB_OK | MB_ICONINFORMATION);
						}
						else
						{
							/// 실패 메시지
							sprintf (buff, "%d초동안 %d개가 삭제하는데 실패했습니다.", dwSec, nSelCnt);
							MessageBox (buff, "파일 삭제", MB_OK | MB_ICONINFORMATION);
						}
					}

					/// 메모리 해제
					delete pFiles;
				}
				else
				{
					/// 메모리 할당 실패했을때 에러 메시지 출력
					MessageBox ("메모리를 할당하는데 실패했습니다", "에러", MB_OK | MB_ICONWARNING);
				}
			}

			/// 텍스트를 지운다. 꼭 역순으로 지워야 한다
			for(int i = nSelCnt - 1; i >= 0 ; i--)
			{
				plistSel->DeleteString (pIndecies[ i ]);
			}
			delete pIndecies;
		}
	}
}
