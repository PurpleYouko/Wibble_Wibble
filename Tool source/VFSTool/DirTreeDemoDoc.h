// DirTreeDemoDoc.h : interface of the CDirTreeDemoDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_DIRTREEDEMODOC_H__D26026FE_73F5_47F7_A444_100CD6CD4DE6__INCLUDED_)
#define AFX_DIRTREEDEMODOC_H__D26026FE_73F5_47F7_A444_100CD6CD4DE6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CDirTreeDemoDoc : public CDocument
{
protected: // create from serialization only
	CDirTreeDemoDoc();
	DECLARE_DYNCREATE(CDirTreeDemoDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDirTreeDemoDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CDirTreeDemoDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CDirTreeDemoDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIRTREEDEMODOC_H__D26026FE_73F5_47F7_A444_100CD6CD4DE6__INCLUDED_)
