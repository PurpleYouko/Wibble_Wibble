#include "stdafx.h"
#include ".\citemdlg.h"
#include "../../GameData/Event/CTEventItem.h"
#include "../../GameCommon/Item.h"
#include "../CDragItem.h"
#include "../it_mgr.h"
#include "../../Object.h"

#include "../Icon/CIconItem.h"
#include "../Command/CTCmdNumberInput.h"
#include "../Command/CTCmdOpenDlg.h"
#include "../Command/CTCmdHotExec.h"
#include "../Command/UICommand.h"
#include "../Command/DragCommand.h"
#include "../../GameData/CExchange.h"
#include "../../System/CGame.h"
#include "../../Network/CNetwork.h"
#include "../../misc/gameutil.h"
#include "../../Country.h"
#include "TRadioBox.h"
#include "ActionEvent.h"
#include "TPane.h"
#include "TImage.h"
#include "../Common/IO_PAT.H"
#include "TRadioButton.h"
#include "TabbedPane.h"
#include "CDragNDropMgr.h"


const	int	MAX_DROP_MONEY	= 100000;///돈을 바닥에 버릴경우 최대 10만까지

const POINT c_ptEquipedSlotOffsets[] = 
{
	{  11, 48 },	///Face			1
	{  56, 48 },	///Helmet		2
	{  69, 93 },	///Armor		5
	{ 101, 48 },	///Knapsack		3
	{  19,159 },	///Gauntlet		7
	{  69,159 },	///boots		8
	{  11, 93 },	///Weapon_R		4
	{ 119,113 },	///Weapon_L		6
	{  69,205 },	///Necklace		10
	{  19,205 },	///Ring			9
	{ 119,205 }		///Earring		11
};

const std::string c_strEquipedSlotName[] = 
{
	"FACE",		
	"HELMET",		
	"ARMOR",		
	"KNAPSACK",	
	"GAUNTLET",	
	"BOOTS",		
	"WEAPON_R",	
	"WEAPON_L",	
	"NECKLACE",	
	"RING",		
	"EARRING",
};

const POINT c_ptBulletEquipSlots[]=
{
	{ 169,67 },
	{ 169,113},
	{ 169,159}
};

const std::string c_strBulletEquipSlotName[]=
{
	"ARROW",
	"BULLET",
	"THROW",
};

const std::string c_strPatEquipSlotName[] = 
{
	"BODY",
	"ENGINE",
	"LEG",
#if defined(_GBC)
	"ABIL",
	"ARMS",	
#else
	"ARMS",	
#endif

};

///배틀 카트 시스템 적용으로 파츠가 1개 추가
#if defined(_GBC)
const POINT c_ptPatEquipSlots[] = 
{
	{ 19,  79},
	{ 19, 125},
	{ 19, 171},
	{119,  79},
	{119, 125}
};
#else
const POINT c_ptPatEquipSlots[] = 
{
	{ 19,  68},
	{ 19, 114},
	{ 19, 160},
	{ 19, 206}
};
#endif

#define INV_SLOT_WIDTH_NUM	6



CItemDlg::CItemDlg(int iType)
{
	SetDialogType( iType );
	CTCommand* pCmd = NULL;

	m_iEquipTab		= 0;
	m_iInventoryTab = 0;
	m_iPatTab		= 0;

	m_iInventoryStorge = 3;

	iprevPane = 0;

	m_bMinimize		= false;
	m_pEquipDragItem = new CDragItem;
	pCmd = new CTCmdDragItemFromEquipInItemDlg;
	m_pEquipDragItem->AddTarget( iType, pCmd );
	m_pEquipDragItem->AddTarget( CDragItem::TARGET_ALL, NULL );///


	int iSlot = 0;

	CWinCtrl * pCtrl = NULL;


	m_pInvenDragItem = new CDragItem;

	pCmd = new CTCmdDragTakeinItem2UpgradeDlg;
	m_pInvenDragItem->AddTarget( DLG_TYPE_UPGRADE, pCmd );


	///분리/분해창으로 이동
	pCmd = new CTCmdDragItem2SeparateDlg;
	m_pInvenDragItem->AddTarget( DLG_TYPE_SEPARATE, pCmd );


	///퀵바로의 이동
	pCmd = new CTCmdDragInven2QuickBar;
	m_pInvenDragItem->AddTarget( DLG_TYPE_QUICKBAR, pCmd );

	///EX퀵바로의 이동
	pCmd = new CTCmdDragInven2QuickBar( DLG_TYPE_QUICKBAR_EXT );
	m_pInvenDragItem->AddTarget( DLG_TYPE_QUICKBAR_EXT, pCmd );

	///매매창으로의 이동
	CTCmdNumberInput* pNumberCmd = new CTCmdAddItem2DealFromInventory;
	CTCmdOpenNumberInputDlg* pOpenCmd = new CTCmdOpenNumberInputDlg;
	pOpenCmd->SetCommand( pNumberCmd );
	m_pInvenDragItem->AddTarget( DLG_TYPE_DEAL, pOpenCmd );

	///교환창으로 이동
	pNumberCmd  = new CTCmdAddMyItem2Exchange;
	pOpenCmd	= new CTCmdOpenNumberInputDlg;
	pOpenCmd->SetCommand( pNumberCmd );
	m_pInvenDragItem->AddTarget( DLG_TYPE_EXCHANGE, pOpenCmd );


	///상점창으로 이동
	pNumberCmd = new CTCmdSellItem;
	pOpenCmd   = new CTCmdOpenNumberInputDlg;
	pOpenCmd->SetCommand( pNumberCmd );
	m_pInvenDragItem->AddTarget( DLG_TYPE_STORE, pOpenCmd );

	///땅 혹은 채팅창으로 드랍
	///채팅창은 보이지는 않지만 일정영역을 가지고 있다.. 
	///채팅창에 걸쳐서 버려도 땅에 버리는 것을 간주한다
	pNumberCmd	= new CTCmdDropItem;
	pOpenCmd	= new CTCmdOpenNumberInputDlg;
	pOpenCmd->SetCommand( pNumberCmd );
	m_pInvenDragItem->AddTarget( CDragItem::TARGET_GROUND, pOpenCmd );



	pNumberCmd	= new CTCmdDropItem;
	pOpenCmd	= new CTCmdOpenNumberInputDlg;
	pOpenCmd->SetCommand( pNumberCmd );
	m_pInvenDragItem->AddTarget( DLG_TYPE_CHAT, pOpenCmd );

	///Bank로 이동
	pNumberCmd	= new CTCmdMoveItemInv2Bank;
	pOpenCmd	= new CTCmdOpenNumberInputDlg;
	pOpenCmd->SetCommand( pNumberCmd );
	m_pInvenDragItem->AddTarget( DLG_TYPE_BANK, pOpenCmd );

	///제조창으로 이동
	pCmd = new CTCmdTakeInItem2MakeDlg;
	m_pInvenDragItem->AddTarget( DLG_TYPE_MAKE, pCmd );


	///자신이 개설한 개인상점창으로
	pCmd = new CTCmdDragItem2PrivateStoreDlg;
	m_pInvenDragItem->AddTarget( DLG_TYPE_PRIVATESTORE, pCmd );

	///다른 아바타의 개인상점을 이용시
	CTCmdDragItem2AvatarStoreDlg* pOpenNumberInputDlg = new CTCmdDragItem2AvatarStoreDlg;
	CTCmdNumberInput* pCmd2 = new CTCmdSellItem2AvatarStore;
	pOpenNumberInputDlg->SetCommand( pCmd2 );
	m_pInvenDragItem->AddTarget( DLG_TYPE_AVATARSTORE, pOpenNumberInputDlg );

	///장착슬롯으로 이동시에는 장착 혹은 재밍등....
	pCmd = new CTCmdDragItemFromInvenInItemDlg;
	m_pInvenDragItem->AddTarget( iType, pCmd );

	m_pCmdDropMoney				= new CTCmdDropMoney;
	m_pCmdAddMyMoney2Exchange	= new CTCmdAddMyMoney2Exchange;
}

