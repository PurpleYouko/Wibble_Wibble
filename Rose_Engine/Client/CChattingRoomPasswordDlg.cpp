#include "stdafx.h"
#include "CChattingRoomPasswordDlg.h"
#include "GameData/CChatRoom.h"
#include "TEditBox.h"
#include "it_mgr.h"
#include "Network/CNetwork.h"
#include "TCaption.h"
#include "TImage.h"
#include "TButton.h"


CChattingRoomPasswordDlg::CChattingRoomPasswordDlg( int iDlgType )
{
	SetDialogType( iDlgType );
}

CChattingRoomPasswordDlg::~CChattingRoomPasswordDlg(void)
{

}

void CChattingRoomPasswordDlg::SetID( WORD wId )
{
	m_wID = wId;
}
void CChattingRoomPasswordDlg::Show()
{
	g_itMGR.AppendDlg(0, this, 0);
	POINT pt = { GetPosition().x, GetPosition().y };
	Clear();
	Create("DlgChatRoomPW");
	SetInterfacePos_After();
	MoveWindow(pt);
	CTDialog::Show();
}

void CChattingRoomPasswordDlg::Hide()
{
	g_itMGR.DeleteDlg(this);
}

void CChattingRoomPasswordDlg::SetInterfacePos_After()
{
	CTDialog::SetInterfacePos_After();

	CWinCtrl * pCtrl = NULL;

	//문자열 추가.


	//비밀번호754
	GetCaption()->SetString( LIST_STRING(754) );

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

unsigned CChattingRoomPasswordDlg::Process( unsigned uiMsg, WPARAM wParam, LPARAM lParam )
{
	if( !IsVision() ) return 0;
	if( unsigned uiProcID = CTDialog::Process( uiMsg, wParam, lParam ) )
	{
		switch( uiMsg )
		{
		case WM_LBUTTONUP:
			{
				switch( uiProcID )
				{
				case IID_BTN_CLOSE:
					Hide();
					break;
				case IID_BTN_CONFIRM:
					if( SendReqJoinRoom() )
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

bool CChattingRoomPasswordDlg::SendReqJoinRoom()
{
	CWinCtrl* pCtrl = Find( IID_EDITBOX );
	assert( pCtrl );
	if( pCtrl == NULL ) return false;

	assert( pCtrl->GetControlType() == CTRL_EDITBOX );
	if(pCtrl->GetControlType() != CTRL_EDITBOX) return false;

	CTEditBox* pEditBox = (CTEditBox*)pCtrl;

	char* pszPass = pEditBox->get_text();
	if( pszPass && strlen(pszPass) > 0 )
	{	
		CChatRoom::GetInstance().SendReqJoinRoom( 0, m_wID, pszPass );
		return true;		
	}
	return false;
}