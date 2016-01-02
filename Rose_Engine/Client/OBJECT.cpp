/*
	$Header: /Client/OBJECT.cpp 78    07-01-12 10:40a Raning $
*/
#include "stdAFX.h"
#include "OBJECT.h"
#include "CObjMORPH.h"
#include "CObjGROUND.h"
#include "CObjCHAR.h"
//#include "CMouse.h"
#include "common\IO_AI.h"
#include "Game_FUNC.h"
#include "IO_Terrain.h"
#include "Interface/it_mgr.h"
#include "CObjCart.h"
#include "ObjCastleGear.h"
#ifdef __NEW_PAT_TYPES
#include ".\CObjNewRide.h"
#endif
#include "CViewMSG.h"
#include "ObjFixedEvent.h"



CObjectMANAGER* CObjectMANAGER::m_pInstance = NULL;


//-------------------------------------------------------------------------------------------------
// Client test function...
D3DVECTOR	g_FindPOS;
int			g_iFindDIST;
CAI_OBJ *AI_FindFirstOBJ( CAI_OBJ *pBaseOBJ, int iDistance )
{
	g_FindPOS   = ((CObjCHAR*)pBaseOBJ)->Get_CurPOS();
	g_iFindDIST = iDistance;
	
	CObjCHAR *pFindCHAR;
	pFindCHAR = g_pObjMGR->Get_FirstCHAR ();
	while( pFindCHAR ) {
		if ( pFindCHAR->Get_DISTANCE( g_FindPOS ) <= iDistance )
			return pFindCHAR;

		pFindCHAR = g_pObjMGR->Get_NextCHAR ();
	}

	return NULL;
}
CAI_OBJ *AI_FindNextOBJ ()
{
	CObjCHAR *pFindCHAR;
	pFindCHAR = g_pObjMGR->Get_NextCHAR ();
	while( pFindCHAR ) {
		if ( pFindCHAR->Get_DISTANCE( g_FindPOS ) <= g_iFindDIST )
			return pFindCHAR;

		pFindCHAR = g_pObjMGR->Get_NextCHAR ();
	}

	return NULL;
}
int AI_SysRANDOM(int iMod)
{
	return rand()%iMod;
}

//-------------------------------------------------------------------------------------------------
CObjectMANAGER::CObjectMANAGER () 
{
	m_iTotalObjCNT = 0;
	::ZeroMemory (m_iObjectCNT, sizeof(int) * OBJ_MAX_TYPE);

	m_iEmptySlotNo = MIN_OBJECTS;

	::ZeroMemory( m_nServer2ClientOBJ, sizeof(short) * MAX_SERVER_OBJECTS );
	::ZeroMemory( m_wClient2ServerOBJ, sizeof(WORD)  * MAX_CLIENT_OBJECTS );

	::ZeroMemory( m_pOBJECTS, sizeof(CGameOBJ*) * MAX_CLIENT_OBJECTS );

#ifdef __NPC_COLLISION
	
	m_iCurrentCollisionNPCState = 0; // NPC in conflict stops: 0, held in: 1, open state: 2

#endif

}

//-------------------------------------------------------------------------------------------------
CObjectMANAGER::~CObjectMANAGER ()
{
	for (int iL=0; iL<MAX_CLIENT_OBJECTS; iL++) {
		SAFE_DELETE( m_pOBJECTS[ iL ] );
	}	

	//Aroa data initialization bakjiho ::
	goddessMgr.Release();

}
CObjectMANAGER* CObjectMANAGER::Instance ()
{
	if ( !m_pInstance ) {
		m_pInstance = new CObjectMANAGER; 
		CObjMOB::m_dwCreateOrder = 0;
		CObjFIXED::m_dwCreateOrder = 0;
	}

	return m_pInstance;
}
void CObjectMANAGER::Destroy ()
{
	SAFE_DELETE( m_pInstance );
}

//-------------------------------------------------------------------------------------------------
int CObjectMANAGER::Get_EmptySlot ()
{
	static int iS;
	for (iS=m_iEmptySlotNo; iS<MAX_CLIENT_OBJECTS; iS++) {
		if ( NULL == m_pOBJECTS[ iS ] ) {
			m_iEmptySlotNo = iS;
			return iS;
		}
	}

	for (iS=MIN_OBJECTS; iS<m_iEmptySlotNo; iS++) {
		if ( NULL == m_pOBJECTS[ iS ] ) {
			m_iEmptySlotNo = iS;
			return iS;
		}
	}

	// no empty slot
	return 0;
}

//-------------------------------------------------------------------------------------------------
void CObjectMANAGER::Set_ServerObjectIndex (short nClientObjectIndex, WORD wServerObjectIndex)
{
	_ASSERT( !m_nServer2ClientOBJ[ wServerObjectIndex ] );

	m_wClient2ServerOBJ[ nClientObjectIndex ] = wServerObjectIndex;
	m_nServer2ClientOBJ[ wServerObjectIndex ] = nClientObjectIndex;
}