CItemDlg::~CItemDlg(void)
{
	SAFE_DELETE( m_pInvenDragItem );
	SAFE_DELETE( m_pEquipDragItem );
	SAFE_DELETE( m_pCmdDropMoney );
	SAFE_DELETE( m_pCmdAddMyMoney2Exchange );

}
void CItemDlg::Draw()
{
	if( !IsVision() )
		return;


	CTDialog::Draw();

	int iSlot = 0;
	if( m_iEquipTab == 1 )
	{
		if( !m_bMinimize) {
			for( iSlot = 0 ; iSlot < MAX_RIDING_PART; ++iSlot )
			{
				m_PatEquipSlots[iSlot].Draw();
			}
		}

		for( iSlot = 0 ; iSlot < INVENTORY_PAGE_SIZE; ++iSlot ) {
			m_ItemSlots[MAX_INV_TYPE-1][iSlot].Draw();
		}
	}
	else
	{
		if( !m_bMinimize)
		{
			for( iSlot = 0 ; iSlot < MAX_EQUIP_IDX-1; ++iSlot ) {
				m_AvatarEquipSlots[iSlot].Draw();
			}

			for( iSlot = 0 ; iSlot < MAX_SHOT_TYPE; ++iSlot ) {
				m_BulletEquipSlots[iSlot].Draw();
			}
		}


		if( m_iInventoryTab == 1 )
		{
			::endSprite();
			::beginSprite( D3DXSPRITE_ALPHABLEND ); 
		}


		for( iSlot = 0 ; iSlot < INVENTORY_PAGE_SIZE; ++iSlot ) {
			m_ItemSlots[m_iInventoryTab][iSlot].Draw();
		}
	}


	CWinCtrl * pCtrl = NULL;

	///무게
	if( pCtrl = Find("WEIGHT_VALUE") )
	{
		((CTImage*)pCtrl)->SetText( CStr::Printf("%d/%d", g_pAVATAR->GetCur_WEIGHT(), g_pAVATAR->GetCur_MaxWEIGHT() ) );
	}
	if( pCtrl = Find("WEIGHT_VALUE2") )
	{
		((CTImage*)pCtrl)->SetText( CStr::Printf("%d/%d", g_pAVATAR->GetCur_WEIGHT(), g_pAVATAR->GetCur_MaxWEIGHT() ) );
	}

	///돈	
	const int money_buffer_size = 64;
	char money_buffer[ money_buffer_size ];
	CGameUtil::ConvertMoney2String( g_pAVATAR->Get_MONEY(), money_buffer, money_buffer_size );

	if( pCtrl = Find("ZULIE_VALUE") )
	{
		((CTImage*)pCtrl)->SetText( money_buffer );		
	}
	if( pCtrl = Find("ZULIE_VALUE2") )
	{
		((CTImage*)pCtrl)->SetText( money_buffer );		
	}

	if( pCtrl = Find("ROW_VALUE") )
	{
		((CTImage*)pCtrl)->SetText( "" );
	}	

	CInventory* pInven = g_pAVATAR->GetInventory();	

	std::string strPatName;
	int iAttackPow			= 0;
	int iDefence			= 0;
	int iMagicResistance	= 0;
	int iFuelRate			= 0;
	int iAttackSpd			= 0;
	int iMoveSpeed			= 0;

	if( pInven->m_ItemRIDE[ RIDE_PART_BODY ].GetItemNO() == PAT_ITEM_PART_TYPE_CART )
	{
		strPatName = CStringManager::GetSingleton().GetString( 732 ); //카트
	}
	else
	{
		strPatName = CStringManager::GetSingleton().GetString( 733 ); //캐슬기어
	}

	for( int i=0; i<MAX_RIDING_PART; i++ )
	{
		//공격력
		iAttackPow += PAT_ITEM_ATK_POW( pInven->m_ItemRIDE[ i ].GetItemNO() );

		for(int j=0; j<PAT_ITEM_ADD_ABILITY_CNT; j++)
		{
			//방어력
			if( PAT_ITEM_ADD_ABILITY_IDX( j, pInven->m_ItemRIDE[ i ].GetItemNO() ) == AT_DEF )
			{
				iDefence += PAT_ITEM_ADD_ABILITY_VALUE( j, pInven->m_ItemRIDE[ i ].GetItemNO() );	
			}
			//마법저항
			if( PAT_ITEM_ADD_ABILITY_IDX( j, pInven->m_ItemRIDE[ i ].GetItemNO() ) == AT_RES )
			{
				iMagicResistance += PAT_ITEM_ADD_ABILITY_VALUE( j, pInven->m_ItemRIDE[ i ].GetItemNO() );	
			}			
		}

		//연료소비
		iFuelRate += PAT_ITEM_USE_FUEL_RATE( pInven->m_ItemRIDE[ i ].GetItemNO() );
		//공격속도
		iAttackSpd += PAT_ITEM_ATK_SPD( pInven->m_ItemRIDE[ i ].GetItemNO() );
		//이동속도
		iMoveSpeed += PAT_ITEM_MOV_SPD( pInven->m_ItemRIDE[ i ].GetItemNO() );
	}	

	if( pCtrl = Find("STR_PAT_POS_01") )
	{
		((CTImage*)pCtrl)->SetText( CStr::Printf( "%s", strPatName.c_str()  ) );
	}	

	if( pCtrl = Find("STR_PAT_POS_03") )
	{
		((CTImage*)pCtrl)->SetText( CStr::Printf( "%d", iAttackPow ) );
	}	
	if( pCtrl = Find("STR_PAT_POS_05") )
	{
		((CTImage*)pCtrl)->SetText( CStr::Printf( "%d", iDefence ) );
	}
	if( pCtrl = Find("STR_PAT_POS_07") )
	{
		((CTImage*)pCtrl)->SetText( CStr::Printf( "%d", iMagicResistance ) );
	}
	if( pCtrl = Find("STR_PAT_POS_09") )
	{
		((CTImage*)pCtrl)->SetText( CStr::Printf( "%d", iFuelRate ) );
	}
	if( pCtrl = Find("STR_PAT_POS_11") )
	{
		((CTImage*)pCtrl)->SetText( CStr::Printf( "%d", iAttackSpd ) );
	}
	if( pCtrl = Find("STR_PAT_POS_13") )
	{
		((CTImage*)pCtrl)->SetText( CStr::Printf( "%d", iMoveSpeed ) );
	}

}

void CItemDlg::OnLButtonUp( unsigned uiProcID, WPARAM wParam, LPARAM lParam )
{
	switch( uiProcID )
	{
	case IID_BTN_MAXIMIZE:
		Maximize();
		break;
	case IID_BTN_MINIMIZE:
		Minimize();
		break;
	case IID_BTN_ICONIZE:
		g_itMGR.AddDialogIcon( 	GetDialogType() );
		break;
	case IID_BTN_CLOSE:
		Hide();
		break;
	case IID_BTN_MONEY:
		{
			int iMaxDropMoney = 0;		

			if( g_itMGR.IsDlgOpened( DLG_TYPE_BANK ) )
			{
				//g_itMGR.OpenMsgBox( STR_CANT_STORE_MONEY2BANK );
			}
			else if( g_itMGR.IsDlgOpened( DLG_TYPE_EXCHANGE ) )
			{
				CTCmdOpenNumberInputDlg OpenCmd;
				OpenCmd.SetCommand( m_pCmdAddMyMoney2Exchange );
				OpenCmd.SetMaximum( g_pAVATAR->Get_MONEY() - CExchange::GetInstance().GetMyTradeMoney() );
				OpenCmd.Exec( NULL );

			}
			else
			{				
				CTCmdOpenNumberInputDlg OpenCmd;
				OpenCmd.SetCommand( m_pCmdDropMoney );

				__int64 i64MaxDropMoney;
				if( MAX_DROP_MONEY >= g_pAVATAR->Get_MONEY() )
					i64MaxDropMoney = g_pAVATAR->Get_MONEY();
				else
					i64MaxDropMoney = MAX_DROP_MONEY;

				OpenCmd.SetMaximum( i64MaxDropMoney );
				OpenCmd.Exec( NULL );
			}

			break;
		}
	default:
		break;
	}
}

