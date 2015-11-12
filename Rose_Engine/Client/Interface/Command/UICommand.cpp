#include "stdafx.h"
#include "uicommand.h"
#include "CTCmdNumberInput.h"
#include "../CToolTipMgr.h"
#include "../it_Mgr.h"
#include "../../Object.h"
#include "../../System/CGame.h"
#include "../Dlgs/CSeparateDlg.h"
#include "../DLGs/COptionDlg.h"
#include "../DLGs/AvatarInfoDlg.h"
#include "../Dlgs/CNumberInputDlg.h"
#include "../Dlgs/QuickToolBAR.h"
#include "../Dlgs/CUpgradeDlg.h"
#include "../Dlgs/DeliveryStoreDlg.h"
#include "../DLGs/CSkillDLG.h"
#include "../DLGs/CItemDlg.h"

#include "../Icon/CIconItem.h"
#include "../Icon/CIconDialog.h"


#include "../../Common/CItem.h"
#include "../../Common/IO_Pat.h"
#include "../../GameData/CManufacture.h"
#include "../../GameData/CSeparate.h"
#include "../../gamedata/cprivatestore.h"
#include "../../GameCommon/Item.h"
#include "../../System/CGame.h"

#include "../../Network/CNetwork.h"
#include "../Dlgs/CCommDlg.h"
#include "../ClanMarkTransfer.h"
#include "../../Country.h"
#include "JCommandState.h"
#include "../dlgs/subclass/CSlot.h"
#include "../interface/icon/ciconskill.h"
#include "../GameCommon/Skill.h"

bool CTCmdTakeInItem2MakeDlg::Exec( CTObject* pObj )
{
	if( pObj == NULL )
	{
		assert( pObj && "CTCmdTakeInItem2MakeDlg::Exec" );
		return true;
	}

	if( strcmp( pObj->toString(),"CIcon" ) == 0 )
		CManufacture::GetInstance().SetMaterialItem( ((CIconItem*)pObj)->GetCItem() );
	else if( strcmp( pObj->toString(), "CItem" ) == 0 )
		CManufacture::GetInstance().SetMaterialItem( (CItem*)pObj );
	else
		assert( 0 && "Invalid TObject Type @CTCmdTakeInItem2makedlg" );

	return true;
}


bool CTCmdTakeOutItemFromMakeDlg::Exec( CTObject* pObj )
{
	if( pObj == NULL )
	{
		assert( pObj && "CTCmdTakeOutItemFromMakeDlg::Exec" );
		return true;
	}

	if( strcmp( pObj->toString(),"CIcon" ) == 0 )
		CManufacture::GetInstance().RemoveMaterialItem( ((CIconItem*)pObj)->GetCItem() );
	else if( strcmp( pObj->toString(), "CItem" ) == 0 )
		CManufacture::GetInstance().RemoveMaterialItem( (CItem*)pObj );
	else
		assert( 0 && "Invalid TObject Type @CTCmdTakeOutItemFrommakedlg" );

	return true;
}


bool CTCmdTakeOutItemFromSeparateDlg::Exec( CTObject* pObj )
{
	if( pObj == NULL )
	{
		assert( pObj );
		return true;
	}

	if( strcmp( pObj->toString(),"CIcon" ) == 0 )
		CSeparate::GetInstance().RemoveItem();
	else
		assert( 0 && "Invalid TObject Type @CTCmdTakeOutItemFromSeparateDlg" );

	return true;
}


