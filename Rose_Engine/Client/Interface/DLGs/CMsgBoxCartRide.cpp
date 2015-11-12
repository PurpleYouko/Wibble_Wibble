#include "stdafx.h"
#include ".\cmsgboxcartride.h"
#include "CObjCART.h"
#include "OBJECT.h"

CMsgBox_CartRide::CMsgBox_CartRide()
{
	m_wOwner = 0;
	m_wGuest = 0;
}

void CMsgBox_CartRide::Update(POINT ptMouse)
{
	CMsgBox::Update( ptMouse );

	//Find Index Owner.
	int iOwnerObjClientIndex = g_pObjMGR->Get_ClientObjectIndex( m_wOwner );
	CObjAVT* pOwnerAVT = g_pObjMGR->Get_CharAVT( iOwnerObjClientIndex, true );

	//Find Index Guest.
	int iGuestObjClientIndex = g_pObjMGR->Get_ClientObjectIndex( m_wGuest );
	CObjAVT* pGuestAVT = g_pObjMGR->Get_CharAVT( iGuestObjClientIndex, true );

	if( pOwnerAVT && pGuestAVT )
	{

#ifdef _DEBUG
		CStr::DebugString( "Dist %f \n", pOwnerAVT->Get_DISTANCE(pGuestAVT) );		
#endif	
		//카트 태우기 스킬 인덱스가 변경될 경우 아래의 인덱스 번호를 수정해야함.
		if( pOwnerAVT->Get_DISTANCE(pGuestAVT) > SKILL_SCOPE(19) )	//2인승 카트 skill Index = 19.
		{
			SendCommand( m_iInvokerDlgID, m_pCmdCancel );
			m_pCmdCancel = NULL;
			ClearCommand();
			Hide();
		}                
	}
}