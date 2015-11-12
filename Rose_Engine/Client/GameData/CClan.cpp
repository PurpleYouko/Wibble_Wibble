#include "stdafx.h"
#include ".\cclan.h"
#include "../Network/CNetwork.h"
#include "IO_Terrain.h"
#include "../Game_FUNC.h"

CClan::CClan(void)
{
	m_strName.clear();
	m_strSlogan.clear();
	m_iClass		= 0;
	m_iClanNo		= 0;
	m_nMarkCenter	= 0;
	m_nMarkBack		= 0;
	m_nLevel		= 0;
	m_iPoint		= 0;
	m_iStoreRate	= 0;
	m_i64Money		= 0;

	m_iRankPoint	= 0;
	m_iRewardPoint	= 0;

	m_iAlly1		= 0;
	m_strAlly1.clear();
	m_iAlly2		= 0;
	m_strAlly2.clear();
	m_iAlly3		= 0;
	m_strAlly3.clear();
}

CClan::~CClan(void)
{

}

CClan& CClan::GetInstance()
{
	static CClan s_Instance;
	return s_Instance;
}

void	CClan::SetName( const char* pszName )
{
	assert( pszName );
	if( pszName )
		m_strName = pszName;
}

void	CClan::SetMarkCenter( short nID )
{
	m_nMarkCenter = nID;
}
void	CClan::SetMarkBack( short nID )
{
	m_nMarkBack = nID;
}
void	CClan::SetLevel( short nLevel )
{
	m_nLevel = nLevel;
}
void	CClan::SetPoint( int iPoint )
{
	m_iPoint = iPoint;
}
void	CClan::SetAlly( int iIndex, int iClanID, const char* pszName )
{
	assert( pszName );
	if( pszName == NULL ) return;

	switch( iIndex )
	{
	case 0:
		m_iAlly1	= iClanID;
		m_strAlly1	= pszName;
		break;
	case 1:
		m_iAlly2	= iClanID;
		m_strAlly2 = pszName;
		break;
	case 2:
		m_iAlly3	= iClanID;
		m_strAlly3 = pszName;
		break;
	default:
		assert( 0 && "Invald Allied Clan Index" );
		break;
	}
}
void	CClan::SetStoreRate( int iRate )
{
	m_iStoreRate = iRate;
}
void	CClan::SetMoney( __int64 i64Money )
{
	m_i64Money = i64Money;
}

void	CClan::SetRankPoint(int iRankPoint)
{
	m_iRankPoint = iRankPoint;
}
int		CClan::GetRankPoint()
{
	return m_iRankPoint;
}

void	CClan::SetRewardPoint(int iRewardPoint)
{
	m_iRewardPoint = iRewardPoint;
}
int		CClan::GetRewardPoint()
{
	return m_iRewardPoint;
}

const char* CClan::GetName()
{
	return m_strName.c_str();
}
const char* CClan::GetSlogan()
{
	return m_strSlogan.c_str();
}
short		CClan::GetMarkCenter()
{
	return m_nMarkCenter;
}

short		CClan::GetMarkBack()
{
	return m_nMarkBack;
}

short		CClan::GetLevel()
{
	return m_nLevel;
}

int			CClan::GetPoint()
{
	return m_iPoint;
}

int			CClan::GetAllyID( int iIndex )
{
	switch( iIndex )
	{
	case 0:
		return m_iAlly1;
	case 1:
		return m_iAlly2;
	case 2:
		return m_iAlly3;
	default:
		assert( 0 && "Invald Allied Clan Index" );
		break;
	}
	return 0;
}

const char* CClan::GetAllyName( int iIndex )
{
	switch( iIndex )
	{
	case 0:
		return m_strAlly1.c_str();
	case 1:
		return m_strAlly2.c_str();
	case 2:
		return m_strAlly3.c_str();
	default:
		assert( 0 && "Invald Allied Clan Index" );
		break;
	}
	return NULL;
}

int			CClan::GetStoreRate()
{
	return m_iStoreRate;
}
__int64		CClan::GetMoney()
{
	return m_i64Money;
}

void CClan::Clear()
{
	m_strNotice.clear();
	m_strName.clear();
	m_strSlogan.clear();
	m_iClanNo		= 0;
	m_nMarkCenter	= 0;
	m_nMarkBack		= 0;
	m_nLevel		= 0;
	m_iPoint		= 0;
	m_iStoreRate	= 0;
	m_i64Money		= 0;
	m_iClass		= 0;
	m_iAlly1		= 0;
	m_strAlly1.clear();
	m_iAlly2		= 0;
	m_strAlly2.clear();
	m_iAlly3		= 0;
	m_strAlly3.clear();
	ClearMemberlist();
	///Observer들에게 해당 Data삭제하도록 알려준다.

	m_Event.SetID( CTEventClan::EID_SET_CLEAR_DATA );
	SetChanged();
	NotifyObservers( &m_Event );

}