void CItemDlg::SetEquipTab(int iIndex)
{

	m_iEquipTab = iIndex;

	CWinCtrl* pCtrl;
	if( pCtrl = FindChildInPane( IID_PANE_INVEN, IID_TABBEDPANE_INVEN_PAT ) )
	{
		if( m_iEquipTab == INVEN_ITEM )
		{
			pCtrl->Hide();
		}
		else
		{
			pCtrl->Show();
		}		
	}
	if( pCtrl = FindChildInPane( IID_PANE_INVEN, IID_TABBEDPANE_INVEN_ITEM ) )
	{
		if( m_iEquipTab == INVEN_ITEM)
		{
			pCtrl->Show();
		}
		else
		{
			pCtrl->Hide();
		}		
	}	

	for( int iSlot = 0 ; iSlot < MAX_RIDING_PART; ++iSlot )
		m_PatEquipSlots[iSlot].MoveWindow( m_sPosition );

	for( int iSlot = 0; iSlot < INVENTORY_PAGE_SIZE; ++iSlot )
		m_ItemSlots[3][iSlot].MoveWindow( m_sPosition );


	pCtrl = Find( IID_EQUIP_PAB );
	if( pCtrl && pCtrl->GetControlType() == CTRL_TABBEDPANE )
	{
		CTabbedPane* pPane = (CTabbedPane*)pCtrl;
		pPane->SetSelectedIndex( m_iEquipTab );
	}	

	MoveWindow( m_sPosition );
}

void CItemDlg::OnLButtonDown( unsigned uiProcID, WPARAM wParam, LPARAM lParam )
{
	switch( uiProcID )
	{
	case IID_BTN_EQUIP_PAT:
		{
			SetEquipTab( 1 );
			break;
		}
	case IID_BTN_EQUIP_AVATAR:
		{
			SetEquipTab( 0 );
			break;
		}
	case IID_BTN_INVEN_EQUIP:
		m_iInventoryStorge = 3;
		m_iInventoryTab =  0;

		if(CDragNDropMgr::GetInstance().IsDraging() && iprevPane != m_iInventoryStorge)
			CDragNDropMgr::GetInstance().DragCancel();

		for( int iSlot = 0; iSlot < INVENTORY_PAGE_SIZE; ++iSlot )
			m_ItemSlots[m_iInventoryTab][iSlot].MoveWindow( m_sPosition );

		break;
	case IID_BTN_INVEN_USE:
		m_iInventoryStorge = 2;
		m_iInventoryTab = 1;

		if(CDragNDropMgr::GetInstance().IsDraging() && iprevPane != m_iInventoryStorge)
			CDragNDropMgr::GetInstance().DragCancel();

		for( int iSlot = 0; iSlot < INVENTORY_PAGE_SIZE; ++iSlot )
			m_ItemSlots[m_iInventoryTab][iSlot].MoveWindow( m_sPosition );

		break;
	case IID_BTN_INVEN_ETC:
		m_iInventoryStorge = 1;
		m_iInventoryTab = 2;

		if(CDragNDropMgr::GetInstance().IsDraging() && iprevPane != m_iInventoryStorge)
			CDragNDropMgr::GetInstance().DragCancel();

		for( int iSlot = 0; iSlot < INVENTORY_PAGE_SIZE; ++iSlot )
			m_ItemSlots[m_iInventoryTab][iSlot].MoveWindow( m_sPosition );
		/*
		case IID_BTN_INVEN_MILEAGE:
		m_iInventoryTab = 3;
		for( int iSlot = 0; iSlot < INVENTORY_PAGE_SIZE; ++iSlot )
		{
		m_ItemSlots[m_iInventoryTab][iSlot].MoveWindow( m_sPosition );
		}
		break;

		case IID_BTN_PAT_EQUIP:
		m_iPatTab = 0;
		for( int iSlot = 0; iSlot < INVENTORY_PAGE_SIZE; ++iSlot )
		{
		m_ItemSlots[m_iPatTab+INV_TYPE_SIZE][iSlot].MoveWindow( m_sPosition );
		}
		break;

		case IID_BTN_PAT_ETC:
		m_iPatTab = 1;
		for( int iSlot = 0; iSlot < INVENTORY_PAGE_SIZE; ++iSlot )
		{
		m_ItemSlots[m_iPatTab+INV_TYPE_SIZE][iSlot].MoveWindow( m_sPosition );
		}
		break;

		case IID_BTN_PAT_MILEAGE:
		m_iPatTab = 2;
		for( int iSlot = 0; iSlot < INVENTORY_PAGE_SIZE; ++iSlot )
		{
		m_ItemSlots[m_iPatTab+INV_TYPE_SIZE][iSlot].MoveWindow( m_sPosition );
		}
		break;
		*/
	default:
		break;
	}
}

