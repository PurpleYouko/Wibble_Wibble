#include "stdAFX.h"
#include "CAI_FILE.h"
#include "CAI_LIB.h"

#ifdef	__SERVER
#include "ZoneLIST.h"
#endif

//-------------------------------------------------------------------------------------------------
void F_AIACT00 (stActHead *pActDATA, t_AIPARAM *pEVENT)
{
#if !defined( __SERVER ) && !defined( __VIRTUAL_SERVER )
	return;
#endif
/*
	Stop motion..
*/
	AIACT00 *pAct = (AIACT00*)pActDATA;
	pEVENT->m_pSourCHAR->SetCMD_STOP ();
}

//-------------------------------------------------------------------------------------------------
void F_AIACT01 (stActHead *pActDATA, t_AIPARAM *pEVENT)
{
/*
struct		AIACT01
{
	DWORD			dwSize ;					// The size of this structure
	AITYPE			Type ;						// Conditions, type
	BYTE			cAction ;					// Operation Number
} ;

	cAction : Operation emotions.
*/
	AIACT01 *pAct = (AIACT01*)pActDATA;
	pEVENT->m_pSourCHAR->Set_EMOTION( pAct->cAction );
}

//-------------------------------------------------------------------------------------------------
void F_AIACT02 (stActHead *pActDATA, t_AIPARAM *pEVENT)
{
/*
struct		AIACT02
{
	DWORD			dwSize ;					// The size of this structure
	AITYPE			Type ;						// Conditions, type
	char			szCon[1] ;					// Metabolic data
} ;
*/
//	AIACT02 *pAct = (AIACT02*)pActDATA;
	AIACT02_STR *pAct = (AIACT02_STR*)pActDATA;
	pEVENT->m_pSourCHAR->Say_MESSAGE( pAct->szCon );
}
//-------------------------------------------------------------------------------------------------
void F_AIACT03 (stActHead *pActDATA, t_AIPARAM *pEVENT)
{
#if !defined( __SERVER ) && !defined( __VIRTUAL_SERVER )
	return;
#endif

/*
// Go to his current position in any...
struct		AIACT03
{
	DWORD			dwSize ;					// The size of this structure
	AITYPE			Type ;						// Conditions, type
	int				iDistance ;					// Distance
	BYTE			cSpeed ;					// Slow, medium, fast
} ;
*/
	AIACT03 *pAct = (AIACT03*)pActDATA;

	float fPosX, fPosY;
	fPosX = pEVENT->m_pSourCHAR->Get_CurXPOS() + AI_SysRANDOM( pAct->iDistance*2 ) - pAct->iDistance;
	fPosY = pEVENT->m_pSourCHAR->Get_CurYPOS() + AI_SysRANDOM( pAct->iDistance*2 ) - pAct->iDistance;

	pEVENT->m_pSourCHAR->SetCMD_MOVE2D( fPosX, fPosY, pAct->cSpeed );
}
//-------------------------------------------------------------------------------------------------
void F_AIACT04 (stActHead *pActDATA, t_AIPARAM *pEVENT)
{
#if !defined( __SERVER ) && !defined( __VIRTUAL_SERVER )
	return;
#endif

/*
// Go to any location in the emergence of their...
struct		AIACT04
{
	DWORD			dwSize ;					// The size of this structure
	AITYPE			Type ;						// Conditions, type
	int				iDistance ;					// Distance
	BYTE			cSpeed ;					// Slow, medium, fast
} ;
*/
	AIACT04 *pAct = (AIACT04*)pActDATA;

	float fPosX, fPosY;
	fPosX = pEVENT->m_pSourCHAR->Get_BornXPOS() + AI_SysRANDOM( pAct->iDistance*2 ) - pAct->iDistance;
	fPosY = pEVENT->m_pSourCHAR->Get_BornYPOS() + AI_SysRANDOM( pAct->iDistance*2 ) - pAct->iDistance;

	pEVENT->m_pSourCHAR->SetCMD_MOVE2D( fPosX, fPosY, pAct->cSpeed );
}

