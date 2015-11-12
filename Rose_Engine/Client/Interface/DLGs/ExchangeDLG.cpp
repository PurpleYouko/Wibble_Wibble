#include "stdafx.h"
#include "exchangedlg.h"
#include "../it_mgr.h"
#include "../command/uicommand.h"
#include "../command/dragcommand.h"
#include "../command/ctcmdnumberinput.h"
#include "../CDragItem.h"
#include "../Icon/CIcon.h"
#include "../../game.h"
#include "../../object.h"
#include "../../GameData/CExchange.h"
#include "../../GameData/Event/CTEventExchange.h"
#include "../../Network/Net_Prototype.h"
#include "../../GameCommon/Item.h"

#include "../../../TGameCtrl/TPushButton.h"
#include "TImage.h"

#define DS_CAPTION_SX	31
#define DS_CAPTION_SY	11

#define DS_TEXT_1X		29
#define DS_TEXT_1Y		46
#define DS_TEXT_2X		140
#define DS_TEXT_2Y		46
#define DS_TEXT_WIDTH	96
#define DS_TEXT_HEIGHT	17

#define DS_S_SX			24				/// 판매 슬롯 시작위치
#define DS_S_SY			64
#define DS_B_SX			139				/// 구입 슬롯 시작위치
#define DS_B_SY			64

#define DS_WIDTH		33				/// 슬롯 가로 크기
#define DS_HEIGHT		33				/// 슬롯 세로 크기

#define DS_COST_BX		48				/// 구입총액
#define DS_COST_BY		168			
#define DS_COST_SX		163				/// 판매총액
#define DS_COST_SY		168			

#define DS_DIGIT_WIDTH	73
#define DS_DIGIT_HEIGHT	17

const int	COLUMN_SPACE	= 2;
const int   ROW_SPACE		= 2;

CExchangeDLG::CExchangeDLG(int iType )
{
	SetDialogType( iType );
	int iSlot = 0;
	
	m_pDragItem = new CDragItem;
	CTCommand* pCmd = new CTCmdRemoveMyItemFromExchange;

	m_pDragItem->AddTarget( iType, NULL );//자기 자신은 아무것도 하지 말것
	m_pDragItem->AddTarget( CDragItem::TARGET_ALL, pCmd );
	

	m_pOtherItemDragItem = new CDragItem;
	///개인상점으로의 이동 = Wish List에 추가
	pCmd = new CTCmdDragItem2WishList;
	m_pOtherItemDragItem->AddTarget( DLG_TYPE_PRIVATESTORE, pCmd );
	

	m_pCmdRemoveMyMoneyFromExchange = new CTCmdRemoveMyMoneyFromExchange;
	m_pCmdRemoveMyItemFromExchange  = new CTCmdRemoveMyItemFromExchange;
}

CExchangeDLG::~CExchangeDLG(void)
{
	SAFE_DELETE( m_pCmdRemoveMyItemFromExchange );
	SAFE_DELETE( m_pCmdRemoveMyMoneyFromExchange );
	SAFE_DELETE( m_pDragItem );
	SAFE_DELETE( m_pOtherItemDragItem );
}

void CExchangeDLG::InitExchangeState()
{
	///교환버튼 Disable
	SetEnableChild( IID_BTN_MONEY, true );
	SetEnableChild( IID_BTN_OK, true );
	SetEnableChild( IID_BTN_EXCHANGE, true );

	/// 생성자에서 불려올경우도 생각해서 있을때만 처리하게끔
	CWinCtrl* pCtrl				= NULL;
	CTPushButton* pPushButton	= NULL;
	pCtrl = Find( IID_BTN_OK );
	if( pCtrl && pCtrl->GetControlType() == CTRL_PUSHBUTTON )
	{
		pPushButton = (CTPushButton*)pCtrl;
		pPushButton->SetButtonState( CTPushButton::BS_NORMAL );
	}

	pCtrl = Find( IID_BTN_EXCHANGE );
	if( pCtrl && pCtrl->GetControlType() == CTRL_PUSHBUTTON )
	{
		pPushButton = (CTPushButton*)pCtrl;
		pPushButton->SetButtonState( CTPushButton::BS_NORMAL );

		pCtrl->SetEnable(false);
	}
}

//

void CExchangeDLG::Show()
{
	if( !IsVision() )
	{
		CTDialog::Show();
		InitExchangeState();
	}
}

