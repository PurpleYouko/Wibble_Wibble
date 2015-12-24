#ifndef	__IO_SKILL_H
#define	__IO_SKILL_H

#include "IO_STB.h"
#include <vector>
#ifndef __SERVER
	#include "../GameCommon/StringManager.h"
#endif
//-------------------------------------------------------------------------------------------------

enum {
	SKILL_TYPE_01 = 1,						//1.The default behavior
	SKILL_TYPE_02 ,							//2.Manufacturing skill
	SKILL_TYPE_03 ,							//3.Damage in action
	SKILL_TYPE_04 ,							//4.Weapon power-up
	SKILL_TYPE_05 ,							//5.Weapon power-up
	SKILL_TYPE_06 ,							//6.Magic launch
	SKILL_TYPE_07 ,							//7.Local magic damage
	SKILL_TYPE_08 ,							//8.State persistence (self)
	SKILL_TYPE_09 ,							//9.State persistence (target)
	SKILL_TYPE_10 ,							//10.Restoration magic (self)
	SKILL_TYPE_11 ,							//11.Restoration magic (target)
	SKILL_TYPE_12 ,							//12.State persistence (change) (self)
	SKILL_TYPE_13 ,							//13.State persistence (change) (target)
	SKILL_TYPE_14 ,							//14.Summoning magic
	SKILL_TYPE_15 ,							// Passive skill
	SKILL_TYPE_PASSIVE = SKILL_TYPE_15,		
	SKILL_TYPE_16 ,							// Emotion
	SKILL_TYPE_17 ,							// Self & damage
	SKILL_TYPE_18 ,							/// Add 2004.4.8 : warp skill
	SKILL_TYPE_19,							/* Add 2004.6.16 :
											Change the strength value (10 times) and damage attacks (3 times)
													Existing types of damaging a target skill three times, giving a
													The specified strength value (21 times, 24 times columnist) levels (22, 25 times columnist) is increased.
													By applying the formula of success upon success skill and ability to apply the changes.
													(HP, MP, while the damage at the target attack took the concept to come)
											*/
	SKILL_TYPE_20,							//20.The resurrection
} ;

enum
{
	SKILL_TAB_COMMON			= 11,
	SKILL_TAB_JOB_COMMON		= 21,
	SKILL_TAB_JOB_TYPE1			= 22,
	SKILL_TAB_JOB_TYPE2			= 23,
	SKILL_TAB_JOB2				= 24,
	SKILL_TAB_PARTS_CART		= 31,
	SKILL_TAB_PARTS_CASTLEGEAR	= 32,
	SKILL_TAB_UNIQUE			= 41,
	SKILL_TAB_MILEAGE			= 51,
};

enum
{
	SKILL_CLEAR_JOB				= 20,
	SKILL_CLEAR_UNIQUE			= 40,
	SKILL_CLEAR_MILEAGE			= 50,
	SKILL_CLEAR_ALL				= 100,
};

enum
{
	SKILL_TYPE_COMMON			= 0X0000,
	
	SKILL_TYPE_JOB_COMMON		= 0X0001,
	SKILL_TYPE_JOB_TYPE1		= 0X0002,
	SKILL_TYPE_JOB_TYPE2		= 0X0004,
	SKILL_TYPE_JOB2				= 0X0008,
	SKILL_TYPE_JOB_ALL			= SKILL_TYPE_JOB_COMMON | SKILL_TYPE_JOB_TYPE1 | SKILL_TYPE_JOB_TYPE2 | SKILL_TYPE_JOB2,

	SKILL_TYPE_PARTS_CART		= 0X0010,
	SKILL_TYPE_PARTS_CASTLEGEAR = 0X0020,
	
	SKILL_TYPE_UNIQUE			= 0X0040,
	
	SKILL_TYPE_MILEAGE			= 0X0080,	
	
	SKILL_TYPE_ALL				= SKILL_TYPE_JOB_ALL | SKILL_TYPE_UNIQUE | SKILL_TYPE_MILEAGE,	
};


enum
{
	JOB_TYPE_SOLDIER	= 41,
	JOB_TYPE_MUSE		= 42,
	JOB_TYPE_HAWKER		= 43,
	JOB_TYPE_DEALER		= 44,