//-------------------------------------------------------------------------------------------------
void F_AIACT05 (stActHead *pActDATA, t_AIPARAM *pEVENT)
{
#if !defined( __SERVER ) && !defined( __VIRTUAL_SERVER )
	return;
#endif

/*
struct		AIACT05
{
	DWORD			dwSize ;					// The size of this structure
	AITYPE			Type ;						// Conditions, type
	BYTE			cSpeed ;					// Slow, medium, fast
	//BYTE			cAttackOrMove ;				// Attack, move
} ;
*/
	if ( NULL == pEVENT->m_pFindCHAR )
		return;

	AIACT05 *pAct = (AIACT05*)pActDATA;

	float fPosX, fPosY;

	// 2003. 1. 2 Random positions around 2m..
	fPosX = pEVENT->m_pFindCHAR->Get_CurXPOS() + AI_SysRANDOM( 400 ) - 200;
	fPosY = pEVENT->m_pFindCHAR->Get_CurYPOS() + AI_SysRANDOM( 400 ) - 200;

	pEVENT->m_pSourCHAR->SetCMD_MOVE2D( fPosX, fPosY, pAct->cSpeed );
}
//-------------------------------------------------------------------------------------------------
void F_AIACT06 (stActHead *pActDATA, t_AIPARAM *pEVENT)
{
#if !defined( __SERVER ) && !defined( __VIRTUAL_SERVER )
	return;
#endif

/*
// Distance x in the avatar of the chapter that is the most offensive stats.
struct		AIACT06
{
	DWORD			dwSize ;					// The size of this structure
	AITYPE			Type ;						// Conditions, type
	int				iDistance ;					// Distance
	BYTE			cAbType ;					// Base type
	BYTE			cMoreLess ;					// Big, jakeunga
} ;
*/
	AIACT06 *pAct = (AIACT06*)pActDATA;

	int iMinValue=9999999, iMaxValue=-9999999, iValue;

	CAI_OBJ *pMinCHAR, *pMaxCHAR;
	CAI_OBJ *pFindCHAR = pEVENT->m_pSourCHAR->AI_FindFirstOBJ( pAct->iDistance );
	int iFindCNT = 0;
//	short nMyTeamNO = pEVENT->m_pSourCHAR->Get_TeamNO();

	while( pFindCHAR ) 
	{
		if ( ( pEVENT->m_pSourCHAR!=pFindCHAR ) && !pEVENT->m_pSourCHAR->Is_ALLIED( pFindCHAR ) ) 
		{
			iValue = AI_Get_Ability( pFindCHAR, pAct->cAbType );
			if ( iValue < iMinValue ) 
			{
				pMinCHAR = pFindCHAR;
				iMinValue = iValue;
			}
			if ( iValue > iMaxValue ) 
			{
				pMaxCHAR = pFindCHAR;
				iMaxValue = iValue;
			}
		}
		pFindCHAR = pEVENT->m_pSourCHAR->AI_FindNextOBJ ();
	} 

	if ( pAct->cMoreLess ) 
	{
		// Small
		if ( pMinCHAR )
			pEVENT->m_pSourCHAR->SetCMD_RUNnATTACK( pMinCHAR->Get_TAG() );
	} else 
	{
		// Large
		if ( pMaxCHAR )
			pEVENT->m_pSourCHAR->SetCMD_RUNnATTACK( pMaxCHAR->Get_TAG() );
	}
}
//-------------------------------------------------------------------------------------------------
void F_AIACT07 (stActHead *pActDATA, t_AIPARAM *pEVENT)
{
#if !defined( __SERVER ) && !defined( __VIRTUAL_SERVER )
	return;
#endif

/*
struct		AIACT07
{
	DWORD			dwSize ;					// The size of this structure
	AITYPE			Type ;						// Conditions, type
												// No data
} ;
*/
	pEVENT->m_pSourCHAR->Special_ATTACK();
}
//-------------------------------------------------------------------------------------------------
void F_AIACT08 (stActHead *pActDATA, t_AIPARAM *pEVENT)
{
#if !defined( __SERVER ) && !defined( __VIRTUAL_SERVER )
	return;
#endif

/*
struct		AIACT08
{
	DWORD			dwSize ;					// The size of this structure
	AITYPE			Type ;						// Conditions, type
	int				iDistance ;					// distance
	BYTE			cSpeed ;					// Slow, medium, fast
} ;
*/
	AIACT08 *pAct = (AIACT08*)pActDATA;

	CAI_OBJ *pTarget = pEVENT->m_pSourCHAR->Get_TARGET ();
	if ( NULL == pTarget )
		return;

	float fPosX, fPosY;
	float fDistance, fX, fY;

	fX = pTarget->Get_CurXPOS() - pEVENT->m_pSourCHAR->Get_CurXPOS();
	fY = pTarget->Get_CurYPOS() - pEVENT->m_pSourCHAR->Get_CurYPOS();
	fDistance = pEVENT->m_pSourCHAR->Get_DISTANCE( pTarget );

	fPosX = pEVENT->m_pSourCHAR->Get_CurXPOS() - ( pAct->iDistance * fX / fDistance );
	fPosY = pEVENT->m_pSourCHAR->Get_CurYPOS() - ( pAct->iDistance * fY / fDistance );

	pEVENT->m_pSourCHAR->SetCMD_MOVE2D( fPosX, fPosY, pAct->cSpeed );
}
//-------------------------------------------------------------------------------------------------
void F_AIACT09 (stActHead *pActDATA, t_AIPARAM *pEVENT)
{
#if !defined( __SERVER ) && !defined( __VIRTUAL_SERVER )
	return;
#endif
/*
typedef struct		tagAIACT09
{
	DWORD			dwSize ;					// The size of this structure
	AITYPE			Type ;						// Conditions, type
	WORD			wMonster ;					// Monster Numbers
} AIACT09 , AIACT10 ;
*/

	AIACT09 *pAct = (AIACT09*)pActDATA;
	pEVENT->m_pSourCHAR->Change_CHAR( pAct->wMonster );
}
//-------------------------------------------------------------------------------------------------
void F_AIACT10 (stActHead *pActDATA, t_AIPARAM *pEVENT)
{
#if !defined( __SERVER ) && !defined( __VIRTUAL_SERVER )
	return;
#endif

/*
typedef struct		tagAIACT09
{
	DWORD			dwSize ;					// The size of this structure
	AITYPE			Type ;						// Conditions, type
	WORD			wMonster ;					// Monster Numbers
} AIACT09 , AIACT10 ;
*/
	AIACT10 *pAct = (AIACT10*)pActDATA;
	pEVENT->m_pSourCHAR->Create_PET ( 
			pAct->wMonster, 
			pEVENT->m_pSourCHAR->Get_CurXPOS(),
			pEVENT->m_pSourCHAR->Get_CurYPOS(), 150 );
}
//-------------------------------------------------------------------------------------------------
void F_AIACT11 (stActHead *pActDATA, t_AIPARAM *pEVENT)
{
#if !defined( __SERVER ) && !defined( __VIRTUAL_SERVER )
	return;
#endif

/*
// idistance Your colleagues in combat numofmons 수만큼 자신의
// Ordered pincer attack targets.
struct		AIACT11
{
	DWORD			dwSize ;					// The size of this structure
	AITYPE			Type ;						// Conditions, type
	int				iDistance ;					// distance
	int				iNumOfMonster;
} ;
*/
	AIACT11 *pAct = (AIACT11*)pActDATA;
	CAI_OBJ *pTarget = pEVENT->m_pSourCHAR->Get_TARGET ();
	if ( NULL == pTarget )
		return;

//	short nMyTeamNO = pEVENT->m_pSourCHAR->Get_TeamNO();

	CAI_OBJ *pFindCHAR = pEVENT->m_pSourCHAR->AI_FindFirstOBJ( pAct->iDistance );
	int iFindCNT = 0;
	while( pFindCHAR ) 
	{
		if ( ( pEVENT->m_pSourCHAR!=pFindCHAR )		&&
			 pEVENT->m_pSourCHAR->Is_SameTEAM( pFindCHAR )	&&
			 pFindCHAR->Get_TARGET() == NULL ) 
		{

			pFindCHAR->SetCMD_RUNnATTACK( pTarget->Get_TAG() );

			if ( ++iFindCNT >= pAct->iNumOfMonster )
				return;
		}
		pFindCHAR = pEVENT->m_pSourCHAR->AI_FindNextOBJ ();
	} 
}
//-------------------------------------------------------------------------------------------------
void F_AIACT12 (stActHead *pActDATA, t_AIPARAM *pEVENT)
{
#if !defined( __SERVER ) && !defined( __VIRTUAL_SERVER )
	return;
#endif

/*
// Close to the target of the attack mention characters...
struct		AIACT12
{
		DWORD			dwSize ;		// The size of this structure
		AITYPE			Type ;			// Conditions, type
} ;
*/
	pEVENT->m_pSourCHAR->SetCMD_RUNnATTACK( pEVENT->m_pNearCHAR->Get_TAG() );
}
//-------------------------------------------------------------------------------------------------
void F_AIACT13 (stActHead *pActDATA, t_AIPARAM *pEVENT)
{
#if !defined( __SERVER ) && !defined( __VIRTUAL_SERVER )
	return;
#endif

/*
// Check the condition of the character to hit the target ...
struct		AIACT13
{
		DWORD			dwSize ;		// The size of this structure
		AITYPE			Type ;			// Conditions, type
} ;
*/
	pEVENT->m_pSourCHAR->SetCMD_RUNnATTACK( pEVENT->m_pFindCHAR->Get_TAG() );
}
//-------------------------------------------------------------------------------------------------
void F_AIACT14 (stActHead *pActDATA, t_AIPARAM *pEVENT)
{
#if !defined( __SERVER ) && !defined( __VIRTUAL_SERVER )
	return;
#endif

/*
// Neighborhood distance Their colleagues in the same kind of attack targets
// Ordered the attack.
struct		AIACT14
{
	DWORD			dwSize ;					// The size of this structure
	AITYPE			Type ;						// Conditions, type
	int				iDistance ;					// Distance
} ;
*/
	CAI_OBJ *pTarget   = pEVENT->m_pSourCHAR->Get_TARGET ();
	if ( NULL == pTarget )
		return;

	AIACT14 *pAct = (AIACT14*)pActDATA;

//	short nMyTeamNO = pEVENT->m_pSourCHAR->Get_TeamNO ();
	short CharNO    = pEVENT->m_pSourCHAR->Get_CharNO ();

	CAI_OBJ *pFindCHAR = pEVENT->m_pSourCHAR->AI_FindFirstOBJ( pAct->iDistance );
	while( pFindCHAR ) 
	{
		if ( ( pEVENT->m_pSourCHAR!=pFindCHAR )		&&
			 pFindCHAR->Get_CharNO() == CharNO		&&
			 pEVENT->m_pSourCHAR->Is_SameTEAM( pFindCHAR )	&&
			 pFindCHAR->Get_TARGET() == NULL ) 
		{
			pFindCHAR->SetCMD_RUNnATTACK( pTarget->Get_TAG() );
		}
		pFindCHAR = pEVENT->m_pSourCHAR->AI_FindNextOBJ ();
	} 
}
//-------------------------------------------------------------------------------------------------
void F_AIACT15 (stActHead *pActDATA, t_AIPARAM *pEVENT)
{
#if !defined( __SERVER ) && !defined( __VIRTUAL_SERVER )
	return;
#endif

/*
// Switch targets to the character that hit me.
struct		AIACT15
{
	DWORD			dwSize ;					// The size of this structure
	AITYPE			Type ;						// Conditions, type
} ;
*/
	pEVENT->m_pSourCHAR->SetCMD_RUNnATTACK( pEVENT->m_pDestCHAR->Get_TAG() );
}

