#include "stdafx.h"
#include "CSkillCommand.h"
#include "it_mgr.h"
#include "OBJECT.h"	
#include "Skill.h"	
#include "../Network/CNetwork.h"	
#include "../JCommandState.h"
#include "Dlgs/ChattingDlg.h"
#include "../NetWork/CNetWork.h"
#include "GameData/CParty.h"
#include "GameData/CExchange.h"
#include "GameData/CPrivateStore.h"
#include "io_terrain.h"
#include "../Country.h"
#include "../System/CGame.h"
#include "../GameProc/UseItemDelay.h"

CCommand* g_QuickCommandPool[ MAX_QUICK_COMMAND ];

extern bool g_bStickMode;

struct TARGET_SET
{
	int		iIndex;
	float	fDist;
};

template<class T>
class within_dist
{ 
public: 
	bool operator() (const T &a, const T &b) const 
	{ 
		if( a.fDist < b.fDist )
		{
			return true;            
		}	
		return false;
	}; 
}; 


//----------------------------------------------------------------------
/// Basic command class
//----------------------------------------------------------------------

bool Proc_BasicSkill_SIT( CSkillSlot * pSkillSlot, CSkill * pSkill )
{	
	/// 펫탑승모드에선.. 금지
	if( g_pAVATAR->GetPetMode() > 0 )
		return false;

	if( ( g_pAVATAR->Get_STATE() != CS_STOP ) && ( g_pAVATAR->Get_STATE() != CS_SIT ) )
	{
		g_itMGR.AppendChatMsg( STR_ACTION_COMMAND_STOP_STATE_FAILED, IT_MGR::CHAT_TYPE_SYSTEM );
		return false;
	}

	g_pNet->Send_cli_TOGGLE( TOGGLE_TYPE_SIT );
	///앉기/서기 토글시 서버에게 나의 HP정보를 요청한다.
	g_pNet->Send_cli_HP_REQ( g_pAVATAR->Get_INDEX() );

	return true;
}

bool Proc_BasicSkill_Pick_Item( CSkillSlot * pSkillSlot, CSkill * pSkill )
{
	{
		//-------------------------------------------------------------------------------
		/// 앉기 상태에선 금지..
		//-------------------------------------------------------------------------------
		//06. 05. 19 - 김주현 : 카트 탑승 상태일때는 아이템 줍기가 안된다.
		/// 펫탑승모드에선.. 금지
		if( g_pAVATAR->GetPetMode() > 0 )
			return false;

		if( g_pAVATAR->Get_STATE() == CS_SIT )
			return false;


		D3DVECTOR vecCenter = g_pAVATAR->Get_CurPOS();
		int iNodeCnt = ::collectBySphere( (float *)( &vecCenter ), 1000 );

		CGameOBJ*	pObj			= NULL;
		HNODE		hNode			= 0;
		HNODE		hUserData		= 0;
		D3DVECTOR	vecTarget;
		int			iItemDistance	= 100000;
		bool		bFindItem		= false;
		CObjITEM*	pFoundItem		= NULL;
		CObjITEM*	pTempItem		= NULL;

		for( int i = 0; i < iNodeCnt; i++ )
		{
			hNode = ::getCollectNode( i );
			hUserData = ::getUserData( hNode );

			if( hUserData )
			{
				pObj = reinterpret_cast< CGameOBJ* >( hUserData );

				/// Item 인지 검사..
				if( pObj->IsA( OBJ_ITEM ) )
				{		
					pTempItem = (CObjITEM*)pObj;
					DWORD dwPassTIME = g_GameDATA.GetGameTime() - pTempItem->m_dwCreatedTIME;
					if ( pTempItem->m_wOwnerServerObjIDX &&  (int)(pTempItem->m_wRemainTIME -  dwPassTIME) >  62 * 1000 ) 
					{
						// 획득 권한이 있는지 조사...
						if ( pTempItem->m_wOwnerServerObjIDX != g_pObjMGR->Get_ServerObjectIndex(  g_pAVATAR->m_nIndex ) ) 
						{
							//g_itMGR.AppendChatMsg( STR_NOTIFY_02, g_dwBLUE );
							continue;
						}
					}

					vecTarget = pObj->Get_CurPOS();
					int iDistance = CD3DUtil::distance ((int)vecCenter.x, (int)vecCenter.y, (int)vecTarget.x, (int)vecTarget.y);
					if( iDistance < iItemDistance )
					{
						iItemDistance = iDistance;

						bFindItem = true;
						pFoundItem = (CObjITEM*)pObj;
					}							
				}
			}
		}

		if( bFindItem )
		{
			if( pFoundItem )
			{						
				g_pNet->Send_cli_MOUSECMD( pFoundItem->Get_INDEX(), pFoundItem->Get_CurPOS() );	
				g_UserInputSystem.ClearMouseState();							
			}
		}
	}
    return true;	
}

