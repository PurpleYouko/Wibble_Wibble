#pragma once

#include <afxwin.h>
#include "resource.h"

// CFV_Files 폼 뷰입니다.

class CFV_Files : public CFormView
{
	DECLARE_DYNCREATE(CFV_Files)

protected:
	CFV_Files();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~CFV_Files();


public:
	enum { IDD = IDDDLG_FILES };
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

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
	/// 리스트된 파일들의 디렉토리
	CString m_strCurPath;		
public:
	afx_msg void OnMenu_AddFiles();
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
	bool IsSelected(void);
	afx_msg void OnMenu_SelectAll();
};


extern CFV_Files * g_pFV_Files ;