void CClan::SetClanNo( int iNo )
{
	m_iClanNo = iNo;
}

int CClan::GetClanNo()
{
	return m_iClanNo;
}


void	CClan::SetSlogan( const char* pszSlogan )
{
	assert( pszSlogan );
	if( pszSlogan )
	{
		m_strSlogan = pszSlogan;

#ifdef __EDIT_CLANDLG
		m_Event.SetID( CTEventClan::EID_SET_SLOGAN );
		SetChanged();
		NotifyObservers( &m_Event );
#endif
	}
}

void CClan::AddMember( const char* pszName, int iClass , int iClanPoint, int iChannelNo, short nLevel, short nJob )
{
	assert( pszName );
	if( pszName )
	{
		S_ClanMember member;
		member.m_strName = pszName;
		member.m_iClass = iClass;
		m_Members.push_back( member );

		m_Event.SetID( CTEventClan::EID_ADD_MEMBER );
		m_Event.SetName( pszName );
		m_Event.SetClass( iClass );
		m_Event.SetPoint( iClanPoint );
		m_Event.SetChannelNo( iChannelNo );
		m_Event.SetJob( nJob );
		m_Event.SetLevel( nLevel );
		SetChanged();
		NotifyObservers( &m_Event );


		if( g_pAVATAR != NULL && _strcmpi( pszName , g_pAVATAR->Get_NAME() ) == 0 )
			m_iMyClanPoint = iClanPoint;
	}
}

void CClan::RemoveMember( const char* pszName )
{
	assert( pszName );
	if( pszName )
	{
		std::list< S_ClanMember >::iterator iter;
		int iIndex = 0;
		for( iter = m_Members.begin(); iter != m_Members.end(); ++iter ,++iIndex )
		{
			if( _strcmpi( iter->m_strName.c_str(), pszName ) == 0)
			{
				m_Event.SetID( CTEventClan::EID_REMOVE_MEMBER );
				m_Event.SetName( pszName );
				SetChanged();
				NotifyObservers( &m_Event );
				m_Members.erase( iter );		
				return;
			}
		}
	}
}

void CClan::ChangeClass( const char* pszName , int iClass )
{
	assert( pszName );
	if( pszName )
	{
		if( g_pAVATAR != NULL && _strcmpi( pszName , g_pAVATAR->Get_NAME() ) == 0 )
			g_pAVATAR->SetClanPos( (BYTE)iClass );
		
		std::list< S_ClanMember >::iterator iter;
		int iIndex = 0;
		for( iter = m_Members.begin(); iter != m_Members.end(); ++iter ,++iIndex )
		{
			if( _strcmpi( iter->m_strName.c_str(), pszName ) == 0)
			{
				iter->m_iClass = iClass;
				m_Event.SetID( CTEventClan::EID_CHANGE_CLASS );
				m_Event.SetName( pszName );
				m_Event.SetClass( iClass );
				SetChanged();
				NotifyObservers( &m_Event );
				return;
			}
		}
	}
}

void CClan::ChangeConnectStatus( const char* pszName, int iChannelNo )
{
	assert( pszName );
	if( pszName )
	{
		m_Event.SetID( CTEventClan::EID_CHANGE_CONNECT_STATUS );
		m_Event.SetName( pszName );
		m_Event.SetChannelNo( iChannelNo );
		SetChanged();
		NotifyObservers( &m_Event );
	}
}

const char*	CClan::GetNotice()
{
	return m_strNotice.c_str();
}

void CClan::SetNotice( const char* pszNotice )
{
	assert( pszNotice );
	if( pszNotice )
	{
		m_strNotice = pszNotice;
		m_Event.SetID( CTEventClan::EID_SET_NOTICE );
		SetChanged();
		NotifyObservers( &m_Event );
	}
}

void CClan::SetClass( int iClass )
{
	m_iClass = iClass;
}

int CClan::GetClass()
{
	return m_iClass;
}

void CClan::RequestMemberList()
{
	g_pNet->Send_cli_CLAN_COMMAND( GCMD_ROSTER, NULL );
}

//void CClan::RequestClanInfo()
//{
//	g_pNet->Send_cli_CLAN_COMMAND( GCMD_INFO, NULL );
//}

void CClan::ClearMemberlist()
{
	std::list< S_ClanMember >::iterator iter;
	for( iter = m_Members.begin(); iter != m_Members.end();)
	{
		m_Event.SetID( CTEventClan::EID_REMOVE_MEMBER );
		m_Event.SetName( iter->m_strName.c_str() );
		SetChanged();
		NotifyObservers( &m_Event );
		iter = m_Members.erase( iter );
	}
}

