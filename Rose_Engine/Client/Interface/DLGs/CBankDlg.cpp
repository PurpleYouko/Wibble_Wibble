#include "stdafx.h"
#include ".\cbankdlg.h"
#include "CBankWindowDlg.h"
#include "../../Network/CNetwork.h"

#include "../../Object.h"
#include "../../System/CGame.h"
#include "../../GameCommon/Item.h"
#include "../../Misc/GameUtil.h"

#include "../IO_ImageRes.h"
#include "../it_mgr.h"
#include "../icon/ciconitem.h"


#include "../CDragItem.h"
#include "../command/ctcmdnumberinput.h"
#include "../command/uicommand.h"

#include "../Country.h"

#include "TRadioBox.h"
#include "TRadioButton.h"
#include "TButton.h"

#include "CNumberInputDlg.h"


void CBankDlg::SetInterfacePos_After()
{
	CWinCtrl * pCtrl = NULL;
	CWinCtrl * pCtrl2 = NULL;
	int GapY =0 , GapX = 0;
	POINT ptOffset;

    if( pCtrl = Find("ITEM_SLOT_00") )
	{
		if( pCtrl2 = Find("ITEM_SLOT_01") )
		{
			GapX = pCtrl2->GetOffset().x - pCtrl->GetOffset().x;
		}
		if( pCtrl2 = Find("ITEM_SLOT_10") )
		{
			GapY = pCtrl2->GetOffset().y - pCtrl->GetOffset().y;
		}
	}

	for( int iPage = 0; iPage < g_iPageCount; ++iPage )
	{
		for( int iSlot = 0 ; iSlot < g_iSlotCountPerPage ; ++iSlot )
		{
			ptOffset.x = pCtrl->GetOffset().x + ( iSlot % 8 ) * GapX;
			ptOffset.y = pCtrl->GetOffset().y + ( iSlot / 8 ) * GapY;

			m_Slots[iPage][iSlot].SetOffset( ptOffset );
			m_Slots[iPage][iSlot].SetParent( GetDialogType() );
			m_Slots[iPage][iSlot].SetDragAvailable();
			m_Slots[iPage][iSlot].SetDragItem( m_pDragItem );
		}
	}

	//창고 344
	//GetCaption()->SetString( LIST_STRING(344) );
	
	if(pCtrl = Find(IID_BTN_TAB1))
	{
		pCtrl->SetText(CStr::Printf("%s%d", LIST_STRING(344), 1));
	}
	if(pCtrl = Find(IID_BTN_TAB2))
	{
		pCtrl->SetText(CStr::Printf("%s%d", LIST_STRING(344), 2));
	}
	if(pCtrl = Find(IID_BTN_TAB3))
	{
		pCtrl->SetText(CStr::Printf("%s%d", LIST_STRING(344), 3));
	}
	if(pCtrl = Find(IID_BTN_TAB4))
	{
//홍근 빌링 수정
#ifdef __JAPAN_BILL		
		pCtrl->SetText(CStr::Printf("%s%d", LIST_STRING(344), 4));
#else
		//플레티넘 590
		pCtrl->SetText( LIST_STRING(590) );
#endif
		
	}
	if(pCtrl = Find(IID_BTN_SAVE))
	{
		//보관 642
		pCtrl->SetText( LIST_STRING(642) );
	}
	if(pCtrl = Find(IID_BTN_WITHDRAW))
	{
		//꺼내기 643
		pCtrl->SetText( LIST_STRING(643) );
	}
	
	POINT pt = { GetPosition().x, GetPosition().y };	
	MoveWindow(pt);

}

void CBankDlg::Show()
{

#ifdef _DEBUG

	POINT pt = { GetPosition().x, GetPosition().y };
	Clear();
	Create("DlgBank");		
	SetInterfacePos_After();
	MoveWindow(pt);

#endif

	CTDialog::Show();

	m_iCurrBankPage = 0;

	CWinCtrl* pCtrl = Find( IID_RADIOBOX );
	if( pCtrl && pCtrl->GetControlType() == CTRL_RADIOBOX )
	{
		CTRadioBox* pRadioBox = (CTRadioBox*)pCtrl;
		pRadioBox->SetPressedButton( IID_BTN_TAB1 );
	}
}

CBankDlg::CBankDlg( int iType )
{
	SetDialogType( iType );

	POINT ptOffset;

	m_pDragItem = new CDragItem;
	CTCmdNumberInput* pNumberCmd = new CTCmdMoveItemBank2Inv;
	CTCmdOpenNumberInputDlg* pOpenCmd = new CTCmdOpenNumberInputDlg;
	pOpenCmd->SetCommand( pNumberCmd );

	m_pDragItem->AddTarget( DLG_TYPE_ITEM, pOpenCmd );

	m_iCurrBankPage = 0;

	m_pCmdSaveMoney			= new CTCmdSaveMoneyFromBank();
	m_pCmdWithdrawMoney		= new CTCmdWithdrawMoneyFromBank();
}

