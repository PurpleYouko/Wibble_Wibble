#include "stdafx.h"
#include ".\cmemodlg.h"
#include "../../Game.h"
#include "../../Network/CNetwork.h"

#include "TEditBox.h"
#include "TButton.h"
#include "TImage.h"

CMemoDlg::CMemoDlg( int iDlgType )
{
	SetDialogType( iDlgType );
}

CMemoDlg::~CMemoDlg(void)
{

}

bool CMemoDlg::Create( const char* IDD )
{
	if( CTDialog::Create( IDD ) )
	{
		return true;
	}
	return false;
}


void CMemoDlg::Show()
{

#ifdef _DEBUG
	
	POINT pt = { GetPosition().x, GetPosition().y };
	Clear();
	Create("DlgMemo");		
	SetInterfacePos_After();
	MoveWindow(pt);

#endif

	CTDialog::Show();
}


void CMemoDlg::Draw()
{
	if( !IsVision() ) return;
	CTDialog::Draw();

	D3DXMATRIX mat;	
	D3DXMatrixTranslation( &mat, (float)m_sPosition.x, (float)m_sPosition.y,0.0f);
	::setTransformSprite( mat );


	GetCaption()->SetString( CStr::Printf("%s", m_strName.c_str() ) );
}

unsigned CMemoDlg::Process( unsigned uiMsg, WPARAM wParam, LPARAM lParam )
{
	if( !IsVision() ) return 0;
	if( unsigned uiProcID = CTDialog::Process( uiMsg, wParam, lParam ))
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
				case IID_BTN_SEND:
					SendMemo();
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

void CMemoDlg::SendMemo()
{
	CWinCtrl* pCtrl = Find( IID_EDITBOX );
	assert( pCtrl );
	if( pCtrl == NULL ) return;

	assert( pCtrl->GetControlType() == CTRL_EDITBOX );
	if( pCtrl->GetControlType() != CTRL_EDITBOX ) return;

	CTEditBox* pEditBox = (CTEditBox*)pCtrl;
	char* pszMemo = pEditBox->get_text();
	if( pszMemo == NULL ) return;
	if( strlen( pszMemo ) <= 0 ) return;

	g_pNet->Send_cli_MEMO( (char*)m_strName.c_str(), pszMemo );
}

void CMemoDlg::SetTargetName( const char* pszName )
{
	assert( pszName );
	if( pszName )
		m_strName = pszName;
}


void CMemoDlg::SetInterfacePos_After()
{
	CWinCtrl * pCtrl = NULL;
	
	//문자열 추가.
	if( pCtrl = Find( IID_BTN_SEND ) )
	{
		//230 보내기
		((CTButton*)pCtrl)->SetText( LIST_STRING(230) );
	}


}