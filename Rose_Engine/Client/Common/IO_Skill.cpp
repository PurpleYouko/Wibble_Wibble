#include "stdAFX.h"
#include "IO_Skill.h"
#ifndef	__SERVER
	#include "..\util\CGameSTB.h"
#else
	#include "..\util\classSTB.h"
#include "..\GameProc\UseItemDelay.h"
#endif


CSkillLIST g_SkillList;

//-------------------------------------------------------------------------------------------------
CSkillLIST::CSkillLIST ()
{
	m_iSkillCount = 0;

	m_pCastingAniSPEED = NULL;
	m_pActionAniSPEED = NULL;

	m_pReloadTIME = NULL;

	m_iCartSkillIndex	= -1;
	m_iCastlegearSkillIndex = -1;
}

CSkillLIST::~CSkillLIST ()
{
	this->Free ();
}

void CSkillLIST::Free ()
{
	m_SkillDATA.Free ();
	SAFE_DELETE_ARRAY( m_pCastingAniSPEED );
	SAFE_DELETE_ARRAY( m_pActionAniSPEED );

	SAFE_DELETE_ARRAY( m_pReloadTIME );
}

bool CSkillLIST::LoadSkillTable(const char* pFileName)
{
	if ( !m_SkillDATA.Load( (char*)pFileName, true, true) )
		return false;

	m_pCastingAniSPEED = new float[ m_SkillDATA.m_nDataCnt ];
	m_pActionAniSPEED = new float[ m_SkillDATA.m_nDataCnt ];

#ifdef	__SERVER
	m_pReloadTIME = new DWORD[ m_SkillDATA.m_nDataCnt ];
	::ZeroMemory( m_pReloadTIME, sizeof(DWORD) * m_SkillDATA.m_nDataCnt );
#else
	m_pReloadTIME = new float[ m_SkillDATA.m_nDataCnt ];
	::ZeroMemory( m_pReloadTIME, sizeof(float) * m_SkillDATA.m_nDataCnt );
#endif

// 홍근.
	m_list_Common.clear();	
	int i, j;
	for( i=0; i< 4; i++ )
	{
		for( j=0; j< 4; j++ )
		{
			m_list_Job_Skill[i][j].clear();
		}
	}

	m_list_Cart.clear();
	m_list_Castlegear.clear();
	m_list_Unique.clear();
	int iJob = 0;

	// 0번 스킬 사용안함...
	for (short nI=1; nI<m_SkillDATA.m_nDataCnt; nI++) 
	{	
		switch(SKILL_AVAILBLE_CLASS_SET(nI))
		{
			//솔져들
		case 41: case 61: case 62:
			iJob = 0;
			break;
			//뮤즈들
		case 42: case 63: case 64:
			iJob = 1;
			break;
			//호커들
		case 43: case 65: case 66:
			iJob = 2;
			break;
			//딜러들
		case 44: case 67: case 68:
			iJob = 3;
			break;
		}

		switch(SKILL_TAB_TYPE(nI))
		{
		case SKILL_TAB_COMMON:
			if( (SKILL_LEVEL( nI ) != 1 && SKILL_LEVEL( nI ) != 0) )	break;
			m_list_Common.push_back( nI );
			break;

		case SKILL_TAB_JOB_COMMON:
			if( iJob < 0 || iJob > 3 || (SKILL_LEVEL( nI ) != 1 && SKILL_LEVEL( nI ) != 0) )	break;
			m_list_Job_Skill[iJob][0].push_back( nI );
			break;

		case SKILL_TAB_JOB_TYPE1:
			if( iJob < 0 || iJob > 3 || (SKILL_LEVEL( nI ) != 1 && SKILL_LEVEL( nI ) != 0) )	break;
			m_list_Job_Skill[iJob][1].push_back( nI );
			break;

		case SKILL_TAB_JOB_TYPE2:
			if( iJob < 0 || iJob > 3 || (SKILL_LEVEL( nI ) != 1 && SKILL_LEVEL( nI ) != 0) )	break;
			m_list_Job_Skill[iJob][2].push_back( nI );
			break;

		case SKILL_TAB_JOB2:
			if( iJob < 0 || iJob > 3 || (SKILL_LEVEL( nI ) != 1 && SKILL_LEVEL( nI ) != 0) )	break;
			m_list_Job_Skill[iJob][3].push_back( nI );
			break;

		case SKILL_TAB_PARTS_CART:

			if( SKILL_TAB_TYPE(nI)==SKILL_TAB_PARTS_CART 
				&& SKILL_TYPE(nI)==SKILL_TYPE_01 )
			{
				m_iCartSkillIndex = nI;
			}

			if( (SKILL_LEVEL( nI ) != 1 && SKILL_LEVEL( nI ) != 0) )	break;
			m_list_Cart.push_back( nI );
			break;

		case SKILL_TAB_PARTS_CASTLEGEAR:
			
			if( SKILL_TAB_TYPE(nI)==SKILL_TAB_PARTS_CASTLEGEAR 
				&& SKILL_TYPE(nI)==SKILL_TYPE_01 )
			{
				m_iCastlegearSkillIndex = nI;
			}

			if( (SKILL_LEVEL( nI ) != 1 && SKILL_LEVEL( nI ) != 0) )	break;
			m_list_Castlegear.push_back( nI );
			break;

		case SKILL_TAB_UNIQUE:
			if( (SKILL_LEVEL( nI ) != 1 && SKILL_LEVEL( nI ) != 0) )	break;
			m_list_Unique.push_back( nI );
			break;
		}
		
		if ( SKILL_ANI_CASTING_REPEAT_CNT(nI) )
		{
			if ( SKILL_ANI_CASTING_REPEAT(nI ) < 1 ) 
			{
				// 반복 동작 횟수가 있는 반복 동작이 없다 !!!
				#ifdef	__INC_WORLD
				char *pMsg = CStr::Printf ("스킬 반복 모션 번호 입력 필요 : %d 라인", nI );
					::MessageBox( NULL, pMsg, "스킬 stb 데이타 입력 오류", MB_OK );
				#endif

				SKILL_ANI_CASTING_REPEAT_CNT(nI) = 0;
			}
		} // else
		//if ( SKILL_ANI_CASTING_REPEAT(nI ) )	// 반복 동작이 있는데 반복 횟수가 0이면 1로 설정.
		//	SKILL_ANI_CASTING_REPEAT_CNT(nI) = 1;

#ifdef	__SERVER
		// 서버에서는 반복 모션이 있을경우 케스팅 동작의 애니속도를 반복 모션값을 참조하여 보정...
		// 가정 :: 케스팅 동작, 케스팅 반복 동작은 장착된 무기에 상관없이 항상 같은 모션을 사용한다 !!
		// 반복 속도 값의 기본은 1.0f, 2.0f이 될경우 모션이 2배 빨라 진다, 0.5f가 되면 2배 느려짐.
		if ( SKILL_ANI_CASTING_REPEAT_CNT(nI) ) 
		{
			tagMOTION *pMOTION = NULL;
			float fSpeed;
			DWORD dwTotTime;
			WORD wTotFrame;
			short nMotionTYPE=0;
			// 반복 모션
			for (short nM=0; nM<g_TblAniTYPE.m_nColCnt; nM++) {
				if ( FILE_MOTION( nM, SKILL_ANI_CASTING_REPEAT(nI) ) ) {
					nMotionTYPE = nM;
					break;
				}
			}

			pMOTION = g_MotionFILE.IDX_GetMOTION( FILE_MOTION( nMotionTYPE, SKILL_ANI_CASTING_REPEAT(nI) ), 0 );
			wTotFrame = pMOTION->Get_TotalFRAME () * SKILL_ANI_CASTING_REPEAT_CNT(nI);

			// 모션 속도
			fSpeed = SKILL_ANI_CASTING_SPEED(nI) / 100.f;

			// 총 반복시 소요될 시간...
			dwTotTime = pMOTION->Get_NeedTIME( wTotFrame, fSpeed );

			// 케스팅 모션
			pMOTION = g_MotionFILE.IDX_GetMOTION( FILE_MOTION( nMotionTYPE, SKILL_ANI_CASTING(nI) ) , 0 );
			wTotFrame = pMOTION->Get_TotalFRAME ();

			// 전체 소요될 시간..
			dwTotTime += pMOTION->Get_NeedTIME( wTotFrame, fSpeed );

			m_pCastingAniSPEED[ nI ] = ( 1000.f * wTotFrame ) / ( dwTotTime * pMOTION->m_wFPS );
		} 
		else 
		{
			m_pCastingAniSPEED[ nI ] = SKILL_ANI_CASTING_SPEED(nI) / 100.f;
		}

		// 스킬 반복가능 딜레이 시간( 단위 1당 0.2초 )
		m_pReloadTIME[ nI ] = SKILL_RELOAD_TIME(nI) * 200 - 100;	// 0.2f * 1000 :: 0.1초 유예~;
#else
		
		m_pCastingAniSPEED[ nI ] = SKILL_ANI_CASTING_SPEED(nI) / 100.f;

//---------------------------------------------------------------------------------------------
//2005. 5. 30. 박 지호 
		//클라이언트에서 충전시간을 1/5 의 시간으로 바꾼다.	
		m_pReloadTIME[ nI ] = ((float)SKILL_RELOAD_TIME(nI) / 5.0f) * 1000.0f;
		 if ( m_pReloadTIME[ nI ] <= 0.f )
				m_pReloadTIME[ nI ] = 1.0f;	
//---------------------------------------------------------------------------------------------				 
#endif

		if ( m_pCastingAniSPEED[ nI ] <= 0.f ) {
			#ifndef	__SERVER
			// _ASSERT( m_pCastingAniSPEED[ nI ] > 0.f );
			#endif
			m_pCastingAniSPEED[ nI ] = 1.0f;		// 디폴드로 만들어 버림...
		}

		m_pActionAniSPEED[ nI ] = SKILL_ANI_ACTION_SPEED(nI) / 100.f;
		if ( m_pActionAniSPEED[ nI ] <= 0.f ) {
			#ifndef	__SERVER
			// _ASSERT( m_pActionAniSPEED[ nI ] > 0.f );
			#endif
			m_pActionAniSPEED[ nI ] = 1.0f;
		}
	}


	m_iSkillCount = m_SkillDATA.m_nDataCnt;

	return true;
}

//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
// 2005. 5. 30 박 지호 
float CSkillLIST::GetDelayTickCount(DWORD i)
{

	if((i < 0) || (i >= m_SkillDATA.m_nDataCnt))
		return 1.0f;

	return m_pReloadTIME[ i ];

}


std::vector<int> & CSkillLIST::Get_List_Common()
{
	return m_list_Common;
}
std::vector<int> & CSkillLIST::Get_List_Job_Skill(int i, int j)
{
	return m_list_Job_Skill[i][j];
}
std::vector<int> & CSkillLIST::Get_List_Cart()
{
	return m_list_Cart;
}
std::vector<int> & CSkillLIST::Get_List_Castlegear()
{
	return m_list_Castlegear;
}
std::vector<int> & CSkillLIST::Get_List_Unique()
{
	return m_list_Unique;
}
int CSkillLIST::Get_Cart_SkillIndex()
{
	return m_iCartSkillIndex;
}
int	CSkillLIST::Get_Castlegear_SkillIndex()
{
	return m_iCastlegearSkillIndex;
}