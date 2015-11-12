#include "stdAFX.h"
#include "CAI_FILE.h"
#include "CAI_LIB.h"

//-------------------------------------------------------------------------------------------------
bool F_AICOND_00 (stCondHead *pConDATA, t_AIPARAM *pAIParam)
{
	// Do not use.
	return false;
}
//-------------------------------------------------------------------------------------------------
bool F_AICOND_01 (stCondHead *pConDATA, t_AIPARAM *pAIParam)
{
/*
struct		AICOND01
{
	DWORD			dwSize ;					// The size of this structure
	AITYPE			Type ;						// Conditions, type
	int				iDamage;					// The amount of damage
	BYTE			cRecvOrGive ;				// 0 = receive, 1 = feed
} ;
*/
	AICOND01 *pCond = (AICOND01*)pConDATA;

	if ( pCond->cRecvOrGive )
		return false;

	if ( pAIParam->m_iDamage >= pCond->iDamage )
		return true;

	return false;
}
//-------------------------------------------------------------------------------------------------
bool F_AICOND_02 (stCondHead *pConDATA, t_AIPARAM *pAIParam)
{
#if !defined( __SERVER ) && !defined( __VIRTUAL_SERVER )
	return false;
#endif

	AICOND02 *pCond = (AICOND02*)pConDATA;

	bool bAllied = ( 0 != pCond->btIsAllied );
	// int CharTYPE = ( pCond->cChrType ) ? OBJ_MOB : OBJ_AVATAR;

	//	short nMyTeamNO = pAIParam->m_pSourCHAR->Get_TeamNO();
	CAI_OBJ *pFindCHAR = pAIParam->m_pSourCHAR->AI_FindFirstOBJ( pCond->iDistance );
	int iFindCNT = 0, iNearDist=999999999, iCurDist;

	while( pFindCHAR ) 
	{
		if ( ( pAIParam->m_pSourCHAR!=pFindCHAR ) && (bAllied==pAIParam->m_pSourCHAR->Is_ALLIED( pFindCHAR ) ) ) 
		{
			if ( pAIParam->m_pSourCHAR->Get_LEVEL() - pFindCHAR->Get_LEVEL() >= pCond->nLevelDiff &&
				 pAIParam->m_pSourCHAR->Get_LEVEL() - pFindCHAR->Get_LEVEL() <= pCond->nLevelDiff2 ) 
			{
				iFindCNT ++;

				iCurDist = (int)( pAIParam->m_pSourCHAR->Get_DISTANCE( pFindCHAR ) );
				if ( iCurDist < iNearDist ) 
				{
					iNearDist = iCurDist;
					// Over the nearest...
					pAIParam->m_pNearCHAR = pFindCHAR;
				}

				if ( iFindCNT >= pCond->wChrNum ) 
				{
					// Found over the last
					pAIParam->m_pFindCHAR = pFindCHAR;
					return true;
				}
			}
		}
		pFindCHAR = pAIParam->m_pSourCHAR->AI_FindNextOBJ ();
	} 
	
	return false;
}
//-------------------------------------------------------------------------------------------------
bool F_AICOND_03 (stCondHead *pConDATA, t_AIPARAM *pAIParam)
{
#if !defined( __SERVER ) && !defined( __VIRTUAL_SERVER )
	return false;
#endif
/*
struct		AICOND03
{
	DWORD			dwSize ;					// The size of this structure
	AITYPE			Type ;						// Conditions, type
	int				iDistance ;					// Distance
} ;
*/
	AICOND03 *pCond = (AICOND03*)pConDATA;
	int iDistance = (int)( pAIParam->m_pSourCHAR->Get_MoveDISTANCE () );
	if ( iDistance >= pCond->iDistance )
		return true;

	return false;
}
//-------------------------------------------------------------------------------------------------
bool F_AICOND_04 (stCondHead *pConDATA, t_AIPARAM *pAIParam)
{
/*
struct		AICOND04
{
	DWORD			dwSize ;					// The size of this structure
	AITYPE			Type ;						// Conditions, type
	int				iDistance ;					// Distance
	BYTE			cMoreLess ;					// Less than more than
} ;
*/
	AICOND04 *pCond = (AICOND04*)pConDATA;
	CAI_OBJ *pTarget = pAIParam->m_pSourCHAR->Get_TARGET ();
	if ( NULL == pTarget )
		return false;

	int iDistance = (int)( pAIParam->m_pSourCHAR->Get_DISTANCE( pTarget ) );
	if ( pCond->cMoreLess ) {
		// Less than
		if ( iDistance <= pCond->iDistance )
			return true;
	} else {
		// More than
		if ( iDistance >= pCond->iDistance )
			return true;
	}

	return false;
}
//-------------------------------------------------------------------------------------------------
bool F_AICOND_05 (stCondHead *pConDATA, t_AIPARAM *pAIParam)
{
/*
struct		AICOND05
{
	DWORD			dwSize ;					// The size of this structure
	AITYPE			Type ;						// Conditions, type
	int				cAbType ;					// Base type
	int				iDiff ;
	BYTE			cMoreLess ;					// Less than more than
} ;
	cAbType 
		0 Level
		1 Attack
		2 Defense
		3 Magic Defence
		4 HP
		5 Charm
*/
	AICOND05 *pCond = (AICOND05*)pConDATA;

	// Target...
	CAI_OBJ *pTarget = pAIParam->m_pSourCHAR->Get_TARGET ();
	if ( NULL == pTarget )
		return false;

	int iValue = AI_Get_Ability ( pTarget, pCond->cAbType );
	if ( pCond->cMoreLess ) 
	{
		// Less than
		if ( iValue <= pCond->iDiff )
			return true;
	} else 
	{
		// More than
		if ( iValue >= pCond->iDiff )
			return true;
	}

	return false;
}
//-------------------------------------------------------------------------------------------------
bool F_AICOND_06 (stCondHead *pConDATA, t_AIPARAM *pAIParam)
{
/*
struct		AICOND06
{
	DWORD			dwSize ;					// The size of this structure
	AITYPE			Type ;						// Conditions, type
	DWORD			wHP ;						// HP Figures
	BYTE			cMoreLess ;					// Less than more than
} ;
*/
	AICOND06 *pCond = (AICOND06*)pConDATA;

	if ( pCond->cMoreLess ) {
		// Less than
		if ( pAIParam->m_pSourCHAR->Get_PercentHP() <= (int)pCond->wHP )
			return true;
	} else {
		// More than
		if ( pAIParam->m_pSourCHAR->Get_PercentHP() >= (int)pCond->wHP )
			return true;
	}
	return false;
}
//-------------------------------------------------------------------------------------------------
bool F_AICOND_07 (stCondHead *pConDATA, t_AIPARAM *pAIParam)
{
/*
struct		AICOND07
{
	DWORD			dwSize ;					// The size of this structure
	AITYPE			Type ;						// Conditions, type
	BYTE			cPercent ;					// Probability
} ;
*/
	AICOND07 *pCond = (AICOND07*)pConDATA;

	if ( pAIParam->m_pSourCHAR->Get_RANDOM( 100 ) < pCond->cPercent )
		return true;

	return false;
}