//-------------------------------------------------------------------------------------------------
void F_AIACT16 (stActHead *pActDATA, t_AIPARAM *pEVENT)
{
#if !defined( __SERVER ) && !defined( __VIRTUAL_SERVER )
	return;
#endif

/*
struct AIACT16
{
	DWORD	dwSize;		/// The size of this structure
	AITYPE	Type;		/// Type = AIACT_16 . Escape
	int		iDistance ;	/// Within a few meters away and the spawn point
};
*/
	AIACT16 *pAct = (AIACT16*)pActDATA;

	pEVENT->m_pSourCHAR->Run_AWAY ( pAct->iDistance );
}

//-------------------------------------------------------------------------------------------------
void F_AIACT17 (stActHead *pActDATA, t_AIPARAM *pEVENT)
{
#if !defined( __SERVER ) && !defined( __VIRTUAL_SERVER )
	return;
#endif

/*
struct AIACT17
{
	DWORD	dwSize;		/// The size of this structure
	AITYPE	Type;		/// Type = AIACT_16 . Escape
	short	item0;		/// Item Number 0  . -1 If there is no item number puts
	short	item1;		/// Item Number 1
	short	item2;		/// Item Number 2
	short	item3;		/// Item Number 3
	short	item4;		/// Item Number 4 
};
*/
	AIACT17 *pAct = (AIACT17*)pActDATA;

	short nDropITEM = pAct->m_ITEMS[ pEVENT->m_pSourCHAR->Get_RANDOM(5) ];
	if ( nDropITEM > 0 )
		pEVENT->m_pSourCHAR->Drop_ITEM( nDropITEM );
}