int CClan::GetMemberCount()
{
	return (int)m_Members.size();
}

int CClan::GetMemberMaxCount()
{
	const int c_iMaxClanLevel = 7;
	const int c_iMaxCount[c_iMaxClanLevel] = { 15,20,25,30,36,43,50 };
	
	if( m_nLevel < 1 || m_nLevel > c_iMaxClanLevel )
		return 0;

	return c_iMaxCount[ m_nLevel - 1];
}

bool CClan::IsValidClassUp( int iMemberClass )
{
	if( iMemberClass <= CLAN_JUNIOR )///All Ok
		return true;

	if( iMemberClass >= CLAN_SUB_MASTER )///All False;
		return false;

	int iCurrClassCount = GetCurrClassCount( iMemberClass + 1 );
	int iMaxClassCount	= GetMaxClassCount( m_nLevel, iMemberClass + 1 );

	if( iCurrClassCount >= iMaxClassCount )
		return false;

	return true;
}
int CClan::GetCurrClassCount( int iClass )
{
	int iCount = 0;
	std::list< S_ClanMember >::iterator iter;
	for( iter = m_Members.begin(); iter != m_Members.end(); ++iter )
	{
		if( iter->m_iClass == iClass )
			++iCount;
	}
	return iCount;
}

int CClan::GetMaxClassCount( int iClanLevel, int iClass )
{
	assert( iClanLevel >= 1 && iClanLevel <= 7 );
	if( iClanLevel < 1 || iClanLevel > 7 )
		return 0;

	assert( iClass < CLAN_MASTER && iClass > CLAN_SENIOR );
	if( iClass >= CLAN_MASTER || iClass <= CLAN_SENIOR )
		return 0;

	switch( iClass )
	{
	case CLAN_SUB_MASTER:
		return (int)(iClanLevel * 0.3 + 1);
	case CLAN_COMMANDER:
		return iClanLevel + 2;
	case CLAN_LANDER:
		return (int)(iClanLevel * 1.8 + 2);
	default:
		break;
	}
	return 0;
}

bool CClan::IsValidJoinMember(char *pszName)
{
	if( m_nLevel < 1 || m_nLevel > 7)
		return false;

	const int c_iMaxMemberCount[7] = { 15, 20, 25, 30, 36, 43, 50 };

	if( (int)m_Members.size() >= c_iMaxMemberCount[ m_nLevel - 1] )
		return false;

	if(FindClanChar(pszName))   //이미 내클랜에 가입된 유저여부 판단....
		return false;

	return true;
}

int CClan::GetMyClanPoint()
{
	return m_iMyClanPoint;
}

void CClan::SetMemberLevel( const char* pszName, short nLev )
{
	assert( pszName );
	if( pszName )
	{
		std::list< S_ClanMember >::iterator iter;
		int iIndex = 0;
		for( iter = m_Members.begin(); iter != m_Members.end(); ++iter ,++iIndex )
		{
			if( _strcmpi( iter->m_strName.c_str(), pszName ) == 0)
			{
				m_Event.SetID( CTEventClan::EID_SET_LEVEL );
				m_Event.SetName( pszName );
				m_Event.SetLevel( nLev );
				SetChanged();
				NotifyObservers( &m_Event );
				return;
			}
		}
	}
}

void CClan::SetMemberJob( const char* pszName, short nJob )
{
	assert( pszName );
	if( pszName )
	{
		std::list< S_ClanMember >::iterator iter;
		int iIndex = 0;
		for( iter = m_Members.begin(); iter != m_Members.end(); ++iter ,++iIndex )
		{
			if( _strcmpi( iter->m_strName.c_str(), pszName ) == 0)
			{
				m_Event.SetID( CTEventClan::EID_SET_JOB );
				m_Event.SetName( pszName );
				m_Event.SetJob( nJob );
				SetChanged();
				NotifyObservers( &m_Event );
				return;
			}
		}
	}
}

void CClan::SetSkill( tagClanSKILL* skilldata, int count )
{
	CClanSkillSlot* clan_skill_slot = CClan::GetInstance().GetClanSkillSlot();

	for( int i = 0 ;i < count; ++i )
	{
		if( skilldata[i].m_nSkillIDX )
			clan_skill_slot->SetSkillSlot( i, skilldata[i].m_nSkillIDX, 1 , skilldata[i].m_dwExpiredAbsSEC );			
		else
			clan_skill_slot->RemoveBySlotIndex( i );
	}
}

CClanSkillSlot* CClan::GetClanSkillSlot()
{ 
	return &m_ClanSkillSlot; 
}