//-------------------------------------------------------------------------------------------------
bool F_AICOND_08 (stCondHead *pConDATA, t_AIPARAM *pAIParam)
{
#if !defined( __SERVER ) && !defined( __VIRTUAL_SERVER )
	return false;
#endif

/*
// find at least 1 aiobj between nLevelDiff and nLevelDiff2 who is btIsAllied within iDistance
// 타겟으로 지정.
struct		AICOND08
{
	DWORD			dwSize ;					// The size of this structure
	AITYPE			Type ;						// Conditions, type
	int				iDistance ;					// Distance
	char			cLevelDiff ;				// Level difference (-128 to 127) than their level at least cLevelDiff
	BYTE			cChrType ;					// Character type (avatar = 0, Monster = 1)
} ;
*/
	AICOND08 *pCond = (AICOND08*)pConDATA;

	bool bAllied = ( 0 != pCond->btIsAllied );

	CAI_OBJ *pFindCHAR = pAIParam->m_pSourCHAR->AI_FindFirstOBJ( pCond->iDistance );
//	short nMyTeamNO = pAIParam->m_pSourCHAR->Get_TeamNO();
	while ( pFindCHAR ) 
	{
		if ( ( pAIParam->m_pSourCHAR!=pFindCHAR ) && (bAllied==pAIParam->m_pSourCHAR->Is_ALLIED( pFindCHAR ) ) ) 
		{
			if ( pAIParam->m_pSourCHAR->Get_LEVEL() - pFindCHAR->Get_LEVEL() >= pCond->nLevelDiff &&
				 pAIParam->m_pSourCHAR->Get_LEVEL() - pFindCHAR->Get_LEVEL() <= pCond->nLevelDiff2 ) 
			{
				pAIParam->m_pFindCHAR = pFindCHAR;
				pAIParam->m_pNearCHAR = pFindCHAR;
				return true;
			}
		}
		pFindCHAR = pAIParam->m_pSourCHAR->AI_FindNextOBJ ();
	} 

	return false;
}

