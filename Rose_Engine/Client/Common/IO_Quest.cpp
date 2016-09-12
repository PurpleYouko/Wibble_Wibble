
#include "stdAFX.h"
#include "IO_SKILL.h"
#include "IO_QUEST.h"
#include "CUserDATA.h"
#include "CObjUSER.h"
#include "../GameData/CClan.h"
#include "../System/SystemProcScript.h"
#include "../Object.h"

#include "../GameCommon/Skill.h"
#include "../Util/ClassTIME.h"
#include "../Event/Quest_FUNC.h"


//---------------------------------------------------------------------------------------------
///
/// Q u e s t T a concrete description of the trigger o o l 2, see QuestInfo.h.
///
///
///
//---------------------------------------------------------------------------------------------


#ifndef	__SERVER
	#include "../Interface/it_mgr.h"
	#include "../System/CGame.h"

	#include "../Util/LogWnd.h"
	#include "..\GameProc\CDayNNightProc.h"

	#include "../Event/QuestRewardQueue.h"
#else
	#include "ZoneLIST.h"

	extern short Get_WorldTIME ();
#endif


t_HASHKEY Make_EventObjectID(int iZoneNO, int iMapX, int iMapY, int iEventID)
{
	CStrVAR stName(128);
	stName.Printf("%d@%dx%d_%d", iZoneNO, iMapX, iMapY, iEventID );
	return ::StrToHashKey( stName.Get() );
}


CQuestDATA g_QuestList;

//-------------------------------------------------------------------------------------------------
template <class dType1, class dType2>
inline bool Check_QuestOP( BYTE btOP, dType1 iLeft, dType2 iRight)
{
	bool bResult;
	switch( btOP ) {
		case 0 :	// Like
			bResult = ( iLeft == iRight );	break;
		case 1 :	// Large
			bResult = ( iLeft >  iRight );	break;
		case 2 :	// Greater than or equal to
			bResult = ( iLeft >= iRight );	break;
		case 3 :	// Small
			bResult = ( iLeft <  iRight );	break;
		case 4 :	// Less than or equal to.
			bResult = ( iLeft <= iRight );	break;
		case 10 :	// Not the same.
			bResult = ( iLeft != iRight );	break;
		default :
			{
#ifndef	__SERVER
				//--------------------------------------------------------------------------------
				LOGERR( "Check_QuestOP( ... ) OP TYPE Mismatch " );
				//--------------------------------------------------------------------------------
#endif
				return false;
			}
	}

#ifndef	__SERVER
	if ( !bResult ) 
	{
		//char *szOP[] = { "==", ">", ">=", "<", "<=" };
		//char *szMsg = CStr::Printf ("		[QST] Comparison operations fails = = d% s% d "%, > 
		//					(int)iLeft, szOP[ btOP ], (int)iRight );

		//--------------------------------------------------------------------------------
		//LOGWAR( "Check_QuestOP( ... ) FAILED[ %s ] ", szMsg );
		//--------------------------------------------------------------------------------
	}
#endif

	return bResult;
}


//-------------------------------------------------------------------------------------------------
#define	INT_FAILED	0x80000000
static int  Get_QuestVAR (tQST_PARAM *pPARAM, STR_QUEST_DATA *pDATA)
{
	switch( pDATA->m_wVarTYPE ) {
		case QST_VARTYPE_VAR	:
			return ( !pPARAM->m_pQUEST) ? INT_FAILED : pPARAM->m_pQUEST->Get_VAR( pDATA->m_wVarNO );
		case QST_VARTYPE_SWITCH	:
			return ( !pPARAM->m_pQUEST) ? INT_FAILED : pPARAM->m_pQUEST->Get_SWITCH( pDATA->m_wVarNO );
		case QST_VARTYPE_TIMER	:
			return ( !pPARAM->m_pQUEST) ? INT_FAILED : pPARAM->m_pQUEST->GetRemainTIME ();

		case QST_VARTYPE_EPISODE :
			return pPARAM->m_pOWNER->Quest_GetEpisodeVAR( pDATA->m_wVarNO );
		case QST_VARTYPE_JOB	 :
			return pPARAM->m_pOWNER->Quest_GetJobVAR( pDATA->m_wVarNO );
		case QST_VARTYPE_PLANET	 :
			return pPARAM->m_pOWNER->Quest_GetPlanetVAR( pDATA->m_wVarNO );
		case QST_VARTYPE_UNION	 :
			return pPARAM->m_pOWNER->Quest_GetUnionVAR( pDATA->m_wVarNO );
#ifdef __CLAN_WAR_SET
		////	Clancy I related quest setting. -Azadeh 2006.7.13 15:41
		case QST_VARTYPE_CLANWAR:
			return pPARAM->m_pOWNER->Quest_GetClanWarVAR( pDATA->m_wVarNO );
#endif

	}

#ifndef	__SERVER
	//--------------------------------------------------------------------------------
	LOGERR( "Get_QuestVAR( ... ) FAILED[ Value Type : %d, pPARAM->m_pQUEST: %d ] ", pDATA->m_wVarTYPE, pPARAM->m_pQUEST );
	//--------------------------------------------------------------------------------
#endif

	return INT_FAILED;
}

bool isQuestOverflowed = false;
static void Set_QuestVAR (tQST_PARAM *pPARAM, STR_QUEST_DATA *pDATA, int iValue)
{
	//if ( isQuestOverflowed )
	//{
	//	isQuestOverflowed = false;
	//	return;
	//}

	switch( pDATA->m_wVarTYPE ) {
		case QST_VARTYPE_VAR	:
			pPARAM->m_pQUEST->Set_VAR( pDATA->m_wVarNO, iValue );
			break;
		case QST_VARTYPE_SWITCH	:
			pPARAM->m_pQUEST->Set_SWITCH( pDATA->m_wVarNO, iValue );
			break;

			/*
		case QST_VARTYPE_TIMER	:
			pPARAM->m_pQUEST->Get_PassMIN ();
			break;
			*/

		case QST_VARTYPE_EPISODE :
			pPARAM->m_pOWNER->Quest_SetEpisodeVAR( pDATA->m_wVarNO, iValue );
			break;
		case QST_VARTYPE_JOB	 :
			pPARAM->m_pOWNER->Quest_SetJobVAR( pDATA->m_wVarNO, iValue );
			break;
		case QST_VARTYPE_PLANET	 :
			pPARAM->m_pOWNER->Quest_SetPlanetVAR( pDATA->m_wVarNO, iValue );
			break;
		case QST_VARTYPE_UNION	 :
			pPARAM->m_pOWNER->Quest_SetUnionVAR( pDATA->m_wVarNO, iValue );
			break;
#ifdef __CLAN_WAR_SET
		////	Setting the value for the clan I-related quests. -Azadeh 2006.7.13 15:41
		case QST_VARTYPE_CLANWAR:
			//pPARAM->m_pOWNER->Quest_SetClanWarVAR( pDATA->m_wVarNO,iValue );
			break;
#endif

		default:
			{
				#ifndef	__SERVER
				//--------------------------------------------------------------------------------
				LOGERR( "Set_QuestVAR( ... ) FAILED[ Value Type : %d ] ", pDATA->m_wVarTYPE );
				//--------------------------------------------------------------------------------
				#endif
			}
			break;
	}
}


//-------------------------------------------------------------------------------------------------
inline bool Check_QuestVAR (tQST_PARAM *pPARAM, STR_QUEST_DATA *pDATA)
{
	int iValue = ::Get_QuestVAR( pPARAM, pDATA );
	if ( iValue != INT_FAILED ) 
	{
#ifndef	__SERVER
		//--------------------------------------------------------------------------------
		LOGOUT( "Check_QuestVAR[ Get_QuestVAR : %d ]  ", iValue );
		//--------------------------------------------------------------------------------
#endif

		return ::Check_QuestOP( pDATA->btOp, iValue, pDATA->nValue );
	}

	#ifndef	__SERVER
	if ( !pPARAM->m_pQUEST ) 
	{
		char *szMsg = CStr::Printf ("	[QST] Database error!!! :: Quest a quest that is selected in the variable lookup:% s \n", pPARAM->m_pCurrentTRIGGER->m_Name.Get() );

		//--------------------------------------------------------------------------------
		LOGERR( "Check_QuestVAR ERROR[ %s ] ", szMsg );
		//--------------------------------------------------------------------------------

		g_itMGR.OpenMsgBox( szMsg );
	}
	#endif

	return false;
}
inline bool Check_UserVAR (tQST_PARAM *pPARAM, STR_ABIL_DATA *pDATA)
{
	if ( AT_CLASS == pDATA->iType ) 
	{
		// Job views: enter the wrong data in the quest tools can be...
		if ( pDATA->iValue < 0 || pDATA->iValue >= g_TblClass.m_nDataCnt ) 
		{
		#ifndef	__SERVER
			char *szMsg = CStr::Printf ("	[QST] Database error!!! :% S% d's job number.", pPARAM->m_pCurrentTRIGGER->m_Name.Get(), pDATA->iValue );

			//--------------------------------------------------------------------------------
			LOGOUT( "Check_UserVAR ERROR[ %s ]  ", szMsg );
			//--------------------------------------------------------------------------------

			g_itMGR.OpenMsgBox( szMsg );
		#endif
			return false;
		}

		return pPARAM->m_pOWNER->Check_JobCollection( pDATA->iValue );
	}

	int iValue = pPARAM->m_pOWNER->GetCur_AbilityValue( pDATA->iType );
	return ::Check_QuestOP( pDATA->btOp, iValue, pDATA->iValue );
}
static bool Check_QuestITEM (tQST_PARAM *pPARAM, STR_ITEM_DATA *pDATA)
{
	tagITEM sITEM, *pFindITEM;

	sITEM.Init( pDATA->uiItemSN );

	if ( pDATA->iWhere >= EQUIP_IDX_FACE_ITEM && pDATA->iWhere < MAX_EQUIP_IDX ) 
	{
		// Check the mounting equipment...
		pFindITEM = pPARAM->m_pOWNER->Quest_FindEquipITEM( pDATA->iWhere );
		if ( pFindITEM && pFindITEM->GetHEADER() == sITEM.GetHEADER() )
		{
			return true;
		}else
		{
#ifndef	__SERVER
			if( pFindITEM )
			{
				//--------------------------------------------------------------------------------
				LOGWAR( "Check_QuestITEM FAILED[ Check the mounting equipment: %d ]", pFindITEM->GetItemNO() );
				//--------------------------------------------------------------------------------
			}else
			{
				//--------------------------------------------------------------------------------
				LOGWAR( "Check_QuestITEM FAILED[ 장착 장비 체크 : pFINEITEM is NULL: 체크하고자하는 아이템입력이 이상하군. ]" );
				//--------------------------------------------------------------------------------
			}
#endif
		}
	} else 
	if ( ITEM_TYPE_QUEST == sITEM.GetTYPE() ) 
	{
		if ( NULL == pPARAM->m_pQUEST ) 
		{	// Due to the wrong quest database...
#ifndef	__SERVER
			if ( !pPARAM->m_pQUEST ) 
			{
				char *szMsg = CStr::Printf ("	[QST] Database error!!! :: Quest item comparison:% s, no quest that is selected in the", pPARAM->m_pCurrentTRIGGER->m_Name.Get() );

				//--------------------------------------------------------------------------------
				DUMPERR( szMsg )( "Check_QuestITEM ERROR[ See data errors, dump data ]" );
				//--------------------------------------------------------------------------------

				g_itMGR.OpenMsgBox( szMsg );
			}
#endif
			return false;
		}

		// Quest inventory check...
		pFindITEM = pPARAM->m_pQUEST->GetSameITEM( sITEM.GetHEADER() );
		if ( pFindITEM ) 
		{
			return ::Check_QuestOP( pDATA->btOp, pFindITEM->GetQuantity(), (UINT)pDATA->iRequestCnt );
		} else {
			// If you do not have zero and compare items...
			return ::Check_QuestOP( pDATA->btOp, (UINT)0, (UINT)pDATA->iRequestCnt );
		}
	} else {
		// Compared to the General inventory...
		pFindITEM = pPARAM->m_pOWNER->Quest_FindITEM( sITEM );
		if ( pFindITEM )
		{
			if ( pFindITEM->IsEnableDupCNT() ) {
				return ::Check_QuestOP( pDATA->btOp, pFindITEM->GetQuantity(), (UINT)pDATA->iRequestCnt );
			} else {
				// Number of duplicate items that do not apply to one..
				return ::Check_QuestOP( pDATA->btOp, (UINT)1, (UINT)pDATA->iRequestCnt );
			}
		} else {
			// If you do not have zero and compare items...
			return ::Check_QuestOP( pDATA->btOp, (UINT)0, (UINT)pDATA->iRequestCnt );
		}
	}

	return false;
}

//-------------------------------------------------------------------------------------------------
#ifdef	__SERVER
static bool QST_Set_ObjVAR (CObjVAR *pObjVAR, tagValueQST *pVALUE)
{
	if ( pVALUE->nVarNo < 0 || pVALUE->nVarNo >= MAX_OBJ_VAR_CNT )
	{
#ifndef	__SERVER
		//--------------------------------------------------------------------------------
		LOGERR( "QST_Set_ObjVAR ERROR[ Data errors : pVALUE->nVarNo < 0 || pVALUE->nVarNo >= MAX_OBJ_VAR_CNT ]" );
		//--------------------------------------------------------------------------------
#endif
		return false;
	}

	int iValue = pObjVAR->Get_ObjVAR( (BYTE)pVALUE->nVarNo );
	switch( pVALUE->btOp ) 
	{
		case 5 :	// Value breaks
			pObjVAR->Set_ObjVAR( (BYTE)pVALUE->nVarNo, pVALUE->iValue );
			break;
		case 6 :	// Increase (given as)
			pObjVAR->Set_ObjVAR( (BYTE)pVALUE->nVarNo, iValue + pVALUE->iValue );
			break;
		case 7 :	// Reduction
			pObjVAR->Set_ObjVAR( (BYTE)pVALUE->nVarNo, iValue - pVALUE->iValue );
			break;
	}

	return true;
}
#endif

