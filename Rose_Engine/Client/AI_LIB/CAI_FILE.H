#ifndef	__CAI_FILE_H
#define	__CAI_FILE_H
#include "CAI_LIB.h"

#pragma pack()		// default

//-------------------------------------------------------------------------------------------------

#define	AIG_NAME_MAX	32
typedef	DWORD			AITYPE;


#define		AICOND_GENERAL				0x04000000						// General conditions

#define		AICOND_00					(0x000000001 | AICOND_GENERAL)	// -00 Condition, in their combat / standby when
#define		AICOND_01					(0x000000002 | AICOND_GENERAL)
#define		AICOND_02					(0x000000003 | AICOND_GENERAL)
#define		AICOND_03					(0x000000004 | AICOND_GENERAL)
#define		AICOND_04					(0x000000005 | AICOND_GENERAL)
#define		AICOND_05					(0x000000006 | AICOND_GENERAL)
#define		AICOND_06					(0x000000007 | AICOND_GENERAL)
#define		AICOND_07					(0x000000008 | AICOND_GENERAL)
#define		AICOND_08					(0x000000009 | AICOND_GENERAL)
#define		AICOND_09					(0x00000000A | AICOND_GENERAL)
#define		AICOND_10					(0x00000000B | AICOND_GENERAL)
#define		AICOND_11					(0x00000000C | AICOND_GENERAL)

/// Now is the day / night is?
#define		AICOND_12					(0x00000000D | AICOND_GENERAL)
/// His / target to check the status stake.
#define		AICOND_13					(0x00000000E | AICOND_GENERAL)
// NPC Variable Check
#define		AICOND_14					(0x00000000F | AICOND_GENERAL)
#define		AICOND_15					(0x000000010 | AICOND_GENERAL)
#define		AICOND_16					(0x000000011 | AICOND_GENERAL)

/// NPC Selection
#define		AICOND_17					(0x000000012 | AICOND_GENERAL)
/// And the owner of his character (his avatar summon) the distance between the check-
#define		AICOND_18					(0x000000013 | AICOND_GENERAL)

/// Check the current time zone
#define		AICOND_19					(0x000000014 | AICOND_GENERAL)


///////////////////////////////////////////////////////////////////////////////////////////////////
// The file header
// Immediately following the title of the file header file ohm
struct	AI_FILE_HEADER 
{
	int		iNumOfPattern ;
	int		iSecond ;				// Check standstill in seconds when
	int		iSecondOfAttackMove;	// Check chance when hit
	int		iNumOfTitle ;
} ;

//======================================================================================
//Structure Name : stEvent
//======================================================================================
struct stEvent {
	char szEventName[AIG_NAME_MAX] ;
	int		iNumOfCond ;
//	std::vector<stCond *> vecpCond ;
	int		iNumOfAct ;
//	std::vector<stAct *> vecpAct ;
} ;

//============================================================================
//struct stPattern
//============================================================================
struct stPattern 
{ 
	char						szName[AIG_NAME_MAX] ;
	int							iNumOfEvent ;
//	std::vector<stEvent *>		vecEvent ;
} ;





////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Struct Name : AICOND_00
// Explain : 
//
struct		AICOND00
{
	DWORD			dwSize ;					// The size of this structure
	AITYPE			Type ;						// Conditions, type
	BYTE			cNotfightOrDelay ;			// 0 = non-combat, 1 = standby
} ;

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Struct Name : AICOND_01
// Explain : 
//
struct		AICOND01
{
	DWORD			dwSize ;					// The size of this structure
	AITYPE			Type ;						// Conditions, type
	int				iDamage;					// The amount of damage
	BYTE			cRecvOrGive ;				// 0 = receive, 1 = feed
} ;

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Struct Name : AICOND_02
// Explain : 
//
/*
struct		AICOND02 
{
	DWORD			dwSize ;					// The size of this structure
	AITYPE			Type ;						// Conditions, type
	int				iDistance ;					// Check distance
	BYTE			cChrType ;					// Character type (avatars, monsters)
	char			cLevelDiff ;				// Level difference (-128 to 127)
	WORD			wChrNum ;					// Number of characters
} ;
*/
/****************************************************************************************************
 * Struct Name : AICOND_02 - <<2004 Year 1 Month 2 Revision>>
 * Check around the character situations (related to class CCondDlg_02)
 * Explain : Check cChrType character types within the scope and level difference iDistance cLevelDiff wChrNum more and greater than or equal to the number of characters
 */