///*-----------------------------------------------------------------------------------------
//CTCmdAssembleRideItem::CTCmdAssembleRideItem( short nPartIdx, short nInvenIdx , short nEquipedBodyPartItemNo )
//{
//	m_nPartIdx	= nPartIdx;
//	m_nInvenIdx = nInvenIdx;
//	m_nEquipedBodyPartItemNo = nEquipedBodyPartItemNo;
//}
bool CTCmdAssembleRideItem::Exec( CTObject* pObj )
{
	if( g_pAVATAR->GetPetMode() >= 0 )
	{
		if( ( g_pAVATAR->GetPetState() != CS_STOP ) )
		{
			g_itMGR.AppendChatMsg( STR_ACTION_COMMAND_STOP_STATE_FAILED, IT_MGR::CHAT_TYPE_SYSTEM );
			return false;
		}
	}


#ifdef _GBC
	// ȫ�� : 2�ν� īƮ	
	// īƮ �������� �Խ�Ʈ�� Ÿ�� ������,
	// īƮ �������� Ÿ�� ������
	// ������ ��ȯ ����.	
	if( g_pAVATAR->GetRideUserIndex() || g_pAVATAR->IsRideUser() )
	{	
		g_itMGR.AppendChatMsg( STR_BOARDING_CANT_USE, IT_MGR::CHAT_TYPE_SYSTEM );		
		return false;
	}
#endif



	if( pObj == NULL )
	{
		assert( pObj && "pObj is NULL @CTCmdAssembleRideItem::Exec" );
		return true;
	}

	short nInvenIdx = 0;
	short nItemNo   = 0;

	if( strcmp(pObj->toString(), "CIcon" ) == 0 )
	{
		CIconItem* pItemIcon = (CIconItem*)pObj;
		nItemNo		= pItemIcon->GetItemNo();
		nInvenIdx	= pItemIcon->GetIndex();
	}
	else if( strcmp( pObj->toString(), "CItem" ) == 0 )
	{
		CItem* pItem = (CItem*)pObj;
		nItemNo		= pItem->GetItemNo();
		nInvenIdx	= pItem->GetIndex();
	}
	else
	{
		assert( 0 && "Invalid TObject Type @CTCmdAssembleRideItem::Exec" );
		return true;
	}

	short nPartIdx = PAT_ITEM_PART_IDX( nItemNo );

	CItemSlot* pItemSlot = g_pAVATAR->GetItemSlot();

	CItem* pBodyItem = pItemSlot->GetItem( INVENTORY_RIDE_ITEM0 );///�����Ǿ��ִ� ������
	CItem* pItem	 = pItemSlot->GetItem( nInvenIdx );///�����Ϸ��� ������
	if( pItem == NULL )
	{
		assert( pItem );
		return true;
	}

	if( !g_pAVATAR->Check_PatEquipCondition ( pItem->GetItem() ) )
	{
		g_itMGR.AppendChatMsg( STR_NOTIFY_06, IT_MGR::CHAT_TYPE_SYSTEM );
		return false;
	}


#if defined(_GBC)
	///���� ����
	if( !g_pAVATAR->Check_JobCollection(PAT_ITEM_EQUIP_REQUIRE_CLASS( nItemNo )) )
	{
		g_itMGR.AppendChatMsg( STR_NOT_ENOUGH_CONDITION, IT_MGR::CHAT_TYPE_SYSTEM );
		return true;
	}
#endif


	if( nPartIdx != RIDE_PART_BODY )
	{
		if( pBodyItem )
		{
			if( PAT_ITEM_PART_TYPE( pBodyItem->GetItemNo() ) != PAT_ITEM_PART_TYPE( nItemNo ) )
			{
				g_itMGR.AppendChatMsg(STR_PAT_ERROR_NOT_EQUAL_CLASS, IT_MGR::CHAT_TYPE_SYSTEM );
				return true;
			}

			if( PAT_ITEM_PART_VER( pBodyItem->GetItemNo() ) < PAT_ITEM_PART_VER( nItemNo ) ) 
			{
				g_itMGR.AppendChatMsg(STR_PAT_ERROR_NOT_VERSION, IT_MGR::CHAT_TYPE_SYSTEM );
				return true;
			}

		}
		else
		{
			g_itMGR.AppendChatMsg(STR_PAT_ERROR_NOT_EQUIP_BODY, IT_MGR::CHAT_TYPE_SYSTEM );			
			return true;
		}
	}
	else///�ٵ� ��ǰ�ϰ��
	{
		if( g_pAVATAR->GetPetMode() >= 0 )///����̺� ��ų ������̶��
		{
			///���� Ÿ�Ը� �����Ҽ� �ִ�.
			if( pBodyItem && PAT_ITEM_PART_TYPE( pBodyItem->GetItemNo() ) != PAT_ITEM_PART_TYPE( pItem->GetItemNo() ) )
			{
				g_itMGR.AppendChatMsg(STR_PAT_ERROR_NOT_EQUAL_CLASS, IT_MGR::CHAT_TYPE_SYSTEM );
				return true;
			}

			CItem* pPartItem = NULL;
			for( int i = 1; i < MAX_RIDING_PART; ++i )
			{
				pPartItem = pItemSlot->GetItem( INVENTORY_RIDE_ITEM0 + i);
				if( pPartItem )
				{   
					if( PAT_ITEM_PART_TYPE( pItem->GetItemNo() ) != PAT_ITEM_PART_TYPE( pPartItem->GetItemNo() ))
					{
						g_itMGR.AppendChatMsg(STR_PAT_ERROR_NOT_EQUAL_CLASS, IT_MGR::CHAT_TYPE_SYSTEM );
						return true;
					}

					if( PAT_ITEM_PART_VER( pItem->GetItemNo() ) < PAT_ITEM_PART_VER( pPartItem->GetItemNo() ) )
					{
						g_itMGR.AppendChatMsg(STR_ERROR_EQUIP_PAT_BODY_VER, IT_MGR::CHAT_TYPE_SYSTEM );
						return true;
					}
				}
			}
		}
		else///����̺� ��ų ������� �ƴ϶��
		{
			/// ���� �����ϴ� Body�� �ٸ� Parts�� �����Ǿ� �ִٸ� �� ������.
			/// ���� �����ϴ� Body�� �������� ���� ��ǰ�� �ִٸ� �� ������.
			CItem* pPartItem = NULL;

			//06. 12. 11 - ������ : ���� �ڵ�� ��ǰ�� �����ϴٰ� ������ �����ϸ� ��Ҹ� �ؼ� �������� �̻��ϰ� �����Ǵ� ������ �־���.
			//�׷��� �̸� �ʿ��� ������ ����ؼ� ���ڶ� ��쿡 �޼����� ��� false�� �����ϰ� ����.
			int iCountPartItem = 0;

			for( int i = 1; i < MAX_RIDING_PART; ++i )
			{
				pPartItem = pItemSlot->GetItem( INVENTORY_RIDE_ITEM0 + i);

				if(pPartItem)
				{
					if( PAT_ITEM_PART_TYPE( pItem->GetItemNo() ) != PAT_ITEM_PART_TYPE( pPartItem->GetItemNo() ) )
					{
						iCountPartItem++;
					}
					else if( PAT_ITEM_PART_VER( pItem->GetItemNo() ) < PAT_ITEM_PART_VER( pPartItem->GetItemNo() ) )
					{
						iCountPartItem++;
					}
				}
			}

			tagITEM sItem = pItem->GetItem();
			t_InvTYPE Type = g_pAVATAR->m_Inventory.GetInvPageTYPE( sItem );

			if( g_pAVATAR->m_Inventory.GetEmptyInvenSlotCount( Type ) < iCountPartItem )
			{
				g_itMGR.AppendChatMsg(STR_NOT_ENOUGH_INVENTORY_SPACE, IT_MGR::CHAT_TYPE_SYSTEM );
				return false;
			}

			for( int i = 1; i < MAX_RIDING_PART; ++i )
			{
				pPartItem = pItemSlot->GetItem( INVENTORY_RIDE_ITEM0 + i);
				if( pPartItem )
				{   
					if( PAT_ITEM_PART_TYPE( pItem->GetItemNo() ) != PAT_ITEM_PART_TYPE( pPartItem->GetItemNo() ) )
					{
						g_pNet->Send_cli_ASSEMBLE_RIDE_ITEM( i, 0 );
					}
					else if( PAT_ITEM_PART_VER( pItem->GetItemNo() ) < PAT_ITEM_PART_VER( pPartItem->GetItemNo() ) )
					{
						g_pNet->Send_cli_ASSEMBLE_RIDE_ITEM( i, 0 );
					}
				}
			}
		}		
	}



	g_pNet->Send_cli_ASSEMBLE_RIDE_ITEM( nPartIdx, nInvenIdx );

	return true;
}

