// DirTreeDemoView.h : interface of the CDirTreeDemoView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_DIRTREEDEMOVIEW_H__65CD355B_11EA_4923_89EB_DD982B1FBE22__INCLUDED_)
#define AFX_DIRTREEDEMOVIEW_H__65CD355B_11EA_4923_89EB_DD982B1FBE22__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DirTreeDemoDoc.h"

class CDirTreeDemoView : public CListView
{
protected: // create from serialization only
	CDirTreeDemoView();
	DECLARE_DYNCREATE(CDirTreeDemoView)

// Attributes
public:
	CDirTreeDemoDoc* GetDocument();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDirTreeDemoView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual void OnInitialUpdate(); // called first time after construct
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CDirTreeDemoView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CDirTreeDemoView)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	afx_msg void OnStyleChanged(int nStyleType, LPSTYLESTRUCT lpStyleStruct);
	DECLARE_MESSAGE_MAP()
public:
	bool DisplayFiles (CString);
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
};

#ifndef _DEBUG  // debug version in DirTreeDemoView.cpp
inline CDirTreeDemoDoc* CDirTreeDemoView::GetDocument()
   { return (CDirTreeDemoDoc*)m_pDocument; }
#endif

/// 디렉토리뷰에서 항목 클릭하면 파일리스트를 보여주는 클래스
extern CDirTreeDemoView * g_pView_FileList ;



/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIRTREEDEMOVIEW_H__65CD355B_11EA_4923_89EB_DD982B1FBE22__INCLUDED_)
