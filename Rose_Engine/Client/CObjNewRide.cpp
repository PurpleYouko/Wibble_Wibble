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
/// @brief /*override*/virtual bool	SetCMD_ATTACK ( int iServerTarget ); 는 정의할 필요없다.
///			CObjAI::SetCMD_ATTACK 가 내부적으로 호출하는 함수이다.
//----------------------------------------------------------------------------------------------------

void CObjNewRide::SetCMD_ATTACK (int iServerTarget, WORD wSrvDIST, const D3DVECTOR& PosGOTO)
{
	CObjCHAR::SetCMD_ATTACK( iServerTarget, wSrvDIST, PosGOTO );

	//----------------------------------------------------------------------------------------------------
	/// 펫 탑승을 한 캐릭터의 모션을 교체한다.
	//----------------------------------------------------------------------------------------------------	
	m_pObjParent->Set_MOTION( this->GetRideAniPos() + PETMODE_AVATAR_ANI_ATTACK01 + +RANDOM(3) );


	m_iOldCartState = m_iCurrentCartState;
	m_iCurrentCartState = CART_STATE_ATTACK;

	StopSound( m_iOldCartState );
	PlaySound( m_iCurrentCartState );
}