bool CTCmdDisAssembleRideItem::Exec( CTObject* pObj )
{
	if( pObj == NULL )
	{
		assert( pObj && "pObj is NULL @CTCmdDisAssembleRideItem::Exec" );
		return true;
	}

	short nPartIdx;
	if( strcmp(pObj->toString(), "CIcon" ) == 0 )
	{
		CIconItem* pItemIcon = (CIconItem*)pObj;
		nPartIdx = pItemIcon->GetIndex() - INVENTORY_RIDE_ITEM0;
		g_pNet->Send_cli_ASSEMBLE_RIDE_ITEM( nPartIdx, 0 );
	}
	else if( strcmp( pObj->toString(), "CItem" ) == 0 )
	{
		CItem* pItem = (CItem*)pObj;
		nPartIdx = pItem->GetIndex() - INVENTORY_RIDE_ITEM0;
		g_pNet->Send_cli_ASSEMBLE_RIDE_ITEM( nPartIdx, 0 );
	}
	else
	{
		assert( 0 && "Invalid TObject Type @CTCmdDisAssembleRideItem::Exec" );
	}

	return true;
}
///*-----------------------------------------------------------------------------------------


///*-----------------------------------------------------------------------------------------
CTCmdOpenNumberInputDlg::CTCmdOpenNumberInputDlg(void)
{
	m_pCmd = NULL;
	m_i64Maximum = 0;
	m_iType	= CNumberInputDlg::TYPE_USE_MAX_HIDE;
}

bool CTCmdOpenNumberInputDlg::Exec( CTObject* pObj )
{
	CTDialog* pDlg = NULL;
	pDlg = g_itMGR.FindDlg( DLG_TYPE_N_INPUT );
	if( pDlg )
	{
		CNumberInputDlg* pNInputDlg = (CNumberInputDlg*)pDlg;

		__int64 iMaxNumber = 0;
		CTObject* pCmdParam = NULL;
		if( pObj == NULL )///���ϰ��
		{
			iMaxNumber = (int)m_i64Maximum;
			LogString( LOG_NORMAL,"maximum money is 0" );
			if( iMaxNumber == 0 )
				return true;
		}
		else if( strcmp( pObj->toString(), "CIcon" ) == 0 )///�巡�׾ص������ ����Ǿ��� ���
		{
			tagITEM& Item = ((CIconItem*)pObj)->GetItem();
			pCmdParam = ((CIconItem*)pObj)->GetCItem();
			if( Item.IsEnableDupCNT() )
			{
				if( m_i64Maximum )			
					iMaxNumber = m_i64Maximum;
				else
					iMaxNumber = Item.GetQuantity();
			}
			else
				iMaxNumber = 1;
		}
		else if( strcmp( pObj->toString(), "CItem" ) == 0 )
		{
			tagITEM& Item = ((CItem*)pObj)->GetItem();
			pCmdParam = pObj;
			if( Item.IsEnableDupCNT() )
			{
				if( m_i64Maximum )			
					iMaxNumber = m_i64Maximum;
				else
					iMaxNumber = Item.GetQuantity();
			}
			else
				iMaxNumber = 1;
		}
		else
		{
			assert( 0 && "�˼� ���� CTobject Type @CTCmdOpenNumberInputDlg::Exec" );
			return true;
		}

		if( iMaxNumber > 1 )
		{
			pNInputDlg->SetMaxNumber( iMaxNumber );
			pNInputDlg->SetNumberInputType(m_iType);
			pNInputDlg->SetCommand( m_pCmd, pCmdParam );
			g_itMGR.OpenDialog( DLG_TYPE_N_INPUT );
		}
		else if( iMaxNumber == 1 )
		{
			m_pCmd->SetNumber( 1 );
			m_pCmd->Exec( pCmdParam );
		}
		else
		{
			assert( 0 && "Maximum ���� 0���� �۰ų� ����");
		}
	}

	return true;
}

void CTCmdOpenNumberInputDlg::SetCommand( CTCmdNumberInput* pCmd )
{
	m_pCmd = pCmd;
}

void CTCmdOpenNumberInputDlg::SetMaximum( __int64 i64Maximum )
{
	m_i64Maximum = i64Maximum;
}