struct		AICOND02
{
	DWORD			dwSize ;				// The size of this structure
	AITYPE			Type ;					// Conditions, type
	int				iDistance;				// Check distance
	BYTE			btIsAllied;				// Character type (enemy = 0, ally = 1)
	short			nLevelDiff ;			// Level difference (-128 to 127) over this
	short			nLevelDiff2;			// Level difference (-128 to 127) over this
	WORD			wChrNum;
	/*
	union {
		struct {
			char	cLevelDiff2 ;			// Level difference (-128 to 127) than this. Example: cLevelDiff cLevelDiff2 more and less. Modification 2 January 2004 << >>
			WORD	wChrNum ;				// Number of characters
		} ;
		struct {
			WORD	wChrNum_Before;
		} ;
	} ;
	*/
} ;



////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Struct Name : AICOND_03
// Explain : 
//
struct		AICOND03
{
	DWORD			dwSize ;					// The size of this structure
	AITYPE			Type ;						// Conditions, type
	int				iDistance ;					// Distance
} ;

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Struct Name : AICOND_04
// Explain : 
//
struct		AICOND04
{
	DWORD			dwSize ;					// The size of this structure
	AITYPE			Type ;						// Conditions, type
	int				iDistance ;					// Distance
	BYTE			cMoreLess ;					// Less than more than
} ;

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Struct Name : AICOND_05
// Explain : 
//
struct		AICOND05
{
	DWORD			dwSize ;					// The size of this structure
	AITYPE			Type ;						// Conditions, type
//	int				cAbType ;					// Base type
	BYTE			cAbType ;					// Base type
	int				iDiff ;
	BYTE			cMoreLess ;					// Big, jakeunga
} ;

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Struct Name : AICOND_06
// Explain : 
//
struct		AICOND06
{
	DWORD			dwSize ;					// The size of this structure
	AITYPE			Type ;						// Conditions, type
	DWORD			wHP ;						// HP Figures
	BYTE			cMoreLess ;					// Big, jakeunga
} ;

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Struct Name : AICOND_07
// Explain : 
//
struct		AICOND07
{
	DWORD			dwSize ;					// The size of this structure
	AITYPE			Type ;						// Conditions, type
	BYTE			cPercent ;					// Probability
} ;

struct		AICOND08
{
	DWORD			dwSize ;					// The size of this structure
	AITYPE			Type ;						// Conditions, type
	int				iDistance ;					// Distance
	short			nLevelDiff ;				// Level difference (-128 to 127) than their level at least cLevelDiff
	short			nLevelDiff2;				// Level difference (-128 to 127) than their level at least cLevelDiff
	//BYTE			cChrType ;					// Character type (avatar = 0, Monster = 1)
	BYTE			btIsAllied;					// Character type (avatar = 0, Monster = 1)
} ;

struct		AICOND09
{
	DWORD			dwSize ;					// The size of this structure
	AITYPE			Type ;						// Conditions, type
} ;

struct		AICOND10
{
	DWORD			dwSize ;					// The size of this structure
	AITYPE			Type ;						// Conditions, type
	BYTE			cAbType ;					// Base type
	BYTE			cMoreLess ;					// Large = 0, 1 = small
} ;

/*******************************************************************************************
 * Struct Name : AICOND_11
 * Explain : Their abilities to attack the character of the A and B values ??greater than or / small
 */
struct		AICOND11
{
	DWORD			dwSize ;		// The size of this structure
	AITYPE			Type ;			// Conditions, type = AICOND_11
	BYTE			cAbType ;		// Stats (level = 0, Damage = 1, DEF = 2, anti-hp = 3, HP = 4, attractive = 5)
	int				iValue;			// Comparison value. Greater than this value / jakeunga. In this way
	BYTE			cMoreLess ;		// Large = 0, 1 = small
} ;


// From here, the server compares the source and correct
/// Now is the day / night is?
struct		AICOND12
{
	DWORD			dwSize ;		// The size of this structure
	AITYPE			Type ;			// Conditions, type = AICOND_12
	BYTE			cWhen;			// Check whether 0 = low, 1 = checks to see if the night
};

/// His / target to check the status stake.
struct		AICOND13
{
	DWORD			dwSize;
	AITYPE			Type;

