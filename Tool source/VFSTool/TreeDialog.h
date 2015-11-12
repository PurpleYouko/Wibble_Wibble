#if !defined(AFX_TREEDIALOG_H__16ADF089_B702_4210_9239_B209E8774E1C__INCLUDED_)
#define AFX_TREEDIALOG_H__16ADF089_B702_4210_9239_B209E8774E1C__INCLUDED_

#include "DirTreeCtrl.h"	// Added by ClassView
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TreeDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTreeDialog dialog

class CTreeDialog : public CDialog
{
// Construction
public:
	CTreeDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CTreeDialog)
	enum { IDD = IDD_TREEDLG };
	CString	m_strPath;
	CString	m_strSelection;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTreeDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CDirTreeCtrl m_TreeCtrl;

	// Generated message map functions
	//{{AFX_MSG(CTreeDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangedTree1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSetpath();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TREEDIALOG_H__16ADF089_B702_4210_9239_B209E8774E1C__INCLUDED_)
