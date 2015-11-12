// TreeDialog.cpp : implementation file
//

#include "stdafx.h"
#include "DirTreeDemo.h"
#include "TreeDialog.h"
#include <direct.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTreeDialog dialog


CTreeDialog::CTreeDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CTreeDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTreeDialog)
	m_strPath = _T("");
	m_strSelection = _T("");
	//}}AFX_DATA_INIT
}


void CTreeDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTreeDialog)
	DDX_Text(pDX, IDC_PATHNAME, m_strPath);
	DDX_Text(pDX, IDC_SELECTION, m_strSelection);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTreeDialog, CDialog)
	//{{AFX_MSG_MAP(CTreeDialog)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE1, OnSelchangedTree1)
	ON_BN_CLICKED(IDC_SETPATH, OnSetpath)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTreeDialog message handlers

BOOL CTreeDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	TCHAR  szWorkDir[MAX_PATH];

	
	// TODO: Add extra initialization here
	// Here we subclass our CDirTreeCtrl
	if ( !m_TreeCtrl.m_hWnd )
	{
		if ( m_TreeCtrl.SubclassDlgItem( IDC_TREE1, this ) )
		{			
			m_TreeCtrl.DisplayTree( NULL /*Display all*/, TRUE /* TRUE = Display Files*/ );
			
			_getcwd( szWorkDir, 256 );
			// set the Path to the current Work-Directory
			m_TreeCtrl.SetSelPath( szWorkDir );
		}
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CTreeDialog::OnSelchangedTree1(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	m_strPath = m_TreeCtrl.GetFullPath( pNMTreeView->itemNew.hItem );
	UpdateData( FALSE );
	*pResult = 0;
}

void CTreeDialog::OnSetpath() 
{
	// TODO: Add your control notification handler code here
	UpdateData();
	if ( !m_TreeCtrl.SetSelPath( m_strSelection ) )
	{
		MessageBox( _T("Whoops, this selection could not be set!"),
			          _T("Set selection failed") );
	}
}
