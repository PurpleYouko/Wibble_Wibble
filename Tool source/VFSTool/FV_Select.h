#pragma once

#include "resource.h"
#include "afxwin.h"

// CFV_Select 폼 뷰입니다.

class CFV_Select : public CFormView
{
	DECLARE_DYNCREATE(CFV_Select)

protected:
	CFV_Select();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~CFV_Select();

public:
	enum { IDD = IDDDLG_SELECT };
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
private:
	bool m_bInit;
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual void OnInitialUpdate();
protected:
	CListBox m_list;
public:
	CListBox * GetListBox(void);
	afx_msg void OnMenu_RemoveFiles();
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
	bool IsSelected(void);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnMenu_SelectAll();
	afx_msg void OnMenu_Extract();
	afx_msg void OnMenu_Randomselect();
private:
	int m_iSelStart;
	int m_iSelEnd;
public:
	afx_msg void OnMenu_RemoveFilesB();
};

extern CFV_Select * g_pFV_Select;