#pragma warning( disable : 4172 )
S_ClanMember* CClan::FindClanChar(const char* pszName )
{
	std::list< S_ClanMember >::iterator iter;
	int iIndex = 0;
	for( iter = m_Members.begin(); iter != m_Members.end(); ++iter ,++iIndex )
	{
		if( _strcmpi( iter->m_strName.c_str(), pszName ) == 0)
		{
			return (S_ClanMember*)&iter;
		}
	}

	return NULL;
}

void CClan::SetClanMarkRegTime( SYSTEMTIME& time )
{
	m_ClanMarkRegTime = time;
}
SYSTEMTIME& CClan::GetClanMarkRegTime()
{
	return m_ClanMarkRegTime;
}


//=======================================================================================
//
//  Macros and Globals
//
//=======================================================================================
// 클랜전 시작과 끝의 연속 메시지 출력 관련 변수들
bool g_isStarted = false;
bool g_isEnded   = false;


#include "../System/SystemProcScript.h"
#include "../Util/SystemUtil.h"

//---------------------------------------------------------------------------------------
//  Name  : SetClanWarStartMsg()
//  Desc  : 클랜전 맵 로드 후 시작 메시지 출력
//  Author: Antonio - 2006-08-23  오후 2:18:44
//---------------------------------------------------------------------------------------
void SetClanWarStartMsg()
{
	g_pTimeTexts->DeleteTexts();
	g_pTimeTexts->AddText( STR_CLANWAR_START, TIME_TEXT_YELLOW );
	g_isStarted = true;
} //SetClanWarStartMsg

//---------------------------------------------------------------------------------------
//  Name  : SetClanWarProgressMsg()
//  Desc  : 클랜전 진행과 관련한 메시지 출력 (서버에서 받음)
//  Author: Antonio - 2006-08-23  오후 2:19:30
//---------------------------------------------------------------------------------------
void SetClanWarProgressMsg( int progress )
{
	switch ( progress )
	{
	case 1:		// 클랜전 시작 -- 맵 로딩 후 클라이언트에서 임의 출력
		{
			break;
		}
	case 2:		// A팀 승리
		{
			g_pTimeTexts->AddText( STR_CLANWAR_KILLED_RED_DRAGON, TIME_TEXT_RED );
			g_isEnded = true;
			if ( g_pAVATAR->Get_TeamNO() == 1000 )
			{
				SE_Congratulation( g_pAVATAR->Get_INDEX() );
			}
			break;
		}
	case 3:		// B팀 승리
		{
			g_pTimeTexts->AddText( STR_CLANWAR_KILLED_BLUE_DRAGON, TIME_TEXT_BLUE );
			g_isEnded = true;
			if ( g_pAVATAR->Get_TeamNO() == 2000 )
			{
				SE_Congratulation( g_pAVATAR->Get_INDEX() );
			}
			break;
		}
	case 4:		// 비김
		{
			g_pTimeTexts->AddText( STR_CLANWAR_WAS_DRAWN, TIME_TEXT_YELLOW );
			g_isEnded = true;
			break;
		}
	case 5:		// 클랜전 취소 통보
		{
			g_pTimeTexts->AddText( STR_CLANWAR_NOTIFY_CANCEL, TIME_TEXT_YELLOW );
			break;
		}
	}
	// 클랜전 채널 이동 메시지
	if ( progress >= 101 && progress <= 105 )
	{
		char str[_MAX_PATH] = { 0, };
		_itoa( progress-100, str, 10 );
		sprintf( str, "%s%s", str, STR_CLANWAR_MOVE_CHANNEL );
		g_pTimeTexts->AddText( str, TIME_TEXT_YELLOW );
	}
} //SetClanWarProgressMsg

//---------------------------------------------------------------------------------------
//  Name  : DrawClanWarDescMessages()
//  Desc  : 클랜전에 관한 정보 메시지 표시
//  Author: Antonio - 2006-09-12  오전 11:58:27
//---------------------------------------------------------------------------------------
void DrawClanWarDescMessages()
{
	if ( g_isStarted )
	{
		g_pTimeTexts->AddText( STR_CLANWAR_DESC, TIME_TEXT_YELLOW );
		g_isStarted = false;
	}
	if ( g_isEnded )
	{
		g_pTimeTexts->AddText( STR_CLANWAR_END_RETURN, TIME_TEXT_YELLOW );
		g_isEnded = false;
	}
} //DrawClanWarDescMessages

//---------------------------------------------------------------------------------------
//  Name  : ClearClanWarVariables()
//  Desc  : 클랜전 이외일 때 클리어 ( ensure safety )
//  Author: Antonio - 2006-09-12  오전 11:57:01
//---------------------------------------------------------------------------------------
void ClearClanWarVariables()
{
	if ( g_isStarted )
		g_isStarted = false;
	if ( g_isEnded )
		g_isEnded = false;
} //ClearClanWarVariables