void CTCmdOpenNumberInputDlg::SetNumberInputType(int iType)
{
	m_iType = iType;
}

//*---------------------------------------------------------------------
#include "../dlgs/CAvatarStoreDlg.h"
bool CTCmdDragItem2AvatarStoreDlg::Exec( CTObject* pObj )
{
	if( pObj == NULL || strcmp( pObj->toString(), "CIcon" ) )
	{
		assert( 0 && "pObj is NULL or Invalid Type" );
		return true;
	}

	CTDialog* pDlg = g_itMGR.FindDlg( DLG_TYPE_AVATARSTORE );
	if( pDlg )
	{
		CAvatarStoreDlg* pStoreDlg = ( CAvatarStoreDlg*) pDlg;
		CIconItem* pItemIcon = (CIconItem*)pObj;

		int iCount = pStoreDlg->IsBuyItem( pItemIcon->GetItem() );
		if( iCount > 0 )
		{
			m_i64Maximum = iCount;
			CTCmdOpenNumberInputDlg::Exec(pItemIcon->GetCItem());
		}
	}
	return true;
}

CTCmdAcceptAddFriend::CTCmdAcceptAddFriend( WORD wUserIdx, BYTE btStatus, const char* pszName )
{

	m_wUserIdx	= wUserIdx;
	m_btStatus	= btStatus;
	assert( pszName );
	if( pszName )
		m_strName	= pszName;

}

bool CTCmdAcceptAddFriend::Exec(CTObject* pObj )
{
	g_pAVATAR->SetAddFriendWnd(false);
	g_pNet->Send_cli_MCMD_APPEND_REPLY( MSGR_CMD_APPEND_ACCEPT, m_wUserIdx , (char*)m_strName.c_str() );
	return true;
}

CTCmdRejectAddFriend::CTCmdRejectAddFriend( WORD wUserIdx , const char* pszName )
{
	m_wUserIdx = wUserIdx;
	m_strName = pszName;
}

bool CTCmdRejectAddFriend::Exec( CTObject* pObj )
{
	g_pAVATAR->SetAddFriendWnd(false);
	g_pNet->Send_cli_MCMD_APPEND_REPLY( MSGR_CMD_APPEND_REJECT, m_wUserIdx , (char*)m_strName.c_str() );
	return true;
}

CTCmdRemoveFriend::CTCmdRemoveFriend( DWORD dwUserTag )
{
	m_dwUserTag = dwUserTag;
}

bool CTCmdRemoveFriend::Exec( CTObject* pObj )
{
	g_pNet->Send_cli_MCMD_TAG( MSGR_CMD_DELETE, m_dwUserTag );
	CTDialog* pDlg = g_itMGR.FindDlg( DLG_TYPE_COMMUNITY );
	assert( pDlg );
	if( pDlg )
	{
		CCommDlg* pCommDlg = ( CCommDlg* )pDlg;
		pCommDlg->RemoveFriend( m_dwUserTag );
	}
	return true;
}

CTCmdAbandonQuest::CTCmdAbandonQuest( int iQuestSlotIdx,int iQuestID )
{
	m_iQuestSlotIdx	= iQuestSlotIdx;
	m_iQuestID		= iQuestID;

}

bool CTCmdAbandonQuest::Exec( CTObject* pObj )
{
	g_pNet->Send_cli_QUEST_REQ (TYPE_QUEST_REQ_DEL, m_iQuestSlotIdx, m_iQuestID );
	return true;
}

CTCmdSendPacketDropItem::CTCmdSendPacketDropItem( short nInvenIdx, int iQuantity )
{
	m_nInvenIdx = nInvenIdx;
	m_iQuantity = iQuantity;;

}

bool CTCmdSendPacketDropItem::Exec( CTObject* pObj )
{
	LogString( LOG_NORMAL,CStr::Printf( "Send_cli_Drop_item %d %d" , m_nInvenIdx, m_iQuantity ) );
	g_pNet->Send_cli_DROP_ITEM( m_nInvenIdx, m_iQuantity );
	return true;
}

bool CTCmdMoveDialogIcon2Ground::Exec( CTObject* pObj )
{
	assert( pObj );
	if( pObj == NULL ) return true;
	CIconDialog* pIcon = (CIconDialog*)pObj;

	POINT ptMouse;
	CGame::GetInstance().Get_MousePos( ptMouse );

	ptMouse.x -= pIcon->GetWidth() / 2;
	ptMouse.y -= pIcon->GetHeight() / 2;

	g_itMGR.SetDialogIconPosition( pIcon->GetDialogType(), ptMouse );

	return true;
}

bool CTCmdMoveDialogIcon2GroundFromMenu::Exec( CTObject* pObj )
{
	assert( pObj );
	if( pObj == NULL ) return true;

	CIconDialog* pIcon = (CIconDialog*)pObj;

	POINT ptMouse;
	CGame::GetInstance().Get_MousePos( ptMouse );

	ptMouse.x -= pIcon->GetWidth() / 2;
	ptMouse.y -= pIcon->GetHeight() / 2;

	g_itMGR.SetDialogIconPosition( pIcon->GetDialogType(), ptMouse );
	g_itMGR.AddDialogIcon( pIcon->GetDialogType() );

	return true;
}

CTCmdRemoveDialogIcon::CTCmdRemoveDialogIcon( int iDialogType )
{
	m_iDialogType = iDialogType;
}

bool CTCmdRemoveDialogIcon::Exec( CTObject* pObj )
{
	g_itMGR.DelDialogIcon( m_iDialogType );
	return true;
}