bool Proc_BasicSkill_Next_Target( CSkillSlot * pSkillSlot, CSkill * pSkill )
{
	if( g_pAVATAR->GetPetMode() > 0 )		return false;
	if( g_pAVATAR->Get_STATE() == CS_SIT )	return false;

	D3DVECTOR vecCenter = g_pAVATAR->Get_CurPOS();

	//SKILL_SCOPE : 타겟 찾을 거리(반지름).
	int iNodeCnt = ::collectBySphere( (float *)( &vecCenter ), (float)SKILL_SCOPE(pSkill->GetSkillIndex()) );

	CGameOBJ*	pObj			= NULL;
	HNODE		hNode			= 0;
	HNODE		hUserData		= 0;
	D3DVECTOR	vecTarget;
	int			iItemDistance	= 100000;
	bool		bFindItem		= false;
	CObjCHAR*	pFoundItem		= NULL;
	CObjCHAR*	pTempItem		= NULL;
	int			iTargetPoolCnt	= 10;


	vector<TARGET_SET> vecTargetPool;	//타겟풀

	for( int i = 0; i < iNodeCnt; i++ )
	{
		hNode = ::getCollectNode( i );
		hUserData = ::getUserData( hNode );

		if( hUserData )
		{
			pObj = reinterpret_cast< CGameOBJ* >( hUserData );

			if( pObj->IsA( OBJ_AVATAR ) || pObj->IsA( OBJ_MOB ) )
			{		
				pTempItem = (CObjCHAR*)pObj;			


				//적대관계 검사. 적이 아니면 타겟 넣지 말자.
				if( g_pAVATAR->Is_ALLIED( pTempItem ))
				{
					continue;
				}
				//						

				vecTarget = pObj->Get_CurPOS();
				int iDistance = CD3DUtil::distance ((int)vecCenter.x, (int)vecCenter.y, (int)vecTarget.x, (int)vecTarget.y);
				if( iDistance < iItemDistance )
				{
					iItemDistance = iDistance;

					bFindItem = true;
					pFoundItem = (CObjCHAR*)pObj;
				}							


				//타겟풀에 찾은 인덱스 넣는다.
				TARGET_SET	ts;
				ts.fDist	= (float)iDistance;
				ts.iIndex	= pObj->Get_INDEX();
				vecTargetPool.push_back( ts );
			}
		}
	}


	if( bFindItem )
	{
		if( pFoundItem )
		{	
			sort( vecTargetPool.begin(), vecTargetPool.end(), within_dist<TARGET_SET>()  );					

			bool bFindSameTarget = false;

			//SKILL_POWER : 검색할 타겟수 제한.
			UINT i;
			for( i=0; i<vecTargetPool.size() && i<(UINT)SKILL_POWER(pSkill->GetSkillIndex()); i++ )
			{
				if( g_UserInputSystem.GetCurrentTarget() == vecTargetPool[i].iIndex )
				{
					bFindSameTarget = true;
					break;
				}						
			}
			if( bFindSameTarget )
			{
				if( i == vecTargetPool.size()-1 || i==SKILL_POWER(pSkill->GetSkillIndex())-1 )
				{
					int iii = vecTargetPool[0].iIndex;
					g_UserInputSystem.SetCurrentTarget( vecTargetPool[0].iIndex );     
#ifdef _DEBUG
					g_itMGR.AppendChatMsg( CStr::Printf("Target = %d", 0) ,IT_MGR::CHAT_TYPE_SYSTEM);
#endif							
				}	
				else
				{
					int iii = vecTargetPool[i+1].iIndex;
					g_UserInputSystem.SetCurrentTarget( vecTargetPool[i+1].iIndex );
#ifdef _DEBUG
					g_itMGR.AppendChatMsg( CStr::Printf("Target = %d", i+1) ,IT_MGR::CHAT_TYPE_SYSTEM);
#endif

				}						
			}
			else
			{
				g_UserInputSystem.SetCurrentTarget( pFoundItem->Get_INDEX() );
#ifdef _DEBUG
				g_itMGR.AppendChatMsg( CStr::Printf("Target = %d", 0) ,IT_MGR::CHAT_TYPE_SYSTEM);
#endif						
			}					
		}
	}
	
	return true;	
}

