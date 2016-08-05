
#ifndef	__CUSERDATA_H
#define	__CUSERDATA_H
#include "CQuest.h"
#include "CInventory.h"
#include "CHotICON.h"
#include "Calculation.h"
#include "IO_STB.H"
#include "IO_Skill.h"
#include "MileageInv.h"

#pragma warning (disable:4201)
//-------------------------------------------------------------------------------------------------

#ifdef	__BORLANDC__
#include <crtdbg.h>
#endif

#pragma pack (push, 1)
//-------------------------------------------------------------------------------------------------

#define	MAX_INT		0x07fffffff

/// Basic Data ...
struct tagBasicINFO {
	// NAME
	// SEX
	char	m_cBirthStone;		// Birthstone
	char	m_cFaceIDX;			// Face Shape
	char	m_cHairIDX;			// �Ӹ� ���
	short	m_nClass;			// ���� ( 1~30 )
	char	m_cUnion;			// �Ҽ� ���� ( 1~20 )
	char	m_cRank;			// ��� ( 1~15 )
	char	m_cFame;			// �� ( 0 ~ 99 )
	void Init (char cBirthStone, char cFaceIDX, char cHairIDX)
	{
		m_cBirthStone = cBirthStone;		
		m_cFaceIDX    = cFaceIDX;			
		m_cHairIDX    = cHairIDX;			

		m_nClass = 0;
		m_cUnion = 0;
		m_cRank  = 0;
		m_cFame  = 0;
	}
} ;

/// Basic stats data ...
struct tagBasicAbility {
	union {
		struct {
			short	m_nSTR;			// �ٷ� 1~200
			short	m_nDEX;			// ��ø 1~200
			short	m_nINT;			// ���� 1~200
			short	m_nCON;			// concentration ���߷� 1~200
			short	m_nCHARM;		// �ŷ� 1~200
			short	m_nSENSE;		// ���� 1~200
		} ;
		short		m_nBasicA[ BA_MAX ];
	} ;
	void Init () 
	{
		m_nSTR = 10;
		m_nDEX = 10;
		m_nINT = 10;
		m_nCON = 10;
		m_nCHARM = 10;
		m_nSENSE = 10;
	}
} ;


#define	MAX_BASIC_ABLITY		500		// 250 Ability to modify the default maximum of 300
#define	MAX_LEVEL				260		// 200 Modifying the maximum level of 250
#define	MAX_EXPERIENCE			0x0fffffff


#define	MAX_MAINTAIN_STATUS		40		// 8 * 50 = 400

 struct tagMaintainSTATUS {
 	DWORD	m_dwExpiredSEC;
 	short	m_nValue;
 	short	m_nDummy;
 } ;

/// Growth stats data ...
struct tagGrowAbility {
	short	m_nHP;				// 0~2000
	short	m_nMP;				// 0~1000

	//PY added	not sure what use they will be yet
	short	m_MaxHP;
	short	m_MaxMP;

	long	m_lEXP;				// Experience 1~100000
	short	m_nLevel;			// 0~250
	short	m_nBonusPoint;		// 1~999
	short	m_nSkillPoint;		// 1~
	BYTE	m_btBodySIZE;		// Body sizes
	BYTE	m_btHeadSIZE;		// Head size
	long	m_lPenalEXP;		// Add experience ...

	short	m_nFameG;			// Leading Indicators: Increased by Quest: 05/27/2004 added
	short	m_nFameB;			// Leading Indicators: Increased by Quest: 05/27/2004 added
	short	m_nUnionPOINT[ MAX_UNION_COUNT ];	// Point combinations: Add 05/27/2004

	int		m_iGuildNO;			// Guild Code: Add 27/05/2004
	short	m_nGuildCNTRB;		// Guild Contribution: 05/27/2004 added
	BYTE	m_btGuildPOS;		// Guild Position: 05.27.2004 Add

	short	m_nPKFlag;			// 2004. 6. 17 added..
	short	m_nSTAMINA;			// 2004. 8. 23 added..

	tagMaintainSTATUS	m_STATUS[ MAX_MAINTAIN_STATUS ];

#ifdef _GBC
	short	m_nPatHP;
	DWORD	m_dwPatCoolTIME;
#endif
/*
	char	m_cChaos;			// Disposition
	short	m_nBattle_LV;		// Battle Level
	short	m_nPK_LV;			//
*/
	void Init () 
	{
		m_nHP = 50;
		m_nMP = 40;

		m_lEXP = 0;
		m_nLevel = 1;
		m_nBonusPoint = 0;
		m_nSkillPoint = 0;
		m_btBodySIZE = 100;		// Body sizes
		m_btHeadSIZE = 100;		// Head size
		m_lPenalEXP = 0;

		m_nFameG = m_nFameB = 0;
		::ZeroMemory( m_nUnionPOINT, sizeof(m_nUnionPOINT) );
		m_iGuildNO = m_nGuildCNTRB = m_btGuildPOS = 0;

		m_nPKFlag = 0;
	}
} ;


/// Avatars hold skill-set to 120 proposals on the use of 90 extra
#ifdef __MILEAGESKILL
#define	MAX_LEARNED_SKILL_CNT			362
#else
#define	MAX_LEARNED_SKILL_CNT			162
#endif

#define	MAX_LEARNED_SKILL_PAGE			4
#define	MAX_LEARNED_SKILL_PER_PAGE		( MAX_LEARNED_SKILL_CNT/MAX_LEARNED_SKILL_PAGE )

#ifdef __MILEAGESKILL
#define MAX_SKILL_TAB		5
#else
#define MAX_SKILL_TAB		4       //Vocational Skills -0, -1 Parts skills, unique skills -2
#endif


#define MAX_JOB_SKILL		60
#define MAX_PATS_SKILL		30
#define MAX_UNIQUE_SKILL	30

#ifdef __MILEAGESKILL
#define MAX_MILEAGE_SKILL	200
#else
#define MAX_MILEAGE_SKILL	0
#endif


#define BEGIN_JOB_SKILL_INDEX			0
#define BEGIN_PATS_SKILL_INDEX			MAX_JOB_SKILL
#define BEGIN_UNIQUE_SKILL_INDEX		MAX_JOB_SKILL+MAX_PATS_SKILL
#define BEGIN_MILEAGE_SKILL_INDEX		MAX_JOB_SKILL+MAX_PATS_SKILL+MAX_UNIQUE_SKILL


#define MAX_SKILL_INV_CNT (MAX_JOB_SKILL+MAX_PATS_SKILL+MAX_UNIQUE_SKILL+MAX_MILEAGE_SKILL)