void CExchangeDLG::Draw()
{
	if( !IsVision()) 
		return;
	
	CTDialog::Draw();

	for( int i = 0 ; i < TOTAL_EXCHANGE_INVENTORY; ++i )
	{
		m_MySlots[i].Draw();
		m_OtherSlots[i].Draw();
	}

	D3DXMATRIX mat;	
	D3DXMatrixTranslation( &mat, (float)m_sPosition.x, (float)m_sPosition.y,0.0f);
	::setTransformSprite( mat );

	CExchange& Data = CExchange::GetInstance();

	CWinCtrl * pCtrl = NULL;
	if(pCtrl = Find("AVATAR_NAME"))
	{
		((CTImage*)pCtrl)->SetText(g_pAVATAR->Get_NAME());
	}
	if(pCtrl = Find("OTHER_NAME"))
	{
		((CTImage*)pCtrl)->SetText(Data.GetOtherName().c_str());
	}
	if(pCtrl = Find("MYMONEY"))
	{
		((CTImage*)pCtrl)->SetText(CStr::Printf("%d", Data.GetMyTradeMoney()));
	}
	if(pCtrl = Find("OTHERMONETY"))
	{
		((CTImage*)pCtrl)->SetText(CStr::Printf("%d", Data.GetOtherTradeMoney()));
	}
}

unsigned int CExchangeDLG::Process( UINT uiMsg, WPARAM wParam, LPARAM lParam )
{
	if(!IsVision()) return 0;

	unsigned iProcID = 0;
	if( iProcID = CTDialog::Process(uiMsg,wParam,lParam)) 
	{
		int iSlot = 0;

		for( iSlot = 0 ; iSlot < TOTAL_EXCHANGE_INVENTORY; ++iSlot )
			if( m_MySlots[iSlot].Process( uiMsg, wParam, lParam ) )
				return uiMsg;

		for( iSlot = 0 ; iSlot < TOTAL_EXCHANGE_INVENTORY; ++iSlot )
			if( m_OtherSlots[iSlot].Process( uiMsg, wParam, lParam ) )
				return uiMsg;


		switch(uiMsg)
		{
			case WM_LBUTTONDOWN:
				OnLButtonDown( iProcID, wParam, lParam );
				break;
			case WM_LBUTTONUP:
				OnLButtonUp( iProcID, wParam, lParam );
				break;
			default:
				break;
		}
		return uiMsg;
	}
	return 0;
}
void CExchangeDLG::OnLButtonUp( unsigned iProcID, WPARAM wParam, LPARAM lParam  )
{
	switch( iProcID )
	{
	case IID_BTN_MONEY:
		{
			CTCmdOpenNumberInputDlg OpenCmd;
			OpenCmd.SetCommand( m_pCmdRemoveMyMoneyFromExchange );
			OpenCmd.SetMaximum( CExchange::GetInstance().GetMyTradeMoney() );
			OpenCmd.Exec(NULL);
			
			break;
		}
	case IID_BTN_CLOSE:
		{
			CExchange::GetInstance().SendTradePacket( RESULT_TRADE_CANCEL );
			Hide();
			break;
		}
	default:
		break;
	}
}
void CExchangeDLG::OnLButtonDown( unsigned iProcID, WPARAM wParam, LPARAM lParam )
{
	switch( iProcID )
	{
	case IID_BTN_OK:
		{
			CExchange::GetInstance().SendTradePacket( RESULT_TRADE_CHECK_READY );
			CExchange::GetInstance().SetReadyMe( true );
			SetMeReadyState( true );

			break;
		}
	case IID_BTN_EXCHANGE:
		{
			CExchange::GetInstance().SendTradePacket( RESULT_TRADE_DONE );
			SetEnableChild( IID_BTN_OK , false );
			break;
		}
	default:
		break;
	}
}


void CExchangeDLG::Hide()
{
	CTDialog::Hide();
	CExchange::GetInstance().EndExchange();
}

void	CExchangeDLG::SetOtherReadyState( bool b )
{ 
	CWinCtrl* pCtrl = Find(IID_BTN_EXCHANGE);
	CTPushButton* pPushButton = (CTPushButton*)pCtrl;

	if( CExchange::GetInstance().IsReadyAll() )
	{
		SetEnableChild( IID_BTN_EXCHANGE, true );

		SetEnableChild( IID_BTN_EXCHANGE, true );
		SetEnableChild( IID_BTN_OK, false );
	}
	else
	{
		pPushButton->SetButtonState( CTPushButton::BS_NORMAL );

		SetEnableChild( IID_BTN_EXCHANGE, false );

		pPushButton = (CTPushButton*)Find( IID_BTN_OK );
		if( CExchange::GetInstance().IsReadyMe() )
			pPushButton->SetButtonState( CTPushButton::BS_CLICKED );
		else
			pPushButton->SetButtonState( CTPushButton::BS_NORMAL );
	}
}

