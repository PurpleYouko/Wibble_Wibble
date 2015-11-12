// DirTreeDemo.h : main header file for the DIRTREEDEMO application
//

#if !defined(AFX_DIRTREEDEMO_H__E4651906_8C71_4C08_B3FA_28E2E11393A9__INCLUDED_)
#define AFX_DIRTREEDEMO_H__E4651906_8C71_4C08_B3FA_28E2E11393A9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CDirTreeDemoApp:
// See DirTreeDemo.cpp for the implementation of this class
//

class CDirTreeDemoApp : public CWinApp
{
public:
	CDirTreeDemoApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDirTreeDemoApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CDirTreeDemoApp)
	afx_msg void OnAppAbout();
	afx_msg void OnTreedlg();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIRTREEDEMO_H__E4651906_8C71_4C08_B3FA_28E2E11393A9__INCLUDED_)
