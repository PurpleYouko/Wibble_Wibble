#include "stdafx.h"
#include "itemcommand.h"
#include "Item.h"
#include "../object.h"
#include "../network/cnetwork.h"
#include "../System/CGame.h"
#include "../interface/it_mgr.h"
#include "../interface/dlgs/chattingdlg.h"
#include "../interface/dlgs/makedlg.h"
#include "../interface/command/ctcmdhotexec.h"
#include "../interface/command/uicommand.h"
#include "../JCommandState.h"
#include "skill.h"
#include "../GameProc/UseItemDelay.h"
#include "../Common/CItem.h"
#include "../IO_Event.h"
#include "../io_terrain.h"
#include "../Country.h"


//Item base command에서 필요한 글로벌 함수들
bool IsUnequip( tagBaseITEM* item, int* curr_abilities, const int ability_count )
{
	assert( item );
	assert( curr_abilities );

	if(	item->GetLife() < 1 )/// 수명이 다한 아이템은 무시
		return false;

	int iAddType = 0;
	for( short nI = 0; nI < ITEM_NEED_DATA_CNT; ++nI )
	{
		if ( iAddType = ITEM_NEED_DATA_TYPE ( item->m_cType, item->m_nItemNo, nI) ) 
		{
			_ASSERT( iAddType < ability_count );		
			if ( curr_abilities[iAddType] < ITEM_NEED_DATA_VALUE( item->m_cType, item->m_nItemNo , nI) )
				return true;
		}
	}
	return false;
}

void SubAbilities( tagBaseITEM* item, int* curr_abilities, const int ability_count )
{
	assert( item );
	assert( curr_abilities );

	if ( item->GetItemNO() < 1 || item->GetLife() < 1 )		/// 아이템이 없거나 수명이 다한것은 통과~
		return;

	short nI, nC, nType, nValue;

	// 옵션/박힌 보석에 대해서...
	if ( item->GetGemNO() && ( item->IsAppraisal() || item->HasSocket() ) ) 
	{
		for (nI=0; nI<2; nI++)
		{
			nC = item->GetGemNO();
			nType  = GEMITEM_ADD_DATA_TYPE ( nC, nI );
			nValue = GEMITEM_ADD_DATA_VALUE( nC, nI );
			_ASSERT( nType < ability_count );
			curr_abilities[ nType ] -= nValue;
		}
	}


	for (nI=0; nI<2; nI++) 
	{
		nType = ITEM_NEED_UNION( item->GetTYPE(), item->m_nItemNo, nI);
		if ( nType && ( nType != curr_abilities[AT_UNION] ) )
			continue;

		nType = ITEM_ADD_DATA_TYPE	( item->GetTYPE(), item->m_nItemNo, nI);
		nValue= ITEM_ADD_DATA_VALUE	( item->GetTYPE(), item->m_nItemNo, nI);
		_ASSERT( nType < ability_count );
		curr_abilities[ nType ] -= nValue;
	}
}

void AddAbilities( tagBaseITEM* item, int* curr_abilities, const int ability_count )
{
	assert( item );
	assert( curr_abilities );

	if ( item->GetItemNO() < 1 || item->GetLife() < 1 )		/// 아이템이 없거나 수명이 다한것은 통과~
		return;

	short nI, nC, nType, nValue;

	// 옵션/박힌 보석에 대해서...
	if ( item->GetGemNO() && ( item->IsAppraisal() || item->HasSocket() ) ) 
	{
		for (nI=0; nI<2; nI++)
		{
			nC = item->GetGemNO();
			nType  = GEMITEM_ADD_DATA_TYPE ( nC, nI );
			nValue = GEMITEM_ADD_DATA_VALUE( nC, nI );
			_ASSERT( nType < ability_count );
			curr_abilities[ nType ] += nValue;
		}
	}


	for (nI=0; nI<2; nI++) 
	{
		nType = ITEM_NEED_UNION( item->GetTYPE(), item->m_nItemNo, nI);
		if ( nType && ( nType != curr_abilities[AT_UNION] ) )
			continue;

		nType = ITEM_ADD_DATA_TYPE	( item->GetTYPE(), item->m_nItemNo, nI);
		nValue= ITEM_ADD_DATA_VALUE	( item->GetTYPE(), item->m_nItemNo, nI);
		_ASSERT( nType < ability_count );
		curr_abilities[ nType ] += nValue;
	}
}
//-------------------------------------------------------------------------------------------------------------------

