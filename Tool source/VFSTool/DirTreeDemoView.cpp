// DirTreeDemoView.cpp : implementation of the CDirTreeDemoView class
//

#include "stdafx.h"
#include "DirTreeDemo.h"

#include "DirTreeDemoDoc.h"
#include "DirTreeDemoView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDirTreeDemoView

IMPLEMENT_DYNCREATE(CDirTreeDemoView, CListView)

BEGIN_MESSAGE_MAP(CDirTreeDemoView, CListView)
	//{{AFX_MSG_MAP(CDirTreeDemoView)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CListView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CListView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CListView::OnFilePrintPreview)
END_MESSAGE_MAP()


CDirTreeDemoView * g_pView_FileList = NULL;


/////////////////////////////////////////////////////////////////////////////
// CDirTreeDemoView construction/destruction

CDirTreeDemoView::CDirTreeDemoView()
{
	// TODO: add construction code here
	g_pView_FileList = this;
}

CDirTreeDemoView::~CDirTreeDemoView()
{
}

BOOL CDirTreeDemoView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CListView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CDirTreeDemoView drawing

void CDirTreeDemoView::OnDraw(CDC* pDC)
{
	CDirTreeDemoDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	CListCtrl& refCtrl = GetListCtrl();
	refCtrl.InsertItem(0, "Item!");
	// TODO: add draw code for native data here
}

void CDirTreeDemoView::OnInitialUpdate()
{
	CListView::OnInitialUpdate();

	LV_COLUMN lvCol;

	lvCol.mask		= LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
	lvCol.fmt		= LVCFMT_LEFT;
	lvCol.pszText  	= "파일명";
	lvCol.iSubItem 	= 0;
	lvCol.cx       	= 200;

	GetListCtrl ().InsertColumn (0, &lvCol);

	 //TODO: You may populate your ListView with items by directly accessing
	 // its list control through a call to GetListCtrl().
}

/////////////////////////////////////////////////////////////////////////////
// CDirTreeDemoView printing

BOOL CDirTreeDemoView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CDirTreeDemoView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CDirTreeDemoView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CDirTreeDemoView diagnostics

#ifdef _DEBUG
void CDirTreeDemoView::AssertValid() const
{
	CListView::AssertValid();
}

void CDirTreeDemoView::Dump(CDumpContext& dc) const
{
	CListView::Dump(dc);
}

CDirTreeDemoDoc* CDirTreeDemoView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CDirTreeDemoDoc)));
	return (CDirTreeDemoDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CDirTreeDemoView message handlers
void CDirTreeDemoView::OnStyleChanged(int nStyleType, LPSTYLESTRUCT lpStyleStruct)
{
	//TODO: add code to react to the user changing the view style of your window
}

bool CDirTreeDemoView::DisplayFiles (CString path)
{
	CFileFind finder;

	SetCurrentDirectory (path);

	BOOL bWorking = finder.FindFile("*.*");
	int i= 1;
	while (bWorking)
	{
		bWorking = finder.FindNextFile();
		// cout << (LPCTSTR) finder.GetFileName() << endl;

		//item.mask		= LVIF_TEXT | LVIF_INDENT ;
		CString strPath = finder.GetFilePath ();
		//item.pszText	= "a"; //(LPSTR)strPath.GetString ();
		//item.iItem		= i++;
		//item.iSubItem	= 0;
		//item.iIndent	= 0;

		GetListCtrl ().InsertItem (i++, (LPCTSTR)strPath.GetString ());
	}

	return false;
}

BOOL CDirTreeDemoView::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	
	return CListView::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
}