	BYTE			btCheckTarget;	// 0 = self, 1 = attack target
	BYTE			btStatusType;	// 0 = harmful, 1 = favorable, 3 = harmful / beneficial Anything
	BYTE			btHave;			// 0 = not? 1 = do?
};

// NPC Variable Check	- AICOND14 AIACT25
// World Variable Check	- AICOND15 AIACT26
// Economic data check - AICOND16 AIACT27
typedef struct	tagValueAI
{
	DWORD			dwSize;
	AITYPE			Type;

	short			nVarNo;	// Variable number: 0 -, John of economic data variables
	int				iValue;	// Compare data values ??(iType abilities Op iValue)
	BYTE			btOp;	// how to compare value in iValue. 0 = same, 1 = large, 2 = greater than or equal to. 3 = small, 4 = Less than or equal to.
							// (Side action) 5 = value breaks, increased 6 = 7 = reduced
} AICOND14, AICOND15, AICOND16, AIACT25, AIACT26, AIACT27;


/// NPC Selection
#define		AICOND_17					(0x000000012 | AICOND_GENERAL)

/// NPC Selection
struct AICOND17
{
	DWORD			dwSize;
	AITYPE			Type;

//	short			nZoneNo;
	int				iNpcNo;		/// NPC selected number
};


/// And the owner of his character (his avatar summon) the distance between the check-in using recalled mopman
struct AICOND18
{
	DWORD			dwSize;
	AITYPE			Type;


	int				iDistance;	// NPC selected number. (The owner is the distance Op iDistance)
	BYTE			btOp;		// 0 = same, 1 = large, 2 = greater than or equal to. 3 = small, 4 = Less than or equal to.

	// If you take advantage of the following related summoned monsters away. Wiman currently using 2004-4-9
	// BYTE			btOrg;		// 0 = summon the character, ...
};


/// Check the current time zone
struct AICOND19
{
	DWORD			dwSize;
	AITYPE			Type;

	unsigned long	ulTime;		//
	unsigned long	ulEndTime;	// ulTime <= Current time zone <= ulEndTime Is ?
};


// His (monster) stat / etc / host character-related data check
struct AICOND20
{
	DWORD	dwSize;
	AITYPE	Type;

	BYTE	btAbType ;		// Stats (level = 0, Damage = 1, DEF = 2, anti-hp = 3, HP = 4, attractive = 5)

	int		iValue;	// Compare data values ??(iType abilities Op iValue)
	BYTE	btOp;	// iData for value comparison method. 0 = same, 1 = large, 2 = greater than or equal to. 3 = small, 4 = Less than or equal to.			
};

// Character is present in the host check
struct AICOND21
{
	DWORD	dwSize;
	AITYPE	Type;
};

// The current owner of the attack target, the character have?
struct AICOND22
{
	DWORD	dwSize;
	AITYPE	Type;
};

/// World time check
struct AICOND23
{
	DWORD			dwSize;
	AITYPE			Type;

	unsigned long	ulTime;		//
	unsigned long	ulEndTime;	// ulTime <= current time zone <= ulEndTime is?
};


/// Check the date + time
struct AICOND24
{
	DWORD			dwSize;
	AITYPE			Type;

	BYTE			btDate;		// Date. (1 ~ 31)

	BYTE			btHour1;	// O'clock(1 ~ 24)
	BYTE			btMin1;		// Minute (1 ~ 60)
	BYTE			btHour2;	// O'clock
	BYTE			btMin2;		// Minute
	//  And the current date is btDate, btHour1 btMin1 minutes hour <= current time <= btHour1 minutes hour true btMin1
};

/// Check the day + time
struct AICOND25
{
	DWORD			dwSize;
	AITYPE			Type;

	BYTE			btWeekDay;	// Day of week (0-6)

	BYTE			btHour1;	// hour
	BYTE			btMin1;		// minute
	BYTE			btHour2;	
	BYTE			btMin2;		
	//  BtWeekDay the current day of the week and, btHour1 btMin1 minutes hour <= current time <= btHour1 minutes hour true btMin1
};

/// Check the channel server
struct AICOND26
{
 	DWORD			dwSize;
 	AITYPE			Type;
 	unsigned short	nX; 
 	unsigned short	nY; // (nX <= channel server <= nY) is true
};


struct stCondHead 
{
	DWORD	dwSize ;
	AITYPE	Type ;
} ;