void	CExchangeDLG::SetMeReadyState( bool b )
{
	CWinCtrl* pCtrl = Find(IID_BTN_OK);
	CTPushButton* pPushButton = (CTPushButton*)pCtrl;
	if( CExchange::GetInstance().IsReadyMe() )
	{
		pPushButton->SetButtonState( CTPushButton::BS_CLICKED );
		SetEnableChild( IID_BTN_MONEY, false );
	}
	else
	{
		pPushButton->SetButtonState( CTPushButton::BS_NORMAL );
		SetEnableChild( IID_BTN_MONEY, true );
	}


	pCtrl = Find(IID_BTN_EXCHANGE);
	pPushButton = (CTPushButton*)pCtrl;

	if( CExchange::GetInstance().IsReadyAll() )
	{
		SetEnableChild( IID_BTN_EXCHANGE, true );
		ShowChild( IID_BTN_EXCHANGE );
	}
	else
	{
		pPushButton->SetButtonState( CTPushButton::BS_NORMAL );

		SetEnableChild( IID_BTN_OK, true );
		ShowChild( IID_BTN_OK );
	}
}

int  CExchangeDLG::IsInValidShow()
{
	if( g_pAVATAR && g_pAVATAR->Get_HP() <= 0 )
		return 99;

	if( g_itMGR.IsDlgOpened( DLG_TYPE_DEAL ) )
		return DLG_TYPE_DEAL;

	if( g_itMGR.IsDlgOpened( DLG_TYPE_EXCHANGE ))
		return DLG_TYPE_EXCHANGE;

	if( g_itMGR.IsDlgOpened( DLG_TYPE_DIALOG ))
		return DLG_TYPE_DIALOG;

	if( g_itMGR.IsDlgOpened( DLG_TYPE_MAKE ))
		return DLG_TYPE_MAKE;

	return 0;
}


/** @param	if( pObj is NULL) 
*				기타 설정 변경( 상대방 설정, 내설정은 내가 패킷 보내기 전에 바꾼다 )
*			else 
*				나 혹은 상대방의 아이템 변경
*/
void CExchangeDLG::Update( CObservable* pObservable, CTObject* pObj )
{
	assert( pObservable );
	if( pObj == NULL )
	{
		assert( pObj && "pObj is NULL @CExchangeDLG::Update" );
		return;
	}
	
	if( strcmp( pObj->toString(), "CTEventExchange" ) )
	{
		assert( 0 && "CTObject's Type is Invalid @CExchangeDLG::Update" );	
		return;
	}


	CTEventExchange* pEvent = (CTEventExchange*)pObj;
	switch( pEvent->GetID() )
	{
	case CTEventExchange::EID_ADD_MYITEM :
		{
			int iSlotIndex = pEvent->GetSlotIndex();
			if( m_MySlots[iSlotIndex].GetIcon() )
			{
				assert( 0 && "Attach Icon to Not Empty Slot @CExchangeDLG::Update" );
			}
			else
			{
				CItem* pItem = pEvent->GetItem();

				CIcon* pIcon = pItem->CreateItemIcon();
				pIcon->SetCommand( m_pCmdRemoveMyItemFromExchange );

				m_MySlots[iSlotIndex].AttachIcon( pIcon );
			}
			break;
		}
	case CTEventExchange::EID_REMOVE_MYITEM:
		{
			int iSlotIndex = pEvent->GetSlotIndex();		
			if( m_MySlots[ iSlotIndex ].GetIcon() == NULL )
				assert( m_MySlots[ iSlotIndex ].GetIcon() && "Detach Icon From Empty Slot @CExchangeDLG::Update" );

			m_MySlots[ iSlotIndex ].DetachIcon();
			break;
		}
	case CTEventExchange::EID_ADD_OTHERITEM:
		{
			int iSlotIndex = pEvent->GetSlotIndex();
			if( m_OtherSlots[iSlotIndex].GetIcon() )
			{
				assert( 0 && "Attach Icon to Not Empty Slot @CExchangeDLG::Update" );
			}
			else
			{
				CItem* pItem = pEvent->GetItem();
				CIcon* pIcon = pItem->CreateItemIcon();
				pItem->SetIndex( iSlotIndex );
				m_OtherSlots[iSlotIndex].AttachIcon( pIcon );
			}
			break;
		}
	case CTEventExchange::EID_REMOVE_OTHERITEM:
		{
			int iSlotIndex = pEvent->GetSlotIndex();		
			if( m_OtherSlots[ iSlotIndex ].GetIcon() == NULL )
				assert( m_OtherSlots[ iSlotIndex ].GetIcon() && "Detach Icon From Empty Slot @CExchangeDLG::Update" );

			m_OtherSlots[ iSlotIndex ].DetachIcon();
			break;
		}
	case CTEventExchange::EID_CHANGE_READYOTHER:
		SetOtherReadyState( pEvent->GetReadyOther() );
		break;
	default:
		assert( 0 && "Event's ID is Invalid @CExchangDLG::Update" );
		break;
	}
}
void CExchangeDLG::MoveWindow( POINT pt )
{
	CTDialog::MoveWindow( pt );
	for( int i = 0 ; i < TOTAL_EXCHANGE_INVENTORY; ++i )
	{
		m_MySlots[i].MoveWindow( m_sPosition );
		m_OtherSlots[i].MoveWindow( m_sPosition );
	}

}
void CExchangeDLG::Update( POINT ptMouse )
{
	if( !IsVision() ) return;
	CTDialog::Update( ptMouse );
	for( int i = 0 ; i < TOTAL_EXCHANGE_INVENTORY; ++i )
	{
		m_MySlots[i].Update( ptMouse );
		m_OtherSlots[i].Update( ptMouse );
	}
}