#define MAX_COMMON_SKILL 42


struct SkillInv
{
	short m_nJobSkill[MAX_JOB_SKILL];               //Vocational Skills
	short m_nPatsSKill[MAX_PATS_SKILL];				//Parts Skills
	short m_nUniqueSkill[MAX_UNIQUE_SKILL];			//Unique Skills


#ifdef __MILEAGESKILL
	short m_nMileageSkill[MAX_MILEAGE_SKILL];		//Mileage skills
#endif
	
};

struct tagSkillAbility 
{
	union
	{
		short	m_nSkillINDEX[ MAX_LEARNED_SKILL_CNT ];
		short	m_nIndex [ MAX_LEARNED_SKILL_PAGE ][ MAX_LEARNED_SKILL_PER_PAGE ];	// Battle, commercial / manufacturing, each holding up to 10 passive skill.
		SkillInv m_nSkillInv;
	} ;
	void Init ()
	{
		::ZeroMemory( m_nSkillINDEX,	sizeof(short) * MAX_LEARNED_SKILL_CNT );
	}
} ;


/// Quest data ...

// #define	__APPLY_EXTAND_QUEST_VAR		// 05.05.21 Additional extensions switches and parameters if applicable.

#define	QUEST_PER_PLAYER			10		// Per person can be registered up to the quest ...
#define	QUEST_PER_PLAYER_CW			QUEST_PER_PLAYER-1

#define	QUEST_EPISODE_VAR_CNT		5		// A variable number of episodes progress
#define	QUEST_JOB_VAR_CNT			3		// Career progression of variable stars
#define	QUEST_PLANET_VAR_CNT		7		// Planet star ...
#define	QUEST_UNION_VAR_CNT			10		// By combining ...

// Quest progression variable number of treatments for keulraenjeon
//	Kim, Young - Hwan added.
#ifdef __CLAN_WAR_SET
	#define	QUEST_CLANWAR_VAR_CNT		10			// Keulraenjeon number of variables in progress ...
#endif

#ifdef	__APPLY_EXTAND_QUEST_VAR
	#define	QUEST_ETC_VAR_CNT			53
	#define	QUEST_SWITCH_CNT			(512+512)	// Quest can switch the whole proceeding.
#else
	#define	QUEST_SWITCH_CNT			(512    )	// Quest can switch the whole proceeding.
#endif
#define	QUEST_SWITCH_GROUP_CNT	  ( QUEST_SWITCH_CNT/sizeof(DWORD) )

struct tagQuestData {
private:
	enum {	BIT_SHIFT=3, WHICH_BIT=0x07 };
	BYTE GetBit (int iIndex)
	{
		return ( m_btSWITCHES[ iIndex >> BIT_SHIFT ] & g_btSwitchBitMask[ iIndex & WHICH_BIT ] ) ? 1 : 0;
	}
	void SetBit (int iIndex)
	{
		m_btSWITCHES[ iIndex >> BIT_SHIFT ] |= g_btSwitchBitMask[ iIndex & WHICH_BIT ];
	}
	void ClearBit (int iIndex)
	{
		m_btSWITCHES[ iIndex >> BIT_SHIFT ] &= ~(g_btSwitchBitMask[ iIndex & WHICH_BIT ]);
	}
	BYTE FlipBit (int iIndex)
	{
		if ( this->GetBit( iIndex ) )
			this->ClearBit( iIndex );
		else
			this->SetBit ( iIndex );

		return this->GetBit( iIndex );
	}

public :
	short		m_nEpisodeVAR[ QUEST_EPISODE_VAR_CNT	];
	short		m_nJobVAR	 [ QUEST_JOB_VAR_CNT		];
	short		m_nPlanetVAR [ QUEST_PLANET_VAR_CNT		];
	short		m_nUnionVAR  [ QUEST_UNION_VAR_CNT		];
	CQUEST		m_QUEST		 [ QUEST_PER_PLAYER			];
	union {
		DWORD	m_dwSWITCHES [ QUEST_SWITCH_CNT/32		];
		BYTE	m_btSWITCHES [ QUEST_SWITCH_CNT/8		];
	} ;
#ifdef	__APPLY_EXTAND_QUEST_VAR
	short		m_nEtcVAR	 [ QUEST_ETC_VAR_CNT		];		// 2 * 50
#endif
#ifdef __CLAN_WAR_SET
	//	Add your quest for progress buffers keulraenjeon
	//	Kim, Young - Hwan added.
	short		m_nClanWarVAR[QUEST_CLANWAR_VAR_CNT		];		// 2 * 10 = 20 BYTE 
#endif

	void Init ()
	{
		::ZeroMemory( m_nEpisodeVAR,	sizeof(short) * QUEST_EPISODE_VAR_CNT	);
		::ZeroMemory( m_nJobVAR,		sizeof(short) * QUEST_JOB_VAR_CNT		);
		::ZeroMemory( m_nPlanetVAR,		sizeof(short) * QUEST_PLANET_VAR_CNT	);
		::ZeroMemory( m_nUnionVAR,		sizeof(short) * QUEST_UNION_VAR_CNT		);
#ifdef	__APPLY_EXTAND_QUEST_VAR
		::ZeroMemory( m_nEtcVAR,		sizeof(short) * QUEST_ETC_VAR_CNT		);
#endif
#ifdef __CLAN_WAR_SET
		//	Quest progression buffer initialized for keulraenjeon
		//	Kim, Young - Hwan added.
		::ZeroMemory( m_nClanWarVAR,	sizeof(short) * QUEST_CLANWAR_VAR_CNT	);
#endif

		this->ClearAllSwitchs ();

		for (short nI=0; nI<QUEST_PER_PLAYER; nI++) {
			m_QUEST[ nI ].Init ();
		}
	}

	//-------------------------------------------------------------------------------------------------
	void ClearAllSwitchs ()
	{
		::ZeroMemory (m_dwSWITCHES, sizeof(m_dwSWITCHES));
	}
	void Set_SWITCH (int iSwitchNO, int iValue)
	{
		if ( iSwitchNO < 0 || iSwitchNO >= QUEST_SWITCH_CNT )
			return;

		if ( iValue )
			this->SetBit( iSwitchNO );
		else
			this->ClearBit( iSwitchNO );
	}
	int  Get_SWITCH (int iSwitchNO)
	{
		if ( iSwitchNO < 0 || iSwitchNO >= QUEST_SWITCH_CNT )
			return -1;

		return this->GetBit( iSwitchNO );
	}
	int  Flip_SWITCH(int iSwitchNO)
	{
		if ( iSwitchNO < 0 || iSwitchNO >= QUEST_SWITCH_CNT )
			return -1;

		return this->FlipBit( iSwitchNO );
	}
} ;


