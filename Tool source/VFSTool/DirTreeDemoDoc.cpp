// DirTreeDemoDoc.cpp : implementation of the CDirTreeDemoDoc class
//

#include "stdafx.h"
#include "DirTreeDemo.h"

#include "DirTreeDemoDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDirTreeDemoDoc

IMPLEMENT_DYNCREATE(CDirTreeDemoDoc, CDocument)

BEGIN_MESSAGE_MAP(CDirTreeDemoDoc, CDocument)
	//{{AFX_MSG_MAP(CDirTreeDemoDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDirTreeDemoDoc construction/destruction

CDirTreeDemoDoc::CDirTreeDemoDoc()
{
	// TODO: add one-time construction code here

}

CDirTreeDemoDoc::~CDirTreeDemoDoc()
{
}

BOOL CDirTreeDemoDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CDirTreeDemoDoc serialization

void CDirTreeDemoDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CDirTreeDemoDoc diagnostics

#ifdef _DEBUG
void CDirTreeDemoDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CDirTreeDemoDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CDirTreeDemoDoc commands