bool CTCmdItemEquipInInventory::Exec( CTObject* pObj )
{
	if( g_pAVATAR->GetPetMode() >= 0 )///드라이브 스킬 사용중이라면
	{
		g_itMGR.AppendChatMsg(STR_CANT_EQUIP_USING_DRIVESKILL, IT_MGR::CHAT_TYPE_SYSTEM );
		return true;
	}

	if( pObj == NULL )
	{
		assert( pObj && "Arg is Null @CTCmdItemEquipInventory::Exec");
		return true;
	}

	CItem* pItem = (CItem*)pObj;
	tagITEM& Item = pItem->GetItem();

	if( Item.IsEmpty() )
	{
		assert( 0 && "Item is empty @CTCmdItemEquipInInventory::Exec" );
		return true;
	}

	short nEquipIDX = 0;

	switch( Item.GetTYPE() )
	{
		case ITEM_TYPE_FACE_ITEM:
			nEquipIDX = EQUIP_IDX_FACE_ITEM;
			break;
		case ITEM_TYPE_HELMET:
			nEquipIDX = EQUIP_IDX_HELMET;
			break;
		case ITEM_TYPE_ARMOR:
			nEquipIDX = EQUIP_IDX_ARMOR;
			break;
		case ITEM_TYPE_KNAPSACK:
			nEquipIDX = EQUIP_IDX_KNAPSACK;
			break;
		case ITEM_TYPE_GAUNTLET:
			nEquipIDX = EQUIP_IDX_GAUNTLET;
			break;
		case ITEM_TYPE_BOOTS:
			nEquipIDX = EQUIP_IDX_BOOTS;
			break;
		case ITEM_TYPE_WEAPON:
			nEquipIDX = EQUIP_IDX_WEAPON_R;	
			break;
		case ITEM_TYPE_SUBWPN:
			nEquipIDX = EQUIP_IDX_WEAPON_L;
			break;
		case ITEM_TYPE_JEWEL:
			{
				switch( ITEM_TYPE(ITEM_TYPE_JEWEL, Item.GetItemNO() ))
				{
				case 171:///반지
					nEquipIDX = EQUIP_IDX_RING;
					break;
				case 172:///목걸이
					nEquipIDX = EQUIP_IDX_NECKLACE;
					break;
				case 173:///귀걸이
					nEquipIDX = EQUIP_IDX_EARRING;
					break;
				default:
					assert( 0 && " Unknown Item Type @CItemEquip::Excute" );
					break;
				}
			}
			break;

		/// 장비 아이템이 아니다.
		default:
			return true;		
	}

	///장착 제한 체크 
	if( g_pAVATAR->Check_EquipCondition ( Item ) )
	{
		CInventory* pInven = g_pAVATAR->GetInventory();

		bool b = Item.IsTwoHands();
		int i  = pInven->GetEmptyInventory( 0 );

		int j  = g_pAVATAR->Get_R_WEAPON();
		int k  = g_pAVATAR->Get_L_WEAPON();
		
		tagITEM equiped_item = g_pAVATAR->Get_EquipITEM( EQUIP_IDX_WEAPON_R );

		if( Item.IsTwoHands() &&  pInven->GetEmptyInventory( 0 ) == - 1 && g_pAVATAR->Get_L_WEAPON() && g_pAVATAR->Get_R_WEAPON() )
			g_itMGR.AppendChatMsg( STR_NOT_ENOUGH_EQUIP_SPACE, IT_MGR::CHAT_TYPE_SYSTEM );
		else if( !equiped_item.IsEmpty() && equiped_item.IsTwoHands() && Item.GetTYPE() == ITEM_TYPE_SUBWPN )
			g_itMGR.AppendChatMsg( STR_EQUIPED_TWO_HAND_WEAPON, IT_MGR::CHAT_TYPE_SYSTEM );
		else
		{
			tagITEM& Item = pItem->GetItem();
			if( !Item.IsEmpty() )
			{
				int			curr_abilities[ AT_MAX ];			///현재 아바타의 능력치 임시 저장 변수
				tagITEM		equiped_items[ MAX_EQUIP_IDX ];		///현재 아바타의 장착장비 임지 저장 변수
				std::list< int > removeitem_indexlist;			///서버에 장착해제 요청할 장착장비들의 인벤토리 인덱스 리스트

				
				for( int index = 0; index < AT_MAX; ++index )
					curr_abilities[ index ] = g_pAVATAR->GetCur_AbilityValue( index );
				
				
				for( int index = 1; index < MAX_EQUIP_IDX; ++index )
					equiped_items[index] = g_pAVATAR->m_Inventory.m_ItemEQUIP[ index ];
				
				//equiped_items[ nEquipIDX ] = Item;

				if( !equiped_items[ nEquipIDX ].IsEmpty() )
					SubAbilities( &(equiped_items[ nEquipIDX ]), curr_abilities, AT_MAX );

				AddAbilities( &Item, curr_abilities, AT_MAX );
				equiped_items[ nEquipIDX ] = Item;

				for( int index = 1; index < MAX_EQUIP_IDX; ++index )
				{
					if( !equiped_items[index].IsEmpty() )
					{
						if( IsUnequip( &equiped_items[index], curr_abilities, AT_MAX ) )
						{
							SubAbilities( &equiped_items[index], curr_abilities, AT_MAX );
							removeitem_indexlist.push_back( index );
							ZeroMemory( &equiped_items[ index ], sizeof( tagITEM ) );
							index = 0;
						}
					}
				}
				
				if( g_pAVATAR->m_Inventory.GetEmptyInvenSlotCount(INV_WEAPON) < (int)removeitem_indexlist.size() )
				{
					g_itMGR.OpenMsgBox( STR_NOT_ENOUGH_INVENTORY_SPACE );
				}
				else
				{
					g_pNet->Send_cli_EQUIP_ITEM( nEquipIDX, pItem->GetIndex() );

					std::list< int >::iterator iter; 
					for( iter = removeitem_indexlist.begin(); iter != removeitem_indexlist.end(); ++iter )
						g_pNet->Send_cli_EQUIP_ITEM( *iter , 0 );		
				}
			}
		}
	}
	else
		g_itMGR.AppendChatMsg( STR_NOTIFY_06, IT_MGR::CHAT_TYPE_SYSTEM );


	return true;
}