struct AI_CONDITION		// stCond
{
	union 
	{
		BYTE			m_pDATA[ 1 ];
		stCondHead		m_Header;
		/*
		struct 
		{
			DWORD	dwSize ;
			AITYPE	Type ;
		} ;
		*/
		//------------------------------------------------------------------------------
		// List all structures less than halkkeot jongeon
		AICOND00 st00;
		AICOND01 st01;
		AICOND02 st02;
		AICOND03 st03;
		AICOND04 st04;
		AICOND05 st05;
		AICOND06 st06;
		AICOND07 st07;
		AICOND08 st08;
		AICOND09 st09;
		AICOND10 st10;

		AICOND11 st11;
		AICOND12 st12;
		AICOND13 st13;
		AICOND14 st14;
		AICOND15 st15;
		AICOND16 st16;
		AICOND17 st17;

		AICOND18 st18;
	} ;
} ;


#define		AIACT_GENERAL				0x0B000000

#define		AIACT_00					(0x000000001 | AIACT_GENERAL) 
#define		AIACT_01					(0x000000002 | AIACT_GENERAL) 
#define		AIACT_02					(0x000000003 | AIACT_GENERAL) 
#define		AIACT_03					(0x000000004 | AIACT_GENERAL) 
#define		AIACT_04					(0x000000005 | AIACT_GENERAL) 
#define		AIACT_05					(0x000000006 | AIACT_GENERAL) 
#define		AIACT_06					(0x000000007 | AIACT_GENERAL) 
#define		AIACT_07					(0x000000008 | AIACT_GENERAL) 
#define		AIACT_08					(0x000000009 | AIACT_GENERAL) 
#define		AIACT_09					(0x00000000A | AIACT_GENERAL) 
#define		AIACT_10					(0x00000000B | AIACT_GENERAL) 
#define		AIACT_11					(0x00000000C | AIACT_GENERAL) 
#define		AIACT_12					(0x00000000D | AIACT_GENERAL)
#define		AIACT_13					(0x00000000E | AIACT_GENERAL)
#define		AIACT_14					(0x00000000F | AIACT_GENERAL)
#define		AIACT_15					(0x000000010 | AIACT_GENERAL)
#define		AIACT_16					(0x000000011 | AIACT_GENERAL)
#define		AIACT_17					(0x000000012 | AIACT_GENERAL)

#define		AIACT_18					(0x000000013 | AIACT_GENERAL)
#define		AIACT_19					(0x000000014 | AIACT_GENERAL)
#define		AIACT_20					(0x000000015 | AIACT_GENERAL)

// #define		AIACT_21					(0x000000016 | AIACT_GENERAL)	// Technology implementation. Disable **
#define		AIACT_22					(0x000000017 | AIACT_GENERAL)	// Summon the monster to the specified location on the specified
#define		AIACT_23					(0x000000018 | AIACT_GENERAL)	// Discard the suicide
#define		AIACT_24					(0x000000019 | AIACT_GENERAL)	// Toward the target skill / behavior implemented


#define		AIACT_25					(0x00000001A | AIACT_GENERAL)	
#define		AIACT_26					(0x00000001B | AIACT_GENERAL)	
#define		AIACT_27					(0x00000001C | AIACT_GENERAL)	
#define		AIACT_28					(0x00000001D | AIACT_GENERAL)	

// Master character (summoned by his character) change in
#define		AIACT_29					(0x00000001E | AIACT_GENERAL)	
// Trigger Execution
#define		AIACT_30					(0x00000001F | AIACT_GENERAL)	

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Struct Name : AIACT00
// Explain : 
//

struct		AIACT00
{
	DWORD			dwSize ;					// The size of this structure
	AITYPE			Type ;						// Conditions, type
} ;

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Struct Name : AIACT01
// Explain : 
//

struct		AIACT01
{
	DWORD			dwSize ;					// The size of this structure
	AITYPE			Type ;						// Conditions, type
	BYTE			cAction ;					// Operation Number
} ;

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Struct Name : AIACT02
// Explain : 
//

struct		AIACT02
{
	DWORD			dwSize ;					// The size of this structure
	AITYPE			Type ;						// Conditions, type
	// char			szCon[1] ;					// Metabolic data
	int				iStrID;
} ;
struct		AIACT02_STR
{
	DWORD			dwSize ;					// The size of this structure
	AITYPE			Type ;						// Conditions, type
	char			szCon[1] ;					// Metabolic data
} ;

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Struct Name : AIACT03
// Explain : 
//