bool CObjectMANAGER::Set_EmptySlot (short nSlotNO, WORD wServerObjectIndex, CGameOBJ *pObject)
{
	_ASSERT( m_pOBJECTS[ nSlotNO ] == NULL );

	if ( NULL == m_pOBJECTS[ nSlotNO ] ) 
	{
		pObject->m_nIndex = nSlotNO;
		m_pOBJECTS[ nSlotNO ] = pObject;
		m_iTotalObjCNT ++;
		m_iObjectCNT[ pObject->Get_TYPE() ] ++;

		switch( pObject->Get_TYPE() )
		{
			case OBJ_ITEM:
				pObject->m_pListNODE = m_ItemLIST.AllocNAppend( nSlotNO );				
				break;
			case OBJ_MORPH:			
			case OBJ_GROUND:
			case OBJ_CNST:
				pObject->m_pListNODE = m_CnstLIST.AllocNAppend( nSlotNO );

				break;

			case OBJ_NPC:		// OBJ_NPC >= character object ...
			case OBJ_MOB:				
			case OBJ_AVATAR:				
			case OBJ_USER:
			case OBJ_CART:
			case OBJ_CGEAR:
			case OBJ_NRIDE:
				{
					pObject->m_pListNODE = m_CharLIST.AllocNAppend( nSlotNO );

					CObjCHAR *pCHAR = (CObjCHAR*) pObject;					
					pCHAR->Set_AdjustSPEED(pCHAR->Get_DefaultSPEED());

		#ifndef __VIRTUAL_SERVER
					_ASSERT( pCHAR->m_fAdjustSPEED >= 0.f && pCHAR->m_fAdjustSPEED < 2000.f );
		#endif
				}
				break;		

			case OBJ_EVENTOBJECT:				
				CObjFixedEvent* pEventObj = (CObjFixedEvent*)pObject;
				m_EventObjectList.insert( std::make_pair< int, CObjFixedEvent* >( pEventObj->GetUniqueEventID(), pEventObj ) );
				break;
		}
	

#ifdef	__VIRTUAL_SERVER
		if ( !wServerObjectIndex )
			wServerObjectIndex = nSlotNO;
#endif

		m_wClient2ServerOBJ[ nSlotNO ] = wServerObjectIndex;
		if ( wServerObjectIndex ) {
			// 0, then the object is not received from the server, or do not receive it yet.
			m_nServer2ClientOBJ[ wServerObjectIndex ] = nSlotNO;
		}

		return true;
	}

	return false;
}

//-------------------------------------------------------------------------------------------------
void CObjectMANAGER::Clear (short nExceptObjIndex)
{
//	WORD wMyServerObjIDX = m_wClient2ServerOBJ[ nExceptObjIndex ];

	for (short nI=MIN_OBJECTS; nI<MAX_CLIENT_OBJECTS; nI++) {
		if ( nExceptObjIndex != nI )
			Del_Object( nI );
	}

	::ZeroMemory( m_nServer2ClientOBJ, sizeof(short) * MAX_SERVER_OBJECTS );
	::ZeroMemory( m_wClient2ServerOBJ, sizeof(WORD)  * MAX_CLIENT_OBJECTS );
/*
	if ( nExceptObjIndex ) {
		_ASSERT( wMyServerObjIDX == m_wClient2ServerOBJ[ nExceptObjIndex ] );
	}
*/
	///2004/5/
	ClearNpcInfoList();
		
}

void CObjectMANAGER::Del_Object (short nIndex)
{
	Del_Object( m_pOBJECTS[ nIndex ] );
}
void CObjectMANAGER::Del_Object (CGameOBJ *pObject)
{
	if ( NULL == pObject ) 
		return;

	m_nServer2ClientOBJ[ m_wClient2ServerOBJ[ pObject->m_nIndex ] ] = 0;
	m_wClient2ServerOBJ[ pObject->m_nIndex ] = 0;

	m_pOBJECTS[ pObject->m_nIndex ] = NULL;
	m_iTotalObjCNT --;
	m_iObjectCNT[ pObject->Get_TYPE() ] --;

	pObject->RemoveFromScene ();

	

	switch( pObject->Get_TYPE() ) 
	{
		case OBJ_MORPH :
		{			
			m_CnstLIST.DeleteNFree( pObject->m_pListNODE );

			delete pObject;
			break;
		}
		case OBJ_ITEM :
		{
			m_ItemLIST.DeleteNFree( pObject->m_pListNODE );
			
			delete pObject;
			break;
		}
		case OBJ_GROUND:
			{
				m_CnstLIST.DeleteNFree( pObject->m_pListNODE );

				///LogString (LOG_NORMAL, "Del_Object... OBJ_GROUND: %d\n", m_iObjectCNT[ OBJ_GROUND ] );
				
				delete pObject;
				break;
			}
		case OBJ_CNST  :
			{
				m_CnstLIST.DeleteNFree( pObject->m_pListNODE );
				
				delete pObject;
				break;
			}
		case OBJ_NPC   :
			{
				m_CharLIST.DeleteNFree( pObject->m_pListNODE );

				CObjNPC *pNPC = (CObjNPC*) pObject;
				delete pNPC;
				break;
			}
		case OBJ_MOB   :
			{
				m_CharLIST.DeleteNFree( pObject->m_pListNODE );

				CObjMOB *pMOB = (CObjMOB*) pObject;
	#ifdef	__VIRTUAL_SERVER
				if ( pMOB->m_pRegenPOINT )
					pMOB->m_pRegenPOINT->ClearCharacter( pMOB );
	#endif
				delete pMOB;
				break;
			}
		case OBJ_AVATAR :
			{
				m_CharLIST.DeleteNFree( pObject->m_pListNODE );

				CObjAVT *pAVT = (CObjAVT*) pObject;
				delete pAVT;
				break;
			}
		case OBJ_USER :
			{
				m_CharLIST.DeleteNFree( pObject->m_pListNODE );
				CObjUSER *pUSER = (CObjUSER*) pObject;
				pUSER->Destroy ();
			}
			break;
		case OBJ_CART:
		case OBJ_CGEAR:
		case OBJ_NRIDE:
			{
				m_CharLIST.DeleteNFree( pObject->m_pListNODE );				
				delete pObject;
				break;
			}
			break;

		case OBJ_EVENTOBJECT:
			{
				CObjFixedEvent* pEventObj = (CObjFixedEvent*)pObject;
				m_EventObjectList.erase( pEventObj->GetUniqueEventID() );
				
				delete pObject;
				break;
			}
			break;

		default:
			LogString (LOG_DEBUG, "Del_Object : Invalid object type \n");
			break;
	}
}