CBasicCommand::CBasicCommand()
{
}

CBasicCommand::~CBasicCommand()
{
}

bool CBasicCommand::Execute()
{	
	/// get skill.
	CSkillSlot* pSkillSlot = g_pAVATAR->GetSkillSlot();
	CSkill* pSkill = pSkillSlot->GetSkill( m_iSlotNO );

	int iBasicCommandType = SKILL_DISTANCE( pSkill->GetSkillIndex() );

	/// 기본명령( 앉기, 서기, 감정표현등 )일 경우 사정거리를 어떤 명령인지로 판단하는데 사용
	switch( iBasicCommandType )
	{		
	case COMMON_COMMAND_SELFTARGET:
		g_UserInputSystem.SetTargetSelf();	break;

	case COMMON_COMMAND_SIT:
		return Proc_BasicSkill_SIT( pSkillSlot, pSkill );	

	case COMMON_COMMAND_PICK_ITEM:
		return Proc_BasicSkill_Pick_Item( pSkillSlot, pSkill );	
	
	case COMMON_COMMAND_JUMP: /// 앉은 상태에선 점프 불가
		{
			/// 펫탑승모드에선.. 금지
			if( g_pAVATAR->GetPetMode() > 0 )
				break;


			if( ( g_pAVATAR->Get_STATE() != CS_SIT ) )
				g_pNet->Send_cli_SET_MOTION( SKILL_ANI_ACTION_TYPE( pSkill->GetSkillIndex() ) );			
			///else
			///	AddMsgToChatWND( STR_ACTION_COMMAND_STOP_STATE_FAILED, g_dwRED, CChatDLG::CHAT_TYPE_SYSTEM );
		}
		break;
	case COMMON_COMMAND_AIR_JUMP:
		{
			/// 펫탑승모드에선.. 금지
			if( g_pAVATAR->GetPetMode() > 0 )
				break;

			if( ( g_pAVATAR->Get_STATE() != CS_SIT ) )
				g_pNet->Send_cli_SET_MOTION( SKILL_ANI_ACTION_TYPE( pSkill->GetSkillIndex() ) );
			///else
			///	AddMsgToChatWND( STR_ACTION_COMMAND_STOP_STATE_FAILED, g_dwRED, CChatDLG::CHAT_TYPE_SYSTEM );
		}
		break;
	/// 오토 타겟..
	case COMMON_COMMAND_AUTO_TARGET:
		{
		}
		break;
	/// 일반 공격
	case COMMON_COMMAND_ATTACK:
		{
			//-------------------------------------------------------------------------------
			/// 앉기 상태라면.. 서기 명령을 날려준다.
			//-------------------------------------------------------------------------------
			if( g_pAVATAR->Get_STATE() == CS_SIT )
			{
				g_pNet->Send_cli_TOGGLE( TOGGLE_TYPE_SIT );
				break;
			}


			int iTargetObj = g_UserInputSystem.GetCurrentTarget();
			if( iTargetObj == 0 )
				break;

			int iServerTarget = g_pAVATAR->Get_TargetIDX();
			int iClientTarget = g_pObjMGR->Get_ClientObjectIndex( iServerTarget );
			/// 현재 내가 공격중인놈은 다시 패킷을 날리지 않는다.
			if( g_pAVATAR->Get_COMMAND() == CMD_ATTACK && iClientTarget == iTargetObj )
				break;				

			CObjCHAR* pChar = g_pObjMGR->Get_CharOBJ( iTargetObj, true );				
			if( pChar && pChar->IsA( OBJ_MOB ) )
			{
				CObjMOB* pObjMOB = (CObjMOB*)pChar;
				if( pObjMOB != NULL )
				{
					/// 공격불가 NPC 는 공격못함..
					if( CUserInputState::IsEnemy( pObjMOB ) && ( NPC_CAN_TARGET( pObjMOB->Get_CharNO() ) != 1 ) ) 
					{
						//----------------------------------------------------------------------------------------------------
						/// PAT에 타고 있는 도중에는 불가
						/// 그러나 Castle Gear 은 가능
						//----------------------------------------------------------------------------------------------------
						int iPetMode = g_pAVATAR->GetPetMode();
						if( iPetMode < 0 )
						{
							g_pNet->Send_cli_ATTACK( iTargetObj );						
						}else
						{
							/// Pet mode 일 경우에는..
							if( g_pAVATAR->CanAttackPetMode() )
							{
								g_pNet->Send_cli_ATTACK( iTargetObj );
							}								
						}
					}
				}
			}
		}
		break;
	case COMMON_COMMAND_DRIVE_CART:
	case COMMON_COMMAND_CASTLEGEAR:
		{
			if( g_pAVATAR->CanRidePET() == false )
			{
				g_itMGR.AppendChatMsg( STR_ERROR_NOT_ASSEMBLED, IT_MGR::CHAT_TYPE_SYSTEM );
				return false;
			}

			int iItemType = g_pAVATAR->m_Inventory.m_ItemRIDE[ 0 ].GetTYPE();
			int iItemNo   = g_pAVATAR->m_Inventory.m_ItemRIDE[ 0 ].GetItemNO();


			//홍근 060323 카트스킬과 캐슬기어스킬 구분
			if( iBasicCommandType == COMMON_COMMAND_DRIVE_CART )
			{
				if( ITEM_TYPE( iItemType, iItemNo ) % 10 != 1 )
				{
					break;
				}
			}
			else
			{
				if( ITEM_TYPE( iItemType, iItemNo ) % 10 != 2 )
				{
					break;
				}
			}

			if( CPrivateStore::GetInstance().IsOpened()  )
			{
				g_itMGR.AppendChatMsg(STR_CANT_USE_DRIVESKILL, IT_MGR::CHAT_TYPE_SYSTEM );
				return false;
			}
			/// 현재 펫탑승상태에서 펫의 상태를 확인..
			if( g_pAVATAR->GetPetMode() >= 0 )
			{
				if( ( g_pAVATAR->GetPetState() != CS_STOP ) )
				{
					g_itMGR.AppendChatMsg( STR_ACTION_COMMAND_STOP_STATE_FAILED, IT_MGR::CHAT_TYPE_SYSTEM );
					return false;
				}
			}
			else
			{
				if( ( g_pAVATAR->Get_STATE() != CS_STOP ) )
				{
					g_itMGR.AppendChatMsg( STR_ACTION_COMMAND_STOP_STATE_FAILED, IT_MGR::CHAT_TYPE_SYSTEM );
					return false;
				}			

				if( IsApplyNewVersion() )
				{
					switch( ZONE_RIDING_REFUSE_FLAG( g_pTerrain->GetZoneNO() ) )
					{
					case 1:
						if( (ITEM_TYPE( iItemType, iItemNo ) / 100 == 5) && (ITEM_TYPE( iItemType, iItemNo ) % 10 == 1 ) )
						{
							g_itMGR.AppendChatMsg( STR_CANT_USE_CART , IT_MGR::CHAT_TYPE_SYSTEM , D3DCOLOR_ARGB( 255, 206, 223, 136));				
							return false;
						}
						break;
					case 2:
						if( (ITEM_TYPE( iItemType, iItemNo ) / 100 == 5) && (ITEM_TYPE( iItemType, iItemNo ) % 10 == 2 ) )
						{
							g_itMGR.AppendChatMsg( STR_CANT_USE_CASTLEGEAR , IT_MGR::CHAT_TYPE_SYSTEM , D3DCOLOR_ARGB( 255, 206, 223, 136));				
							return false;
						}
						break;
					case 3:
						g_itMGR.AppendChatMsg(STR_CANT_USE_DRIVESKILL, IT_MGR::CHAT_TYPE_SYSTEM, D3DCOLOR_ARGB( 255, 206, 223, 136) );
						return false;
						break;
					default:
						break;
					}
				}
			}


			//20050818 홍근 카트 게이지가 0일때 카트 드라이브 스킬 사용시 "조건이 맞지 않아서 사용할 수 없습니다."
			if( g_pAVATAR->GetCur_PatHP() <= 0 )
			{
				g_itMGR.AppendChatMsg( STR_NOTIFY_06, IT_MGR::CHAT_TYPE_SYSTEM, D3DCOLOR_ARGB( 255, 206, 223, 136) );
			}			

			g_pNet->Send_cli_TOGGLE( TOGGLE_TYPE_DRIVE );
		}
		break;

		//2인승 카트
	case COMMON_COMMAND_BOARD_CART: //카트 2인승에 유저를 태우고 내리고 해주는 스킬.
		{
			//Exception Proc.//
			//보조석에 캐릭터가 존재하면 하차.
			if( g_pAVATAR->IsRideUser() || g_pAVATAR->GetRideUserIndex() )
			{
				g_pNet->Send_cli_TOGGLE( TOGGLE_TYPE_DRIVE );
				return false;
			}

			//아바타가 카트에 탑승하고 있어야 함.
			if( g_pAVATAR->GetPetMode() < 0 )
			{
				g_itMGR.AppendChatMsg(STR_MUST_BOARD_CART_ABLE_TO_USE, IT_MGR::CHAT_TYPE_SYSTEM, D3DCOLOR_ARGB( 255, 206, 223, 136) );
				return false;
			}

#ifdef _GBC
			//보조석이 장착되어 있어야 함.
			if( g_pAVATAR->IsRideItem( RIDE_PART_ABIL ) == false )
			{
				g_itMGR.AppendChatMsg(STR_AFFIX_RUMBLE, IT_MGR::CHAT_TYPE_SYSTEM, D3DCOLOR_ARGB( 255, 206, 223, 136) );
				return false;
			}								
#endif

			//Find Index.
			int iTargetObjClientIndex = g_UserInputSystem.GetCurrentTarget();
			CObjAVT* pAVT = g_pObjMGR->Get_CharAVT( iTargetObjClientIndex, true );

			// 케릭터를 선택해야 합니다.
			if( !pAVT )
			{
				g_itMGR.AppendChatMsg(STR_REQUEST_SELECT_CHARACTER, IT_MGR::CHAT_TYPE_SYSTEM, D3DCOLOR_ARGB( 255, 206, 223, 136) );				
				return false;
			}
			// 대상이 개인 상점을 열고 있다.
			if(pAVT->m_bPersonalStoreMode == true)
			{
//				g_itMGR.AppendChatMsg("개인상점이 열려 있소이다..-_-+", IT_MGR::CHAT_TYPE_SYSTEM, D3DCOLOR_ARGB( 255, 206, 223, 136) );				
				return false;
			}
			// 거리가 너무 멈니다. (200 이하 == 2미터)
#ifdef _DEBUG
			CStr::DebugString( "Dist: %f \n", g_pAVATAR->Get_DISTANCE( pAVT ) );
			CStr::DebugString( "SKILL_SCOPE: %f \n", SKILL_SCOPE(pSkill->GetSkillIndex()) );			
#endif		

			if( g_pAVATAR->Get_DISTANCE( pAVT ) >= SKILL_SCOPE(pSkill->GetSkillIndex()) )
			{				
				g_itMGR.AppendChatMsg( STR_OUTOFRANGE, IT_MGR::CHAT_TYPE_SYSTEM, D3DCOLOR_ARGB( 255, 206, 223, 136) );							
				return false;
			}

			//카트를 태울수 없는 상태 입니다. (상대방이 카트 탑승하고 있음.)
			if( pAVT->GetPetMode() >= 0 )
			{
				g_itMGR.AppendChatMsg(STR_CANT_BOARD_CONDITION, IT_MGR::CHAT_TYPE_SYSTEM, D3DCOLOR_ARGB( 255, 206, 223, 136) );
				return false;
			}			

			//너무 무거워서 태울 수 없습니다.
			if( pAVT->GetWeightRate() >= 100 )
			{
				g_itMGR.AppendChatMsg(STR_CANT_TOO_HEAVY, IT_MGR::CHAT_TYPE_SYSTEM, D3DCOLOR_ARGB( 255, 206, 223, 136) );
				return false;
			}

			//Send Msg.
			if( iTargetObjClientIndex != g_pAVATAR->Get_INDEX() && pAVT )
			{
                g_pNet->Send_cli_CART_RIDE( CART_RIDE_REQ,
					g_pObjMGR->Get_ServerObjectIndex(g_pAVATAR->Get_INDEX()),
					g_pObjMGR->Get_ServerObjectIndex(iTargetObjClientIndex) );
				
#ifdef _DEBUG
				CStr::DebugString("Send_cli_CART_RIDE\n");
#endif				
			}			
		}
		break;

	case COMMON_COMMAND_PARTY:
		{
			int iTargetObjClientIndex = g_UserInputSystem.GetCurrentTarget();
			CObjAVT* pAVT = g_pObjMGR->Get_CharAVT( iTargetObjClientIndex, true );

			if( iTargetObjClientIndex != g_pAVATAR->Get_INDEX() && pAVT )
			{
				if( !g_pTerrain->IsPVPZone() ||
					( g_pTerrain->IsPVPZone() && pAVT->Get_TeamNO() == g_pAVATAR->Get_TeamNO() )
				)
				{
					if( CParty::GetInstance().HasParty() )
					{
						///파티 초대
						if( CParty::GetInstance().IsPartyLeader() )
						{
							///파티 풀
							if( CParty::GetInstance().IsPartyFull() )
								g_itMGR.AppendChatMsg(STR_PARTY_IS_FULL, IT_MGR::CHAT_TYPE_SYSTEM );
							else
								g_pNet->Send_cli_PARTY_REQ(PARTY_REQ_JOIN,  g_pObjMGR->Get_ServerObjectIndex(iTargetObjClientIndex ));
						}
						else///Error
						{
							g_itMGR.AppendChatMsg(STR_CANT_INVITE_TO_PARTY, IT_MGR::CHAT_TYPE_SYSTEM );
						}
					}
					else
					{
						g_pNet->Send_cli_PARTY_REQ(PARTY_REQ_MAKE,  g_pObjMGR->Get_ServerObjectIndex(iTargetObjClientIndex));
					}
				}
				else
				{
					g_itMGR.OpenMsgBox( STR_CANTPARTY_WITHOTHERTEAM_INPVPZONE );
				}
			}
			break;
		}
	case COMMON_COMMAND_EXCHANGE:
		{
			int iTargetObjClientIndex = g_UserInputSystem.GetCurrentTarget();

						
			if( iTargetObjClientIndex != g_pAVATAR->Get_INDEX()	)				// 타겟이 자신일 경우 안됨				
			{
				CObjAVT* pObjAVT = g_pObjMGR->Get_CharAVT( iTargetObjClientIndex , true );
				if( pObjAVT && !pObjAVT->IsRideUser() ) //홍근 : 보조석에 탄 아바타에게 거래 금지.
				{
					if( CExchange::GetInstance().SendCliTradeReq( g_pObjMGR->Get_ServerObjectIndex(iTargetObjClientIndex) ) )
					{
						string Temp = pObjAVT->Get_NAME();
						Temp.append(STR_REQ_TRADE);
						g_itMGR.AppendChatMsg( Temp.c_str(), IT_MGR::CHAT_TYPE_SYSTEM);
					}
				}
			}
			break;
		}
	case COMMON_COMMAND_PRIVATESTORE:
		{

//홍근 : 일본 빌링 수정
#ifdef __JAPAN_BILL
			if( !(CGame::GetInstance().GetPayType() & PLAY_FLAG_TRADE) )
			{
				g_itMGR.OpenMsgBox( STR_JP_BILL_CANT_OPEN_PRIVATESTORE );
				return false;
			}        			
#endif
			/// 주위 10미터 안에 NPC가 있다면 상점 개설 금지
			g_itMGR.OpenDialog( DLG_TYPE_PRIVATESTORE );
			g_itMGR.OpenDialog( DLG_TYPE_ITEM, false );
		}
		break;


	//20060713 홍근 : 다음타겟 일반 스킬 추가.
	case COMMON_COMMAND_15:
		Proc_BasicSkill_Next_Target( pSkillSlot, pSkill );
		break;
	}	
	return true;
}