//-------------------------------------------------------------------------------------------------
void F_AIACT18 (stActHead *pActDATA, t_AIPARAM *pEVENT)
{
#if !defined( __SERVER ) && !defined( __VIRTUAL_SERVER )
	return;
#endif

/*
 * Struct Name : AIACT18 - <<2004.1.29 More >>
 * Specified number of non-combatant commands are invoked to attack mobs. (Monster numbers, distance, number of animals) 
 * Additional Explanation: The specified distance in a specified number of a specified number of dogs attacking a monster call instruction
struct AIACT18
{
	DWORD		dwSize;			/// The size of this structure
	AITYPE		Type;			/// Type Value AIACT_18

	WORD		cMonster ;		// Monster Numbers
	WORD		wHowMany ;		// Some of them
	int			iDistance ;		// Distance
};
*/
	CAI_OBJ *pTarget   = pEVENT->m_pSourCHAR->Get_TARGET ();
	if ( NULL == pTarget )
		return;

	AIACT18 *pAct = (AIACT18*)pActDATA;

	CAI_OBJ *pFindCHAR = pEVENT->m_pSourCHAR->AI_FindFirstOBJ( pAct->iDistance );

//	short nMyTeamNO = pEVENT->m_pSourCHAR->Get_TeamNO();

	WORD wHowMany = 0;
	while( pFindCHAR ) {
		if ( ( pEVENT->m_pSourCHAR!=pFindCHAR )				&&
			 pFindCHAR->Get_CharNO() == pAct->cMonster		&&
			 pEVENT->m_pSourCHAR->Is_SameTEAM( pFindCHAR )	&&
			 pFindCHAR->Get_TARGET() == NULL ) {

			pFindCHAR->SetCMD_RUNnATTACK( pTarget->Get_TAG() );

			if ( ++wHowMany >= pAct->wHowMany )
				break;
		}
		pFindCHAR = pEVENT->m_pSourCHAR->AI_FindNextOBJ ();
	} 
}