	JOB_TYPE_SOLDIER_1	= 61,
	JOB_TYPE_SOLDIER_2	= 62,

	JOB_TYPE_MUSE_1		= 63,
	JOB_TYPE_MUSE_2		= 64,

	JOB_TYPE_HAWKER_1	= 65,
	JOB_TYPE_HAWKER_2	= 66,

	JOB_TYPE_DEALER_1	= 67,
	JOB_TYPE_DEALER_2	= 68,

};

enum enumSKILL_TAGER_FILTER
{
	SKILL_TARGET_FILTER_SELF = 0,			///< Self
	SKILL_TARGET_FILTER_GROUP = 1,			///< Group (party) won
	SKILL_TARGET_FILTER_GUILD = 2,			///< Guild
	SKILL_TARGET_FILTER_FRIEND_ALL = 3,		///< Spiked (avatar, summoning AdMob)
	SKILL_TARGET_FILTER_MOB = 4,			///< All Monster (summoning AdMob, the enemy mob)
	SKILL_TARGET_FILTER_ENEMY_ALL = 5,		///< Ever PC + Monster 	
	SKILL_TARGET_FILTER_ENEMY_PC = 6,		///< Ever PC (status)
	SKILL_TARGET_FILTER_ALL_PC = 7,			///< All PC
	SKILL_TARGET_FILTER_ALL_CHAR = 8,		///< All characters (monsters, player)	
	SKILL_TARGET_FILTER_DEAD_USER = 9,		/// Dead users
	SKILL_TARGET_FILTER_ENEMY_MOB = 10,		/// Enemy monsters
	SKILL_TARGET_FILTER_MAX,
};

//-------------------------------------------------------------------------------------------------

#ifdef __SERVER
	#define	SKILL_NAME(I)						g_SkillList.m_SkillDATA.m_ppNAME[ I ]			// Skill name
	#define SKILL_DESC(I)						g_SkillList.m_SkillDATA.m_ppDESC[ I ]			// Skill description
#else
	#define	SKILL_NAME(I)						CStringManager::GetSingleton().GetSkillName( I )
	#define SKILL_DESC(I)						CStringManager::GetSingleton().GetSkillDesc( I )
#endif

#define	SKILL_1LEV_INDEX(S)					g_SkillList.m_SkillDATA.GetData( S ,  1 )		// First skill id of the block
#define	SKILL_LEVEL(S)						g_SkillList.m_SkillDATA.GetData( S ,  2 )		// Skill Level
#define	SKILL_NEED_LEVELUPPOINT(S)			g_SkillList.m_SkillDATA.GetData( S ,  3 )		// SP to level up
#define	SKILL_TAB_TYPE(S)					g_SkillList.m_SkillDATA.GetData( S ,  4 )		// Skill Tree Tab ID
#define SKILL_TYPE( I )						g_SkillList.m_SkillDATA.GetData( I ,  5 )		// Skill Type

#define SKILL_DISTANCE( I )					g_SkillList.m_SkillDATA.GetData( I ,  6 )		// Range
#define SKILL_WARP_PLANET_NO( I )			g_SkillList.m_SkillDATA.GetData( I ,  6 )		// Warp to Planet

#define SKILL_CLASS_FILTER( I )				g_SkillList.m_SkillDATA.GetData( I ,  7 )		// Skill target type
#define	SKILL_SCOPE(S)						g_SkillList.m_SkillDATA.GetData( S ,  8 )		// AOE range
#define	SKILL_POWER(S)						g_SkillList.m_SkillDATA.GetData( S ,  9 )		// Skill Power
#define	SKILL_ITEM_MAKE_NUM					SKILL_POWER										// Craft Skill Number
#define SKILL_HARM( I )						g_SkillList.m_SkillDATA.GetData( I , 10 )		

#define SKILL_SUCCESS_RATIO( I )			g_SkillList.m_SkillDATA.GetData( I , 13 )		// Success rate
#define SKILL_DURATION( I )					g_SkillList.m_SkillDATA.GetData( I , 14 )		// Skill Duration
#define	SKILL_DAMAGE_TYPE(S)				g_SkillList.m_SkillDATA.GetData( S , 15 )		// The type of the formula to be applied


