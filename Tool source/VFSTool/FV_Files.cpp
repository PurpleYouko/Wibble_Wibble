// FV_Files.cpp : 구현 파일입니다.
//

#include "stdafx.h"
//#include "VFSTool.h"
#include "FV_Files.h"
#include "FV_Select.h"
#include "__Global.h"
#include <string>
#include "TriggerVFS.h"
#include "Dlg_Main.h"

// CFV_Files

IMPLEMENT_DYNCREATE(CFV_Files, CFormView)


CFV_Files * g_pFV_Files = NULL;


CFV_Files::CFV_Files()
	: CFormView(CFV_Files::IDD)	, m_bInit(false)
	, m_strCurPath(_T(""))
{
	g_pFV_Files = this;
}


CFV_Files::~CFV_Files()
{
	g_pFV_Files = NULL;
}

void CFV_Files::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDCLIST, m_list);
}

BEGIN_MESSAGE_MAP(CFV_Files, CFormView)
	ON_WM_SIZE()
	ON_COMMAND(ID_FILELIST_ADD, OnMenu_AddFiles)
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_FILELIST_SELECTALL, OnMenu_SelectAll)
END_MESSAGE_MAP()


// CFV_Files 진단입니다.

#ifdef _DEBUG
void CFV_Files::AssertValid() const
{
	CFormView::AssertValid();
}

void CFV_Files::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG


// CFV_Files 메시지 처리기입니다.

void CFV_Files::OnSize(UINT nType, int cx, int cy)
{
	CFormView::OnSize(nType, cx, cy);
	
	RECT rect;
	GetClientRect (&rect);

	if(m_bInit) { m_list.MoveWindow (10, 32, rect.right - 20, rect.bottom - 52); }
}

void CFV_Files::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	m_bInit = true;
}


CListBox * CFV_Files::GetListBox(void)
{
	return &m_list;
}


/**
 * 디렉토리에 있는 모든 파일을 리스트박스에 보여 준다
 * @param path 디렉토리
 */
bool CFV_Files::DisplayFiles (CString path)
{
	/// 경로명의 길이가 0이 아니고, 현재 디렉토리를 변경 가능하면
	if(path.GetLength () > 0 && SetCurrentDirectory (path) && GetListBox ())
	{
		/// 현재 디렉토리를 m_strCurPath에 등록
		m_strCurPath = path;
		/// 리스트박스의 기존 내용을 모두 지운다
		GetListBox ()->ResetContent ();

		CFileFind finder;
		BOOL bWorking = finder.FindFile("*.*");
		while (bWorking)
		{
			bWorking = finder.FindNextFile ();
			if(!finder.IsDirectory ())
			{ GetListBox ()->AddString (finder.GetFileName ()); }
		}

		return true;
	}	

	return false;
}

void CFV_Files::OnMenu_AddFiles()
{
	int *			pIndecies	= NULL;		/// 선택된 인덱스
	CString			strText; 
	CListBox *		plistSel	= NULL;
	CVFS_Manager *	pVFS		= NULL;		/// 인스턴스

	CString VfsName = ::g_pDlg_Main->GetSelectedVfsName ();	/// 현재 선택된 Vfs이름을 가져옴
	plistSel = ::g_pFV_Select->GetListBox ();				/// 파일이 추가되는 리스트박스를 가져옴
	if(m_list.GetSelCount () > 0 && plistSel && VfsName.GetLength () > 0)
	{
		/// 인덱스를 저장하기 위해서 메모리를 할당
		std::string sSelPath = m_strCurPath.GetString ();
		const std::basic_string <char>::size_type npos = -1;
		if(sSelPath.find (::__GetAbsPath ().GetString (), 0) == npos)
		{
			MessageBox ("기준디렉토리를 선택하지 않았거나 범위를 벗어나는 디렉토리입니다");
			return;
		}
		if((pIndecies = new int[m_list.GetSelCount ()]))
		{
			m_list.GetSelItems (m_list.GetSelCount (), pIndecies);
			CString prefix = ::__SubString (::__GetAbsPath ().GetLength (), m_strCurPath);
			prefix.TrimLeft ('\\');
			if(prefix.GetLength () > 0 ) { prefix += "\\"; }

			for(short i = 0; i < m_list.GetSelCount (); i++)
			{
				strText = "";
				m_list.GetText (pIndecies[ i ], strText);
				plistSel->AddString (prefix + strText);
				/// 지금 오픈된 vfs 파일이 있으면 바로 추가한다
				if((pVFS = ::g_pDlg_Main->GetVFS ()) && ::__GetAbsPath ().GetLength () > 0)
				{
					SetCurrentDirectory (__GetAbsPath ());
					CString strAddFile = prefix + strText;
					short shVersion = 0;
					pVFS->AddFile (VfsName.GetString (), strAddFile.GetString (), strAddFile.GetString (), shVersion, 0, 0);
				}
			}
			delete pIndecies;
		}
	}
}


void CFV_Files::OnContextMenu(CWnd* /*pWnd*/, CPoint point)
{
		CMenu menu , *menuSub ;
	//int iSelected ;

	//if( (iSelected = m_listEvent.GetCurSel())==LB_ERR) return ;
	menu.LoadMenu( IDR_MENU1 ) ;

	menuSub = menu.GetSubMenu(1);
	
	if(menuSub && IsSelected () ) 
	{
		//menuSub->EnableMenuItem(IDMENU_EL_COPY,MF_DISABLED | MF_GRAYED ) ;
		//menuSub->EnableMenuItem(IDMENU_EL_CUT,MF_DISABLED | MF_GRAYED ) ;
		//menuSub->EnableMenuItem(IDMENU_EL_DELETE,MF_DISABLED | MF_GRAYED ) ;
		//menuSub->EnableMenuItem(IDMENU_EL_EDIT,MF_DISABLED | MF_GRAYED ) ;

		menuSub->TrackPopupMenu(TPM_LEFTALIGN |TPM_RIGHTBUTTON, point.x, point.y, this);
	}
	
}


bool CFV_Files::IsSelected(void)
{
	return (m_list.GetSelCount () > 0);
}


void CFV_Files::OnMenu_SelectAll()
{
	for(short i = 0; i < m_list.GetCount (); i++)
	{ m_list.SetSel (i); }
}