bool CTCmdRegistDialogIcon2QuickBar::Exec( CTObject* pObj )
{
	if( pObj == NULL )
	{
		assert( 0 && "Invalid Param @CTCmdDragInven2QuickBar::Exec" );
		return true;
	}

	CIconDialog* pIcon = (CIconDialog*)pObj;

	POINT ptMouse;
	CGame::GetInstance().Get_MousePos( ptMouse );

	CTDialog* pDlg = g_itMGR.FindDlg( DLG_TYPE_QUICKBAR );
	if( pDlg == NULL )
	{
		assert( 0 && "Not Found QuickBar Dialog @CTCmdDragInven2QuickBar::Exec" );
		return true;
	}

	CQuickBAR* pQuickBar = (CQuickBAR*)pDlg;

	short nQuickSlotIdx = pQuickBar->GetMouseClickSlot( ptMouse );

	if( nQuickSlotIdx == -1 )///�ش� ��ġ�� ������ ����.
		return true;

	tagHotICON hotICON;

	hotICON.m_cType   = DIALOG_ICON;
	hotICON.m_nSlotNo = pIcon->GetDialogType();

	if( hotICON.m_nSlotNo < 0 || hotICON.m_nSlotNo >= DLG_TYPE_MAX   )
	{
		assert( 0 && "Invalid Dialog Type @CTCmdRegistDialogIcon2QuickBar::Exec" );
		return true;
	}

	g_pNet->Send_cli_SET_HOTICON( (BYTE)nQuickSlotIdx, hotICON );
	return true;
}

CTCmdAddItem2WishList::CTCmdAddItem2WishList( tagITEM& Item )
{
	m_Item = Item;
}

bool CTCmdAddItem2WishList::Exec( CTObject* pObj )
{
	CPrivateStore::GetInstance().AddItemWishList( m_Item, true );
	return true;
}



CTCmdChangeStateUpgradeDlg::CTCmdChangeStateUpgradeDlg( int iState )
{
	m_iState = iState;
}

bool CTCmdChangeStateUpgradeDlg::Exec( CTObject* pObj )
{
	if( CTDialog* pDlg = g_itMGR.FindDlg( DLG_TYPE_UPGRADE ) )
	{
		CUpgradeDlg* pUpgradeDlg = (CUpgradeDlg*)pDlg;
		pUpgradeDlg->ChangeState( m_iState );
	}
	return true;
}

bool CTCmdEndRepair::Exec( CTObject* pObj )
{
	CGame::GetInstance().EndRepair();
	return true;
}


bool CTCmdEndAppraisal::Exec( CTObject* pObj )
{
	CGame::GetInstance().EndAppraisal();
	return true;
}

bool CTCmdEndMakeSocket::Exec( CTObject* pObj )
{
	CGame::GetInstance().EndMakeSocket();
	return true;
}
CTCmdSendAppraisalReq::CTCmdSendAppraisalReq( int iIndex )
{
	m_iIndex = iIndex;
}

bool CTCmdSendAppraisalReq::Exec( CTObject* pObj )
{
	g_pNet->Send_cli_APPRAISAL_REQ( m_iIndex );
	return true;
}

CTCmdQuerySellItem2PrivateStore::CTCmdQuerySellItem2PrivateStore( WORD wSellerSvrIdx, int iItemCount, tagSELL_ITEM& Item )
{
	m_wSellerSvrIdx	= wSellerSvrIdx;
	m_iItemCount	= iItemCount;
	m_SellItem		= Item;


}

bool CTCmdQuerySellItem2PrivateStore::Exec( CTObject* pObj )
{
	g_pNet->Send_cli_P_STORE_SELL_REQ( m_wSellerSvrIdx, m_iItemCount, &m_SellItem );
	return true;
}


CTCmdQueryBuyItemFromPrivateStore::CTCmdQueryBuyItemFromPrivateStore( WORD wSellerSvrIdx, int iItemCount, tagPS_SLOT_ITEM& Item )
{
	m_wSellerSvrIdx	= wSellerSvrIdx;
	m_iItemCount	= iItemCount;;
	m_BuyItem		= Item;
}
bool CTCmdQueryBuyItemFromPrivateStore::Exec( CTObject* pObj )
{
	g_pNet->Send_cli_P_STORE_BUY_REQ( m_wSellerSvrIdx, m_iItemCount, &m_BuyItem );
	return true;
}

CTCmdAcceptReqJoinClan::CTCmdAcceptReqJoinClan( const char* pszMasterName )
{
	assert( pszMasterName );
	if( pszMasterName )
		m_strMasterName = pszMasterName;
}

bool CTCmdAcceptReqJoinClan::Exec( CTObject* pObj )
{
	assert( m_strMasterName.c_str() );
	if( m_strMasterName.c_str() )
		g_pNet->Send_cli_CLAN_COMMAND(   GCMD_INVITE_REPLY_YES ,  (char*)m_strMasterName.c_str()  );
	return true;
}

CTCmdRejectReqJoinClan::CTCmdRejectReqJoinClan( const char* pszMasterName )
{
	assert( pszMasterName );
	if( pszMasterName )
		m_strMasterName = pszMasterName;
}

bool CTCmdRejectReqJoinClan::Exec( CTObject* pObj )
{
	assert( m_strMasterName.c_str() );
	if( m_strMasterName.c_str() )
		g_pNet->Send_cli_CLAN_COMMAND(   GCMD_INVITE_REPLY_NO ,  (char*)m_strMasterName.c_str()  );
	return true;
}