unsigned CItemDlg::Process( unsigned uiMsg, WPARAM wParam, LPARAM lParam )
{
	if( !IsVision() ) return 0;



	CWinCtrl* pCtrl;
	pCtrl = Find( IID_TABBEDPANE_INVEN_ITEM );
	if( pCtrl && pCtrl->GetControlType() == CTRL_TABBEDPANE )
	{
		CTabbedPane* pPane = (CTabbedPane*)pCtrl;
		iprevPane = pPane->GetSelectedIndex();
	}

	
	if( unsigned iProcID = CTDialog::Process( uiMsg, wParam, lParam ) )
	{
		if( ProcessSlots( uiMsg, wParam, lParam ) )
			return uiMsg;
		
		switch( uiMsg )
		{
		case WM_LBUTTONDOWN:
			OnLButtonDown( iProcID, wParam , lParam );

			if( !IsInside4Move(LOWORD( lParam ), HIWORD( lParam ) ) )
			{
				return false;
			}

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
void CItemDlg::Show()
{
	CTDialog::Show();

	CWinCtrl* pCtrl = NULL;
	CWinCtrl* pCtrl2 = NULL;

	SetEquipTab( m_iEquipTab );

	if( m_bMinimize )
	{
		Minimize();
	}
	else
	{
		Maximize();
	}
}

// 아이템이 들어 있는 슬롯값 찾기.
// 없을경우 -1.
// param :  stb 아이템 인덱스.
CSlot * CItemDlg::SearchItem_Inventory( int iIndex )
{	
	CIcon* pIcon = NULL;
	for( int iType=0; iType<MAX_INV_TYPE; ++iType )
	{
		for( int iSlot = 0; iSlot < INVENTORY_PAGE_SIZE; ++iSlot )
		{
			if( pIcon = m_ItemSlots[ iType ][ iSlot ].GetIcon() )
			{
				if( g_pAVATAR->Get_InventoryITEM( pIcon->GetIndex() ).GetItemNO() == iIndex )
				{
                    return &m_ItemSlots[ iType ][ iSlot ];
				}
			}
		}	
	}	
	return NULL;
}

void CItemDlg::Update( CObservable* pObservable, CTObject* pObj )
{
	assert( pObservable );
	if( pObj == NULL || strcmp( pObj->toString(), "CTEventItem") )
	{
		assert( 0 && "CTEvent is NULL or Invalid" );	
		return;
	}

	CTEventItem* pEvent = (CTEventItem*)pObj;
	int iIndex = pEvent->GetIndex();
	switch( pEvent->GetID() )
	{
	case CTEventItem::EID_ADD_ITEM:
		{
			CItem* pItem = pEvent->GetItem();

			if( iIndex >= 1	&& iIndex < MAX_EQUIP_IDX )
				m_AvatarEquipSlots[ iIndex - 1].AttachIcon( pItem->CreateItemIcon() );
			else if(  iIndex >= INVENTORY_SHOT_ITEM0 && iIndex < INVENTORY_SHOT_ITEM0 + MAX_SHOT_TYPE )
				m_BulletEquipSlots[ iIndex - INVENTORY_SHOT_ITEM0 ].AttachIcon( pItem->CreateItemIcon() );
			else if( iIndex >= INVENTORY_RIDE_ITEM0 && iIndex < INVENTORY_RIDE_ITEM0 + MAX_RIDING_PART )
				m_PatEquipSlots[ iIndex - INVENTORY_RIDE_ITEM0 ].AttachIcon( pItem->CreateItemIcon() );
			else if( iIndex >= INVENTORY_ITEM_INDEX_0 && iIndex <= INVENTORY_ITEM_INDEX_LAST )
			{
				int iInvenSlotIndex = iIndex - INVENTORY_ITEM_INDEX_0;
				int iType = iInvenSlotIndex / INVENTORY_PAGE_SIZE;
				for( int iSlot = 0; iSlot < INVENTORY_PAGE_SIZE; ++iSlot )
				{
					if( m_ItemSlots[iType][iSlot].GetIcon() == NULL )
					{
						m_ItemSlots[iType][iSlot].AttachIcon( pItem->CreateItemIcon() );
						break;
					}
				}
			}
			break;
		}
	case CTEventItem::EID_DEL_ITEM:
		{
			if( iIndex >= 1	&& iIndex < MAX_EQUIP_IDX )
				m_AvatarEquipSlots[ iIndex - 1].DetachIcon();
			else if(  iIndex >= INVENTORY_SHOT_ITEM0 && iIndex < INVENTORY_SHOT_ITEM0 + MAX_SHOT_TYPE )
				m_BulletEquipSlots[ iIndex - INVENTORY_SHOT_ITEM0 ].DetachIcon();
			else if( iIndex >= INVENTORY_RIDE_ITEM0 && iIndex < INVENTORY_RIDE_ITEM0 + MAX_RIDING_PART )
				m_PatEquipSlots[ iIndex - INVENTORY_RIDE_ITEM0 ].DetachIcon();
			else if( iIndex >= INVENTORY_ITEM_INDEX_0 && iIndex <= INVENTORY_ITEM_INDEX_LAST )
			{
				int iInvenSlotIndex = iIndex - INVENTORY_ITEM_INDEX_0;
				int iType = iInvenSlotIndex / INVENTORY_PAGE_SIZE;
				CIcon* pIcon = NULL;
				for( int iSlot = 0; iSlot < INVENTORY_PAGE_SIZE; ++iSlot )
				{
					if( pIcon = m_ItemSlots[ iType ][ iSlot ].GetIcon() )
					{
						if( pIcon->GetIndex() == iIndex )
						{
							m_ItemSlots[ iType ][ iSlot ].DetachIcon();				
							break;
						}
					}
				}
			}
			break;
		}
	default:
		break;
	}


}
void CItemDlg::MoveWindow( POINT pt )
{
	CTDialog::MoveWindow( pt );
	int iSlot;


	if( m_iEquipTab == 1 )
	{
		for( iSlot = 0 ; iSlot < MAX_RIDING_PART; ++iSlot )
			m_PatEquipSlots[iSlot].MoveWindow( m_sPosition );

		for( int iSlot = 0; iSlot < INVENTORY_PAGE_SIZE; ++iSlot )
			m_ItemSlots[MAX_INV_TYPE-1][iSlot].MoveWindow( m_sPosition );
	}
	else
	{
		for( iSlot = 0 ; iSlot < MAX_EQUIP_IDX-1; ++iSlot )
			m_AvatarEquipSlots[iSlot].MoveWindow( m_sPosition );

		for( iSlot = 0 ; iSlot < MAX_SHOT_TYPE; ++iSlot )
			m_BulletEquipSlots[iSlot].MoveWindow( m_sPosition );

		for( iSlot = 0; iSlot < INVENTORY_PAGE_SIZE; ++iSlot )
			m_ItemSlots[m_iInventoryTab][iSlot].MoveWindow( m_sPosition );
	}

}
void CItemDlg::Update( POINT ptMouse )
{
	if( !IsVision() ) return;
	CTDialog::Update( ptMouse );

	int iSlot = 0;
	if( m_iEquipTab == 1 )
	{
		if( !m_bMinimize )
			for( iSlot = 0 ; iSlot < MAX_RIDING_PART; ++iSlot )
				m_PatEquipSlots[iSlot].Update( ptMouse );

		for( iSlot = 0 ; iSlot < INVENTORY_PAGE_SIZE; ++iSlot )
			m_ItemSlots[MAX_INV_TYPE-1][iSlot].Update(ptMouse );
	}
	else
	{
		if( !m_bMinimize )
		{
			for( iSlot = 0 ; iSlot < MAX_EQUIP_IDX-1; ++iSlot )
				m_AvatarEquipSlots[iSlot].Update( ptMouse );

			for( iSlot = 0 ; iSlot < MAX_SHOT_TYPE; ++iSlot )
				m_BulletEquipSlots[iSlot].Update( ptMouse );
		}

		for( iSlot = 0 ; iSlot < INVENTORY_PAGE_SIZE; ++iSlot )
			m_ItemSlots[m_iInventoryTab][iSlot].Update( ptMouse );
	}

}

bool CItemDlg::ProcessSlots( unsigned uiMsg, WPARAM wParam, LPARAM lParam )
{
	int iSlot;
	if( m_iEquipTab == 1 )
	{
		if( !m_bMinimize )
			for( iSlot = 0 ; iSlot < MAX_RIDING_PART; ++iSlot )
				if( m_PatEquipSlots[iSlot].Process( uiMsg, wParam , lParam ) )
					return true;

		for( iSlot = 0 ; iSlot < INVENTORY_PAGE_SIZE; ++iSlot )
			if( m_ItemSlots[MAX_INV_TYPE-1][iSlot].Process( uiMsg, wParam, lParam ) )
				return true;
	}
	else
	{
		if( !m_bMinimize )
		{
			for( iSlot = 0 ; iSlot < MAX_EQUIP_IDX-1; ++iSlot )
				if( m_AvatarEquipSlots[iSlot].Process( uiMsg, wParam, lParam) )
					return true;

			for( iSlot = 0 ; iSlot < MAX_SHOT_TYPE; ++iSlot )
				if( m_BulletEquipSlots[iSlot].Process( uiMsg, wParam, lParam) )
					return true;
		}

		for( iSlot = 0 ; iSlot < INVENTORY_PAGE_SIZE; ++iSlot )
			if( m_ItemSlots[m_iInventoryTab][iSlot].Process( uiMsg, wParam,lParam) )
				return true;
	}
	return false;
}


bool CItemDlg::IsInsideInven( POINT pt )
{
	CWinCtrl * pCtrl = NULL;
	RECT rc = { 10, 290, 220, 530 };

	if( pCtrl = Find("INVENTORY_PANE") )
	{
		pCtrl->GetOffset().x;
		SetRect( &rc,
			pCtrl->GetOffset().x, pCtrl->GetOffset().y,
			pCtrl->GetOffset().x+pCtrl->GetWidth(), pCtrl->GetOffset().y+pCtrl->GetHeight() );
	}
	POINT ptTemp = { pt.x - m_sPosition.x, pt.y - m_sPosition.y };

	return PtInRect( &rc, ptTemp )?true:false;
}

bool CItemDlg::IsInsideEquip( POINT pt )
{
	if( m_bMinimize )
		return false;

	CWinCtrl * pCtrl = NULL;
	RECT rc = { 10, 60, 220, 250 };

	if( pCtrl = Find("EQUIP_PANE") )
	{
        pCtrl->GetOffset().x;
		SetRect( &rc,
				pCtrl->GetOffset().x, pCtrl->GetOffset().y,
				pCtrl->GetOffset().x+pCtrl->GetWidth(), pCtrl->GetOffset().y+pCtrl->GetHeight() );
	}	
	POINT ptTemp = { pt.x - m_sPosition.x, pt.y - m_sPosition.y };
	return PtInRect( &rc, ptTemp )?true:false;
}

//----------------------------------------------------------------------------------------------------
/// @param
/// @brief 현재마우스 위치로 장비 장착창 슬롯 번호를 구한다.
/// @return < 0 이라면 적당한 장비장착창 위가 아님..
//----------------------------------------------------------------------------------------------------

int CItemDlg::GetEquipSlot( POINT pt )
{
	if( m_bMinimize )
		return -1;

	CWinCtrl * pCtrl = NULL;
	RECT rt;
	POINT ptTemp = { pt.x - m_sPosition.x, pt.y - m_sPosition.y };

	for( int iSlot = 0 ; iSlot < MAX_EQUIP_IDX-1; ++iSlot )
	{
		if( pCtrl = Find(c_strEquipedSlotName[iSlot].c_str()) )
		{
			SetRect( &rt,
					pCtrl->GetOffset().x, pCtrl->GetOffset().y,
					pCtrl->GetOffset().x+pCtrl->GetWidth(), pCtrl->GetOffset().y+pCtrl->GetHeight() );

			if( PtInRect( &rt, ptTemp ) )
			{
				return iSlot + 1;
			}
		}		
	}

	return -1;
}

CSlot*	CItemDlg::GetInvenSlot( POINT pt )
{
	int iInvenType = 0;


	if( m_iEquipTab == 1 )
	{
		iInvenType = MAX_INV_TYPE-1;
	}
	else
	{
		iInvenType = m_iInventoryTab;
	}


	for( int iSlot = 0 ; iSlot < INVENTORY_PAGE_SIZE; ++iSlot )
	{
		if( m_ItemSlots[iInvenType][iSlot].IsInside( pt.x, pt.y ) )
		{
			return &(m_ItemSlots[iInvenType][iSlot]);
		}
	}

	return NULL;
}
CWinCtrl* CItemDlg::FindChildInPane( unsigned uiPaneID, unsigned uiChildID )
{
	CWinCtrl* pCtrl = Find( uiPaneID );
	if( pCtrl && pCtrl->GetControlType() == CTRL_PANE )
	{
		CTPane* pPane = (CTPane*)pCtrl;
		return pPane->FindChild( uiChildID );
	}
	return NULL;
}

void CItemDlg::Minimize()
{
	CWinCtrl* pCtrl = NULL;
	CWinCtrl* pCtrl2 = NULL;

	m_bMinimize = true;

	//확대 축소 버튼 온, 오프.
	if( pCtrl = FindChildInPane( IID_PANE_INVEN, IID_BTN_MINIMIZE ) )
		pCtrl->Hide();

	if( pCtrl = FindChildInPane( IID_PANE_INVEN, IID_BTN_MAXIMIZE ) )
		pCtrl->Show();

	if( pCtrl = Find( IID_PANE_EQUIP ) )
	{
		pCtrl->Hide();
	}


	if( pCtrl = Find( "INVENTORY_PANE" ) )
	{
		GetCaption()->SetOffset( pCtrl->GetOffset() );
	}
	

	//축소되었을때 무게, 돈 보여주는 컨트롤 Show
	if( pCtrl = Find( "MINI_VALUE_PAN" ) )
	{
		pCtrl->Show();
	}

	if( pCtrl = Find( "SEPARATE_BAR" ) )
	{
		pCtrl->Show();
	}

	if( pCtrl = Find( "MARK" ) )
	{
		if( pCtrl2 = Find( "INV_BASE_POS" ) )
		{
			pCtrl->SetOffset( pCtrl2->GetOffset().x, pCtrl2->GetOffset().y );
		}				
	}	
	MoveWindow( m_sPosition );
}

void CItemDlg::Maximize()
{
	CWinCtrl* pCtrl = NULL;
	CWinCtrl* pCtrl2 = NULL;
	m_bMinimize = false;

	if( pCtrl = FindChildInPane( IID_PANE_INVEN, IID_BTN_MINIMIZE ) )
		pCtrl->Show();

	if( pCtrl = FindChildInPane( IID_PANE_INVEN, IID_BTN_MAXIMIZE ) )
		pCtrl->Hide();

	if( pCtrl = Find( IID_PANE_EQUIP ) )
	{
		pCtrl->Show();	
	}


	if( pCtrl = Find( "INVENTORY_PANE" ) )
	{
		GetCaption()->SetOffset( 0, 0 );
	}


	//축소되었을때 무게, 돈 보여주는 컨트롤 Hide
	if( pCtrl = Find( "MINI_VALUE_PAN" ) )
	{
		pCtrl->Hide();
	}
	if( pCtrl = Find( "SEPARATE_BAR" ) )
	{
		pCtrl->Hide();
	}

	if( pCtrl = Find( "MARK" ) )
	{
		if( pCtrl2 = Find( "INV_BASE_POS" ) )
		{
			pCtrl->SetOffset( 0, 0 );
		}				
	}

	MoveWindow( m_sPosition );
}

void CItemDlg::SwitchIcon( int iReal, int iVirtual )
{
	if( iReal == 0 ) return;

	//int iRealType = iReal / INVENTORY_PAGE_SIZE;
	//int iRealSlot = iReal % INVENTORY_PAGE_SIZE;

	int iVirtualType = iVirtual / INVENTORY_PAGE_SIZE;
	int iVirtualSlot = iVirtual % INVENTORY_PAGE_SIZE;

	//assert( iRealType == iVirtualType );
	//if( iRealType != iVirtualType )
	//	return;

	CSlot* pSlot = GetInvenSlotByRealIndex( iReal );

	if( pSlot && pSlot->GetIcon() )
	{
		CSlot* pTargetSlot = &(m_ItemSlots[iVirtualType][iVirtualSlot]);
		if( pTargetSlot != pSlot )
		{
			CIconItem* pItemIcon = (CIconItem*)pSlot->GetIcon();
			if( CIcon* pTargetIcon = pTargetSlot->GetIcon() )
			{
				CItem* pTempItem = ((CIconItem*)pTargetIcon)->GetCItem();
				assert( pTempItem );
				pTargetSlot->DetachIcon();

				CItem* pSourceItem = pItemIcon->GetCItem();
				assert( pSourceItem );
				pSlot->DetachIcon();

				pTargetSlot->AttachIcon( pSourceItem->CreateItemIcon() );
				pSlot->AttachIcon( pTempItem->CreateItemIcon() );
			}
			else
			{
				CItem* pItem = pItemIcon->GetCItem();
				pSlot->DetachIcon();
				pTargetSlot->AttachIcon( pItem->CreateItemIcon() );
			}
		}
	}
}

//*-----------------------------------------------------------------------
/// @brief 실제 인벤토리인덱스를 가진 아이템 아이콘을 가진 Slot을 찾는다.
//*-----------------------------------------------------------------------
CSlot*	CItemDlg::GetInvenSlotByRealIndex( int iIndex )
{
	CIcon* pIcon = NULL;
	for( int iType = 0 ; iType < MAX_INV_TYPE ; ++iType )
	{
		for( int iSlot = 0 ; iSlot < INVENTORY_PAGE_SIZE; ++iSlot )
		{
			if( pIcon = m_ItemSlots[iType][iSlot].GetIcon() )
			{
				if( pIcon->GetIndex() == iIndex )
					return &(m_ItemSlots[iType][iSlot]);
			}
		}
	}
	return NULL;
}

void CItemDlg::ApplySavedVirtualInventory( std::list<S_InventoryData>& list )
{
	std::list<S_InventoryData>::iterator iter;
	for( iter = list.begin(); iter != list.end(); ++iter )
		SwitchIcon( iter->lRealIndex, iter->lVirtualIndex );
}
void CItemDlg::GetVirtualInventory( std::list<S_InventoryData>& list )
{
	list.clear();
	S_InventoryData Data;
	CIcon* pIcon;
	for( int iType  = 0;  iType < MAX_INV_TYPE; ++iType )
	{
		for( int iSlot = 0 ; iSlot < INVENTORY_PAGE_SIZE; ++iSlot )
		{
			if( pIcon = m_ItemSlots[iType][iSlot].GetIcon() )
			{
				Data.lRealIndex    = pIcon->GetIndex();
				Data.lVirtualIndex = iType * INVENTORY_PAGE_SIZE + iSlot;
				list.push_back( Data );
			}
		}
	}
}

unsigned	 CItemDlg::ActionPerformed( CActionEvent* e )
{
	assert( e );
	if( e == NULL ) return 0;
	switch( e->GetID() )
	{
	case WM_LBUTTONDOWN:
		{
			CWinCtrl* pSource = e->GetSource();
			assert( pSource );
			if( pSource == NULL ) break;

			unsigned wLParam = e->GetLParam();
			POINT ptMouse = { LOWORD( wLParam ), HIWORD( wLParam ) };
			if( !pSource->IsInside( ptMouse.x, ptMouse.y ) )
				break;

			switch( pSource->GetControlType() )
			{
			case CTRL_SLOT:
				{
					CSlot* pSlot = (CSlot*)pSource;
					if( CIcon* pIcon = pSlot->GetIcon() )
					{
						switch( g_itMGR.GetState() )
						{
						case IT_MGR::STATE_MAKESOCKET:
							{
								// 소켓  생성
								CGame& refGame = CGame::GetInstance();
								if( refGame.GetMakeSocketMode() != CGame::MAKESOCKET_NONE ) /// 소켓 생성 모드일경우
								{
									pSlot->ResetClicked();

									CIconItem* pItemIcon =( CIconItem*) pIcon;

									if(IsAvailableMakeSocket( pIcon ))
									{
										switch( refGame.GetMakeSocketMode() )
										{
										case CGame::MAKESOCKET_ITEM:
											{
												g_pNet->Send_cli_CRAFT_DRILL_SOCKET_REQ(CRAFT_DRILL_SOCKET_FROM_ITEM,refGame.GetUsingMakeSocketItemInvenIdx(), pItemIcon->GetIndex());
												return e->GetID();	
												break;
											}
										case CGame::MAKESOCKET_NPC:
											{
												//												g_pNet->Send_cli_REPAIR_FROM_NPC( refGame.GetRepairNpcSvrIdx(), pItemIcon->GetIndex());
												//												CTCommand* pCmd = new CTCmdEndRepair;
												//												g_itMGR.AddTCommand( DLG_TYPE_MAX, pCmd );
												//												return e->GetID();
												break;
											}
										default:
											break;
										}
									}

								}
							}
							break;
						case IT_MGR::STATE_REPAIR:
							{
								CGame& refGame = CGame::GetInstance();
								if( refGame.GetRepairMode() != CGame::REPAIR_NONE  )///수리모드일경우
								{
									pSlot->ResetClicked();
									if( IsAvailableRepair( pIcon ) )
									{
										CIconItem* pItemIcon =( CIconItem*) pIcon;
										switch( refGame.GetRepairMode() )
										{
										case CGame::REPAIR_ITEM:
											{
												g_pNet->Send_cli_USE_ITEM_TO_REPAIR( refGame.GetUsingRepairItemInvenIdx(), pItemIcon->GetIndex() );
												//												CTCommand* pCmd = new CTCmdEndRepair;
												//												g_itMGR.AddTCommand( DLG_TYPE_MAX, pCmd );
												return e->GetID();
												break;
											}
										case CGame::REPAIR_NPC:
											{
												g_pNet->Send_cli_REPAIR_FROM_NPC( refGame.GetRepairNpcSvrIdx(), pItemIcon->GetIndex());
												//												CTCommand* pCmd = new CTCmdEndRepair;
												//												g_itMGR.AddTCommand( DLG_TYPE_MAX, pCmd );
												return e->GetID();
												break;
											}
										default:
											break;
										}
									}
								}
								break;
							}
						case IT_MGR::STATE_APPRAISAL:
							{
								pSlot->ResetClicked();
								CIconItem* pItemIcon =( CIconItem*) pIcon;
								tagITEM& Item = pItemIcon->GetItem();
								if( Item.IsEnableAppraisal() )
								{
									__int64 i64Price = (ITEM_BASE_PRICE( Item.GetTYPE(), Item.GetItemNO() ) + 10000 ) * ( Item.GetDurability() + 50 ) / 10000;
									if( i64Price <= g_pAVATAR->Get_MONEY() )
									{
										CGame::GetInstance().SetAppraisalCost( i64Price );
										CTCommand* pCmd = new CTCmdSendAppraisalReq( pItemIcon->GetIndex() );
										g_itMGR.OpenMsgBox(CStr::Printf( STR_MSG_IDENTIFY_COST, pItemIcon->GetName(), i64Price ),  
											CMsgBox::BT_OK | CMsgBox::BT_CANCEL, 
											true,
											0,
											pCmd );
									}
									else
									{
										g_itMGR.OpenMsgBox( STR_NOTENOUGH_IDENTIFYCOST );
									}
								}
								break;
							}
						default:
							break;
						}
					}
					break;
				}
			default:
				break;
			}
			break;
		}
	default:
		break;
	}
	return 0;
}

bool CItemDlg::IsAvailableMakeSocket( CIcon* pIcon )
{
	assert( pIcon );
	if( pIcon == NULL ) return false;

	CIconItem* pItemIcon = (CIconItem*)pIcon;
	tagITEM& Item = pItemIcon->GetItem();

	POINT ptMouse;
	CGame::GetInstance().Get_MousePos(ptMouse);

	if(0 < GetEquipSlot(ptMouse))
	{
		g_itMGR.OpenMsgBox( STR_NO_USE_EQUIPITEM );
		return false;
	}


	int a = Item.GetTYPE();
	if( Item.GetTYPE() == ITEM_TYPE_JEWEL ||			//반지
		Item.GetTYPE() == ITEM_TYPE_WEAPON ||		//무기
		Item.GetTYPE() == ITEM_TYPE_ARMOR )			//갑옷
	{
		return true;
	}


	return false;
}

bool CItemDlg::IsAvailableRepair( CIcon* pIcon )
{
	assert( pIcon );
	if( pIcon == NULL ) return false;

	CIconItem* pItemIcon = (CIconItem*)pIcon;
	/*const*/ tagITEM& Item = pItemIcon->GetItem();

	if( Item.GetTYPE() != ITEM_TYPE_RIDE_PART && !Item.IsEquipITEM() )
		return false;

	if( Item.GetLife() >= 1000 )
		return false;

	if( ITEM_TYPE( Item.GetTYPE(), Item.GetItemNO() ) == TUNING_PART_ENGINE_CART ||
		ITEM_TYPE( Item.GetTYPE(), Item.GetItemNO() ) == TUNING_PART_ENGINE_CASTLEGEAR )
		return false;

	if( Item.GetDurability() <= 0 )
	{
		g_itMGR.AppendChatMsg(STR_CANT_REPAIR_DURABILITY_IS_ZERO, IT_MGR::CHAT_TYPE_SYSTEM );	
		return false;
	}

	return true;
}

void CItemDlg::AddActionEventListener2Slots( )
{
	int iIndex = 0;

	for( iIndex = 0 ; iIndex < MAX_EQUIP_IDX-1 ; ++iIndex )
		m_AvatarEquipSlots[iIndex].AddActionListener( this );

	for( int iIndex = 0; iIndex  < MAX_RIDING_PART; ++iIndex )
		m_PatEquipSlots[iIndex].AddActionListener( this );

	for( int iType = 0; iType < MAX_INV_TYPE; ++iType )
		for( int iSlot = 0; iSlot < INVENTORY_PAGE_SIZE; ++iSlot )
			m_ItemSlots[iType][iSlot].AddActionListener( this );
}

void CItemDlg::RemoveActionEventListener2Slots( )
{
	int iIndex = 0;
	for( iIndex = 0 ; iIndex < MAX_EQUIP_IDX-1 ; ++iIndex )
		m_AvatarEquipSlots[iIndex].RemoveActionListener( this );

	for( int iIndex = 0; iIndex  < MAX_RIDING_PART; ++iIndex )
		m_PatEquipSlots[iIndex].RemoveActionListener( this );

	for( int iType = 0; iType < MAX_INV_TYPE; ++iType )
		for( int iSlot = 0; iSlot < INVENTORY_PAGE_SIZE; ++iSlot )
			m_ItemSlots[iType][iSlot].RemoveActionListener( this );
}

void CItemDlg::SetInterfacePos_After()
{
	CWinCtrl * pCtrl = NULL;
	///무게
	if( pCtrl = Find("WEIGHT_VALUE") )
	{
		((CTImage*)pCtrl)->SetFont(FONT_SMALL);
		((CTImage*)pCtrl)->SetAlign( DT_VCENTER | DT_RIGHT );
		((CTImage*)pCtrl)->SetTextColor( D3DCOLOR_XRGB( 0, 210, 255 ) );	
	}
	if( pCtrl = Find("WEIGHT_VALUE2") )
	{
		((CTImage*)pCtrl)->SetFont(FONT_SMALL);
		((CTImage*)pCtrl)->SetAlign( DT_VCENTER | DT_RIGHT );
		((CTImage*)pCtrl)->SetTextColor( D3DCOLOR_XRGB( 0, 210, 255 ) );	
	}
	if( pCtrl = Find("ZULIE_VALUE") )
	{
		((CTImage*)pCtrl)->SetFont(FONT_SMALL);
		((CTImage*)pCtrl)->SetAlign( DT_VCENTER | DT_RIGHT );
		((CTImage*)pCtrl)->SetTextColor( D3DCOLOR_XRGB( 255, 240, 0 ) );	
	}
	if( pCtrl = Find("ZULIE_VALUE2") )
	{
		((CTImage*)pCtrl)->SetFont(FONT_SMALL);
		((CTImage*)pCtrl)->SetAlign( DT_VCENTER | DT_RIGHT );
		((CTImage*)pCtrl)->SetTextColor( D3DCOLOR_XRGB( 255, 240, 0 ) );	
	}
	if( pCtrl = Find("ROW_VALUE") )
	{
		((CTImage*)pCtrl)->SetFont(FONT_SMALL);
		((CTImage*)pCtrl)->SetAlign( DT_VCENTER | DT_RIGHT );
		((CTImage*)pCtrl)->SetTextColor( D3DCOLOR_XRGB( 255, 240, 0 ) );	
	}

	//문자열 추가
	if( pCtrl = Find("WEIGHT_TXT") )
	{
		//무게 107
		((CTImage*)pCtrl)->SetText( LIST_STRING(107) );
#ifdef __JAPAN_NEW
		((CTImage*)pCtrl)->SetFont(FONT_SMALL);
#else
		((CTImage*)pCtrl)->SetFont(FONT_NORMAL);
#endif
        ((CTImage*)pCtrl)->SetAlign( DT_VCENTER | DT_RIGHT );		
		((CTImage*)pCtrl)->SetTextColor( D3DCOLOR_XRGB( 255, 255, 255) );	

	}
	if( pCtrl = Find("MONEY_TXT") )
	{
		//줄리 31
		((CTImage*)pCtrl)->SetText( LIST_STRING(31) );
#ifdef __JAPAN_NEW
		((CTImage*)pCtrl)->SetFont(FONT_SMALL);
#else
		((CTImage*)pCtrl)->SetFont(FONT_NORMAL);
#endif
		((CTImage*)pCtrl)->SetAlign( DT_VCENTER | DT_CENTER );
		((CTImage*)pCtrl)->SetTextColor( D3DCOLOR_XRGB( 255, 255, 255) );	
	}
	if( pCtrl = Find("ROW_TXT") )
	{
		//로우 838
		((CTImage*)pCtrl)->SetText( LIST_STRING(838) );
		((CTImage*)pCtrl)->SetFont(FONT_NORMAL);
		((CTImage*)pCtrl)->SetAlign( DT_VCENTER | DT_CENTER );
		((CTImage*)pCtrl)->SetTextColor( D3DCOLOR_XRGB( 255, 255, 255) );	
	}
	if( pCtrl = Find("AVATA_RBTN") )
	{
		//아바타 839
		((CTRadioButton*)pCtrl)->SetText( LIST_STRING(839) );	
		pCtrl->SetFont( FONT_NORMAL_OUTLINE );
	}
	if( pCtrl = Find("PAT_RBTN") )
	{
		//PAT
		((CTRadioButton*)pCtrl)->SetText("P A T");		
		pCtrl->SetFont( FONT_NORMAL_OUTLINE );
	}
	if( pCtrl = Find("AVATA_TBTN") )
	{
		//아바타 839
		((CTRadioButton*)pCtrl)->SetText( LIST_STRING(839) );		
		pCtrl->SetFont( FONT_NORMAL_OUTLINE );
	}
	if( pCtrl = Find("PAT_TBTN") )
	{
		((CTRadioButton*)pCtrl)->SetText("P A T");
		pCtrl->SetFont( FONT_NORMAL_OUTLINE );
	}
	if( pCtrl = Find(IID_BTN_INVEN_EQUIP) )
	{
		//장비 840
		((CTRadioButton*)pCtrl)->SetText( LIST_STRING(840) );
		pCtrl->SetFont( FONT_NORMAL_OUTLINE );
	}
	if( pCtrl = Find(IID_BTN_INVEN_USE) )
	{
		//소모 841
		((CTRadioButton*)pCtrl)->SetText( LIST_STRING(841) );
		pCtrl->SetFont( FONT_NORMAL_OUTLINE );
	}
	if( pCtrl = Find(IID_BTN_INVEN_ETC) )
	{
		//재료 842
		((CTRadioButton*)pCtrl)->SetText( LIST_STRING(842) );
		pCtrl->SetFont( FONT_NORMAL_OUTLINE );
	}
	if( pCtrl = Find(IID_BTN_INVEN_MILEAGE) )
	{
		//마일리지 843
		((CTRadioButton*)pCtrl)->SetText( LIST_STRING(843) );
		pCtrl->SetFont( FONT_NORMAL_OUTLINE );
	}
	if( pCtrl = Find(IID_BTN_PAT_EQUIP) )
	{
		//장비 840
		((CTRadioButton*)pCtrl)->SetText( LIST_STRING(840) );
		pCtrl->SetFont( FONT_NORMAL_OUTLINE );
	}
	if( pCtrl = Find(IID_BTN_PAT_ETC) )
	{
		//기타 844
		((CTRadioButton*)pCtrl)->SetText( LIST_STRING(844) );
		pCtrl->SetFont( FONT_NORMAL_OUTLINE );
	}
	if( pCtrl = Find(IID_BTN_PAT_MILEAGE) )
	{
		//마일리지 843
		((CTRadioButton*)pCtrl)->SetText( LIST_STRING(843) );
		pCtrl->SetFont( FONT_NORMAL_OUTLINE );
	}

	//인벤토리 65
	GetCaption()->SetString( LIST_STRING(826) );

	//Equip pos
	int iSlot = 0;
	for( iSlot = 0 ; iSlot < MAX_EQUIP_IDX-1; ++iSlot )
	{		
		if( pCtrl = Find(c_strEquipedSlotName[iSlot].c_str()) )
		{
			m_AvatarEquipSlots[iSlot].SetParent( GetDialogType() );
			m_AvatarEquipSlots[iSlot].SetOffset( pCtrl->GetOffset() );
			m_AvatarEquipSlots[iSlot].SetDragAvailable();
			m_AvatarEquipSlots[iSlot].SetDragItem( m_pEquipDragItem );            
			m_AvatarEquipSlots[iSlot].MoveWindow( m_AvatarEquipSlots[iSlot].GetPosition() );
		}		
	}
	//Bullet pos
	for( iSlot = 0 ; iSlot < MAX_SHOT_TYPE; ++iSlot )
	{
		if( pCtrl = Find( c_strBulletEquipSlotName[iSlot].c_str() ) )
		{
			m_BulletEquipSlots[iSlot].SetParent( GetDialogType() );
			m_BulletEquipSlots[iSlot].SetOffset( pCtrl->GetOffset() );
			m_BulletEquipSlots[iSlot].SetDragAvailable();
			m_BulletEquipSlots[iSlot].SetDragItem( m_pEquipDragItem );
			m_BulletEquipSlots[iSlot].MoveWindow( m_BulletEquipSlots[iSlot].GetPosition() );
		}		
	}
	//pat
	for( iSlot = 0 ; iSlot < MAX_RIDING_PART; ++iSlot )
	{
		if( pCtrl = Find( c_strPatEquipSlotName[iSlot].c_str() ) )
		{
			m_PatEquipSlots[iSlot].SetParent( GetDialogType() );
			m_PatEquipSlots[iSlot].SetOffset( pCtrl->GetOffset() );
			m_PatEquipSlots[iSlot].SetDragAvailable();
			m_PatEquipSlots[iSlot].SetDragItem( m_pEquipDragItem );
			m_PatEquipSlots[iSlot].MoveWindow( m_PatEquipSlots[iSlot].GetPosition() );
		}		
	}

	POINT ptOffset;


	//	CWinCtrl * pCtrlItemSlot00 = Find( "ITEM_SLOT00" );
	//	CWinCtrl * pCtrlItemSlot01 = Find( "ITEM_SLOT01" );

	CWinCtrl * pCtrlItemSlot[6];
	pCtrlItemSlot[0] = Find( "ITEM_SLOT00" );
	pCtrlItemSlot[1] = Find( "ITEM_SLOT01" );
	pCtrlItemSlot[2] = Find( "ITEM_SLOT02" );
	pCtrlItemSlot[3] = Find( "ITEM_SLOT03" );
	pCtrlItemSlot[4] = Find( "ITEM_SLOT04" );
	pCtrlItemSlot[5] = Find( "ITEM_SLOT05" );
	CWinCtrl * pCtrlItemSlot10 = Find( "ITEM_SLOT10" );


	if( pCtrlItemSlot10 
		&& pCtrlItemSlot[0] 
		&& pCtrlItemSlot[1] 
		&& pCtrlItemSlot[2] 
		&& pCtrlItemSlot[3] 
		&& pCtrlItemSlot[4] 
		&& pCtrlItemSlot[5] )
		{
			int iHeightGap	= pCtrlItemSlot10->GetOffset().y - pCtrlItemSlot[0]->GetOffset().y;

			for( int iItemType  = 0 ; iItemType < MAX_INV_TYPE; ++iItemType )
			{					
				for( iSlot = 0; iSlot < INVENTORY_PAGE_SIZE; ++iSlot )
				{
					ptOffset.x = pCtrlItemSlot[iSlot%INV_SLOT_WIDTH_NUM]->GetOffset().x;
					ptOffset.y = pCtrlItemSlot[0]->GetOffset().y + (iSlot/INV_SLOT_WIDTH_NUM) * iHeightGap;

					m_ItemSlots[iItemType][iSlot].SetParent( GetDialogType() );
					m_ItemSlots[iItemType][iSlot].SetOffset( ptOffset );
					m_ItemSlots[iItemType][iSlot].SetDragAvailable();
					m_ItemSlots[iItemType][iSlot].SetDragItem( m_pInvenDragItem );
					m_ItemSlots[iItemType][iSlot].MoveWindow( m_ItemSlots[iItemType][iSlot].GetPosition() );
				}					
			}
		}	

		if( pCtrl = Find("STR_PAT_POS_00") )
		{
			//형태
			((CTImage*)pCtrl)->SetText( CStringManager::GetSingleton().GetString( 732 ) );
			((CTImage*)pCtrl)->SetFont(FONT_SMALL);
			((CTImage*)pCtrl)->SetAlign( DT_VCENTER | DT_LEFT );
			((CTImage*)pCtrl)->SetTextColor( D3DCOLOR_XRGB( 255, 255, 255 ) );
		}	

		if( pCtrl = Find("STR_PAT_POS_02") )
		{
			//공격력
			((CTImage*)pCtrl)->SetText( CStringManager::GetSingleton().GetString( 735 ) );
			((CTImage*)pCtrl)->SetFont(FONT_SMALL);
			((CTImage*)pCtrl)->SetAlign( DT_VCENTER | DT_LEFT );
			((CTImage*)pCtrl)->SetTextColor( D3DCOLOR_XRGB( 255, 255, 255 ) );
		}

		if( pCtrl = Find("STR_PAT_POS_04") )
		{
			//방어력
			((CTImage*)pCtrl)->SetText( CStringManager::GetSingleton().GetString( 736 ) );
			((CTImage*)pCtrl)->SetFont(FONT_SMALL);
			((CTImage*)pCtrl)->SetAlign( DT_VCENTER | DT_LEFT );
			((CTImage*)pCtrl)->SetTextColor( D3DCOLOR_XRGB( 255, 255, 255 ) );
		}	

		if( pCtrl = Find("STR_PAT_POS_06") )
		{
			//마법저항
			((CTImage*)pCtrl)->SetText( CStringManager::GetSingleton().GetString( 737 ) );
			((CTImage*)pCtrl)->SetFont(FONT_SMALL);
			((CTImage*)pCtrl)->SetAlign( DT_VCENTER | DT_LEFT );
			((CTImage*)pCtrl)->SetTextColor( D3DCOLOR_XRGB( 255, 255, 255 ) );
		}

		if( pCtrl = Find("STR_PAT_POS_08") )
		{
			//연료소비
			((CTImage*)pCtrl)->SetText( CStringManager::GetSingleton().GetString( 738 ) );
			((CTImage*)pCtrl)->SetFont(FONT_SMALL);
			((CTImage*)pCtrl)->SetAlign( DT_VCENTER | DT_LEFT );
			((CTImage*)pCtrl)->SetTextColor( D3DCOLOR_XRGB( 255, 255, 255 ) );
		}

		if( pCtrl = Find("STR_PAT_POS_10") )
		{
			//공격속도
			((CTImage*)pCtrl)->SetText( CStringManager::GetSingleton().GetString( 739 ) );
			((CTImage*)pCtrl)->SetFont(FONT_SMALL);
			((CTImage*)pCtrl)->SetAlign( DT_VCENTER | DT_LEFT );
			((CTImage*)pCtrl)->SetTextColor( D3DCOLOR_XRGB( 255, 255, 255 ) );
		}

		if( pCtrl = Find("STR_PAT_POS_12") )
		{
			//이동속도
			((CTImage*)pCtrl)->SetText( CStringManager::GetSingleton().GetString( 740 ) );
			((CTImage*)pCtrl)->SetFont(FONT_SMALL);
			((CTImage*)pCtrl)->SetAlign( DT_VCENTER | DT_LEFT );
			((CTImage*)pCtrl)->SetTextColor( D3DCOLOR_XRGB( 255, 255, 255 ) );
		}


		if( pCtrl = Find("STR_PAT_POS_01") )
		{
			((CTImage*)pCtrl)->SetTextColor( D3DCOLOR_XRGB( 255, 247, 118 ) );
		}	
		if( pCtrl = Find("STR_PAT_POS_03") )
		{
			((CTImage*)pCtrl)->SetTextColor( D3DCOLOR_XRGB( 255, 247, 118 ) );
		}	
		if( pCtrl = Find("STR_PAT_POS_05") )
		{
			((CTImage*)pCtrl)->SetTextColor( D3DCOLOR_XRGB( 255, 247, 118 ) );
		}
		if( pCtrl = Find("STR_PAT_POS_07") )
		{	
			((CTImage*)pCtrl)->SetTextColor( D3DCOLOR_XRGB( 255, 247, 118 ) );
		}
		if( pCtrl = Find("STR_PAT_POS_09") )
		{		
			((CTImage*)pCtrl)->SetTextColor( D3DCOLOR_XRGB( 255, 247, 118 ) );
		}
		if( pCtrl = Find("STR_PAT_POS_11") )
		{
			((CTImage*)pCtrl)->SetTextColor( D3DCOLOR_XRGB( 255, 247, 118 ) );
		}
		if( pCtrl = Find("STR_PAT_POS_13") )
		{
			((CTImage*)pCtrl)->SetTextColor( D3DCOLOR_XRGB( 255, 247, 118 ) );
		}
}


bool CItemDlg::IsInside4Move(int x, int y)
{
	CWinCtrl * ctrl = NULL;

	if( ctrl = Find( IID_PANE_EQUIP ) )
	{
		if( ctrl->IsVision() && ctrl->IsInside( x, y ) )
		{
			return true;
		}
	}
	if( ctrl = Find( IID_PANE_INVEN ) )
	{
		if( ctrl->IsVision() && ctrl->IsInside( x, y ) )
		{
			return true;
		}
	}
	return false;

}


static bool ItemIconSorter(CIconItem* i1, CIconItem* i2)
{
	return i1->GetItemNo() < i2->GetItemNo();
}

void CItemDlg::SortItems()
{
	int iSlot;
	std::set<int> itemTypes;
	std::map<int, std::vector<CIconItem* > > itemMap;
	for( iSlot = 0 ; iSlot < INVENTORY_PAGE_SIZE; ++iSlot ) {
		CIcon* slotIcon = m_ItemSlots[m_iInventoryTab][iSlot].GetIcon();
		CIconItem* itemIcon = NULL;
		CItem* slotItem = NULL;
		if (slotIcon) {
			itemIcon = reinterpret_cast<CIconItem*>(slotIcon);
			if (itemIcon) {
				slotItem = itemIcon->GetCItem();
				if (slotItem) {
					itemMap[slotItem->GetType()].push_back(itemIcon);
					itemTypes.insert( slotItem->GetType());
				}
			}
		}
	}


	for (iSlot = 0 ; iSlot < INVENTORY_PAGE_SIZE; ++iSlot) {
		CIcon* icon = m_ItemSlots[m_iInventoryTab][iSlot].MoveIcon();
	}


	iSlot = 0;
	std::set<int>::const_iterator typeIter;
	for (typeIter = itemTypes.begin(); typeIter != itemTypes.end(); ++typeIter)
	{
		std::sort(itemMap[*typeIter].begin(), itemMap[*typeIter].end(), ItemIconSorter);
		std::vector<CIconItem* >::iterator iconIter;
		for (iconIter = itemMap[*typeIter].begin(); iconIter != itemMap[*typeIter].end(); ++iconIter)
		{
			m_ItemSlots[m_iInventoryTab][iSlot].AttachIcon(*iconIter);
			iSlot++;
		}
	}
}
