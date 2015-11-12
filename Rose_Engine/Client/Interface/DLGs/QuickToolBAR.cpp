
#include "StdAfx.h"
#include "QuickToolBAR.h"
#include "../it_mgr.h"
#include "../CDragItem.h"
#include "../CTDrawImpl.h"
#include "../IO_ImageRes.h"
#include "../../GameCommon/Skill.h"	
#include "../CClientStorage.h"
#include "../../OBJECT.h"
#include "../../Network/CNetwork.h"
#include "../../Common/IO_STB.h"


#include "../../System/CGame.h"
#include "../../JCommandState.h"	
#include "../../GameCommon/CSkillCommand.h"
#include "../../gamecommon/item.h"
#include "../Icon/CIconQuick.h"
#include "../Icon/CIconItem.h"
#include "../command/dragcommand.h"

#include "../../GameData/Event/CTEventItem.h"
#include "../../Country.h"

#include "TPane.h"
#include "TEditBox.h"
#include "TImage.h"
#include "ResourceMgr.h"
#include "TControlMgr.h"

//#define TYPE1_BODY_SIZE		4	//몸통
//#define TYPE1_TAIL_SIZE		33	//슬롯박스 크기 
//#define TYPE2_BODY_SIZE		30
//#define TYPE2_TAIL_SIZE		33


CQuickBAR::CQuickBAR( int iType )
{
	SetDialogType( iType );
	m_nCurrentPage	= 0;	
	m_nType			= TYPE_HORIZONTAL;		

	m_pDragItem = new CDragItem;
	CTCommand* pCmd = new CTCmdDragItemFromQuickBar;
	m_pDragItem->AddTarget( CDragItem::TARGET_ALL, pCmd );
	pCmd = new CTCmdMoveIconInQuickBar( iType );
	m_pDragItem->AddTarget( iType , pCmd );

	m_pCmdDragItemFromQuickBar = new CTCmdDragItemFromQuickBar;

	for( int i = 0 ; i < HOT_ICONS_PER_PAGE; ++i )
	{
		m_QuickSlot[i].SetParent( iType );
		m_QuickSlot[i].SetDragAvailable();
		m_QuickSlot[i].SetDragItem( m_pDragItem );

//		m_QuickSlot[i].SetText( CStr::Printf("F%d", i+1) );
//		m_QuickSlot[i].SetAlign( DT_RIGHT|DT_BOTTOM );
//		m_QuickSlot[i].SetFont( FONT_NORMAL_OUTLINE );
	}
	
	SetQuickBarType( QUICKBAR_TYPE_NORMAL );	
}

CQuickBAR::~CQuickBAR()
{	
	SAFE_DELETE( m_pDragItem );
	SAFE_DELETE( m_pCmdDragItemFromQuickBar );
}

void CQuickBAR::Show()
{
	CTDialog::Show();
	ChangeType( m_nType );
}

void CQuickBAR::Draw()
{
	if( !IsVision() ) return;

	CTDialog::Draw();

	/// 게이지 진행상태를 보여주기 위해서..
	::endSprite();
	::beginSprite( D3DXSPRITE_ALPHABLEND ); 	

	/// quick slot rendering
	DrawQuickSlot();

	CWinCtrl * pCtrl = NULL;	
	for( int i=0 ; i<IID_FUNC_TXT_CNT; i++ )
	{
		if( pCtrl = Find( i + IID_FUNC_TXT ) )
		{			
			pCtrl->Draw();
		}	
	}
}


//----------------------------------------------------------------------------------------------------
/// @param
/// @brief 각 CSlot 들의 MoveWindow 호출..
//----------------------------------------------------------------------------------------------------

void CQuickBAR::MoveWindow( POINT pt )
{
	/// offset 먼저 설정..
	UpdateCSlotPosition();

	CTDialog::MoveWindow( pt );

	for( int i = 0 ; i < HOT_ICONS_PER_PAGE; i++ )
	{
		m_QuickSlot[ i ].MoveWindow( this->m_sPosition );
	}
}


/// quick slot rendering
void CQuickBAR::DrawQuickSlot()
{
	for( int i = 0 ; i < HOT_ICONS_PER_PAGE; i++ )
	{
		m_QuickSlot[ i ].Draw();
	}
}

//----------------------------------------------------------------------------------------------------
/// @param PINT ptMouse
/// @brief	매프레임 호출..
//----------------------------------------------------------------------------------------------------

