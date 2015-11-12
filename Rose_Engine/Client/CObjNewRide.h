#ifndef _OBJ_NEW_RIDE_
#define _OBJ_NEW_RIDE_

#include "cobjcart.h"


enum enumNEW_RIDE_ANI
{
	NEW_RIDE_ANI_STOP1 = 0,
	NEW_RIDE_ANI_MOVE,	
	NEW_RIDE_ANI_ATTACK01,	
	NEW_RIDE_ANI_ATTACK02,	
	NEW_RIDE_ANI_ATTACK03,	
	NEW_RIDE_ANI_DIE,	
	NEW_RIDE_ANI_CASTING,
	NEW_RIDE_ANI_SKILL,
};


//----------------------------------------------------------------------------------------------------
/// class CObjCastleGear
/// switch 문으로 CObjCART 로 처리 할수도 있었지만 차후 Castle Gear 만의 기능추가를 위해서 분리..
//----------------------------------------------------------------------------------------------------

class CObjNewRide :	public CObjCART
{
public:
	CObjNewRide(void);
	~CObjNewRide(void);

	//////////////////////////////////////////////////////////////////////////////////////////	
	/// < Inherited from CGameOBJ virtual functions
	/// <

	/*override*/virtual int		Get_TYPE()				{	return OBJ_NRIDE;					}	

	/// <
	/// < end	
	//////////////////////////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////////////////////////////	
	/// < Inherited from CObjAI virtual functions
	/// <

	/*override*/virtual int		GetANI_Stop();
	/*override*/virtual int		GetANI_Move();
	/*override*/virtual int		GetANI_Attack();
	/*override*/virtual int		GetANI_Die ();
	/*override*/virtual int		GetANI_Hit();
	/*override*/virtual int		GetANI_Casting();
	/*override*/virtual int		GetANI_Skill();

	
	/*override*/virtual void	SetCMD_ATTACK (int iServerTarget, WORD wSrvDIST, const D3DVECTOR& PosGOTO);	
	/// <
	/// < End
	//////////////////////////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////////////////////////////	
	/// < Inherited from CObjCHAR virtual functions
	/// <
	
	/// <
	/// < end	
	//////////////////////////////////////////////////////////////////////////////////////////


	virtual bool	Create( CObjCHAR* pParent, int iCartType, D3DVECTOR &Position );

};

#endif ///_OBJ_CASTLE_GEAR_