CBankDlg::~CBankDlg(void)
{
	SAFE_DELETE( m_pDragItem );

	SAFE_DELETE( m_pCmdSaveMoney );
	SAFE_DELETE( m_pCmdWithdrawMoney );
}
bool CBankDlg::Create( const char* szIDD )
{
	CTDialog::Create( szIDD );

	CTButton* pBtn	= NULL;
	CWinCtrl* pCtrl = Find( IID_BTN_SAVE );
	if( pCtrl && pCtrl->GetControlType() == CTRL_BUTTON )
	{
		pBtn = (CTButton*)pCtrl;
		pBtn->SetText( STR_SAVE );
		pBtn->SetAlign(DT_VCENTER|DT_CENTER);		
		pBtn->SetTextColor( g_dwWHITE );		
	}

	pCtrl = Find( IID_BTN_WITHDRAW );
	if( pCtrl && pCtrl->GetControlType() == CTRL_BUTTON )
	{
		pBtn = (CTButton*)pCtrl;
		pBtn->SetText( STR_WITHDRAW );
		pBtn->SetAlign(DT_VCENTER|DT_CENTER);		
		pBtn->SetTextColor( g_dwWHITE );		
	}

	return true;
}

void CBankDlg::MoveWindow( POINT pt )
{
	CTDialog::MoveWindow( pt );
	for( int iPage = 0; iPage < g_iPageCount; ++iPage )
		for( int iSlot = 0 ; iSlot < g_iSlotCountPerPage ; ++iSlot )
			m_Slots[iPage][iSlot].MoveWindow( m_sPosition );

}
void CBankDlg::Draw()
{
	if( !IsVision() ) return;
	CTDialog::Draw();

	for( int iSlot = 0 ; iSlot < g_iSlotCountPerPage ; ++iSlot )
		m_Slots[m_iCurrBankPage][iSlot].Draw();


	D3DXMATRIX mat;	
	D3DXMatrixTranslation( &mat, (float)m_sPosition.x, (float)m_sPosition.y,0.0f);
	::setTransformSprite( mat );


	::drawFontf( g_GameDATA.m_hFONT[ FONT_NORMAL_BOLD ], true, 35, 5, g_dwWHITE, F_STR_BANK_TITLE,g_GameDATA.m_Account.Get() );


	const int money_buffer_size = 64;
	char money_buffer[ money_buffer_size ];
	CGameUtil::ConvertMoney2String( CBank::GetInstance().GetMoney(), money_buffer, money_buffer_size );

	CWinCtrl * pCtrl = NULL;
	if( pCtrl = Find("MONEY") )
	{
		::drawFont( g_GameDATA.m_hFONT[ FONT_NORMAL ], true, pCtrl->GetOffset().x, pCtrl->GetOffset().y, g_dwWHITE, money_buffer );
	}
	
}

/**
* 은행원( 은행을 열수 있는 NPC )와 일정거리 이상이 되엇을경우 창을 닫는다.
*/
void CBankDlg::Update( POINT ptMouse )
{
	if( !IsVision() ) return;
	CTDialog::Update( ptMouse );

	for( int iSlot = 0 ; iSlot < g_iSlotCountPerPage ; ++iSlot )
	{
		m_Slots[m_iCurrBankPage][iSlot].Update( ptMouse );
	}

	CObjCHAR* pObjChar = g_pObjMGR->Get_CharOBJ( CBank::GetInstance().GetNpcClientIndex(), false );
	///Object가 없거나 거리가 일정이상이면 닫아버려라.
	if( pObjChar == NULL )
		Hide();
	else if( g_pAVATAR->Get_DISTANCE( pObjChar ) >= 1000 )
		Hide();

}

unsigned int CBankDlg::Process(UINT uiMsg,WPARAM wParam,LPARAM lParam)
{
	if( !IsVision() ) return 0;
	unsigned iProcID = 0;
	if( iProcID = CTDialog::Process( uiMsg, wParam, lParam ) )
	{
		for( int iSlot = 0 ; iSlot < g_iSlotCountPerPage ; ++iSlot )
		{
			if( m_Slots[m_iCurrBankPage][iSlot].Process( uiMsg,wParam,lParam) )
				return uiMsg;
		}
		switch( uiMsg )
		{
		case WM_LBUTTONUP:
			OnLButtonUp( iProcID, wParam, lParam );
			break;
		case WM_LBUTTONDOWN:
			OnLButtonDown( iProcID, wParam, lParam );
			break;
		default:
			break;
		}
		return uiMsg;
	}
	return 0;
}