bool CTCmdItemUseInInventory::Exec( CTObject* pObj )
{
	if( pObj == NULL )
	{
		assert( pObj && "Arg is Null @CTCmdItemUseInInventory::Exec");
		return true;
	}

	CItem* pItem = (CItem*)pObj;
	
	tagITEM& Item = pItem->GetItem();


	if( Item.IsEmpty() )
	{
		assert( 0 && "Item is Empty @CTCmdItemUseInInventory::Exec");	
		return true;
	}



	DWORD dwNowTime = timeGetTime();
	///일정시간동안 사용 금지
//	m_LastUseTime = dwNowTime;

	int iType = USEITEM_NEED_DATA_TYPE( Item.GetItemNO() );
	if( iType == AT_CURRENT_PLANET )///행성 조건일경우
	{
		int iPlanetNo = USEITEM_NEED_DATA_VALUE(  Item.GetItemNO() );
		if( iPlanetNo != ZONE_PLANET_NO( g_pTerrain->GetZoneNO() ) )
		{
			g_itMGR.AppendChatMsg( STR_CANTUSE_WARPSCROLL_CURRENTZONE, IT_MGR::CHAT_TYPE_SYSTEM );
			return true;
		}
	}


	switch( ITEM_TYPE( Item.GetTYPE(), Item.GetItemNO()  ) )
	{
	///수리용 아이템
	case USE_ITEM_REPAIR_ITEM:
		{
			CGame::GetInstance().BeginRepair( CGame::REPAIR_ITEM );
			CGame::GetInstance().SetUsingRepairItemInvenIdx( pItem->GetIndex() );
			return true;
		}
		break;
	/// 드릴 아이템
	case USE_ITEM_DRILL:
		{
			CGame::GetInstance().BeginMakeSocket( CGame::MAKESOCKET_ITEM );
			CGame::GetInstance().SetUsingMakeSocketItemInvenIdx( pItem->GetIndex() );
			return true;		
		}
		break;
#ifdef __EXPERIENCE_ITEM
	case USE_ITEM_MAINTAIN_ITEM:
		{
			//06. 12. 06 - 김주현 : 마일리지 아이템을 사용할경우 확인 메세지 뜨게 수정.
			CTCmdUseItem * pCmd = new CTCmdUseItem( pItem );
			g_itMGR.OpenMsgBox( CStr::Printf(LIST_STRING(958), pItem->GetName()) , CMsgBox::BT_OK | CMsgBox::BT_CANCEL, true, 0, pCmd , NULL );
			return true;
		}
		break;
#endif
	case USE_ITEM_SKILL_LEARN:
		{
			/// 2004 / 2 / 11 : nAvy - 사용아이템이라는 조건아래
			BYTE btT = g_pAVATAR->Skill_LearnCondition( USEITEM_SCROLL_LEARN_SKILL( Item.GetItemNO()) );						

			if( btT != RESULT_SKILL_LEARN_SUCCESS )
			{
				switch( btT )
				{
				case RESULT_SKILL_LEARN_FAILED :		// 배우기 실패.	
					g_itMGR.AppendChatMsg( STR_LEARN_SKILL_FAILED, IT_MGR::CHAT_TYPE_SYSTEM );
					break;
				case RESULT_SKILL_LEARN_NEED_JOB : 		// 직업이 일치하지 않는다.		
					g_itMGR.AppendChatMsg( STR_LEARN_SKILL_JOB_FAILED, IT_MGR::CHAT_TYPE_SYSTEM );
					break;
				case RESULT_SKILL_LEARN_NEED_SKILL :	// 보유할 스킬이 필요한다.		
					g_itMGR.AppendChatMsg( STR_LEARN_SKILL_NEED_PRESKILL_FAILED, IT_MGR::CHAT_TYPE_SYSTEM );
					break;
				case RESULT_SKILL_LEARN_NEED_ABILITY :	// 능력치가 부족하다		
					g_itMGR.AppendChatMsg( STR_LEARN_SKILL_NEED_ABILITY_FAILED, IT_MGR::CHAT_TYPE_SYSTEM );
					break;
				case RESULT_SKILL_LEARN_OUTOFSLOT :		// 더이상 스킬을 배울수 없다.		
					g_itMGR.AppendChatMsg( STR_LEARN_SKILL_SLOT_FULL_FAILED, IT_MGR::CHAT_TYPE_SYSTEM );
					break;
				case RESULT_SKILL_LEARN_INVALID_SKILL :	// 잘못된 스킬번호 입니다.		
					g_itMGR.AppendChatMsg( STR_LEARN_SKILL_INVALID_SKILL_FAILED, IT_MGR::CHAT_TYPE_SYSTEM );
					break;
				case RESULT_SKILL_LEARN_OUTOFPOINT ://	0x07   // 스킬 포인트 부족
					g_itMGR.AppendChatMsg( STR_NOT_ENOUGH_SKILLPOINT, IT_MGR::CHAT_TYPE_SYSTEM );
					break;
				default:
					break;
				}
				return true;
			}

			
			//20060623 스킬북으로 스킬 배울때 확인 메세지 뜨게.
			CTCmdUseItem * pCmd = new CTCmdUseItem( pItem );			

			int iExclusiveSkill = SKILL_CANT_USE_OTHER_SKILL( USEITEM_ADD_DATA_VALUE( Item.GetItemNO() ) );
			if( iExclusiveSkill )
			{
				//해당 스킬을 배우면 %s 를 배울 수 없습니다. 스킬을 배우시겠습니까?
				g_itMGR.OpenMsgBox( CStr::Printf(LIST_STRING(913) ,	SKILL_NAME(iExclusiveSkill)) ,
					CMsgBox::BT_OK | CMsgBox::BT_CANCEL, true, 0, pCmd , NULL );
			}
			else
			{
				//스킬을 배우시겠습니까?
				g_itMGR.OpenMsgBox( LIST_STRING(911) , CMsgBox::BT_OK | CMsgBox::BT_CANCEL, true, 0, pCmd , NULL );
			}


			//홍근: 스킬인 경우 Send 메세지 cmd로 만들어 실행 시킨다.
			return true;
		}
	/// 스크롤 마법
	case USE_ITEM_SKILL_DOING:
		{	
			if( g_pAVATAR->GetPetMode() > 0 )
			{
				g_itMGR.AppendChatMsg( STR_CANT_USESKILL_USING_DRIVESKILL,  IT_MGR::CHAT_TYPE_SYSTEM);
				return false;
			}

			int iSkillIDX		= USEITEM_SCROLL_USE_SKILL( Item.GetItemNO() );
			int iCurrentTarget	= g_UserInputSystem.GetCurrentTarget();				
	
			if( SKILL_TYPE( iSkillIDX ) == SKILL_TYPE_18 )///warp용 scroll일경우
			{
				if( SKILL_WARP_PLANET_NO( iSkillIDX ) != ZONE_PLANET_NO( g_pTerrain->GetZoneNO() ) )
				{
					g_itMGR.AppendChatMsg( STR_CANTUSE_WARPSCROLL_CURRENTZONE, IT_MGR::CHAT_TYPE_SYSTEM );
					return true;
				}

				///<-MP 체크 
				int	iRequireMp = (int)(g_pAVATAR->GetCur_WEIGHT() * 0.05);
				if( g_pAVATAR->Get_AbilityValue( AT_MP ) < iRequireMp )
				{
					g_itMGR.AppendChatMsg( STR_NOT_ENOUGH_CONDITION, IT_MGR::CHAT_TYPE_SYSTEM );
					return true;
				}
			}

			//----------------------------------------------------------------------------------------------
			/// Emotion skill 일경우
			/// 해애할 모션으로 변경해서 서버에 요청한다.
			if( SKILL_TYPE( iSkillIDX ) == SKILL_TYPE_16 )
			{
				/// 기본명령들은 정지상태에서만 동작한다.
				if( g_pAVATAR->Get_STATE() != CS_STOP && g_pAVATAR->Get_STATE() != CS_SIT )
				{
					g_itMGR.AppendChatMsg( STR_ACTION_COMMAND_STOP_STATE_FAILED, IT_MGR::CHAT_TYPE_SYSTEM);
					return false;
				}

				int iActionType = SKILL_ANI_ACTION_TYPE( iSkillIDX );
				if( iActionType != 0 )
				{
					g_pNet->Send_cli_SET_MOTION( iActionType, 1 );					
				}
			}
			//----------------------------------------------------------------------------------------------

			
			if( CSkillManager::CheckConditionForFireSkill( iSkillIDX, iCurrentTarget ) == false )
			{
				g_itMGR.AppendChatMsg( STR_NOT_ENOUGH_CONDITION, IT_MGR::CHAT_TYPE_SYSTEM );
				return true;
			}

			/// 스크롤로 사용하는 스킬은 사정거리 체크
			int iTargetDistance = SKILL_DISTANCE( iSkillIDX );
			CObjCHAR* pTarget = g_pObjMGR->Get_CharOBJ( iCurrentTarget, true );

			if( SKILL_TYPE( iSkillIDX ) != SKILL_TYPE_18 )
			{
				/// iTargetDistance 이 0 이면 체크하지 않는다.
				if( iTargetDistance && pTarget && !g_pAVATAR->IsInRANGE( pTarget, iTargetDistance ) )
				{
					g_itMGR.AppendChatMsg( STR_NOT_ENOUGH_CONDITION, IT_MGR::CHAT_TYPE_SYSTEM );
					return true;
				}
			}

		}
		break;			

	case USE_ITEM_EVENT_ITEM:
		{
			g_pEventLIST->Run_EVENT( g_pAVATAR->Get_INDEX(), USEITEM_CONFILE_IDX( Item.GetItemNO() ), -1, true );
		}
		break;
	}

	//----------------------------------------------------------------------------------------------------
	/// Use Item Delay 적용 및
	/// Use Item Delay 설정
	//----------------------------------------------------------------------------------------------------
	
	// 허용된 국가만이 New Delay type을 적용 할수 있음. 
	// 현재 아이템의 딜레이 타입을 가져온다. 
	if(CCountry::GetSingleton().IsUseItemDelayNewVersion())
	{
		short iItem = Item.GetItemNO();
		int iDelayType	 = USEITME_DELAYTIME_TYPE( iItem );
		float iDelayTick  = (float)(USEITME_DELAYTIME_TICK(iItem)) * 1000;

		//0 이 아니면 딜레이를 설정한다. 
		if( iDelayType )
		{
			//딜레이 타입을 넣어서 같은 타입이 딜레이 중이라면 사용할수 없다는 
			//메세지를 채팅창에 출력한다. 
			if( g_UseItemDelay.GetUseItemDelay( iDelayType ) > 0.0f)
			{
				///Use item delay 가 설정되어 있다면 패스..
				g_itMGR.AppendChatMsg( STR_CANT_DOUBLE_USE, IT_MGR::CHAT_TYPE_SYSTEM );
				return false;
			}
		}
		//공백일때 단독으로 처리한다. 
		else
		{
			//자기 자신이 딜레이 중 
			if(g_SoloUseItemDelayTick.GetUseItemDelay( iItem ) > 0.0f)
			{
				g_itMGR.AppendChatMsg( STR_CANT_DOUBLE_USE, IT_MGR::CHAT_TYPE_SYSTEM );
				return false;
			}
		}
	}	
	//--------------------------------------------------------------------------------------------------


	//사용?

	//--------------------------------------------------------------------------------------------------
	/// Target skill 일경우는 패킷뒤에 타겟 번호를 붙인다.
	//--------------------------------------------------------------------------------------------------
	bool bTargetSkill = false;
	if( ITEM_TYPE( Item.GetTYPE(), Item.GetItemNO()  ) == USE_ITEM_SKILL_DOING )
	{
		int iSkillIDX		= USEITEM_SCROLL_USE_SKILL( Item.GetItemNO() );
		if( SKILL_TYPE( iSkillIDX ) == SKILL_ACTION_FIRE_BULLET ||
			SKILL_TYPE( iSkillIDX ) == SKILL_ACTION_TARGET_BOUND_DURATION ||
			SKILL_TYPE( iSkillIDX ) == SKILL_ACTION_TARGET_BOUND ||
			SKILL_TYPE( iSkillIDX ) == SKILL_ACTION_TARGET_STATE_DURATION )
			bTargetSkill = true;
	}

	
	if ( Item.GetTYPE() == ITEM_TYPE_USE && USEITEM_TYPE(Item.GetItemNO()) == USE_ITEM_GIFT_BOX)
	{
		if(!HasEnoughInventoryEmptySlot())
		{
			g_itMGR.OpenMsgBox(STR_NOTENOUGH_INVENTORY_SLOT);
			return false;
		}

		//홍근 : 오전 10:27 2007-01-16 : 상자에 쿨타임을 넣었다. 연타 방지.
		g_pAVATAR->SetCoolTimeUseItem( g_pAVATAR->Get_NAME(), Item.GetItemNO() );


		g_pNet->Send_cli_CRAFT_BREAKUP_REQ( CRAFT_BREAKUP_USE_SKILL, 327/*분해 스킬*/, pItem->GetIndex()	);
//		g_pSoundLIST->IDX_PlaySound( USEITEM_USE_SOUND( Item.m_nItemNo_1) );
	}

	else if(Item.GetTYPE() == ITEM_TYPE_USE && USEITEM_TYPE(Item.GetItemNO()) == USE_ITEM_SKILL_DOING)
	{
		// 313번 타입에도 25번컬럼에 값이 있을경우 쿨타임 적용.
		if ( g_pNet->Send_cli_USE_ITEM( pItem->GetIndex(), bTargetSkill, g_UserInputSystem.GetCurrentTarget() ) ) 
		{
			g_pAVATAR->SetCoolTimeUseItem( g_pAVATAR->Get_NAME(), Item.GetItemNO() );
			g_pSoundLIST->IDX_PlaySound( USEITEM_USE_SOUND( Item.m_nItemNo_1) );
		}
	}
	

#ifdef __CLEAR_SKILL
	else if( ITEM_TYPE( ITEM_TYPE_USE, Item.GetItemNO()  ) == USE_ITEM_CLEAR_SKILL )
	{	
		CTCmdUseItem * pCmd = new CTCmdUseItem( pItem );

		switch( USEITEM_ADD_DATA_VALUE(Item.GetItemNO()) )
		{
		case SKILL_CLEAR_JOB:
			//직업 스킬을 초기화 하겠습니까?
			g_itMGR.OpenMsgBox( LIST_STRING(926) , CMsgBox::BT_OK | CMsgBox::BT_CANCEL, true, 0, pCmd , NULL );
			break;
		case SKILL_CLEAR_UNIQUE:
			//유니크 스킬을 초기화 하겠습니까?
			g_itMGR.OpenMsgBox( LIST_STRING(927) , CMsgBox::BT_OK | CMsgBox::BT_CANCEL, true, 0, pCmd , NULL );			
			break;
		case SKILL_CLEAR_MILEAGE:
			//마일리지 스킬을 초기화 하겠습니까?
			g_itMGR.OpenMsgBox( LIST_STRING(928) , CMsgBox::BT_OK | CMsgBox::BT_CANCEL, true, 0, pCmd , NULL );			
			break;
		case SKILL_CLEAR_ALL:
			//모든 스킬을 초기화 하겠습니까?
			g_itMGR.OpenMsgBox( LIST_STRING(929) , CMsgBox::BT_OK | CMsgBox::BT_CANCEL, true, 0, pCmd , NULL );			
			break;
		}
	}
#endif
	else
	{
		if ( g_pNet->Send_cli_USE_ITEM( pItem->GetIndex(), bTargetSkill, g_UserInputSystem.GetCurrentTarget() ) ) 
			g_pSoundLIST->IDX_PlaySound( USEITEM_USE_SOUND( Item.m_nItemNo_1) );
	}


	return true;
}