//-------------------------------------------------------------------------------------------------
static bool Set_QuestVAR (tQST_PARAM *pPARAM, STR_QUEST_DATA *pDATA)
{
	int iValue = ::Get_QuestVAR( pPARAM, pDATA );
	if ( iValue == INT_FAILED ) 
	{
	#ifndef	__SERVER
		if ( !pPARAM->m_pQUEST ) {
			char *szMsg = CStr::Printf ("	[QST] Database error!!! :: Quest set:% s of the variable that is selected in the quest: none", pPARAM->m_pCurrentTRIGGER->m_Name.Get() );

			//--------------------------------------------------------------------------------
			DUMPERR( szMsg )( "Set_QuestVAR ERROR[ Data error: dump]" );
			//--------------------------------------------------------------------------------

			g_itMGR.OpenMsgBox( szMsg );
		}
	#endif
		return false;
	}

	switch( pDATA->btOp ) {
		case 5 :	// Value breaks
			::Set_QuestVAR( pPARAM, pDATA, pDATA->nValue );
			break;
		case 6 :	// Increase (given as)
			::Set_QuestVAR( pPARAM, pDATA, iValue + pDATA->nValue );
			break;
		case 7 :	// Reduction
			::Set_QuestVAR( pPARAM, pDATA, iValue - pDATA->nValue );
			break;
		case 8 :	// Switch Off
			::Set_QuestVAR( pPARAM, pDATA, 0 );
			break;
		case 9 :	// Switch On
			::Set_QuestVAR( pPARAM, pDATA, 1 );
			break;
	}

	return true;
}
//-------------------------------------------------------------------------------------------------
bool F_QSTCOND000 ( uniQstENTITY *pCOND, tQST_PARAM *pPARAM )
{
// * Being the current quests
	BYTE btQuestSLOT = pPARAM->m_pOWNER->Quest_GetRegistered( pCOND->m_Cond000.iQuestSN );
	if ( btQuestSLOT >= QUEST_PER_PLAYER )
	{
#ifndef	__SERVER
		//--------------------------------------------------------------------------------
		LOGERR( "[ %s ] _QSTCOND000[ Being the current quests ] ERROR[ btQuestSLOT >= QUEST_PER_PLAYER ]", pPARAM->m_pCurrentTRIGGER->m_Name.Get() );
		//--------------------------------------------------------------------------------
#endif
		return false;
	}

	pPARAM->m_btQuestSLOT = btQuestSLOT;
	pPARAM->m_pQUEST = &pPARAM->m_pOWNER->m_Quests.m_QUEST[ btQuestSLOT ];

	if ( NULL == pPARAM->m_pQUEST ) 
	{
#ifndef	__SERVER
		char *szMsg = CStr::Printf ("	[QST] %sData error [condition type:%d ] !!! :: None of the selected quest", 
					pPARAM->m_pCurrentTRIGGER->m_Name.Get(), pCOND->iType );

		//--------------------------------------------------------------------------------
		DUMPERR( szMsg )( "_QSTCOND000[ Being the current quests ] ERROR[ Data errors : 덤프 참조 ]" );
		//--------------------------------------------------------------------------------

#endif
		return false;
	}

	return true;
} 
bool F_QSTCOND001 ( uniQstENTITY *pCOND, tQST_PARAM *pPARAM  )		
{	
// * Quest data check
	for (int iL=0; iL<pCOND->m_Cond001.iDataCnt; iL++) 
	{
		if ( !::Check_QuestVAR(pPARAM, &pCOND->m_Cond001.CheckData[ iL ] ) ) 
		{
			pPARAM->m_nErrSTEP = iL;

#ifndef	__SERVER
			//--------------------------------------------------------------------------------
			LOGWAR( "[ %s ] F_QSTCOND001[ Quest data check ] FAILED ", pPARAM->m_pCurrentTRIGGER->m_Name.Get() );
			//--------------------------------------------------------------------------------
#endif

			return false;
		}
	}

	return true;
} 
bool F_QSTCOND002 ( uniQstENTITY *pCOND, tQST_PARAM *pPARAM  )
{	
// * Quest progress variable check
	for (int iL=0; iL<pCOND->m_Cond002.iDataCnt; iL++) 
	{
		if ( !::Check_QuestVAR(pPARAM, &pCOND->m_Cond002.CheckData[ iL ] ) ) 
		{
			pPARAM->m_nErrSTEP = iL;

#ifndef	__SERVER
			//--------------------------------------------------------------------------------
			LOGWAR( "[ %s ] F_QSTCOND002[ Quest progress variable check ] FAILED ", pPARAM->m_pCurrentTRIGGER->m_Name.Get() );
			//--------------------------------------------------------------------------------
#endif

			return false;
		}
	}

	return true;	
} 
bool F_QSTCOND003 ( uniQstENTITY *pCOND, tQST_PARAM *pPARAM  )
{	
// * Check stats
	for (int iL=0; iL<pCOND->m_Cond003.iDataCnt; iL++) 
	{
		if ( !::Check_UserVAR( pPARAM, &pCOND->m_Cond003.CheckData[ iL ] ) ) 
		{
			pPARAM->m_nErrSTEP = iL;
	
#ifndef	__SERVER
			//--------------------------------------------------------------------------------
			LOGWAR( "[ %s ] F_QSTCOND003[ Check stats ] FAILED ", pPARAM->m_pCurrentTRIGGER->m_Name.Get() );
			//--------------------------------------------------------------------------------
#endif
			return false;
		}
	}

	return true;
} 
bool F_QSTCOND004 ( uniQstENTITY *pCOND, tQST_PARAM *pPARAM  )
{
// * Possession/mounting check the item (s)
	for (int iL=0; iL<pCOND->m_Cond004.iDataCnt; iL++) 
	{
		if ( !::Check_QuestITEM( pPARAM, &pCOND->m_Cond004.CheckData[ iL ] ) ) 
		{
			pPARAM->m_nErrSTEP = iL;

#ifndef	__SERVER
			//--------------------------------------------------------------------------------
			LOGWAR( "[ %s ] F_QSTCOND004[ Possession/equipped Check ] FAILED ", pPARAM->m_pCurrentTRIGGER->m_Name.Get() );
			//--------------------------------------------------------------------------------
#endif

			return false;
		}
	}

	return true;	
} 
bool F_QSTCOND005 ( uniQstENTITY *pCOND, tQST_PARAM *pPARAM  )
{	
// * Party check
	if ( pCOND->m_Cond005.btIsLeader ) 
	{
		if ( !pPARAM->m_pOWNER->Quest_IsPartyLEADER () )
		{
#ifndef	__SERVER
			//--------------------------------------------------------------------------------
			LOGWAR( "[ %s ] F_QSTCOND005[ Party check ] FAILED ", pPARAM->m_pCurrentTRIGGER->m_Name.Get() );
			//--------------------------------------------------------------------------------
#endif
			return false;
		}
	}

	if ( pPARAM->m_pOWNER->Quest_GetPartyLEVEL() >= pCOND->m_Cond005.iLevel )
		return true;

#ifndef	__SERVER
		char *szMsg = CStr::Printf ("pPARAM->m_pOWNER->Quest_GetPartyLEVEL() >= pCOND->m_Cond005.iLevel" );
		//--------------------------------------------------------------------------------
		DUMPWAR( szMsg ) ( "[ %s ] F_QSTCOND005[ Party check ] FAILED[ See the dump ] ", pPARAM->m_pCurrentTRIGGER->m_Name.Get() );
		//--------------------------------------------------------------------------------
#endif

	return false;	
} 
bool F_QSTCOND006 ( uniQstENTITY *pCOND, tQST_PARAM *pPARAM  )
{	
	if ( !pPARAM->m_pOWNER ) {
#ifndef	__SERVER
		_ASSERT( 0 );	// Made the wrong data...
#endif
		return false;
	}

// * Position check
	if ( pPARAM->m_pOWNER->Quest_GetZoneNO() != pCOND->m_Cond006.iZoneSN )
	{
#ifndef	__SERVER
		//--------------------------------------------------------------------------------
		LOGWAR( "[ %s ] F_QSTCOND006[ Position check ] FAILED ", pPARAM->m_pCurrentTRIGGER->m_Name.Get() );
		//--------------------------------------------------------------------------------
#endif
		return false;
	}

	if ( pPARAM->m_pOWNER->Quest_DistanceFrom( pCOND->m_Cond006.iX, pCOND->m_Cond006.iY, pCOND->m_Cond006.iZ ) <= ( pCOND->m_Cond006.iRadius * 100 ) )
		return true;
	
#ifndef	__SERVER
		char *szMsg = CStr::Printf ( "pPARAM->m_pOWNER->Quest_DistanceFrom( pCOND->m_Cond006.iX, pCOND->m_Cond006.iY, pCOND->m_Cond006.iZ ) <= pCOND->m_Cond006.iRadius" );
		//--------------------------------------------------------------------------------
		DUMPWAR( szMsg ) ( "[ %s ] F_QSTCOND006[ Position check ] FAILED[ See the dump ] ", pPARAM->m_pCurrentTRIGGER->m_Name.Get() );
		//--------------------------------------------------------------------------------
#endif

	return false;	
} 

bool F_QSTCOND007 ( uniQstENTITY *pCOND, tQST_PARAM *pPARAM  )
{	
/*
 * World Vision check
struct STR_COND_007
{
	unsigned int	uiSize;
	int				iType;

	unsigned long	ulTime;			/// 
	unsigned long	ulEndTime;		/// On the basis of world vision every day 00:00 More than ulEndTime or less is the ulTime ?
};
*/
	WORD wWorldTIME = (WORD) ::Get_WorldTIME ();

	if ( wWorldTIME >= pCOND->m_Cond007.ulTime &&
		 wWorldTIME <= pCOND->m_Cond007.ulEndTime )
		return true;

#ifndef	__SERVER
	//--------------------------------------------------------------------------------
	LOGWAR( "[ %s ] F_QSTCOND007[ World Vision check ] FAILED ", pPARAM->m_pCurrentTRIGGER->m_Name.Get() );
	//--------------------------------------------------------------------------------
#endif

	return false;	
}

bool F_QSTCOND008 ( uniQstENTITY *pCOND, tQST_PARAM *pPARAM  )
{	
/*
 * Registration check remaining time quest
struct STR_COND_008
{
	unsigned int	uiSize;
	int				iType;

	unsigned long	ulTime;			/// The value of the check (ulTime Op quest registration hours)
	BYTE			btOp;			/// Comparison of methods for ulTime. 0 = 1 =, 2 = greater than or equal to, is greater. 3 = small, 4 = less than or equal to.
};
*/
	if ( NULL == pPARAM->m_pQUEST )	// Due to the wrong quest database...
	{
#ifndef	__SERVER
		//--------------------------------------------------------------------------------
		LOGERR( "[ %s ] F_QSTCOND008[ Register performs ERROR checking the remaining time] the quest [wrong] ", pPARAM->m_pCurrentTRIGGER->m_Name.Get() );
		//--------------------------------------------------------------------------------
#endif
		return false;
	}

	return ::Check_QuestOP( pCOND->m_Cond008.btOp, pPARAM->m_pQUEST->GetRemainTIME(), pCOND->m_Cond008.ulTime );
} 

bool F_QSTCOND009 ( uniQstENTITY *pCOND, tQST_PARAM *pPARAM  )
{
	/*
	/// Skill and check
	struct STR_COND_009
	{
	unsigned int	uiSize;
	int				iType;

	int				iSkillSN1;
	int				iSkillSN2;	/// iSkillSN1 ~ iSkillSN2 Once you have the skill/?
	BYTE			btOp;		/// 0 = no check, 1 = check
	} ;
	*/


	short nI, nT, nTab1, nTab2;

#ifdef __KRRE_NEW_AVATAR_DB
	//Later, seemed to be, even if the process without the preprocessor 
	//To check all the skill..

	if ( 0 == pCOND->m_Cond009.btOp ) 
	{
		// Check that there are no

		for (nI=0; nI<MAX_LEARNED_SKILL_CNT; nI++) 
		{
			if ( pPARAM->m_pOWNER->m_Skills.m_nSkillINDEX[ nI ] >= pCOND->m_Cond009.iSkillSN1 &&
				pPARAM->m_pOWNER->m_Skills.m_nSkillINDEX[ nI ] <= pCOND->m_Cond009.iSkillSN1 )
			{
				return false;
			}
		}
		return true;
	}

	// Check to see if...

	for (nI=0; nI<MAX_LEARNED_SKILL_CNT; nI++) 
	{
		if ( pPARAM->m_pOWNER->m_Skills.m_nSkillINDEX[ nI ] >= pCOND->m_Cond009.iSkillSN1 &&
			pPARAM->m_pOWNER->m_Skills.m_nSkillINDEX[ nI ] <= pCOND->m_Cond009.iSkillSN1 )
		{
			return true;
		}
	}

	return false;

#endif

	nTab1 = SKILL_TAB_TYPE( pCOND->m_Cond009.iSkillSN1 );
	nTab2 = SKILL_TAB_TYPE( pCOND->m_Cond009.iSkillSN2 );


	if ( 0 == pCOND->m_Cond009.btOp ) 
	{
		// Check that there are no
		for (nT=nTab1; nT<=nTab2; nT++) 
		{
			for (nI=0; nI<MAX_LEARNED_SKILL_PER_PAGE; nI++) 
			{
				if ( pPARAM->m_pOWNER->m_Skills.m_nIndex[ nT ][ nI ] >= pCOND->m_Cond009.iSkillSN1 &&
					pPARAM->m_pOWNER->m_Skills.m_nIndex[ nT ][ nI ] <= pCOND->m_Cond009.iSkillSN1 )
				{
#ifndef	__SERVER
					//--------------------------------------------------------------------------------
					LOGWAR( "[ %s ] F_QSTCOND009[ Skill and check] FAILED [skill: none] ", pPARAM->m_pCurrentTRIGGER->m_Name.Get() );
					//--------------------------------------------------------------------------------
#endif
					return false;
				}
			}
		}
		return true;
	}

	// Check to see if...
	for (nT=nTab1; nT<=nTab2; nT++) 
	{
		for (nI=0; nI<MAX_LEARNED_SKILL_PER_PAGE; nI++) 
		{
			if ( pPARAM->m_pOWNER->m_Skills.m_nIndex[ nT ][ nI ] >= pCOND->m_Cond009.iSkillSN1 &&
				pPARAM->m_pOWNER->m_Skills.m_nIndex[ nT ][ nI ] <= pCOND->m_Cond009.iSkillSN1 )
			{
				return true;
			}
		}
	}

#ifndef	__SERVER
	//--------------------------------------------------------------------------------
	LOGWAR( "[ %s ] F_QSTCOND009[ Skill and check] FAILED [skill: none] ", pPARAM->m_pCurrentTRIGGER->m_Name.Get() );
	//--------------------------------------------------------------------------------
#endif

	return false;
}
bool F_QSTCOND010 ( uniQstENTITY *pCOND, tQST_PARAM *pPARAM  )
{
#ifndef	__SERVER
	// Len Bush only check the chances of Klein...
	int iRand = rand() % 101;

	if ( iRand < pCOND->m_Cond010.btLowPcnt ||
		 iRand > pCOND->m_Cond010.btHighPcnt )
	{
		return false;
	}
#endif

	return true;
}

