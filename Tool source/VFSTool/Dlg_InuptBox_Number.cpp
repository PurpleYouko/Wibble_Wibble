// Dlg_InuptBox_Number.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
//#include "VFSTool.h"
#include "Dlg_InuptBox_Number.h"


// CDlg_InuptBox_Number ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CDlg_InuptBox_Number, CDialog)
CDlg_InuptBox_Number::CDlg_InuptBox_Number(CWnd* pParent /*=NULL*/)
	: CDialog(CDlg_InuptBox_Number::IDD, pParent)
	, m_iNumber(0)
{
}

CDlg_InuptBox_Number::~CDlg_InuptBox_Number()
{
}

void CDlg_InuptBox_Number::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDCEDIT_NUMBER, m_editNumber);
}


BEGIN_MESSAGE_MAP(CDlg_InuptBox_Number, CDialog)
END_MESSAGE_MAP()


// CDlg_InuptBox_Number �޽��� ó�����Դϴ�.

/**
 * �Էµ� ���ڸ� ����
 */
int CDlg_InuptBox_Number::GetNumber(void)
{ 
	return m_iNumber;
}


/**
 * Okay�� ������ ���� �Է¹ڽ��κ��� ���ڸ� �����Ѵ�
 */
void CDlg_InuptBox_Number::OnOK()
{
	m_iNumber = 0;

	CString strNumber;

	m_editNumber.GetWindowText (strNumber);
	if(strNumber.GetLength () > 0) 
	{ 
		m_iNumber = atoi (strNumber.GetString ()); 
	}

	CDialog::OnOK();
}
