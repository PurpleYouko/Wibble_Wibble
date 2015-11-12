// LeftView.cpp : implementation of the CLeftView class
//

#include "stdafx.h"
#include "DirTreeDemo.h"

#include "DirTreeDemoDoc.h"
#include "LeftView.h"
#include <direct.h>
#include "Dlg_Main.h"
#include "lib_regexp.h"
#include "TriggerVFS.h"
#include "__Global.h"
#include "FV_Select.h"
#include "libFile.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLeftView

IMPLEMENT_DYNCREATE(CLeftView, CView)

BEGIN_MESSAGE_MAP(CLeftView, CView)
	//{{AFX_MSG_MAP(CLeftView)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
//	ON_WM_RBUTTONUP()
ON_WM_CONTEXTMENU()
ON_COMMAND(IDM_SETABS, OnConMenu_SetAbsPath)
ON_COMMAND(ID_DIRTREE_137, OnMenu_AddAllFiles)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLeftView construction/destruction

CLeftView::CLeftView()
{
	// TODO: add construction code here

}

CLeftView::~CLeftView()
{
}

BOOL CLeftView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CLeftView drawing

void CLeftView::OnDraw(CDC* pDC)
{
	CDirTreeDemoDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	// TODO: add draw code for native data here
}


/////////////////////////////////////////////////////////////////////////////
// CLeftView printing

BOOL CLeftView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CLeftView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CLeftView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

void CLeftView::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	// TODO: You may populate your TreeView with items by directly accessing
	//  its tree control through a call to GetTreeCtrl().
}

/////////////////////////////////////////////////////////////////////////////
// CLeftView diagnostics

#ifdef _DEBUG
void CLeftView::AssertValid() const
{
	CView::AssertValid();
}

void CLeftView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CDirTreeDemoDoc* CLeftView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CDirTreeDemoDoc)));
	return (CDirTreeDemoDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CLeftView message handlers

BOOL CLeftView::Create (LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	BOOL  bRet;

	bRet = CWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);

	// this part creates the TreeCtrl and use the CLeftView
	// as his parent Window
	
	char szWorkDir[ 256 ] ;
	/// 여기서 왼쪽 창의 Directory Tree를 위한 Tree Control을 추가한다
	if ( m_DirTree.m_hWnd == NULL && bRet )
	{
		bRet = m_DirTree.Create (WS_CHILD | TVS_LINESATROOT | TVS_HASBUTTONS | WS_VISIBLE | TVS_HASLINES, 
			                     CRect(0, 0, 0, 0), this, ID_TREECTRL );
		_getcwd( szWorkDir, 256 );
		// set the Path to the current Work-Directory
		m_DirTree.SetSelPath ( szWorkDir );

		/// 
		if ( bRet )
		{ m_DirTree.DisplayTree ( NULL, TRUE ); }
	}

	return bRet;
}

void CLeftView::OnSize(UINT nType, int cx, int cy) 
{
	CView::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	if ( m_DirTree.m_hWnd )
		m_DirTree.SetWindowPos( NULL, 0, 0, cx, cy, SWP_NOZORDER | SWP_NOMOVE );
	
}


/**
 * Context Menu를 눌렀을때
 */
void CLeftView::OnContextMenu(CWnd* /*pWnd*/, CPoint point)
{
		CMenu menu , *menuSub ;
	//int iSelected ;

	//if( (iSelected = m_listEvent.GetCurSel())==LB_ERR) return ;
	menu.LoadMenu( IDR_MENU1 ) ;

	menuSub = menu.GetSubMenu(0);
	
	if(menuSub && IsSelected () ) 
	{
		//menuSub->EnableMenuItem(IDMENU_EL_COPY,MF_DISABLED | MF_GRAYED ) ;
		//menuSub->EnableMenuItem(IDMENU_EL_CUT,MF_DISABLED | MF_GRAYED ) ;
		//menuSub->EnableMenuItem(IDMENU_EL_DELETE,MF_DISABLED | MF_GRAYED ) ;
		//menuSub->EnableMenuItem(IDMENU_EL_EDIT,MF_DISABLED | MF_GRAYED ) ;

		menuSub->TrackPopupMenu(TPM_LEFTALIGN |TPM_RIGHTBUTTON, point.x, point.y, this);
	}
	
}

bool CLeftView::IsSelected(void)
{
	if(m_DirTree.GetSelectedItem ()) { return true; }

	return false;
}

void CLeftView::OnConMenu_SetAbsPath()
{
    ::g_pDlg_Main->OnMenu_SetAbsPath ();

	if(IsSelected ())
	{ 
		/// 현재 작업 디렉토리를 설정
		SetCurrentDirectory (m_DirTree.GetSelectedPath ());
		m_DirTree.SetSelPath (m_DirTree.GetSelectedPath ()); 
	}
}

void CLeftView::OnMenu_AddAllFiles()
{
	CString			strExt, strDir;
	strExt			= ::g_pDlg_Main->GetExcExt ();
	strDir			= ::g_pDlg_Main->GetExcPrefix ();
	stFindFiles *	Find = NULL;
	CVFS_Manager *	pVFS = NULL;
	CString			VfsName;							/// 현재 VfsName을 저장

	pVFS = ::g_pDlg_Main->GetVFS ();					/// VFS객체를 가져옴
	VfsName = ::g_pDlg_Main->GetSelectedVfsName ();

	if(IsSelected () && m_DirTree.GetSelectedPath ().GetLength () > 0)
	{		
		std::string sSelPath = m_DirTree.GetSelectedPath ().GetString ();
		const std::basic_string <char>::size_type npos = -1;
		if(sSelPath.find (::__GetAbsPath (), 0) != npos)
		{
			/// 작업디렉토리를 꼭 바꿔야 한다
			SetCurrentDirectory (::__GetAbsPath ());
			/// 전체 경로에서 상대경로 해석. 기준디렉토리 기준
			CString Base = ::__SubString (::__GetAbsPath ().GetLength (), m_DirTree.GetSelectedPath ());
			Base.TrimLeft ('\\');
			if((Find = ::GetFileList (Base.GetString (), strExt.GetString (), strDir.GetString ())))
			{
				std::vector<std::string>::iterator iv = Find->FileList.begin ();
				for(; iv != Find->FileList.end (); iv++)
				{
					/// 리스트박스에 추가한다
					::g_pFV_Select->GetListBox ()->AddString ((*iv).c_str ());
					/// 현재 선택된 VFsName이 있고, VFS인스턴스가 있으면
					if(pVFS && VfsName.GetLength () > 0)
					{ 
						short shVersion;
						shVersion = ::g_pDlg_Main->GetVersion ();
						if(shVersion > 0)
						{ pVFS->AddFile (VfsName.GetString (), (*iv).c_str (), (*iv).c_str (), shVersion, 0, 0, true); }
						else
						{ MessageBox ("버젼정보가 올바르지 않습니다."); }
					}
				}
			}
		}
		else
		{
			MessageBox ("기준디렉토리를 선택하지 않았거나 범위를 벗어나는 디렉토리입니다");
		}
	}
}
