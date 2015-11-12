#include "stdafx.h"
#include ".\CObjNewRide.h"
#include "Common/IO_Pat.h"

CObjNewRide::CObjNewRide(void)
{
}

CObjNewRide::~CObjNewRide(void)
{
}

int CObjNewRide::GetANI_Stop()	{	return PAT_RELATIVE_MOTION_POS( m_nPartItemIDX[ RIDE_PART_BODY ] ) +  NEW_RIDE_ANI_STOP1;}
int CObjNewRide::GetANI_Move()	{	return PAT_RELATIVE_MOTION_POS( m_nPartItemIDX[ RIDE_PART_BODY ] ) +  NEW_RIDE_ANI_MOVE;	}
int	CObjNewRide::GetANI_Attack()	{	return PAT_RELATIVE_MOTION_POS( m_nPartItemIDX[ RIDE_PART_BODY ] ) +  NEW_RIDE_ANI_ATTACK01 + RANDOM(3);	}
int	CObjNewRide::GetANI_Die ()	{	return PAT_RELATIVE_MOTION_POS( m_nPartItemIDX[ RIDE_PART_BODY ] ) +  NEW_RIDE_ANI_DIE;	}
int	CObjNewRide::GetANI_Hit()	{	return PAT_RELATIVE_MOTION_POS( m_nPartItemIDX[ RIDE_PART_BODY ] ) +  NEW_RIDE_ANI_STOP1;	}	
int	CObjNewRide::GetANI_Casting(){	return 0;	}	
int	CObjNewRide::GetANI_Skill()	{	return 0;	}	


bool CObjNewRide::Create( CObjCHAR* pParent, int iCartType, D3DVECTOR &Position )
{
	if( CObjCART::Create( pParent, iCartType, Position ) )
	{
		///::setScale( this->GetZMODEL(), 1.2f, 1.2f, 1.2f );
		return true;
	}

	return false;
}

//----------------------------------------------------------------------------------------------------
/// @param
/// @brief /*override*/virtual bool	SetCMD_ATTACK ( int iServerTarget ); �� ������ �ʿ����.
///			CObjAI::SetCMD_ATTACK �� ���������� ȣ���ϴ� �Լ��̴�.
//----------------------------------------------------------------------------------------------------

void CObjNewRide::SetCMD_ATTACK (int iServerTarget, WORD wSrvDIST, const D3DVECTOR& PosGOTO)
{
	CObjCHAR::SetCMD_ATTACK( iServerTarget, wSrvDIST, PosGOTO );

	//----------------------------------------------------------------------------------------------------
	/// �� ž���� �� ĳ������ ����� ��ü�Ѵ�.
	//----------------------------------------------------------------------------------------------------	
	m_pObjParent->Set_MOTION( this->GetRideAniPos() + PETMODE_AVATAR_ANI_ATTACK01 + +RANDOM(3) );


	m_iOldCartState = m_iCurrentCartState;
	m_iCurrentCartState = CART_STATE_ATTACK;

	StopSound( m_iOldCartState );
	PlaySound( m_iCurrentCartState );
}