void CQuickBAR::Update( POINT ptMouse )
{
	if( !IsVision() ) return;

	CTDialog::Update( ptMouse );

	for( int i = 0 ; i < HOT_ICONS_PER_PAGE; ++i )
		m_QuickSlot[i].Update( ptMouse );
}


//----------------------------------------------------------------------------------------------------
/// @param
/// @brief 유져 이벤트( 마우스, 키보드.. ) 처리..
//----------------------------------------------------------------------------------------------------

unsigned int CQuickBAR::Process( UINT uiMsg, WPARAM wParam, LPARAM lParam )
{
	switch( uiMsg )
	{
	case WM_SYSKEYDOWN :	

		if( GetQuickBarType() == QUICKBAR_TYPE_EXTENSION )
		{
			m_nCurrentPage = QSLOT_ALT;
			UpdateHotIconSlot();
		}	

		switch( wParam )
		{
		case '1':
			if( GetQuickBarType() == QUICKBAR_TYPE_NORMAL )
			{
				m_nCurrentPage = 0;
				UpdateHotIconSlot();
				return uiMsg;
			}		
			return 0;
			

		case '2':
		case VK_F10:
			if( GetQuickBarType() == QUICKBAR_TYPE_NORMAL )
			{
				m_nCurrentPage = 1;
				UpdateHotIconSlot();
				return uiMsg;
			}		
			return 0;
			
		case '3':
			if( GetQuickBarType() == QUICKBAR_TYPE_NORMAL )
			{
				m_nCurrentPage = 2;
				UpdateHotIconSlot();
				return uiMsg;
			}			
			return 0;
		case '4':
			if( GetQuickBarType() == QUICKBAR_TYPE_NORMAL )
			{
				m_nCurrentPage = 3;
				UpdateHotIconSlot();
				return uiMsg;
			}			
			return 0;

		case VK_F1:
		case VK_F2:
		case VK_F3:
		case VK_F4:
		case VK_F5:
		case VK_F6:
		case VK_F7:
		case VK_F8:
			//ALT
			if( GetQuickBarType() == QUICKBAR_TYPE_EXTENSION )
			{	
				CIcon* pIcon = m_QuickSlot[ wParam-VK_F1 ].GetIcon();			
				if( pIcon )
				{
					pIcon->ExecuteCommand();
				}				
				return uiMsg;
			}			
			return 0;

		default:
			break;
		}
		break;
	case WM_KEYUP:
		{
			switch ( wParam ) 
			{
			case '1':
			case '2':
			case '3':
			case '4':
				{
					if( IsApplyNewVersion() 
						&& CTControlMgr::GetInstance()->GetKeyboardInputType() == CTControlMgr::INPUTTYPE_NORMAL 
						&& NULL == CTEditBox::s_pFocusEdit
						&& GetQuickBarType() == QUICKBAR_TYPE_NORMAL )
					{
						m_nCurrentPage = wParam - '1';
						UpdateHotIconSlot();
						return uiMsg;
					}
				}
				break;

			case VK_F1:
			case VK_F2:
			case VK_F3:
			case VK_F4:
			case VK_F5:
			case VK_F6:
			case VK_F7:
			case VK_F8:
				{
					if( GetQuickBarType() == QUICKBAR_TYPE_NORMAL )
					{
						CIcon* pIcon = m_QuickSlot[ wParam - VK_F1 ].GetIcon();			
						if( pIcon )
							pIcon->ExecuteCommand();
						return uiMsg;	
					}				
					else
					{
						if( GetAsyncKeyState( VK_CONTROL ) < 0 )
						{
							CIcon* pIcon = m_QuickSlot[ wParam - VK_F1 ].GetIcon();			
							if( pIcon )
								pIcon->ExecuteCommand();
							return uiMsg;	
						}						
					}					
					return 0;					
				}
			case VK_F9:
				if( GetQuickBarType() == QUICKBAR_TYPE_NORMAL )
				{
					m_nCurrentPage = 0;
					UpdateHotIconSlot();
					return uiMsg;
				}								
				return 0;

			case VK_F11:
				if( GetQuickBarType() == QUICKBAR_TYPE_NORMAL )
				{				
					m_nCurrentPage = 2;
					UpdateHotIconSlot();
					return uiMsg;
				}								
				return 0;

			case VK_F12:
				if( GetQuickBarType() == QUICKBAR_TYPE_NORMAL )
				{
					m_nCurrentPage = 3;
					UpdateHotIconSlot();
					return uiMsg;
				}								
				return 0;

			default:
				break;
			}
			break;
		}

	case WM_KEYDOWN:
		{			
			if( GetAsyncKeyState( VK_CONTROL ) < 0  && GetQuickBarType() == QUICKBAR_TYPE_EXTENSION )
			{
				m_nCurrentPage = QSLOT_CTRL;
				UpdateHotIconSlot();
			}
		}		
		
	default:
		break;
	}

	if(!IsVision()) return 0;

	//Dlg보다 Slot부터 수정.
	for( int i  = 0; i < HOT_ICONS_PER_PAGE; ++i )
	{
		if( m_QuickSlot[i].Process( uiMsg, wParam, lParam ) )
			return uiMsg;
	}

	unsigned iProcID = 0;
	if( iProcID = CTDialog::Process(uiMsg,wParam,lParam) )
	{
//		for( int i  = 0; i < HOT_ICONS_PER_PAGE; ++i )
//		{
//			if( m_QuickSlot[i].Process( uiMsg, wParam, lParam ) )
//				return uiMsg;
//		}
		switch(uiMsg)
		{
		case WM_LBUTTONUP:
			On_LButtonUP( iProcID, wParam, lParam );
			break;
		default:
			break;
		}
		return uiMsg;
	}
	
	return 0;
}