bool F_QSTCOND011 ( uniQstENTITY *pCOND, tQST_PARAM *pPARAM  )	
{	
/*
/// NPC Check the value of a variable
typedef struct	tagValue
{
	unsigned int	uiSize;
	int				Type;

	BYTE			btWho;	// 0:NPC, 1:EventOBJ

	short			nVarNo;	// A number of variables: 0 ~, John's economic data variables
	int				iValue;	// Compare data values (iType of Op iValue)
	BYTE			btOp;	// Comparison of methods for the value of the iValue. 0 = 1 =, 2 = greater than or equal to, is greater. 3 = small, 4 = less than or equal to.
							// (Action), an increase of 5 = value breaks, 6 = 7 = reduction
} STR_COND_011, STR_REWD_011;
*/
#ifdef	__SERVER
	int iValue;
	if ( pCOND->m_Cond011.btWho ) 
	{
		// evnet object
		if ( NULL == pPARAM->m_pEventVAR ) {
			pPARAM->m_bServerFUNC = true;
			return false;
		}
		iValue = pPARAM->m_pEventVAR->Get_ObjVAR( (BYTE)pCOND->m_Cond011.nVarNo );
	} else {
		// npc
		if ( NULL == pPARAM->m_pNpcVAR ) {
			pPARAM->m_bServerFUNC = true;
			return false;
		}

		iValue = pPARAM->m_pNpcVAR->Get_ObjVAR( (BYTE)pCOND->m_Cond011.nVarNo );
	}

	return ::Check_QuestOP( pCOND->m_Cond011.btOp, iValue,pCOND->m_Cond011.iValue );
#else
	return true;
#endif
}
bool F_QSTCOND012 ( uniQstENTITY *pCOND, tQST_PARAM *pPARAM  )	
{
/*
/// Select the event object
struct STR_COND_012
{
	unsigned int	uiSize;
	int				iType;

	short			iZone;
	int				iX;
	int				iY;
	int				iEventID;
};
*/
#ifdef	__SERVER
	pPARAM->m_pEventVAR = g_pZoneLIST->Get_EventOBJ( pCOND->m_Cond012.iZone, pCOND->m_Cond012.m_HashEventOBJ );
	if ( NULL == pPARAM->m_pEventVAR ) {
		pPARAM->m_bServerFUNC = true;
		return false;
	}
#endif
	return true;
}

bool F_QSTCOND013 ( uniQstENTITY *pCOND, tQST_PARAM *pPARAM  )	
{
/*
/// NPC Select
struct STR_COND_013
{
	unsigned int	uiSize;
	int				iType;

	int				iNpcNo; /// NPC number, select
};
*/
#ifndef	__SERVER
	return true;
#else
	pPARAM->m_pNpcVAR = g_pZoneLIST->Get_NpcVAR( pCOND->m_Cond013.iNpcNo );
	return ( NULL != pPARAM->m_pNpcVAR );
#endif
}

bool F_QSTCOND014 ( uniQstENTITY *pCOND, tQST_PARAM *pPARAM  )	
{
/*
/// Switch Check
struct STR_COND_014
{
	unsigned int	uiSize;
	int				iType;

	short			nSN; // 0 ~ 255
	BYTE			btOp; // 0 = Off 1 = On, check whether the check, whether the
};
*/
	if ( !pPARAM->m_pOWNER ) {
		_ASSERT( 0 );
		return false;
	}

	return ( pCOND->m_Cond014.btOp == pPARAM->m_pOWNER->m_Quests.Get_SWITCH( pCOND->m_Cond014.nSN ) );
}

bool F_QSTCOND015 ( uniQstENTITY *pCOND, tQST_PARAM *pPARAM  )
{
/*
/// Party personnel check. 
struct STR_COND_015
{
	unsigned int	uiSize;
	int				iType;

	short			nNumber1;
	short			nNumber2; // nNumber1 <= Head of the party <= nNumber2 , true
							  // Non-party check to the party-0 = 0 = < < true judged as being
};
*/
	if ( !pPARAM->m_pOWNER )
		return false;

	return ( pPARAM->m_pOWNER->Quest_PartyMemberCNT() >= pCOND->m_Cond015.nNumber1 &&
			 pPARAM->m_pOWNER->Quest_PartyMemberCNT() <= pCOND->m_Cond015.nNumber2 );
}
bool F_QSTCOND016 ( uniQstENTITY *pCOND, tQST_PARAM *pPARAM  )
{
/*
/// John Visual check
struct STR_COND_016
{
	unsigned int	uiSize;
	int				iType;

	BYTE			btWho;	/// 0 = the currently selected NPC, 1 = the specified in the condition Event, 2 = user

	unsigned long	ulTime;		//
	unsigned long	ulEndTime;	// ulTime <= Currently John vision <= ulEndTime , true
};
*/
	unsigned int uiZoneTIME;
#ifndef	__SERVER
	uiZoneTIME = g_DayNNightProc.GetZoneTime();
#else
	switch( pCOND->m_Cond016.btWho ) {
		case 0 :	// npc
			_ASSERT( pPARAM->m_pNpcVAR );
			uiZoneTIME = (UINT)pPARAM->m_pNpcVAR->VGetCur_ZoneTIME();
			break;
		case 1 :	// evnet
			_ASSERT( pPARAM->m_pEventVAR );
			uiZoneTIME = (UINT)pPARAM->m_pEventVAR->VGetCur_ZoneTIME();
			break;
		case 2 :	// owner
			_ASSERT( pPARAM->m_pOWNER );
			uiZoneTIME = (UINT)pPARAM->m_pOWNER->GetCur_ZoneTIME();
			break;
	} 
#endif
	return ( uiZoneTIME >= pCOND->m_Cond016.ulTime && uiZoneTIME <= pCOND->m_Cond016.ulEndTime );
}

bool F_QSTCOND017 ( uniQstENTITY *pCOND, tQST_PARAM *pPARAM  )
{
/*
/// NPC Variable comparison. (NPC1 A variable OP NPC2 C Variable)
struct STR_COND_017
{
	unsigned int	uiSize;
	int				iType;

	STR_NPCVAR		NpcVar1;			
	STR_NPCVAR		NpcVar2;			
	BYTE			btOp; // The same .  <NpcVar1.nVarNo The value of the > btOp < NpcVar2.nVarNo The value of the>
*/
#ifdef	__SERVER
	CObjVAR *pNpc = g_pZoneLIST->Get_NpcVAR( pCOND->m_Cond017.NpcVar1.iNpcNo );

	short nL, nR;
	nL = pNpc->Get_ObjVAR( pCOND->m_Cond017.NpcVar1.nVarNo );

	if ( pCOND->m_Cond017.NpcVar1.iNpcNo != pCOND->m_Cond017.NpcVar2.iNpcNo ) {
		pNpc = g_pZoneLIST->Get_NpcVAR( pCOND->m_Cond017.NpcVar2.iNpcNo );
	}

	nR = pNpc->Get_ObjVAR( pCOND->m_Cond017.NpcVar2.nVarNo );

	return ::Check_QuestOP( pCOND->m_Cond017.btOp, nL, nR );
#else
	return true;
#endif
}

bool F_QSTCOND018 ( uniQstENTITY *pCOND, tQST_PARAM *pPARAM  )
{
/*
/// Date + Visual check
struct STR_COND_018
{
	unsigned int	uiSize;
	int				iType;

	BYTE			btDate;		// Date. (1 ~ 31)

	BYTE			btHour1;	// : (1 ~ 24)
	BYTE			btMin1;		// Min. (1 ~ 60)
	BYTE			btHour2;	// :
	BYTE			btMin2;		// Min.
	//  The current date is t H b o u btDate, r t M i n 1 minute 1: b current time t H b < = o = u r < 1: b t M i n one minute true
};
*/
#ifdef	__SERVER
	SYSTEMTIME sTIME;

	::GetLocalTime( &sTIME );

	if ( pCOND->m_Cond018.btDate && sTIME.wDay != pCOND->m_Cond018.btDate )
		return false;
	
	WORD wMin = sTIME.wHour * 60 + sTIME.wMinute;
	WORD wFrom, wTo;

	wFrom = pCOND->m_Cond018.btHour1 * 60 + pCOND->m_Cond018.btMin1;
	wTo   = pCOND->m_Cond018.btHour2 * 60 + pCOND->m_Cond018.btMin2;
	if ( wMin < wFrom || wMin > wTo )
		return false;
#endif
	return true;
}


bool F_QSTCOND019 ( uniQstENTITY *pCOND, tQST_PARAM *pPARAM  )
{
/*
/// Day + visual check
struct STR_COND_019
{
	unsigned int	uiSize;
	int				iType;

	BYTE			btWeekDay;	// Day of the week (0 ~ 6)

	BYTE			btHour1;	// :
	BYTE			btMin1;		// Min.
	BYTE			btHour2;	
	BYTE			btMin2;		
	//  The current day of the week btWeekDay t H b o u r, b M i n 1:1 min. t current time t H b < = o = u r < 1: b t M i n one minute true
};
*/
#ifdef	__SERVER
	SYSTEMTIME sTIME;

	::GetLocalTime( &sTIME );
	if ( sTIME.wDayOfWeek != pCOND->m_Cond019.btWeekDay )
		return false;

	WORD wMin = sTIME.wHour * 60 + sTIME.wMinute;
	WORD wFrom, wTo;

	wFrom = pCOND->m_Cond019.btHour1 * 60 + pCOND->m_Cond019.btMin1;
	wTo   = pCOND->m_Cond019.btHour2 * 60 + pCOND->m_Cond019.btMin2;
	if ( wMin < wFrom || wMin > wTo )
		return false;
#endif
	return true;
}

bool F_QSTCOND020 ( uniQstENTITY *pCOND, tQST_PARAM *pPARAM  )
{
	if ( !pPARAM->m_pOWNER )
		return false;

	
	CObjUSER* pUser = (CObjUSER*)( pPARAM->m_pOWNER );

	return ( pUser->Get_TeamNO() >= pCOND->m_Cond020.iNo1 &&
		 pUser->Get_TeamNO() <= pCOND->m_Cond020.iNo2 );

	return true;
}

bool F_QSTCOND021 ( uniQstENTITY *pCOND, tQST_PARAM *pPARAM  )
{
	return true;
}

bool F_QSTCOND022 ( uniQstENTITY *pCOND, tQST_PARAM *pPARAM  )
{
	return true;
}

//-------------------------------------------------------------------------------------
//
// Clan related
//
//-------------------------------------------------------------------------------------

bool F_QSTCOND023 ( uniQstENTITY *pCOND, tQST_PARAM *pPARAM  )
{
	// No additional data check-sign up for a clan
	//struct STR_COND_023
	//{
	//	unsigned int	uiSize;
	//	int				iType;

	//	BYTE			btReg;	// 0: non-subscribers, if you're a true true, 1: subscribers
	//};

	if ( !pPARAM->m_pOWNER )
		return false;

	
	CObjUSER* pUser = (CObjUSER*)( pPARAM->m_pOWNER );

	switch( pCOND->m_Cond023.btReg )
	{
	case 0:///Non-subscribers?
		if( pUser->GetClanID() )
			return false;
		return true;
	case 1:///Subscriber?
		if( pUser->GetClanID() )
			return true;
		return false;
	default:
		assert( 0 && "F_QSTCOND023::Invalid btReg Value" );
		return false;
	}
	return false;
	//return ( pCOND->m_Cond023.btReg ^ (( pUser->GetClanID() )? 1: 0 ) )?true:false;
}


bool F_QSTCOND024 ( uniQstENTITY *pCOND, tQST_PARAM *pPARAM  )
{
	// Clancy title check
	//struct STR_COND_024
	//{
	//	unsigned int	uiSize;
	//	int				iType;

	//	short			nPOS;	// Title number
	//	BYTE			btOP;	// 0 = Like, 1 = Large, 2 = Greater than or equal to. 3 = Small, 4 = Less than or equal to.
	//};

	if ( !pPARAM->m_pOWNER )
		return false;

	
	CObjUSER* pUser = (CObjUSER*)( pPARAM->m_pOWNER );

	return Check_QuestOP( pCOND->m_Cond024.btOP, pUser->GetClanPos(), pCOND->m_Cond024.nPOS );
}