bool CTCmdItemUseInInventory::HasEnoughInventoryEmptySlot()
{
	///일단 모든 타입의 빈 슬롯을 구한다.
	short nEmptySlotCount[ MAX_INV_TYPE ];

	CInventory* pInven = g_pAVATAR->GetInventory();
	assert( pInven );

	for( int i = INV_WEAPON; i < MAX_INV_TYPE; ++i )
	{
		nEmptySlotCount[i] = pInven->GetEmptyInvenSlotCount( (t_InvTYPE)i );

		if(nEmptySlotCount[i] < 4)
			return false;
	}
	
	return true;
}

bool CTCmdItemEtcInInventory::Exec( CTObject* pObj )
{

	if( pObj == NULL )
	{
		assert( pObj && "Arg is Null @CTCmdItemUseInInventory::Exec");
		return true;
	}



	CItem* pItem = (CItem*)pObj;
	tagITEM& Item = pItem->GetItem();

	if( Item.IsEmpty() )
	{
		assert( 0 && "Item is Empty @CTCmdTemEtcInInventory::Exec");
		return true;
	}

	switch( Item.GetTYPE() )
	{
		case ITEM_TYPE_NATURAL:
			{
				CMakeDLG* pDlg = (CMakeDLG*)g_itMGR.FindDlg( DLG_TYPE_MAKE );
				if( pDlg && pDlg->IsVision() )///제조창이 열려 있는동안에는 소모탄 장착불가
				{
					//CTCmdTakeInItem2MakeDlg* pCmd = new CTCmdTakeInItem2MakeDlg( pItem->GetIndex() );
					//g_itMGR.AddTCommand( DLG_TYPE_MAKE, pCmd );
				}
				else if( NATURAL_BULLET_NO(Item.GetItemNO() ) )
				{
					if( g_pAVATAR->GetPetMode() >= 0 )///드라이브 스킬 사용중이라면
					{
						g_itMGR.AppendChatMsg(STR_CANT_EQUIP_USING_DRIVESKILL, IT_MGR::CHAT_TYPE_SYSTEM );
						return true;
					}
					t_eSHOT type = tagITEM::GetNaturalBulletType( Item.GetItemNO() );
				
					if( type != MAX_SHOT_TYPE )
						g_pNet->Send_cli_SET_BULLET( type, pItem->GetIndex() );
				}
			}break;
		case ITEM_TYPE_GEM:
			{

			}
			break;
	}
	return true;
}