//-------------------------------------------------------------------------------------------------
int CObjectMANAGER::Add_GndITEM( WORD wServerObjectIndex, tagITEM &sITEM, tPOINTF &PosXY, float fRefZPos, bool bUseRefZPos )
{
	int iObjSlot;

	iObjSlot = Get_EmptySlot ();
	if ( !iObjSlot ) {
		LogString (LOG_DEBUG, "Out of object slot ... in add ground tree \n");
		return 0;
	}

	CObjITEM *pITEM = new CObjITEM;
	if ( NULL == pITEM ) {
		LogString (LOG_DEBUG, "Out of memory ... in add ground tree \n");
		return 0;
	}
	
	D3DVECTOR PosSET;

	PosSET.x = PosXY.x;
	PosSET.y = PosXY.y;	

	//if( !bUseRefZPos )
	if( 1 )
		PosSET.z = g_pTerrain->GetHeightTop( PosXY.x, PosXY.y );
	else
		PosSET.z = fRefZPos;

	if ( !pITEM->Create( g_DATA.m_ModelFieldITEM.GetMODEL( sITEM.GetModelINDEX() ), PosSET, true) ) {
		delete pITEM;
		return 0;
	} 

	
	pITEM->m_ITEM = sITEM;

	this->Set_EmptySlot( iObjSlot, wServerObjectIndex, pITEM );

	::setUserData( pITEM->GetRootZNODE(), (HNODE)pITEM );
	
	_ASSERT( ::getUserData( pITEM->GetRootZNODE() ) );
	
	return iObjSlot;
}

//-------------------------------------------------------------------------------------------------
int	CObjectMANAGER::Add_MORPHER( short nObjID, D3DVECTOR &Position, D3DXQUATERNION &Rotate, D3DVECTOR &Scale )
{
	int iObjSlot;

	iObjSlot = Get_EmptySlot ();
	if ( !iObjSlot ) {
		LogString (LOG_DEBUG, "Out of object slot ... in add ground tree \n");
		return 0;
	}

	CObjMORPH *pMORPH = new CObjMORPH;
	if ( NULL == pMORPH ) {
		LogString (LOG_DEBUG, "Out of memory ... in add morpher object \n");
		return 0;
	}
	
	if ( !pMORPH->Create( nObjID, Position ) ) {
		delete pMORPH;
		return 0;
	} 

	pMORPH->m_nMorphIDX = nObjID;
	pMORPH->Scale( Scale );
	pMORPH->Rotate( Rotate );
	this->Set_EmptySlot( iObjSlot, 0, pMORPH );

	return iObjSlot;
}

//-------------------------------------------------------------------------------------------------
int CObjectMANAGER::Add_GndTREE( short nTreeIdx, D3DVECTOR &Position, D3DXQUATERNION &Rotate, D3DVECTOR &Scale )
{
	int iObjSlot;

	iObjSlot = Get_EmptySlot ();
	if ( !iObjSlot ) {
		LogString (LOG_DEBUG, "Out of object slot ... in add ground tree \n");
		return 0;
	}

	CObjTREE *pTREE = new CObjTREE;
	if ( NULL == pTREE ) {
		LogString (LOG_DEBUG, "Out of memory ... in add ground tree \n");
		return 0;
	}
	
	if ( !pTREE->Create( g_DATA.m_ModelDECO.GetMODEL( nTreeIdx ), Position, true) ) {
		delete pTREE;
		return 0;
	} 

	::setUserData( pTREE->GetRootZNODE(), (HNODE)pTREE );

	pTREE->m_nTreeIdx = nTreeIdx;
	pTREE->Scale( Scale );
	pTREE->Rotate( Rotate );
	this->Set_EmptySlot( iObjSlot, 0, pTREE );

	return iObjSlot;
}

//-------------------------------------------------------------------------------------------------
int CObjectMANAGER::Add_GndCNST( short nCnstIdx, D3DVECTOR &Position, D3DXQUATERNION &Rotate, D3DVECTOR &Scale )
{
	int iObjSlot;

	iObjSlot = Get_EmptySlot ();
	if ( !iObjSlot ) {
		LogString (LOG_DEBUG, "Out of object slot ... in add ground cnst \n");
		return 0;
	}

	CObjCNST *pCNST = new CObjCNST;
	if ( NULL == pCNST ) {
		LogString (LOG_DEBUG, "Out of memory ... in add ground cnst \n");
		return 0;
	}

	if ( !pCNST->Create( g_DATA.m_ModelCNST.GetMODEL( nCnstIdx ), Position, true) ) {
		delete pCNST;
		return 0;
	}

	pCNST->m_nCnstIdx = nCnstIdx;
	LogString (LOG_DEBUG, "Add_GndCNST( %d ) %f, %f \n", nCnstIdx, Position.x, Position.y);

	::setUserData( pCNST->GetRootZNODE(), (HNODE)pCNST );

	pCNST->Scale( Scale );
	pCNST->Rotate (Rotate);
	this->Set_EmptySlot( iObjSlot, 0, pCNST );

	return iObjSlot;
}