bool F_QSTCOND025 ( uniQstENTITY *pCOND, tQST_PARAM *pPARAM  )
{
	// Check your contributions clan
	//struct STR_COND_025
	//{
	//	unsigned int	uiSize;
	//	int				iType;

	//	short			nCONT;	// Check your contributions clan
	//	BYTE			btOP;	// 0 = Like, 1 = Large, 2 = Greater than or equal to. 3=Small, 4=Less than or equal to.
	//};

	if ( !pPARAM->m_pOWNER )
		return false;

	
	CObjUSER* pUser = (CObjUSER*)( pPARAM->m_pOWNER );

	return Check_QuestOP( pCOND->m_Cond025.btOP, CClan::GetInstance().GetMyClanPoint(), pCOND->m_Cond025.nCONT );
}


bool F_QSTCOND026 ( uniQstENTITY *pCOND, tQST_PARAM *pPARAM  )
{
	// Grade check
	//struct STR_COND_026
	//{
	//	unsigned int	uiSize;
	//	int				iType;

	//	short			nGRD;	// Clan ratings
	//	BYTE			btOP;	// 0 = Like, 1 = Large, 2 = Greater than or equal to. 3=Small, 4=Less than or equal to.
	//};

	if ( !pPARAM->m_pOWNER )
		return false;

	
	CObjUSER* pUser = (CObjUSER*)( pPARAM->m_pOWNER );
	return Check_QuestOP( pCOND->m_Cond026.btOP, CClan::GetInstance().GetLevel(), pCOND->m_Cond026.nGRD );
}


bool F_QSTCOND027 ( uniQstENTITY *pCOND, tQST_PARAM *pPARAM  )
{
	// Score check
	//struct STR_COND_027
	//{
	//	unsigned int	uiSize;
	//	int				iType;

	//	short			nPOINT;	// Clancy score
	//	BYTE			btOP;	// 0 = Like, 1 = Large, 2 = Greater than or equal to. 3=Small, 4=Less than or equal to.
	//};

	if ( !pPARAM->m_pOWNER )
		return false;

	
	CObjUSER* pUser = (CObjUSER*)( pPARAM->m_pOWNER );

	return Check_QuestOP( pCOND->m_Cond027.btOP, CClan::GetInstance().GetPoint(), pCOND->m_Cond027.nPOINT );
}

bool F_QSTCOND028 ( uniQstENTITY *pCOND, tQST_PARAM *pPARAM  )
{
	// Money check
	//struct STR_COND_028
	//{
	//	unsigned int	uiSize;
	//	int				iType;

	//	int				iMONEY;	// Clan money
	//	BYTE			btOP;	// 0 = Like, 1 = Large, 2 = Greater than or equal to. 3=Small, 4=Less than or equal to.
	//};

	if ( !pPARAM->m_pOWNER )
		return false;

	return Check_QuestOP( pCOND->m_Cond028.btOP, CClan::GetInstance().GetMoney(), pCOND->m_Cond028.iMONEY );
	
}


bool F_QSTCOND029 ( uniQstENTITY *pCOND, tQST_PARAM *pPARAM  )
{
	// Personnel checks
	//struct STR_COND_029
	//{
	//	unsigned int	uiSize;
	//	int				iType;

	//	short			nMemberCNT;	// Clancy personnel
	//	BYTE			btOP;	// 0 = Like, 1 = Large, 2 = Greater than or equal to. 3=Small, 4=Less than or equal to.
	//};

	if ( !pPARAM->m_pOWNER )
		return false;

	
	CObjUSER* pUser = (CObjUSER*)( pPARAM->m_pOWNER );

	return Check_QuestOP( pCOND->m_Cond029.btOP, CClan::GetInstance().GetMemberCount(), pCOND->m_Cond029.nMemberCNT );
}


bool F_QSTCOND030 ( uniQstENTITY *pCOND, tQST_PARAM *pPARAM  )
{
	// Skill check
	//struct STR_COND_030
	//{
	//	unsigned int	uiSize;
	//	int				iType;

	//	short			nSkill1;
	//	short			nSkill2; // nSkill1 ~ nSkill2 Not sure/check the skill of
	//	BYTE			btOP;	// 0: Does not have ?  1: The ?
	//};

	if ( !pPARAM->m_pOWNER )
		return false;

	
	//CObjUSER* pUser = (CObjUSER*)( pPARAM->m_pOWNER );

	CClanSkillSlot* pSkillSlot = CClan::GetInstance().GetClanSkillSlot();
	if( pSkillSlot == NULL )
		return false;

	int iSkill1 =  pCOND->m_Cond030.nSkill1;
	int iSkill2 =  pCOND->m_Cond030.nSkill2;

	if( iSkill1 > iSkill2 )
	{
		int Temp = iSkill1;
		iSkill1 = iSkill2;
		iSkill2 = Temp;
	}

	/// iSkill1 From iSkill2 If you have a skill between the..
    for( int i = iSkill1 ; i <= iSkill2 ; i++ )
	{
		CSkill* pSkill = pSkillSlot->GetSkillBySkillIDX( i );
		if( pSkill )
		{
			if( pSkill->HasExpiredTime() )
			{
				DWORD dwExpiredTime = pSkill->GetExpiredTime();
				
				DWORD dwCurruntAbsSec = classTIME::GetCurrentAbsSecond ();
				if( dwExpiredTime < dwCurruntAbsSec )
					return false;
			}
			return true;
		}
	}

	return false;
}




bool F_QUEST_COND_NULL ( uniQstENTITY *pCOND, tQST_PARAM *pPARAM  )
{
	return true;
}
bool F_QUEST_REWD_NULL ( uniQstENTITY *pCOND, tQST_PARAM *pPARAM, bool bDoReward  )
{
	return true;
}





//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
bool F_QSTREWD000 ( uniQstENTITY *pREWD, tQST_PARAM *pPARAM, bool bDoReward )
{
	if ( !bDoReward )	// 05.05.21 icarus:: Building a list is unnecessary compensation...
		return true;

/// Quest registration/deletion request
	switch( pREWD->m_Rewd000.btOp ) 
	{
		case 0 :	// 삭제.
	
			#ifdef	__SERVER
				/// Will be automatically deleted by the quest ... I say leave it??
			#endif

			if ( NULL == pPARAM->m_pQUEST )	// Due to the wrong quest database...
			{

#ifndef	__SERVER
			//--------------------------------------------------------------------------------
			LOGWAR( "[ %s ] F_QSTREWD000[ Quest FAILED deletion requests] [wrong quest database ] ", pPARAM->m_pCurrentTRIGGER->m_Name.Get() );
			//--------------------------------------------------------------------------------
#endif
				return false;
			}

/*#ifndef __SERVER			
			g_itMGR.AppendChatMsg( CStr::Printf( 
										"%s Completed", 
										CStringManager::GetSingleton().GetQuestName( pREWD->m_Rewd000.iQuestSN ) 
										) , 
									IT_MGR::CHAT_TYPE_QUEST
								);
#endif	*/		
			pPARAM->m_pQUEST->Init ();	// pREWD->m_Rewd000.iQuestSN;
			break;

		case 1 :	// 등록.
			{
				// Begin - 2006-08-31  PM 5:21:41 - Azadeh: General quest slots 10 Dog -> 9 Use of modified
				short nSlotNO = pPARAM->m_pOWNER->Quest_Append( pREWD->m_Rewd000.iQuestSN );
				//short nSlotNO = pPARAM->m_pOWNER->Quest_Append_9( pREWD->m_Rewd000.iQuestSN );
				//isQuestOverflowed = ( nSlotNO == -1 ) ? true : false;
				// End   - 2006-08-31  PM 5:22:08 - Azadeh
				#ifdef	__SERVER
				if ( nSlotNO >= 0 ) 
				{
					// 퀘스트 등록 로그...
					// g_pSockLOG->When_RecvQUEST( pPARAM->m_pOWNER, pREWD->m_Rewd000.iQuestSN );

					if ( NULL == pPARAM->m_pQUEST ) {
						// If there is no currently selected quest forced into this current quest???
						pPARAM->m_btQuestSLOT = (BYTE)nSlotNO;
						pPARAM->m_pQUEST = &pPARAM->m_pOWNER->m_Quests.m_QUEST[ nSlotNO ];
					}
				}
				#endif

#ifndef __SERVER
				//if ( !isQuestOverflowed )
				{
					g_itMGR.ShowQuestStartMessage( pREWD->m_Rewd000.iQuestSN );
				}
#endif

				break;
			}

		case 2 :	// Change (data retained)
			if ( NULL == pPARAM->m_pQUEST )	// Due to the wrong quest data...
			{

#ifndef	__SERVER
			//--------------------------------------------------------------------------------
			LOGWAR( "[ %s ] F_QSTREWD000[ Quest keeps changing request] FAILED [wrong quest database ] ", pPARAM->m_pCurrentTRIGGER->m_Name.Get() );
			//--------------------------------------------------------------------------------
#endif
				return false;
			}

			pPARAM->m_pQUEST->SetID( pREWD->m_Rewd000.iQuestSN, false );

#ifndef __SERVER
			g_itMGR.ShowQuestStartMessage( pREWD->m_Rewd000.iQuestSN );
#endif

			break;

		case 3 :	// 3 = Change/initialized
			if ( NULL == pPARAM->m_pQUEST )	// Due to the wrong quest data. ...
			{

#ifndef	__SERVER
			//--------------------------------------------------------------------------------
			LOGWAR( "[ %s ] F_QSTREWD000[ Quest change/initialized request] FAILED [wrong quest database ] ", pPARAM->m_pCurrentTRIGGER->m_Name.Get() );
			//--------------------------------------------------------------------------------
#endif
				return false;
			}

			pPARAM->m_pQUEST->Init ();
			pPARAM->m_pQUEST->SetID( pREWD->m_Rewd000.iQuestSN, true );

#ifndef __SERVER
			g_itMGR.ShowQuestStartMessage( pREWD->m_Rewd000.iQuestSN );
#endif

			break;

		case 4 :	// 4 = Select.
		{
			BYTE btQuestSLOT = pPARAM->m_pOWNER->Quest_GetRegistered( pREWD->m_Rewd000.iQuestSN );
			if ( btQuestSLOT >= QUEST_PER_PLAYER )
			{

#ifndef	__SERVER
			//--------------------------------------------------------------------------------
			LOGWAR( "[ %s ] F_QSTREWD000[ Quest select ] FAILED[ btQuestSLOT >= QUEST_PER_PLAYER ] ", pPARAM->m_pCurrentTRIGGER->m_Name.Get() );
			//--------------------------------------------------------------------------------
#endif
				return false;
			}

			pPARAM->m_btQuestSLOT = btQuestSLOT;
			pPARAM->m_pQUEST = &pPARAM->m_pOWNER->m_Quests.m_QUEST[ btQuestSLOT ];

			return true;
		}
	}

	return true;
} 
bool F_QSTREWD001 ( uniQstENTITY *pREWD, tQST_PARAM *pPARAM, bool bDoReward )
{	
	if ( !bDoReward )	{
		// 05.05.21 icarus:: need to write a list of rewards ...
		#pragma message ( "TODO:: A list where you can write quest rewards.." __FILE__ )
		return true;
	}

	/// Quest-only item cycle/snatch (General items, you can use these)
	tagITEM sITEM;

	sITEM.Init( pREWD->m_Rewd001.uiItemSN, pREWD->m_Rewd001.nDupCNT );
	if ( 0 == sITEM.GetHEADER() )
	{
#ifndef	__SERVER
			//--------------------------------------------------------------------------------
			LOGWAR( "[ %s ] F_QSTREWD001[ Quest-only item cycle/snatch ] FAILED[ 0 == sITEM.GetHEADER() ] ", pPARAM->m_pCurrentTRIGGER->m_Name.Get() );
			//--------------------------------------------------------------------------------
#endif
		return false;
	}

	switch( pREWD->m_Rewd001.btOp ) 
	{
		case 0 :	
		{
			if ( sITEM.GetTYPE() == ITEM_TYPE_QUEST ) {
				// Bereft-quest inventory
				if ( NULL == pPARAM->m_pQUEST )
				{
#ifndef	__SERVER
			//--------------------------------------------------------------------------------
			LOGWAR( "[ %s ] F_QSTREWD001[ Quest-only items taken away ] FAILED[ NULL == pPARAM->m_pQUEST ] ", pPARAM->m_pCurrentTRIGGER->m_Name.Get() );
			//--------------------------------------------------------------------------------
#endif
            		return false;
				}

				return pPARAM->m_pQUEST->SubITEM( sITEM );
			}

			// Delete the item from your inventory, the item is a general mounting does not take...
			return pPARAM->m_pOWNER->Quest_SubITEM( sITEM );
		}
		case 1 :	// Cycle
			// btPartyOpt;	/// 0 = The party won do not apply, 1 = The party won applied
			pPARAM->m_pOWNER->Reward_ITEM( sITEM, pREWD->m_Rewd001.btPartyOpt, pPARAM->m_btQuestSLOT );
			return true;
	}

	return false;	
} 
bool F_QSTREWD002 ( uniQstENTITY *pREWD, tQST_PARAM *pPARAM, bool bDoReward )
{	
	if ( !bDoReward )	// 05.05.21 icarus:: Building a list is unnecessary compensation...
		return true;

	/// Quest for the values of the variables/switches value changes
	for (int iL=0; iL<pREWD->m_Rewd002.iDataCnt; iL++) 
	{
		if ( !::Set_QuestVAR( pPARAM, &pREWD->m_Rewd002.CheckData[ iL ] ) ) 
		{

#ifndef	__SERVER
			//--------------------------------------------------------------------------------
			LOGWAR( "[ %s ] F_QSTREWD002[ Quest for the values of the variables/switches value changes ] FAILED[ Set_QuestVAR Failure ]", pPARAM->m_pCurrentTRIGGER->m_Name.Get() );
			//--------------------------------------------------------------------------------
#endif

			pPARAM->m_nErrSTEP = iL;
			return false;
		}
	}

	return true;
} 
bool F_QSTREWD003 ( uniQstENTITY *pREWD, tQST_PARAM *pPARAM, bool bDoReward )
{	
	if ( !bDoReward ) {
		// 05.05.21 icarus:: Need to write a list of rewards...
		#pragma message ( "TODO:: A list where you can write quest rewards.." __FILE__ )
		return true;
	}

	/// Change the character stats
	for (int iL=0; iL<pREWD->m_Rewd003.iDataCnt; iL++) 
	{
		switch( pREWD->m_Rewd003.CheckData[ iL ].btOp ) 
		{
			case 5 :	// Value breaks
				pPARAM->m_pOWNER->Reward_ABILITY( 1, 
						pREWD->m_Rewd003.CheckData[ iL ].iType, 
						pREWD->m_Rewd003.CheckData[ iL ].iValue, 
						pREWD->m_Rewd003.btPartyOpt );
				break;
			case 6 :	// Increase (as given)
				pPARAM->m_pOWNER->Reward_ABILITY( 0, 
						pREWD->m_Rewd003.CheckData[ iL ].iType, 
						pREWD->m_Rewd003.CheckData[ iL ].iValue, 
						pREWD->m_Rewd003.btPartyOpt );
				break;
			case 7 :	// Reduction
				pPARAM->m_pOWNER->Reward_ABILITY( 0, 
						pREWD->m_Rewd003.CheckData[ iL ].iType, 
					   -pREWD->m_Rewd003.CheckData[ iL ].iValue, 
					   pREWD->m_Rewd003.btPartyOpt );
				break;
		}
	}
	return true;
} 