#define	BANKSLOT_TOTAL_SIZE		160
struct tagBankData {
	tagITEM		m_ItemLIST[	BANKSLOT_TOTAL_SIZE ];
	__int64		m_i64ZULY;
	void Init ()
	{
		::ZeroMemory( m_ItemLIST,	sizeof(tagITEM)*BANKSLOT_TOTAL_SIZE );
	}

	short Get_EmptySlot (short nStartSlotNO);
	short Add_ITEM(tagITEM &sITEM);
	short Add_ITEM(short nSlotNO, tagITEM &sITEM);
	short Sub_ITEM(short nSlotNO, tagITEM &sITEM);
	void  Set_ITEM(short nSlotNO, tagITEM &sITEM)
	{
		m_ItemLIST[ nSlotNO ] = sITEM;
	}
} ;


#define	MAX_WISH_ITEMS		30				// The maximum number of items you wish to buy steamed
struct tagWishLIST {
	tagITEM	m_WishITEM[ MAX_WISH_ITEMS ];

	void Init ()
	{
		::ZeroMemory( m_WishITEM,	sizeof(tagITEM)*MAX_WISH_ITEMS );
	}
	bool Set(BYTE btSlotNO, tagITEM &sITEM)
	{
		if ( btSlotNO >= MAX_WISH_ITEMS )
			return false;

		this->m_WishITEM[ btSlotNO ] = sITEM;
		return true;
	}
} ;

//-------------------------------------------------------------------------------------------------
#pragma pack (pop)


/// Data combat abilities - Get been calculated
struct tagBattleAbility {
	short	m_nMaxHP;
	short	m_nMaxMP;
	short	m_nATT;			// ATK 1~200
	short	m_nDEF;			// DEF 1~200
	short	m_nMAG;			// Depends, 1~200
	short	m_nHIT;			// Myeongjungryeok 1~500
	short	m_nRES;			// Anti-horsepower 1~200
	short	m_nAVOID;		// Hoepiryeok 1~999
	short	m_nSPD;			// Movement 1~50
	short	m_nMaxWEIGHT;	// weight Carrying amount of the item.
	short	m_nWEIGHT;		// Hanryang currently hold

//	int		m_iDefQuality;	// Full Armor Quality
	int		m_iDefDurabity;	// Full Armor Durability sum
	int		m_iDefGrade;	// The total sum armor rating

	int		m_iCritical;	// Critical levels
	float	m_fRateUseMP;	// MP MP consumption rate is calculated as the savings rate been.


#ifdef _GBC
	short	m_nImmunity;	// Immunity
#endif
	

} ;


//-------------------------------------------------------------------------------------------------

/// Base user data
class CUserDATA {
protected:
	inline int	GetPassiveSkillValue(short nAbilityTYPE)
	{
		return this->m_iAddValue[ nAbilityTYPE ];
	}
	inline void AddPassiveSkillValue(short nAbilityTYPE, short nValue)
	{
		this->m_iAddValue[ nAbilityTYPE ] += nValue;
	}
	inline short GetPassiveSkillRate(short nAbilityTYPE)
	{
		return this->m_nPassiveRate[ nAbilityTYPE ];
	}
	inline void AddPassiveSkillRate(short nAbilityTYPE, short nRate)
	{
		this->m_nPassiveRate[ nAbilityTYPE ] += nRate;
	}

	short	GetPassiveSkillAttackPower( int iCurAP, short nRightWeaponItemNo );
	short	GetPassiveSkillAttackSpeed( float fCurSpeed, short nRightWeaponItemNo );
	short	GetPassiveSkillAttackSpeed( short nRightWeaponItemNo );

public:
	//PY Stuff for compatibility
	//int MaxHP;
	//int HP;
	
	
	void	Cal_AddAbility (tagITEM &sITEM, short nItemTYPE);
	void	Cal_AddAbility ();

	void	Cal_RecoverHP ();
	void	Cal_RecoverMP ();

	int		Cal_MaxHP ();
	int		Cal_MaxMP ();

	int		Cal_HIT ();
	int		Cal_DEFENCE();

	int		Cal_RESIST ();
	int		Cal_MaxWEIGHT ();
	int		Cal_AvoidRATE ();
	int		Cal_CRITICAL ();

	float	Cal_RunSPEED ();
	void	Cal_BattleAbility ();
	int		Cal_ATTACK ();
	float	Cal_NEWKRAttack(int AttckPWR, unsigned short igrade);

	short GetCur_HP ()				{	return	this->m_GrowAbility.m_nHP;			}	// Vitality
	short GetCur_MP ()				{	return	this->m_GrowAbility.m_nMP;			}
	virtual short GetCur_WEIGHT ()			{	return	this->m_Battle.m_nWEIGHT;			}

	int	  GetCur_LEVEL ()			{	return	this->m_GrowAbility.m_nLevel;		}

	int	  GetDef_ATK ()				{	return	this->m_Battle.m_nATT;				}	// ATK
	int	  GetDef_DEF ()				{	return	this->m_Battle.m_nDEF;				}	// DEF
	int	  GetDef_RES ()				{	return	this->m_Battle.m_nRES;				}	// Anti-horsepower
	int	  GetDef_HIT ()				{	return	this->m_Battle.m_nHIT;				}	// Accuracy
	int	  GetDef_AVOID ()			{	return	this->m_Battle.m_nAVOID;			}	// Dodge
	int	  GetDef_CRITICAL()			{	return	this->m_Battle.m_iCritical;			}


	int	  GetDef_IMMUNITY();
	void  SetDef_IMMUNITY(int iImmunity);


	int	  GetCur_BIRTH()			{	return  this->m_BasicINFO.m_cBirthStone;	}	// Birth Stone
	int	  GetCur_RANK()				{	return	this->m_BasicINFO.m_cRank;			}	// ���
	int	  GetCur_UNION ()			{	return	this->m_BasicINFO.m_cUnion;			}	// �Ҽ�
	int	  GetCur_FAME ()			{	return	this->m_BasicINFO.m_cFame;			}	// ��
	int	  GetCur_JOB ()				{	return	this->m_BasicINFO.m_nClass;			}	// ����

