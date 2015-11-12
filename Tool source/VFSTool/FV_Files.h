#pragma once

#include <afxwin.h>
#include "resource.h"

// CFV_Files �� ���Դϴ�.

class CFV_Files : public CFormView
{
	DECLARE_DYNCREATE(CFV_Files)

protected:
	CFV_Files();           // ���� ����⿡ ���Ǵ� protected �������Դϴ�.
	virtual ~CFV_Files();


public:
	enum { IDD = IDDDLG_FILES };
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()


private:
	bool m_bInit;


protected:
	CListBox m_list;


public:
	CListBox *		GetListBox (void);
	afx_msg void	OnSize(UINT nType, int cx, int cy);
	virtual void	OnInitialUpdate();

	bool DisplayFiles(CString path);
private:
	/// ����Ʈ�� ���ϵ��� ���丮
	CString m_strCurPath;		
public:
	afx_msg void OnMenu_AddFiles();
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
	bool IsSelected(void);
	afx_msg void OnMenu_SelectAll();
};


extern CFV_Files * g_pFV_Files ;