bool F_QSTREWD004 ( uniQstENTITY *pREWD, tQST_PARAM *pPARAM, bool bDoReward )
{	
	if ( !bDoReward )	// 05.05.21 icarus:: Reward list building unnecessary. ...
		return true;

	/// Change the value of a variable character progression
	for (int iL=0; iL<pREWD->m_Rewd004.iDataCnt; iL++) 
	{
		if ( !::Set_QuestVAR( pPARAM, &pREWD->m_Rewd004.CheckData[ iL ] ) ) 
		{

#ifndef	__SERVER
			//--------------------------------------------------------------------------------
			LOGWAR( "[ %s ] F_QSTREWD004[ Change the value of a variable character progression ] FAILED[ Set_QuestVAR Failure ] ", pPARAM->m_pCurrentTRIGGER->m_Name.Get() );
			//--------------------------------------------------------------------------------
#endif
			pPARAM->m_nErrSTEP = iL;
			return false;
		}
	}

	return true;
} 
bool F_QSTREWD005 ( uniQstENTITY *pREWD, tQST_PARAM *pPARAM, bool bDoReward )
{	
/*
/// Compensation
struct STR_REWD_005
{
	unsigned int	uiSize;
	int				iType;

	BYTE			btTarget;	/// 0 = Experience, 1 = Don , 2 = Item (s)
	BYTE			btEquation;	/// The appointment formula in advance
	int				iValue;		/// The base value
	int				iItemSN;	/// The target item
	BYTE			btPartyOpt;	/// 0 = The party won do not apply, 1 = The party won applied
} ;
*/
	if ( !bDoReward ) {
		// 05.05.21 icarus:: Need to write a list of rewards...
		#pragma message ( "TODO:: A list where you can write quest rewards.." __FILE__ )
		return true;
	}
	// Experience rewards
	if ( 0 == pREWD->m_Rewd005.btTarget ) 
	{
		return pPARAM->m_pOWNER->Reward_CalEXP( 
			pREWD->m_Rewd005.btEquation, 
			pREWD->m_Rewd005.iValue,
			pREWD->m_Rewd005.btPartyOpt );
	} else
	// Money rewards
	if ( 1 == pREWD->m_Rewd005.btTarget ) 
	{
		if ( NULL == pPARAM->m_pQUEST )
		{

#ifndef	__SERVER
			//--------------------------------------------------------------------------------
			LOGWAR( "[ %s ] F_QSTREWD005[ Money rewards ] FAILED[ NULL == pPARAM->m_pQUEST ] ", pPARAM->m_pCurrentTRIGGER->m_Name.Get() );
			//--------------------------------------------------------------------------------
#endif
			return false;
		}

		if ( pPARAM->m_pOWNER->Reward_CalMONEY( 
			pREWD->m_Rewd005.btEquation, 
			pREWD->m_Rewd005.iValue,
			pREWD->m_Rewd005.btPartyOpt,
			pPARAM->m_pQUEST->Get_VAR( QUEST_VAR_PER_QUEST-1 ) ) ) {
			pPARAM->m_pQUEST->Set_VAR( QUEST_VAR_PER_QUEST-1, 0 );
			return true;
		}

#ifndef	__SERVER
			//--------------------------------------------------------------------------------
			LOGWAR( "[ %s ] F_QSTREWD005[ Money rewards ] FAILED[ Reward_CalMONEY FAILED ] ", pPARAM->m_pCurrentTRIGGER->m_Name.Get() );
			//--------------------------------------------------------------------------------
#endif

		return false;
	} else
	// Item rewards
	if ( 2 == pREWD->m_Rewd005.btTarget ) 
	{
		return pPARAM->m_pOWNER->Reward_CalITEM( 
			pREWD->m_Rewd005.btEquation, 
			pREWD->m_Rewd005.iValue,
			pREWD->m_Rewd005.btPartyOpt,
			pREWD->m_Rewd005.iItemSN,
			pREWD->m_Rewd005.nItemOpt,
			pPARAM->m_btQuestSLOT );
	}
	
#ifndef	__SERVER
			//--------------------------------------------------------------------------------
			LOGWAR( "[ %s ] F_QSTREWD005[ Item rewards ] FAILED[ Reward_CalITEM FAILED ] ", pPARAM->m_pCurrentTRIGGER->m_Name.Get() );
			//--------------------------------------------------------------------------------
#endif

	return false;
} 

bool F_QSTREWD006 ( uniQstENTITY *pREWD, tQST_PARAM *pPARAM, bool bDoReward )
{
	if ( !bDoReward ) {
		// 05.05.21 icarus:: Need to write a list of rewards...
		#pragma message ( "TODO:: A list where you can write quest rewards.." __FILE__ )
		return true;
	}

#ifndef	__SERVER
	//--------------------------------------------------------------------------------
	LOGOUT( "[ %s ] F_QSTREWD006[ Character Hp, Mp Recovery ]", pPARAM->m_pCurrentTRIGGER->m_Name.Get() );
	//--------------------------------------------------------------------------------
#endif

/// Character Hp, Mp recovery
	int iAdjHP = pPARAM->m_pOWNER->GetCur_MaxHP() * pREWD->m_Rewd006.iPercentOfHP / 100;
	int iAdjMP = pPARAM->m_pOWNER->GetCur_MaxMP() * pREWD->m_Rewd006.iPercentOfMP / 100;

	if ( iAdjHP < 1 ) iAdjHP = 1;
	if ( iAdjMP < 1 ) iAdjMP = 1;

	pPARAM->m_pOWNER->SetCur_HP( iAdjHP );
	pPARAM->m_pOWNER->SetCur_MP( iAdjMP );

	return true;	
} 

bool F_QSTREWD009 ( uniQstENTITY *pREWD, tQST_PARAM *pPARAM, bool bDoReward )
{	
	//if ( !bDoReward )	// 05.05.21 icarus:: Building a list is unnecessary compensation...
	//	return true;
/*
/// *** Go to check a couple of times ==> Change the name of the variable, shNameLen Add
struct STR_REWD_009
{
	unsigned int	uiSize;
	int				iType;

	short			shNameLen;
	char			szNextTriggerSN[ 1 ];	/// INextTriggerSN in the current pattern (O bass) go to the second trigger check
};
*/
	pPARAM->m_HashNextTRIGGER = ::StrToHashKey( pREWD->m_Rewd009.szNextTriggerSN );
	return ( 0 != pPARAM->m_HashNextTRIGGER );
} 
bool F_QSTREWD010 ( uniQstENTITY *pREWD, tQST_PARAM *pPARAM, bool bDoReward )
{
	if ( !bDoReward ) {
		// 05.05.21 icarus:: Need to write a list of rewards...
		#pragma message ( "TODO:: A list where you can write quest rewards.." __FILE__ )
		return true;
	}

	if ( NULL == pPARAM->m_pOWNER )
	{

#ifndef	__SERVER
		//--------------------------------------------------------------------------------
		LOGERR( "[ %s ] F_QSTREWD010[ ] ERROR[ NULL == pPARAM->m_pOWNER ] ", pPARAM->m_pCurrentTRIGGER->m_Name.Get() );
		//--------------------------------------------------------------------------------
#endif
		return false;
	}

	pPARAM->m_pOWNER->Reward_InitSTATUS();

	return true;
}
bool F_QSTREWD011 ( uniQstENTITY *pREWD, tQST_PARAM *pPARAM, bool bDoReward )	
{	
/*
/// NPC Check the value of a variable
typedef struct	tagValue
{
	unsigned int	uiSize;
	int				Type;

	BYTE			btWho;	// 0:NPC, 1:EventOBJ

	short			nVarNo;	// Variable number : 0 ~ , John's economic data variables
	int				iValue;	// Data values to compare (IType of Op iValue)
	BYTE			btOp;	// iValue값에 대한 비교방법. 0 = 같다, 1 = 크다, 2 = 크거나 같다. 3=작다, 4=작거나 같다.
							// (액션쪽) 5 = 값바꿈, 6 = 증가, 7 = 감소
} STR_COND_011, STR_REWD_011;
*/
#ifdef	__SERVER
	if ( pREWD->m_Rewd011.btWho ) {
		// evnet object
		if ( NULL == pPARAM->m_pEventVAR ) {
		#ifndef	__SERVER
			char *szMsg = CStr::Printf ("	[QST] 데이타오류 !!! :: Event 객체 변수 설정시 %s 에서 선택된 Event객체 없음", pPARAM->m_pCurrentTRIGGER->m_Name.Get() );
			g_itMGR.OpenMsgBox( szMsg );
		#endif
			return false;
		}

		#ifdef	__SERVER
		::QST_Set_ObjVAR( pPARAM->m_pEventVAR, (tagValueQST*)pREWD );
		#endif
	} else {
		// npc
		if ( NULL == pPARAM->m_pNpcVAR ) {
		#ifndef	__SERVER
			char *szMsg = CStr::Printf ("	[QST] 데이타오류 !!! :: NPC 변수 설정시 %s 에서 선택된 NPC 없음", pPARAM->m_pCurrentTRIGGER->m_Name.Get() );

			g_itMGR.OpenMsgBox( szMsg );
		#endif
			return false;
		}

		#ifdef	__SERVER
		::QST_Set_ObjVAR( pPARAM->m_pNpcVAR, (tagValueQST*)pREWD );
		#endif
	}
#endif
	return true;
}
bool F_QSTREWD012 ( uniQstENTITY *pREWD, tQST_PARAM *pPARAM, bool bDoReward )
{	
/*
struct STR_REWD_012
{
	unsigned int	uiSize;
	int				iType;

	BYTE		btMsgType;		/// 0 = 현재필드, 1 = 현재존, 2=전체월드
	short		nMsgLength;		/// 대사의 길이 NULL포함
	char		szMsg[ 1 ];		/// 대사
};
*/
#ifdef	__SERVER
	if ( !pPARAM->m_pNpcVAR )
		return false;

	switch( pREWD->m_Rewd012.btMsgType ) {
		case 0 :
			pPARAM->m_pNpcVAR->VSend_gsv_CHAT( pREWD->m_Rewd012.szMsg );
			break;
		case 1 :
			pPARAM->m_pNpcVAR->VSend_gsv_SHOUT( pREWD->m_Rewd012.szMsg );
			break;
		case 2 :
			pPARAM->m_pNpcVAR->VSend_gsv_ANNOUNCE_CHAT( pREWD->m_Rewd012.szMsg );
			break;
	}
#endif
	return true;	
}
bool F_QSTREWD013 ( uniQstENTITY *pREWD, tQST_PARAM *pPARAM, bool bDoReward )	
{
/*
/// 몆초 후에 어떤 트리거 수행
struct STR_REWD_013
{
	unsigned int	uiSize;
	int				iType;

	BYTE			btWho;	// 0:NPC, 1:EventOBJ

	int				iSec; /// 몇초후에.
	short			nNameLength;
	char			szTriggerName[ 1 ]; /// 트리거명 NULL포함
};
*/
#ifdef	__SERVER
	if ( pREWD->m_Rewd013.btWho ) {
		// evnet object
		if ( NULL == pPARAM->m_pEventVAR ) {
		#ifndef	__SERVER
			char *szMsg = CStr::Printf ("	[QST] 데이타오류 !!! :: Event 객체 변수 설정시 %s 에서 선택된 Event객체 없음", pPARAM->m_pCurrentTRIGGER->m_Name.Get() );
			g_itMGR.OpenMsgBox( szMsg );
		#endif
			return false;
		}

		#ifdef	__SERVER
		pPARAM->m_pEventVAR->Set_NextTRIGGER( pREWD->m_Rewd013.iSec, pREWD->m_Rewd013.m_HashNextTrigger );
		#endif
	} else {
		// npc
		if ( NULL == pPARAM->m_pNpcVAR ) {
		#ifndef	__SERVER
			char *szMsg = CStr::Printf ("	[QST] 데이타오류 !!! :: NPC 변수 설정시 %s 에서 선택된 NPC 없음", pPARAM->m_pCurrentTRIGGER->m_Name.Get() );
			g_itMGR.OpenMsgBox( szMsg );
		#endif
			return false;
		}

		#ifdef	__SERVER
		pPARAM->m_pNpcVAR->Set_NextTRIGGER( pREWD->m_Rewd013.iSec, pREWD->m_Rewd013.m_HashNextTrigger );
		#endif
	}
#endif
	return true;
}