//-------------------------------------------------------------------------------------------------
bool F_AICOND_09 (stCondHead *pConDATA, t_AIPARAM *pAIParam)
{
/*
// Attacked his character and another character is the target of the attack?
struct		AICOND09
{
	DWORD			dwSize ;					// The size of this structure
	AITYPE			Type ;						// Conditions, type
} ;
*/
	AICOND09 *pCond = (AICOND09*)pConDATA;
	if ( pAIParam->m_pSourCHAR->Get_TARGET() != pAIParam->m_pDestCHAR )
		return true;

	return false;
}

//-------------------------------------------------------------------------------------------------
bool F_AICOND_10 (stCondHead *pConDATA, t_AIPARAM *pAIParam)
{
/*
// Attacking their character's abilities to attack targets greater than character / small?
struct		AICOND10
{
	DWORD			dwSize ;					// The size of this structure
	AITYPE			Type ;						// Conditions, type
	BYTE			cAbType ;					// Base type
	BYTE			cMoreLess ;					// Large = 0, 1 = small
} ;
*/
	AICOND10 *pCond = (AICOND10*)pConDATA;

	// The target must be present.
	if ( NULL == pAIParam->m_pSourCHAR->Get_TARGET()  || 
		 NULL == pAIParam->m_pDestCHAR )
		return false;

	int iV1, iV2;
	iV1 = AI_Get_Ability (pAIParam->m_pDestCHAR, pCond->cAbType );
	iV2 = AI_Get_Ability (pAIParam->m_pSourCHAR->Get_TARGET(), pCond->cAbType );
	if ( pCond->cMoreLess ) 
	{
		// Small
		if ( iV1 < iV2 )
			return true;
	} else 
	{
		// Large
		if ( iV1 > iV2 )
			return true;
	}

	return false;
}

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
bool F_AICOND_11 (stCondHead *pConDATA, t_AIPARAM *pAIParam)
{
/*
 * Explain : Their abilities to attack the character of the A and B values ??greater than or / small
struct		AICOND11
{
	DWORD			dwSize ;		// The size of this structure
	AITYPE			Type ;			// Conditions, type = AICOND_11
	BYTE			cAbType ;		// Stats (level = 0, Damage = 1, DEF = 2, anti-hp = 3, HP = 4, attractive = 5)
	int				iValue;			// Comparison value. Greater than this value / jakeunga. In this way
	BYTE			cMoreLess ;		// Large = 0, 1 = small
} ;
*/
	AICOND11 *pCond = (AICOND11*)pConDATA;

	// This should not necessarily be targeted.
	if ( pAIParam->m_pSourCHAR->Get_TARGET() )
		return false;

	if ( NULL == pAIParam->m_pDestCHAR )
		return false;

	int iValue = AI_Get_Ability (pAIParam->m_pDestCHAR, pCond->cAbType );

	if ( pCond->cMoreLess ) 
	{
		// Less than
		if ( iValue <= pCond->iValue )
			return true;
	} else 
	{
		// More than
		if ( iValue >= pCond->iValue )
			return true;
	}

	return false;
}

