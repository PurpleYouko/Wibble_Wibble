#include "stdafx.h"
#include ".\cseparate.h"
#include "../interface/DLGs/CSeparateDlg.h"


#include "../interface/command/uicommand.h"

#include "../GameCommon/Item.h"
#include "../interface/it_mgr.h"
#include "../Common/CItem.h"
#include "../Common/IO_STB.h"
#include "../Network/CNetwork.h"
#include "event/CTEventItem.h"

#include "../interface/icon/ciconitem.h"

CSeparate::CSeparate(void)
{
	m_pMaterialItem = NULL;///분리 하고자 하는 아이템
	m_iRequireMp    = 0;
	m_wNpcSvrIdx    = 0;
	m_iSkillSlotIdx = 0;
	m_bWaitServerReply = false;

	for(int i = 0; i < 10 ; i++)
	{
		m_listQuantity[i] = 0;
	}
}

CSeparate::~CSeparate(void)
{
	SAFE_DELETE( m_pMaterialItem );	

}

CSeparate& CSeparate::GetInstance()
{
	static CSeparate s_Instance;
	return s_Instance;	
}


void CSeparate::SetItem( CItem* pItem )
{
	assert( pItem );
	tagITEM& Item = pItem->GetItem();
/*
	int a = CSeparateDlg::GetInstance()->m_OutputItemSlots.size();
	// 초기화를 안해줬더니;; 쓰레기값이 4개가 더 들어있어서 나중에 빈 슬롯 계산할때 삑 난다;;
//	CSeparateDlg::GetInstance()->OutputItemSlotClear(); 
	int c = CSeparateDlg::GetInstance()->m_OutputItemSlots.size();	
*/
	m_iType = TYPE_NONE;

	if( !Item.IsEnableSeparate() )
	{
		g_itMGR.OpenMsgBox(STR_ITEM_CANT_SEPARATED);
		return;
	}

	if( Item.HasSocket() && Item.GetGemNO() > 300 )
		return;

#ifdef _NEWBREAK
	else if( ITEM_DECOMPOSITION_NUMBER( Item.GetTYPE(), Item.GetItemNO() ) )
#else
	else if( ITEM_PRODUCT_IDX( Item.GetTYPE(), Item.GetItemNO() ) )
#endif
		m_iType = TYPE_DECOMPOSITION;
	else
	{
		assert( 0 && "Invalid Separate Type" );
		return;
	}

	if( m_pMaterialItem )
		RemoveItem();

	m_pMaterialItem = new CItemFragment( pItem );	
	
	if( Item.IsEnableDupCNT() )
		m_pMaterialItem->SetQuantity( 1 );

	m_Event.SetID( CTEventSeparate::EID_SET_MATERIAL_ITEM );
	m_Event.SetItem( m_pMaterialItem );
	SetChanged();
	NotifyObservers( &m_Event );

	m_iRequireMoney = Item.GetSeparateCost();

	switch( m_iType )
	{
	case TYPE_SEPARATE:
		{
			tagITEM MainMaterialItem;		

			MainMaterialItem.Init(  setItemFullNo( pItem->GetType(), pItem->GetItemNo() ) );

			CItem* pMainMaterialItem = new CItem;
			pMainMaterialItem->SetItem( MainMaterialItem );
			m_pItems.push_back( pMainMaterialItem );

			m_Event.SetID( CTEventSeparate::EID_SET_OUTPUT_ITEM );
			m_Event.SetIndex( 0 );
			m_Event.SetItem( pMainMaterialItem );
			SetChanged(); 
			NotifyObservers( &m_Event );


			tagITEM GemItem;
			GemItem.Init( setItemFullNo( ITEM_TYPE_GEM, Item.GetGemNO() ) );
			CItem* pGemItem	 = new CItem;
			pGemItem->SetItem( GemItem );
			m_pItems.push_back( pGemItem );

			m_Event.SetID( CTEventSeparate::EID_SET_OUTPUT_ITEM );
			m_Event.SetIndex( 1 );
			m_Event.SetItem( pGemItem );
			SetChanged();
			NotifyObservers( &m_Event);

			m_iRequireMp = ITEM_QUALITY( MainMaterialItem.GetTYPE(), MainMaterialItem.GetItemNO() ) / 2 +
				ITEM_QUALITY( GemItem.GetTYPE(), GemItem.GetItemNO() );

			break;
		}
	case TYPE_DECOMPOSITION:
		{
	//05.11.03 - 김주현 : 분해 관련 새로운 시스템.
	#if defined ( _NEWBREAK )

			m_iNumMat = 1;
			tagITEM MaterialItem;
			CItem*  pMaterialItem = NULL;

			int iCompositionIdx = ITEM_DECOMPOSITION_NUMBER( Item.GetTYPE(), Item.GetItemNO() );
			//PY temporary fix for items that point off the end of the break list
			if(iCompositionIdx > 2028) iCompositionIdx = 2028; //There are currently only 2028 entries in LIST_BREAK.STB and some items point to 2300 +
			if( int iRawMaterial = ITEM_BREAK_POTISION( iCompositionIdx ) )
			{

				int iOriQual = ITEM_QUALITY( Item.GetTYPE(), Item.GetItemNO() );

				if( getItemType( iRawMaterial ) )
				{
					MaterialItem.Init( iRawMaterial );
				}
				else
				{
					int iTemp    = ( iOriQual - 20 ) / 12;

					if( iTemp < 1 ) 	iTemp = 1;
					if( iTemp > 10 )	iTemp = 10;

					int iItemNo = ( iRawMaterial - 421 ) * 10 + iTemp;

					assert( iItemNo >= 1 && iItemNo <= g_TblNATUAL.m_nDataCnt );
					if( iItemNo < 0 || iItemNo > g_TblNATUAL.m_nDataCnt )
						return;

					MaterialItem.Init( setItemFullNo( ITEM_TYPE_NATURAL, iItemNo ) );
				}

				if( MaterialItem.IsEnableDupCNT() )
					MaterialItem.m_uiQuantity = ITEM_BREAK_NEED_CNT( iCompositionIdx, 0 );


				pMaterialItem = new CItem;
				pMaterialItem->SetItem( MaterialItem );
	


				m_pItems.push_back( pMaterialItem );
				m_Event.SetID( CTEventSeparate::EID_SET_OUTPUT_ITEM );
				m_Event.SetIndex( 0 );
				m_Event.SetItem( pMaterialItem );
				SetChanged(); 
				NotifyObservers( &m_Event );

				int iMaterialIdx  = 0 ;
				int iClass = 0;
				int iIndex = 0;

				for( int i = 1 ; i < 4; ++i )
				{
					if( iMaterialIdx = ITEM_BREAK_COUNT( iCompositionIdx, i ) )
					{
						MaterialItem.Init( iMaterialIdx );
						if( MaterialItem.IsEnableDupCNT() )
							MaterialItem.m_uiQuantity = ITEM_BREAK_NEED_CNT( iCompositionIdx, i);

						pMaterialItem = new CItem;
						pMaterialItem->SetItem( MaterialItem );
						m_pItems.push_back( pMaterialItem );						
						++iIndex;
						
						m_Event.SetID( CTEventSeparate::EID_SET_OUTPUT_ITEM );
						m_Event.SetIndex( iIndex );
						m_Event.SetItem( pMaterialItem );
						SetChanged(); 
						NotifyObservers( &m_Event );
						++m_iNumMat;

					}
				}

				m_iRequireMp = iOriQual + 30;


#else
		
			tagITEM MaterialItem;
			CItem*  pMaterialItem = NULL;
			int iProductIdx = ITEM_PRODUCT_IDX( Item.GetTYPE(), Item.GetItemNO() );
			if( int iRawMaterial = PRODUCT_RAW_MATERIAL( iProductIdx ) )
			{
				int iOriQual = ITEM_QUALITY( Item.GetTYPE(), Item.GetItemNO() );
	
				if( getItemType( iRawMaterial ) )
				{
					MaterialItem.Init( iRawMaterial );
				}
				else
				{
					int iTemp    = ( iOriQual - 20 ) / 12;

					if( iTemp < 1 ) 	iTemp = 1;
					if( iTemp > 10 )	iTemp = 10;

					int iItemNo = ( iRawMaterial - 421 ) * 10 + iTemp;

					assert( iItemNo >= 1 && iItemNo <= g_TblNATUAL.m_nDataCnt );
					if( iItemNo < 0 || iItemNo > g_TblNATUAL.m_nDataCnt )
						return;

					MaterialItem.Init( setItemFullNo( ITEM_TYPE_NATURAL, iItemNo ) );
				}

				if( MaterialItem.IsEnableDupCNT() )
					MaterialItem.m_uiQuantity = GetQuantity( Item, PRODUCT_NEED_ITEM_CNT( iProductIdx, 0 ) );


				pMaterialItem = new CItem;
				pMaterialItem->SetItem( MaterialItem );	

				m_pItems.push_back( pMaterialItem );
				m_Event.SetID( CTEventSeparate::EID_SET_OUTPUT_ITEM );
				m_Event.SetIndex( 0 );
				m_Event.SetItem( pMaterialItem );
				SetChanged(); 
				NotifyObservers( &m_Event );

				int iMaterialIdx  = 0 ;
				int iClass = 0;
				int iIndex = 0;
				for( int i = 1 ; i < 4; ++i )
				{
					if( iMaterialIdx = PRODUCT_NEED_ITEM_NO( iProductIdx, i ) )
					{
						MaterialItem.Init( iMaterialIdx );
						if( MaterialItem.IsEnableDupCNT() )
							MaterialItem.m_uiQuantity = GetQuantity( Item, PRODUCT_NEED_ITEM_CNT( iProductIdx, i) );

						pMaterialItem = new CItem;
						pMaterialItem->SetItem( MaterialItem );
						m_pItems.push_back( pMaterialItem );						
						++iIndex;
						
						m_Event.SetID( CTEventSeparate::EID_SET_OUTPUT_ITEM );
						m_Event.SetIndex( iIndex );
						m_Event.SetItem( pMaterialItem );
						SetChanged(); 
						NotifyObservers( &m_Event );
					}
				}

				m_iRequireMp = iOriQual + 30;
			///		

#endif
			}
		break;
	}
	default:
		break;
	}
}

void CSeparate::Update( CObservable* pObservable, CTObject* pObj )
{
	assert( pObservable );
	assert( pObj );

	if( m_pMaterialItem && pObj && strcmp( pObj->toString(), "CTEventItem") == 0  )
	{
		CTEventItem* pEvent = (CTEventItem*)pObj;
		int iIndex = pEvent->GetIndex();
		if( pEvent->GetID() == CTEventItem::EID_DEL_ITEM || 
			pEvent->GetID() == CTEventItem::EID_CHANGE_ITEM )
		{

			if( iIndex == m_pMaterialItem->GetIndex() )
			{
				m_bWaitServerReply = false;

				m_Event.SetID( CTEventSeparate::EID_REMOVE_MATERIAL_ITEM );
				SetChanged();
				NotifyObservers( &m_Event );
				delete m_pMaterialItem;
				m_pMaterialItem = NULL;

				std::vector< CItem* >::iterator iter;
				int iIndex = 0;
				for( iter = m_pItems.begin(); iter != m_pItems.end(); ++iIndex )
				{
					m_Event.SetID( CTEventSeparate::EID_REMOVE_OUTPUT_ITEM );
					m_Event.SetIndex( iIndex );
					SetChanged();
					NotifyObservers( &m_Event );

					delete *iter;
					iter = m_pItems.erase( iter );
				}
			}
		}
	}
}

void CSeparate::RemoveItem()
{
//	assert( m_pMaterialItem );
	if( m_pMaterialItem == NULL) return;

	if( m_pMaterialItem )
	{
		m_Event.SetID( CTEventSeparate::EID_REMOVE_MATERIAL_ITEM );
		SetChanged();
		NotifyObservers( &m_Event );
		delete m_pMaterialItem;
		m_pMaterialItem = NULL;
	}

	std::vector< CItem* >::iterator iter;
	int iIndex = 0;
	for( iter = m_pItems.begin(); iter != m_pItems.end(); ++iIndex )
	{
		m_Event.SetID( CTEventSeparate::EID_REMOVE_OUTPUT_ITEM );
		m_Event.SetIndex( iIndex );
		SetChanged();
		NotifyObservers( &m_Event );

		delete *iter;
		iter = m_pItems.erase( iter );
	}

	m_iRequireMoney = 0;
	m_iRequireMp = 0;
}


int CSeparate::GetRequireMp()
{
	return m_iRequireMp;
}

int	CSeparate::GetQuantity( tagITEM& Item, int iRequireQuantity )
{
	int iDuration = 0;
	if( Item.HasDurability() )
		iDuration = Item.GetDurability();

	int iLife = 1000;
	if( Item.HasLife() )
		iLife = Item.GetLife();

	return iRequireQuantity * ( 130 ) * ( iDuration / 2 + iLife / 10 + 100 ) / 55000;
}

bool CSeparate::Send_CRAFT_BREAKUP_REQ()
{
	if( m_bWaitServerReply ) return false;

	assert( m_pMaterialItem );
	if( m_pMaterialItem )
	{
		if( m_iSkillSlotIdx && m_wNpcSvrIdx == 0 )
		{

			g_pNet->Send_cli_CRAFT_BREAKUP_REQ( 
				CRAFT_BREAKUP_USE_SKILL, 
				m_iSkillSlotIdx, 
				m_pMaterialItem->GetIndex() 
			);
			m_bWaitServerReply = true;
			return true;
		}
		else if( m_wNpcSvrIdx && m_iSkillSlotIdx == 0)
		{

			g_pNet->Send_cli_CRAFT_BREAKUP_REQ( 
				CRAFT_BREAKUP_FROM_NPC, 
				m_wNpcSvrIdx, 
				m_pMaterialItem->GetIndex() 
			);
			m_bWaitServerReply = true;
			return true;
		}
		else
		{
			assert( 0 && "Invalid BreakUp Req Type" );
			return false;
		}
	}
	return false;
}

void CSeparate::SetSkillSlotIdx( int iSkillSlotIdx )
{
	m_iSkillSlotIdx = iSkillSlotIdx;
	m_wNpcSvrIdx    = 0;
}

void CSeparate::SetNpcSvrIdx( WORD wNpcSvrIdx )
{
	m_wNpcSvrIdx    = wNpcSvrIdx;
	m_iSkillSlotIdx = 0;
}

WORD CSeparate::GetNpcSvrIdx()
{
	return m_wNpcSvrIdx;
}

int CSeparate::GetType()
{
	if( m_iSkillSlotIdx && m_wNpcSvrIdx == 0 )
		return TYPE_SKILL;
	else if( m_wNpcSvrIdx && m_iSkillSlotIdx == 0)
		return TYPE_NPC;

	return TYPE_NONE;
}
int CSeparate::GetRequireMoney()
{
	return m_iRequireMoney;
}

//*----------------------------------------------------------------------//
/// 모든 결과가 Setting된 후에 호출되어야 한다.
//*----------------------------------------------------------------------//
void CSeparate::SetResult( BYTE btResult )
{
	m_btResult = btResult;
	m_Event.SetID( CTEventSeparate::EID_RECEIVE_RESULT );
	SetChanged();
	NotifyObservers( &m_Event );
}

BYTE CSeparate::GetResult()
{
	return m_btResult;
}



int CSeparate::IsValidSeparateCondition()
{
	if( m_pMaterialItem == NULL ) return INVALID_TARGETITEM;
	if( !m_pMaterialItem->GetItem().IsEnableSeparate() ) return INVALID_TARGETITEM;

	if(g_pAVATAR->m_bPersonalStoreMode == true)
		return 6;
	switch( GetType() )
	{
	case TYPE_SKILL:
		 // 06. 01. 10 - MP 소모 제거.
		break;
	case TYPE_NPC:
		if( g_pAVATAR->Get_MONEY() < m_iRequireMoney )
			return NOTENOUGH_MONEY;
		break;
	default:
		return INVALID_TYPE;
		break;
	}

	return ALL_OK;

	
}

bool CSeparate::SendPacketSeparate()
{
	assert( m_pMaterialItem );
	if ( m_pMaterialItem == NULL ) return false;

	if( int iError = IsValidSeparateCondition() )
	{
		switch( iError )
		{
		case NOTENOUGH_MP:
			g_itMGR.OpenMsgBox( STR_NOT_ENOUGH_MANA ); /// 필요한 마나가 부족.
			break;
		case INVALID_TARGETITEM: /// 분해할 아이템이 없거나 잘못된 아이템이다.
			g_itMGR.OpenMsgBox( STR_ITEM_CANT_SEPARATED );
			break;
		case NOTENOUGH_MONEY:	 /// 돈이 부족합니다.
			g_itMGR.OpenMsgBox( STR_NOT_ENOUGH_MONEY );
			break;
		default:
			break;
		}
		return false;		
	}

	if(Send_CRAFT_BREAKUP_REQ())
		return true;
	else
		return false;

	return true;
}

int CSeparate::GetMaterialCount()
{
	return m_iNumMat;
}


void CSeparate::ClearResultItemSet()
{
	m_ResultItemSet.clear();
}

void CSeparate::AddResultItemSet( tag_SET_INVITEM Item )
{
	ClientLog (LOG_NORMAL, "AddResultItemSet:: item type: %i item Number: %i amount %i", Item.m_ITEM.GetTYPE(),Item.m_ITEM.GetItemNO(), Item.m_ITEM.GetQuantity());
	// m_ResultItemSet starts out with a size of zero here

	for(int i = 0; i <  m_ResultItemSet.size(); i++)
	{
		tag_SET_INVITEM TempInvItem = m_ResultItemSet[i];
		
		if( TempInvItem.m_ITEM.IsEqual(Item.m_ITEM.GetTYPE(), Item.m_ITEM.GetItemNO()) && Item.m_ITEM.IsEnableDupCNT())
		{
			m_ResultItemSet[i] = Item;
			ClientLog (LOG_NORMAL, "AddResultItemSet:: item same as last one. type: %i Number: %i Overwriting entry. Amount = %i",m_ResultItemSet[i].m_ITEM.m_cType, m_ResultItemSet[i].m_ITEM.m_nItemNo, m_ResultItemSet[i].m_ITEM.GetQuantity1());
			return;
		}		
	}
	ClientLog (LOG_NORMAL, "AddResultItemSet:: Adding new entry. type: %i Number: %i Amount = %i",Item.m_ITEM.m_cType, Item.m_ITEM.m_nItemNo, Item.m_ITEM.GetQuantity1());
			
	m_ResultItemSet.push_back(Item);
}

void CSeparate::ApplyResultItemSet()
{
	std::vector<tag_SET_INVITEM>::iterator iter;
	for( iter = m_ResultItemSet.begin(); iter != m_ResultItemSet.end(); ++iter )
	{
		ClientLog (LOG_NORMAL, "Seperate::ApplyResultItemSet:: item being set to inventory. Slot %i ItemType %i ItenNum %i count %i",iter->m_btInvIDX,iter->m_ITEM.m_cType, iter->m_ITEM.m_nItemNo, iter->m_ITEM.GetQuantity());
		if(iter->m_ITEM.m_cType != 0)		//PY: see if we can prevent later errors by making no attempt to set an item with type zero
		{
			g_pAVATAR->Set_ITEM( iter->m_btInvIDX, iter->m_ITEM );
		}
	}
}


int CSeparate::GetResultCnt()
{
	return m_ResultItemSet.size();
}



std::vector<tag_SET_INVITEM> CSeparate::GetResultItemSet()
{
	return m_ResultItemSet;
}

void CSeparate::RunSettingOutputItem()
{
	std::vector< CItem* >::iterator iter;

	int iIndex = 0;
	for( iter = m_pItems.begin(); iter != m_pItems.end(); ++iIndex )
	{
		m_Event.SetID( CTEventSeparate::EID_REMOVE_OUTPUT_ITEM );
		m_Event.SetIndex( iIndex );
		SetChanged();
		NotifyObservers( &m_Event );

		delete *iter;
		iter = m_pItems.erase( iter );
	}
	SetOutputItem();
}

void CSeparate::SetOutputItem()
{
	CItem*  pMaterialItem = NULL;

	CIconItem* pItemIcon = NULL;	
	tagITEM*	pItem = NULL;
	tagITEM*	pItemTemp = NULL;
	int i = 0;
	m_MsgBoxData.m_Icons.clear();
	std::vector<tag_SET_INVITEM>::iterator iter;
	for( iter = m_ResultItemSet.begin(); iter != m_ResultItemSet.end(); ++iter )
	{
		tagBaseITEM Item = iter->m_ITEM;
		tagBaseITEM TempItem;
		pItem = &Item;
		TempItem = Item;
		pMaterialItem = new CItem;
		
		if( !iter->m_ITEM.IsEmpty() )
		{
			if( !iter->m_ITEM.IsEnableDupCNT() )
			{
				pMaterialItem->SetItem( iter->m_ITEM );
				m_listQuantity[i] = iter->m_ITEM.m_uiQuantity;

				pItemIcon = new CIconItem(pItem);	


				m_MsgBoxData.m_Icons.push_back( pItemIcon );
			}
			else
			{
				if( iter->m_ITEM.m_uiQuantity > g_pAVATAR->m_Inventory.m_ItemLIST[ iter->m_btInvIDX ].m_uiQuantity )
				{
					TempItem.m_uiQuantity -= g_pAVATAR->m_Inventory.m_ItemLIST[ iter->m_btInvIDX ].m_uiQuantity; 
					m_listQuantity[i] = TempItem.m_uiQuantity;
					pItemTemp = &TempItem;
					pItemIcon = new CIconItem(pItemTemp);	
					m_MsgBoxData.m_Icons.push_back( pItemIcon );

				}


				pMaterialItem->SetItem( TempItem );
			}

		m_pItems.push_back( pMaterialItem );						


		m_Event.SetID( CTEventSeparate::EID_SET_OUTPUT_ITEM );
		m_Event.SetIndex( i );
		m_Event.SetItem( pMaterialItem );
		SetChanged(); 
		NotifyObservers( &m_Event );

		}
		i++;
	}

}

CreateMsgBoxData CSeparate::GetCreateMsgBoxData()
{
	m_MsgBoxData.strMsg = STR_CRAFE_BREAKUP_SUCCESS;

	CTCmdChangeStateSeparateDlg* pCmd = new CTCmdChangeStateSeparateDlg(CSeparateDlg::STATE_NORMAL );

	m_MsgBoxData.pCmdOk = pCmd;

	return m_MsgBoxData;
}

int CSeparate::GetOutputItemQuantity(int number)
{
	return m_listQuantity[number];
}

CItemFragment* CSeparate::GetTargetItem()
{
	return m_pMaterialItem;
}