//----------------------------------------------------------------------
/// Emotion expression command
//----------------------------------------------------------------------

CEmotionCommand::CEmotionCommand()
{
}

CEmotionCommand::~CEmotionCommand()
{
}

bool CEmotionCommand::Execute()
{	
	/// 기본명령들은 정지상태에서만 동작한다.
	if( g_pAVATAR->Get_STATE() != CS_STOP && g_pAVATAR->Get_STATE() != CS_SIT )
	{
		g_itMGR.AppendChatMsg( STR_ACTION_COMMAND_STOP_STATE_FAILED, IT_MGR::CHAT_TYPE_SYSTEM);
		return false;
	}

	/// get skill.
	CSkillSlot* pSkillSlot = g_pAVATAR->GetSkillSlot();
	CSkill* pSkill = pSkillSlot->GetSkill( m_iSlotNO );


	/// 
	/// !! 2005/01/10 서버에 모션 변경을 요청할때에는 모션타입번호를 요청한다.
	///


	// 오른손 무기 종류에따라...
	//short nWeaponTYPE = WEAPON_MOTION_TYPE( g_pAVATAR->Get_R_WEAPON() );
	int iActionType = SKILL_ANI_ACTION_TYPE( pSkill->GetSkillIndex() );

	///short nFileIDX = FILE_MOTION( nWeaponTYPE, iActionType );	

	if( iActionType != 0 )
		g_pNet->Send_cli_SET_MOTION( iActionType, 1 );

	return true;
}