//-------------------------------------------------------------------------------------------------
void F_AIACT19 (stActHead *pActDATA, t_AIPARAM *pEVENT)
{
#if !defined( __SERVER ) && !defined( __VIRTUAL_SERVER )
	return;
#endif
/*
 * Struct Name : AIACT19 - <<2004.1.29 More>>
 * Avatar to attack the nearest target designation
struct AIACT19
{
	DWORD		dwSize;				/// The size of this structure
	AITYPE		Type;				/// Type Value AIACT_19

	/// Requires additional conditions
};
*/
	if ( pEVENT->m_pNearCHAR ) {
		pEVENT->m_pSourCHAR->SetCMD_RUNnATTACK( pEVENT->m_pNearCHAR->Get_TAG() );
	}
}

//-------------------------------------------------------------------------------------------------
void F_AIACT20 (stActHead *pActDATA, t_AIPARAM *pEVENT)
{
#if !defined( __SERVER ) && !defined( __VIRTUAL_SERVER )
	return;
#endif
/*
 * Struct Name : AIACT20 - <<2004.1.29 more>>
 * Specified in the summons monsters. His / attacker (the attack on his character) / attack a target (he is attacking the character).
 * Nearby? Meters summons within
 * Additional comments: one is the number of animals.
struct AIACT20
{
	DWORD		dwSize;				/// The size of this structure
	AITYPE		Type;				/// Type Value AIACT_20

	WORD		cMonster;			/// Monster Numbers
	BYTE		btPos;				/// Summon position: 0 = his surroundings, around 1 = attacker (he attacked), 2 = attacked targets around (he is attacking)
	int			iDistance ;			// Distance. Summons within this distance
};
*/

	AIACT20 *pAct = (AIACT20*)pActDATA;
	switch( pAct->btPos ) {
		case 0 :	// Around their...
		{
			pEVENT->m_pSourCHAR->Create_PET( pAct->cMonster, 
							pEVENT->m_pSourCHAR->Get_CurXPOS(),
							pEVENT->m_pSourCHAR->Get_CurYPOS(),
							pAct->iDistance );
			break;
		}

		case 1 :	// Attacked their...
		{
			if ( pEVENT->m_pDestCHAR ) {
				pEVENT->m_pSourCHAR->Create_PET( pAct->cMonster, 
								pEVENT->m_pDestCHAR->Get_CurXPOS(),
								pEVENT->m_pDestCHAR->Get_CurYPOS(),
								pAct->iDistance );
			}
			break;
		}
			
		case 2 :	// Attacking their...
		{
			CAI_OBJ *pTarget = pEVENT->m_pSourCHAR->Get_TARGET ();
			if ( pTarget ) {
				pEVENT->m_pSourCHAR->Create_PET( pAct->cMonster, 
								pTarget->Get_CurXPOS(),
								pTarget->Get_CurYPOS(),
								pAct->iDistance );
			}
			break;
		}
	} ;
}