	int	  GetCur_EXP ()				{	return	this->m_GrowAbility.m_lEXP;			}
	int	  GetCur_BonusPOINT ()		{	return	this->m_GrowAbility.m_nBonusPoint;	}
	int	  GetCur_SkillPOINT ()		{	return	this->m_GrowAbility.m_nSkillPoint;	}
	BYTE  GetCur_HeadSIZE()			{	return	this->m_GrowAbility.m_btHeadSIZE;	}
	BYTE  GetCur_BodySIZE()			{	return	this->m_GrowAbility.m_btBodySIZE;	}
	void  SetCur_HeadSIZE(short nValue) { this->m_GrowAbility.m_btHeadSIZE = nValue; }
	void  SetCur_BodySIZE(short nValue) { this->m_GrowAbility.m_btBodySIZE = nValue; }
	
	short	GetCur_PatHP();
	void	SetCur_PatHP( short nPatHP );
	DWORD	GetCur_PatCoolTIME();
	void	SetCur_PatCoolTIME( DWORD dwCoolTIME );


	/// �⺻ �ɷ�ġ...
	int	  GetDef_STR ()				{	return	this->m_BasicAbility.m_nSTR;		}	// �ٷ�
	int	  GetDef_DEX ()				{	return	this->m_BasicAbility.m_nDEX;		}	// ��ø
	int	  GetDef_INT ()				{	return	this->m_BasicAbility.m_nINT;		}	// ����
	int	  GetDef_CON ()				{	return	this->m_BasicAbility.m_nCON;		}	// ����
	int	  GetDef_CHARM ()			{	return	this->m_BasicAbility.m_nCHARM;		}	// �ŷ�
	int	  GetDef_SENSE ()			{	return	this->m_BasicAbility.m_nSENSE;		}	// ����

	void  SetDef_STR(short nValue)	{	this->m_BasicAbility.m_nSTR=nValue;			}
	void  SetDef_DEX(short nValue)	{	this->m_BasicAbility.m_nDEX=nValue;			}
	void  SetDef_INT(short nValue)	{	this->m_BasicAbility.m_nINT=nValue;			}
	void  SetDef_CON(short nValue)	{	this->m_BasicAbility.m_nCON=nValue;			}
	void  SetDef_CHARM(short nValue){	this->m_BasicAbility.m_nCHARM=nValue;		}
	void  SetDef_SENSE(short nValue){	this->m_BasicAbility.m_nSENSE=nValue;		}

	int	  GetCur_STR ()				{	return	( GetDef_STR()   + m_iAddValue[ AT_STR		] + m_PassiveAbilityFromRate[ AT_STR  -AT_STR ] );	}	// �ٷ�
	int	  GetCur_DEX ()				{	return	( GetDef_DEX()   + m_iAddValue[ AT_DEX		] + m_PassiveAbilityFromRate[ AT_DEX  -AT_STR ] );	}	// ��ø
	int	  GetCur_INT ()				{	return	( GetDef_INT()   + m_iAddValue[ AT_INT		] + m_PassiveAbilityFromRate[ AT_INT  -AT_STR ] );	}	// ����
	int	  GetCur_CON ()				{	return	( GetDef_CON()   + m_iAddValue[ AT_CON		] + m_PassiveAbilityFromRate[ AT_CON  -AT_STR ] );	}	// ����
	int	  GetCur_CHARM ()			{	return	( GetDef_CHARM() + m_iAddValue[ AT_CHARM	] + m_PassiveAbilityFromRate[ AT_CHARM-AT_STR ] );	}	// �ŷ�
	int	  GetCur_SENSE ()			{	return	( GetDef_SENSE() + m_iAddValue[ AT_SENSE	] + m_PassiveAbilityFromRate[ AT_SENSE-AT_STR ] );	}	// ����

	int	  GetCur_SaveMP ();	// MP Savings ratio
	
	//int	  GetCur_DropRATE()			{	return  ( GetPassiveSkillValue( AT_PSV_DROP_RATE ) + m_iAddValue[ AT_DROP_RATE ] );	}	// ��� Ȯ��
	int	  GetCur_DropRATE()			{	return  m_iDropRATE;	}	// ��� Ȯ��
	void  Cal_DropRATE ()	// ��� Ȯ��		
	{
		m_iDropRATE = GetPassiveSkillValue( AT_PSV_DROP_RATE ) + m_iAddValue[ AT_DROP_RATE ] + (short)(m_iAddValue[ AT_DROP_RATE ]*GetPassiveSkillRate( AT_PSV_DROP_RATE )/100.f);	
	}
	

	int	  GetCur_UnionPOINT( WORD wAbilityTYPE )				{	return this->m_GrowAbility.m_nUnionPOINT[ wAbilityTYPE-AT_UNION_POINT1 ];	}
	void  SetCur_UnionPOINT( WORD wAbilityTYPE, short nValue )	{	this->m_GrowAbility.m_nUnionPOINT[ wAbilityTYPE-AT_UNION_POINT1 ] = nValue;	}
	void  AddCur_UnionPOINT( WORD wAbilityTYPE, short nValue )	{	this->m_GrowAbility.m_nUnionPOINT[ wAbilityTYPE-AT_UNION_POINT1 ]+= nValue;	}

	int	  GetCur_FameB ()			{	return	this->m_GrowAbility.m_nFameB;		}
	int	  GetCur_FameG ()			{	return	this->m_GrowAbility.m_nFameG;		}

	int	  GetCur_GuildNO()			{	return	this->m_GrowAbility.m_iGuildNO;		}
	int	  GetCur_GuildPOS()			{	return	this->m_GrowAbility.m_nGuildCNTRB;	}
	int	  GetCur_GuildCONTRIBUTION(){	return	this->m_GrowAbility.m_btGuildPOS;	}


	short GetDef_MaxHP ()			{	return	this->m_Battle.m_nMaxHP;			}
	short GetDef_MaxMP ()			{	return	this->m_Battle.m_nMaxMP;			}

	short GetCur_MaxWEIGHT ()		{	return	this->m_Battle.m_nMaxWEIGHT;		}
	int	  GetCur_MAG ()				{	return	this->m_Battle.m_nMAG;				}	// ������

//	int	  GetTot_DEF_QUALITY()		{	return	this->m_Battle.m_iDefQuality;		}
	int	  GetTot_DEF_DURABITY()		{	return	this->m_Battle.m_iDefDurabity;		}
	int	  GetTot_DEF_GRADE()		{	return	this->m_Battle.m_iDefGrade;			}

	float GetCur_RateUseMP()		{	return	this->m_Battle.m_fRateUseMP;		}