CTCmdClanCommand::CTCmdClanCommand( BYTE btCmd, const char* pszMsg )
{
	m_btCmd = btCmd;
	if( pszMsg )
		m_strMsg = pszMsg;
	else
		m_strMsg.clear();
}

bool CTCmdClanCommand::Exec( CTObject* pObj )
{
	if( m_strMsg.empty() )
	{
		g_pNet->Send_cli_CLAN_COMMAND( m_btCmd, NULL );

		// Ŭ�� ��ü �� Ŭ�� Ż���
		//if ( m_btCmd == GCMD_DISBAND ) // || m_btCmd == GCMD_QUIT )
		//{
		//	g_pNet->Send_cli_CLANWAR_INFO( 3 );
		//}
	}
	else
	{
		g_pNet->Send_cli_CLAN_COMMAND( m_btCmd, (char*)m_strMsg.c_str() );

		// Ŭ������ Ŭ���� ���� Ż���
		//if ( m_btCmd == GCMD_REMOVE )
		//{
		//	g_pNet->Send_cli_CLANWAR_INFO( 4 );
		//}
	}

	return true;
}

bool CTCmdCancelWaitDisconnect::Exec( CTObject* pObj )
{
	g_pNet->Send_cli_LOGOUT_CANCEL();
	g_itMGR.ChangeState( IT_MGR::STATE_NORMAL );
	return true;
}
/*--------------------------------------------------------------------------------------------------------------*/
void CTCmdInputName::SetName( const char* pszName )
{
	assert( pszName && strlen( pszName ) > 0);
	if( pszName && strlen( pszName ) > 0 )
		m_name = pszName;
}

bool CTCmdInputName::Exec( CTObject* pObj )
{
	return true;
}

/*--------------------------------------------------------------------------------------------------------------*/
bool CTCmdInputNameGiftReceiver::Exec( CTObject* pObj )
{
	if( m_name.empty() ) return true;

	if( CTDialog* pDlg = g_itMGR.FindDlg( DLG_TYPE_DELIVERYSTORE ) )
	{
		CDeliveryStoreDlg* p = (CDeliveryStoreDlg*)pDlg;
		p->save_receiver_name( m_name.c_str() );
	}

	g_pNet->Send_cli_MAIL_ITEM_FIND_CHAR( (char*)m_name.c_str() );
	return true;
}

CTCmdGiftMallItem::CTCmdGiftMallItem( BYTE slotindex, const char* name )
{
	assert( name );
	if( name )
	{
		m_slotindex = slotindex;
		m_name      = name;
	}
}

bool CTCmdGiftMallItem::Exec( CTObject* pObj )
{
	if( !m_name.empty() )
		g_pNet->Send_cli_MALL_ITEM_GIVE( m_slotindex, (char*)m_name.c_str() );
	return true;
}

CTCmdRegisterClanMark::CTCmdRegisterClanMark( int clan_id, const char* filename )
{
	m_clan_id = clan_id;
	assert( filename );
	if( filename )
		m_filename = filename;
}

bool CTCmdRegisterClanMark::Exec( CTObject* pObj )
{
	if( m_filename.empty() ) return true;

	CClanMarkTransfer::GetSingleton().RegisterMarkToServer( m_clan_id, m_filename.c_str() );
	return true;
}

CTCmdChangeStateInterface::CTCmdChangeStateInterface( int state )
{
	m_state = state;
}

bool CTCmdChangeStateInterface::Exec( CTObject* pObj )
{
	g_itMGR.ChangeState( m_state );
	return true;
}

CTCmdSetOption::CTCmdSetOption(const char * pszOption, const char * pszStatus)
{
	m_pszOption = pszOption;
	m_pszStatus = pszStatus;
}

bool CTCmdSetOption::Exec(CTObject* pObj)
{
	CTDialog * pDialog = NULL;
	CWinCtrl * pCtrl = NULL;

	if( strcmp( m_pszOption, "CTRL")==0
		|| strcmp( m_pszOption, "CHAT")==0
		|| strcmp( m_pszOption, "SOUND")==0)
	{	

		pDialog = g_itMGR.FindDlg( DLG_TYPE_INFO );
		assert( pDialog );
		((CAvatarInfoDlg*)pDialog)->SetOptionStatus(m_pszOption, m_pszStatus);		

		pDialog = g_itMGR.FindDlg( DLG_TYPE_OPTION  );
		assert( pDialog );
		((COptionDlg*)pDialog)->SetOptionStatus(m_pszOption, m_pszStatus);	

	}
	else if( strcmp( m_pszOption, "TIP")==0 )
	{
		if( strcmp( m_pszStatus, "SHOW" )==0 )
		{
			CToolTipMgr::GetInstance().Show();
		}
		else
		{
			CToolTipMgr::GetInstance().Show(false);
		}
	}	
	return true;
}

CTCmdChangeStateSeparateDlg::CTCmdChangeStateSeparateDlg( int iState )
{
	m_iState = iState;
}

bool CTCmdChangeStateSeparateDlg::Exec( CTObject* pObj )
{
	if( CTDialog* pDlg = g_itMGR.FindDlg( DLG_TYPE_SEPARATE ) )
	{
		CSeparateDlg* pSeparateDlg = (CSeparateDlg*)pDlg;
		pSeparateDlg->ChangeState( m_iState );
	}
	return true;
}