void F_AIACT21 (stActHead *pActDATA, t_AIPARAM *pEVENT)		
{	
	// Off.
}
void F_AIACT22 (stActHead *pActDATA, t_AIPARAM *pEVENT)		
{	
	// off.
}
void F_AIACT23 (stActHead *pActDATA, t_AIPARAM *pEVENT)		
{
/*
// The suicide
struct AIACT23
{
	DWORD	dwSize;
	AITYPE	Type;
};
*/
#ifdef	__SERVER
	pEVENT->m_pSourCHAR->Add_DAMAGE( pEVENT->m_pSourCHAR->Get_HP()+1 );
#endif
}
void F_AIACT24 (stActHead *pActDATA, t_AIPARAM *pEVENT)		
{
/*
/// Using skills and behavior toward the target
struct AIACT24
{
	DWORD		dwSize;			/// The size of this structure
	AITYPE		Type;			/// Type

	BYTE		btTarget;		// Check condition of the character 0 =, 1 = current attack target, 2 = oneself (self skill)
	short		nSkill;			// Skill number
	short		nMotion;		// Number -1 = no action motion
};
*/
#ifdef	__SERVER
	AIACT24 *pAct = (AIACT24*)pActDATA;
	switch( pAct->btTarget )
	{
		case 0 :
			pEVENT->m_pSourCHAR->SetCMD_Skill2OBJ( pEVENT->m_pFindCHAR->Get_TAG(), pAct->nSkill, pAct->nMotion );
			break;
		case 1 :
		{
			CAI_OBJ *pTarget   = pEVENT->m_pSourCHAR->Get_TARGET ();
			if ( NULL == pTarget )
				return;

			pEVENT->m_pSourCHAR->SetCMD_Skill2OBJ( pTarget->Get_TAG(), pAct->nSkill, pAct->nMotion );
			break;
		}
		case 2 :
			pEVENT->m_pSourCHAR->SetCMD_Skill2SELF( pAct->nSkill, pAct->nMotion );
			break;
	}
#endif
}
void F_AIACT25 (stActHead *pActDATA, t_AIPARAM *pEVENT)		
{
/*
// NPC Variable Check	- AICOND14
typedef struct	tagValue
{
	DWORD			dwSize;
	AITYPE			Type;

	short			nVarNo;	//Variable number: 0 -, John of economic data variables
	int				iValue;	// Compare data values ??(iType abilities Op iValue)
	BYTE			btOp;	// how to compare value in iValue. 0 = same, 1 = large, 2 = greater than or equal to. 3 = small = less than or equal to 4.
							// (Side action) 5 = value breaks, increased 6 = 7 = reduced
} AICOND14, AICOND15, AICOND16, AIACT25, AIACT26, AIACT27;
*/
#ifdef	__SERVER
	if ( !pEVENT->m_pLocalNPC )
		return;

	AIACT25 *pAct = (AIACT25*)pActDATA;

	int iValue  = pEVENT->m_pLocalNPC->Get_ObjVAR( pAct->nVarNo );
	int iResult = ::Result_AiOP( pAct->btOp, iValue, pAct->iValue );

	pEVENT->m_pLocalNPC->Set_ObjVAR( pAct->nVarNo, iResult );
#endif
}
void F_AIACT26 (stActHead *pActDATA, t_AIPARAM *pEVENT)		
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
#ifdef	__SERVER
	AIACT26 *pAct = (AIACT26*)pActDATA;

	int iValue  = pEVENT->m_pSourCHAR->Get_WorldVAR( pAct->nVarNo );
	int iResult = ::Result_AiOP( pAct->btOp, iValue, pAct->iValue );

	pEVENT->m_pSourCHAR->Set_WorldVAR( pAct->nVarNo, iResult );