	void	SetCur_MONEY (__int64 iMoney)	{	this->m_Inventory.m_i64Money=iMoney;	}
	__int64 GetCur_MONEY ()					{	return	this->m_Inventory.m_i64Money;	}	// ��
	void	Add_CurMONEY (int iMoney)		{	this->m_Inventory.m_i64Money+=iMoney;	}
	void	Sub_CurMONEY (int iMoney)		{	this->m_Inventory.m_i64Money-=iMoney;	}
	void  SetCur_HP (short nValue);// �����
	void  SetCur_MP (short nValue);// ����
	void  SetCur_MaxHP (short nValue);

	int   AddCur_HP (short nValue)	
	{	
		m_GrowAbility.m_nHP+=nValue; 
		if ( m_GrowAbility.m_nHP > GetCur_MaxHP() )
			m_GrowAbility.m_nHP = GetCur_MaxHP();
		return m_GrowAbility.m_nHP;
	}
	int	  AddCur_MP (short nValue)	
	{	
		m_GrowAbility.m_nMP+=nValue;
		if ( m_GrowAbility.m_nMP > GetCur_MaxMP() )
			m_GrowAbility.m_nMP = GetCur_MaxMP();
		return m_GrowAbility.m_nMP;
	}
	int   SubCur_HP (short nValue)
	{	
		m_GrowAbility.m_nHP-=nValue;
		if ( m_GrowAbility.m_nHP < 0 )
			m_GrowAbility.m_nHP = 0;
		return m_GrowAbility.m_nHP;
	}
	int	  SubCur_MP (short nValue)
	{	
		m_GrowAbility.m_nMP-=nValue;
		if ( m_GrowAbility.m_nMP < 0 )
			m_GrowAbility.m_nMP = 0;
		return m_GrowAbility.m_nMP;
	}

#ifndef _SERVER
	int   AddCur_STAMINA (short nValue)	
	{	
		m_GrowAbility.m_nSTAMINA += nValue;		
		if ( m_GrowAbility.m_nSTAMINA > GetCur_MaxSTAMINA() )
			m_GrowAbility.m_nSTAMINA = GetCur_MaxSTAMINA();
		return m_GrowAbility.m_nSTAMINA;
	}
	int   SubCur_STAMINA (short nValue)
	{	
		m_GrowAbility.m_nSTAMINA-=nValue;
		if ( m_GrowAbility.m_nSTAMINA < 0 )
			m_GrowAbility.m_nSTAMINA = 0;
		return m_GrowAbility.m_nSTAMINA;
	}
#endif


	void  SetCur_FAME (char cValue)			{	this->m_BasicINFO.m_cFame=cValue;			}
	void  SetCur_UNION (char cValue)		{	this->m_BasicINFO.m_cUnion=cValue;			}
	void  SetCur_RANK (char cValue)			{	this->m_BasicINFO.m_cRank=cValue;			}

	virtual void  SetCur_JOB (short nValue)	{	this->m_BasicINFO.m_nClass=nValue;			}

	void  SetCur_LEVEL(WORD wValue)			{	this->m_GrowAbility.m_nLevel = ( wValue>MAX_LEVEL) ? MAX_LEVEL : wValue;	}
	void  SetCur_EXP (long lValue)			{	this->m_GrowAbility.m_lEXP=lValue;			}
	void  SetCur_BonusPOINT(short nValue)	{	this->m_GrowAbility.m_nBonusPoint=nValue;	}
	void  SetCur_SkillPOINT(short nValue)	{	this->m_GrowAbility.m_nSkillPoint=nValue;	}

	tagBasicINFO		m_BasicINFO;					// Basic Data ...	
	tagBasicAbility		m_BasicAbility;					// Basic stats data ...
	tagGrowAbility		m_GrowAbility;					// Growth stats data ...
	tagSkillAbility		m_Skills;						// Avatar reserves skills
	short				m_CommonSkill[MAX_COMMON_SKILL];
	
	tagQuestData		m_Quests;						// quest
	CInventory			m_Inventory;					// inventory
	CHotICONS			m_HotICONS;						// ��������� ��� ����...
	CMileageInv         m_MileageInv;                   //  <���ϸ��� �κ�....

	tagBankData			m_Bank;							// ���� ����Ÿ.
	tagWishLIST			m_WishLIST;						// ���� �������� ���� ���Խ� ����� ������� �����۸��.

	tagBattleAbility	m_Battle;						// ���� �ɷ�ġ ������ - ���Ǿ��� ����

	int					m_iAddValue   [ AT_MAX ];		// ���� �����ۿ� ���� ������ ��ġ : ���Ǿ���.
	short				m_nPassiveRate[ AT_MAX ];		// �нú� ��ų�� ���� ������ ����

	BYTE				m_btRecoverHP;					// == m_PassiveSkill[ PST_RECOVER_HP ] + this->m_iAddValue[ AT_RECOVER_HP ]
	BYTE				m_btRecoverMP;					// == m_PassiveSkill[ PST_RECOVER_MP ] + this->m_iAddValue[ AT_RECOVER_MP ]

	int					m_iDropRATE;					// ��� Ȯ��

	short				m_PassiveAbilityFromValue[ BA_MAX ];	// �⺻ �ɷ�ġ :: ��ġ �нú� ��ų�� ���� ������ ��
	short				m_PassiveAbilityFromRate [ BA_MAX ];	// �⺻ �ɷ�ġ :: ���� �нú� ��ų�� ���� ������ ��
	short				m_nPassiveAttackSpeed;					// �ѱ�/�Ϻ�/IRose�� ���� �нú� ��ų�� ���� ���� �߰�


	int					m_iAppliedPenaltyEXP;			/// ��Ȱ�� ����ġ ������ ���� ����..


	tagITEM	Get_EquipITEM (WORD wEquipIDX)							{	return m_Inventory.m_ItemEQUIP[ wEquipIDX ];			}
	tagITEM*Get_EquipItemPTR(WORD wEquipIDX)						{	return &m_Inventory.m_ItemEQUIP[ wEquipIDX ];			}