//-------------------------------------------------------------------------------------------------
bool F_AICOND_12 (stCondHead *pConDATA, t_AIPARAM *pAIParam)	
{	
/*
/// John is now applying the current day / night is?
struct		AICOND12
{
	DWORD			dwSize ;		// The size of this structure
	AITYPE			Type ;			// Conditions, type = AICOND_12
	BYTE			cWhen;			// Check whether 0 = low, 1 = checks to see if the night
};
*/
	int iZoneTIME = pAIParam->m_pSourCHAR->Get_ZoneTIME();

	return ( ((AICOND12*)pConDATA)->cWhen != pAIParam->m_pSourCHAR->Is_DAY() );
}
//-------------------------------------------------------------------------------------------------
bool F_AICOND_13 (stCondHead *pConDATA, t_AIPARAM *pAIParam)	
{	
/*
/// His / target to check the status stake. 
struct		AICOND13
{
	DWORD			dwSize;					// The size of this structure
	AITYPE			Type;					// Conditions, type

	BYTE			btCheckTarget;	// 0 = self, 1 = attack target
	BYTE			btStatusType;	// 0 = harmful, 1 = favorable, 3 = harmful / beneficial Anything
	BYTE			btHave;			// 0 = not? 1 = do?
};
*/
#ifndef	__SERVER
	return false;
#else
	AICOND13 *pCond = (AICOND13*)pConDATA;

	CAI_OBJ *pTarget;
	if ( pCond->btCheckTarget ) {
		// Attack the target ...
		pTarget = pAIParam->m_pSourCHAR->Get_TARGET ();
		if ( NULL == pTarget )
			return false;
	} else {
		// Themselves.
		pTarget = pAIParam->m_pSourCHAR;
	}

	DWORD dwStatus;
	switch( pCond->btStatusType ) {
		case 0 :	// Harmful
			dwStatus = FLAG_ING_BAD  & pTarget->Get_MagicSTATUS();
			break;
		case 1 :	// Favorable
			dwStatus = FLAG_ING_GOOD & pTarget->Get_MagicSTATUS();
			break;
		case 2 :	// Or cancer.
			dwStatus = pTarget->Get_MagicSTATUS();
			break;
	}

	if ( dwStatus ) {
		// The state has ...
		return ( pCond->btHave ) ? true : false;
	}

	// State not
	return ( 0 == pCond->btHave ) ? true : false;
#endif
}
//-------------------------------------------------------------------------------------------------
bool F_AICOND_14 (stCondHead *pConDATA, t_AIPARAM *pAIParam)	
{
/*
// NPC Variable Check	- AICOND14
typedef struct	tagValue
{
	DWORD			dwSize;
	AITYPE			Type;

	short			nVarNo;	// Variable number: 0 -, John of economic data variables
	int				iValue;	// Compare data values ??(iType abilities Op iValue)
	BYTE			btOp;	// how to compare value in iValue. 0 = same, 1 = large, 2 = greater than or equal to. 3 = small = less than or equal to 4.
							// (Side action) 5 = value breaks, increased 6 = 7 = reduced
} AICOND14, AICOND15, AICOND16, AIACT25, AIACT26, AIACT27;
*/
#ifndef	__SERVER
	return true;
#else
	if ( NULL == pAIParam->m_pLocalNPC )
		return false;

	tagValueAI *pVALUE = (tagValueAI*)pConDATA;

	int iValue = pAIParam->m_pLocalNPC->Get_ObjVAR( (BYTE)pVALUE->nVarNo );
	return ::Check_AiOP( pVALUE->btOp, iValue, pVALUE->iValue );
#endif
}
//-------------------------------------------------------------------------------------------------
bool F_AICOND_15 (stCondHead *pConDATA, t_AIPARAM *pAIParam)	
{	
/*
// World Variable Check	- AICOND15
typedef struct	tagValue
{
	DWORD			dwSize;
	AITYPE			Type;

	short			nVarNo;	// Variable number: 0 -, John of economic data variables
	int				iValue;	// Compare data values ??(iType abilities Op iValue)
	BYTE			btOp;	// how to compare value in iValue. 0 = same, 1 = large, 2 = greater than or equal to. 3 = small = less than or equal to 4.
							// (Side action) 5 = value breaks, increased 6 = 7 = reduced
} AICOND14, AICOND15, AICOND16, AIACT25, AIACT26, AIACT27;
*/
#ifndef	__SERVER
	return false;
#else
	tagValueAI *pVALUE = (tagValueAI*)pConDATA;

	int iValue = pAIParam->m_pSourCHAR->Get_WorldVAR( pVALUE->nVarNo );
	return ::Check_AiOP( pVALUE->btOp, iValue, pVALUE->iValue );
#endif
}
//-------------------------------------------------------------------------------------------------
bool F_AICOND_16 (stCondHead *pConDATA, t_AIPARAM *pAIParam)	
{	
/*
// Economic data check - AICOND16
typedef struct	tagValue
{
	DWORD			dwSize;
	AITYPE			Type;

	short			nVarNo;	// Variable number: 0 -, John of economic data variables
	int				iValue;	// Compare data values ??(iType abilities Op iValue)
	BYTE			btOp;	// how to compare value in iValue. 0 = same, 1 = large, 2 = greater than or equal to. 3 = small = less than or equal to 4.
							// (Side action) 5 = value breaks, increased 6 = 7 = reduced
} AICOND14, AICOND15, AICOND16, AIACT25, AIACT26, AIACT27;
*/
#ifndef	__SERVER
	return false;
#else
	tagValueAI *pVALUE = (tagValueAI*)pConDATA;

	int iValue = pAIParam->m_pSourCHAR->Get_EconomyVAR( pVALUE->nVarNo );
	return ::Check_AiOP( pVALUE->btOp, iValue, pVALUE->iValue );
#endif
}
//-------------------------------------------------------------------------------------------------
bool F_AICOND_17 (stCondHead *pConDATA, t_AIPARAM *pAIParam)	
{	
/*
/// NPC Selection
struct AICOND17
{
	DWORD			dwSize;
	AITYPE			Type;

	int				iNpcNo; /// NPC selected number
};
*/
#ifndef	__SERVER
	return false;
#else
	AICOND17 *pCond = (AICOND17*)pConDATA;

	pAIParam->m_pLocalNPC = (CObjNPC*)pAIParam->m_pSourCHAR->Find_LocalNPC( pCond->iNpcNo );

	return ( NULL != pAIParam->m_pLocalNPC );
#endif
}