//----------------------------------------------------------------------------------------------------
/// @param
/// @brief update cslot of hoticon
//----------------------------------------------------------------------------------------------------
void CQuickBAR::UpdateHotIconSlot()
{
	for( int i = 0 ; i < HOT_ICONS_PER_PAGE; i++ )
	{
		m_QuickSlot[ i ].DetachIcon();

		CHotIconSlot* pHotIconSlot	= g_pAVATAR->GetHotIconSlot();
		CIcon* pIcon				= pHotIconSlot->GetHotIcon( m_nCurrentPage * HOT_ICONS_PER_PAGE + i );

		m_QuickSlot[ i ].AttachIcon( pIcon );
	}
	
	UpdateCSlotPosition();	
}


//----------------------------------------------------------------------------------------------------
/// @param
/// @brief 현재의 상태에 따라 각 Slot 객체들의 위치를 갱신한다.
//----------------------------------------------------------------------------------------------------

void CQuickBAR::UpdateCSlotPosition()
{
	POINT pt = { 0, 0 };

	CWinCtrl * pCtrl = NULL;

	
	if( m_nType == TYPE_HORIZONTAL )
	{
		if( pCtrl = Find("HOR_ICON_POS") )
		{
			pt.y = pCtrl->GetOffset().y;
		}
		for( int i = 0 ; i < HOT_ICONS_PER_PAGE ; ++i )
		{			
			pt.x = pCtrl->GetOffset().x + i * 42;
			m_QuickSlot[i].SetOffset( pt );
		}
	}
	else
	{
		if( pCtrl = Find("VER_ICON_POS") )
		{
			pt.x = pCtrl->GetOffset().x;
		}		
		for( int i = 0; i < HOT_ICONS_PER_PAGE; ++i )
		{			
            pt.y = pCtrl->GetOffset().y + i * 42;
			m_QuickSlot[i].SetOffset( pt );
		}
	}

	if( m_nType == TYPE_HORIZONTAL )
	{
		pCtrl = Find( "HORIZONTAL_FUNC" );
	}
	else
	{
		pCtrl = Find( "VERTICAL_FUNC" );
	}

	if( pCtrl == NULL )	return;

	///현재 페이지번호를 바꾸어 준다.
	CWinCtrl* pCtrl1	= pCtrl->Find( "1" );
	CWinCtrl* pCtrl2	= pCtrl->Find( "2" );
	CWinCtrl* pCtrl3	= pCtrl->Find( "3" );
	CWinCtrl* pCtrl4	= pCtrl->Find( "4" );
	CWinCtrl* pCtrlAlt	= pCtrl->Find( "ALT" );
	CWinCtrl* pCtrlCtrl	= pCtrl->Find( "CTRL" );


	if( pCtrl1 && pCtrl2 && pCtrl3 && pCtrl4 && pCtrlAlt && pCtrlCtrl )
	{
		pCtrl1->Hide();	pCtrl2->Hide();	pCtrl3->Hide();	pCtrl4->Hide(); 
		pCtrlAlt->Hide(); pCtrlCtrl->Hide();

		switch( m_nCurrentPage )
		{
		case 0:
			pCtrl1->Show();
			break;
		case 1:
			pCtrl2->Show();
			break;
		case 2:
			pCtrl3->Show();
			break;
		case 3:
			pCtrl4->Show();
			break;
		case 4:
			pCtrlAlt->Show();
			break;
		case 5:
			pCtrlCtrl->Show();
		}
	}
}