#define	SKILL_USE_PROPERTY_CNT				2
#define	SKILL_USE_PROPERTY(S,T)				g_SkillList.m_SkillDATA.GetData( S , 16+(T)*2 )	// Consumption id. MP, HP, Stamina or whatever
#define	SKILL_USE_VALUE(S,T)				g_SkillList.m_SkillDATA.GetData( S , 17+(T)*2 )	// Amount consumed


#define	SKILL_RELOAD_TIME(S)				g_SkillList.m_SkillDATA.GetData( S , 20 )

// Passive (persistence) to elevate a period of time when the ability of skill ...
//#define	SKILL_INCREASE_ABILITY_CNT			2
//#define	SKILL_INCREASE_ABILITY(S,T)			g_SkillList.m_SkillDATA.GetData( S , 21+(T)*3 )
//#define	SKILL_INCREASE_ABILITY_VALUE(S,T)	g_SkillList.m_SkillDATA.GetData( S , 22+(T)*3 )
//#define	SKILL_CHANGE_ABILITY_RATE(S,T)		g_SkillList.m_SkillDATA.GetData( S , 23+(T)*3 )

#define	SKILL_WARP_ZONE_NO( S )				g_SkillList.m_SkillDATA.GetData( S , 21 )		// If you have the warp skill, a Warp zone number
#define	SKILL_WARP_ZONE_XPOS( S )			g_SkillList.m_SkillDATA.GetData( S , 22 )		// X position
#define	SKILL_WARP_ZONE_YPOS( S )			g_SkillList.m_SkillDATA.GetData( S , 23 )		// Y position



#define	MAX_SKILL_RELOAD_TYPE				20	// 0~19
#define	SKILL_RELOAD_TYPE(S)				g_SkillList.m_SkillDATA.GetData( S , 27 )		// Skill margin

#define	SKILL_SUMMON_PET(S)					g_SkillList.m_SkillDATA.GetData( S , 28 )		//	Summon mob number

#define	SKILL_ACTION_MODE(S)				g_SkillList.m_SkillDATA.GetData( S , 29 )		// Skill attack type
// The use of the equipment to be fitted.  T = 0 ~ 3
#define	SKILL_NEED_WEAPON_CNT				5		// <--- 2004.2. 26 4-increase and subsequent columns to 5 numbers + > 1 by icarus
#define	SKILL_NEED_WEAPON(S,T)				g_SkillList.m_SkillDATA.GetData( S , 30+(T) )	// required Equipment type

// LIST_CLASS.stb Reference number...
#define	SKILL_AVAILBLE_CLASS_SET(S)			g_SkillList.m_SkillDATA.GetData( S , 35 )		// required job	

// When you learn to check the first belonging to the Union...
#define	SKILL_AVAILBLE_UNION_CNT			3
#define	SKILL_AVAILBLE_UNION(S,T)			g_SkillList.m_SkillDATA.GetData( S , 36+(T) )	// Union skills??


// The first to be owned by advance when learning a skill. T = 0 ~ 2
#define	SKILL_NEED_SKILL_CNT				3
#define	SKILL_NEED_SKILL_INDEX(S,T)			g_SkillList.m_SkillDATA.GetData( S , 39+(T)*2 )	// Prerequisite skill
#define	SKILL_NEDD_SKILL_LEVEL(S,T)			g_SkillList.m_SkillDATA.GetData( S , 40+(T)*2 )	// Prerequisite skill level

// The first condition need to learn or level up ability. T = 0 ~ 2
#define	SKILL_NEED_ABILITY_TYPE_CNT			2
#define	SKILL_NEED_ABILITY_TYPE(S,T)		g_SkillList.m_SkillDATA.GetData( S , 45+(T)*2 )	// Required Ability. The value is used only for AT_DEX-AT_SENSE
#define	SKILL_NEED_ABILITY_VALUE(S,T)		g_SkillList.m_SkillDATA.GetData( S , 46+(T)*2 )	// Required Ability Level