bool F_QSTREWD014 ( uniQstENTITY *pREWD, tQST_PARAM *pPARAM, bool bDoReward )
{
	if ( !bDoReward ) {
		// 05.05.21 icarus:: Need to write a list of rewards...
		#pragma message ( "TODO:: A list where you can write quest rewards.." __FILE__ )
		return true;
	}
/// 스킬를 주거나 삭제한다
/*
struct STR_REWD_014
{
	unsigned int	uiSize;
	int				iType;

	BYTE			btOp;		// 0 - 뺏는다 , 1 - 준다
	int				iSkillNo;	// 스킬번호
};
*/
	if ( !pPARAM->m_pOWNER )
		return false;

	if ( pREWD->m_Rewd014.btOp ) {
		// 준다.
		return pPARAM->m_pOWNER->Add_SkillNSend( pREWD->m_Rewd014.iSkillNo );
	}
	
	// 뺏는다.
	return pPARAM->m_pOWNER->Sub_SkillNSend( pREWD->m_Rewd014.iSkillNo );
}


bool F_QSTREWD015 ( uniQstENTITY *pREWD, tQST_PARAM *pPARAM, bool bDoReward )
{
	if ( !bDoReward )	// 05.05.21 icarus:: 보상목록 작성 불필요...
		return true;
/*
/// Switch Off On
struct STR_REWD_015
{
	unsigned int	uiSize;
	int				iType;

	short			nSN;	// 스위치 번호 0 ~ 512
	BYTE			btOp;	// 0 = Off , 1 = On
};
*/
	if ( !pPARAM->m_pOWNER ) {
		_ASSERT( 0 );
		return false;
	}

	pPARAM->m_pOWNER->m_Quests.Set_SWITCH( pREWD->m_Rewd015.nSN, pREWD->m_Rewd015.btOp );
	return true;
}
bool F_QSTREWD016 ( uniQstENTITY *pREWD, tQST_PARAM *pPARAM, bool bDoReward )
{
	if ( !bDoReward )	// 05.05.21 icarus:: Building a list is unnecessary compensation...
		return true;
/*
/// Group Switch Clear
//#define		TYPE_REWD_016
struct STR_REWD_016
{
	unsigned int	uiSize;
	int				iType;

	short			nGroupSN;	// Switch number 0 ~ 512 / 32
};
*/
	if ( !pPARAM->m_pOWNER ) {
		_ASSERT( 0 );
		return false;
	}

	pPARAM->m_pOWNER->m_Quests.m_dwSWITCHES[ pREWD->m_Rewd016.nGroupSN ] = 0;

	return true;
}
bool F_QSTREWD017 ( uniQstENTITY *pREWD, tQST_PARAM *pPARAM, bool bDoReward )
{
	if ( !bDoReward )	// 05.05.21 icarus:: Building a list is unnecessary compensation...
		return true;

/// Full Switch Clear
	if ( !pPARAM->m_pOWNER ) {
		_ASSERT( 0 );
		return false;
	}

	pPARAM->m_pOWNER->m_Quests.ClearAllSwitchs ();
	return true;
}

bool F_QSTREWD018 ( uniQstENTITY *pREWD, tQST_PARAM *pPARAM, bool bDoReward )
{
/*
/// NPC Variable output
struct STR_REWD_018
{
	unsigned int	uiSize;
	int				iType;

	short			nFormatLength;
	short			nCnt;
	BYTE			Data[ 1 ];
	/* Data Inside Null Including, without limitation, the char szFormat[ nFormatLength ] + STR_NPCVAR NpcVars[ 1 ]
		Data Size = nFormatLength + nCnt * sizeof (STR_NPCVAR) */
	/* szFormat[ nFormatLength ]. Contains NULL. Printf format string of the same.
		"xxxx %d xxxx %d xxx "In as much as the% d is nCnt. 
	*/
#ifdef	__SERVER
	CObjVAR *pNpc;

	char *szResult = new char[ pREWD->m_Rewd018.nFormatLength + 6*pREWD->m_Rewd018.nCnt + 1 ];
	char *szParam  = new char[ sizeof(int)*(pREWD->m_Rewd018.nCnt+1) ];
	STR_NPCVAR *pVAR = (STR_NPCVAR*)( &pREWD->m_Rewd018.Data[ pREWD->m_Rewd018.nFormatLength+1 ] );

	va_list va = szParam;
	for (short nI=0; nI<pREWD->m_Rewd018.nCnt; nI++) {
		pNpc = g_pZoneLIST->Get_NpcVAR( pVAR[ nI ].iNpcNo );

		( *( (int*)va ) ) = pNpc->Get_ObjVAR( pVAR[ nI ].nVarNo );
		va_arg(va, int);
	}

	va = szParam;
	vsprintf( szResult, (const char*)pREWD->m_Rewd018.Data, va );
	va_end(va);

	// 존 공지....
	if ( pPARAM->m_nCurZoneNO )
		g_pZoneLIST->Send_gsv_ANNOUNCE_CHAT(  pPARAM->m_nCurZoneNO, szResult, "SERVER" );
	else
		g_pZoneLIST->Send_gsv_ANNOUNCE_CHAT( pNpc, szResult );

	SAFE_DELETE_ARRAY( szResult );
	SAFE_DELETE_ARRAY( szParam  );
#endif
	return true;
}
bool F_QSTREWD019 ( uniQstENTITY *pREWD, tQST_PARAM *pPARAM, bool bDoReward )
{
/// The number of avatars with certain teams trigger execution
/*
struct STR_REWD_019
{
	unsigned int	uiSize;
	int				iType;

	short			nZoneNo;
	short			nTeamNo;
	short			nTriggerLength;
	char			TriggerName[ 1 ]; // char Trigger[ nTriggerLength ], NULL 포함
};
*/
#ifdef	__SERVER
	return g_pZoneLIST->Do_QuestTrigger( pREWD->m_Rewd019.nZoneNo, pREWD->m_Rewd019.nTeamNo, pREWD->m_Rewd019.m_HashTrigger );
#else
	return true;
#endif
}

bool F_QSTREWD020 ( uniQstENTITY *pREWD, tQST_PARAM *pPARAM, bool bDoReward )
{
	/// Team number (PVP 존) - Fires a trigger that minutes after team number
	return true;
}
bool F_QSTREWD021 ( uniQstENTITY *pREWD, tQST_PARAM *pPARAM, bool bDoReward )
{
	if ( !bDoReward )	// 05.05.21 icarus:: Building a list is unnecessary compensation...
		return true;
	/// Specify the location of the resurrection of the current zone (PVP John) 
	if ( !pPARAM->m_pOWNER )
		return false;
	///return pPARAM->m_pOWNER->Set_RevivePOS( pREWD->m_Rewd021.nX * 100, pREWD->m_Rewd021.nY * 100 );

	return true;
}


bool F_QSTREWD023 ( uniQstENTITY *pREWD, tQST_PARAM *pPARAM, bool bDoReward )
{
	if ( !bDoReward ) {	// 05.05.21 icarus:: Need a write a list of rewards...
		#pragma message ( "TODO:: A list where you can write quest rewards.." __FILE__ )
		return true;
	}
/*
// No additional data to a step increase-the clan ratings
struct STR_REWD_023
{
	unsigned int	uiSize;
	int				iType;
} ;
*/
	return true;
}
bool F_QSTREWD024 ( uniQstENTITY *pREWD, tQST_PARAM *pPARAM, bool bDoReward )
{
	if ( !bDoReward ) {
		// 05.05.21 icarus:: Need to write a list of rewards...
		#pragma message ( "TODO:: 퀘스트 보상 목록 작성할곳.." __FILE__ )
		return true;
	}
/*
// Decrease/increase money 
struct STR_REWD_024
{
	unsigned int	uiSize;
	int				iType;

	int				iMONEY; // 
	BYTE			btOP;	// 5 = Value breaks, 6 = Increase (as given), 7 = Reduction (as given)
} ;
*/
	//if ( pPARAM->m_pOWNER && pPARAM->m_pOWNER->Is_ClanMASTER() ) {
	//	switch( pREWD->m_Rewd024.btOP ) {
	//		case 5	:	// Value breaks
	//			pPARAM->m_pOWNER->SetClanMONEY( pREWD->m_Rewd024.iMONEY );
	//			break;
	//		case 6	:	// Increase (as given)
	//			pPARAM->m_pOWNER->AddClanMONEY( pREWD->m_Rewd024.iMONEY );
	//			break;
	//		case 7	:	// Reduction (as given)
	//			pPARAM->m_pOWNER->AddClanMONEY( -1 * pREWD->m_Rewd024.iMONEY );
	//			break;
	//		default :
	//			return false;
	//	}

	//	return true;
	//}
	//return false;
	return true;
}
bool F_QSTREWD025 ( uniQstENTITY *pREWD, tQST_PARAM *pPARAM, bool bDoReward )
{
	if ( !bDoReward ) {
		// 05.05.21 icarus:: Need to write a list of rewards...
		#pragma message ( "TODO:: A list where you can write quest rewards.." __FILE__ )
		return true;
	}
/*
// Decrease/increase your score
struct STR_REWD_025
{
	unsigned int	uiSize;
	int				iType;

	short			nPOINT;
	BYTE			btOP;	// 5 = Value breaks, 6 = Increase (as given), 7 = Reduction (as given)
} ;
*/
	//if ( pPARAM->m_pOWNER && pPARAM->m_pOWNER->GetClanID() ) {
	//	switch( pREWD->m_Rewd025.btOP ) {
	//		case 5	:	// Value breaks
	//			pPARAM->m_pOWNER->SetClanSCORE( pREWD->m_Rewd025.nPOINT );
	//			break;
	//		case 6	:	// Increase (as given)
	//			pPARAM->m_pOWNER->AddClanSCORE( pREWD->m_Rewd025.nPOINT );
	//			break;
	//		case 7	:	// Reduction (as given)
	//			pPARAM->m_pOWNER->AddClanSCORE( -1 * pREWD->m_Rewd025.nPOINT );
	//			break;
	//		default :
	//			return false;
	//	}

	//	return true;
	//}
	//return false;
	return true;
}
bool F_QSTREWD026 ( uniQstENTITY *pREWD, tQST_PARAM *pPARAM, bool bDoReward )
{
	if ( !bDoReward ) {
		// 05.05.21 icarus:: Need to write a list of rewards...
		#pragma message ( "TODO:: A list where you can write quest rewards.." __FILE__ )
		return true;
	}
/*
// Skill acquisition/delete
struct STR_REWD_026
{
	unsigned int	uiSize;
	int				iType;

	short			nSkillNo;	// The skill number
	BYTE			btOP;		// 0: Delete, 1: Acquisition
} ;
*/
	//if ( pPARAM->m_pOWNER && pPARAM->m_pOWNER->GetClanID() ) {
	//	if ( pREWD->m_Rewd026.btOP )
	//		return pPARAM->m_pOWNER->AddClanSKILL( pREWD->m_Rewd026.nSkillNo );
	//	else
	//		return pPARAM->m_pOWNER->DelClanSKILL( pREWD->m_Rewd026.nSkillNo );
	//}

	//return false;
	return true;
}
bool F_QSTREWD027 ( uniQstENTITY *pREWD, tQST_PARAM *pPARAM, bool bDoReward )
{
	if ( !bDoReward ) {
		// 05.05.21 icarus:: Need to write a list of rewards...
		#pragma message ( "TODO:: A list where you can write quest rewards.." __FILE__ )
		return true;
	}
/*
// Decrease/increase the contribution
struct STR_REWD_027
{
	unsigned int	uiSize;
	int				iType;

	short			nCONT; // The contribution
	BYTE			btOP;	// 5 = Value breaks, 6 = Increase (as given), 7 = Reduction (as given)
} ;
*/
	//if ( pPARAM->m_pOWNER && pPARAM->m_pOWNER->GetClanID() ) {
	//	switch( pREWD->m_Rewd027.btOP ) {
	//		case 5	:	// Value breaks
	//			pPARAM->m_pOWNER->SetClanSCORE( pREWD->m_Rewd027.nCONT );
	//			break;
	//		case 6	:	// Increase (as given)
	//			pPARAM->m_pOWNER->AddClanSCORE( pREWD->m_Rewd027.nCONT );
	//			break;
	//		case 7	:	// Reduction (as given)
	//			pPARAM->m_pOWNER->AddClanSCORE( -1 * pREWD->m_Rewd027.nCONT );
	//			break;

	//		default :
	//			return false;
	//	}

	//	return true;
	//}
	//return false;
	return true;
}


//--------------------------------------------------------------------------------------
/// Script execution. (Client only)
//--------------------------------------------------------------------------------------
bool F_QSTREWD029 ( uniQstENTITY *pREWD, tQST_PARAM *pPARAM, bool bDoReward )
{
	if ( !bDoReward )	// 05.05.21 icarus:: Building a list is unnecessary compensation...
		return true;

	if ( !pPARAM->m_pOWNER )
		return false;
	
	//struct STR_REWD_029
	//{
	//	unsigned int	uiSize;
	//	int				iType;

	//	short	nLength; // The length of the script (NULL)
	//	char	szSrciptName[ 1 ]; // A script people
	//} ;

	CSystemProcScript::GetSingleton().CallLuaFunction( pREWD->m_Rewd029.szSrciptName, ZZ_PARAM_INT, g_pAVATAR->Get_INDEX(), ZZ_PARAM_END );

	return true;
}
bool F_QSTREWD030 ( uniQstENTITY *pREWD, tQST_PARAM *pPARAM, bool bDoReward )
{
	if ( !bDoReward ) {	// 05.05.21 icarus:: Need to wrie a list of rewards...
		#pragma message ( "TODO:: A list where you can write quest rewards.." __FILE__ )
		return true;
	}

 	if ( NULL == pPARAM->m_pOWNER )
 	{
 #ifndef	__SERVER
 		//--------------------------------------------------------------------------------
 		LOGERR( "[ %s ] F_QSTREWD030[ ] ERROR[ NULL == pPARAM->m_pOWNER ] ", pPARAM->m_pCurrentTRIGGER->m_Name.Get() );
 		//--------------------------------------------------------------------------------
 #endif
 		return false;
 	}
 	pPARAM->m_pOWNER->Reward_InitSKILL();
 	return true;
}


