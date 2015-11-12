#ifndef	__IO_SKILL_H
#define	__IO_SKILL_H

#include "IO_STB.h"
#include <vector>
#ifndef __SERVER
	#include "../GameCommon/StringManager.h"
#endif
//-------------------------------------------------------------------------------------------------

enum {
	SKILL_TYPE_01 = 1,						//1.기본동작
	SKILL_TYPE_02 ,							//2.제조스킬
	SKILL_TYPE_03 ,							//3.데미지액션
	SKILL_TYPE_04 ,							//4.무기파워업
	SKILL_TYPE_05 ,							//5.무기파워업
	SKILL_TYPE_06 ,							//6.마법발사
	SKILL_TYPE_07 ,							//7.지역마법데미지
	SKILL_TYPE_08 ,							//8.상태지속(셀프)
	SKILL_TYPE_09 ,							//9.상태지속(타겟)
	SKILL_TYPE_10 ,							//10.회복마법(셀프)
	SKILL_TYPE_11 ,							//11.회복마법(타겟)
	SKILL_TYPE_12 ,							//12.상태지속(변경)(셀프)
	SKILL_TYPE_13 ,							//13.상태지속(변경)(타겟)
	SKILL_TYPE_14 ,							//14.소환마법
	SKILL_TYPE_15 ,							// 패시브 스킬
	SKILL_TYPE_PASSIVE = SKILL_TYPE_15,		
	SKILL_TYPE_16 ,							// Emotion
	SKILL_TYPE_17 ,							// Self & damage
	SKILL_TYPE_18 ,							/// 추가 2004.4.8 : warp skill
	SKILL_TYPE_19,							/* 추가 2004.6.16 :
											능력치 변경(10번유형) & 데미지공격(3번유형)
													기존의 3번스킬 유형으로 타겟에게 데미지를 주고, 자신에게는
													지정된 능력치(21번,24번 칼럼)의 수치(22,25번칼럼)가 증가된다.
													스킬성공계산식을 적용하여 성공시에만 능력치 변경을 적용시킨다.
													(타겟에 데미지공격을 하면서 HP,MP 뺏어오기 개념)
											*/
	SKILL_TYPE_20,							//20.부활
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
	SKILL_TARGET_FILTER_SELF = 0,			///< 자기 자신
	SKILL_TARGET_FILTER_GROUP = 1,			///< 그룹(파티)원
	SKILL_TARGET_FILTER_GUILD = 2,			///< 길드원
	SKILL_TARGET_FILTER_FRIEND_ALL = 3,		///< 아군 (아바타, 소환몹)
	SKILL_TARGET_FILTER_MOB = 4,			///< 모든 몬스터 (소환몹,적군몹)
	SKILL_TARGET_FILTER_ENEMY_ALL = 5,		///< 적 PC + 몬스터 	
	SKILL_TARGET_FILTER_ENEMY_PC = 6,		///< 적 PC( 적 상태 포함 )
	SKILL_TARGET_FILTER_ALL_PC = 7,			///< 모든 PC
	SKILL_TARGET_FILTER_ALL_CHAR = 8,		///< 모든 캐릭터( 몬스터, 플레이어 )	
	SKILL_TARGET_FILTER_DEAD_USER = 9,		/// 죽은 사용자
	SKILL_TARGET_FILTER_ENEMY_MOB = 10,		/// 적 몬스터
	SKILL_TARGET_FILTER_MAX,
};

//-------------------------------------------------------------------------------------------------

#ifdef __SERVER
	#define	SKILL_NAME(I)						g_SkillList.m_SkillDATA.m_ppNAME[ I ]			// 스킬 이름
	#define SKILL_DESC(I)						g_SkillList.m_SkillDATA.m_ppDESC[ I ]			// 스킬 설명
#else
	#define	SKILL_NAME(I)						CStringManager::GetSingleton().GetSkillName( I )
	#define SKILL_DESC(I)						CStringManager::GetSingleton().GetSkillDesc( I )
#endif

