// FV_Files.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
//#include "VFSTool.h"
#include "FV_Files.h"
#include "FV_Select.h"
#include "__Global.h"
#include <string>
#include "TriggerVFS.h"
#include "Dlg_Main.h"

// CFV_Files

IMPLEMENT_DYNCREATE(CFV_Files, CFormView)


CFV_Files * g_pFV_Files = NULL;


CFV_Files::CFV_Files()
	: CFormView(CFV_Files::IDD)	, m_bInit(false)
	, m_strCurPath(_T(""))
{
	g_pFV_Files = this;
}


CFV_Files::~CFV_Files()
{
	g_pFV_Files = NULL;
}

void CFV_Files::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDCLIST, m_list);
}

BEGIN_MESSAGE_MAP(CFV_Files, CFormView)
	ON_WM_SIZE()
	ON_COMMAND(ID_FILELIST_ADD, OnMenu_AddFiles)
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_FILELIST_SELECTALL, OnMenu_SelectAll)
END_MESSAGE_MAP()


// CFV_Files �����Դϴ�.

#ifdef _DEBUG
void CFV_Files::AssertValid() const
{
	CFormView::AssertValid();
}

void CFV_Files::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG


// CFV_Files �޽��� ó�����Դϴ�.

void CFV_Files::OnSize(UINT nType, int cx, int cy)
{
	CFormView::OnSize(nType, cx, cy);
	
	RECT rect;
	GetClientRect (&rect);

	if(m_bInit) { m_list.MoveWindow (10, 32, rect.right - 20, rect.bottom - 52); }
}

void CFV_Files::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	m_bInit = true;
}


CListBox * CFV_Files::GetListBox(void)
{
	return &m_list;
}


/**
 * ���丮�� �ִ� ��� ������ ����Ʈ�ڽ��� ���� �ش�
 * @param path ���丮
 */
bool CFV_Files::DisplayFiles (CString path)
{
	/// ��θ��� ���̰� 0�� �ƴϰ�, ���� ���丮�� ���� �����ϸ�
	if(path.GetLength () > 0 && SetCurrentDirectory (path) && GetListBox ())
	{
		/// ���� ���丮�� m_strCurPath�� ���
		m_strCurPath = path;
		/// ����Ʈ�ڽ��� ���� ������ ��� �����
		GetListBox ()->ResetContent ();

		CFileFind finder;
		BOOL bWorking = finder.FindFile("*.*");
		while (bWorking)
		{
			bWorking = finder.FindNextFile ();
			if(!finder.IsDirectory ())
			{ GetListBox ()->AddString (finder.GetFileName ()); }
		}

		return true;
	}	

	return false;
}

void CFV_Files::OnMenu_AddFiles()
{
	int *			pIndecies	= NULL;		/// ���õ� �ε���
	CString			strText; 
	CListBox *		plistSel	= NULL;
	CVFS_Manager *	pVFS		= NULL;		/// �ν��Ͻ�

	CString VfsName = ::g_pDlg_Main->GetSelectedVfsName ();	/// ���� ���õ� Vfs�̸��� ������
	plistSel = ::g_pFV_Select->GetListBox ();				/// ������ �߰��Ǵ� ����Ʈ�ڽ��� ������
	if(m_list.GetSelCount () > 0 && plistSel && VfsName.GetLength () > 0)
	{
		/// �ε����� �����ϱ� ���ؼ� �޸𸮸� �Ҵ�
		std::string sSelPath = m_strCurPath.GetString ();
		const std::basic_string <char>::size_type npos = -1;
		if(sSelPath.find (::__GetAbsPath ().GetString (), 0) == npos)
		{
			MessageBox ("���ص��丮�� �������� �ʾҰų� ������ ����� ���丮�Դϴ�");
			return;
		}
		if((pIndecies = new int[m_list.GetSelCount ()]))
		{
			m_list.GetSelItems (m_list.GetSelCount (), pIndecies);
			CString prefix = ::__SubString (::__GetAbsPath ().GetLength (), m_strCurPath);
			prefix.TrimLeft ('\\');
			if(prefix.GetLength () > 0 ) { prefix += "\\"; }

			for(short i = 0; i < m_list.GetSelCount (); i++)
			{
				strText = "";
				m_list.GetText (pIndecies[ i ], strText);
				plistSel->AddString (prefix + strText);
				/// ���� ���µ� vfs ������ ������ �ٷ� �߰��Ѵ�
				if((pVFS = ::g_pDlg_Main->GetVFS ()) && ::__GetAbsPath ().GetLength () > 0)
				{
					SetCurrentDirectory (__GetAbsPath ());
					CString strAddFile = prefix + strText;
					short shVersion = 0;
					pVFS->AddFile (VfsName.GetString (), strAddFile.GetString (), strAddFile.GetString (), shVersion, 0, 0);
				}
			}
			delete pIndecies;
		}
	}
}


void CFV_Files::OnContextMenu(CWnd* /*pWnd*/, CPoint point)
{
		CMenu menu , *menuSub ;
	//int iSelected ;

	//if( (iSelected = m_listEvent.GetCurSel())==LB_ERR) return ;
	menu.LoadMenu( IDR_MENU1 ) ;

	menuSub = menu.GetSubMenu(1);
	
	if(menuSub && IsSelected () ) 
	{
		//menuSub->EnableMenuItem(IDMENU_EL_COPY,MF_DISABLED | MF_GRAYED ) ;
		//menuSub->EnableMenuItem(IDMENU_EL_CUT,MF_DISABLED | MF_GRAYED ) ;
		//menuSub->EnableMenuItem(IDMENU_EL_DELETE,MF_DISABLED | MF_GRAYED ) ;
		//menuSub->EnableMenuItem(IDMENU_EL_EDIT,MF_DISABLED | MF_GRAYED ) ;

		menuSub->TrackPopupMenu(TPM_LEFTALIGN |TPM_RIGHTBUTTON, point.x, point.y, this);
	}
	
}


bool CFV_Files::IsSelected(void)
{
	return (m_list.GetSelCount () > 0);
}


void CFV_Files::OnMenu_SelectAll()
{
	for(short i = 0; i < m_list.GetCount (); i++)
	{ m_list.SetSel (i); }
}