struct		AIACT03
{
	DWORD			dwSize ;					// The size of this structure
	AITYPE			Type ;						// Conditions, type
	int				iDistance ;					// Distance
	BYTE			cSpeed ;					// 0: Walking, 1: Run
} ;

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Struct Name : AIACT04
// Explain : 
//

struct		AIACT04
{
	DWORD			dwSize ;					// The size of this structure
	AITYPE			Type ;						// Conditions, type
	int				iDistance ;					// Distance
	BYTE			cSpeed ;					// 0: Walking, 1: Run
} ;

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Struct Name : AIACT05
// Explain : 
//
struct		AIACT05
{
	DWORD			dwSize ;					// The size of this structure
	AITYPE			Type ;						// Conditions, type
	BYTE			cSpeed ;					// 0: Walking, 1: Run
	//BYTE			cAttackOrMove ;				// Attack, move
} ;

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Struct Name : AIACT06
// Explain : 
//

struct		AIACT06
{
	DWORD			dwSize ;					// The size of this structure
	AITYPE			Type ;						// Conditions, type
	int				iDistance ;					// Distance
	BYTE			cAbType ;					// Base type
	BYTE			cMoreLess ;					// Big, jakeunga
} ;

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Struct Name : AIACT07
// Explain : 
//

struct		AIACT07
{
	DWORD			dwSize ;					// The size of this structure
	AITYPE			Type ;						// Conditions, type
												// No data
} ;

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Struct Name : AIACT08
// Explain : 
//

struct		AIACT08
{
	DWORD			dwSize ;					// The size of this structure
	AITYPE			Type ;						// Conditions, type
	int				iDistance ;					// Distance
	BYTE			cSpeed ;					// 0: Walking, 1: Run
} ;

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Struct Name : AIACT09
// Explain : 
//

typedef struct		tagAIACT09
{
	DWORD			dwSize ;					// The size of this structure
	AITYPE			Type ;						// Conditions, type
//	BYTE			cMonster ;					// Monster Numbers
	WORD			wMonster ;					// Monster Numbers
} AIACT09 , AIACT10 ;

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Struct Name : AIACT10
// Explain : 
//

//struct		AIACT10
//{
//	DWORD			dwSize ;					// The size of this structure
//	AITYPE			Type ;						// Conditions, type
//} ;

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Struct Name : AIACT11
// Explain : 
//

struct		AIACT11
{
	DWORD			dwSize ;					// The size of this structure
	AITYPE			Type ;						// Conditions, type
	int				iDistance ;					// Distance
//	BYTE			cCallType ;					// Calling the attack mode, (all, being out of combat)
	int				iNumOfMonster;
} ;


struct		AIACT12
{
		DWORD			dwSize ;		// The size of this structure
		AITYPE			Type ;			// Conditions, type
} ;


struct		AIACT13
{
		DWORD			dwSize ;		// The size of this structure
		AITYPE			Type ;			// Conditions, type
} ;

struct		AIACT14
{
	DWORD			dwSize ;					// The size of this structure
	AITYPE			Type ;						// Conditions, type
	int				iDistance ;					// Distance
} ;

struct		AIACT15
{
	DWORD			dwSize ;					// The size of this structure
	AITYPE			Type ;						// Conditions, type
} ;


/**********************************************************************************
 * Struct Name : AIACT16
 * Away: mobile and delimited. Matahdo keep running away feeling during
 */
struct AIACT16
{
	DWORD	dwSize;		/// The size of this structure
	AITYPE	Type;		/// Type = AIACT_16 . Escape
	int		iDistance ;	/// Within a few meters away and the center of Agrigento
};

/**********************************************************************************
 * Struct Name : AIACT17
 * Away: Item number 5 has a randomly dropping. The 5-digit item number
 */
struct AIACT17
{
	DWORD	dwSize;		/// The size of this structure
	AITYPE	Type;		/// Type = AIACT_16 . Escape
	union {
		struct {
			short	item0;		/// Item Number 0  . If put into -1 if no item number
			short	item1;		/// Item Number 1
			short	item2;		/// Item Number 2
			short	item3;		/// Item Number 3
			short	item4;		/// Item Number 4 
		} ;
		short	m_ITEMS[ 5 ];
	} ;
};