CTCmdItemUpgradeOk::CTCmdItemUpgradeOk(BYTE btType, short nSkillSLOTorNpcIDX, BYTE btTargetInvIDX, BYTE btUseItemINV[ UPGRADE_ITEM_STEP ])
{
	m_btType = btType;
	m_nSkillSLOTorNpcIDX = nSkillSLOTorNpcIDX;
	m_btTargetInvIDX = btTargetInvIDX;

	for( int i = 0 ; i < UPGRADE_ITEM_STEP; ++i )
		m_btUseItemINV[i] = btUseItemINV[i];
}

bool CTCmdItemUpgradeOk::Exec( CTObject* pObj )
{
	g_pNet->Send_cli_CRAFT_UPGRADE_REQ( m_btType, m_nSkillSLOTorNpcIDX, m_btTargetInvIDX, m_btUseItemINV );
	return true;
}

#include "../gamedata/CUpgrade.h"
CTCmdItemUpgradeCancel::CTCmdItemUpgradeCancel()
{

}

bool CTCmdItemUpgradeCancel::Exec( CTObject* pObj )
{
	CUpgrade::GetInstance().RemoveTargetItem();

	if( CTDialog* pDlg = g_itMGR.FindDlg( DLG_TYPE_UPGRADE ) )
	{
		CUpgradeDlg* pUpgradeDlg = (CUpgradeDlg*)pDlg;
		pUpgradeDlg->ChangeState( 0 ); // 0 = NORMAL_
	}	
	return true;
}


CTCmdSetSkillDlg::CTCmdSetSkillDlg( int iJobIndex )
{
	m_iJobIndex = iJobIndex;
}

bool CTCmdSetSkillDlg::Exec( CTObject* pObj )
{
	if( CTDialog* pDlg = g_itMGR.FindDlg( DLG_TYPE_SKILL ) )
	{
		CSkillDLG * pSkillDlg = (CSkillDLG*)pDlg;
		pSkillDlg->Set_Skill4Job( m_iJobIndex );
	}
	return true;
}

CTCmdLearnSkill::CTCmdLearnSkill(CIconSkill * pSkillIcon)
{
	m_pSkillIcon = pSkillIcon;
}

bool CTCmdLearnSkill::Exec( CTObject* pObj )
{
	if( m_pSkillIcon )
	{
		int iSkillBookIndex = SKILL_NEED_SKILLBOOK( m_pSkillIcon->GetSkill()->GetClanSkillSlot() );

		if( CTDialog* pDlg = g_itMGR.FindDlg( DLG_TYPE_ITEM ) )
		{			
			CSlot * pSlot = ((CItemDlg*)pDlg)->SearchItem_Inventory( iSkillBookIndex );

			if( pSlot )
			{
				if( pSlot->GetIcon() )
				{
					pSlot->GetIcon()->ExecuteCommand();			
				}
				else if( pSlot->GetCommand() )
				{
					pSlot->GetCommand()->Exec( NULL );			
				}									
				return true;
			}			
		}				
	}
	//���ڿ� �߰�.
	//��ų���� �����ϴ�.
	g_itMGR.OpenMsgBox( LIST_STRING(912) );
	return false;
}


CTCmdUpdateLearnAbleSkill::CTCmdUpdateLearnAbleSkill()
{

}
bool CTCmdUpdateLearnAbleSkill::Exec(CTObject * pObj)
{
	CTDialog * pDlg = g_itMGR.FindDlg( DLG_TYPE_SKILL );
	if( pDlg && pDlg->IsVision() )
	{		
		((CSkillDLG*)pDlg)->Update_LearnSkill();
	}	
	return false;
}



//ȫ��: ������ ���.
CTCmdUseItem::CTCmdUseItem(CItem * pItem)
{
	m_pItem = pItem;
	m_bTargetSkill = false;
}

bool CTCmdUseItem::Exec( CTObject* pObj )
{	
	if(USEITEM_TYPE( m_pItem->GetItem().GetItemNO() )==USE_ITEM_SKILL_LEARN)
	{
		BYTE btT = g_pAVATAR->Skill_LearnCondition( USEITEM_SCROLL_LEARN_SKILL( m_pItem->GetItem().GetItemNO() ) );
		if( btT != RESULT_SKILL_LEARN_SUCCESS )
		{
			switch( btT )
			{
			case RESULT_SKILL_LEARN_FAILED :		// ���� ����.	
				g_itMGR.AppendChatMsg( STR_LEARN_SKILL_FAILED, IT_MGR::CHAT_TYPE_SYSTEM );
				break;
			case RESULT_SKILL_LEARN_NEED_JOB : 		// ������ ��ġ���� �ʴ´�.		
				g_itMGR.AppendChatMsg( STR_LEARN_SKILL_JOB_FAILED, IT_MGR::CHAT_TYPE_SYSTEM );
				break;
			case RESULT_SKILL_LEARN_NEED_SKILL :	// ������ ��ų�� �ʿ��Ѵ�.		
				g_itMGR.AppendChatMsg( STR_LEARN_SKILL_NEED_PRESKILL_FAILED, IT_MGR::CHAT_TYPE_SYSTEM );
				break;
			case RESULT_SKILL_LEARN_NEED_ABILITY :	// �ɷ�ġ�� �����ϴ�		
				g_itMGR.AppendChatMsg( STR_LEARN_SKILL_NEED_ABILITY_FAILED, IT_MGR::CHAT_TYPE_SYSTEM );
				break;
			case RESULT_SKILL_LEARN_OUTOFSLOT :		// ���̻� ��ų�� ���� ����.		
				g_itMGR.AppendChatMsg( STR_LEARN_SKILL_SLOT_FULL_FAILED, IT_MGR::CHAT_TYPE_SYSTEM );
				break;
			case RESULT_SKILL_LEARN_INVALID_SKILL :	// �߸��� ��ų��ȣ �Դϴ�.		
				g_itMGR.AppendChatMsg( STR_LEARN_SKILL_INVALID_SKILL_FAILED, IT_MGR::CHAT_TYPE_SYSTEM );
				break;
			case RESULT_SKILL_LEARN_OUTOFPOINT ://	0x07   // ��ų ����Ʈ ����
				g_itMGR.AppendChatMsg( STR_NOT_ENOUGH_SKILLPOINT, IT_MGR::CHAT_TYPE_SYSTEM );
				break;
			default:
				break;
			}
			return true;
		}
	}

	if ( g_pNet->Send_cli_USE_ITEM( m_pItem->GetIndex(), m_bTargetSkill, g_UserInputSystem.GetCurrentTarget() ) ) 
	{
		g_pSoundLIST->IDX_PlaySound( USEITEM_USE_SOUND(  m_pItem->GetItem().m_nItemNo_1) );
	}
	return false;
}