#endif
}
void F_AIACT27 (stActHead *pActDATA, t_AIPARAM *pEVENT)		
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
#ifdef	__SERVER
	AIACT27 *pAct = (AIACT27*)pActDATA;

	int iValue  = pEVENT->m_pSourCHAR->Get_EconomyVAR( pAct->nVarNo );
	int iResult = ::Result_AiOP( pAct->btOp, iValue, pAct->iValue );
	
	pEVENT->m_pSourCHAR->Set_EconomyVAR( pAct->nVarNo, iResult );
#endif
}
void F_AIACT28 (stActHead *pActDATA, t_AIPARAM *pEVENT)		
{	
/*
struct AIACT28
{
	DWORD		dwSize;			/// The size of this structure
	AITYPE		Type;			/// Type

	BYTE		btMsgType;		/// 0 = the current field, 1 = current zone, 2 = Full World
	short		nMsgLength;		/// Body, including the length of the NULL
	char		szMsg[ 1 ];		/// Body
};
*/
//	AIACT28 *pAct = (AIACT28*)pActDATA;
	AIACT28_STR *pAct = (AIACT28_STR*)pActDATA;
	
#ifndef	__SERVER
	pEVENT->m_pSourCHAR->Say_MESSAGE( pAct->szMsg );
#else
	char *szMSG = pAct->szMsg;
	switch( pAct->btMsgType )
	{
		case 0 :
			pEVENT->m_pSourCHAR->Send_gsv_CHAT( szMSG  );
			break;
		case 1 :
			pEVENT->m_pSourCHAR->Send_gsv_SHOUT( szMSG );
			break;
		case 2 :
			pEVENT->m_pSourCHAR->Send_gsv_ANNOUNCE_CHAT( szMSG );
			break;
	}
#endif
}

void F_AIACT29 (stActHead *pActDATA, t_AIPARAM *pEVENT)		
{	
#ifdef	__SERVER
	CAI_OBJ *pOwner = pEVENT->m_pSourCHAR->Get_OWNER ();
	if ( !pOwner )
		return;

	AIACT29 *pAct = (AIACT29*)pActDATA;

	float fPosX, fPosY;
	float fDistance;

	fDistance = pEVENT->m_pSourCHAR->Get_DISTANCE( pOwner );

	fPosX = pOwner->Get_CurXPOS() - ( 0.2f * fDistance );
	fPosY = pOwner->Get_CurYPOS() - ( 0.2f * fDistance );

	pEVENT->m_pSourCHAR->SetCMD_MOVE2D( fPosX, fPosY, 1 /* run mode */ );
#endif
}

void F_AIACT30 (stActHead *pActDATA, t_AIPARAM *pEVENT)		
{	
#ifdef	__SERVER
	AIACT30 *pAct = (AIACT30*)pActDATA;

	t_HASHKEY*pHash = (t_HASHKEY*)( pAct->szTrigger );

	pEVENT->m_pSourCHAR->Set_TRIGGER( *pHash );
#endif
}

