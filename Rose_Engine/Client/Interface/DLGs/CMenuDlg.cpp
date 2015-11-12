#include "stdafx.h"
#include ".\cmenudlg.h"
#include "../it_mgr.h"
#include "../CDragNDropMgr.h"
#include "../../System/CGame.h"
#include "CDialogDlg.h"
#include "../icon/cicondialog.h"
#include "../command/uicommand.h"
#include "../CDragItem.h"
#include "CMinimapDlg.h"
#include "../../Object.h"
#include "../../GameData/CParty.h"
#include "../../System/System_FUNC.h"
#include "../CToolTipMgr.h"

int g_iMenuMoveSpeed = 10;

CMenuDlg::CMenuDlg(void)
{
	m_uiClickedButtonID = 0;
	m_iOpenType = _STOP;
}

CMenuDlg::~CMenuDlg(void)
{
}

unsigned CMenuDlg::Process( unsigned uiMsg, WPARAM wParam, LPARAM lParam )
{
	if( unsigned uiProcID = CTDialog::Process( uiMsg, wParam, lParam ) )
	{
		switch( uiMsg )
		{
		case WM_LBUTTONUP:
			OnLButtonUp( uiProcID, wParam, lParam );
			break;
		case WM_LBUTTONDOWN:
			OnLButtonDown( uiProcID, wParam, lParam );
			break;			
		default:
			{				
			}
			break;
		}
		return uiProcID;
	}
	return 0;
}
void CMenuDlg::OnLButtonUp( unsigned uiProcID, WPARAM wParam, LPARAM lParam )
{
	m_uiClickedButtonID = 0;

	// Name Process.
	CWinCtrl * ctrl = Find( uiProcID );
	const char * szCtrlName = NULL;
	if( ctrl )
	{
		szCtrlName = ctrl->GetControlName();
		if( strlen(szCtrlName) )			
		{
			if( strcmp( szCtrlName, "OPEN_BTN") == 0 )
			{
				SetOpen();
				return;
			}
			else if( strcmp( szCtrlName, "CLOSE_BTN") == 0 )
			{
				SetClose();
				return;
			}			
		}			
	}
			


	switch( uiProcID )
	{
	case IID_BTN_CHARACTER:
		g_itMGR.OpenDialog( DLG_TYPE_CHAR );	
		break;
	case IID_BTN_ITEM:
		g_itMGR.OpenDialog( DLG_TYPE_ITEM );
		break;
	case IID_BTN_SKILL:
		g_itMGR.OpenDialog( DLG_TYPE_SKILL );
		break;
	case IID_BTN_QUEST:
		g_itMGR.OpenDialog( DLG_TYPE_QUEST );
		break;
	case IID_BTN_CUMMUNITY:
		g_itMGR.OpenDialog( DLG_TYPE_COMMUNITY );
		break;
	case IID_BTN_GUILD:
		g_itMGR.OpenDialog( DLG_TYPE_CLAN );
		break;
	case IID_BTN_EXIT:
		g_itMGR.OpenDialog( DLG_TYPE_SYSTEM );
		break;
	default:
		break;
	}
}
void CMenuDlg::OnLButtonDown( unsigned uiProcID, WPARAM wParam, LPARAM lParam )
{
	switch( uiProcID )
	{
	case IID_BTN_CHARACTER:
	case IID_BTN_ITEM:
	case IID_BTN_SKILL:
	case IID_BTN_QUEST:
	case IID_BTN_CUMMUNITY:	
	case IID_BTN_GUILD:
	case IID_BTN_EXIT:
		m_uiClickedButtonID = uiProcID;
		m_ptClickedButton.x = LOWORD( lParam );
		m_ptClickedButton.y = HIWORD( lParam );
		break;
	default:
		break;
	}
}