	tagITEM Get_InventoryITEM (WORD wTotalListIndex)				{	return m_Inventory.m_ItemLIST[ wTotalListIndex ];		}
	tagITEM Get_InventoryITEM (t_InvTYPE InvTYPE, WORD wPageIndex)	{	return m_Inventory.m_ItemPAGE[ InvTYPE ][ wPageIndex ];	}

#ifdef __SERVER
	short	Add_CatchITEM (short nListRealNO, tagITEM &sITEM);//		{	return m_Inventory.AppendITEM( nListRealNO, sITEM, m_Battle.m_nWEIGHT );	}
	short	Add_ITEM (tagITEM &sITEM)								{	return m_Inventory.AppendITEM( sITEM, m_Battle.m_nWEIGHT );					}
	short	Add_ITEM(short nListRealNO, tagITEM &sITEM);//				{	return m_Inventory.AppendITEM( nListRealNO, sITEM, m_Battle.m_nWEIGHT);		}
	void	Sub_ITEM(short nListRealNO, tagITEM &sITEM);//				{	m_Inventory.SubtractITEM( nListRealNO, sITEM, m_Battle.m_nWEIGHT);			}
	void    Set_ITEM(short nListRealNO, tagITEM& sITEM);
	void	ClearITEM(WORD wListRealNO)									
	{	
///		m_Battle.m_nWEIGHT -= m_Inventory.GetWEIGHT (wListRealNO);
		m_Inventory.DeleteITEM(wListRealNO); 
	}
#else
	virtual short	Add_CatchITEM (short nListRealNO, tagITEM &sITEM);//		{	return m_Inventory.AppendITEM( nListRealNO, sITEM, m_Battle.m_nWEIGHT );	}
	virtual short	Add_ITEM (tagITEM &sITEM)								{	return m_Inventory.AppendITEM( sITEM, m_Battle.m_nWEIGHT );					}
	virtual short	Add_ITEM(short nListRealNO, tagITEM &sITEM);//				{	return m_Inventory.AppendITEM( nListRealNO, sITEM, m_Battle.m_nWEIGHT);		}
	virtual void	Sub_ITEM(short nListRealNO, tagITEM &sITEM);//				{	m_Inventory.SubtractITEM( nListRealNO, sITEM, m_Battle.m_nWEIGHT);			}
	virtual void    Set_ITEM(short nListRealNO, tagITEM& sITEM);
	virtual void	ClearITEM(WORD wListRealNO)									
	{	
///		m_Battle.m_nWEIGHT -= m_Inventory.GetWEIGHT (wListRealNO);
		m_Inventory.DeleteITEM(wListRealNO); 
	}

#endif
	/// ���� ��ų�� ���� ������ ���� ������ �Լ�...
	virtual short GetCur_RACE() = 0;            /// ����,,,, gsv_SELECT_CHAR.m_btCharRACE �� �����ϸ��
	virtual short GetCur_ATK_SPD () = 0;
	virtual float GetCur_MOVE_SPEED () = 0;
	virtual int	  GetCur_ATK() = 0;
	virtual int	  GetCur_DEF() = 0;
	virtual int	  GetCur_RES() = 0;
	virtual int	  GetCur_HIT() = 0;
	virtual int	  GetCur_AVOID() = 0;
	virtual int	  GetCur_CRITICAL() = 0;

	virtual int	  GetCur_MaxHP () = 0;			/// ��ų�� ���� ������ ���� ������� �������� ������ �Լ�
	virtual int	  GetCur_MaxMP () = 0;	        /// ��ų�� ���� ������ ���� ������� �������� ������ �Լ�
#ifndef _SERVER
	virtual int	  GetCur_MaxSTAMINA () = 0;
#endif
	virtual void  UpdateCur_Ability() = 0;
	virtual BYTE  GetCur_MOVE_MODE ()			{	return 0;	}

	virtual void  SetCur_TeamNO(short /*nValue*/) = 0;
	void	SetCur_PK_FLAG (short nValue )		{	this->m_GrowAbility.m_nPKFlag;				}
	short	GetCur_PK_FLAG ()					{	return this->m_GrowAbility.m_nPKFlag;		}

	virtual void  AddCur_EXP ( int /*iValue*/ )	{	/* nop */	}	// ȣ��ɰ�� �������� ���۵�.
	void	AddCur_BonusPOINT (short nValue)	{	this->m_GrowAbility.m_nBonusPoint+=nValue;	}
	void	AddCur_SkillPOINT (short nValue)	{	this->m_GrowAbility.m_nSkillPoint+=nValue;	}

	void	SubCur_BounsPOINT (short nValue)	{	this->m_GrowAbility.m_nBonusPoint-=nValue;	}
	void	SubCur_SkillPOINT (short nValue)	{	this->m_GrowAbility.m_nSkillPoint-=nValue;	}
		
	bool	Set_AbilityValue (WORD nType, int iValue);
	void	Add_AbilityValue (WORD nType, int iValue);

	virtual void	ChangeModelInfo( short nType, int iValue ) = 0;


	
#ifdef	__SERVER
	virtual int GetCur_AbilityValue (WORD nType) = 0;
#else
	int		GetCur_AbilityValue (WORD nType)						{	return Get_AbilityValue(nType);		}
	int		Get_AbilityValue (WORD nType);
	
	void	RefreshLookUpTable()									{   m_Inventory.MakeItemIndexList();	}
	virtual void  UpdateModelData() = 0{ *(int*)0 = 10; }		/// �� ������ ����..
#endif

	bool	Use_ITEM (WORD wUseItemNO);
	bool	Set_EquipITEM (short nEquipIDX, tagITEM &sITEM);
	bool	Check_PatEquipCondition(tagITEM &sITEM );
	bool	Check_EquipCondition (tagITEM &sITEM);
	bool	Check_JobCollection( short nClassStbIDX );
	
	virtual void  SetCur_PartITEM (short nPartIdx, tagITEM &sItem) = 0;
	virtual short GetCur_PartITEM(short nPartIdx) = 0;

public :
	int	Get_NeedEXP (int iLevel)
	{
		return ( m_GrowAbility.m_lPenalEXP + CCal::Get_NeedRawEXP(iLevel) );
	}

	/// ����� �г�Ƽ ����ġ
	void Set_PenalEXP (BYTE btAddPercent)
	{
		if ( this->GetCur_LEVEL() >= 10 ) 
		{
			// 10�� �̻��̸� ���Ƽ ����..
			int iNeedEXP  = CCal::Get_NeedRawEXP( m_GrowAbility.m_nLevel );
			int iPenalEXP = (int) ( iNeedEXP * btAddPercent / 100.f );
			m_iAppliedPenaltyEXP = iPenalEXP;

			if ( m_GrowAbility.m_lEXP >= iPenalEXP ) 
			{
				m_GrowAbility.m_lEXP -= iPenalEXP;
			} else 
			{
				iPenalEXP -= m_GrowAbility.m_lEXP;
				m_GrowAbility.m_lPenalEXP += iPenalEXP;
				m_GrowAbility.m_lEXP = 0;

				if ( m_GrowAbility.m_lPenalEXP > DIV02(iNeedEXP) ) 
				{
					m_iAppliedPenaltyEXP -= ( DIV02(iNeedEXP) - m_GrowAbility.m_lPenalEXP );
					m_GrowAbility.m_lPenalEXP = DIV02(iNeedEXP);
				}
			}
		}

	}