#define SKILL_SCRIPT1( I )					g_SkillList.m_SkillDATA.GetData( I , 49 )		

#define SKILL_RESERVE_02( I )				g_SkillList.m_SkillDATA.GetData( I , 50 )		
#define SKILL_ICON_NO( I )					g_SkillList.m_SkillDATA.GetData( I , 51 )		

// Casting operation
#define	SKILL_ANI_CASTING(S)				g_SkillList.m_SkillDATA.GetData( S , 52 )
#define	SKILL_ANI_CASTING_SPEED(S)			g_SkillList.m_SkillDATA.GetData( S , 53 )

/// Cast loop action
#define	SKILL_ANI_CASTING_REPEAT(S)			g_SkillList.m_SkillDATA.GetData( S , 54 )
#define	SKILL_ANI_CASTING_REPEAT_CNT(S)		g_SkillList.m_SkillDATA.GetData( S , 55 )

/// Cast effect
#define SKILL_CASTING_EFFECT_CNT			4
#define SKILL_CASTING_EFFECT( I,T )			g_SkillList.m_SkillDATA.GetData( I , 56 + (T * 3) )
#define SKILL_CASTING_EFFECT_POINT( I,T )	g_SkillList.m_SkillDATA.GetData( I , 57 + (T * 3) )
#define SKILL_CASTING_SOUND( I,T )			g_SkillList.m_SkillDATA.GetData( I , 58 + (T * 3) )

// Actual behavior
#define	SKILL_ANI_ACTION_TYPE(S)			g_SkillList.m_SkillDATA.GetData( S , 68 )
#define	SKILL_ANI_ACTION_SPEED(S)			g_SkillList.m_SkillDATA.GetData( S , 69 )
#define	SKILL_ANI_HIT_COUNT(S)				g_SkillList.m_SkillDATA.GetData( S , 70 )

/// To launch a bullet effects
#define SKILL_BULLET_NO( I )				g_SkillList.m_SkillDATA.GetData( I , 71 )
#define SKILL_BULLET_LINKED_POINT( I )		g_SkillList.m_SkillDATA.GetData( I , 72 )
#define SKILL_BULLET_FIRE_SOUND( I )		g_SkillList.m_SkillDATA.GetData( I , 73 )

/// Blow effects
#define SKILL_HIT_EFFECT( I )				g_SkillList.m_SkillDATA.GetData( I , 74 )			//	Blow effects
#define SKILL_HIT_EFFECT_LINKED_POINT( I )	g_SkillList.m_SkillDATA.GetData( I , 75 )			//	Blow effects
#define SKILL_HIT_SOUND( I )				g_SkillList.m_SkillDATA.GetData( I , 76 )			//	Blow effects

/// Battered pile effect
#define SKILL_HIT_DUMMY_EFFECT_CNT			2
#define SKILL_HIT_DUMMY_EFFECT( I, T )					g_SkillList.m_SkillDATA.GetData( I , 77 + 3*T )			//	The effect of hitting a pile of
#define SKILL_HIT_DUMMY_EFFECT_LINKED_POINT( I, T )		g_SkillList.m_SkillDATA.GetData( I , 78 + 3*T )			//	The effect of hitting a pile of
#define SKILL_HIT_DUMMY_SOUND( I, T )					g_SkillList.m_SkillDATA.GetData( I , 79 + 3*T )			//	Hitting a pile of sound effects

#define SKILL_AREA_HIT_EFFECT( I )			g_SkillList.m_SkillDATA.GetData( I , 83 )								//	The local strike effects
#define SKILL_AREA_HIT_SOUND( I )			g_SkillList.m_SkillDATA.GetData( I , 84 )								//	Local blow effects

#define	SKILL_LEVELUP_NEED_ZULY( I )		g_SkillList.m_SkillDATA.GetData( I , 85 )				


#define	SKILL_ATTRIBUTE( I )				g_SkillList.m_SkillDATA.GetData( I , 86 )			///1 : Avatar 2: PAT skill, skill, 3: common skill