void CMenuDlg::Update( POINT ptMouse )
{
	if( !IsVision() ) return;
	CTDialog::Update( ptMouse );
	if( m_uiClickedButtonID )
	{
		///일정이상 드래그상태일경우
		if( abs(m_ptClickedButton.x - ptMouse.x ) >= g_pCApp->GetWIDTH() / 100 ||
			abs(m_ptClickedButton.y - ptMouse.y ) >= g_pCApp->GetHEIGHT() / 100 )
		{
			int iDialogType = 0;
			switch( m_uiClickedButtonID )
			{
			case IID_BTN_GUILD:
				iDialogType = DLG_TYPE_CLAN;
				break;
			case IID_BTN_CHARACTER:
				iDialogType = DLG_TYPE_CHAR;
				break;
			case IID_BTN_ITEM:
				iDialogType = DLG_TYPE_ITEM;
				break;
			case IID_BTN_SKILL:
				iDialogType = DLG_TYPE_SKILL;
				break;
			case IID_BTN_QUEST:
				iDialogType = DLG_TYPE_QUEST;
				break;
			case IID_BTN_CUMMUNITY:
				iDialogType = DLG_TYPE_COMMUNITY;
				break;	
			default:
				break;
			}
			
			if( iDialogType )
			{
				///다이얼로그 아이콘은 한개씩만( 바탕화면에 )
				if( g_itMGR.FindDialogIcon( iDialogType ) == NULL )
				{
					CIconDialog* pIcon = CIconDialog::CreateIcon( iDialogType , false );

					CDragItem* pDragItem = new CDragItem;
					CTCommand* pCmd = new CTCmdMoveDialogIcon2GroundFromMenu;
					pDragItem->AddTarget( CDragItem::TARGET_GROUND, pCmd );
					pDragItem->SetIcon( pIcon );
					pIcon->SetDragItem( pDragItem );

					g_itMGR.SetDialogIconFromMenu( pIcon );

					CDragNDropMgr::GetInstance().DragStart( pDragItem );
				}
				m_uiClickedButtonID = 0;
			}
		}
	}	


	CWinCtrl * pCtrl = NULL;

	//문자열 추가.
	//std::string strBuf[] = { "케릭터 정보", "인벤토리", "스킬", "퀘스트", "커뮤니티", "클랜", "시스템", };
	std::string strBufShortCut[] = { "Alt+A", "Alt+I", "Alt+S", "Alt+Q", "Alt+C", "Alt+N", "Alt+X", };

	for(int i=IID_BTN_CHARACTER; i<IID_BTN_SIZE; i++)
	{
		if( pCtrl = Find(i) )
		{
			if( pCtrl->IsInside( ptMouse.x, ptMouse.y ) )
			{	
				CInfo Info;
				Info.Clear();
				POINT pt = { pCtrl->GetPosition().x - 20, pCtrl->GetPosition().y - 40 };
				Info.SetPosition( pt );
				Info.AddString( LIST_STRING(825+i-IID_BTN_CHARACTER), g_dwWHITE, g_GameDATA.m_hFONT[ FONT_NORMAL ], DT_CENTER );
				Info.AddString( strBufShortCut[i-IID_BTN_CHARACTER].c_str(), g_dwWHITE, g_GameDATA.m_hFONT[ FONT_NORMAL ], DT_CENTER );	
				CToolTipMgr::GetInstance().RegistInfo( Info );				

				break;
			}					
		}
	}	

	ProcOpen();
}

void CMenuDlg::Show()
{
	CTDialog::Show();
	m_uiClickedButtonID = 0;
}

void CMenuDlg::SetInterfacePos_After()
{
	Show();
	RefreshDlg();
	SetOpen();
	
}