void CBankDlg::OnLButtonDown( unsigned iProcID, WPARAM wParam, LPARAM lParam )
{
	switch( iProcID )
	{
	case IID_BTN_TAB1:
		m_iCurrBankPage = 0;
		break;
	case IID_BTN_TAB2:
		m_iCurrBankPage = 1;
		break;
	case IID_BTN_TAB3:
		m_iCurrBankPage = 2;
		break;
	case IID_BTN_TAB4:
		m_iCurrBankPage = 3;
		break;
	default:
		break;
	}
}

void CBankDlg::OnLButtonUp( unsigned iProcID, WPARAM wParam, LPARAM lParam )
{
	switch( iProcID )
	{
	case IID_BTN_SAVE:
		//test//
		if(1)
		{
			g_itMGR.PostMsg2Dlg( DLG_TYPE_BANKWINDOW, CBankWindowDlg::MSG_SETYPE, CBankWindowDlg::TYPE_SAVE , 0);
			g_itMGR.OpenDialog( DLG_TYPE_BANKWINDOW );
		}
		else		
		{
			CTCmdOpenNumberInputDlg OpenCmd;
			OpenCmd.SetCommand( m_pCmdSaveMoney );			
			OpenCmd.SetMaximum( g_pAVATAR->Get_MONEY() );
			OpenCmd.SetNumberInputType(CNumberInputDlg::TYPE_NORMAL);
			OpenCmd.Exec( NULL );
		}

		break;
	case IID_BTN_WITHDRAW:

		//test//
		if(1)
		{
			g_itMGR.PostMsg2Dlg( DLG_TYPE_BANKWINDOW, CBankWindowDlg::MSG_SETYPE, CBankWindowDlg::TYPE_WITHDRAW, 0 );
			g_itMGR.OpenDialog( DLG_TYPE_BANKWINDOW );
		}
		else		
		{
			CTCmdOpenNumberInputDlg OpenCmd;
			OpenCmd.SetCommand( m_pCmdWithdrawMoney );
			OpenCmd.SetMaximum( CBank::GetInstance().GetMoney() );
			OpenCmd.SetNumberInputType(CNumberInputDlg::TYPE_NORMAL);
			OpenCmd.Exec( NULL );
		}

		break;
	case IID_BTN_CLOSE:
		Hide();
		break;
	default:
		break;
	}
}

void CBankDlg::SetTabButtonText( int iID, char* szText )
{
	CWinCtrl* pCtrl = Find( iID );
	if( pCtrl && pCtrl->GetControlType() == CTRL_RADIOBUTTON )
	{
		CTRadioButton* pBtn = (CTRadioButton*)pCtrl;
		pBtn->SetText( szText );
		pBtn->SetAlign(DT_VCENTER | DT_CENTER);		
	}
}

void CBankDlg::Update( CObservable* pObservable, CTObject* pObj )
{
	assert( pObservable );
	if( pObj == NULL )
	{
		assert( pObj && "pObj is NULL @CBankDlg::update" );
		return;
	}

	CItem* pItem = (CItem*)pObj;
	tagITEM& Item = pItem->GetItem();
	CIcon* pIcon = NULL;

	int iPage = pItem->GetIndex() / g_iSlotCountPerPage;
	int iSlot = pItem->GetIndex() % g_iSlotCountPerPage;

	if( Item.IsEmpty() )///삭제
	{
		if( m_Slots[iPage][iSlot].GetIcon() == NULL )
		{
			assert( 0 && "빈 슬롯을 다시 비우려고 합니다 @CBankDlg::Update" );
		}
		else
		{
			m_Slots[iPage][iSlot].DetachIcon();
		}
	}
	else///추가
	{
		if( m_Slots[iPage][iSlot].GetIcon() )
		{
			assert( 0 && "비워있지 않은 슬롯에 Item을 넣을려고 합니다 @CBankDlg::Update" );
		}
		else
		{
			pIcon = pItem->CreateItemIcon();
			m_Slots[iPage][iSlot].AttachIcon( pIcon );
		}
	}
}
int CBankDlg::GetCurrentTab()
{
	return m_iCurrBankPage;/// 0 ~ 3
}


bool CBankDlg::IsFullFirstTab()
{
	for( int i = 0 ; i < g_iSlotCountPerPage; ++i )
	{
		if( NULL == m_Slots[0][i].GetIcon() )
			return false;
	}
	return true;
}