#define	SKILL_1LEV_INDEX(S)					g_SkillList.m_SkillDATA.GetData( S ,  1 )
#define	SKILL_LEVEL(S)						g_SkillList.m_SkillDATA.GetData( S ,  2 )
#define	SKILL_NEED_LEVELUPPOINT(S)			g_SkillList.m_SkillDATA.GetData( S ,  3 )
#define	SKILL_TAB_TYPE(S)					g_SkillList.m_SkillDATA.GetData( S ,  4 )
#define SKILL_TYPE( I )						g_SkillList.m_SkillDATA.GetData( I ,  5 )

#define SKILL_DISTANCE( I )					g_SkillList.m_SkillDATA.GetData( I ,  6 )		//	타겟과의 거리
#define SKILL_WARP_PLANET_NO( I )			g_SkillList.m_SkillDATA.GetData( I ,  6 )		//	워프 스킬일 경우 존이 위치한 행성 번호

#define SKILL_CLASS_FILTER( I )				g_SkillList.m_SkillDATA.GetData( I ,  7 )	
#define	SKILL_SCOPE(S)						g_SkillList.m_SkillDATA.GetData( S ,  8 )		// 적용범위
#define	SKILL_POWER(S)						g_SkillList.m_SkillDATA.GetData( S ,  9 )
#define	SKILL_ITEM_MAKE_NUM		SKILL_POWER	// 제조번호
#define SKILL_HARM( I )						g_SkillList.m_SkillDATA.GetData( I , 10 )	

#define SKILL_SUCCESS_RATIO( I )			g_SkillList.m_SkillDATA.GetData( I , 13 )		//	성공률
#define SKILL_DURATION( I )					g_SkillList.m_SkillDATA.GetData( I , 14 )		//	지속 시간
#define	SKILL_DAMAGE_TYPE(S)				g_SkillList.m_SkillDATA.GetData( S , 15 )		// 적용될 계산식 유형


#define	SKILL_USE_PROPERTY_CNT				2
#define	SKILL_USE_PROPERTY(S,T)				g_SkillList.m_SkillDATA.GetData( S , 16+(T)*2 )
#define	SKILL_USE_VALUE(S,T)				g_SkillList.m_SkillDATA.GetData( S , 17+(T)*2 )


#define	SKILL_RELOAD_TIME(S)				g_SkillList.m_SkillDATA.GetData( S , 20 )

// 패시브(지속성) 스킬일 경우 일정시간 상승시킬 능력치...
//#define	SKILL_INCREASE_ABILITY_CNT			2
//#define	SKILL_INCREASE_ABILITY(S,T)			g_SkillList.m_SkillDATA.GetData( S , 21+(T)*3 )
//#define	SKILL_INCREASE_ABILITY_VALUE(S,T)	g_SkillList.m_SkillDATA.GetData( S , 22+(T)*3 )
//#define	SKILL_CHANGE_ABILITY_RATE(S,T)		g_SkillList.m_SkillDATA.GetData( S , 23+(T)*3 )

#define	SKILL_WARP_ZONE_NO( S )				g_SkillList.m_SkillDATA.GetData( S , 21 )		/// 워프 스킬일 경우 워프할 존 번호
#define	SKILL_WARP_ZONE_XPOS( S )			g_SkillList.m_SkillDATA.GetData( S , 22 )		/// 워프 스킬일 경우 워프할 존 위치
#define	SKILL_WARP_ZONE_YPOS( S )			g_SkillList.m_SkillDATA.GetData( S , 23 )



#define	MAX_SKILL_RELOAD_TYPE				20	// 0~19
#define	SKILL_RELOAD_TYPE(S)				g_SkillList.m_SkillDATA.GetData( S , 27 )		// 스킬여백

#define	SKILL_SUMMON_PET(S)					g_SkillList.m_SkillDATA.GetData( S , 28 )		//	소환몹 번호

#define	SKILL_ACTION_MODE(S)				g_SkillList.m_SkillDATA.GetData( S , 29 )		// 스킬공격 유형
// 사용시 장착하고 있어야하는 장비.  T = 0 ~ 3
#define	SKILL_NEED_WEAPON_CNT				5		// <--- 2004. 2. 26 4->5로 늘림고 이후 컬럼 번호+1 by icarus
#define	SKILL_NEED_WEAPON(S,T)				g_SkillList.m_SkillDATA.GetData( S , 30+(T) )