//-------------------------------------------------------------------------------------------------
int CObjectMANAGER::Add_MobCHAR( WORD wServerObjectIndex, short nCharIdx, const D3DVECTOR &Position, short nQuestIDX, bool bRunMODE, short nMonLevel, short nMonSize)
{
/*
	if ( nCharIdx == 46 )	// Gorilla General -> Stony
		nCharIdx = 48;
*/

	int iObjSlot;

	iObjSlot = Get_EmptySlot ();
	if ( !iObjSlot ) 
	{
		LogString (LOG_DEBUG, "Out of object slot ... in add mob char \n");
		return 0;
	}

	CObjMOB *pCHAR = new CObjMOB;
	if ( NULL == pCHAR ) 
	{
		LogString (LOG_DEBUG, "Out of memory ... in add mob char \n");
		return 0;
	}
	
	if ( !pCHAR->Create( nCharIdx, Position, nQuestIDX, bRunMODE, nMonLevel, nMonSize) ) 
	{
		LogString (LOG_DEBUG, "MOB Char[ %d ] create failed \n", nCharIdx);
		delete pCHAR;
		return 0;
	}

	::setUserData( pCHAR->GetZMODEL(), (HNODE)pCHAR );
	_ASSERT( ::getUserData( pCHAR->GetZMODEL() ) );

#ifdef	__VIRTUAL_SERVER
	g_AI_LIST.AI_Created ( NPC_AI_TYPE( nCharIdx ), pCHAR );
#endif

//	LogString (LOG_DEBUG, "Add_MobCHAR:: %d \n", nCharIdx);

	this->Set_EmptySlot( iObjSlot, wServerObjectIndex, pCHAR );
	///ON_CharAppear( iObjSlot );

	return iObjSlot;
}

/// Warp object Ãß°¡
int	CObjectMANAGER::Add_WARP( short nEventID, D3DVECTOR &Position, D3DXQUATERNION &Rotate, D3DVECTOR &Scale )
{
	int iObjSlot;

	iObjSlot = Get_EmptySlot ();
	if ( !iObjSlot ) {
		LogString (LOG_DEBUG, "Out of object slot ... in add ground tree \n");
		return 0;
	}

	CObjTREE *pTREE = new CObjTREE;
	if ( NULL == pTREE ) {
		LogString (LOG_DEBUG, "Out of memory ... in add ground tree \n");
		return 0;
	}
	
	if ( !pTREE->Create( g_DATA.m_ModelSpecial.GetMODEL( SPECIAL_WARP_OBJECT ), Position, true) ) {
		delete pTREE;
		return 0;
	} 

	pTREE->SetSpecialUserNumericData( nEventID + WARP_OBJECT_INDEX_OFFSET );

	::setUserData( pTREE->GetRootZNODE(), (HNODE)pTREE );

	pTREE->m_nTreeIdx = SPECIAL_WARP_OBJECT;
	pTREE->Scale( Scale );
	pTREE->Rotate( Rotate );
	this->Set_EmptySlot( iObjSlot, 0, pTREE );

	return iObjSlot;
}

/// Collision object More
int	CObjectMANAGER::Add_CollisionBox( int iReserved, D3DVECTOR &Position, D3DXQUATERNION &Rotate, D3DVECTOR &Scale )
{
	int iObjSlot;

	iObjSlot = Get_EmptySlot ();
	if ( !iObjSlot ) {
		LogString (LOG_DEBUG, "Out of object slot ... in add ground tree \n");
		return 0;
	}

	CObjTREE *pTREE = new CObjTREE;
	if ( NULL == pTREE ) {
		LogString (LOG_DEBUG, "Out of memory ... in add ground tree \n");
		return 0;
	}

	if ( !pTREE->Create( g_DATA.m_ModelSpecial.GetMODEL( SPECIAL_COLLISION_OBJECT ), Position, true) ) {
		delete pTREE;
		return 0;
	} 
	
	pTREE->m_nTreeIdx = SPECIAL_COLLISION_OBJECT;
	pTREE->Scale( Scale );
	pTREE->Rotate( Rotate );
	this->Set_EmptySlot( iObjSlot, 0, pTREE );

	return iObjSlot;
}

//-------------------------------------------------------------------------------------------------
int CObjectMANAGER::Add_NpcCHAR( WORD wServerObjectIndex, short nCharIdx, const D3DVECTOR &Position, short nQuestIDX, float fModelDIR )
{
	int iObjSlot;

	iObjSlot = Get_EmptySlot ();
	if ( !iObjSlot ) {
		LogString (LOG_DEBUG, "Out of object slot ... in add npc char \n");
		return 0;
	}

	CObjNPC *pCHAR = new CObjNPC;
	if ( NULL == pCHAR ) {
		LogString (LOG_DEBUG, "Out of memory ... in add mob npc \n");
		return 0;
	}
	
	if ( !pCHAR->Create( nCharIdx, Position, nQuestIDX, (NPC_WALK_SPEED( nCharIdx )==0)?false:true) ) {
		LogString (LOG_DEBUG, "mob char create failed \n");
		delete pCHAR;
		return 0;
	}
#ifdef	__VIRTUAL_SERVER
	g_AI_LIST.AI_Created ( NPC_AI_TYPE( nCharIdx ), pCHAR );
#endif

	this->Set_EmptySlot( iObjSlot, wServerObjectIndex, pCHAR );
	///ON_CharAppear( iObjSlot );

	::setModelDirection( pCHAR->GetZMODEL(), fModelDIR, false );
	int iRet = ::setUserData( pCHAR->GetZMODEL(), (HNODE)pCHAR );
	_ASSERT( iRet );

	return iObjSlot;
}

//----------------------------------------------------------------------------------------------------
/// @param
/// @brief user char, avt char Here, because no generation of common userdata model setting is impossible.
//----------------------------------------------------------------------------------------------------

CObjUSER* CObjectMANAGER::New_UserCHAR (WORD wServerObjectIndex, char *szName)
{
	int iObjSlot;

	iObjSlot = Get_EmptySlot ();
	if ( !iObjSlot ) {
		LogString (LOG_DEBUG, "Out of object slot ... in add avatar char \n");
		return NULL;
	}

	CObjUSER *pCHAR = CObjUSER::Instance ();
	if ( NULL == pCHAR ) {
		LogString (LOG_DEBUG, "Out of memory ... in add avatar char \n");
		return NULL;
	}
	pCHAR->SetAvataName (szName);

	this->Set_EmptySlot( iObjSlot, wServerObjectIndex, pCHAR );
	
	///int iRet = ::setUserData( pCHAR->GetZMODEL(), (HNODE)pCHAR );

	return pCHAR;
}