void CExchangeDLG::SetInterfacePos_After()
{
	CWinCtrl * pCtrl00 = Find("SLOT_POS_00");
	CWinCtrl * pCtrl01 = Find("SLOT_POS_01");
	CWinCtrl * pCtrl10 = Find("SLOT_POS_10");
	CWinCtrl * pCtrl20 = Find("SLOT_POS_20");
	int iGapX = 0, iGapY = 0;
	int iSlot;
	POINT ptOffset;

	if( pCtrl00 && pCtrl01 && pCtrl10 && pCtrl20 )
	{
		iGapX = pCtrl01->GetOffset().x - pCtrl00->GetOffset().x;
		iGapY = pCtrl10->GetOffset().y - pCtrl00->GetOffset().y;

		for( iSlot = 0; iSlot < TOTAL_EXCHANGE_INVENTORY; ++iSlot )
		{
			m_MySlots[iSlot].SetWidth( pCtrl00->GetWidth() );
			m_MySlots[iSlot].SetHeight( pCtrl00->GetHeight() );

			ptOffset.x = pCtrl00->GetOffset().x  + ( iSlot % 5 ) * iGapX;
			ptOffset.y = pCtrl00->GetOffset().y  + ( iSlot / 5 ) * iGapY;

			m_MySlots[iSlot].SetOffset( ptOffset );
			m_MySlots[iSlot].SetParent( GetDialogType() );
			m_MySlots[iSlot].SetDragAvailable();
			m_MySlots[iSlot].SetDragItem( m_pDragItem );
		}


		for( iSlot = 0; iSlot < TOTAL_EXCHANGE_INVENTORY; ++iSlot )
		{
			m_OtherSlots[iSlot].SetWidth( pCtrl00->GetWidth() );
			m_OtherSlots[iSlot].SetHeight( pCtrl00->GetHeight() );

			ptOffset.x = pCtrl20->GetOffset().x + ( iSlot % 5 ) * iGapX;
			ptOffset.y = pCtrl20->GetOffset().y + ( iSlot / 5 ) * iGapY;

			m_OtherSlots[iSlot].SetOffset( ptOffset );
			m_OtherSlots[iSlot].SetParent( GetDialogType() );
			m_OtherSlots[iSlot].SetDragAvailable();
			m_OtherSlots[iSlot].SetDragItem( m_pOtherItemDragItem );
		}
	}

	
	//문자열 추가.
	if( pCtrl00 = Find(IID_BTN_EXCHANGE) )
	{
		//897
        pCtrl00->SetText(LIST_STRING(897));
	}
	if( pCtrl00 = Find(IID_BTN_OK) )
	{
		//858
		pCtrl00->SetText(LIST_STRING(858));
	}
}