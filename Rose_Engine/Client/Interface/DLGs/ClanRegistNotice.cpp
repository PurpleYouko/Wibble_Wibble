#include "stdafx.h"
#include ".\clanregistnotice.h"
#include "../../Network/CNetwork.h"

#include "TEditBox.h"

CClanRegistNotice::CClanRegistNotice(void)
{
}

CClanRegistNotice::~CClanRegistNotice(void)
{
}

void CClanRegistNotice::Show()
{
	CWinCtrl * pCtrl = NULL;
	if(pCtrl = Find(IID_EDIT_NOTICE_CONTENT))
	{
		pCtrl->SetFocus(true);
	}
	CTDialog::Show();
}
unsigned CClanRegistNotice::Process( unsigned uiMsg, WPARAM wParam, LPARAM lParam )
{
	if( !IsVision() ) return 0;

	if( unsigned uiProcID = CTDialog::Process( uiMsg, wParam, lParam ) )
	{
		if( uiMsg == WM_LBUTTONUP )
		{
			switch( uiProcID )
			{
			case IID_BTN_CONFIRM:
				{
					CWinCtrl* pCtrl = Find( IID_EDIT_NOTICE_CONTENT	);
					if( pCtrl && pCtrl->GetControlType() == CTRL_EDITBOX )
					{
						CTEditBox* pEditBox = (CTEditBox*)pCtrl;
						if( char* pszMsg = pEditBox->get_text() )
						{
							g_pNet->Send_cli_CLAN_COMMAND(  GCMD_MOTD, pszMsg );
							Hide();
						}
					}
				}
				break;
			case IID_BTN_CLOSE:
			case IID_BTN_CANCEL:
				Hide();
				break;
			default:
				break;
			}
		}
		return uiMsg;
	}
	return 0;
}

void CClanRegistNotice::SetInterfacePos_After()
{
	CWinCtrl * pCtrl = NULL;

	//문자열 추가.
	if( pCtrl = Find( IID_BTN_CONFIRM ) )
	{
		//858 확인
		pCtrl->SetText( LIST_STRING(858) );
	}
	if( pCtrl = Find( IID_BTN_CLOSE ) )
	{
		//859 취소
		pCtrl->SetText( LIST_STRING(859) );
	}
	//공지등록 879
	GetCaption()->SetString( LIST_STRING(879) );
}