//----------------------------------------------------------------------------------------------------
/// @param
/// @brief 현재 선택된 퀵 슬롯을 얻는다. 절대위치 슬롯번호이다.
//----------------------------------------------------------------------------------------------------

short CQuickBAR::GetMouseClickSlot( POINT& ptMouse )
{
	/// 현재 페이지에 등록된 슬롯들을 돌며 체크
	for( int i = 0 ; i < HOT_ICONS_PER_PAGE; i++ )
	{
		if( m_QuickSlot[ i ].IsInside( ptMouse.x, ptMouse.y ) == true )
		{
			return m_nCurrentPage * HOT_ICONS_PER_PAGE + i;
		}
	}

	return -1;
}
//----------------------------------------------------------------------------------------------------
/// @param
/// @brief process each child control..
//----------------------------------------------------------------------------------------------------

bool CQuickBAR::On_LButtonUP( unsigned iProcID, WPARAM wParam, LPARAM lParam )
{	
	switch( iProcID )
	{
	case IID_BTN_HORIZONTAL_PREV:
	case IID_BTN_VERTICAL_PREV:
		{
			--m_nCurrentPage;
			if( m_nCurrentPage < m_nStartPage )
				m_nCurrentPage = m_nEndPage - 1;

			UpdateHotIconSlot();
			break;
		}
	case IID_BTN_HORIZONTAL_NEXT:
	case IID_BTN_VERTICAL_NEXT:
		{
			m_nCurrentPage++;

			if( m_nCurrentPage >= m_nEndPage ) 
				m_nCurrentPage = m_nStartPage;

			UpdateHotIconSlot();
		}
		return true;
	case IID_BTN_ROTATE:
		{
			if( m_nType == TYPE_HORIZONTAL )
				ChangeType( TYPE_VERTICAL );
			else
				ChangeType( TYPE_HORIZONTAL );

			UpdateCSlotPosition();	

			break;
		}
	default:
		break;
	}

	

	return false;
}

//----------------------------------------------------------------------------------------------------
/// @param
/// @brief Change quick bar type ( vertical <-> horizontal 
/// @todo  Number Image Offset, Rotate Button Offset Change
//----------------------------------------------------------------------------------------------------

void CQuickBAR::ChangeType( int iType , bool bInit )
{
	RECT rc = {0,0,0,0};
	CWinCtrl * ctrl = NULL;

	if( iType == TYPE_HORIZONTAL )
	{
		ShowChild( "HORIZONTAL" );
		HideChild( "VERTICAL");
		ShowChild( "HORIZONTAL_FUNC" );
		HideChild( "VERTICAL_FUNC");
		ctrl = Find( "HORIZONTAL" );			
	}
	else
	{
		HideChild( "HORIZONTAL" );
		ShowChild( "VERTICAL");
		HideChild( "HORIZONTAL_FUNC" );
		ShowChild( "VERTICAL_FUNC");
		ctrl = Find( "VERTICAL" );		
	}

	if( ctrl )
	{
		//dlg size.
		SetHeight( ctrl->GetHeight() );
		SetWidth( ctrl->GetWidth() );

		//dlg caption.
		SetRect( &rc, 0, 0, ctrl->GetWidth(), ctrl->GetHeight() );
		ChangeCaptionRect( rc );		
	}

	POINT pt = GetPosition();

	if(m_nType != iType)
	{
		// 회전버튼 기준으로 위치 이동.
		int iRectSize = GetWidth() - GetHeight();	
		pt.x += -iRectSize;
		pt.y += iRectSize;
	}

	
	if( pt.x < 0 ) pt.x = 1;
	if( pt.y < 0 ) pt.y = 1;
	if( pt.x + GetWidth()  > g_pCApp->GetWIDTH() )
		pt.x = g_pCApp->GetWIDTH() - GetWidth();

	if( pt.y + GetHeight()  > g_pCApp->GetHEIGHT() )
		pt.y = g_pCApp->GetHEIGHT() - GetHeight();

	m_nType = iType;

	MoveWindow( pt );

}