//=======================================================================================
//
//  Ŭ�������� UI Callback
//
//=======================================================================================
#ifdef __CLAN_WAR_SET

#include "../System/SystemProcScript.h"

// Ŭ���� ���� ���� �ʱ�ȭ
bool	CTCmdClanWarJoinAck::isPresent	= false;
int		CTCmdClanWarJoinAck::err		= 0;
DWORD	CTCmdClanWarJoinAck::time		= 120000;


//---------------------------------------------------------------------------------------
//  Name  : EnterTheClanField()
//  Desc  : Helper inline for below function
//  Author: Antonio - 2006-07-26  ���� 4:55:32
//---------------------------------------------------------------------------------------
inline void EnterTheClanField( gsv_cli_CLAN_WAR_OK *p, bool isCancel )
{
	char psz[_MAX_PATH] = { '\0' };

	if ( CTCmdClanWarJoinAck::err == 1 )
	{
		sprintf( psz, STR_CLANWAR_ABSTENTION );
		g_itMGR.OpenMsgBox( psz );
		CTCmdClanWarJoinAck::err = 0;
		isCancel = true;
	}
	else if ( CTCmdClanWarJoinAck::err == 2 )
	{
		sprintf( psz, STR_CLANWAR_QUEST_LIST_FULL );
		g_itMGR.OpenMsgBox( psz );
		CTCmdClanWarJoinAck::err = 0;
		isCancel = true;
	}
	if ( p->m_Sub_Type == 1 )	// Ŭ������ ���
	{
		if ( isCancel )			// Ŭ���� ����� NPC ���� �ʱ�ȭ
		{
			CSystemProcScript::GetSingleton().CallLuaFunction( "RejectJoinClanWar", ZZ_PARAM_END );
		}
		else					// Ŭ���� ���ν� Ŭ���� ����Ʈ ���
		{
			CSystemProcScript::GetSingleton().CallLuaFunction( "RequestJoinClanWar", ZZ_PARAM_END );
		}
	}
	if ( isCancel )				// Ŭ���� ���� �ڵ� ����
	{
		p->m_Sub_Type = 100;

		//ȫ�� : ������ �źν� ����ȣ 0���� ����. 20070509
		if( p->m_Team_N == p->m_Zone_N )
		{
			p->m_Zone_N = 0;
		}
	}
	g_pNet->Send_cli_CLANWAR_JOIN_ACK( p );
}

//---------------------------------------------------------------------------------------
//  Name  : CTCmdJoinAckClanWar
//  Desc  : "Ŭ������ �����ϰڳ�?"�� ���� ��ȭ���ڿ� handler
//  Author: Antonio - 2006-07-24  ���� 12:31:10
//---------------------------------------------------------------------------------------
bool CTCmdClanWarJoinAck::Exec( CTObject *pObj )
{
	bool isCancel = false;

	if ( isOK )
	{
		if ( isRecured )
		{
			isCancel = true;
		}
		EnterTheClanField( &p, isCancel );
	}
	else
	{
		if ( !isRecured )		// ù��° ��ȭ���� (�����ϰڳ�?) "Cancel"�� ���� Ȯ�� ��ȭ���� ���
		{
			char psz[_MAX_PATH];
			CTCmdClanWarJoinAck *pCmdOk		= new CTCmdClanWarJoinAck( &p, true,  true );
			CTCmdClanWarJoinAck *pCmdCancel	= new CTCmdClanWarJoinAck( &p, false, true );
			short btType = CMsgBox::BT_OK | CMsgBox::BT_CANCEL;

			if ( p.m_Sub_Type == 1 )
			{
				sprintf( psz, STR_CLANWAR_JOIN_REQ_RECONFIRM_M );
			}
			else
			{
				sprintf( psz, STR_CLANWAR_JOIN_REQ_RECONFIRM_S );
			}
			g_itMGR.OpenMsgBox( psz,
								btType,
								false,
								0,
								pCmdOk, pCmdCancel,
								CMsgBox::MSGTYPE_RECV_CLANWAR_JOIN_REQ_RECONFIRM,
								"MsgBox",
								true,
								CTCmdClanWarJoinAck::time,
								CMsgBox::CHECKCOMMAND_NONE );
		}
		else					// �ι�° ��ȭ���� (�����ϰڳ�?) "Cancel"�� ����
		{
			EnterTheClanField( &p, isCancel );
		}
	}

	return true;
}
#endif

