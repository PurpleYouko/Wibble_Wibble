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

// ȫ��.
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

	// 0�� ��ų ������...
	for (short nI=1; nI<m_SkillDATA.m_nDataCnt; nI++) 
	{	
		switch(SKILL_AVAILBLE_CLASS_SET(nI))
		{
			//������
		case 41: case 61: case 62:
			iJob = 0;
			break;
			//�����
		case 42: case 63: case 64:
			iJob = 1;
			break;
			//ȣĿ��
		case 43: case 65: case 66:
			iJob = 2;
			break;
			//������
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
				// �ݺ� ���� Ƚ���� �ִ� �ݺ� ������ ���� !!!
				#ifdef	__INC_WORLD
				char *pMsg = CStr::Printf ("��ų �ݺ� ��� ��ȣ �Է� �ʿ� : %d ����", nI );
					::MessageBox( NULL, pMsg, "��ų stb ����Ÿ �Է� ����", MB_OK );
				#endif

				SKILL_ANI_CASTING_REPEAT_CNT(nI) = 0;
			}
		} // else
		//if ( SKILL_ANI_CASTING_REPEAT(nI ) )	// �ݺ� ������ �ִµ� �ݺ� Ƚ���� 0�̸� 1�� ����.
		//	SKILL_ANI_CASTING_REPEAT_CNT(nI) = 1;

#ifdef	__SERVER
		// ���������� �ݺ� ����� ������� �ɽ��� ������ �ִϼӵ��� �ݺ� ��ǰ��� �����Ͽ� ����...
		// ���� :: �ɽ��� ����, �ɽ��� �ݺ� ������ ������ ���⿡ ������� �׻� ���� ����� ����Ѵ� !!
		// �ݺ� �ӵ� ���� �⺻�� 1.0f, 2.0f�� �ɰ�� ����� 2�� ���� ����, 0.5f�� �Ǹ� 2�� ������.
		if ( SKILL_ANI_CASTING_REPEAT_CNT(nI) ) 
		{
			tagMOTION *pMOTION = NULL;
			float fSpeed;
			DWORD dwTotTime;
			WORD wTotFrame;
			short nMotionTYPE=0;
			// �ݺ� ���
			for (short nM=0; nM<g_TblAniTYPE.m_nColCnt; nM++) {
				if ( FILE_MOTION( nM, SKILL_ANI_CASTING_REPEAT(nI) ) ) {
					nMotionTYPE = nM;
					break;
				}
			}

			pMOTION = g_MotionFILE.IDX_GetMOTION( FILE_MOTION( nMotionTYPE, SKILL_ANI_CASTING_REPEAT(nI) ), 0 );
			wTotFrame = pMOTION->Get_TotalFRAME () * SKILL_ANI_CASTING_REPEAT_CNT(nI);

			// ��� �ӵ�
			fSpeed = SKILL_ANI_CASTING_SPEED(nI) / 100.f;

			// �� �ݺ��� �ҿ�� �ð�...
			dwTotTime = pMOTION->Get_NeedTIME( wTotFrame, fSpeed );

			// �ɽ��� ���
			pMOTION = g_MotionFILE.IDX_GetMOTION( FILE_MOTION( nMotionTYPE, SKILL_ANI_CASTING(nI) ) , 0 );
			wTotFrame = pMOTION->Get_TotalFRAME ();

			// ��ü �ҿ�� �ð�..
			dwTotTime += pMOTION->Get_NeedTIME( wTotFrame, fSpeed );

			m_pCastingAniSPEED[ nI ] = ( 1000.f * wTotFrame ) / ( dwTotTime * pMOTION->m_wFPS );
		} 
		else 
		{
			m_pCastingAniSPEED[ nI ] = SKILL_ANI_CASTING_SPEED(nI) / 100.f;
		}

		// ��ų �ݺ����� ������ �ð�( ���� 1�� 0.2�� )
		m_pReloadTIME[ nI ] = SKILL_RELOAD_TIME(nI) * 200 - 100;	// 0.2f * 1000 :: 0.1�� ����~;
#else
		
		m_pCastingAniSPEED[ nI ] = SKILL_ANI_CASTING_SPEED(nI) / 100.f;

//---------------------------------------------------------------------------------------------
//2005. 5. 30. �� ��ȣ 
		//Ŭ���̾�Ʈ���� �����ð��� 1/5 �� �ð����� �ٲ۴�.	
		m_pReloadTIME[ nI ] = ((float)SKILL_RELOAD_TIME(nI) / 5.0f) * 1000.0f;
		 if ( m_pReloadTIME[ nI ] <= 0.f )
				m_pReloadTIME[ nI ] = 1.0f;	
//---------------------------------------------------------------------------------------------				 
#endif

		if ( m_pCastingAniSPEED[ nI ] <= 0.f ) {
			#ifndef	__SERVER
			// _ASSERT( m_pCastingAniSPEED[ nI ] > 0.f );
			#endif
			m_pCastingAniSPEED[ nI ] = 1.0f;		// ������� ����� ����...
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
// 2005. 5. 30 �� ��ȣ 
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