// LIST_CLASS.stb 참조 번호...
#define	SKILL_AVAILBLE_CLASS_SET(S)			g_SkillList.m_SkillDATA.GetData( S , 35 )

// 최초 배울때 소속 조합 체크...
#define	SKILL_AVAILBLE_UNION_CNT			3
#define	SKILL_AVAILBLE_UNION(S,T)			g_SkillList.m_SkillDATA.GetData( S , 36+(T) )


// 최초로 배울때 미리 소유하고 있어야하는 스킬. T = 0 ~ 2
#define	SKILL_NEED_SKILL_CNT				3
#define	SKILL_NEED_SKILL_INDEX(S,T)			g_SkillList.m_SkillDATA.GetData( S , 39+(T)*2 )
#define	SKILL_NEDD_SKILL_LEVEL(S,T)			g_SkillList.m_SkillDATA.GetData( S , 40+(T)*2 )

// 최초로 배우거나 레벨업시 필요한 조건 능력치. T = 0 ~ 2
#define	SKILL_NEED_ABILITY_TYPE_CNT			2
#define	SKILL_NEED_ABILITY_TYPE(S,T)		g_SkillList.m_SkillDATA.GetData( S , 45+(T)*2 )	// 값은 AT_DEX~AT_SENSE 만 사용됨
#define	SKILL_NEED_ABILITY_VALUE(S,T)		g_SkillList.m_SkillDATA.GetData( S , 46+(T)*2 )


#define SKILL_SCRIPT1( I )					g_SkillList.m_SkillDATA.GetData( I , 49 )		

#define SKILL_RESERVE_02( I )				g_SkillList.m_SkillDATA.GetData( I , 50 )		
#define SKILL_ICON_NO( I )					g_SkillList.m_SkillDATA.GetData( I , 51 )		

// 캐스팅 동작
#define	SKILL_ANI_CASTING(S)				g_SkillList.m_SkillDATA.GetData( S , 52 )
#define	SKILL_ANI_CASTING_SPEED(S)			g_SkillList.m_SkillDATA.GetData( S , 53 )

/// 캐스팅 루프 동작
#define	SKILL_ANI_CASTING_REPEAT(S)			g_SkillList.m_SkillDATA.GetData( S , 54 )
#define	SKILL_ANI_CASTING_REPEAT_CNT(S)		g_SkillList.m_SkillDATA.GetData( S , 55 )

/// 캐스팅 이펙트
#define SKILL_CASTING_EFFECT_CNT			4
#define SKILL_CASTING_EFFECT( I,T )			g_SkillList.m_SkillDATA.GetData( I , 56 + (T * 3) )
#define SKILL_CASTING_EFFECT_POINT( I,T )	g_SkillList.m_SkillDATA.GetData( I , 57 + (T * 3) )
#define SKILL_CASTING_SOUND( I,T )			g_SkillList.m_SkillDATA.GetData( I , 58 + (T * 3) )

// 실제 동작
#define	SKILL_ANI_ACTION_TYPE(S)			g_SkillList.m_SkillDATA.GetData( S , 68 )
#define	SKILL_ANI_ACTION_SPEED(S)			g_SkillList.m_SkillDATA.GetData( S , 69 )
#define	SKILL_ANI_HIT_COUNT(S)				g_SkillList.m_SkillDATA.GetData( S , 70 )

/// 발사 총알 이펙트
#define SKILL_BULLET_NO( I )				g_SkillList.m_SkillDATA.GetData( I , 71 )
#define SKILL_BULLET_LINKED_POINT( I )		g_SkillList.m_SkillDATA.GetData( I , 72 )
#define SKILL_BULLET_FIRE_SOUND( I )		g_SkillList.m_SkillDATA.GetData( I , 73 )

/// 타격 이펙트
#define SKILL_HIT_EFFECT( I )				g_SkillList.m_SkillDATA.GetData( I , 74 )			//	타격 효과
#define SKILL_HIT_EFFECT_LINKED_POINT( I )	g_SkillList.m_SkillDATA.GetData( I , 75 )			//	타격 효과
#define SKILL_HIT_SOUND( I )				g_SkillList.m_SkillDATA.GetData( I , 76 )			//	타격 효과음