/**********************************************************************************
 * Struct Name : AIACT18 - <<2004.1.29 More>>
 * Specified number of non-combatant commands are invoked to attack mobs. (Monster numbers, distance, number of animals)
 * Additional Explanation: The specified distance in a specified number of a specified number of dogs attacking a monster call instruction
 */
struct AIACT18
{
	DWORD		dwSize;			/// The size of this structure
	AITYPE		Type;			/// Type the value in the AIACT_18

	WORD		cMonster ;		// Monster Numbers
	WORD		wHowMany ;		// Some of them
	int			iDistance ;		// Distance
};


/**********************************************************************************
 * Struct Name : AIACT19 - <<2004.1.29 More>>
 * Avatar to attack the nearest target designation
 */
struct AIACT19
{
	DWORD		dwSize;				/// The size of this structure
	AITYPE		Type;				/// Type the value in the AIACT_19

	/// Requires additional conditions
};


/**********************************************************************************
 * Struct Name : AIACT20 - <<2004.1.29 More>>
 * Specified in the summons monsters. His / attacker (the attack on his character) / attack a target (he is attacking the character).
 * Nearby? Meters summons within
 * Additional comments: one is the number of animals.
 */
struct AIACT20
{
	DWORD		dwSize;				/// The size of this structure
	AITYPE		Type;				/// Type the value in the AIACT_20

	WORD		cMonster;			/// Monster Numbers
	BYTE		btPos;				/// Summon position: 0 = his surroundings, around 1 = attacker (he attacked), 2 = attacked targets around (in their attacks)
	int			iDistance ;			// Distance. Summons within this distance
};



/**********************************************************************************
 * Struct Name : AIACT22 - <<2004.2.21 추가>>
 * Summon monsters to a specified location on the specified
 */
/*
struct AIACT22
{
	DWORD		dwSize;			/// The size of this structure
	AITYPE		Type;			/// Type the value in the AIACT_22

};
*/

// The suicide
struct AIACT23
{
	DWORD	dwSize;
	AITYPE	Type;
};

/// Using skills and behavior toward the target
struct AIACT24
{
	DWORD		dwSize;			/// The size of this structure
	AITYPE		Type;			/// Type

	BYTE		btTarget;		// Check condition of the character 0 =, 1 = current attack target, 2 = oneself (self skill)
	short		nSkill;			// Skill number
	short		nMotion;		// Number -1 = no action motion
};

struct AIACT28
{
	DWORD		dwSize;			/// The size of this structure
	AITYPE		Type;			/// Type

	BYTE		btMsgType;		/// 0 = the current field, 1 = current zone, 2 = Full World
	// short	nMsgLength;		/// Metabolism, including the length of the NULL
	// char		szMsg[ 1 ];		/// Metabolism
	int			iStrID;
};
struct AIACT28_STR
{
	DWORD		dwSize;			/// The size of this structure
	AITYPE		Type;			/// Type

	BYTE		btMsgType;		/// 0 = the current field, 1 = current zone, 2 = Full World
	char		szMsg[ 1 ];		/// Metabolism
};


// Master character (one character to summon his) change in
struct AIACT29
{
	DWORD		dwSize;
	AITYPE		Type;

	// 2004-4-9 Until now, no data
	// short	nCheckUnit; // Check how many units. This also can be fit. Units = time frame
	// BYTE		btSpeed; // 0 = slow, 1 = fast (Maybe has a small movement speed go to the patient)
	// BYTE		btAddOp; // If you summon dead party (or absence) No return position
};


// Trigger Execution
struct AIACT30
{
	DWORD		dwSize;
	AITYPE		Type;

	short		shNameLen;

//  union If you set the wrong struct structure into a rough alignment editor and M
//	union {
//		t_HASHKEY	m_HashNextTrigger[0];	/// Loading trigger name change swigap Sikkim H. 
		char		szTrigger[ 1 ];	// 트리거명. NULL포함
//	} ;
};


// Share attacks have targeted the owner
struct AIACT31
{
	DWORD		dwSize;
	AITYPE		Type;
};

// PK mode On / Off. Zone in the presence of NPC.
struct AIACT32
{
	DWORD		dwSize;
	AITYPE		Type;

	short		nZoneNo;	// Zone number, the current is unconditionally 0
	BYTE		btOnOff;	// 0 = Off , 1 = On
};