bool F_AICOND_18 (stCondHead *pConDATA, t_AIPARAM *pAIParam)	
{
#ifndef	__SERVER
	return false;
#else
	CAI_OBJ *pOwner = pAIParam->m_pSourCHAR->Get_OWNER ();
	if ( NULL == pOwner ) 
		return false;

	AICOND18 *pCond = (AICOND18*)pConDATA;
	int iDistance = pAIParam->m_pSourCHAR->Get_DISTANCE( pOwner );

	return ::Check_AiOP( pCond->btOp, iDistance, pCond->iDistance );
#endif
}

bool F_AICOND_19 (stCondHead *pConDATA, t_AIPARAM *pAIParam)	
{
#ifndef	__SERVER
	return false;
#else
	AICOND19 *pCond = (AICOND19*)pConDATA;

	if ( (UINT)pAIParam->m_pSourCHAR->Get_ZoneTIME() >= pCond->ulTime &&
		 (UINT)pAIParam->m_pSourCHAR->Get_ZoneTIME() <= pCond->ulEndTime )
		return true;

	return false;
#endif
}


bool F_AICOND_20 (stCondHead *pConDATA, t_AIPARAM *pAIParam)	
{
	AICOND20 *pCond = (AICOND20*)pConDATA;
/*
// His (monster) stat / etc / host character-related data check
struct AICOND20
{
	DWORD	dwSize;
	AITYPE	Type;

	BYTE	btAbType ;		// Stats (level = 0, Damage = 1, DEF = 2, anti-hp = 3, HP = 4, attractive = 5)

	int		iValue;	// Compare data values ??(iType abilities Op iValue)
	BYTE	btOp;	// iData for value comparison method. 0 = same, 1 = large, 2 = greater than or equal to. 3 = small = less than or equal to 4.			
};
*/
#ifndef	__SERVER
	return false;
#else
	int iValue;

	iValue = AI_Get_Ability (pAIParam->m_pSourCHAR, pCond->btAbType );
	return ::Check_AiOP( pCond->btOp, iValue, pCond->iValue );
#endif
}

