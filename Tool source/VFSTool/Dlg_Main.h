#pragma once

#include "resource.h"
#include "afxwin.h"
//#include "TriggerVFS.h"
#include "VFS_Manager.h"

// CDlg_Main 폼 뷰입니다.

class CDlg_Main : public CFormView
{
	DECLARE_DYNCREATE(CDlg_Main)

protected:
	CDlg_Main();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~CDlg_Main();

public:
	enum { IDD = IDDDLG_MAIN };
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
private:
	CStatic m_stAbsPath;
public:
	void SetAbsPath(CString path);
	CString GetAbsPath(void);
	void OnMenu_SetAbsPath(void);
private:
	CStatic m_FileName;
	CEdit m_editExcPrefix;
	CEdit m_editExcExt;
public:
	CString GetExcPrefix(void);
	CString GetExcExt(void);
	afx_msg void OnBnClick_Savefile();
	afx_msg void OnBnClick_OpenFile();
	void SetFileName(CString);
	CString GetFileName(void);
	CVFS_Manager * GetVFS(void);
	afx_msg void OnBnClickedOpen2();
private:
	CStatic m_staticFileNum;
public:
	void SetFileNum(DWORD dwNum);
	afx_msg void OnBnClick_ClearBlank();
private:
	CListBox m_listVFS;
public:
	afx_msg void OnlistVFS_SelChange();
	afx_msg void OnBnClick_AddVFS();
private:
	CString m_strWorkingDir;
public:
	CString GetSelectedVfsName(void);
	CString GetSelVfsName(void);
private:
	CString m_strCurVfsName;
public:
	CString GetWorkDir(void);
private:
	CEdit m_editVersion;
public:
	short GetVersion(void);
};

extern CDlg_Main * g_pDlg_Main ;