/// 연타 더미 이펙트
#define SKILL_HIT_DUMMY_EFFECT_CNT			2
#define SKILL_HIT_DUMMY_EFFECT( I, T )					g_SkillList.m_SkillDATA.GetData( I , 77 + 3*T )			//	더미 타격 효과
#define SKILL_HIT_DUMMY_EFFECT_LINKED_POINT( I, T )		g_SkillList.m_SkillDATA.GetData( I , 78 + 3*T )			//	더미 타격 효과
#define SKILL_HIT_DUMMY_SOUND( I, T )					g_SkillList.m_SkillDATA.GetData( I , 79 + 3*T )			//	더미 타격 효과음

#define SKILL_AREA_HIT_EFFECT( I )			g_SkillList.m_SkillDATA.GetData( I , 83 )								//	지역 타격 효과
#define SKILL_AREA_HIT_SOUND( I )			g_SkillList.m_SkillDATA.GetData( I , 84 )								//	지역 타격 효과음

#define	SKILL_LEVELUP_NEED_ZULY( I )		g_SkillList.m_SkillDATA.GetData( I , 85 )				


#define	SKILL_ATTRIBUTE( I )				g_SkillList.m_SkillDATA.GetData( I , 86 )///1 : 아바타 스킬 , 2: PAT스킬 , 3: 공통 스킬



#define SKILL_INCREASE_ABILITY_CNT			3														// 88~99
#define SKILL_STATE_STB( I,C )				g_SkillList.m_SkillDATA.GetData( I , 88+(C)*4 )		// 상태 번호
#define SKILL_INCREASE_ABILITY( I,C )		g_SkillList.m_SkillDATA.GetData( I , 89+(C)*4 )		// 변경능력치.
#define SKILL_INCREASE_ABILITY_VALUE( I,C )	g_SkillList.m_SkillDATA.GetData( I , 90+(C)*4 )		// 변경수치.
#define SKILL_CHANGE_ABILITY_RATE( I,C )	g_SkillList.m_SkillDATA.GetData( I , 91+(C)*4 )		// 변경비율.


#define SKILL_CANT_USE_OTHER_SKILL( I )		g_SkillList.m_SkillDATA.GetData( I , 100 )

#define SKILL_CHILD_SKILL_CNT				5														// 101~110
#define SKILL_CHILD_SKILL_INDEX( I,C )		g_SkillList.m_SkillDATA.GetData( I , 101+(C)*2 )		// 하위 스킬
#define SKILL_CHILD_SKILL_LEVEL( I,C )		g_SkillList.m_SkillDATA.GetData( I , 102+(C)*2 )		// 하위 스킬 레벨

#define SKILL_SLOT_NUM( I )					g_SkillList.m_SkillDATA.GetData( I , 111 )			// 슬롯 번호
#define SKILL_NEED_SKILLBOOK( I )			g_SkillList.m_SkillDATA.GetData( I , 112 )			// 필요 스킬북


// 11, 12, 21~26 컬럼이 비어 있다..추가 할때 그곳 써도 될듯.

#define SKILL_ATTRIBUTE_AVATAR				1
#define SKILL_ATTRIBUTE_CART				2
#define SKILL_ATTRIBUTE_CASTLEGEAR			4


#define	SA_STOP						0	// 스킬 적용후 정지
#define	SA_ATTACK					1	// 스킬 적용후 타겟을 공격
#define	SA_RESTORE					2	// 스킬 사용후 이전 명령 복귀

#ifdef	__SERVER
	#define	SKILL_RELOAD_SECOND(S)			g_SkillList.m_pReloadTIME[ S ]
#endif

class CSkillLIST 
{
private:
	int			m_iSkillCount;	

	float	   *m_pCastingAniSPEED;		/// 미리 계산되어진 스킬 케스팅 동작 속도
	float	   *m_pActionAniSPEED;		/// 미리 계산되어지 스킬 실제 동작 속도.
	
	std::vector<int>	m_list_Common;			//공통 스킬 리스트.	.
	std::vector<int>	m_list_Job_Skill[4][4];	//1레벨 스킬 리스트.
											//솔져, 뮤즈, 호커, 딜러.
											//솔져 : 기본, 한손, 양손, 2차전직스킬	
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