//----------------------------------------------------------------------------------------------------
/// @param
/// @brief user char, avt char Here, because no generation of common userdata model setting is impossible.
//----------------------------------------------------------------------------------------------------
CObjAVT* CObjectMANAGER::New_AvtCHAR (WORD wServerObjectIndex, char *szName)
{
	int iObjSlot;

	iObjSlot = Get_EmptySlot ();
	if ( !iObjSlot ) {
		LogString (LOG_DEBUG, "Out of object slot ... in add avatar char \n");
		return NULL;
	}

	CObjAVT *pCHAR = new CObjAVT;
	if ( NULL == pCHAR ) {
		LogString (LOG_DEBUG, "Out of memory ... in add avatar char \n");
		return NULL;
	}
	pCHAR->SetAvataName (szName);

	this->Set_EmptySlot( iObjSlot, wServerObjectIndex, pCHAR );
	
	///int iRet = ::setUserData( pCHAR->GetZMODEL(), (HNODE)pCHAR );

	return pCHAR;
}

//----------------------------------------------------------------------------------------------------
/// @param
/// @brief user char, avt char Here, because no generation of common userdata model setting is impossible.
//----------------------------------------------------------------------------------------------------
CObjCART* CObjectMANAGER::Add_CartCHAR( int iPetType, CObjAVT* pOwner, WORD wServerObjectIndex )
{
	int iObjSlot;

	iObjSlot = Get_EmptySlot ();
	if ( !iObjSlot ) 
	{
		g_itMGR.OpenMsgBox( "IS NOT EMPTY OBJECT SLOT" );
		LogString (LOG_DEBUG, "Out of object slot ... in Add_CartCHAR \n");
		return NULL;
	}

	CObjCART *pCHAR = NULL;

	//----------------------------------------------------------------------------------------------------
	/// Pet type Generated according to the appropriate class.
	//----------------------------------------------------------------------------------------------------
	// 06. 12. 18 - Kim, Joo - Hyun: Cart type extension.
	switch( iPetType )
	{
		case PET_TYPE_CART01:
		case PET_TYPE_CART02:
		case PET_TYPE_CART03:
		case PET_TYPE_CART04:
		case PET_TYPE_CART05:
		case PET_TYPE_CART06:
		case PET_TYPE_CART07:
		case PET_TYPE_CART08:
		case PET_TYPE_CART09:
			pCHAR = new CObjCART();
			break;
		case PET_TYPE_CASTLE_GEAR01:
		case PET_TYPE_CASTLE_GEAR02:
		case PET_TYPE_CASTLE_GEAR03:
		case PET_TYPE_CASTLE_GEAR04:
		case PET_TYPE_CASTLE_GEAR05:
		case PET_TYPE_CASTLE_GEAR06:
		case PET_TYPE_CASTLE_GEAR07:
		case PET_TYPE_CASTLE_GEAR08:
		case PET_TYPE_CASTLE_GEAR09:
			pCHAR = new CObjCastleGear();
			break;
		case PET_TYPE_NEW_RIDE01:
		case PET_TYPE_NEW_RIDE02:
		case PET_TYPE_NEW_RIDE03:
		case PET_TYPE_NEW_RIDE04:
		case PET_TYPE_NEW_RIDE05:
		case PET_TYPE_NEW_RIDE06:
		case PET_TYPE_NEW_RIDE07:
		case PET_TYPE_NEW_RIDE08:
		case PET_TYPE_NEW_RIDE09:
#ifdef __NEW_PAT_TYPES
			pCHAR = new CObjNewRide();
#endif
			break;
	}

	if ( NULL == pCHAR ) {
		LogString (LOG_DEBUG, "Out of memory ... in Add_CartCHAR \n");
		return NULL;
	}
	
	this->Set_EmptySlot( iObjSlot, wServerObjectIndex, pCHAR );	

	///int iRet = ::setUserData( pCHAR->GetZMODEL(), (HNODE)pCHAR );

	return pCHAR;
}

//-------------------------------------------------------------------------------------------------
/// Event object More
//-------------------------------------------------------------------------------------------------
int CObjectMANAGER::Add_FixedEvent( int iEventID, short nObjID, 
								   const char* pTriggerName,
								   const char* pScriptName,
								   D3DVECTOR &Position, D3DXQUATERNION &Rotate, D3DVECTOR &Scale )
{
	int iObjSlot;

	iObjSlot = Get_EmptySlot ();
	if ( !iObjSlot ) {
		LogString (LOG_DEBUG, "Out of object slot ... in add ground tree \n");
		return 0;
	}

	CObjFixedEvent *pObj = new CObjFixedEvent;
	if ( NULL == pObj ) {
		LogString (LOG_DEBUG, "Out of memory ... in add event object \n");
		return 0;
	}
	
	if ( !pObj->Create( g_DATA.m_ModelEvent.GetMODEL( nObjID ), Position, true) ) {
		delete pObj;
		return 0;
	} 

	::setUserData( pObj->GetRootZNODE(), (HNODE)pObj );


	pObj->SetTriggerName( const_cast< char* >(pTriggerName) );
	pObj->SetScriptName( const_cast< char* >(pScriptName) );

	pObj->SetUniqueEventID( iEventID );
	pObj->m_nTreeIdx = nObjID;
	pObj->Scale( Scale );
	pObj->Rotate( Rotate );

	this->Set_EmptySlot( iObjSlot, 0, pObj );

	return iObjSlot;
}