void F_AIACT31 (stActHead *pActDATA, t_AIPARAM *pEVENT)		
{	
/*
// Share attacks have targeted the owner
struct AIACT31
{
	DWORD		dwSize;
	AITYPE		Type;
};
*/
#ifdef	__SERVER
	AIACT31 *pAct = (AIACT31*)pActDATA;

	CAI_OBJ *pOwner = pEVENT->m_pSourCHAR->Get_OWNER ();
	if ( pOwner ) {
		CAI_OBJ *pTarget = pOwner->Get_TARGET();
		if ( pTarget ) {
			// Attack
			if ( !pEVENT->m_pSourCHAR->Is_ALLIED( pTarget ) )
				pEVENT->m_pSourCHAR->SetCMD_RUNnATTACK( pTarget->Get_TAG() );
		}
	}
#endif
}

void F_AIACT32 (stActHead *pActDATA, t_AIPARAM *pEVENT)		
{	
/*
// PK Mode On / Off. Zone in the presence of NPC.
struct AIACT32
{
	DWORD		dwSize;
	AITYPE		Type;

	short		nZoneNo;	// Zone number, the current is unconditionally 0
	BYTE		btOnOff;	// 0 = Off , 1 = On
};
*/	
#ifdef	__SERVER
	AIACT32 *pAct = (AIACT32*)pActDATA;

	if ( 0 == pAct->nZoneNo ) {
		// Current zone...
		g_pZoneLIST->Set_PK_FLAG( pEVENT->m_pSourCHAR, pAct->btOnOff );
	} else {
		g_pZoneLIST->Set_PK_FLAG( pAct->nZoneNo, pAct->btOnOff );
	}
#endif
}

void F_AIACT_NULL (stActHead *pActDATA, t_AIPARAM *pEVENT)	
{
	;
}

//-------------------------------------------------------------------------------------------------
tagActionFUNC g_FuncACTION[] = {
	{	NULL		},
	{	F_AIACT00	},
	{	F_AIACT01	},
	{	F_AIACT02	},
	{	F_AIACT03	},
	{	F_AIACT04	},
	{	F_AIACT05	},
	{	F_AIACT06	},
	{	F_AIACT07	},
	{	F_AIACT08	},
	{	F_AIACT09	},
	{	F_AIACT10	},
	{	F_AIACT11	},
	{	F_AIACT12	},
	{	F_AIACT13	},
	{	F_AIACT14	},
	{	F_AIACT15	},
	{	F_AIACT16	},
	{	F_AIACT17	},
	{	F_AIACT18	},
	{	F_AIACT19	},
	{	F_AIACT20	},

	{	F_AIACT21	},

	{	F_AIACT22	},
	{	F_AIACT23	},
	{	F_AIACT24	},
	{	F_AIACT25	},
	{	F_AIACT26	},
	{	F_AIACT27	},
	{	F_AIACT28	},

	{	F_AIACT29	},
	{	F_AIACT30	},

	{	F_AIACT31	},
	{	F_AIACT32	},

	{	F_AIACT_NULL	},
	{	F_AIACT_NULL	},
	{	F_AIACT_NULL	},
	{	F_AIACT_NULL	},
	{	F_AIACT_NULL	},
	{	F_AIACT_NULL	},
	{	F_AIACT_NULL	},
	{	F_AIACT_NULL	},
	{	F_AIACT_NULL	},
	{	F_AIACT_NULL	},
	{	F_AIACT_NULL	},
	{	F_AIACT_NULL	},
	{	F_AIACT_NULL	},
	{	F_AIACT_NULL	},
	{	F_AIACT_NULL	},
	{	F_AIACT_NULL	},
	{	F_AIACT_NULL	},
	{	F_AIACT_NULL	},
	{	F_AIACT_NULL	},
	{	F_AIACT_NULL	},
	{	F_AIACT_NULL	},
	{	F_AIACT_NULL	},
	{	F_AIACT_NULL	},
	{	F_AIACT_NULL	},
	{	F_AIACT_NULL	},
	{	F_AIACT_NULL	},
	{	F_AIACT_NULL	},
	{	F_AIACT_NULL	},
	{	F_AIACT_NULL	},
	{	F_AIACT_NULL	},
	{	F_AIACT_NULL	},
	{	F_AIACT_NULL	},
	{	F_AIACT_NULL	},
	{	F_AIACT_NULL	},
} ;

//-------------------------------------------------------------------------------------------------