//----------------------------------------------------------------------
/// Inventory slot command class
//----------------------------------------------------------------------

CInventoryCommand::CInventoryCommand()
{
}

CInventoryCommand::~CInventoryCommand()
{
}

bool CInventoryCommand::Execute()
{
/*	CInventoryDLG* pDlg = g_itMGR.GetAvtInventoryDLG();

	pDlg->UseItem( m_iSlotNO );
*/
	return true;
}


//----------------------------------------------------------------------
/// Normal command class
//----------------------------------------------------------------------

CNormalCommand::CNormalCommand()
{
}

CNormalCommand::~CNormalCommand()
{
}

bool CNormalCommand::Execute()
{

	return true;
}


//----------------------------------------------------------------------
/// Skill command class
//----------------------------------------------------------------------

CSkillCommand::CSkillCommand()
{
	m_iSlotNO = 0;
}

CSkillCommand::~CSkillCommand()
{
}

/// 타겟이 필요치 않은 스킬들( 셀프 관련 )
bool CSkillCommand::Execute()
{	
	//-------------------------------------------------------------------------------
	/// 앉기 상태에선 금지..
	//-------------------------------------------------------------------------------				
	if( g_pAVATAR->Get_STATE() == CS_SIT )
		return false;

	int iTarget = g_UserInputSystem.GetCurrentTarget();
	CObjCHAR* pObjCHAR = g_pObjMGR->Get_CharOBJ( iTarget, true );
    
	D3DXVECTOR3 vPosTO( 0.0f, 0.0f, 0.0f );

	if( pObjCHAR )
	{
		vPosTO = pObjCHAR->Get_CurPOS();
	}
	// 활성 스킬 작동...
	g_SkillManager.FireSkill( m_iSlotNO, g_UserInputSystem.GetCurrentTarget(), vPosTO );
	return true;
}