//-------------------------------------------------------------------------------------------------
bool CObjectMANAGER::Add_AvtCHAR(CObjAVT *pAvtCHAR, D3DVECTOR &Position, BYTE btCharRACE)
{
	if ( NULL == pAvtCHAR )
		return false;

	if ( pAvtCHAR->Create( Position, btCharRACE ) ) 
	{
		::setUserData( pAvtCHAR->GetZMODEL(), (HNODE)pAvtCHAR );
		_ASSERT( ::getUserData( pAvtCHAR->GetZMODEL() ) );
		return true;
	} 

	
	// Perhaps ... because a duplicate character names should not be generated if the engine hModelNODE ...
	g_itMGR.OpenMsgBox( "Duplicate character names" );

	return false;
}

//-------------------------------------------------------------------------------------------------
CGameOBJ *CObjectMANAGER::Get_OBJECT (int iObjectIndex, t_ObjTAG ObjTYPE)
{
	if ( iObjectIndex >= MIN_OBJECTS && iObjectIndex < MAX_CLIENT_OBJECTS ) {
		CGameOBJ *pObject = (CObjCHAR *)g_pObjMGR->m_pOBJECTS[ iObjectIndex ];
		if ( pObject ) {
			if ( ObjTYPE ) {
				if ( pObject->IsA( ObjTYPE ) )
					return pObject;

				return NULL;
			}
			return pObject;
		}
	}

	return NULL;
}

//-------------------------------------------------------------------------------------------------
CObjCHAR*CObjectMANAGER::Get_CharOBJ (int iObjectIndex, bool bCheckHP)
{
	if ( iObjectIndex >= MIN_OBJECTS && iObjectIndex < MAX_CLIENT_OBJECTS ) 
	{
		CObjCHAR *pObject = (CObjCHAR *)g_pObjMGR->m_pOBJECTS[ iObjectIndex ];
		if ( pObject ) 
		{
			if ( pObject->IsCHAR() ) 
			{
				
				if ( bCheckHP && pObject->Get_HP() <= 0 )
				{
					return NULL;
				}					
				return pObject;
			}
		}
	}

	return NULL;
}

CObjCHAR*	CObjectMANAGER::Get_ClientCharOBJByName( const char* szName )
{
	if( szName == NULL )
		return NULL;
	CObjCHAR* pObject = NULL;
	for( int i = MIN_OBJECTS; i < MAX_CLIENT_OBJECTS; ++i )
	{
		pObject = (CObjCHAR *)g_pObjMGR->m_pOBJECTS[ i ];
		if( pObject && pObject->IsCHAR() )
		{
			///In a case-insensitive comparison.
			if( pObject->Get_NAME() && _stricmp( pObject->Get_NAME(), szName ) == 0 )
				return pObject;
		}
	}
	return NULL;
}

///STB INDEX A character object is obtained.
CObjCHAR*	CObjectMANAGER::Get_ClientCharOBJBySTBIndex( int iStbIndex )
{
	if( iStbIndex < 0 )		return NULL;

	CObjCHAR* pObject = NULL;
	for( int i = MIN_OBJECTS; i < MAX_CLIENT_OBJECTS; ++i )
	{
		pObject = (CObjCHAR *)g_pObjMGR->m_pOBJECTS[ i ];
		if( pObject && pObject->IsNPC() )
		{			
			if( pObject->Get_CharNO() == iStbIndex )
			{
				return pObject;
			}
		}
	}
	return NULL;
}

//-------------------------------------------------------------------------------------------------
CObjAVT*CObjectMANAGER::Get_CharAVT (int iObjectIndex, bool bCheckHP)
{
	if ( iObjectIndex >= MIN_OBJECTS && iObjectIndex < MAX_CLIENT_OBJECTS ) {
		CObjCHAR *pObject = (CObjCHAR *)g_pObjMGR->m_pOBJECTS[ iObjectIndex ];
		if ( pObject ) {
			if ( pObject->IsUSER () ) {
				if ( bCheckHP && pObject->Get_HP() <= 0 )
					return NULL;
				
				return reinterpret_cast<CObjAVT*>(pObject);
			}
		}
	}

	return NULL;
}
//-------------------------------------------------------------------------------------------------
CObjITEM*CObjectMANAGER::Get_ItemOBJ (int iObjectIndex)
{
	if ( iObjectIndex >= MIN_OBJECTS && iObjectIndex < MAX_CLIENT_OBJECTS ) {
		CGameOBJ *pObject = (CObjITEM *)g_pObjMGR->m_pOBJECTS[ iObjectIndex ];
		if ( pObject ) {
			if ( pObject->IsA( OBJ_ITEM ) )
				return reinterpret_cast<CObjITEM*>(pObject);
		}
	}

	return NULL;
}


//-------------------------------------------------------------------------------------------------
char *CObjectMANAGER::Get_NAME (int iTargetObjectIndex)
{
	if ( iTargetObjectIndex > 0 ) {
		CGameOBJ*pObject = g_pObjMGR->m_pOBJECTS[ iTargetObjectIndex ];
		if ( pObject ) {
			return pObject->Get_NAME ();
		}
	}

	return NULL;
}

//-------------------------------------------------------------------------------------------------
void CObjectMANAGER::InsertToScene ()
{
	for (int iO=MIN_OBJECTS; iO<MAX_CLIENT_OBJECTS; iO++) {
		if ( NULL == m_pOBJECTS[ iO ] )
			continue;

		m_pOBJECTS[ iO ]->InsertToScene ();
	}
}
void CObjectMANAGER::RemoveFromScene ()
{
	for (int iO=MIN_OBJECTS; iO<MAX_CLIENT_OBJECTS; iO++) {
		if ( NULL == m_pOBJECTS[ iO ] )

			continue;

		m_pOBJECTS[ iO ]->RemoveFromScene ();
	}
}