void CMenuDlg::RefreshDlg()
{
	MoveWindow( getScreenWidth() - GetWidth(), getScreenHeight() - GetHeight() );
	SetOpenPos( GetPosition() );

	CWinCtrl * ctrl = Find( "OPEN_BTN" );
	POINT pt = { 0, 0 };
	if( ctrl )
	{
		pt.x = getScreenWidth() - ctrl->GetWidth();
		pt.y = getScreenHeight() - ctrl->GetHeight();
		ctrl->MoveWindow( pt );
		ctrl->SetOffset( 0, 0);
	}
	ctrl = Find( "CLOSE_BTN" );
	if( ctrl )
	{
		pt.x = getScreenWidth() - ctrl->GetWidth();
		pt.y = getScreenHeight() - ctrl->GetHeight();
		ctrl->MoveWindow( pt );
		ctrl->SetOffset( 0, 0);
	}
}

void CMenuDlg::SetOpen(bool b)
{
	m_bIsOpen = b;

	CWinCtrl * ctrl = NULL;
	ctrl = Find( "CLOSE_BTN" );
	if( ctrl )
	{
		if( b )	{	ctrl->Show();	}
		else	{	ctrl->Hide();	}
		
	}
	ctrl = Find( "OPEN_BTN" );
	if( ctrl )
	{
		if( b )	{	ctrl->Hide();	}
		else	{	ctrl->Show();	}		
	}

	if( b )
	{
		SetOpenType( _OPEN );
	}
	else
	{
		SetOpenType( _CLOSE );
	}
}

void CMenuDlg::SetClose()
{
	SetOpen( false );
}

bool CMenuDlg::GetIsOpen()
{
	return m_bIsOpen;
}

void CMenuDlg::SetOpenPos( POINT & pt )
{
	m_ptOpenPos = pt;
}
POINT & CMenuDlg::GetOpenPos()
{
	return m_ptOpenPos;
}

void CMenuDlg::SetOpenType( int iType )
{
	m_iOpenType = iType;
}

int CMenuDlg::GetOpenType()
{
	return m_iOpenType;
}

void CMenuDlg::ProcOpen()
{
	switch( GetOpenType() )
	{
	case _STOP:
		break;

	case _OPEN:
		{
			// 씽크 구차나서 안마춤..^^
			int iPosX = GetPosition().x - g_iMenuMoveSpeed;			

			if( iPosX <= ( GetOpenPos().x ) )
			{
				SetOpenType( _STOP );
				iPosX = GetOpenPos().x;
			}
			MoveWindow( iPosX, GetPosition().y );

			CWinCtrl * ctrl = Find( "OPEN_BTN" );
			POINT pt = { 0, 0 };
			if( ctrl )
			{
				pt.x = getScreenWidth() - ctrl->GetWidth();
				pt.y = getScreenHeight() - ctrl->GetHeight();
				ctrl->MoveWindow( pt );
			}
			ctrl = Find( "CLOSE_BTN" );
			if( ctrl )
			{
				pt.x = getScreenWidth() - ctrl->GetWidth();
				pt.y = getScreenHeight() - ctrl->GetHeight();
				ctrl->MoveWindow( pt );
			}
		}
		break;

	case _CLOSE:
		{
			int iPosX = GetPosition().x + g_iMenuMoveSpeed;

			CWinCtrl * ctrl = Find( "OPEN_BTN" );

			if( ctrl )
			{
				iPosX += ctrl->GetWidth();
			}

            if( iPosX > ( getScreenWidth() ) )
			{
                SetOpenType( _STOP );
			}

			if( ctrl )
			{
				iPosX -= ctrl->GetWidth();
			}

			MoveWindow( iPosX, GetPosition().y );	

			ctrl = Find( "OPEN_BTN" );
			POINT pt = { 0, 0 };
			if( ctrl )
			{
				pt.x = getScreenWidth() - ctrl->GetWidth();
				pt.y = getScreenHeight() - ctrl->GetHeight();
				ctrl->MoveWindow( pt );
			}
			ctrl = Find( "CLOSE_BTN" );
			if( ctrl )
			{
				pt.x = getScreenWidth() - ctrl->GetWidth();
				pt.y = getScreenHeight() - ctrl->GetHeight();
				ctrl->MoveWindow( pt );
			}
		}
		break;
	}
}