bool F_AICOND_21 (stCondHead *pConDATA, t_AIPARAM *pAIParam)	
{
#ifndef	__SERVER
	return false;
#else
	AICOND21 *pCond = (AICOND21*)pConDATA;
	/// Character is present in the host check

	return ( NULL == pAIParam->m_pSourCHAR->Get_OWNER() );
#endif
}

bool F_AICOND_22 (stCondHead *pConDATA, t_AIPARAM *pAIParam)	
{
#ifndef	__SERVER
	return false;
#else
	AICOND22 *pCond = (AICOND22*)pConDATA;
	/// The current owner of the attack target, the character have?

	CAI_OBJ *pOwner = pAIParam->m_pSourCHAR->Get_OWNER ();
	if ( !pOwner )
		return false;
	
	return ( NULL != pOwner->Get_TARGET() );
#endif
}

bool F_AICOND_23 (stCondHead *pConDATA, t_AIPARAM *pAIParam)	
{
	/// World time check
	AICOND23 *pCond = (AICOND23*)pConDATA;

	if ( (UINT)pAIParam->m_pSourCHAR->Get_WorldTIME() >= pCond->ulTime &&
		 (UINT)pAIParam->m_pSourCHAR->Get_WorldTIME() <= pCond->ulEndTime )
		return true;

	return false;
}

bool F_AICOND_24 (stCondHead *pConDATA, t_AIPARAM *pAIParam)	
{
/*
/// Check the date + time
struct AICOND24
{
	DWORD			dwSize;
	AITYPE			Type;

	BYTE			btDate;		// Date. (1 ~ 31)

	BYTE			btHour1;	// O'clock (1 ~ 24)
	BYTE			btMin1;		// Minute (1 ~ 60)
	BYTE			btHour2;	// O'clock
	BYTE			btMin2;		// Minute
	//  And the current date is btDate, btHour1 btMin1 minutes O'clock <= current time <= btHour1 minutes O'clock true btMin1
};
*/
#ifdef	__SERVER
	AICOND24 *pCond = (AICOND24*)pConDATA;
	SYSTEMTIME sTIME;

	::GetLocalTime( &sTIME );
	if ( pCond->btDate && sTIME.wDay != pCond->btDate )
		return false;
	
	WORD wMin = sTIME.wHour * 60 + sTIME.wMinute;
	WORD wFrom, wTo;

	wFrom = pCond->btHour1 * 60 + pCond->btMin1;
	wTo   = pCond->btHour2 * 60 + pCond->btMin2;
	if ( wMin >= wFrom && wMin <= wTo )
		return true;
#endif

	return false;
}

bool F_AICOND_25 (stCondHead *pConDATA, t_AIPARAM *pAIParam)	
{
/*
/// Check the day + time
struct AICOND25
{
	DWORD			dwSize;
	AITYPE			Type;

	BYTE			btWeekDay;	// Day of the week (0 ~ 6)

	BYTE			btHour1;	// O'clock
	BYTE			btMin1;		// 분
	BYTE			btHour2;	
	BYTE			btMin2;		
	//  BtWeekDay the current day of the week and, btHour1 btMin1 minutes O'clock <= current time <= btHour1 minutes O'clock true btMin1
};
*/
#ifdef	__SERVER
	AICOND25 *pCond = (AICOND25*)pConDATA;
	SYSTEMTIME sTIME;

	::GetLocalTime( &sTIME );
	if ( sTIME.wDayOfWeek != pCond->btWeekDay )
		return false;

	WORD wMin = sTIME.wHour * 60 + sTIME.wMinute;
	WORD wFrom, wTo;

	wFrom = pCond->btHour1 * 60 + pCond->btMin1;
	wTo   = pCond->btHour2 * 60 + pCond->btMin2;
	if ( wMin >= wFrom && wMin <= wTo )
		return true;
#endif
	return false;
}