struct stActHead 
{
	DWORD	dwSize ;
	AITYPE	Type ;
} ;
struct AI_ACTION	// stAct
{
	union 
	{
		BYTE			m_pDATA[ 1 ];
		stActHead		m_Header;
		/*
		struct
		{
			DWORD	dwSize ;
			AITYPE	Type ;
		} ;
		*/
		AIACT00			st00 ;
		AIACT01			st01 ;
		AIACT02			st02 ;
		AIACT03			st03 ;
		AIACT04			st04 ;
		AIACT05			st05 ;
		AIACT06			st06 ;
		AIACT07			st07 ;
		AIACT08			st08 ;
		AIACT09			st09 ;
		AIACT10			st10 ;
		AIACT11			st11 ;
		AIACT12			st12 ;
		AIACT13			st13 ;
		AIACT14			st14 ;
		AIACT15			st15 ;
		AIACT16			st16 ;
		AIACT17			st17 ;
		AIACT18			st18 ;
		AIACT19			st19 ;
		AIACT20			st20 ;

		// AIACT22		st22 ;
		AIACT23			st23 ;
		AIACT24			st24 ;
		AIACT25			st25 ;
		AIACT26			st26 ;
		AIACT27			st27 ;
		AIACT28			st28 ;
		AIACT29			st29 ;
		AIACT30			st30 ;
		AIACT31			st31 ;

		AIACT32			st32 ;
	} ;
} ;


//-------------------------------------------------------------------------------------------------
struct t_AIPARAM {
	CAI_OBJ	*m_pSourCHAR;	
	CAI_OBJ	*m_pDestCHAR;
	CAI_OBJ	*m_pFindCHAR;
	CAI_OBJ	*m_pNearCHAR;
#ifdef	__SERVER
	CObjNPC *m_pLocalNPC;
#endif

	int	     m_iDamage;
} ;

class CFileSystem;
class CAI_EVENT
{
private:
	AI_CONDITION  **m_ppConditionLIST;
	int				m_iConditionCNT;

	AI_ACTION	  **m_ppActionLIST;
	int				m_iActionCNT;

public :
	CAI_EVENT ();
	~CAI_EVENT ();

#ifdef	__SERVER
	bool Load (FILE *fp, STBDATA *pSTB, int iLangCol);
#else
	bool Load (CFileSystem* pFileSystem, STBDATA *pSTB, int iLangCol );
#endif
	bool Check (t_AIPARAM *pAIParam);
} ;

//-------------------------------------------------------------------------------------------------


class CAI_PATTERN
{
private:
	CAI_EVENT	*m_pEvent;
	int			 m_iEventCNT;

public :
	CAI_PATTERN ();
	~CAI_PATTERN ();

#ifdef	__SERVER
	bool Load ( FILE *fp, STBDATA *pSTB, int iLangCol );
#else
	bool Load ( CFileSystem* pFileSystem, STBDATA *pSTB, int iLangCol );
#endif
	void Check( t_AIPARAM *pAIParam );
} ;


//-------------------------------------------------------------------------------------------------
struct tagConditionFUNC {
	bool (*m_fpCondition) (stCondHead *pConDATA, t_AIPARAM *pParam);
} ;
extern tagConditionFUNC g_FuncCOND[];

struct tagActionFUNC {
	void (*m_fpAction) (stActHead *pActDATA, t_AIPARAM *pParam);
} ;
extern tagActionFUNC	g_FuncACTION[];


//-------------------------------------------------------------------------------------------------

int  AI_Get_Ability (CAI_OBJ *pCHAR, int iAbTYPE);

//-------------------------------------------------------------------------------------------------

template <class dType1, class dType2>
inline bool Check_AiOP( BYTE btOP, dType1 iLeft, dType2 iRight)
{
	bool bResult;
	switch( btOP ) {
		case 0 :	// Like
			bResult = ( iLeft == iRight );	break;
		case 1 :	// Greater
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
			return false;
	}

	return bResult;
}

template <class dType>
inline dType Result_AiOP( BYTE btOP, dType iLeft, dType iRight)
{
	switch( btOP ) {
		case 5 :	// 값바꿈
			return iRight;
		case 6 :	// 증가(주어진 만큼)
			return (iLeft+iRight);
		case 7 :	// 감소
			return (iLeft-iRight);
	}
	return 0;
}

//-------------------------------------------------------------------------------------------------
#endif