	/// ��Ȱ�� ������ų ����ġ
	void Cancel_PenalEXP (BYTE btPercent)
 	{
 		if ( this->GetCur_LEVEL() >= 10 && m_iAppliedPenaltyEXP > 0 ) 
		{
 			// 10�� �̻��̸� �����ߴ� ���Ƽ ����...
 			m_iAppliedPenaltyEXP = (int)( m_iAppliedPenaltyEXP * btPercent / 100.f );
 			if ( m_iAppliedPenaltyEXP ) {
 				m_GrowAbility.m_lEXP += m_iAppliedPenaltyEXP;
 			}
 		}
 		m_iAppliedPenaltyEXP = 0;
 	}


	short Get_NeedPoint2AbilityUP (short BasicAbilityType)
	{
		_ASSERT( BasicAbilityType >= 0 && BasicAbilityType < 6 );

		return (short)( m_BasicAbility.m_nBasicA[ BasicAbilityType ] * 0.2 );
	}

	// skill stb�� �÷����� ����ϴ°�����...
	short Get_NeedPoint2SkillUP (short nSkillSLOT);

	//  -------------------------------------------------------------------------------------------
	/// ��ų ���� ���� �Լ�...
	virtual int		GetCur_R_WEAPON () = 0;
	virtual int		GetCur_L_WEAPON () = 0;
	virtual DWORD	GetCur_IngStatusFLAG() = 0;

	/// ��ų�� �� ���� ã��
	short Skill_FindEmptySlot( short nSkillIDX );

	/// ������ ��ų�� ������ ��´�.
	short Skill_FindLearnedLevel( short nSkillIDX );

	/// ������ ��ų�� ������ ã�´�.
	short Skill_FindLearnedSlot ( short nSkillIDX );

	/// �ɷ�ġ ��� �Լ��� ������ ��ų���� ����ϴ� �ɷ�ġ�� �Ҽ��� �Ұ� ������
	/// �ӵ��� ���̱� ���ؼ� ��ų�� ����ϴ� �ɷ�ġ�� ������ �Լ�...
	int   Skill_GetAbilityValue( short nAbilityType );

	/// ��ų���� �Ҹ�Ǵ� ��ġ ����...
	int   Skill_ToUseAbilityVALUE(short nSKillIDX, short nPropertyIDX );
	bool  Skill_UseAbilityValue( short nSkillIDX );


	bool Skill_CheckExclusive( short nSkillIDX );
	bool Skill_CheckJOB( short nSkillIDX );				/// ����, ������� ���� üũ 
	bool Skill_CheckLearnedSKILL( short nSkillIDX );	/// ����, ������� �̹� ������ ��ų üũ 
	bool Skill_CheckNeedABILITY( short nSkillIDX );		/// ����, ������� ����� �ɷ�ġ üũ..

	/// ��ų ó�� ���� ���� üũ �Լ�
	BYTE  Skill_LearnCondition( short nSkillIDX );		

	/// ��ų ������ ���� üũ �Լ�
	BYTE  Skill_LevelUpCondition( short nCurLevelSkillIDX, short nNextLevelSkillIDX );

	/// ��ų ��� ���� üũ �Լ�
	bool  Skill_ActionCondition( short nSkillIDX );

	/// ��ų ���� :: �����̳� �������� ȣ���ؾ� �нú� ��ų �����..
	BYTE  Skill_LEARN( short nSkillSLOT, short nSkillIDX, bool bSubPOINT=true );

	/// ������ ��ų�� �����Ѵ�.
	bool  Skill_DELETE( short nSkillSLOT, short nSkillIDX );

	/// �нú� ��ų ���� �Լ�.
	void  InitPassiveSkill ();				/// ������ ���� ��ų ����Ÿ ������ �ѹ� ȣ���ؾ���...
	short GetPassiveSkillAttackSpeed()		{	return m_nPassiveAttackSpeed + m_iAddValue[ AT_ATK_SPD ];	}
	//short GetPassiveSkillAttackSpeed();


	short GetBuySkillVALUE()				
	{
#ifdef __KRRE_NEW_AVATAR_DB
		return GetPassiveSkillRate( AT_PSV_BUYING  );	
#endif
		return GetPassiveSkillValue( AT_PSV_BUYING  );	
	}

	short GetSellSkillVALUE()				
	{
#ifdef __KRRE_NEW_AVATAR_DB
		return GetPassiveSkillRate( AT_PSV_SELLING  );	
#endif
		return GetPassiveSkillValue( AT_PSV_SELLING );	
	}


	//  -------------------------------------------------------------------------------------------
	/// ����Ʈ ���� ���� �Լ�...
#ifdef	__SERVER
	virtual bool Quest_IsPartyLEADER() = 0;						    /// ��Ƽ ¯�̳� ?
	virtual int	 Quest_GetPartyLEVEL() = 0;						    /// ���� ��Ƽ ����
	virtual int	 Quest_GetZoneNO() = 0;							    /// ���� �� ��ȣ
	virtual int	 Quest_DistanceFrom(int iX, int iY, int iZ) = 0;	/// ������ġ���� �Ÿ�
	virtual int	 Quest_PartyMemberCNT() = 0;                        /// ��Ƽ �ο���
#else
	virtual bool Quest_IsPartyLEADER() = 0							{	*(int*)0 = 10; return false;		}	/// ��Ƽ ¯�̳� ?
	virtual int	 Quest_GetPartyLEVEL() = 0							{	*(int*)0 = 10; return -1;			}	/// ���� ��Ƽ ����
	virtual int	 Quest_GetZoneNO() = 0								{	*(int*)0 = 10; return 0;			}	/// ���� �� ��ȣ
	virtual int	 Quest_DistanceFrom(int iX, int iY, int iZ) = 0		{	*(int*)0 = 10; return 999999999;	}	/// ������ġ���� �Ÿ�
	virtual int	 Quest_PartyMemberCNT() = 0							{	*(int*)0 = 10; return 0;			}	/// ��Ƽ �ο���
#endif

