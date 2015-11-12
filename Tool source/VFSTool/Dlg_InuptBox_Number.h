#pragma once
#include "afxwin.h"
#include "resource.h"

// CDlg_InuptBox_Number 대화 상자입니다.

class CDlg_InuptBox_Number : public CDialog
{
	DECLARE_DYNAMIC(CDlg_InuptBox_Number)

public:
	CDlg_InuptBox_Number(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlg_InuptBox_Number();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
private:
	CEdit m_editNumber;
public:
	int GetNumber(void);
protected:
	virtual void OnOK();
private:
	int m_iNumber;
};