bool CTCmdItemEquiped::Exec( CTObject* pObj )
{
	if( g_pAVATAR->GetPetMode() >= 0 )///드라이브 스킬 사용중이라면
	{
		g_itMGR.AppendChatMsg(STR_CANT_UNEQUIP_USING_DRIVESKILL, IT_MGR::CHAT_TYPE_SYSTEM );
		return true;
	}

	if( pObj == NULL )
	{
		assert( pObj && "Arg is Null @CTCmdItemEquiped::Exec");
		return true;
	}

	CItem* pItem = ( CItem*)pObj;
	///장착
	tagITEM& Item = pItem->GetItem();
	if( !Item.IsEmpty() )
	{
		
		int			curr_abilities[ AT_MAX ];			///현재 아바타의 능력치 임시 저장 변수
		tagITEM		equiped_items[ MAX_EQUIP_IDX ];		///현재 아바타의 장착장비 임지 저장 변수
		std::list< int > removeitem_indexlist;			///서버에 장착해제 요청할 장착장비들의 인벤토리 인덱스 리스트

		
		for( int index = 0; index < AT_MAX; ++index )
			curr_abilities[ index ] = g_pAVATAR->GetCur_AbilityValue( index );
		
		
		for( int index = 1; index < MAX_EQUIP_IDX; ++index )
			equiped_items[index] = g_pAVATAR->m_Inventory.m_ItemEQUIP[ index ];
		
		
		SubAbilities( &(pItem->GetItem()), curr_abilities, AT_MAX );
		removeitem_indexlist.push_back( pItem->GetIndex() );
		ZeroMemory( &equiped_items[ pItem->GetIndex() ], sizeof( tagITEM ) );


		for( int index = 1; index < MAX_EQUIP_IDX; ++index )
		{
			if( !equiped_items[index].IsEmpty() )
			{
				if( IsUnequip( &equiped_items[index], curr_abilities, AT_MAX ) )
				{
					SubAbilities( &equiped_items[index], curr_abilities, AT_MAX );
					removeitem_indexlist.push_back( index );
					ZeroMemory( &equiped_items[ index ], sizeof( tagITEM ) );
					index = 0;
				}
			}
		}
		
		if( g_pAVATAR->m_Inventory.GetEmptyInvenSlotCount(INV_WEAPON) < (int)removeitem_indexlist.size() )
		{
			g_itMGR.OpenMsgBox( STR_NOT_ENOUGH_INVENTORY_SPACE );
		}
		else
		{
			std::list< int >::iterator iter; 
			for( iter = removeitem_indexlist.begin(); iter != removeitem_indexlist.end(); ++iter )
				g_pNet->Send_cli_EQUIP_ITEM( *iter , 0 );		
		}
	}

	return true;
}



bool CTCmdItemBullet::Exec( CTObject* pObj )
{
	if( pObj == NULL )
	{
		assert( 0 && "Arg is Null @CTCmdItemBullet::Exec");
		return true;
	}

	CItem* pItem = ( CItem*)pObj;
	tagITEM& Item = pItem->GetItem();
	if( !Item.IsEmpty() )
	{
		if( g_pAVATAR->GetPetMode() >= 0 )///드라이브 스킬 사용중이라면
		{
			g_itMGR.AppendChatMsg(STR_CANT_UNEQUIP_BULLET_USING_DRIVESKILL, IT_MGR::CHAT_TYPE_SYSTEM );
			return true;
		}

		t_eSHOT ShotType = Item.GetBulletType();
		if( ShotType != MAX_SHOT_TYPE )
			g_pNet->Send_cli_SET_BULLET( ShotType, 0 );
	}
	return true;
}