	BYTE    Quest_GetRegistered( int iQuestIDX );
	short	Quest_Append( int iQuestIDX );
	short	Quest_Append_9( int iQuestIDX );						// 2006-08-31  ���� 5:21:23 - �迵ȯ
	bool	Quest_Append( BYTE btSlot, int iQuestIDX );
	bool	Quest_Delete( int iQuestIDX );
	bool	Quest_Delete( BYTE btSLOT, int iQuestID );
	tagITEM*Quest_FindEquipITEM(WORD wEquipIDX)		{	return ( wEquipIDX < MAX_EQUIP_IDX ) ? &m_Inventory.m_ItemEQUIP[ wEquipIDX ] : NULL;		}
	tagITEM*Quest_FindITEM( tagITEM &sFindITEM )	
	{
		short nFindSLOT = m_Inventory.FindITEM( sFindITEM );
		return ( nFindSLOT >= 0 ) ? &m_Inventory.m_ItemLIST[ nFindSLOT ] : NULL;
	}
	bool	Quest_SubITEM( tagITEM &sSubITEM );

	short Quest_GetEpisodeVAR( WORD wVarIDX )		{	return ( wVarIDX < QUEST_EPISODE_VAR_CNT	) ? m_Quests.m_nEpisodeVAR	[ wVarIDX ] : 0;	}
	short Quest_GetJobVAR( WORD wVarIDX )			{	return ( wVarIDX < QUEST_JOB_VAR_CNT		) ? m_Quests.m_nJobVAR		[ wVarIDX ] : 0;	}
	short Quest_GetUnionVAR( WORD wVarIDX )			{	return ( wVarIDX < QUEST_UNION_VAR_CNT		) ? m_Quests.m_nUnionVAR	[ wVarIDX ] : 0;	}
	short Quest_GetPlanetVAR( WORD wVarIDX )		
	{	
		if ( wVarIDX < QUEST_PLANET_VAR_CNT	) 
			return m_Quests.m_nPlanetVAR[ wVarIDX ];
#ifdef	__APPLY_EXTAND_QUEST_VAR
		else
		if ( wVarIDX < QUEST_PLANET_VAR_CNT+QUEST_ETC_VAR_CNT ) {
			return m_Quests.m_nEtcVAR[ wVarIDX-QUEST_PLANET_VAR_CNT ];
		}
#endif
		return 0;
	}
#ifdef __CLAN_WAR_SET	
	//	Ŭ���� ����Ʈ ���� ���
	//	������ ���� ���� ��´�.
//	short Quest_GetClanWarVAR( WORD wVarIDX )		{	return ( wVarIDX < QUEST_CLANWAR_VAR_CNT	) ? m_Quests.m_nClanWarVAR	[ wVarIDX ] : 0;	}
	short Quest_GetClanWarVAR( WORD wVarIDX )
	{
		short i = 0;
		if ( wVarIDX < QUEST_CLANWAR_VAR_CNT	)
			i = m_Quests.m_nClanWarVAR	[ wVarIDX ];
		return i;
	}
#endif

	void Quest_SetEpisodeVAR( WORD wI, short nV )	{	if ( wI < QUEST_EPISODE_VAR_CNT )	m_Quests.m_nEpisodeVAR[ wI ]	= nV;	}
	void Quest_SetJobVAR	( WORD wI, short nV )	{	if ( wI < QUEST_JOB_VAR_CNT		)	m_Quests.m_nJobVAR[ wI ]		= nV;	}
	void Quest_SetUnionVAR	( WORD wI, short nV )	{	if ( wI < QUEST_UNION_VAR_CNT	)	m_Quests.m_nUnionVAR[ wI ]		= nV;	}
	void Quest_SetPlanetVAR	( WORD wI, short nV )	
	{	
		if ( wI < QUEST_PLANET_VAR_CNT ) 
			m_Quests.m_nPlanetVAR[ wI ] = nV;
#ifdef	__APPLY_EXTAND_QUEST_VAR
		else
		if ( wI < QUEST_PLANET_VAR_CNT+QUEST_ETC_VAR_CNT ) {
			m_Quests.m_nEtcVAR[ wI-QUEST_PLANET_VAR_CNT ] = nV;
		}
#endif
	}
#ifdef __CLAN_WAR_SET	
	//	Ŭ���� ����Ʈ ���� ����
	//	������ ���� ���� �����Ѵ�.
//	void Quest_SetClanWarVAR( WORD wI, short nV )	{	if ( wI < QUEST_CLANWAR_VAR_CNT	)	m_Quests.m_nClanWarVAR[ wI ]	= nV;	}
	void Quest_SetClanWarVAR( WORD wI, short nV )
	{
		if ( wI < QUEST_CLANWAR_VAR_CNT	)
			m_Quests.m_nClanWarVAR[ wI ] = nV;
	}
#endif

	// ����Ʈ ���� ���� �Լ�
#ifdef	__SERVER
	virtual bool Reward_WARP( int iZoneNO, tPOINTF &PosGOTO) = 0;
#endif

	virtual bool Quest_CHANGE_SPEED()							{	return true;	}

	virtual bool Add_MoneyNSend( int iAmount, WORD wType )		{	return true;	}
	virtual bool Add_ExpNSend( int iExp )						{	return true;	}
	virtual bool Add_ItemNSend( tagITEM &sITEM )				{	return true;	}
	virtual bool Add_SkillNSend( short nSkillIDX )				{	return true;	}
	virtual bool Sub_SkillNSend( short nSkillIDX )				{	return true;	}
	virtual bool Set_AbilityValueNSend( WORD nType, int iValue ){	return true;	}
	virtual void Add_AbilityValueNSend( WORD wType, int iValue ){	/* nop */		}

	bool Reward_InitSTATUS (void);

	bool Reward_InitSKILL ();
	bool Reward_InitSKILLType ( int iInitType = -1, int iSkillSlot = -1 );
	int	Set_InitSKILLType ( int iBeginSkillIndex, int iSize );

	bool Reward_ITEM	( tagITEM &sITEM, BYTE btRewardToParty, BYTE btQuestSLOT );
	bool Reward_ABILITY	( bool bSetOrAdj, int iType, int iValue, BYTE btRewardToParty );
	bool Reward_CalEXP	( BYTE btEquation, int iBaseValue, BYTE btRewardToParty );
	bool Reward_CalMONEY( BYTE btEquation, int iBaseValue, BYTE btRewardToParty, short nDupCNT );
	bool Reward_CalITEM	( BYTE btEquation, int iBaseValue, BYTE btRewardToParty, int iItemSN, short nItemOP, BYTE btQuestSLOT );

public :
	DWORD	m_dwCoolItemStartTime[ MAX_USEITEM_COOLTIME_TYPE ];
	DWORD	m_dwCoolItemEndTime	 [ MAX_USEITEM_COOLTIME_TYPE ];
} ;


//-------------------------------------------------------------------------------------------------
#pragma warning (default:4201)
#endif