bool F_QSTREWD031 ( uniQstENTITY *pREWD, tQST_PARAM *pPARAM, bool bDoReward )
{
	if ( !bDoReward ) {
		// 05.05.21 icarus:: Need to write a list of rewards...
		#pragma message ( "TODO:: A list where you can write quest rewards.." __FILE__ )
		return true;
	}
/*	/// Monster hunt variable increases
	struct STR_REWD_031
	{
		unsigned int	uiSize;
		int				iType;

		int				iMonsterSN;		/// Do you summon a monster number
		int				iCompareValue;	// Comparative figures
		STR_QUEST_DATA	Var;			// Variable information
	};
*/
	if ( !::Set_QuestVAR( pPARAM, &pREWD->m_Rewd031.Var ) ) {
		return false;
	}

	//pREWD->m_Rewd031.iMonsterSN;
	//pREWD->m_Rewd031.iCompareValue;

	return true;
}
bool F_QSTREWD032 ( uniQstENTITY *pREWD, tQST_PARAM *pPARAM, bool bDoReward )
{
	if ( !bDoReward ) {
		// 05.05.21 icarus:: Need to write a list of rewards...
		#pragma message ( "TODO:: A list where you can write quest rewards.." __FILE__ )
		return true;
	}
/*	/// Quest item drop information
	struct STR_REWD_032
	{
		unsigned int	uiSize;
		int				iType;

		unsigned int	uiItemSN;
		int				iCompareValue;
		BYTE			btPartyOpt;		// 0 = Party not applicable, applies party
	};
*/
	return true;
}
bool F_QSTREWD033 ( uniQstENTITY *pREWD, tQST_PARAM *pPARAM, bool bDoReward )
{
	if ( !bDoReward ) {	
		// 05.05.21 icarus:: Need to write a list of rewards...
		#pragma message ( "TODO:: A list where you can write quest rewards.." __FILE__ )
		return true;
	}
/*	/// Choice rewards trigger information
	struct STR_REWD_033
	{
		unsigned int	uiSize;
		int				iType;

		short			nNextRewardSplitter;	// -1 Indicates that the trigger's end IM rewarded
	};
*/
	return true;
}


#ifdef __CLAN_WAR_SET
// Add a portion of the structure for the clan fight.
//	Add Azadeh.
bool F_QSTREWD035 ( uniQstENTITY *pREWD, tQST_PARAM *pPARAM, bool bDoReward )
{
#ifdef	__SERVER
	if ( !pPARAM->m_pNpcVAR )
		return false;
#endif

	// Processing logic requires written
	//pPARAM->User information-change options set up the teams. >
	return true;	
}
#endif


//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
struct tagF_QstCOND {
	bool	(*fpCheck)( uniQstENTITY *pCOND, tQST_PARAM *pPARAM );
} g_fpQstCOND[] = {
	F_QSTCOND000,
	F_QSTCOND001,
	F_QSTCOND002,
	F_QSTCOND003,
	F_QSTCOND004,
	F_QSTCOND005,
	F_QSTCOND006,
	F_QSTCOND007,
	F_QSTCOND008,
	F_QSTCOND009,
	F_QSTCOND010,

	F_QSTCOND011,
	F_QSTCOND012,
	F_QSTCOND013,

	F_QSTCOND014,

	F_QSTCOND015,
	F_QSTCOND016,

	F_QSTCOND017,

	F_QSTCOND018,
	F_QSTCOND019,
	F_QSTCOND020,
	F_QSTCOND021,
	F_QSTCOND022,

	F_QSTCOND023,			/// Clan related
	F_QSTCOND024,			/// 클랜관련
	F_QSTCOND025,			/// 클랜관련
	F_QSTCOND026,			/// 클랜관련
	F_QSTCOND027,			/// 클랜관련
	F_QSTCOND028,			/// 클랜관련
	F_QSTCOND029,			/// 클랜관련
	F_QSTCOND030,			/// 클랜관련

	F_QUEST_COND_NULL,
	F_QUEST_COND_NULL,
	F_QUEST_COND_NULL,
	F_QUEST_COND_NULL,
	F_QUEST_COND_NULL,
	F_QUEST_COND_NULL,
	F_QUEST_COND_NULL,
	F_QUEST_COND_NULL,
	F_QUEST_COND_NULL,
	F_QUEST_COND_NULL,
	F_QUEST_COND_NULL,
	F_QUEST_COND_NULL,
	F_QUEST_COND_NULL,
	F_QUEST_COND_NULL,
	F_QUEST_COND_NULL,
	F_QUEST_COND_NULL,
	F_QUEST_COND_NULL,
} ;

struct tagF_QstREWD {
	bool	(*fpReward)( uniQstENTITY *pREWD, tQST_PARAM *pPARAM, bool bDoReward );
} g_fpQstREWD[] = {
	F_QSTREWD000,
	F_QSTREWD001,
	F_QSTREWD002,
	F_QSTREWD003,
	F_QSTREWD004,
	F_QSTREWD005,
	F_QSTREWD006,
	F_QUEST_REWD_NULL,	// Warp..
	F_QUEST_REWD_NULL,	// Mob Summoner
	F_QSTREWD009,
	F_QSTREWD010,

	F_QSTREWD011,
	F_QSTREWD012,
	F_QSTREWD013,
	F_QSTREWD014,

	F_QSTREWD015,
	F_QSTREWD016,
	F_QSTREWD017,

	F_QSTREWD018,
	F_QSTREWD019,
	F_QSTREWD020,
	F_QSTREWD021,

	F_QUEST_REWD_NULL,	// 22	Now John's Regent manipulation.
	F_QSTREWD023,		// 23	Clan ratings increase one step 
	F_QSTREWD024,		// 24	Clan money loss/gain 
	F_QSTREWD025,		// 25	Clancy score decrease/increase
	F_QSTREWD026,		// 26	Skill acquisition/delete
	F_QSTREWD027,		// 27	Decrease/increase the contribution
	F_QUEST_REWD_NULL,		// 28	Clancy won Wharf
	F_QSTREWD029,		// 29
	F_QSTREWD030,

	F_QSTREWD031,	/// Monster hunt variable increases
	F_QSTREWD032,	/// Quest item drop information
	F_QSTREWD033,	/// Choice rewards trigger information
#ifdef __CLAN_WAR_SET
	F_QUEST_REWD_NULL,
	F_QSTREWD035,
#else
	F_QUEST_REWD_NULL,
	F_QUEST_REWD_NULL,
#endif
	F_QUEST_REWD_NULL,
	F_QUEST_REWD_NULL,
	F_QUEST_REWD_NULL,
	F_QUEST_REWD_NULL,
	F_QUEST_REWD_NULL,
	F_QUEST_REWD_NULL,
	F_QUEST_REWD_NULL,
	F_QUEST_REWD_NULL,
	F_QUEST_REWD_NULL,
	F_QUEST_REWD_NULL,
} ;

//-------------------------------------------------------------------------------------------------
bool CQuestDATA::LoadQuestTable ()
{
	if ( this->m_bEnable || this->m_lRefCnt ) {
		return false;
	}

	this->Free ();
	if ( !this->m_STB.Load( m_QuestFILE.Get(),	true, true ) )
		return false;

	CGameSTB cFILE;
	if ( cFILE.Open( m_QuestListSTB.Get() ) ) {
		char *szFileName;
		CStrVAR TmpStr( 512 );

		short nFileCNT = (short)cFILE.GetRowCount ();

		for (short nY=0; nY<nFileCNT; nY++) {
			if ( m_BaseDIR.Get() ) {
				szFileName = TmpStr.Printf("%s%s", m_BaseDIR.Get(), cFILE.GetString ( 0, nY ) );
			} else
				szFileName = cFILE.GetString ( 0, nY );
 
			if ( szFileName )
			{
#ifndef	__SERVER
				this->Client_LoadDATA( szFileName );
#else
				this->LoadDATA( szFileName );
#endif
			}
		}

		cFILE.Close ();
		return true;
	}

	return false;
}

bool CQuestDATA::LoadQuestTable (char *szQuestFile, char *szQuestListSTB, char *szBaseDIR)
{
	this->m_BaseDIR.Set( szBaseDIR );

	if ( !szBaseDIR ) {
		m_QuestFILE.Set( szQuestFile );
		m_QuestListSTB.Set( szQuestListSTB );
	} else {
		m_QuestFILE.Alloc( strlen(szQuestFile) + strlen(szBaseDIR) + 10 );
		m_QuestFILE.Printf("%s%s", szBaseDIR, szQuestFile);

		m_QuestListSTB.Alloc( strlen(szQuestListSTB) + strlen(szBaseDIR) + 10 );
		m_QuestListSTB.Printf("%s%s", szBaseDIR, szQuestListSTB);
	}

	// char *szFileName;

	// szFileName = ( !szBaseDIR ) ? szQuestFile : CStr::Printf("%s%s", szBaseDIR, szQuestFile);
	// this->m_QuestFILE.Set( szFileName );

	// szFileName = ( !szBaseDIR ) ? szQuestListSTB : CStr::Printf("%s%s", szBaseDIR, szQuestListSTB);
	// this->m_QuestListSTB.Set( szFileName );

	if ( this->LoadQuestTable() ) {
		this->m_bEnable = true;
	}

	return this->m_bEnable;
}

#ifdef	__SERVER
void CQuestDATA::LoadQuestTrigger (FILE *fpIN, unsigned int uiTriggerCNT, char *szFileName)
#else
void CQuestDATA::LoadQuestTrigger (CFileSystem* pFileSystem, unsigned int uiTriggerCNT, char *szFileName)
#endif
{
	t_HASHKEY	HashKey;
	CQuestTRIGGER *pTrigger, *pFindTrigger;
	CQuestTRIGGER *pPrevTrigger = NULL;

	for (unsigned int uiT=0; uiT<uiTriggerCNT; uiT++) {
		pTrigger = new CQuestTRIGGER;

	#ifdef	__SERVER
		if ( !pTrigger->Load( fpIN ) ) 
	#else
		if ( !pTrigger->Client_Load( pFileSystem ) ) 
	#endif
		{
			_ASSERT( 0 );
			SAFE_DELETE( pTrigger );
			continue;
		}

		HashKey = ::StrToHashKey( pTrigger->m_Name.Get() );

		tagHASH< CQuestTRIGGER* > *pHashNode = m_HashQUEST.Search( HashKey );
		pFindTrigger = pHashNode ? pHashNode->m_DATA : NULL;
		if ( pFindTrigger ) {
		#if	defined( __INC_WORLD ) || !defined( __SERVER )
			g_LOG.CS_ODS(0xffff, "ERROR:: QUEST TRIGGER: HashKey is equal at \"%s\" skip \"%s\" trigger, [ %s ]\n", pTrigger->m_Name.Get(), pFindTrigger->m_Name.Get(), szFileName );
		#endif
			SAFE_DELETE( pTrigger );
			continue;
		}

#if	defined( __INC_WORLD )
		g_LOG.CS_ODS(0xffff, "QUEST TRIGGER: \"%s\" registered\n", pTrigger->m_Name.Get() );
#endif

		m_HashQUEST.Insert( HashKey, pTrigger );
		if ( pPrevTrigger ) {
			if ( pPrevTrigger->GetCheckNext() )
				pPrevTrigger->m_pNextTrigger = pTrigger;
		}
		pPrevTrigger = pTrigger;
	}
}

#ifdef	__SERVER
bool CQuestDATA::LoadDATA (char *szFileName)
{
	FILE *fpIN;

	fpIN = fopen( szFileName, "rb" );
	if ( NULL == fpIN )
		return false;

	unsigned long ulPatternCNT, ulSize;
	fread( &ulSize,			1,		sizeof(unsigned long),		fpIN);
	fread( &ulPatternCNT,	1,		sizeof(unsigned long),		fpIN);
	short nStrLen;
	fread( &nStrLen,		1,		sizeof(short),				fpIN);
	fseek( fpIN,	nStrLen,	SEEK_CUR);

	unsigned int uiTriggerCNT;
	for (unsigned long ulP=0; ulP<ulPatternCNT; ulP++) {
		fread( &uiTriggerCNT,	1,	sizeof(UINT),	fpIN );
		fread( &nStrLen,		1,	sizeof(short),	fpIN );
		fseek( fpIN,	nStrLen,	SEEK_CUR );

		this->LoadQuestTrigger( fpIN, uiTriggerCNT, szFileName );
	}

	fclose( fpIN );

	return true;
}
#else
bool CQuestDATA::Client_LoadDATA (char *szFileName)
{
	CFileSystem* pFileSystem = (CVFSManager::GetSingleton()).GetFileSystem();
	if( pFileSystem->OpenFile( szFileName ) == false )
	{
		//::MessageBox (NULL, "File open error...", szFileName, MB_OK);
		(CVFSManager::GetSingleton()).ReturnToManager( pFileSystem );
		return false;
	}

	unsigned int ulPatternCNT, ulSize;
	pFileSystem->ReadUInt32( &ulSize );
	pFileSystem->ReadUInt32( &ulPatternCNT );
	short nStrLen;
	pFileSystem->ReadInt16( &nStrLen );
	pFileSystem->Seek( nStrLen,	FILE_POS_CUR );

	unsigned int uiTriggerCNT;
	for (unsigned long ulP=0; ulP<ulPatternCNT; ulP++) 
	{
		pFileSystem->ReadUInt32( &uiTriggerCNT );
		pFileSystem->ReadInt16( &nStrLen );
		pFileSystem->Seek( nStrLen,	FILE_POS_CUR );

		this->LoadQuestTrigger( pFileSystem, uiTriggerCNT, szFileName );
	}

	pFileSystem->CloseFile();
	(CVFSManager::GetSingleton()).ReturnToManager( pFileSystem );

	return true;
}
#endif