#define SKILL_INCREASE_ABILITY_CNT			3													// 88~99
#define SKILL_STATE_STB( I,C )				g_SkillList.m_SkillDATA.GetData( I , 88+(C)*4 )		// Status effect
#define SKILL_INCREASE_ABILITY( I,C )		g_SkillList.m_SkillDATA.GetData( I , 89+(C)*4 )		// The ability to change.
#define SKILL_INCREASE_ABILITY_VALUE( I,C )	g_SkillList.m_SkillDATA.GetData( I , 90+(C)*4 )		// Change the figures.
#define SKILL_CHANGE_ABILITY_RATE( I,C )	g_SkillList.m_SkillDATA.GetData( I , 91+(C)*4 )		// The rate of change.


#define SKILL_CANT_USE_OTHER_SKILL( I )		g_SkillList.m_SkillDATA.GetData( I , 100 )			// Blocks this skill from being learned

#define SKILL_CHILD_SKILL_CNT				5													// 101~110
#define SKILL_CHILD_SKILL_INDEX( I,C )		g_SkillList.m_SkillDATA.GetData( I , 101+(C)*2 )	// Unlocks this skill
#define SKILL_CHILD_SKILL_LEVEL( I,C )		g_SkillList.m_SkillDATA.GetData( I , 102+(C)*2 )	// Unlocks this skill level

																								

#define SKILL_SLOT_NUM( I )					g_SkillList.m_SkillDATA.GetData( I , 111 )			// Slot number
#define SKILL_NEED_SKILLBOOK( I )			g_SkillList.m_SkillDATA.GetData( I , 112 )			// Skill in the North


// 11, 12, 21~26 Column is empty. .. When there seemed to be further justified.

#define SKILL_ATTRIBUTE_AVATAR				1
#define SKILL_ATTRIBUTE_CART				2
#define SKILL_ATTRIBUTE_CASTLEGEAR			4


#define	SA_STOP						0	// After applying a skill stop
#define	SA_ATTACK					1	// After applying targeted attack skill
#define	SA_RESTORE					2	// After returning the previous command using a skill

#ifdef	__SERVER
	#define	SKILL_RELOAD_SECOND(S)			g_SkillList.m_pReloadTIME[ S ]
#endif

class CSkillLIST 
{
private:
	int			m_iSkillCount;	

	float	   *m_pCastingAniSPEED;		/// Calculated in advance skill casting speed
	float	   *m_pActionAniSPEED;		/// Calculate the actual speed is a skill.
	
	std::vector<int>	m_list_Common;			//Common skill list.	.
	std::vector<int>	m_list_Job_Skill[4][4];	//Rank 1 skill list.
											//The soldier, Muse, hawker, the dealer.
											//Soldier: Basic, with one hand, two hands, the second ex-skill	
	std::vector<int>	m_list_Cart;
	std::vector<int>	m_list_Castlegear;
	std::vector<int>	m_list_Unique;

	int			m_iCartSkillIndex;
	int			m_iCastlegearSkillIndex;

public :
	STBDATA		m_SkillDATA;
#ifdef	__SERVER
	DWORD	  *m_pReloadTIME;
#else
	float	  *m_pReloadTIME;
#endif

	CSkillLIST ();
	~CSkillLIST ();
	void	Free ();

	int		Get_SkillCNT()							{	return m_iSkillCount;					}
	float	Get_CastingAniSPEED(short nSkillIDX)	{	return m_pCastingAniSPEED[ nSkillIDX ];	}
	float	Get_ActionAniSPEED(short nSkillIDX)		{	return m_pActionAniSPEED[ nSkillIDX ];	}

	bool	LoadSkillTable( const char* pFileName );
	float  GetDelayTickCount(DWORD i);

	std::vector<int> & Get_List_Common();
	std::vector<int> & Get_List_Job_Skill(int i, int y);
	std::vector<int> & Get_List_Cart();
	std::vector<int> & Get_List_Castlegear();
	std::vector<int> & Get_List_Unique();

	int		Get_Cart_SkillIndex();
	int		Get_Castlegear_SkillIndex();
} ;

extern CSkillLIST g_SkillList;

//-------------------------------------------------------------------------------------------------
#endif
