#pragma once
#include "afxwin.h"
#include "resource.h"

// CDlg_InuptBox_Number ��ȭ �����Դϴ�.

class CDlg_InuptBox_Number : public CDialog
{
	DECLARE_DYNAMIC(CDlg_InuptBox_Number)

public:
	CDlg_InuptBox_Number(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CDlg_InuptBox_Number();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

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