//-------------------------------------------------------------------------------------------------
CObjCHAR*CObjectMANAGER::Get_FirstCHAR ()
{
	CObjCHAR *pFindCHAR;

	m_pFindNODE = m_CharLIST.GetHeadNode ();
	while( m_pFindNODE ) {
		pFindCHAR = this->Get_CharOBJ( m_pFindNODE->DATA, true );
		if ( pFindCHAR )
			return pFindCHAR;

		m_pFindNODE = m_CharLIST.GetNextNode( m_pFindNODE );
	} 
	return NULL;
}
CObjCHAR*CObjectMANAGER::Get_NextCHAR ()
{
	if ( NULL == m_pFindNODE )
		return NULL;

	CObjCHAR *pFindCHAR;

	m_pFindNODE = m_CharLIST.GetNextNode( m_pFindNODE );
	while( m_pFindNODE ) {
		pFindCHAR = this->Get_CharOBJ( m_pFindNODE->DATA, true );
		if ( pFindCHAR )
			return pFindCHAR;

		m_pFindNODE = m_CharLIST.GetNextNode( m_pFindNODE );
	}

	return NULL;
}
//-------------------------------------------------------------------------------------------------

void CObjectMANAGER::ProcOBJECT ()
{
/*
	static int iO;
	for (iO=MIN_OBJECTS; iO<MAX_CLIENT_OBJECTS; iO++) {
		if ( NULL == m_pOBJECTS[ iO ] )
			continue;

		m_pOBJECTS[ iO ]->Proc ();
	}
*/
	DWORD dwTIME;

	dwTIME = g_GameDATA.GetGameTime();

	m_dwPassTIME = dwTIME - m_dwCurTIME;
	m_dwCurTIME  = dwTIME;

	//----------------------------------------------------------------------------------------------------	
	/// @brief Char list loop
	//----------------------------------------------------------------------------------------------------
	classDLLNODE< int > *pNode;
	pNode = m_CharLIST.GetHeadNode ();
	while( pNode ) 
	{
		if ( m_pOBJECTS[ pNode->DATA ] ) 
		{			
			if ( !m_pOBJECTS[ pNode->DATA ]->Proc () ) {
				classDLLNODE< int > *pDelNode;
				pDelNode = pNode;
				pNode = m_CharLIST.GetNextNode( pNode );

				this->Del_Object( pDelNode->DATA );

				continue;
			}			
		}

		pNode = m_CharLIST.GetNextNode( pNode );
	} 

	// 05.05.19 icarus:: Quest for the update process is complete, paying so Fused ..
	g_pAVATAR->m_bQuestUpdate = false;


	//----------------------------------------------------------------------------------------------------	
	/// @brief Item list loop
	//----------------------------------------------------------------------------------------------------	
	g_pViewMSG->ResetItemInfoView();

	/// alt- If the key is pressed
	if( g_GameDATA.m_bShowDropItemInfo )
	{
		pNode = m_ItemLIST.GetHeadNode ();
		while( pNode ) 
		{
			if ( m_pOBJECTS[ pNode->DATA ] ) 
			{			
				m_pOBJECTS[ pNode->DATA ]->Proc ();			
			}

			pNode = m_ItemLIST.GetNextNode( pNode );
		}
	}	
}


//--------------------------------------------------------------------------------
/// @param const CMAP* pMap	  - CMAP is loaded into the memory pointer npc
/// @param const int iNpcNo   - The STB No NPC
/// @param D3DVECTOR Position - Location Information
/// @brief  : NPC displayed on the mini-map can be added to the list information.
//--------------------------------------------------------------------------------

void CObjectMANAGER::AddNpcInfo( CMAP* pMap, int iNpcNo, D3DVECTOR Position )
{
	NpcInfoInMap Info;
	Info.m_pMap		= pMap;
	Info.m_iNpcID	= iNpcNo;
	Info.m_Position = Position;
	m_listNpcInfo.push_back( Info );
}
//--------------------------------------------------------------------------------
/// @param const CMAP* pMap	  - CMAP is loaded into the memory pointer npc
/// @brief  : CMAP::Free yourself from the load at the NPC to remove it.
//--------------------------------------------------------------------------------
void CObjectMANAGER::RemoveNpcInfo( CMAP* pMap )
{
	std::list< NpcInfoInMap >::iterator iter;
	for( iter = m_listNpcInfo.begin(); iter != m_listNpcInfo.end(); )
	{
		if( iter->m_pMap == pMap )
			iter = m_listNpcInfo.erase( iter );
		else
			iter++;
	}
}
//--------------------------------------------------------------------------------
/// @brief  : Erase all the npc info list.
//--------------------------------------------------------------------------------
void CObjectMANAGER::ClearNpcInfoList()
{
	m_listNpcInfo.clear();
}
//--------------------------------------------------------------------------------
/// @return : npc info list of const reference
/// @brief  : npc info list of const To return a reference.
//--------------------------------------------------------------------------------
const std::list< NpcInfoInMap >& CObjectMANAGER::GetNpcInfoList()
{
	return m_listNpcInfo;
}


