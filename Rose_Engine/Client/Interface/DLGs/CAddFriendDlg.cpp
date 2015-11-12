#include "stdafx.h"
#include ".\caddfrienddlg.h"
#include "TEditBox.h"
#include "CCommDlg.h"
#include "../it_mgr.h"
#include "../../Network/CNetwork.h"
#include "TCaption.h"
#include "TImage.h"
#include "TButton.h"

CAddFriendDlg::CAddFriendDlg( int iDlgType )
{
	SetDialogType( iDlgType );
}

CAddFriendDlg::~CAddFriendDlg(void)
{

}

void CAddFriendDlg::Show()
{

#ifdef _DEBUG

	POINT pt = { GetPosition().x, GetPosition().y };
	Clear();
	Create("DlgAddFriend");		
	SetInterfacePos_After();
	MoveWindow(pt);

#endif

#ifdef __PRIVATECHAT2
	SetReturnID(IID_BTN_CONFIRM);
#endif

	CTDialog::Show();
}


void CAddFriendDlg::SetInterfacePos_After()
{
	CTDialog::SetInterfacePos_After();

	CWinCtrl * pCtrl = NULL;

	//문자열 추가.

	//친구추가 780
	GetCaption()->SetString( LIST_STRING(780) );

	if( pCtrl = Find("NAME_TXT") )
	{		
		//이름 755
		((CTImage*)pCtrl)->SetText(LIST_STRING(755));
		((CTImage*)pCtrl)->SetTextColor(g_dwWHITE);
		((CTImage*)pCtrl)->SetAlign(DT_CENTER | DT_VCENTER);	
	}
	if( pCtrl = Find(IID_BTN_CONFIRM) )
	{		
		//확인 858
		((CTButton*)pCtrl)->SetText(LIST_STRING(858));		
		pCtrl->SetFont(FONT_NORMAL);
		((CTButton*)pCtrl)->SetTextColor(g_dwWHITE);
	}

}

unsigned CAddFriendDlg::Process( unsigned uiMsg, WPARAM wParam, LPARAM lParam )
{
	if( !IsVision() ) return 0;
	if( unsigned uiProcID = CTDialog::Process( uiMsg, wParam, lParam ) )
	{
		switch( uiMsg )
		{
		case WM_KEYDOWN:
		case WM_LBUTTONUP:
			{
				switch( uiProcID )
				{
				case IID_BTN_CLOSE:
					Hide();
					break;
				case IID_BTN_CONFIRM:
					if( SendReqAddFriend() )
						Hide();
					break;
				default:
					break;
				}
				break;
			}
		default:
			break;
		}
		return uiMsg;
	}
	return 0;
}

bool CAddFriendDlg::SendReqAddFriend()
{
	CWinCtrl* pCtrl = Find( IID_EDITBOX );
	assert( pCtrl );
	if( pCtrl == NULL ) return false;

	assert( pCtrl->GetControlType() == CTRL_EDITBOX );
	if(pCtrl->GetControlType() != CTRL_EDITBOX) return false;

	CTEditBox* pEditBox = (CTEditBox*)pCtrl;

	char* pszName = pEditBox->get_text();
	if( pszName && strlen(pszName) > 0 )
	{
		if( _strcmpi( pszName, g_pAVATAR->Get_NAME() ))///자기 자신은 안된다.
		{
			CTDialog* pDlg = g_itMGR.FindDlg( DLG_TYPE_COMMUNITY );
			assert( pDlg );
			if( pDlg )
			{
				CCommDlg* pCommDlg = (CCommDlg*)pDlg;
				if( pCommDlg->FindFriendByName( pszName ) == NULL )
					g_pNet->Send_cli_MCMD_APPEND_REQ( pszName );
				else
					g_itMGR.OpenMsgBox(STR_DUPLICATED_FRIENDNAME);
			}
			return true;
		}
	}
	return false;
}


void CAddFriendDlg::SetFriendName(const char * szName)
{
	CWinCtrl* pCtrl = Find( IID_EDITBOX );
	assert( pCtrl );
	if( pCtrl == NULL ) return;	

	CTEditBox* pEditBox = (CTEditBox*)pCtrl;
	pEditBox->SetText( szName );
}