//----------------------------------------------------------------------------------------------------
/// @param
/// @brief CItemSlot이 변경되어질때마다 호출되는 Method : 추후 CSkillSlot에서도 호출되어질수 있다( Skill이 없어지는 경우가 생긴다면 )
//----------------------------------------------------------------------------------------------------
void CQuickBAR::Update( CObservable* pObservable, CTObject* pObj )
{
	////등록된 아이콘중 아이템이고 위와 같은것일경우 지운다.
	assert( pObservable );

	if( pObj && strcmp( pObj->toString(), "CTEventItem" ) == 0 )
	{
		CTEventItem* pEvent = (CTEventItem*)pObj;

		if( pEvent->GetID() == CTEventItem::EID_DEL_ITEM )///지워지는것만 처리하면된다.
		{
			CIcon* pIcon			= NULL;
			CIconItem* pItemIcon	= NULL;
			CIconQuick* pQuickIcon	= NULL;	

			int iIndex = pEvent->GetIndex();

			for( int i = 0 ; i < HOT_ICONS_PER_PAGE; ++i )
			{
				if( g_pAVATAR->m_HotICONS.m_IconPAGE[ m_nCurrentPage ][ i ].m_cType == 1 )
				{
					if( pIcon = m_QuickSlot[i].GetIcon() )
					{
						pQuickIcon = (CIconQuick*)pIcon;
						pItemIcon  = (CIconItem*)pQuickIcon->GetIcon();
						///인벤토리 인덱스가 같고 지워지는 아이템일경우
						///같은 아이템이 2군데 등록되어 있을수 있다.
						if( pItemIcon->GetIndex() == iIndex  )
						{
							/// Detach시에 pQuickIcon은 파괴된다.( Detach전에 Command실행할것 )
							m_pCmdDragItemFromQuickBar->Exec( pQuickIcon );					
							m_QuickSlot[i].DetachIcon();
						}
					}
				}
			}
		}
	}
	else
	{
		assert( 0 && "CTEvent is NULL or Invalid Type@CQuickBAR::Update" );
	}
}

//*-----------------------------------------------------------------------------------------------------/
/// 서버에 패킷 보내지 않고 클라이언트에서만 Data를 삭제할 경우
/// 게임내에서 캐릭터 선택창으로 이동시 
//*-----------------------------------------------------------------------------------------------------/
void CQuickBAR::Clear()
{
	for( int i = 0; i < HOT_ICONS_PER_PAGE; ++i )
		m_QuickSlot[ i ].DetachIcon();
}

void CQuickBAR::SetInterfacePos_After()
{	
	for( int i = 0 ; i < HOT_ICONS_PER_PAGE; ++i )
	{
		MoveCtrl2ListEnd( m_QuickSlot[i].GetControlID() );		
	}

	MoveCtrl2ListEnd( IID_PAN_HOR_FUNC );
	MoveCtrl2ListEnd( IID_PAN_VER_FUNC );	


	t_OptionShortCut ShortCutOption;
	g_ClientStorage.GetShortCutOption( ShortCutOption );
	if( ShortCutOption.iShortCutView==0 )
	{
		Hide();
	}

	CWinCtrl * pCtrl = NULL;	
	for( int i=0 ; i<IID_FUNC_TXT_CNT; i++ )
	{
		if( pCtrl = Find( i + IID_FUNC_TXT ) )
		{			
			((CTImage*)pCtrl)->SetText( CStr::Printf( "F%d", (i)%8+1 ) );					
			((CTImage*)pCtrl)->SetFont( FONT_NORMAL_OUTLINE );
			((CTImage*)pCtrl)->SetAlign( DT_VCENTER | DT_RIGHT );
		}	
	}
	
}

void CQuickBAR::SetQuickBarType( short nType )
{
	m_nQuickBarType = nType;

	switch( nType )
	{
	case QUICKBAR_TYPE_NORMAL:
		SetStartEndPage( 0, MAX_ICONS_PAGES_NORMAL );		
		break;
	case QUICKBAR_TYPE_EXTENSION:
		SetStartEndPage( MAX_ICONS_PAGES_NORMAL, MAX_ICONS_PAGES );
		break;
	}	
	m_nCurrentPage	= m_nStartPage;
}

short	CQuickBAR::GetQuickBarType()
{
	return m_nQuickBarType;
}

void CQuickBAR::SetStartEndPage( short nStart, short nEnd )
{
	m_nStartPage	= nStart;
	m_nEndPage		= nEnd;
}