//-------------------------------------------------------------------------------------------------
/// Object-related events
//--------------------------------------------------------------------------------
CObjFixedEvent*	CObjectMANAGER::GetEventObject( int iEventID )
{
	std::map< int, CObjFixedEvent* >::iterator searched = m_EventObjectList.find( iEventID );
	if( searched != m_EventObjectList.end() )
		return (*searched).second;

	return NULL;
}
//-------------------------------------------------------------------------------------------------
/// All information is updated CObjAVT clan mark.
//--------------------------------------------------------------------------------
void CObjectMANAGER::ResetClanMarkInfo( DWORD dwClanID, WORD crc16 )
{
	classDLLNODE< int > *pNode;
	pNode = m_CharLIST.GetHeadNode ();
	CObjAVT* pAvatar;
	while( pNode ) 
	{
		if ( m_pOBJECTS[ pNode->DATA ] ) 
		{			
			if( m_pOBJECTS[ pNode->DATA ]->IsUSER () )
			{
				pAvatar = (CObjAVT*)m_pOBJECTS[ pNode->DATA ];
				if( pAvatar->GetClanID() == dwClanID )
					pAvatar->SetClanMark( 0, crc16 );
			}

		}
		pNode = m_CharLIST.GetNextNode( pNode );
	} 
}

#ifdef __NPC_COLLISION

void CObjectMANAGER::ResponseCollisionNPC(int iMotionIDX)
{
	bool bMotionIng;
	CObjCHAR *pObjectChar;
	classDLLNODE< int > *pNode;
	
	pNode = m_CharLIST.GetHeadNode ();
	while( pNode ) 
	{
		if ( m_pOBJECTS[ pNode->DATA ] ) 
		{			
			if (m_pOBJECTS[ pNode->DATA ]->IsNPC()) //true if object is OBJ_NPC (6) or OBJ_MOB (7)
			{
				pObjectChar = (CObjCHAR*)g_pObjMGR->m_pOBJECTS[ pNode->DATA ];
				if(pObjectChar->m_bCollisionOnOff)
				{
					::setShadowOnOffRecursive(pObjectChar->GetZMODEL(), false);
					bMotionIng = pObjectChar->ProcSpecialMotionFrame();
				    
					if(!bMotionIng && (pObjectChar->m_iPreCollisionNPCState != pObjectChar->m_iCurrentCollisionNPCState))
					{
						if(pObjectChar->m_iCurrentCollisionNPCState == 1)
						{
							pObjectChar->Set_MOTION(pObjectChar->m_iCurrentCollisionNPCState, 0.0f, 1.0f, false, 1);
						}
						else
						{
							pObjectChar->Set_MOTION(pObjectChar->m_iCurrentCollisionNPCState, 0.0f, 1.0f, false, 0);
						}
						    
						if(pObjectChar->m_iCurrentCollisionNPCState == 1 && pObjectChar->m_iPreCollisionNPCState == 0)
						{
							pObjectChar->m_iPreCollisionNPCState = 1;
							pObjectChar->m_iCurrentCollisionNPCState = 2;
						}
						else
						{
							pObjectChar->m_iPreCollisionNPCState = pObjectChar->m_iCurrentCollisionNPCState;
						}
						
					}
				}
			}			
		}

		pNode = m_CharLIST.GetNextNode( pNode );
	} 
	
}

void CObjectMANAGER::OpeningCollisionNPCMotion()
{	
	CObjCHAR *pObjectChar;
	classDLLNODE< int > *pNode;
	
	m_iCurrentCollisionNPCState = 1;

	pNode = m_CharLIST.GetHeadNode ();
	while( pNode ) 
	{
		if ( m_pOBJECTS[ pNode->DATA ] ) 
		{			
			if (m_pOBJECTS[ pNode->DATA ]->IsNPC()) //true if object is OBJ_NPC (6) or OBJ_MOB (7)
			{
				pObjectChar = (CObjCHAR*)g_pObjMGR->m_pOBJECTS[ pNode->DATA ];
				if(pObjectChar->m_bCollisionOnOff)
				{
					pObjectChar->m_iCurrentCollisionNPCState = 1;
					pObjectChar->m_iPreCollisionNPCState = 0;
				}
			}			
		}

		pNode = m_CharLIST.GetNextNode( pNode );
	} 
	
}

void CObjectMANAGER::ClosedCollisionNPCMotion()
{
	CObjCHAR *pObjectChar;
	classDLLNODE< int > *pNode;

	m_iCurrentCollisionNPCState = 0;
	
	pNode = m_CharLIST.GetHeadNode ();
	while( pNode ) 
	{
		if ( m_pOBJECTS[ pNode->DATA ] ) 
		{			
			if (m_pOBJECTS[ pNode->DATA ]->IsNPC()) //true if object is OBJ_NPC (6) or OBJ_MOB (7)
			{
				pObjectChar = (CObjCHAR*)g_pObjMGR->m_pOBJECTS[ pNode->DATA ];
				if(pObjectChar->m_bCollisionOnOff)
				{
					pObjectChar->m_iCurrentCollisionNPCState = 0;
					pObjectChar->m_iPreCollisionNPCState = 1;
				}
			}			
		}

		pNode = m_CharLIST.GetNextNode( pNode );
	} 

}

void CObjectMANAGER::OpenedCollisionNPCMotion() 
{
	CObjCHAR *pObjectChar;
	classDLLNODE< int > *pNode;

	m_iCurrentCollisionNPCState = 2;

	pNode = m_CharLIST.GetHeadNode ();
	while( pNode ) 
	{
		if ( m_pOBJECTS[ pNode->DATA ] ) 
		{			
			if (m_pOBJECTS[ pNode->DATA ]->IsNPC()) //true if object is OBJ_NPC (6) or OBJ_MOB (7)
			{
				pObjectChar = (CObjCHAR*)g_pObjMGR->m_pOBJECTS[ pNode->DATA ];
				if(pObjectChar->m_bCollisionOnOff)
				{
					pObjectChar->m_iCurrentCollisionNPCState = 2;
					pObjectChar->m_iPreCollisionNPCState = 1;
				}
			}			
		}

		pNode = m_CharLIST.GetNextNode( pNode );
	} 

}

#endif

