#pragma once

#include "CItem.h"
#pragma warning (disable:4201)

#pragma pack (push, 1)

class CMileageInv
{
public:
	union
	{
		MileageItem		m_ItemLIST[ MAX_MILEAGE_INV_SIZE ];
		struct 
		{
			MileageItem	m_ItemEQUIP[ MAX_MILEAGE_EQUIP	];		// 착용 장비..
			MileageItem	m_ItemPAGE [ INVENTORY_PAGE_SIZE ];    //아이템 저장 영역..
		} ;
	} ;	
public:
	CMileageInv(void){};
	~CMileageInv(void){};

	short	AppendITEM (MileageItem &sITEM, short &nCurWEIGHT);
	short	AppendITEM (short nListNO, MileageItem &sITEM, short &nCurWEIGHT);
	void	DeleteITEM  (WORD wListRealNO);
	short	FindITEM (MileageItem &sITEM);

	void	SubtractITEM (short nLitNO, MileageItem &sITEM, short &nCurWEIGHT);

	short GetWEIGHT (short nListNO);
	void  SetInventory (short nListNO, int iItemNO, int iQuantity=1);

	short GetEmptyInventory();	
	short GetEmptyInvenSlotCount( );




};
#pragma pack (pop)

//-------------------------------------------------------------------------------------------------
#pragma warning (default:4201)