bool F_AICOND_26 (stCondHead *pConDATA, t_AIPARAM *pAIParam)	
{
	/// Check the channel server
	//struct AICOND26
	//{
 //		DWORD			dwSize;
 //		AITYPE			Type;
 //		unsigned short	nX; 
 //		unsigned short	nY; // (nX <= Channel server <= nY) is true
	//};

	return true;
}

bool F_AICOND_NULL (stCondHead *pConDATA, t_AIPARAM *pAIParam)	
{
	return true;
}


//-------------------------------------------------------------------------------------------------
tagConditionFUNC g_FuncCOND[] = {
	{	NULL			},
	{	F_AICOND_00		},	// 0x000000001
	{	F_AICOND_01		},	// 0x000000002
	{	F_AICOND_02		},	// 0x000000003
	{	F_AICOND_03		},	// 0x000000004
	{	F_AICOND_04		},	// 0x000000005
	{	F_AICOND_05		},	// 0x000000006
	{	F_AICOND_06		},	// 0x000000007
	{	F_AICOND_07		},  // 0x000000008
	{	F_AICOND_08		},	// 0x000000009
	{	F_AICOND_09		},	// 0x00000000A
	{	F_AICOND_10		},	// 0x00000000B

	{	F_AICOND_11		},	// 0x00000000c

	{	F_AICOND_12		},	
	{	F_AICOND_13		},	
	{	F_AICOND_14		},	
	{	F_AICOND_15		},	
	{	F_AICOND_16		},	
	{	F_AICOND_17		},

	{	F_AICOND_18		},
	{	F_AICOND_19		},

	{	F_AICOND_20		},
	{	F_AICOND_21		},
	{	F_AICOND_22		},
	{	F_AICOND_23		},

	{	F_AICOND_24		},
	{	F_AICOND_25		},
	{	F_AICOND_26		},

	{	F_AICOND_NULL	},
	{	F_AICOND_NULL	},
	{	F_AICOND_NULL	},
	{	F_AICOND_NULL	},
	{	F_AICOND_NULL	},
	{	F_AICOND_NULL	},
	{	F_AICOND_NULL	},
	{	F_AICOND_NULL	},
	{	F_AICOND_NULL	},
	{	F_AICOND_NULL	},
	{	F_AICOND_NULL	},
	{	F_AICOND_NULL	},
	{	F_AICOND_NULL	},
	{	F_AICOND_NULL	},
	{	F_AICOND_NULL	},
	{	F_AICOND_NULL	},
	{	F_AICOND_NULL	},
	{	F_AICOND_NULL	},
	{	F_AICOND_NULL	},
	{	F_AICOND_NULL	},
	{	F_AICOND_NULL	},
	{	F_AICOND_NULL	},
	{	F_AICOND_NULL	},
	{	F_AICOND_NULL	},
	{	F_AICOND_NULL	},
	{	F_AICOND_NULL	},
	{	F_AICOND_NULL	},
	{	F_AICOND_NULL	},
	{	F_AICOND_NULL	},
	{	F_AICOND_NULL	},
	{	F_AICOND_NULL	},
	{	F_AICOND_NULL	},
	{	F_AICOND_NULL	},
	{	F_AICOND_NULL	},
	{	F_AICOND_NULL	},
	{	F_AICOND_NULL	},
	{	F_AICOND_NULL	},
	{	F_AICOND_NULL	},
	{	F_AICOND_NULL	},
	{	F_AICOND_NULL	},
} ;

//-------------------------------------------------------------------------------------------------