void CQuestDATA::Free ()
{
	t_HASHKEY		HashKEY;
	CQuestTRIGGER  *pDATA;

	while( m_HashQUEST.GetFirst( &HashKEY, &pDATA ) ) {
		m_HashQUEST.Delete( HashKEY );
	}
///*
//    tagHASH< CQuestTRIGGER* > *pFindNODE;
//	for (int iL=0; iL<m_HashQUEST.GetTableCount(); iL++) {
//		pFindNODE = m_HashQUEST.GetEntryNode( iL );
//		while( pFindNODE ) {
//			SAFE_DELETE( pFindNODE->m_DATA );
//
//			pFindNODE = pFindNODE->m_NEXT;
//		}
//	}
//*/
	m_STB.Free();
}

eQST_RESULT CQuestDATA::CheckQUEST( CUserDATA *pUSER, t_HASHKEY HashQuest, bool bDoReward )
{
#ifndef	__SERVER
	//--------------------------------------------------------------------------------
	LOGERR( "===트리거 수행===" );
	//--------------------------------------------------------------------------------
#else
	if ( !this->m_bEnable ) {
		// 퀘스트 데이타 점검중...
		return QST_RESULT_STOPPED;
	}
	::InterlockedIncrement( &this->m_lRefCnt );
#endif	

	eQST_RESULT eResult = QST_RESULT_INVALID;
	CQuestTRIGGER *pTrigger;
	tQST_PARAM	   qstPARAM;

	qstPARAM.Init( pUSER, pUSER ? pUSER->Quest_GetZoneNO() : 0 );

	tagHASH< CQuestTRIGGER* > *pHashNode = m_HashQUEST.Search( HashQuest );
	pTrigger = pHashNode ? pHashNode->m_DATA : NULL;

#ifndef	__SERVER
	/// 트리거 없음
	if( pTrigger == NULL )
	{
		//--------------------------------------------------------------------------------
		LOGERR( "트리거 없음 " );
		//--------------------------------------------------------------------------------
		return QST_RESULT_INVALID;
	}
#endif

	while ( pTrigger ) 
	{
#ifndef	__SERVER
		qstPARAM.m_pCurrentTRIGGER = pTrigger;
#endif
		if ( pTrigger->Proc( &qstPARAM, bDoReward ) ) 
		{
			if ( qstPARAM.m_HashNextTRIGGER ) 
			{
				pHashNode = m_HashQUEST.Search( qstPARAM.m_HashNextTRIGGER ); 
				pTrigger = pHashNode ? pHashNode->m_DATA : NULL;
				qstPARAM.Init( pUSER );
				eResult = QST_RESULT_SUCCESS;
				continue;
			}

#ifdef	__SERVER
			::InterlockedDecrement( &this->m_lRefCnt );
#endif

#ifndef	__SERVER
			//----------------------------------------------------------------------------------------------------
			/// 뷰잉된 서버로 부터 받은 보상내용 실행
			//----------------------------------------------------------------------------------------------------
			g_QuestRewardQueue.ApplyReward();
#endif

			return QST_RESULT_SUCCESS;
		}
		
		if ( 0 == pTrigger->GetCheckNext() )
			break;

		pTrigger = pTrigger->m_pNextTrigger;
	}

#ifdef	__SERVER
	::InterlockedDecrement( &this->m_lRefCnt );
#endif

	if ( QST_RESULT_INVALID == eResult && qstPARAM.m_bServerFUNC )
		return QST_RESULT_FAILED;

#ifndef	__SERVER
	//--------------------------------------------------------------------------------
	LOGERR( "===트리거 수행 끝===" );
	//--------------------------------------------------------------------------------
#endif

	return eResult;
}


//-------------------------------------------------------------------------------------------------
void CQuestTRIGGER::Init_COND( uniQstENTITY *pCOND )
{
	// 로딩시 데이타 수정할것들...
	switch( pCOND->iType ) {
		case 12 :	// F_QSTCOND012
		{
			pCOND->m_Cond012.m_HashEventOBJ = ::Make_EventObjectID(
					pCOND->m_Cond012.iZone,
					pCOND->m_Cond012.iX, 
					pCOND->m_Cond012.iY, 
					pCOND->m_Cond012.iEventID );
			break;
		}
	}
}
void CQuestTRIGGER::Init_REWD( uniQstENTITY *pREWD )
{
	// 로딩시 데이타 수정할것들...
	switch( pREWD->iType ) {
		case 13 :	// STR_REWD_013
		{
			t_HASHKEY HashKey = ::StrToHashKey( pREWD->m_Rewd013.szTriggerName );
			pREWD->m_Rewd013.m_HashNextTrigger = HashKey;
			break;
		}
		case 19 :	//
		{
			t_HASHKEY HashKey = ::StrToHashKey( pREWD->m_Rewd019.TriggerName );
			pREWD->m_Rewd019.m_HashTrigger = HashKey;
			break;
		}
	}
}

//-------------------------------------------------------------------------------------------------
#ifndef	__SERVER
bool CQuestTRIGGER::Client_Load ( CFileSystem* pFileSystem )
{
	short nStrLen;

	pFileSystem->ReadByte( &m_btCheckNEXT );
	pFileSystem->ReadUInt32( &m_uiCondCNT );
	pFileSystem->ReadUInt32( &m_uiRewdCNT );

	pFileSystem->ReadInt16( &nStrLen );
	char *pName = this->m_Name.Alloc( nStrLen+1 );
	pFileSystem->Read( pName, nStrLen );

	m_ppCondition = new uniQstENTITY* [ m_uiCondCNT ];
	m_ppReward    = new uniQstENTITY* [ m_uiRewdCNT ];

	StrHeader Header;
	unsigned int uiC;
	for(uiC=0; uiC<m_uiCondCNT; uiC++) 
	{
		// 조건 데이터를 읽어옴
		pFileSystem->ReadUInt32( &Header.uiSize );
		pFileSystem->ReadInt32( &Header.iType );

		m_ppCondition[ uiC ] = (uniQstENTITY*) new BYTE[ Header.uiSize+1 ];
		m_ppCondition[ uiC ]->uiSize = Header.uiSize;
		m_ppCondition[ uiC ]->iType  = Header.iType & 0x0ffff;
		m_ppCondition[ uiC ]->m_pDATA[ Header.uiSize ] = 0;

		pFileSystem->Read( &m_ppCondition[ uiC ]->m_pDATA[ sizeof(StrHeader) ], 							
							Header.uiSize-sizeof(StrHeader) );

		this->Init_COND( m_ppCondition[ uiC ] );
	}

	for(uiC=0; uiC<m_uiRewdCNT; uiC++) 
	{
		// 행동 데이터를 읽어옴
		pFileSystem->ReadUInt32( &Header.uiSize );
		pFileSystem->ReadInt32( &Header.iType );

		m_ppReward[ uiC ] = (uniQstENTITY*) new BYTE[ Header.uiSize+1 ];
		m_ppReward[ uiC ]->uiSize = Header.uiSize;
		m_ppReward[ uiC ]->iType  = Header.iType & 0x0ffff;
		m_ppReward[ uiC ]->m_pDATA[ Header.uiSize ] = 0;

		pFileSystem->Read( &m_ppReward[ uiC ]->m_pDATA[ sizeof(StrHeader) ], 							 
							Header.uiSize-sizeof(StrHeader) );

		this->Init_REWD( m_ppReward[ uiC ] );
	}

	return true;
}
#else
bool CQuestTRIGGER::Load (FILE *fpIN)
{
	short nStrLen;

	fread( &m_btCheckNEXT,	1,			sizeof(BYTE),	fpIN );
	fread( &m_uiCondCNT,	1,			sizeof(UINT),	fpIN );
	fread( &m_uiRewdCNT,	1,			sizeof(UINT),	fpIN );

	fread( &nStrLen,		1,			sizeof(short),	fpIN );
	char *pName = this->m_Name.Alloc( nStrLen+1 );
	fread( pName,			nStrLen,	sizeof(char),	fpIN);

	m_ppCondition = new uniQstENTITY* [ m_uiCondCNT ];
	m_ppReward    = new uniQstENTITY* [ m_uiRewdCNT ];

	StrHeader Header;
	unsigned int uiC;
	for(uiC=0; uiC<m_uiCondCNT; uiC++) {
		// 조건 데이터를 읽어옴
		fread( &Header.uiSize,	1,	sizeof(UINT),	fpIN);
		fread( &Header.iType, 	1,	sizeof(int),	fpIN);

		m_ppCondition[ uiC ] = (uniQstENTITY*) new BYTE[ Header.uiSize+1 ];
		m_ppCondition[ uiC ]->uiSize = Header.uiSize;
		m_ppCondition[ uiC ]->iType  = Header.iType & 0x0ffff;
		m_ppCondition[ uiC ]->m_pDATA[ Header.uiSize ] = 0;

		fread ( &m_ppCondition[ uiC ]->m_pDATA[ sizeof(StrHeader) ], sizeof(BYTE), Header.uiSize-sizeof(StrHeader),	fpIN);

		this->Init_COND( m_ppCondition[ uiC ] );
	}

	for(uiC=0; uiC<m_uiRewdCNT; uiC++) {
		// 행동 데이터를 읽어옴
		fread( &Header.uiSize,	1,	sizeof(UINT),	fpIN);
		fread( &Header.iType, 	1,	sizeof(int),	fpIN);

		m_ppReward[ uiC ] = (uniQstENTITY*) new BYTE[ Header.uiSize+1 ];
		m_ppReward[ uiC ]->uiSize = Header.uiSize;
		m_ppReward[ uiC ]->iType  = Header.iType & 0x0ffff;
		m_ppReward[ uiC ]->m_pDATA[ Header.uiSize ] = 0;

		fread ( &m_ppReward[ uiC ]->m_pDATA[ sizeof(StrHeader) ], sizeof(BYTE), Header.uiSize-sizeof(StrHeader), fpIN);

		this->Init_REWD( m_ppReward[ uiC ] );
	}

	return true;
}
#endif

void CQuestTRIGGER::Free ()
{
	for(unsigned int uiC=0; uiC<m_uiCondCNT; uiC++)
		SAFE_DELETE_ARRAY( m_ppCondition[ uiC ] );

	for(unsigned int uiR=0; uiR<m_uiRewdCNT; uiR++)
		SAFE_DELETE_ARRAY( m_ppReward[ uiR ] );

	SAFE_DELETE_ARRAY( m_ppCondition );
	SAFE_DELETE_ARRAY( m_ppReward   );
}

bool CQuestTRIGGER::Proc( tQST_PARAM *pPARAM, bool bDoReward )
{
	unsigned int uiC;

#ifndef	__SERVER
	//if ( !bDoReward ) 
	{
#endif
		for (uiC=0; uiC<m_uiCondCNT; uiC++) {
			if ( !g_fpQstCOND[ m_ppCondition[uiC]->iType ].fpCheck( m_ppCondition[ uiC ], pPARAM) ) {
				#ifndef	__SERVER
					/// 랜덤 체크하는 부분들은 서버로부터 받은건 다시 체크하지 않는다.
					if( bDoReward && ( m_ppCondition[uiC]->iType == 10 ) )
						continue;

					char *szMsg;
					if ( pPARAM->m_nErrSTEP >= 0 ) {
						szMsg = CStr::Printf ("	[QST] %s에서 %d번째 조건데이타( 조건타입:%d ) 중 %d번째 조건 만족 못함", pPARAM->m_pCurrentTRIGGER->m_Name.Get(), uiC, m_ppCondition[uiC]->iType, pPARAM->m_nErrSTEP );					
					} else {
						szMsg = CStr::Printf ("	[QST] %s에서 %d번째 조건데이타( 조건타입:%d ) 조건 만족 못함", pPARAM->m_pCurrentTRIGGER->m_Name.Get(), uiC, m_ppCondition[uiC]->iType );					
					}

					//--------------------------------------------------------------------------------
					DUMPWAR( szMsg )( "!!!조건만족 못함!!! ", pPARAM->m_pCurrentTRIGGER->m_Name.Get() );
					//--------------------------------------------------------------------------------
				#endif
				// pPARAM->m_pErrENTITY = m_ppCondition[ uiC ];
				return false;
			}
		}
#ifndef	__SERVER
		// 클라이언트에서는 bDoReward = false 상태로 조건만 체크하고
		// 조건이 true일경우 서버에 보상 요청 패킷을 보낸다.
		// 서버에서 조건 체크후 보상 조건이 만족하면 보상후 
		// 보상 하라는 패킷을 보내므로 그때 bDoReward = true로 본 함수를 호출한다.
		//if ( !bDoReward ) 05.05.21 icarus:: 보상없이 체크만 할경우 화면에 출력할 보상 목록을 작성한다...
		//	return true;
	}
#endif

	for (uiC=0; uiC<m_uiRewdCNT; uiC++) {
		if ( !g_fpQstREWD[ m_ppReward[uiC]->iType ].fpReward( m_ppReward[ uiC ], pPARAM, bDoReward ) ) {
			// pPARAM->m_pErrENTITY = m_ppReward[ uiC ];
			#ifndef	__SERVER
				char *szMsg;
				if ( pPARAM->m_nErrSTEP >= 0 ) {
					szMsg = CStr::Printf ("	[QST] %s에서 %d번째 보상데이타( 조건타입:%d ) 중 %d번째 조건 만족 못함", pPARAM->m_pCurrentTRIGGER->m_Name.Get(), uiC, m_ppReward[uiC]->iType, pPARAM->m_nErrSTEP );					
				} else {
					szMsg = CStr::Printf ("	[QST] %s에서 %d번째 보상데이타( 조건타입:%d ) 조건 만족 못함", pPARAM->m_pCurrentTRIGGER->m_Name.Get(), uiC, m_ppReward[uiC]->iType );				
				}
				//--------------------------------------------------------------------------------
				DUMPWAR( szMsg )( "!!!조건만족 못함!!! ", pPARAM->m_pCurrentTRIGGER->m_Name.Get() );
				//--------------------------------------------------------------------------------
			#endif
			return false;
		}
	}

	return true;
}

//-------------------------------------------------------------------------------------------------
