/*
	$Header: /Client/CObjCHAR.cpp 698   07-01-25 6:14p Raning $
*/
#include "stdAFX.h"
#include "Game.h"
#include "OBJECT.h"
#include "BULLET.h"
#include "IO_Terrain.h"
#include "IO_Event.h"
#include "CViewMSG.h"
#include "Network\CNetwork.h"
#include "common\Calculation.h"
#include "Game_FUNC.h"
#include "Interface/it_Mgr.h"
#include "Interface/CUIMediator.h"
#include "Interface/TypeResource.h"
#include "Interface/Dlgs/ChattingDlg.h"
#include "Interface/CClanMarkUserDefined.h"
#include "GameCommon/Skill.h"
#include "Event/Quest_FUNC.h"
#include "CCamera.h"
#include "Game_FUNC.h"

#include "GameProc/ChangeVisibility.h"
#include "CObjCART.h"
#include "CObjCHAR_Collision.h"

#ifdef __NPC_COLLISION
#include "CObjCHAR_Collision2NPC.h"
#endif

#include "GameCommon/Item.h"
#include "GameData/CParty.h"
#include "Common/IO_Pat.h"
#include "Misc/GameUtil.h"
#include "CommandFilter.h"
#include "../GameProc/DelayedExp.h"
#include "../Country.h"
#include "../common/CInventory.h"
#include "system/System_Func.h"


extern CCamera * g_pCamera;


extern CAI_OBJ *AI_FindFirstOBJ( CAI_OBJ *pBaseOBJ, int iDistance );
extern CAI_OBJ *AI_FindNextOBJ ();


//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @param int iDistance 거리
/// @brief  : AI Action to find the first object. ..
///              Search operation, the first object is flowed.
//--------------------------------------------------------------------------------

CAI_OBJ* CObjCHAR::AI_FindFirstOBJ( int iDistance )
{
	return ::AI_FindFirstOBJ( this, iDistance );
}

//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @param
/// @brief  :AI_FindFirstOBJ 호출후 호출하여 리스트상의 다음 오브젝트를 꺼집어 냄
//--------------------------------------------------------------------------------

CAI_OBJ* CObjCHAR::AI_FindNextOBJ ()
{
	return ::AI_FindNextOBJ ();
}


float CObjCHAR::GetStature()
{
	return m_fStature;
}


int CObjCHAR::GetMinStateValue( int iIng_Type, int iValue )
{
#ifdef __CAL_BUF2

	switch(iIng_Type)
	{
	case AT_ATK:
		if( iValue<MIN_ATK )
			iValue = MIN_ATK;
		break;
	case AT_DEF:
		if( iValue<MIN_DEF )
			iValue = MIN_DEF;
		break;
	case AT_HIT:
		if( iValue<MIN_HIT )
			iValue = MIN_HIT;
		break;
	case AT_AVOID:
		if( iValue<MIN_AVOID )
			iValue = MIN_AVOID;
		break;
	case AT_ATK_SPD:
		if( iValue<MIN_ATK_SPD )
			iValue = MIN_ATK_SPD;
		break;
	case AT_RES:
		if( iValue<MIN_RES )
			iValue = MIN_RES;
		break;
	case AT_CRITICAL:
		if( iValue<MIN_CRITICAL )
			iValue = MIN_CRITICAL;
		break;
	case AT_SPEED:
		if( iValue<MIN_SPEED )
			iValue = MIN_SPEED;
		break;
	}
	return iValue;
#else
	switch(iIng_Type)
	{
	case AT_ATK_SPD:
		if( iValue<MIN_ATK_SPD )
			iValue = MIN_ATK_SPD;
		break;
	case AT_SPEED:
		if( iValue<MIN_SPEED )
			iValue = MIN_SPEED;
		break;
	}
	return iValue;
#endif
}

//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @param wSrvDIST We can see the target location and the server is calculated from the current position of the two-dimensional Street. Unit cm
/// @param PosGOTO Move target position, flew in from the server. At present, the Z value is meaningless.
/// @brief  : Speed calibration. Updated every frame. If the car or the speed of the server and the client,
///           Increase the speed force. Or too much difference to me when, forced to move.
///
//--------------------------------------------------------------------------------

void CObjCHAR::Adj_MoveSPEED ( WORD wSrvDIST, const D3DVECTOR& PosGOTO )
{
	int iClientDIST; // The difference between the current-on the client position. Unit CM
	float fCurSpeed, fNewSpeed, fNeedTime;

	fCurSpeed = this->Get_DefaultSPEED(); // The current default rate calculated by the expression

	if ( 0 == fCurSpeed ) // If the velocity is zero, the same as the current location and the PosGOTO is?
	{ 
		m_fAdjustSPEED = 0;

		//assert(m_PosCUR.x == PosGOTO.x);
		//assert(m_PosCUR.y == PosGOTO.y);

		return;
	}

	fNeedTime = float(wSrvDIST) / fCurSpeed;

	// The client calculates the distance of the current goals.
	iClientDIST = CD3DUtil::distance ((int)m_PosCUR.x, (int)m_PosCUR.y, (int)PosGOTO.x, (int)PosGOTO.y);

	//assert(iClientDIST >= 0);		//PY: Can't have these damn asserts breaking things all the time
	if(iClientDIST < 0)				//this is probably a bad idea in the long run as it will cause discrepencies between server and client but it will do for now
		iClientDIST = 0;

	if ( 0 == iClientDIST ) { // If you have already reached the target area,
		// The client does not need to be moved.
		m_fAdjustSPEED = 0;
		return;
	}
	/// To go the distance to go from the client on the server is greater than the distance
	else if( iClientDIST <= wSrvDIST )
	{

		/// To keep the current rate.
		/// Because if you wait for a client to go ahead.
		Set_AdjustSPEED( fCurSpeed );
		return;

	}
	else // wSrvDIST < iClientDIST. The distance of the server from the client if, rather than distance from the rate increase.
	{
		fNewSpeed = float(iClientDIST) / fNeedTime; // Calculate the new speed to reach in time

		/// If the subject or just run sucked. (Without jumping)
		if( this->IsA( OBJ_USER ) == false )
		{
			int iDiffDistance = iClientDIST - wSrvDIST; // The server calculates the distance and the client street car. Unit: cm

			float fNeedTimeDiff = float(iDiffDistance) / fNewSpeed; // It takes time to overcome the distance car

			if (fNeedTimeDiff > 1.0f)
			{
				// Street car to overcome delays over one second (too slow).
				fNewSpeed = fCurSpeed; // The force that moves the displacement

				D3DXVECTOR3 vDir = (D3DXVECTOR3)PosGOTO - m_PosCUR;
				float fRatio = (float)iDiffDistance / (float)iClientDIST;

				D3DXVECTOR3 vPosCur = m_PosCUR + ( vDir * fRatio );
				vPosCur.z = m_PosCUR.z;

				this->ResetCUR_POS( vPosCur );
			}
		}
	}
	Set_AdjustSPEED( fCurSpeed );
}

//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @brief  : Attack speed adjustment ...
//--------------------------------------------------------------------------------

void CObjCHAR::Adj_AniSPEED ( float fAniSpeed )
{
	::setAnimatableSpeed( this->m_hNodeMODEL, fAniSpeed  );
}

//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @param int iServerTarget Object Server index
/// @param tPOINTF &PosFROM
/// @param tPOINTF &PosGOTO
/// @brief  : The attack command set
//--------------------------------------------------------------------------------

void CObjCHAR::SetCMD_ATTACK (int iServerTarget, WORD wSrvDIST, const D3DVECTOR& PosGOTO)
{
	if( this->IsA( OBJ_USER ) )
	{
		CObjAttackCommand* pObjCommand = (CObjAttackCommand*)g_CommandFilter.GetCommandObject( OBJECT_COMMAND_ATTACK );
		pObjCommand->SetCMD_ATTACK( iServerTarget, wSrvDIST, PosGOTO );

		g_CommandFilter.SetPrevCommand( pObjCommand );
	}

	/// You can get the current command?
	if( this->CanApplyCommand() == false )
	{
		this->PushCommandAttack( iServerTarget, wSrvDIST, PosGOTO );
		return;
	}

	//--------------------------------------------------------------------------------
	/// If this char is under pet mode, pass the command to pet.
	int iPetMode = this->GetPetMode();
	if( iPetMode >= 0 )
	{
		/// Pet mode In the event that ...
		if( this->CanAttackPetMode() )
		{
			SetCMD_PET_ATTACK( iServerTarget, wSrvDIST, PosGOTO );
		}
//Park Ji-Ho:: pet mode to set the properties of the avatar attacks.
#ifndef _GBC
		return;
#endif
	}

	/// The server received the attack packet processing in ...
	this->Adj_MoveSPEED( wSrvDIST, PosGOTO );

	this->m_PosGOTO = PosGOTO;

	CObjAI::SetCMD_ATTACK( iServerTarget );
}


//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @param
/// @brief  : Constructor
//--------------------------------------------------------------------------------

CObjCHAR::CObjCHAR () : m_EndurancePack( this ), m_ChangeActionMode( this ), m_ObjVibration( this, 200 )
{
#ifdef	__VIRTUAL_SERVER
	m_pRegenPOINT = NULL;
	m_pRegenListNODE = NULL;
#endif

	m_fScale = 1.0f;

	m_fHeightOfGround = 0.0f;
	m_hNodeMODEL = NULL;

	m_ppBoneEFFECT = NULL;
	m_hR_TRAIL = m_hL_TRAIL = NULL;

	for (short nP=0; nP<MAX_BODY_PART; nP++) {
		m_phPartVIS[ nP ] = NULL;
		m_pppEFFECT[ nP ] = NULL;
		m_nEffectPointCNT[ nP ] = 0;
	}

	::ZeroMemory( &m_DeadDAMAGE, sizeof( tagDAMAGE ) );
	m_lDeadTIME = 0;

	m_DamageList.reserve( 10 );

	m_bProcEffectedSkill = false;
	m_iEffectedSkillIndex = 0;
	m_EffectedSkillList.clear();

	m_fAltitude = 0.0;

	m_iHP	 = 1;
	m_iMP	 = 1;
	m_iMaxHP = 1;
	m_iMaxMP = 1;

	m_dwLastRecoveryUpdateTime	= g_GameDATA.GetGameTime();
	m_dwElapsedTime				= 0;
	m_dwFrameElapsedTime		= 0;

	m_bDead = false;

	m_pChangeVisibility			= NULL;
	m_bStopDead					= false;

	m_bCartInvitationWnd		= false;
	m_bExChangeWnd				= false;
	m_bAddFriendWnd				= false;
	m_bPartyInvitationWnd		= false;


	m_bIsVisible				= true;

	m_hNodeGround				= NULL;

	m_pCollision				= new CObjCHAR_Collision;				/// will be deleted in DeletCHAR()

#ifdef __NPC_COLLISION
	m_pCollision2NPC            = new CObjCHAR_Collision2NPC;           // NPC Crash
#endif


	memset( &m_SummonMob, 0 , sizeof( gsv_MOB_CHAR ) );
	m_bHaveSummonedMob			= false;							/// Should be summoned to the mob?

	m_bUseResetPosZ				= false;
	m_fResetPosZ				= 0;
	m_fModelSPEED				= 0;

	m_iLastCastingSkill			= 0;

	m_dwClanID					= 0;
	m_wClanMarkBack				= 0;
	m_wClanMarkCenter			= 0;

	m_bFrameING					= 0;
	m_ClanMarkUserDefined		= 0;


	m_ReviseHP					= 0;
	m_ReviseMP					= 0;

//-------------------------------------------------------------------------------
	//Cho Sung-Hyun
	m_bDisguise					= false;


//--------------------------------------------------------------------------------
	///Park Ji-Ho
	//Cart variables initialization
	m_iPetType					= -1;
	m_pObjCART					= NULL;
	m_pRideUser					= NULL;

	m_bUseCartSkill				= FALSE;
	m_IsRideUser				= FALSE;

	m_saveSpeed					= 0.0f;

	m_iPetType					= -1;
	m_iRideIDX					= 0;
	m_skCartIDX					= 0;

	//Oh Loa status variable initialization
	m_IsAroa					= 0;
	m_IsCartVA					= 0;
//--------------------------------------------------------------------------------
	m_AruaAddMoveSpeed			= 0;
}


//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @param
/// @brief  : Destructor
//--------------------------------------------------------------------------------

CObjCHAR::~CObjCHAR ()
{
	// Delete among the registered engine HNODE.
	this->DeleteCHAR ();

	ClearExternalEffect();
	m_EndurancePack.ClearEntityPack();
	SAFE_DELETE( m_pCollision );

#ifdef __NPC_COLLISION
	SAFE_DELETE( m_pCollision2NPC );
#endif

}

//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @param
/// @brief  : For the recovery timer reset
//--------------------------------------------------------------------------------
void CObjCHAR::ClearTimer()
{
	m_dwLastRecoveryUpdateTime	= g_GameDATA.GetGameTime();
	m_dwElapsedTime				= 0;
	m_dwFrameElapsedTime		= 0;
}


//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @param
/// @brief  : Dead or destroyed, all internal list cleanup
//--------------------------------------------------------------------------------

void CObjCHAR::ClearAllEntityList()
{
	/// proc all effect of skill
	ProcEffectedSkill();

	/// clear all damage list
	ClearAllDamage();

	/// drop field item list
	DropFieldItemFromList();

	/// clear all command
	ClearAllCommand();
}


//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @param
/// @brief  : The cost of that enrollment outside of the effects list cleanup
//--------------------------------------------------------------------------------

void CObjCHAR::ClearExternalEffect()
{
	classDLLNODE< CEffect* > *pNode;
	pNode = m_ExternalEffectLIST.GetHeadNode ();
	while( pNode )
	{
		/// Delete the only effect of the parents should not handle the effects. Why? I'm a parent ...
		g_pEffectLIST->Del_EFFECT( pNode->DATA, false );

		m_ExternalEffectLIST.DeleteNFree( pNode );
		pNode = m_ExternalEffectLIST.GetHeadNode();
	}

	m_ExternalEffectLIST.ClearList();

	DeleteWeatherEffect();
}


//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @param CEffect* pEffect Delete an effect
/// @brief  : Registration of external effects
//--------------------------------------------------------------------------------

void CObjCHAR::AddExternalEffect(CEffect* pEffect)
{
	m_ExternalEffectLIST.AllocNAppend( pEffect );
}


//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @param  CEffect* pEffect Delete an effect
/// @brief  : Clear registered outside of the effects
//--------------------------------------------------------------------------------

void CObjCHAR::DeleteExternalEffect( CEffect* pEffect )
{
	if( pEffect == NULL )
		return;

	classDLLNODE< CEffect* > *pNode;
	pNode = m_ExternalEffectLIST.GetHeadNode ();
	while( pNode )
	{
		if ( pNode->DATA == pEffect )
		{
			m_ExternalEffectLIST.DeleteNFree( pNode );
			return;
		}
		pNode = m_ExternalEffectLIST.GetNextNode( pNode );
	}
}


//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @param  D3DVECTOR &PosSCR Come get screen coordinates (output)
/// @brief  : Get current character position of the screen coordinates of the ohm
//--------------------------------------------------------------------------------

void CObjCHAR::GetScreenPOS ( D3DVECTOR &PosSCR)
{
	// The coordinates of the model plus the name of the location of the output location settings key
	::worldToScreen( m_PosCUR.x, m_PosCUR.y, getPositionZ(m_hNodeMODEL) + m_fStature, &PosSCR.x, &PosSCR.y, &PosSCR.z );
}


//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @param  HNODE hLinkNODE The target node of the link handle
/// @param  short nDummyIDX A dummy link index
/// @brief  : Enter enter the node corresponding to the index piles in a pile of received link.
//--------------------------------------------------------------------------------

bool CObjCHAR::LinkDummy ( HNODE hLinkNODE, short nDummyIDX )
{
	int iDummyCnt = ::getNumDummies( m_hNodeMODEL );
	if ( iDummyCnt >= nDummyIDX )
		return ( 0 != ::linkDummy( m_hNodeMODEL, hLinkNODE, nDummyIDX) );

	return false;
}


//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @param  HNODE hLinkNODE The target node of the link handle
/// @brief  : Enter a link to the last node in the pile of Sikkim
//--------------------------------------------------------------------------------

bool CObjCHAR::Link2LastDummy(HNODE hLinkNODE)
{
/*
	int iDummyCnt = ::getNumDummies( m_hNodeMODEL );
	// The last pile ...
	if ( iDummyCnt > 0 )
		m_iLastDummyIDX = iDummyCnt - 1;
*/
	return ( 0 != ::linkDummy( m_hNodeMODEL, hLinkNODE, m_iLastDummyIDX) );
}


//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @param
/// @brief  : The current character in the scene. (Includes all child objects)
//--------------------------------------------------------------------------------

void CObjCHAR::InsertToScene (void)
{
	if ( m_bIsVisible || !m_hNodeMODEL || !m_pCharMODEL )
		return;

	m_bIsVisible = true;

	::insertToScene( m_hNodeMODEL );		// CObjCHAR::InsertToScene

	//----------------------------------------------------------------------------------------------------
	/// @brief InsertToScene removal of CHILD
	//----------------------------------------------------------------------------------------------------
	//for (short nP=0; nP<MAX_BODY_PART; nP++)
	//{
	//	CMODEL<CCharPART> *pCharPART = m_pCharMODEL->GetCharPART( nP );

	//	if ( pCharPART  )
	//	{
	//		short nI;

	//		if ( m_pppEFFECT[ nP ] )
	//		{
	//			for (nI=0; nI<pCharPART->m_nDummyPointCNT; nI++)
	//			{
	//				if ( m_pppEFFECT[ nP ][ nI ] )
	//				{
	//					m_pppEFFECT[ nP ][ nI ]->InsertToScene ();
	//				}
	//			}
	//		}

	//		for (nI=0; nI<pCharPART->m_nPartCNT; nI++)
	//		{
	//			if ( m_phPartVIS[ nP ][ nI ] )
	//			{
	//				::insertToScene( m_phPartVIS[ nP ][ nI ] );		// CObjCHAR::InsertToScene
	//			}
	//		}
	//	}
	//}

	//
	//// Skeleton effect.
	//if ( m_ppBoneEFFECT )
	//{
 	//	for (nP=0; nP<m_pCharMODEL->GetBoneEffectCNT(); nP++)
	//		m_ppBoneEFFECT[ nP ]->InsertToScene ();
	//}

	//// Black ghosting effects.
	//for (nP=0; nP<2; nP++)
	//{
	//	if ( m_hTRAIL[ nP ] )
	//		::insertToScene( m_hTRAIL[ nP ] );
	//}
}


//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @param  bool bIncludeEFFECT Effects have been included?
/// @brief  : Removed from the scene. (Object)
//--------------------------------------------------------------------------------

void CObjCHAR::RemoveFromScene (bool bIncludeEFFECT)
{
	if ( !m_bIsVisible )
		return;

	m_bIsVisible = false;
	::removeFromScene ( m_hNodeMODEL );								// CObjCHAR::RemoveFromScene

	//----------------------------------------------------------------------------------------------------
	/// @brief CHILD의removeFromScene제거
	//----------------------------------------------------------------------------------------------------
	//for (short nP=0; nP<MAX_BODY_PART; nP++)
	//{
	//	CMODEL<CCharPART> *pCharPART = m_pCharMODEL->GetCharPART( nP );

	//	if ( pCharPART  )
	//	{
	//		short nI;
	//		if ( bIncludeEFFECT && m_pppEFFECT[ nP ] )
	//		{
	//			for (nI=0; nI<pCharPART->m_nDummyPointCNT; nI++)
	//				if ( m_pppEFFECT[ nP ][ nI ] ) {
	//					m_pppEFFECT[ nP ][ nI ]->RemoveFromScene ();
	//				}
	//		}

	//		for (nI=0; nI<pCharPART->m_nPartCNT; nI++) {
	//			if ( m_phPartVIS[ nP ][ nI ] ) {
	//				::removeFromScene( m_phPartVIS[ nP ][ nI ] );	// CObjCHAR::RemoveFromScene
	//			}
	//		}
	//	}
	//}

	//if ( bIncludeEFFECT )
	//{
	//	// Skeleton effect.
	//	if ( m_ppBoneEFFECT )
	//	{
	//		for (nP=0; nP<m_pCharMODEL->GetBoneEffectCNT(); nP++)
	//		{
	//			if( m_ppBoneEFFECT[ nP ] )
	//				m_ppBoneEFFECT[ nP ]->RemoveFromScene ();
	//		}
	//	}

	//	for (nP=0; nP<2; nP++) {
	//		if ( m_hTRAIL[ nP ] )
	//			::removeFromScene( m_hTRAIL[ nP ] );
	//	}
	//}
}

/*override*/D3DXVECTOR3& CObjCHAR::Get_CurPOS ()
{
	m_PosCUR.z = ::getPositionZ(m_hNodeMODEL);

	return m_PosCUR;
}

/*override*/ void CObjCHAR::SetEffectedSkillFlag( bool bResult )
{
	m_bProcEffectedSkill = bResult;
	///Log_String( LOG_NORMAL, "SetEffectedSkillFlag[ %d ]\n", bResult );
}

/*override*/ bool CObjCHAR::bCanActionActiveSkill()
{
	///Log_String( LOG_NORMAL, "bCanActionActiveSkill[ %d ]\n", m_bProcEffectedSkill );
	return m_bProcEffectedSkill;
}


//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @param  CEffect *pEffect Link effects
/// @param  short nPartIDX	 Linking character parts
/// @param  short nPointIDX  Pointer to the index of the character to link parts
/// @brief  : A specific part of the character of the effect on the links a specific pointer objects.
//--------------------------------------------------------------------------------

bool CObjCHAR::LinkEffectToPOINT (CEffect *pEffect, short nPartIDX, short nPointIDX)
{
	CMODEL<CCharPART> *pCharPART;
	pCharPART = m_pCharMODEL->GetCharPART( nPartIDX );

	if ( pCharPART &&
		( !pCharPART->m_pDummyPoints || nPointIDX >= pCharPART->m_nDummyPointCNT ) )
	{
		_ASSERT( FALSE );

		return false;
	}

	pEffect->Transform( pCharPART->m_pDummyPoints[ nPointIDX ].m_Transform );
	pEffect->LinkNODE ( m_phPartVIS[ nPartIDX ][ pCharPART->m_pDummyPoints[ nPointIDX ].m_nParent ] );

	return true;
}

//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @param  CEffect *pEffect Link effects
/// @param  int iPointNO  A pile pointer index link
/// @brief  : Link to a specific pile of character effects
//--------------------------------------------------------------------------------

bool CObjCHAR::LinkEffectToDUMMY (CEffect *pEffect, short nDummyIDX )
{
	int iDummyCnt = ::getNumDummies( m_hNodeMODEL );
	if ( iDummyCnt < nDummyIDX )
		return false;

	pEffect->LinkDUMMY ( this->GetZMODEL(), nDummyIDX );

	return true;
}


//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @param  CMODEL<CCharPART> *pCharPART A specific part of the character (right hand or left hand weapon?)
/// @param  nPartIDX Body part index (right hand weapon is the weapon of the left hand index to come)
/// @param  bool bLinkBONE		This link will do?
/// @param  int iColorNO		Black ghosting color number
/// @param  int iDuration		Duration?
/// @param  int iBaseDummyIDX	Right hand, left hand for bass patterns index
/// @brief  : You can hear two of the weapons in both hands, it is possible to set up.
//--------------------------------------------------------------------------------

void CObjCHAR::LoadTRAIL (CMODEL<CCharPART> *pCharPART, short nPartIDX, bool bLinkBONE, int iColorNO, int iDuration, int iBaseDummyIDX )
{
	if ( pCharPART->m_nDummyPointCNT < 3 )
		return;

	short nTrailIDX = nPartIDX - BODY_PART_WEAPON_R;
	_ASSERT( nTrailIDX >= 0 && nTrailIDX < 2 );

	m_hTRAIL[ nTrailIDX ] = ::loadTrail ( NULL,	// ZSTRING pName,
		 10,									// float fDistancePerPoint,
		 iDuration,								// int iDurationInMilliSecond,
		 1,										// int bUseCurve,
		 g_GameDATA.m_TrailTexture.Get(),		// ZSTRING pTexturePath,
		 g_dwCOLOR[ iColorNO ],					// D3DCOLOR,
		 //0, 0, 0,								// float fSP_X, float fSP_Y, float fSP_Z,
		 pCharPART->m_pDummyPoints[ 0 + iBaseDummyIDX ].m_Transform,
		 //0, 0, 100 );							// float fEP_X, float fEP_Y, float fEP_Z );
		 pCharPART->m_pDummyPoints[ 1 + iBaseDummyIDX ].m_Transform );

	if ( m_hTRAIL[ nTrailIDX ] )
	{
		::controlTrail ( m_hTRAIL[ nTrailIDX ], 0 );	// stop !!

		//----------------------------------------------------------------------------------------------------
		/// @brief InsertToScene removal of CHILD
		//----------------------------------------------------------------------------------------------------
		//if ( m_bIsVisible )		// Currently appears. ..
		//	::insertToScene( m_hTRAIL[ nTrailIDX ] );
	}

	if ( bLinkBONE )
	{
		::linkDummy ( m_hNodeMODEL, m_hTRAIL[ nTrailIDX ], nTrailIDX+DUMMY_IDX_R_HAND );
	}
}


//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @param  nPartIDX Body part index (right hand weapon is the weapon of the left hand index to come)
/// @brief  : The sword stuck off
//--------------------------------------------------------------------------------

void CObjCHAR::UnloadTRAIL (short nPartIDX)
{
	short nTrailIDX = nPartIDX - BODY_PART_WEAPON_R;

	_ASSERT( nTrailIDX >= 0 && nTrailIDX < 2 );

	if ( m_hTRAIL[ nTrailIDX ] ) {
		::controlTrail( m_hTRAIL[ nTrailIDX ], 0 );	// stop !!
		::unlinkNode  ( m_hTRAIL[ nTrailIDX ] );
		::unloadTrail ( m_hTRAIL[ nTrailIDX ] );
		m_hTRAIL[ nTrailIDX ] = NULL;
	}
}

//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @param
/// @brief  : The effect of firing on the character (models made in the course of the character's tools) links
//--------------------------------------------------------------------------------

void CObjCHAR::Link_EFFECT (void)
{
	short nP;

	for (nP=0; nP<MAX_BODY_PART; nP++)
	{
		if ( NULL == m_pppEFFECT[ nP ] )
			continue;

		CMODEL<CCharPART> *pCharPART = m_pCharMODEL->GetCharPART( nP );
		_ASSERT( pCharPART ) ;

		for (short nI=0; nI<pCharPART->m_nDummyPointCNT; nI++)
		{
			if ( NULL != m_pppEFFECT[ nP ][ nI ] )
				m_pppEFFECT[ nP ][ nI ]->LinkNODE( m_phPartVIS[ nP ][ pCharPART->m_pDummyPoints[ nI ].m_nParent ] );
		}
	}

	for (nP=0; nP<2; nP++)
	{
		if ( m_hTRAIL[ nP ] )
		{
			::linkDummy ( m_hNodeMODEL, m_hTRAIL[ nP ], nP+DUMMY_IDX_R_HAND );
		}
	}
}


//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @param
/// @brief  : The effect of firing on the character (models made in the course of the character's tools) to link
//--------------------------------------------------------------------------------

void CObjCHAR::Unlink_EFFECT (void)
{
	short nP;

	for (nP=0; nP<2; nP++) {
		if ( m_hTRAIL[ nP ] ) {
			::unlinkNode ( m_hTRAIL[ nP ] );
		}
	}
	for (nP=0; nP<MAX_BODY_PART; nP++)
	{
		if ( NULL == m_pppEFFECT[ nP ] )
			continue;

		// Item changes, effects are generated in the first place.
		// set part Model pCharPART changed items before this call,
		// Do not set the nuts ~
		for (short nI=0; nI<m_nEffectPointCNT[ nI ]; nI++)
		{
			if ( NULL != m_pppEFFECT[ nP ][ nI ] )
				m_pppEFFECT[ nP ][ nI ]->UnlinkNODE ();
		}
	}
}


//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @param  CMODEL<CCharPART> *pCharPART To add the effect to the character's part
/// @param  short nPartIDX	   Part index
/// @param  short nPointIDX	   Pointer to the index of the part
/// @param  t_HASHKEY HashEffectFILE  Effects of the file hash key
/// @param  bool bLinkNODE			  Want to link to?
/// @brief  : The effect of firing on the character (models made in the course of the character's tools)
///	@bug    : 04/4/28 g_pEffectLIST->Add_EFFECT(.., false ) It was just a. .. G_pEffectLIST-Del_EFFECT was commissioned to delete as .. >
/// @bug    : 04/4/28 Ringkeuhalddae Rotation is not?
//--------------------------------------------------------------------------------

void CObjCHAR::Add_EFFECT( CMODEL<CCharPART> *pCharPART, short nPartIDX, short nPointIDX, t_HASHKEY HashEffectFILE, bool bLinkNODE )
{
	if ( NULL == pCharPART )
		return;

	if ( nPointIDX >= pCharPART->m_nDummyPointCNT )
		return;

	if ( m_pppEFFECT[ nPartIDX ] )
	{
		// The effect of deleting an existing point ...
		///g_pEffectLIST->Del_EFFECT( m_pppEFFECT[ nPartIDX ][ nPointIDX ] );
		SAFE_DELETE( m_pppEFFECT[ nPartIDX ][ nPointIDX ] );
		m_pppEFFECT[ nPartIDX ][ nPointIDX ] = NULL;
	} else
	{
		m_nEffectPointCNT[ nPartIDX ] = pCharPART->m_nDummyPointCNT;
		m_pppEFFECT[ nPartIDX ] = new CEffect*[ pCharPART->m_nDummyPointCNT ];
		for (short nI=0; nI<pCharPART->m_nDummyPointCNT; nI++)
			m_pppEFFECT[ nPartIDX ][ nI ] = NULL;
	}

	m_pppEFFECT[ nPartIDX ][ nPointIDX ] = g_pEffectLIST->Add_EFFECT( HashEffectFILE );
	if ( m_pppEFFECT[ nPartIDX ][ nPointIDX ] )
	{
#ifdef	_DEBUG
		if ( !pCharPART->m_pDummyPoints ||
		      nPointIDX >= pCharPART->m_nDummyPointCNT )
		{
			_ASSERT( FALSE );
		}
#endif

		m_pppEFFECT[ nPartIDX ][ nPointIDX ]->Transform( pCharPART->m_pDummyPoints[ nPointIDX ].m_Transform );

		if ( m_bIsVisible )
			m_pppEFFECT[ nPartIDX ][ nPointIDX ]->InsertToScene ();

		if ( bLinkNODE && m_phPartVIS[ nPartIDX ] )
			m_pppEFFECT[ nPartIDX ][ nPointIDX ]->LinkNODE( m_phPartVIS[ nPartIDX ][ pCharPART->m_pDummyPoints[ nPointIDX ].m_nParent ] );
	}
}


//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @param  short nPartIDX	   Part index
/// @param  short nPointIDX	   Pointer to the index of the part
/// @param  t_HASHKEY HashEffectFILE  Effects of the file hash key
/// @brief  : The effect of firing on the character (models made in the course of the character's tools)
///	@bug    : From the inside out Add_EFFECT( CMODEL<CCharPART> *pCharPART, short nPartIDX, short nPointIDX, t_HASHKEY HashEffectFILE, bool bLinkNODE ) Function calls
//--------------------------------------------------------------------------------

void CObjCHAR::Add_EFFECT(short nPartIDX, short nPointIDX, t_HASHKEY HashEffectFILE)
{
	CMODEL<CCharPART> *pCharPART;
	pCharPART = m_pCharMODEL->GetCharPART( nPartIDX );
	this->Add_EFFECT (pCharPART, nPartIDX, nPointIDX, HashEffectFILE);
}


//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @param  short nPartIDX	   Part index
/// @brief  : The effect of firing on the character (models made in the course of the character's tools)
///	@bug    : /// @Bug AddEffect( .. , false ) Bug ...
//--------------------------------------------------------------------------------

void CObjCHAR::Del_EFFECT( short nPartIDX )
{
	CMODEL<CCharPART> *pCharPART;
	pCharPART = m_pCharMODEL->GetCharPART( nPartIDX );
	if ( pCharPART ) {
		if ( m_pppEFFECT[ nPartIDX ] )
		{
			for (short nT=0; nT<pCharPART->m_nDummyPointCNT; nT++)
			{
				///g_pEffectLIST->Del_EFFECT( m_pppEFFECT[ nPartIDX ][ nT ] );
				if( m_pppEFFECT[ nPartIDX ][ nT ] )
				{
					delete m_pppEFFECT[ nPartIDX ][ nT ];
					m_pppEFFECT[ nPartIDX ][ nT ] = NULL;
				}
				m_pppEFFECT[ nPartIDX ][ nT ] = NULL;
			}
		}
	}

	SAFE_DELETE_ARRAY( m_pppEFFECT[ nPartIDX ] );

	switch( nPartIDX ) {
		case BODY_PART_WEAPON_R :
		case BODY_PART_WEAPON_L :
			this->UnloadTRAIL( nPartIDX );
			break;
	}
}


//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @param  short nPartIDX	   Part index
/// @param  short nItemNo	   Part of the item index
/// @param  bool bLinkNODE	   Want to link to?
/// @brief  : The effect of firing on the character generation
///				Weapon retention, there is also a special option of the firearm or by a particular effect when you paste ...
//--------------------------------------------------------------------------------

void CObjCHAR::New_EFFECT (short nPartIdx, short nItemNo, bool bLinkNODE)
{
	// Delete the default effects attached to the item.
	this->Del_EFFECT( nPartIdx );

	CMODEL<CCharPART> *pCharPART = g_DATA.Get_CharPartMODEL( nPartIdx, nItemNo, this->IsFemale() );	//m_pMD_CharPART[ nPartIdx ]->GetMODEL( nItemNo );
	if ( NULL == pCharPART )
		return;

	short nEffectIDX=0;
	switch( nPartIdx )
	{
		case BODY_PART_WEAPON_R :
			{
				nEffectIDX = WEAPON_DEFAULT_EFFECT( nItemNo );

				if ( EFFECT_TRAIL_NORMAL( nEffectIDX ) )
				{
					this->LoadTRAIL( pCharPART, BODY_PART_WEAPON_R, bLinkNODE, EFFECT_TRAIL_NORMAL( nEffectIDX ), EFFECT_TRAIL_DURATION( nEffectIDX ) );

					/// If this flow of weapons, both hands. ..
					if( ( pCharPART->m_nDummyPointCNT > 2 ) &&
						( ( WEAPON_TYPE( nItemNo ) == 251 ) || ( WEAPON_TYPE( nItemNo ) == 252 ) ) )
						this->LoadTRAIL( pCharPART, BODY_PART_WEAPON_L, bLinkNODE, EFFECT_TRAIL_NORMAL( nEffectIDX ), EFFECT_TRAIL_DURATION( nEffectIDX ), 2 );
				}
			}
			break;
		case BODY_PART_WEAPON_L :
			// nEffectIDX = SUBWPN_DEFAULT_EFFECT( nItemNo );
			if ( EFFECT_TRAIL_NORMAL( nEffectIDX ) ) {
				this->LoadTRAIL( pCharPART, BODY_PART_WEAPON_L, bLinkNODE, EFFECT_TRAIL_NORMAL( nEffectIDX ), EFFECT_TRAIL_DURATION( nEffectIDX ) );
			}
			break;
		default :
			return;
	}

	if ( nEffectIDX )
	{
		t_HASHKEY EffectHASH;
		for (short nP=0; nP<EFFECT_POINT_CNT; nP++)
		{
			EffectHASH = g_pEffectLIST->Get_IndexHashKEY( EFFECT_POINT( nEffectIDX, nP ) );
			if ( EffectHASH )
				this->Add_EFFECT( pCharPART, nPartIdx, nP, EffectHASH, bLinkNODE );
		}
	}
}


//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @param  char *szName Name (a unique name by appending a number of internal generating order count.)
/// @param  int iPartIDX Part index
/// @brief  : Create specific parts
//--------------------------------------------------------------------------------

void CObjCHAR::CreateSpecificPART( char *szName, int iPartIDX )
{
	m_phPartVIS[ iPartIDX ] = m_pCharMODEL->CreatePART( szName, m_hNodeMODEL, iPartIDX );
}


//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @param  int iPartIDX Part index
/// @param  HNODE *pVIS  Part of the parts that comprise an array of nodes visiable
/// @brief  : Delete a specific area
//--------------------------------------------------------------------------------

void CObjCHAR::DeleteSpecificPART( short nPartIdx, HNODE *pVIS )
{
	m_pCharMODEL->DeletePART( nPartIdx, pVIS );
}


//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @param  char *szName Name
/// @brief  : Create each character parts, each part from the inside by CreateSpecificPART creation
//--------------------------------------------------------------------------------

void CObjCHAR::CreatePARTS (char *szName)
{
	m_pCharMODEL->ClearRenderUnitParts();

	// npc Weapons in MAX_BODY_PART까지 ...
	for (short nP=0; nP<MAX_BODY_PART; nP++)
	{
		CreateSpecificPART( szName, nP );
		//m_phPartVIS[ nP ] = m_pCharMODEL->CreatePART( szName, m_hNodeMODEL, nP );
	}

	// Characters of the kidney.
	m_fStature = ::getModelHeight (this->m_hNodeMODEL);
}


//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @param  bool bDelEFFECT Also erase the effect?
/// @brief  : Delete each character parts
//--------------------------------------------------------------------------------

void CObjCHAR::DeletePARTS (bool bDelEFFECT)
{
	this->RemoveFromScene (bDelEFFECT);

	if ( this->m_hNodeMODEL )
	{
		// addRenderUnit Delete the things.
		::clearRenderUnit( this->m_hNodeMODEL );
	}

	// loadVisible Delete the things.
	for (short nP=0; nP<MAX_BODY_PART; nP++)
	{
		DeleteSpecificPART( nP, m_phPartVIS[ nP ] );
		//m_pCharMODEL->DeletePART( nP, m_phPartVIS[ nP ] );
		m_phPartVIS[ nP ] = NULL;
	}
}


//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @param  char *szName Model name
/// @brief  : The engine of the model node produces the characters.
//--------------------------------------------------------------------------------

bool CObjCHAR::LoadModelNODE (char *szName)
{
	HNODE hSkel  = this->Get_SKELETON ();
	if ( hSkel == NULL ) {
		LogString (LOG_DEBUG, "failed creat char.. SKEL == NULL !! \n");
		return false;
	}

	if ( !this->Set_CurMOTION( this->Get_MOTION(0) ) ) {		// default stop motion ..
		LogString (LOG_DEBUG, "failed creat char.. MOTION == NULL !! \n");
		return false;
	}

	m_hNodeMODEL = ::loadModel (szName, hSkel, this->Get_ZMOTION(), 1.0f);
	if ( m_hNodeMODEL )
	{
		::setCollisionLevel( m_hNodeMODEL, 4 );

		// The height of the default terrain character Center
		m_fHeightOfGround = ::getPositionZ (this->m_hNodeMODEL);
		m_iLastDummyIDX   = ::getNumDummies( m_hNodeMODEL ) - 1;

		::setScale   (m_hNodeMODEL,  m_fScale, m_fScale, m_fScale);
		::setPosition( m_hNodeMODEL, m_PosCUR.x, m_PosCUR.y, m_PosCUR.z );

		return true;
	}

	return false;
}


//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @brief  : The engine of the model node and delete the character. Delete all registered effects from outside are also
//--------------------------------------------------------------------------------

void CObjCHAR::UnloadModelNODE ()
{
	/// Delete all registered effects from outside. ..
	ClearExternalEffect();


	if ( m_hNodeMODEL )
	{
		::unloadModel (m_hNodeMODEL);
		m_hNodeMODEL = NULL;
	}
}


//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @param char *szName			Name
/// @param CCharMODEL *pMODEL	Character models to manage the information. .. Model class
/// @param short nCharPartCNT	Character part counts
/// @param D3DVECTOR &Position	Create a character position
/// @brief  : Character creation
///				1. Character models node creation
///				2. Character parts creation
///				3. This effect (the character set from the tool)
//--------------------------------------------------------------------------------

bool CObjCHAR::CreateCHAR (char *szName, CCharMODEL *pMODEL, short nCharPartCNT, const D3DVECTOR &Position)
{
	m_bIsVisible = false;

	m_PosBORN	 = Position;
	m_PosCUR     = Position;
	m_pCharMODEL = pMODEL;

	if ( pMODEL == NULL || nCharPartCNT <= 0 )	return false;

	if ( this->LoadModelNODE (szName) )
	{
		this->CreatePARTS( szName );

		m_ppBoneEFFECT = m_pCharMODEL->CreateBoneEFFECT( m_hNodeMODEL );
		this->InsertToScene ();

		this->SetCMD_STOP();

		DropFromSky( Position.x, Position.y );
	 	return true;
	}

	return false;
}


//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @brief  : Delete the character
///				1. This effect (the character set from the tool)
///				2. Character parts deleted
///				3. Character models node deletion
///				4. Engine objects (visiable)
//--------------------------------------------------------------------------------

void CObjCHAR::DeleteCHAR (void)
{
	this->RemoveFromScene ();

	m_pCharMODEL->DeleteBoneEFFECT( m_ppBoneEFFECT );

	short nP;
	for (nP=0; nP<MAX_BODY_PART; nP++)
		this->Del_EFFECT( nP );

	this->DeletePARTS ();
	this->UnloadModelNODE ();

	for (short nI=0; nI<MAX_BODY_PART; nI++)
	{
		SAFE_DELETE_ARRAY( m_phPartVIS[ nI ] );
	}

	if( m_ClanMarkUserDefined )
	{
		m_ClanMarkUserDefined->Release();
		m_ClanMarkUserDefined = NULL;
	}
}


//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @param  t_HASHKEY HashMOTION Motion hash key
/// @param  float fMoveSpeed	 Moving speed
/// @param  int iRepeatCnt		 Repeat count
/// @brief  : Yu lost the motion settings
//--------------------------------------------------------------------------------

void CObjCHAR::Set_UserMOITON (t_HASHKEY HashMOTION, float fMoveSpeed, int iRepeatCnt )
{
	tagMOTION *pMotion = g_MotionFILE.KEY_GetMOTION( HashMOTION );

	Set_UserMOITON( pMotion, fMoveSpeed, iRepeatCnt );
}

//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @param  tagMOTION* pMotion   Motion.
/// @param  float fMoveSpeed	 Moving speed
/// @param  int iRepeatCnt		 Repeat count
/// @brief  : Yu lost the motion settings
//--------------------------------------------------------------------------------

void CObjCHAR::Set_UserMOITON (tagMOTION *pMotion, float fMoveSpeed, int iRepeatCnt )
{
	// If you have the same ongoing motion and motion is not updated
	// Speed setting is eject out of the Chg_CurMOTION ...
	this->Set_ModelSPEED( fMoveSpeed );

	if ( this->Chg_CurMOTION( pMotion ) )
	{

#ifndef __VIRTUAL_SERVER
		_ASSERT( fMoveSpeed >= 0.f && fMoveSpeed < 2000.f );
#endif

		::attachMotion		( this->m_hNodeMODEL, this->Get_ZMOTION()  );
		::setAnimatableSpeed( this->m_hNodeMODEL, 1.0f  );
		::setRepeatCount( this->GetZMODEL(), iRepeatCnt );

		// 본 애니가 없는 경우에도 메쉬 애니가 있을수 있나????
		this->m_pCharMODEL->SetMeshMOTION( m_phPartVIS, this->Get_ActionIDX() );
	}
}


//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @param  short nActionIdx Motion hash key
/// @param  float fMoveSpeed	 Moving speed
/// @param  float fAniSpeed		 Repeat count
/// @param  float bool bAttackMotion	 Repeat count
/// @brief  : Motion settings
/// @bug At first, start attacking places sound here ... Put the information in the frame to start in the correct frame.
//--------------------------------------------------------------------------------

void CObjCHAR::Set_MOTION (short nActionIdx, float fMoveSpeed, float fAniSpeed, bool bAttackMotion, int iRepeatCnt )
{

#ifdef _DEBUG
	//if( IsA( OBJ_MOB ) )
	//{
	//	g_itMGR.AppendChatMsg(	CStr::Printf ("ActionIdx:%d, MoveSpeed:%f, AniSpeed:%f\n", nActionIdx, fMoveSpeed, fAniSpeed ),
	//		IT_MGR::CHAT_TYPE_ALL );
	//}
#endif


	if ( !bAttackMotion && m_bAttackSTART )
	{
		Attack_END ();
	}

	if ( this->Chg_CurMOTION( this->Get_MOTION (nActionIdx) ) )
	{

#ifndef __VIRTUAL_SERVER
//		_ASSERT( fMoveSpeed >= 0.f && fMoveSpeed < 2000.f );
#endif

		this->Set_ModelSPEED( fMoveSpeed );

		::attachMotion		( this->m_hNodeMODEL, this->Get_ZMOTION()  );
		::setAnimatableSpeed( this->m_hNodeMODEL, fAniSpeed  );
		::setRepeatCount	( this->m_hNodeMODEL, iRepeatCnt  );

		// This ANI is even if you don't have a mesh can be anime????
		this->m_pCharMODEL->SetMeshMOTION( m_phPartVIS, this->Get_ActionIDX() );
	}

	::controlAnimatable ( this->m_hNodeMODEL, 0 );
	::controlAnimatable ( this->m_hNodeMODEL, 1 );

}


//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @param  D3DXVECTOR3& Pos The location to renew
/// @brief  : Renew your current location
//--------------------------------------------------------------------------------

void CObjCHAR::ResetCUR_POS( D3DXVECTOR3& Pos )
{
	CGameOBJ::Set_CurPOS( Pos );
	::setPositionVec3( m_hNodeMODEL, m_PosCUR );
}

//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @param  CObjCHAR *pAtkOBJ An attacker could
/// @param  WORD wDamage      Damage
/// @brief  : Apply the treatment damage
//--------------------------------------------------------------------------------

void CObjCHAR::Apply_DAMAGE (CObjCHAR *pAtkOBJ, uniDAMAGE sDamage)	//Hong-Geun: Edit the max damage
{
	if ( this->Get_HP() <= DEAD_HP )
		return;

	if( IsA( OBJ_USER ) )///I have suffered damage if
		g_pAVATAR->SetBattleTime( g_GameDATA.GetGameTime() );

	if( pAtkOBJ && pAtkOBJ->IsA( OBJ_USER ) )///If you attack
		g_pAVATAR->SetBattleTime( g_GameDATA.GetGameTime() );


//	uniDAMAGE Damage;
//	Damage.m_wDamage = wDamage;


	if ( sDamage.m_wACTION & DMG_ACT_DEAD )
	{
		/// Himself from the dead.
		this->Do_DeadAI (pAtkOBJ, sDamage.m_wVALUE );

		/// If you're a noob like attacker and I. .. Pet line as. .. Something like that ... If the motion is not to die.
		/// If you are getting a bump I get when Billy Tea is getting rid of the motion when the settings set up situation. .. Once the User, the path. -04/05/25
		m_bStopDead = ( pAtkOBJ == this );

		Dead();

		if( m_bStopDead && ( this->IsA( OBJ_USER ) == false ) )
		{
			m_pChangeVisibility = new CChangeVisibility( this, 5000, true );
			m_bStopDead = false;
		}

#ifdef _DEBUG
		if( ( pAtkOBJ == this ) && (this->IsA( OBJ_USER )) )
			assert( 0 && "Earl Rae looks like the attacker and I Wen Yu? Well the cause of a bug in the County" );
#endif

		if ( pAtkOBJ )
		{
			if( g_pAVATAR )
			{
				if( g_pAVATAR->Get_INDEX() == pAtkOBJ->Get_INDEX() )
				{
					switch( this->Get_TYPE() )
					{
						case OBJ_MOB:
							sprintf( g_MsgBuf, F_STR_SUCCESS_HUNT, this->Get_NAME() );
							break;

						case OBJ_AVATAR:
							sprintf( g_MsgBuf, F_STR_WIN_PVP, this->Get_NAME() );
							break;
					}

					g_itMGR.AppendChatMsg( g_MsgBuf, IT_MGR::CHAT_TYPE_SYSTEM );
				}
			}

			if ( pAtkOBJ->m_iServerTarget == g_pObjMGR->Get_ServerObjectIndex( this->Get_INDEX() ) )	// Target deleted.
				pAtkOBJ->m_iServerTarget = 0;

			/// Killing AI performed...
			pAtkOBJ->Do_KillAI( this, sDamage.m_wVALUE );

//---------------------------------------------------------------------------------------------
#if defined(_GBC)
			//Park Ji-Ho:: pet mode does not rotate.
			if(GetPetMode() < 0)
				Set_ModelDIR( pAtkOBJ->Get_CurPOS() );
#else
	Set_ModelDIR( pAtkOBJ->Get_CurPOS() );
#endif
//---------------------------------------------------------------------------------------------



			//----------------------------------------------------------------------------------
			/// Penalty experience correction
			//----------------------------------------------------------------------------------
			/// If I lose
			if( this->IsA( OBJ_USER ) == true )
			{
				int iPanalEXP = 0;

				/// It's a monster
				if( pAtkOBJ->IsA( OBJ_MOB ) )
				{
					/// If you're not a monster while cutting the Soviet experience
					if( pAtkOBJ->m_EndurancePack.GetStateFlag() & ING_DEC_LIFE_TIME )
					{
						iPanalEXP = 0;
					}else
					{
						g_pAVATAR->Set_PenalEXP( 3 );
					}
				}
			}
		}

		//------------------------------------------------------------------------------------
		/// delayed exp
		//------------------------------------------------------------------------------------
		CDelayedExp::GetSingleton().GetExp( this->Get_INDEX() );


		/*
		Life events CObjCHAR::Dead Go to ...
		*/

		if( !m_FieldItemList.empty() )
			DropFieldItemFromList();

		this->m_lDeadTIME = 0;

	}
	else
	{
		this->Do_DamagedAI( pAtkOBJ, sDamage.m_wVALUE );

		if ( this->Sub_HP( sDamage.m_wVALUE ) <= 0 )
		{
#ifdef __TEST//This is looking at sending packets that appear to be asking.
			//I did zero damage die?

			if( IsA( OBJ_USER ) )
			{

			}
#endif
			this->Set_HP( 1 );
		}
	}

	if ( pAtkOBJ )
	{
		if ( sDamage.m_wACTION & DMG_ACT_CRITICAL )
		{
#ifdef _DEBUG
			// Critical damage !!!
			g_itMGR.AppendChatMsg(	CStr::Printf ("Critical:: %sDamage to a% s %d Investiture", pAtkOBJ->Get_NAME(), this->Get_NAME (), sDamage.m_wVALUE ),
									IT_MGR::CHAT_TYPE_ALL );
#endif
		}
		else
		{
		}

		if ( sDamage.m_wDamage & DMG_BIT_HITTED )
		{
			// Over the applicable context behavior to fit the ...
			/// Pat Tan, fit and not the behavior. (2/12/10 the Bell fits small ..4 If you do attack the castle was abandoned in the gears.)
			if( this->GetPetMode() > 0 )
			{

			}else
			{
				if ( !(this->Get_STATE() & CS_BIT_INT2) )
				{
					this->Set_MOTION( this->GetANI_Hit() );
					this->Set_STATE( CS_HIT );
				}
			}
		}
	}

//#ifdef	_DEBUG
//	if ( this->IsA( OBJ_USER ) ) {
//		if ( pAtkOBJ ) {
//			int iDist = CD3DUtil::distance ((int)pAtkOBJ->m_PosCUR.x, (int)pAtkOBJ->m_PosCUR.y,
//											(int)this->m_PosCUR.x, (int)this->m_PosCUR.y);
//
//			AddMsgToChatWND( CStr::Printf ("거리 %d [%.0f, %.0f] ~ %s[%d] [ %.0f, %.0f ]",
//					iDist,
//					this->m_PosCUR.x, this->m_PosCUR.y,
//					pAtkOBJ->Get_NAME(),
//					g_pObjMGR->Get_ServerObjectIndex( pAtkOBJ->Get_INDEX() ),
//					pAtkOBJ->m_PosCUR.x, pAtkOBJ->m_PosCUR.y), g_dwGREEN );
//		} else {
//			;
//		}
//	}
//#endif
}


//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @param  CObjCHAR *pTarget Target object
/// @brief  : Skill use. Fencing weapons retention begins throwing ... (To be held was only stuck the sword Attack_Start ... problem)
//--------------------------------------------------------------------------------

bool CObjCHAR::Skill_START (CObjCHAR *pTarget)
{
	if ( m_nActiveSkillIDX )
	{
		/// Proximity immediately fires a sword skill retention trigger)
		if( SKILL_TYPE( m_nActiveSkillIDX ) == SKILL_ACTION_IMMEDIATE )
		{
			// The sword stuck.
			for (short nI=0; nI<2; nI++)
			{
				if ( m_hTRAIL[ nI ] )
				{
					::controlTrail ( m_hTRAIL[ nI ], 0 );	// stop & clear
					::controlTrail ( m_hTRAIL[ nI ], 1 );	// start
				}
			}

		}

		return true;
	}

	return false;
}


//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @param  CObjCHAR *pTarget Target object
/// @brief  : The attack begins ... The sword stuck animation start. ..
//--------------------------------------------------------------------------------

bool CObjCHAR::Attack_START (CObjCHAR *pTarget)
{
	/*
	// Labelling the weapons/sword stuck ...
	CObjCHAR *pTarget = g_pObjMGR->Get_CharOBJ( this->m_iTargetObject );
	int iBulletIDX = Get_BulletNO ();
	if ( pTarget && iBulletIDX ) {
		g_pBltMGR->Add_BULLET( this, pTarget, iBulletIDX );
	}
	*/

	// The sword stuck.
	for (short nI=0; nI<2; nI++)
	{
		if ( m_hTRAIL[ nI ] )
		{
			::controlTrail ( m_hTRAIL[ nI ], 0 );	// stop & clear
			::controlTrail ( m_hTRAIL[ nI ], 1 );	// start
		}
	}

	m_bAttackSTART  = true;
	m_iActiveObject = g_pObjMGR->Get_ClientObjectIndex( m_iServerTarget );

	_ASSERT( m_iServerTarget == g_pObjMGR->Get_ServerObjectIndex( m_iActiveObject ) );


	/// If you start with an attack of last things
	m_EndurancePack.ClearStateByAttack();


	return true;
}


//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @param  bool bStopTrail Sword retention quiescing?
/// @brief  : The end of the attack.
//--------------------------------------------------------------------------------

void CObjCHAR::Attack_END (bool bStopTrail)
{
	// Return to the default animation speed.
	::setAnimatableSpeed ( GetZMODEL(), 1.0 );

	// Black ghosting effects motion stop..
	if ( bStopTrail )
	{
		for (short nI=0; nI<2; nI++)
		{
			if ( m_hTRAIL[ nI ] )
			{
				::controlTrail ( m_hTRAIL[ nI ], 3 );	// no spawn
			}
		}
	}

	m_bAttackSTART = false;
}


//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @param  CObjCHAR *pTarget Target object
/// @brief  : Casting began the behavior. .. (Marked the beginning of skill.) It is very important ... /
///				If you are getting the timer settings the breast. Because of the delay of skill.
//--------------------------------------------------------------------------------

bool CObjCHAR::Casting_START (CObjCHAR *pTarget)
{
	SetCastingState( true );
	/// m_bProcEffectedSkill = false;

	//--------------------------------------------------------------------------------
	/// If you are getting the timer settings the breast.
	//--------------------------------------------------------------------------------
	//if( this->IsA( OBJ_USER ) )
	//{
	//	/// Timer setting
	//	CSkillSlot* pSkillSlot = g_pAVATAR->GetSkillSlot();
	//	CSkill* pSkill = pSkillSlot->GetSkillBySkillIDX( m_nToDoSkillIDX );
	//	if( pSkill )
	//	{
	//		pSkill->SetSkillDelayTime( SKILL_RELOAD_TIME( m_nToDoSkillIDX ) * 200 );
	//	}
	//}

	/// If you start with an attack of last things
	if( SKILL_TYPE( m_nToDoSkillIDX ) == SKILL_ACTION_IMMEDIATE ||
		SKILL_TYPE( m_nToDoSkillIDX ) == SKILL_ACTION_FIRE_BULLET ||
		SKILL_TYPE( m_nToDoSkillIDX ) == SKILL_ACTION_SELF_DAMAGE )
	{
		m_EndurancePack.ClearStateByAttack();
	}

	return true;
}


//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @brief  : End-behavior of the cast ... (To mark the end of skill.) It is very important ... /
//--------------------------------------------------------------------------------

void CObjCHAR::Casting_END ()
{
	/// If the currently active skills. .. Cast the status quo ...
	SetCastingState( false );
	m_nActiveSkillIDX = 0;
	m_nToDoSkillIDX = 0;
}


//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @brief  : Non v start
//--------------------------------------------------------------------------------
void CObjCHAR::MoveStart ()
{
	/// Movement and at the same time automatically with things ...
	m_EndurancePack.ClearStateByMove();
}


//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @brief  : Save a number of bullets.
//--------------------------------------------------------------------------------

int CObjCHAR::Get_BulletNO ()
{
	short nWeaponItem = Get_R_WEAPON();
	if( nWeaponItem )
		return WEAPON_BULLET_EFFECT( nWeaponItem );

	return 0;
}


//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @brief  : Die processing. .. (Entered into force done casting routine. .. This is very important)
///				M_nActiveSkillIDX reset ... If you are dead ... If this flag settings. Casting_End is null and void.
//--------------------------------------------------------------------------------

void CObjCHAR::Dead ()
{
	if ( Get_HP() <= DEAD_HP )
	{
		SetCastingState( false );
		return;
	}


	//----------------------------------------------------------------------------------------------------
	/// Avatar if you die
	//----------------------------------------------------------------------------------------------------
	if( this->Is_AVATAR() )
	{
		CObjAVT* pAVT = ( CObjAVT* )this;
		/// Pet If you're already on.
		if( GetPetMode() >= 0 )
		{
			pAVT->DeleteCart( false );

			//----------------------------------------------------------------------------------------------------
			/// When you die, the output of the effect the destruction of Pat. ..
			//----------------------------------------------------------------------------------------------------
			if( pAVT->IsVisible() )
			{
				int iEffectNO	= PAT_DEAD_EFFECT( pAVT->m_sBodyIDX.m_nItemNo );
				int iSoundNO	= PAT_DEAD_SOUND( pAVT->m_sBodyIDX.m_nItemNo );

				this->ShowEffectOnCharByIndex( iEffectNO, iSoundNO );
			}

			m_btMoveMODE = MOVE_MODE_RUN;
			//PY: this is just for setting animation rates. 
			pAVT->Update_ANI_SPEED();
		}
		else
		{
			//----------------------------------------------------------------------------------------------------
			/// When a character dies, the output of the effect of
			//----------------------------------------------------------------------------------------------------
			SE_CharDie( this->Get_INDEX() );
		}

		/// Reset your shop-related ...
		if( pAVT->IsPersonalStoreMode() )
		{
			pAVT->SetPersonalStoreTitle( NULL );
			g_UIMed.SubPersonalStoreIndex( this->Get_INDEX() );
		}


		/// If I were dead ...
		if( this->IsA( OBJ_USER ) )
		{
			(( CObjUSER*)pAVT)->ClearSummonedMob();
			g_itMGR.ChangeState( IT_MGR::STATE_DEAD );
		}

	}

	//----------------------------------------------------------------------------------------------------
	/// If the effect output die mob
	//----------------------------------------------------------------------------------------------------
	if( this->IsA( OBJ_MOB ) )
	{
		///SE_MobDie( this->Get_INDEX() );
		int iMobDeadEffect = NPC_DEAD_EFFECT( this->Get_CharNO() );
		int iSoundIDX = NPC_DIE_SOUND( this->Get_CharNO() );
		this->ShowEffectOnCharByIndex( iMobDeadEffect, iSoundIDX );

	}

	Set_HP( DEAD_HP );
	SetCMD_DIE ();


	m_nActiveSkillIDX = 0;
	SetCastingState( false );

	m_EndurancePack.ClearEntityPack();
	ClearAllEntityList();

	// Loop prevention.
	::setRepeatCount( m_hNodeMODEL, 1 );
	// Click the ridiculously over dead ...

	if( this->Is_AVATAR() == false )
	{
		/// 2004-11-26 Dead guys being clicked, even ...
		::setCollisionLevel( m_hNodeMODEL, 0 );
	}


	::setModelBlinkCloseMode( this->GetZMODEL(), true );
}


//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @param int iSkillIDX Skill index
/// @param gsv_DAMAGE_OF_SKILL EffectedSkill Skill result information
/// @param bool bDamageOfSkill	Damage as a result? (In the case of damage of the skill range...)
/// @brief  : As a result, the skills to carry out the action frame, Chan Pak-Ah.
//--------------------------------------------------------------------------------

void CObjCHAR::PushEffectedSkillToList( int iSkillIDX, gsv_DAMAGE_OF_SKILL EffectedSkill, int iCasterINT, bool bDamageOfSkill )
{
	stEFFECT_OF_SKILL steffectOfSkill;

	steffectOfSkill.m_dwCreateTime	= g_GameDATA.GetGameTime();
	steffectOfSkill.iSkillIDX		= iSkillIDX;
	steffectOfSkill.bDamageOfSkill	= bDamageOfSkill;
	steffectOfSkill.iCasterINT		= iCasterINT;

	steffectOfSkill.EffectOfSkill = EffectedSkill;
	m_EffectedSkillList.push_back( steffectOfSkill );
}


//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @brief  : As a result the last skill to timeout.
//--------------------------------------------------------------------------------
const int SKILL_PROC_LIMIT = 1000 * 10;
void CObjCHAR::ProcTimeOutEffectedSkill()
{
	stEFFECT_OF_SKILL*	pEffectOfSkill	= NULL;
	DWORD dwElapsedTime = 0;

	std::vector< stEFFECT_OF_SKILL >::iterator begin = m_EffectedSkillList.begin();
	for( ; begin != m_EffectedSkillList.end(); )
	{
		pEffectOfSkill	= &( *begin );

		dwElapsedTime = g_GameDATA.GetGameTime() - pEffectOfSkill->m_dwCreateTime;
		if( dwElapsedTime > SKILL_PROC_LIMIT )
		{
			ProcOneEffectedSkill( pEffectOfSkill );
			begin = m_EffectedSkillList.erase( begin );
#ifdef _DEBUG

			sprintf( g_MsgBuf, "ProcTimeOutEffectedSkill [ 대상 : %s ] ", this->Get_NAME() );
			///assert( 0 && Buf );
			MessageBox( NULL, g_MsgBuf, "WARNING", MB_OK );
#endif //_DEBUG
		}else
			++begin;
	}
}

//----------------------------------------------------------------------------------------------------
/// @param
/// @brief /// Damage of skill When it comes to applying results to reflect the skill and. ..
//----------------------------------------------------------------------------------------------------

void CObjCHAR::ProcEffectOfSkillInDamageOfSkill( int iSkillIDX, int iObjIDX, CObjCHAR* pChar, stEFFECT_OF_SKILL*	pEffectOfSkill )
{
	int iSkillType = SKILL_TYPE( iSkillIDX );

	switch( iSkillType )
	{
		case SKILL_ACTION_SELF_AND_TARGET:
			{
				if( this->IsA( OBJ_USER ) )
				{
					for( int i = 0; i < SKILL_INCREASE_ABILITY_CNT; i++ )
					{
						if( ( 0x01 << i ) & pEffectOfSkill->EffectOfSkill.m_btSuccessBITS )
						{
							int iAbilityType = SKILL_INCREASE_ABILITY( iSkillIDX, i );
							switch( iAbilityType )
							{
								case AT_HP:
									g_pAVATAR->Add_HP( SKILL_INCREASE_ABILITY_VALUE( iSkillIDX, i ) );
									break;
								case AT_MP:
									g_pAVATAR->Add_MP( SKILL_INCREASE_ABILITY_VALUE( iSkillIDX, i ) );
									break;
								case AT_STAMINA:
									g_pAVATAR->AddCur_STAMINA( SKILL_INCREASE_ABILITY_VALUE( iSkillIDX, i ) );
									break;

							}

						}
					}
				}
			}
			break;

		default:
			ApplyEffectOfSkill( iSkillIDX, iObjIDX, pChar, pEffectOfSkill );
			break;
	}
}


//----------------------------------------------------------------------------------------------------
/// @param
/// @brief In fact the character skill apply. ..
/// void CRecvPACKET::Recv_gsv_EFFECT_OF_SKILL ()The same thing over on the side of the ... If you modify this side will come out will be modified.
//----------------------------------------------------------------------------------------------------

void CObjCHAR::ApplyEffectOfSkill( int iSkillIDX, int iObjIDX, CObjCHAR* pEffectedChar, stEFFECT_OF_SKILL*	pEffectOfSkill )
{
	if( pEffectOfSkill->EffectOfSkill.m_btSuccessBITS == 0 )/// The effect of applying the delete right after ... In other words, fail to apply the skill
	{
		/// I apply a skill, a skill only when applying failure message is displayed.
		int iClientObjIndex = g_pObjMGR->Get_ClientObjectIndex( pEffectOfSkill->EffectOfSkill.m_wSpellObjIDX );
		if( iClientObjIndex == g_pAVATAR->Get_INDEX() )
		{
			///AddMsgToChatWND( STR_SKILL_APPLY_FAILED, g_dwRED ,CChatDLG::CHAT_TYPE_SYSTEM);

		}
	}
	else
	{
		///Skill convert up to one status persisted, can be up to n
		for( int i = 0 ; i < SKILL_INCREASE_ABILITY_CNT ; i++ )
		{
			if ( ( 0x01 << i ) & pEffectOfSkill->EffectOfSkill.m_btSuccessBITS )
			{
				int iStateIndex = 0;

				/// Not a simple stats rise persisted type. ..
				/*if( SKILL_TYPE( iSkillIDX ) != SKILL_ACTION_SELF_BOUND &&
					SKILL_TYPE( iSkillIDX ) != SKILL_ACTION_TARGET_BOUND )*/
				{
					iStateIndex = SKILL_STATE_STB( iSkillIDX, i );

					/// 유리상태 해지, 불리상태 해지 등의 상태 해제 스킬들
					if( iStateIndex && STATE_TYPE( iStateIndex ) > ING_CHECK_END )
					{
						pEffectedChar->ProcFlushStateSkill( iStateIndex );
						continue;
					}
				}

				if( iStateIndex != 0 )
				/// If the durable skills. ..
				/*if( SKILL_TYPE( iSkillIDX ) == SKILL_ACTION_SELF_BOUND_DURATION ||
					SKILL_TYPE( iSkillIDX ) == SKILL_ACTION_TARGET_BOUND_DURATION ||
					SKILL_TYPE( iSkillIDX ) == SKILL_ACTION_SELF_STATE_DURATION ||
					SKILL_TYPE( iSkillIDX ) == SKILL_ACTION_TARGET_STATE_DURATION )*/
				{
					/// Add objects properties. ..

					pEffectedChar->AddEnduranceEntity( iSkillIDX,	iStateIndex, SKILL_DURATION( iSkillIDX ), ENDURANCE_TYPE_SKILL ) ;

					/// Status type ...
					int iStateType = STATE_TYPE( iStateIndex );

					/// Condition number 1, number 2, LIST_STATUS. Note the value of the STB and
					if( iStateType == ING_INC_HP || iStateType == ING_INC_MP || iStateType == ING_POISONED )
					{
						pEffectedChar->m_EndurancePack.SetStateValue( iStateType, STATE_APPLY_ABILITY_VALUE( iStateIndex, i ) );
					}
					else
					{
						int iIncValue = 0;
#ifdef __CAL_BUF2
						iIncValue = CCal::Get_SkillAdjustVALUE( ( CObjUSER* )pEffectedChar, iSkillIDX, i, pEffectOfSkill->iCasterINT, iStateType );
#else

						if( pEffectedChar->IsA( OBJ_USER ) )
						{
							iIncValue = CCal::Get_SkillAdjustVALUE( ( CObjUSER* )pEffectedChar, iSkillIDX, i, pEffectOfSkill->iCasterINT, iStateType );
						}
						else
						{
							iIncValue = 1;

							/// Oil is not lost if you can't tell. Monster, ball, try to calculate only this genus.
							if( pEffectedChar->IsA( OBJ_MOB ) )
							{
								int iAbilityValue = 0;
								switch( SKILL_INCREASE_ABILITY( iSkillIDX, i ) )
								{

								case AT_SPEED:
									iAbilityValue = pEffectedChar->GetOri_RunSPEED();
									break;
								case AT_ATK_SPD:
									iAbilityValue = pEffectedChar->GetOri_ATKSPEED();
									break;
								}


								iIncValue = (short)( iAbilityValue * SKILL_CHANGE_ABILITY_RATE( iSkillIDX, i ) / 100.f + SKILL_INCREASE_ABILITY_VALUE( iSkillIDX, i ) );
							}

							//--------------------------------------------------------------------------------------------
							/// If other avatars, ... MAX_HP shall take into consideration ...
							if( pEffectedChar->IsA( OBJ_AVATAR ) )
							{
								int iAbilityValue = 0;
								switch( SKILL_INCREASE_ABILITY( iSkillIDX, i ) )
								{
								case AT_MAX_HP:
									iAbilityValue = pEffectedChar->Get_MaxHP();
									break;
								case AT_SPEED:
									iAbilityValue = pEffectedChar->Get_MoveSPEED();
									break;
								case AT_ATK_SPD:
									iAbilityValue = pEffectedChar->Get_nAttackSPEED();
									break;


								}
								iIncValue = (short)( iAbilityValue * SKILL_CHANGE_ABILITY_RATE( iSkillIDX, i) / 100.f + SKILL_INCREASE_ABILITY_VALUE( iSkillIDX, i) * ( pEffectOfSkill->iCasterINT + 300 ) / 315.f );







							}
							//--------------------------------------------------------------------------------------------

						}
#endif //__CAL_BUF2

						pEffectedChar->m_EndurancePack.SetStateValue( iStateType, iIncValue );

						/// If you take the case of AVATAR State skill stats update ...
						if( pEffectedChar->IsA( OBJ_USER ) )
						{
							(( CObjUSER* )pEffectedChar)->UpdateAbility();
						}
					}


				}else if( SKILL_TYPE( iSkillIDX ) == SKILL_ACTION_SELF_BOUND ||
							SKILL_TYPE( iSkillIDX ) == SKILL_ACTION_TARGET_BOUND )
				{
					/// hp Apply the formula. ..
					int iIncValue = CCal::Get_SkillAdjustVALUE( ( CObjUSER* )pEffectedChar, iSkillIDX, i, pEffectOfSkill->iCasterINT );
					//06. 02. 09 - Kim Joo-Hyun: HP/MP, and will send it to you on the server each second.
					switch( SKILL_INCREASE_ABILITY( iSkillIDX, i ) )
					{
						case AT_HP:
							pEffectedChar->Add_HP( iIncValue );
							break;
						case AT_MP:
							pEffectedChar->Add_MP( iIncValue );
							break;
						case AT_STAMINA:
							/// If you take the case of AVATAR State skill stats update ...
							if( pEffectedChar->IsA( OBJ_USER ) )
							{
								(( CObjUSER* )pEffectedChar)->AddCur_STAMINA( SKILL_INCREASE_ABILITY_VALUE( iSkillIDX, i ) );
							}
							break;
						default:
							g_itMGR.AppendChatMsg( "You'll add a drive..", IT_MGR::CHAT_TYPE_SYSTEM);
							break;
					}
				}
			}
		}
	}
}

//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @param stEFFECT_OF_SKILL*	pEffectOfSkill As a result of skill one..
/// @brief  : Skill when the processing of the results
//--------------------------------------------------------------------------------

void CObjCHAR::ProcOneEffectedSkill( stEFFECT_OF_SKILL*	pEffectOfSkill )
{
	int iSkillIDX		= pEffectOfSkill->iSkillIDX;
	int iObjIDX			= pEffectOfSkill->EffectOfSkill.m_wObjectIDX;

	CObjCHAR *pChar = g_pObjMGR->Get_ClientCharOBJ( iObjIDX, true );

	if( pChar == NULL )
		return;

			/*
	*	If m_nResultVALUE is a magical range of damage.
	*/
	if( pEffectOfSkill->bDamageOfSkill )
	{
		D3DXVECTOR3	pos = pChar->Get_CurPOS();

		pChar->Apply_DAMAGE( this, pEffectOfSkill->EffectOfSkill.m_Damage );

		/// Damage of skill The following reflects the results of applying the skills to..
		ProcEffectOfSkillInDamageOfSkill( iSkillIDX, iObjIDX, pChar, pEffectOfSkill );

		/// Skill blow effects
		// The effect of the strike.
		if ( pChar->IsVisible() )
		{
			g_UIMed.CreateDamageDigit( pEffectOfSkill->EffectOfSkill.m_Damage.m_wVALUE, pos.x, pos.y, pos.z + pChar->m_fStature, pChar->IsA( OBJ_USER ) );

			int iEffectIDX = SKILL_HIT_EFFECT( iSkillIDX );
			if ( iEffectIDX  )
			{
				CEffect *pHitEFT = g_pEffectLIST->Add_EffectWithIDX( iEffectIDX, true );
				if ( pHitEFT )
				{
					if( SKILL_HIT_EFFECT_LINKED_POINT( iSkillIDX ) == INVALID_DUMMY_POINT_NUM )
					{
						pHitEFT->LinkNODE ( pChar->GetZMODEL() );
					}else
					{
						pChar->LinkDummy( pHitEFT->GetZNODE(), SKILL_HIT_EFFECT_LINKED_POINT( iSkillIDX ) );
					}

					pHitEFT->SetParentCHAR( pChar );
					pChar->AddExternalEffect( pHitEFT );

					pHitEFT->UnlinkVisibleWorld ();
					pHitEFT->InsertToScene ();
				}

				if ( SKILL_HIT_SOUND( iSkillIDX ) )
				{
					if ( pChar->IsUSER() )
					{ // If you are not in his avatar 3 D mode output.
						g_pSoundLIST->IDX_PlaySound( SKILL_HIT_SOUND( iSkillIDX ) );
					}
					else
					{
						g_pSoundLIST->IDX_PlaySound3D( SKILL_HIT_SOUND( iSkillIDX ), pChar->Get_CurPOS() );
					}
				}
			}
		}

	}else /// The persistence or ... Changing the status of the skill..
	{
		ApplyEffectOfSkill( iSkillIDX, iObjIDX, pChar, pEffectOfSkill );
	}
}


//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @brief  : Archived results event frame all skill..
//--------------------------------------------------------------------------------

bool CObjCHAR::ProcEffectedSkill()
{
	bool bResult = false;

	stEFFECT_OF_SKILL*	pEffectOfSkill	= NULL;


	for( int i = 0; i < m_EffectedSkillList.size(); i++ )
	{
		pEffectOfSkill	= &( m_EffectedSkillList[ i ] );

		ProcOneEffectedSkill( pEffectOfSkill );

		bResult = true;
	}

	/// After processing all the archived skill results clear
    m_EffectedSkillList.clear();

	SetEffectedSkillFlag( false );
	Log_String( LOG_NORMAL, "ProcEffectedSkill\n" );

	return bResult;
}


//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @param int iattacker An attacker could
/// @param uniDAMAGE wDamage	Applying damage
/// @brief  : Damage to the list of additional sh..
//--------------------------------------------------------------------------------

void CObjCHAR::PushDamageToList( int iAttacker, uniDAMAGE wDamage )
{
	stAPPLY_DAMAGE applyDamage;
	applyDamage.m_Damage			= wDamage;
	applyDamage.m_iAttacker			= iAttacker;
	applyDamage.m_dwCreateTime		= g_GameDATA.GetGameTime();

	m_DamageList.push_back( applyDamage );
}


//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @param int iattacker An attacker could
/// @brief  : Take a list of the current damage damage apply. (The attacker only the same damage, eject)
///				The action follows the last us ... ^^; A lot of bugs in the wind, the cumulative action to..
//--------------------------------------------------------------------------------

uniDAMAGE CObjCHAR::PopCurrentAttackerDamage( int iAttacker )
{
	uniDAMAGE wDamage;
	wDamage.m_wDamage = 0;

	std::vector< stAPPLY_DAMAGE >::iterator begin = m_DamageList.begin();

	for( ; begin != m_DamageList.end() ; )
	{
		if( (*(begin)).m_iAttacker == iAttacker )
		{
			wDamage.m_wVALUE += (*(begin)).m_Damage.m_wVALUE;
			wDamage.m_wACTION |= (*(begin)).m_Damage.m_wACTION;

			begin = m_DamageList.erase( begin );
		}else
		{
			 ++begin;
		}
	}

	return wDamage;
}


//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @param int& iMaxDamage
/// @brief  : In addition to the current list of all damage return..
//--------------------------------------------------------------------------------

uniDAMAGE CObjCHAR::PopTotalDamageFromList( int& iMaxDamage )
{
	uniDAMAGE wDamage;
	wDamage.m_wDamage = 0;
	iMaxDamage = 0;

	for( int i =0; i < m_DamageList.size() ; i++ )
	{
		/// Once the largest value of the strike value output is..
		if( m_DamageList[ i ].m_Damage.m_wVALUE > iMaxDamage )
			iMaxDamage = m_DamageList[ i ].m_Damage.m_wVALUE;

		wDamage.m_wVALUE += m_DamageList[ i ].m_Damage.m_wVALUE;
		wDamage.m_wACTION |= m_DamageList[ i ].m_Damage.m_wACTION;
	}
	m_DamageList.clear();

	return wDamage;
}


//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @param
/// @brief  : All Daisy (dead or ... something like that from happening ...)
//--------------------------------------------------------------------------------

void CObjCHAR::ClearAllDamage()
{
	uniDAMAGE wDamage;

	DWORD dwCurrentTime = g_GameDATA.GetGameTime();

	std::vector< stAPPLY_DAMAGE >::iterator begin = m_DamageList.begin();

	for( ; begin != m_DamageList.end() ; )
	{
		begin = m_DamageList.erase( begin );
	}
}


//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @param
/// @brief  : Time-out handling damage
//--------------------------------------------------------------------------------

void CObjCHAR::ProcDamageTimeOut()
{
	uniDAMAGE wDamage;

	DWORD dwCurrentTime = g_GameDATA.GetGameTime();

	std::vector< stAPPLY_DAMAGE >::iterator begin = m_DamageList.begin();

	for( ; begin != m_DamageList.end() ; )
	{
		/// 5 seconds jinassdamyeon
		if( ( dwCurrentTime - (*begin).m_dwCreateTime ) > 5000 )
		{
			this->Apply_DAMAGE( g_pObjMGR->Get_CharOBJ( (*begin).m_iAttacker, true ), (*begin).m_Damage );
			begin = m_DamageList.erase( begin );
		}else
		{
			++begin;
		}
	}
}


//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @param  WORD wDamage Applying damage
/// @brief  : Blow the Chi appear immediately
//--------------------------------------------------------------------------------

void CObjCHAR::CreateImmediateDigitEffect( DWORD wDamage )
{
	/// Batting figures applied directly. .. There is no proper place once here..
	uniDAMAGE Damage;
	Damage.m_wDamage = wDamage;
	D3DXVECTOR3	pos = this->Get_CurPOS();
	g_UIMed.CreateDamageDigit( Damage.m_wVALUE, pos.x, pos.y, pos.z + this->m_fStature, this->IsA( OBJ_USER ) );
}


//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @param  gsv_DAMAGE_OF_SKILL stDamageOfSkill damage of kill Information
/// @brief  : Damage_of_Skill => General switch to storage damage..
//--------------------------------------------------------------------------------

void CObjCHAR::ConvertDamageOfSkillToDamage( gsv_DAMAGE_OF_SKILL stDamageOfSkill )
{
	uniDAMAGE		Damage;

	CObjCHAR *pAtkOBJ;

	pAtkOBJ = g_pObjMGR->Get_ClientCharOBJ( stDamageOfSkill.m_wSpellObjIDX, false );

	if ( pAtkOBJ )
	{
		/// Damage Registration..
		PushDamageToList( g_pObjMGR->Get_ClientObjectIndex( stDamageOfSkill.m_wSpellObjIDX ),
							stDamageOfSkill.m_Damage );
		return;
	}
}


//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @param
/// @brief  : Item drop..
//--------------------------------------------------------------------------------

void CObjCHAR::DropFieldItemFromList()
{
	for( int i = 0; i < m_FieldItemList.size(); i++ )
	{
		//gsv_ADD_FIELDITEM& ItemInfo = m_FieldItemList[ i ];

		int iItemOBJ = g_pObjMGR->Add_GndITEM( 	m_FieldItemList[ i ].m_wServerItemIDX,
												m_FieldItemList[ i ].m_ITEM,
												m_FieldItemList[ i ].m_PosCUR, this->m_PosCUR.z, true );

		if ( iItemOBJ )
		{
			CObjITEM *pITEM = (CObjITEM *)g_pObjMGR->Get_OBJECT( iItemOBJ );
			pITEM->m_wOwnerServerObjIDX = m_FieldItemList[ i ].m_wOwnerObjIDX;
			pITEM->m_wRemainTIME		= ITEM_OBJ_LIVE_TIME;

			pITEM->InsertToScene ();

			// m_pRecvPacket->m_gsv_ADD_FIELDITEM.m_wOwnerObjIDX;
			// m_pRecvPacket->m_gsv_ADD_FIELDITEM.m_wRemainTIME;

			tagITEM sITEM = m_FieldItemList[ i ].m_ITEM;

			/*if ( sITEM.m_cType != ITEM_TYPE_MONEY )
				AddMsgToChatWND(CStr::Printf ("Item% s drop Type: %d, NO: %d ",
														ITEM_NAME( sITEM.m_cType, sITEM.m_nItemNo ),
														sITEM.m_cType, sITEM.m_nItemNo ), g_dwBLUE ,CChatDLG::CHAT_TYPE_SYSTEM);
			else
				AddMsgToChatWND(CStr::Printf ("The money drop %d ", sITEM.m_nItemNo), g_dwBLUE ,CChatDLG::CHAT_TYPE_SYSTEM);*/
		}
	}

	m_FieldItemList.clear();
}


//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @param
/// @brief  : When a bullet was shot skill handling..
//--------------------------------------------------------------------------------

bool CObjCHAR::ProcessSkillHit( CObjCHAR *pFromOBJ )
{
	if( pFromOBJ == NULL )
		return false;

	return pFromOBJ->ProcEffectedSkill();
}


//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @param
/// @brief  : A mob summoned. .. (It is currently focused on not just the frame, creating a mob summoned. .. ordered with the server synchronization issues
///				On the server, you can order an attack on mobs that are generated on the client, not yet created..
//--------------------------------------------------------------------------------

void CObjCHAR::SetSummonMobInfo( gsv_MOB_CHAR& MobInfo )
{
	memcpy( &m_SummonMob, &MobInfo, sizeof( gsv_MOB_CHAR ) );
	m_bHaveSummonedMob = true;

	SetEffectedSkillFlag( true );
}

//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @param
/// @brief  : A mob summoned. .. (It is currently focused on not just the frame, creating a mob summoned. .. ordered with the server synchronization issues
///				On the server, you can order an attack on mobs that are generated on the client, not yet created..
//--------------------------------------------------------------------------------

void CObjCHAR::SummonMob()
{
	if( m_bHaveSummonedMob )
	{
		D3DVECTOR PosCUR;

		PosCUR.x = m_SummonMob.m_PosCUR.x;
		PosCUR.y = m_SummonMob.m_PosCUR.y;
		PosCUR.z = 0.0f;


		short nCObj;

		if ( NPC_TYPE( m_SummonMob.m_nCharIdx ) != 999 ) 
		{
			nCObj = g_pObjMGR->Add_MobCHAR( m_SummonMob.m_wObjectIDX,
											m_SummonMob.m_nCharIdx,
											PosCUR,
											m_SummonMob.m_nQuestIDX,
											m_SummonMob.m_btMoveMODE);
			if ( g_pNet->Recv_tag_ADD_CHAR( nCObj, &m_SummonMob ) )
			{
				short nOffset = sizeof( gsv_MOB_CHAR );
				g_pNet->Recv_tag_ADJ_STATUS( nOffset, &m_SummonMob );
			}
		}
	}

	m_bHaveSummonedMob = false;
}


//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @param  CObjCHAR *pFromOBJ An attacker could
/// @param  int iEffectIDX		Bullet number
/// @param  int iSkillIDX		Skill, the ... The skill number
/// @param  bool bIsSkillEffect	This is the skill of magic?
/// @param  bool bIsBulletHit
/// @brief  : When the behavioral treatment of a. .. It gives damage. Effects-discharge or whatever.
/// @todo Reg_DAMAGE detects the need to determine the type of strike...
//--------------------------------------------------------------------------------

bool CObjCHAR::Hitted (CObjCHAR *pFromOBJ, int iEffectIDX, int iSkillIDX, bool bIsSkillEffect, bool bIsBulletHit, bool bJustEffect )
{
	if ( NULL == pFromOBJ )
		return true;

//-------------------------------------------------------------------------
///Park Ji-Ho: the attacker to determine the user argument. :: Effect number
#define HIT_AROA_EFF 1613

	BOOL IsAcceptAroa = FALSE;
   if(IsApplyNewVersion())
   {
	   if(pFromOBJ->IsUSER() || pFromOBJ->IsPET())
			IsAcceptAroa = TRUE;
   }
//-------------------------------------------------------------------------

	//--------------------------------------------------------------------------------
	/// When hit with the effects..
	//--------------------------------------------------------------------------------
   //06. 04. 05 - Kim Joo-Hyun: when applied after damage because this appears to be a loose
   //There is a bug with respect to the interface. Bug fix in order to check whether the first sleep sleep off.
	 if(!m_EndurancePack.GetFlagEntitySleep())
		m_EndurancePack.ClearStateByHitted();

	m_EndurancePack.SetFlagEntitySleep(false);

	D3DXVECTOR3	pos = this->Get_CurPOS();

	//--------------------------------------------------------------------------------
	/// When to apply effects.. ApplyDamage Of course, it does not blow to Chi too..
	//--------------------------------------------------------------------------------
	if( !bJustEffect )
	{
		if ( this->m_lDeadTIME )
		{
			/// 2003. 12. 26 Last modified herbal treatment...
			/// PTarget is a must at this time.
			/// When this collapsed and killed over a beat? Or somebody would collapse if hit???
			// if ( pTarget->m_DeadDAMAGE.m_nTargetObjIDX == this->Get_INDEX() )
			{
				// CObjCHAR *pAttacker = g_pObjMGR->Get_CharOBJ( this->m_DeadDAMAGE.m_nTargetObjIDX, true );
				this->Apply_DAMAGE( pFromOBJ, this->m_DeadDAMAGE.m_wDamage );
				this->m_lDeadTIME = 0;

				uniDAMAGE wDamage;
				wDamage = this->m_DeadDAMAGE.m_wDamage;
				g_UIMed.CreateDamageDigit( wDamage.m_wVALUE, pos.x, pos.y, pos.z + m_fStature, this->IsA( OBJ_USER ) );

				if( wDamage.m_wACTION & DMG_ACT_CRITICAL )
				{
					if( iEffectIDX )
					{
						int iCriticalEffect = EFFECT_HITTED_CRITICAL( iEffectIDX );

						//Oh Loa effects
						ChangeHittedEffect(pFromOBJ,IsAcceptAroa,iCriticalEffect);

						CEffect *pHitEFT = g_pEffectLIST->Add_EffectWithIDX( iCriticalEffect, true );
						if ( pHitEFT )
						{
							///D3DXVECTOR3 vec( pos.x, pos.y, pos.z + m_fStature );
							///pHitEFT->SetPOSITION( vec );

							pHitEFT->SetPOSITION( this->Get_CurPOS() );

							pHitEFT->InsertToScene();
						}
					}
				}
			}
		}else
		{
			int iTotalDmage = 0;
			uniDAMAGE stDmage;

			/// The damage caused by the Damage of skill and skill come me.
			if( !bIsSkillEffect )
			{
				/// Processing of received damage from the server


				///
				/// In the end, fit guy I or
				/// I hit the guy or hitting..
				/// If this slap my fat (Finally I slap) ... The processing of the..
				///

				/// If I beat these guys, or depends on the server.
				int iTargetObject = g_pObjMGR->Get_ClientObjectIndex( g_pAVATAR->Get_TargetIDX() );
				//--------------------------------------------------------------------------------
				/// Avatar If you have to ride the current Pat Pat's targeted inspection
				int iPetTargetObject = 0;
				if( g_pAVATAR->GetPetMode() > 0 )
					 iPetTargetObject = g_pObjMGR->Get_ClientObjectIndex( g_pAVATAR->m_pObjCART->Get_TargetIDX() );
				//--------------------------------------------------------------------------------
				if( this->IsA( OBJ_USER ) ||
					( this->Get_INDEX() == iTargetObject ) ||
					( this->Get_INDEX() == iPetTargetObject ) )
				{
					//stDmage = PopTotalDamageFromList( iTotalDmage );
					stDmage = PopCurrentAttackerDamage( pFromOBJ->Get_INDEX() );
					this->Apply_DAMAGE( pFromOBJ, stDmage );

					g_UIMed.CreateDamageDigit( stDmage.m_wVALUE, pos.x, pos.y, pos.z + m_fStature, this->IsA( OBJ_USER ) );

					/// Batting: jiggle..
					if( stDmage.m_wVALUE )
					{
						if( this->GetPetMode() < 0 )
							this->StartVibration();
						else
						{
							//Park Ji-Ho:: pet mode set to flicker
							m_IsCartVA = GetPetMode() ? TRUE : FALSE;
						}
					}

					if( stDmage.m_wACTION & DMG_ACT_CRITICAL )
					{
						/// Camera vibration..
						if ( IsA( OBJ_USER ) )
						{
							D3DXVECTOR3 vMin(-10, -10, -10), vMax(10, 10, 50);
							::shakeCamera( g_pCamera->GetZHANDLE(), vMin, vMax, 200 ); // Camera shake.
						}

						if( iEffectIDX )
						{
							int iCriticalEffect = EFFECT_HITTED_CRITICAL( iEffectIDX );

							///Oh Loa effects
							ChangeHittedEffect(pFromOBJ,IsAcceptAroa,iCriticalEffect);

							CEffect *pHitEFT = g_pEffectLIST->Add_EffectWithIDX( iCriticalEffect, true );
							if ( pHitEFT )
							{
								///D3DXVECTOR3 vec( pos.x, pos.y, pos.z + m_fStature );
								///pHitEFT->SetPOSITION( vec );

								pHitEFT->SetPOSITION( this->Get_CurPOS() );

								pHitEFT->InsertToScene();
							}
						}
					}

				}else	/// The client must be handled properly in the damage..
				{
					///
					/// Something is. .. In unusual circumstances, I'm not a guy to attack the situation but received damage from the server.
					///
					if( !m_DamageList.empty() )
					{
						stDmage = PopTotalDamageFromList( iTotalDmage );
						this->Apply_DAMAGE( pFromOBJ, stDmage );

						g_UIMed.CreateDamageDigit( stDmage.m_wVALUE, pos.x, pos.y, pos.z + m_fStature, this->IsA( OBJ_USER ) );

						/// Batting: jiggle
						if( stDmage.m_wVALUE )
						{
							if( this->GetPetMode() < 0 )
								this->StartVibration();
						}
					}else
					/// Purely on the client..
					{
						if( iSkillIDX )
							stDmage.m_wDamage = CCal::Get_SkillDAMAGE ( pFromOBJ, this, iSkillIDX, 1 );
						else
							stDmage.m_wDamage = CCal::Get_DAMAGE ( pFromOBJ, this, 1 );
					}

					iTotalDmage = stDmage.m_wVALUE;
				}
			}else
			{
				/// If the bullet was shot ... BIsSkillEffect settings allows the process under.

				ProcessSkillHit( pFromOBJ );

				{
					//----------------------------------------------------------------------------------------------------
					/// @brief In the case of a long-distance bullet such as the mana Vault skill damage comes away.
					//----------------------------------------------------------------------------------------------------
					if( SKILL_TYPE( iSkillIDX ) == SKILL_ACTION_FIRE_BULLET )
					{
						/// If I beat these guys, or depends on the server.
						int iTargetObject = g_pObjMGR->Get_ClientObjectIndex( g_pAVATAR->Get_TargetIDX() );
						if( this->IsA( OBJ_USER ) || ( this->Get_INDEX() == iTargetObject ) )
						{
							//stDmage = PopTotalDamageFromList( iTotalDmage );
							stDmage = PopCurrentAttackerDamage( pFromOBJ->Get_INDEX() );
							this->Apply_DAMAGE( pFromOBJ, stDmage );

							g_UIMed.CreateDamageDigit( stDmage.m_wVALUE, pos.x, pos.y, pos.z + m_fStature, this->IsA( OBJ_USER ) );
							/// Batting: jiggle
							if( stDmage.m_wVALUE )
							{
								if( this->GetPetMode() < 0 )
									this->StartVibration();
							}
						}
					}
				}
			}
		}
	}


	///
	///	Miss If you don't take the job.
	///

	//if( stDmage.m_wVALUE <= 0 )
	//	return true;

	// Blow effects.
	if ( this->IsVisible() )
	{
		// short nEffectIDX = WEAPON_DEFAULT_EFFECT( pFromOBJ->Get_R_WEAPON() );
		if ( iEffectIDX  )
		{
			int iHitEffect = iEffectIDX;

			/// skill In a direct hit from file_effect
			/// General attack weapon table. .. In other words, from the ... List_Effect
			if( !bIsSkillEffect )
				iHitEffect = EFFECT_HITTED_NORMAL( iEffectIDX );



			/// I know if you go off the total matching....
			if( bIsSkillEffect && bIsBulletHit )
			{
				iHitEffect = SKILL_HIT_EFFECT( iSkillIDX );

				if ( iHitEffect )
				{
					///Oh Loa effects
					ChangeHittedEffect(pFromOBJ,IsAcceptAroa,iHitEffect);

					CEffect *pHitEFT = g_pEffectLIST->Add_EffectWithIDX( iHitEffect, true );
					if ( pHitEFT )
					{
						if( SKILL_HIT_EFFECT_LINKED_POINT( iSkillIDX ) == INVALID_DUMMY_POINT_NUM )
							pHitEFT->LinkNODE ( this->GetZMODEL() );
						else
							this->LinkDummy( pHitEFT->GetZNODE(), SKILL_HIT_EFFECT_LINKED_POINT( iSkillIDX ) );


						///pHitEFT->SetParentCHAR( this );
						///AddExternalEffect( pHitEFT );

						pHitEFT->UnlinkVisibleWorld ();
						pHitEFT->InsertToScene ();
					}
				}
			}else
			// If the General..
			{
				if ( iHitEffect )
				{
					///Oh Loa effects
					ChangeHittedEffect(pFromOBJ,IsAcceptAroa,iHitEffect);

					CEffect *pHitEFT = g_pEffectLIST->Add_EffectWithIDX( iHitEffect, true );
					if ( pHitEFT )
					{
						pHitEFT->LinkNODE ( this->GetZMODEL() );

						///pHitEFT->SetParentCHAR( this );
						///AddExternalEffect( pHitEFT );

						pHitEFT->UnlinkVisibleWorld ();
						pHitEFT->InsertToScene ();
					}
				}
			}


			//----------------------------------------------------------------------------------------------------
			/// @brief Hit sound 출력
			//----------------------------------------------------------------------------------------------------
			int iHitSound = 0;
			if( !bIsSkillEffect )
			{
				iHitSound = EFFFCT_HIT_SND_IDX( iEffectIDX );
			}else
			{
				iHitSound = SKILL_HIT_SOUND( iSkillIDX );
			}

			if ( iHitSound )
			{
				g_pSoundLIST->IDX_PlaySound3D( iHitSound, Get_CurPOS() );
			}
			//----------------------------------------------------------------------------------------------------


		}
	}


#ifdef	__VIRTUAL_SERVER
	g_pNet->Send_cli_DAMAGE( pFromOBJ, this, stDmage.m_wDamage );
#endif
	return true;
}


//--------------------------------------------------------------------------------
/// class : ChangeHittedEffect
/// Park Ji-Ho: Loa and cart set the skill effects.
//--------------------------------------------------------------------------------
void CObjCHAR::ChangeHittedEffect(CObjCHAR* pAttackObj,BOOL bA,int& hitIDX)
{

	//Oh Loa effects
	if(bA)
	{
		CObjCHAR* pTg = pAttackObj;

		//If you are a pet...
		if(pAttackObj->IsPET())
		{
			//A parent brings the avatar class.
			CObjCART* pCart = (CObjCART*)pAttackObj;
			 if(pCart)
				 pTg = pCart->GetParent();
		}

		//At present? As the effects of a blow will set to the effect the AH.
		if(pTg && bA && pTg->SetAroaState())
			hitIDX = HIT_AROA_EFF;
	}

}


//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @brief  : Save the street attack. If an attack of the skill to the skill at the table were the General attack of the weapons in the weapon off the table
//--------------------------------------------------------------------------------

int CObjCHAR::Get_AttackRange()
{
	/// If you enter distance attack skill skill or weapon distance distance..
	if ( this->m_nToDoSkillIDX > 0 )
	{
		if( this->m_nToDoSkillIDX < g_SkillList.Get_SkillCNT() )
		{
			if(	SKILL_DISTANCE( this->m_nToDoSkillIDX ) )
			{
				return SKILL_DISTANCE( this->m_nToDoSkillIDX );
			}
		}else
			assert( 0 && "Get_AttackRange Failed[ ToDoSkillIDX is invalid ]" );
	}

	if ( Get_R_WEAPON() )
	{
		return WEAPON_ATTACK_RANGE( Get_R_WEAPON() ) + ( Get_SCALE() * 120 );
	}

	// Default attack range...
	return ( Def_AttackRange () + ( Get_SCALE() * 120 ) );
}


//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @brief  :
//--------------------------------------------------------------------------------

void CObjCHAR::Set_EMOTION (short nEmotionIDX)
{
	;
}


//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @brief  :
//--------------------------------------------------------------------------------

void CObjCHAR::Special_ATTACK ()
{
	// CObjCHAR *pTarget = Get_TARGET ();

	;
	;
}


//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @brief  : Warp ... Renew your current location and warp ... Terrain update..
//--------------------------------------------------------------------------------

void CObjCHAR::Warp_POSITION (float fX, float fY)
{
	SetCMD_STOP ();

	m_PosCUR.x = fX;
	m_PosCUR.y = fY;

	g_pTerrain->SetCenterPosition( fX, fY );

	DropFromSky( fX, fY );
}


//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @brief  : The heavens and crashes ... That position..
//--------------------------------------------------------------------------------

void CObjCHAR::DropFromSky ( float fX, float fY )
{
	m_PosCUR.x = fX;
	m_PosCUR.y = fY;
	m_PosCUR.z = ::g_pTerrain->GetHeightTop(fX, fY);

	::setPosition ( m_hNodeMODEL, m_PosCUR.x, m_PosCUR.y, m_PosCUR.z );
	::savePrevPosition( m_hNodeMODEL );

}


//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @brief  : Rotate the model in a particular direction.
//--------------------------------------------------------------------------------

void CObjCHAR::Set_ModelDIR (t_POSITION &PosToView, bool bImmediate )
{
	::setModelDirectionByPosition ( this->m_hNodeMODEL, PosToView.x, PosToView.y );
}


//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @brief  : Rotate the model in a particular direction.
//--------------------------------------------------------------------------------

void CObjCHAR::Set_ModelDIR( float fAngleDegree, bool bImmediate )
{
	::setModelDirection( this->m_hNodeMODEL, fAngleDegree, bImmediate );
}

//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @brief  : Rotate the model in a particular direction.
//--------------------------------------------------------------------------------

void CObjCHAR::Add_ModelDIR( float fAngleDegree )
{
	float fDir = ::getModelDirection( this->m_hNodeMODEL );

	::setModelDirection( this->m_hNodeMODEL, fAngleDegree + fDir, true );
}


//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @brief  : Finishing move..
//--------------------------------------------------------------------------------

void CObjCHAR::Move_COMPLETED ()
{
	/*
	Characters is not a client, you can determine the speed of the...
	if ( !this->IsUSER() )
	{
		this->m_fMoveCmPerSec = this->Get_DefaultSPEED();
	}
	*/
	/// x-y Forced to move but don't size with cells. Or in the possibility of a transparent mob.
	/// z The position is the same as the previous location in the first set.
	m_PosGOTO.z = m_PosCUR.z;
	m_PosCUR = m_PosGOTO;

	::setPosition (this->m_hNodeMODEL, m_PosCUR.x, m_PosCUR.y, m_PosCUR.z);

	/// 2003. 12.16 :: Packet after receiving compensation changes the current state of the movement that was speed.
	Set_AdjustSPEED( this->Get_DefaultSPEED() );
}


//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @brief : Started at the height of the position value is set to a value from the server to enjoy your height,.
//--------------------------------------------------------------------------------

void CObjCHAR::Reset_Position ()
{
	return;

	///m_PosCUR.z = m_PosGOTO.z; // Starting point is the value for the height of the location where the value is actually a z.
	///::setPositionVec3( this->m_hNodeMODEL, m_PosCUR );
}


//----------------------------------------------------------------------------------------
/// class : CObjCHAR
/// @brief  : More HP from RECOVER_STATE_CHECK_TIME intervals Get_RecoverHP.
///			- When HP's client on the server at a certain amount per car of this case.
///			- If you are good at: 50% increase while arua 2005/07/12-nAvy
//----------------------------------------------------------------------------------------

void CObjCHAR::RecoverHP( short nRecoverMODE )
{
#ifdef _NoRecover
	Add_HP( m_ReviseHP );
#else
	int iRecoverHP = Get_RecoverHP( nRecoverMODE );
	int iAruaAddHP = (m_IsAroa)?iRecoverHP >> 1:0;


	Add_HP( iRecoverHP + iAruaAddHP );


	//	_RPT2( _CRT_WARN,"RecoverHP:%d, AruaAddHP:%d\n", iRecoverHP, iAruaAddHP );

	int iReviseConstHP = iRecoverHP;//Get_MaxHP() / 30;///To reduce the difference between the server and every tick correction value for
	if( iReviseConstHP < 10 )
		iReviseConstHP = 10;

	if( m_ReviseHP > 0 )
	{
		if( m_ReviseHP > iReviseConstHP )
		{
			Add_HP( iReviseConstHP );
			_RPT1( _CRT_WARN,"Add Revise HP %d\n", iReviseConstHP );
			m_ReviseHP -= iReviseConstHP;
		}
		else
		{
			Add_HP( m_ReviseHP );
			_RPT1( _CRT_WARN,"Add Revise HP %d\n", m_ReviseHP );
			m_ReviseHP = 0;
		}
	}
	else if( m_ReviseHP < 0 )
	{
		if( abs(m_ReviseHP) > iReviseConstHP )
		{
			Add_HP( -iReviseConstHP );
			_RPT1( _CRT_WARN,"Add Revise HP %d\n", -iReviseConstHP );
			m_ReviseHP += iReviseConstHP;
		}
		else
		{
			Add_HP( m_ReviseHP );
			_RPT1( _CRT_WARN,"Add Revise HP %d\n", m_ReviseHP );
			m_ReviseHP = 0;
		}
	}
#endif

	int iMaxHP = Get_MaxHP();
	if( Get_HP() > Get_MaxHP() )
		Set_HP( Get_MaxHP() );
}

//----------------------------------------------------------------------------------------
/// class : CObjCHAR
/// @brief  : RECOVER_STATE_CHECK_TIME Get_ReocverMP MP from more intervals.
///			- MP on the server when I got in the car by a certain amount of clients here.
///			- If you are good at: 50% increase while arua 2005/07/12-nAvy
//----------------------------------------------------------------------------------------

void CObjCHAR::RecoverMP( short nRecoverMODE )
{
#ifdef _NoRecover
	Add_MP(m_ReviseMP);

#else
	int iRecoverMP = Get_RecoverMP( nRecoverMODE );
	int iAruaAddMP = (m_IsAroa)?iRecoverMP >> 1:0;

	Add_MP( iRecoverMP + iAruaAddMP );

	int iReviseConstMP = iRecoverMP;//Get_MaxMP() / 30;///To reduce the difference between the server and every tick correction value for
	if( iReviseConstMP < 10 )
		iReviseConstMP = 10;

	if( m_ReviseMP > 0 )
	{
		if( m_ReviseMP > iReviseConstMP )
		{
			Add_MP( iReviseConstMP );
			m_ReviseMP -= iReviseConstMP;
		}
		else
		{
			Add_MP( m_ReviseMP );
			m_ReviseMP = 0;
		}
	}
	else if( m_ReviseMP < 0 )
	{
		if( abs(m_ReviseMP) > iReviseConstMP )
		{
			Add_MP( -iReviseConstMP );
			m_ReviseMP += iReviseConstMP;
		}
		else
		{
			Add_MP( m_ReviseMP );
			m_ReviseMP = 0;
		}
	}
#endif


	if( Get_MP() > Get_MaxMP() )
		Set_MP( Get_MaxMP() );
}


//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @brief  : Character height. The various conflicts here.
//--------------------------------------------------------------------------------

void CObjCHAR::Adjust_HEIGHT ()
{
	m_pCollision->UpdateHeight( this );

#ifdef __NPC_COLLISION
	m_pCollision2NPC->CollisionDetectionAvatar2Mob(this);
#endif
}


//----------------------------------------------------------------------------------------------------
/// @param
/// @brief In the middle of a certain action would have been to replace the equipment (especially weapons) and replace it with the converted equipment fits in motion
//----------------------------------------------------------------------------------------------------

void CObjCHAR::UpdateMotionByChangingEquip()
{
	if( this->IsA( OBJ_AVATAR ) || this->IsA( OBJ_USER ) )
	{
		CObjAVT* pAvt = (CObjAVT*)this;
		/// If you have a weapon to change after..
		if( pAvt->GetUpdateMotionFlag() || pAvt->GetChangeWeaponR() || pAvt->GetChangeWeaponL() )
		{
			/// New motion settings for the vertical cloud command settings
			switch( pAvt->Get_COMMAND() )
			{
				case CMD_STOP:
					{
						pAvt->SetCMD_STOP( );
					}
					break;
				case CMD_MOVE:
					{
						//pAvt->SetCMD_MOVE( pAvt->m_PosGOTO.x, pAvt->m_PosGOTO.y, pAvt->m_bRunMODE );
						pAvt->Set_MOTION( pAvt->GetANI_Move(), pAvt->m_fCurMoveSpeed, pAvt->m_fRunAniSPEED );
					}
					break;
				case CMD_ATTACK:
					{
						pAvt->SetCMD_ATTACK( pAvt->m_iServerTarget );
						//pAvt->Set_MOTION( pAvt->GetANI_Attack(), pAvt->m_fCurMoveSpeed, pAvt->m_fRunAniSPEED );
					}
					break;
				case CMD_DIE:
					break;
				case CMD_PICK_ITEM:
					break;
				case CMD_SKILL2SELF:
					break;
				case CMD_SKILL2OBJ:
					break;
				case CMD_SKILL2POS:
					break;
				case CMD_RUNAWAY:
					break;
			}

			//pAvt->Update ();

			/// if pCHAR is my avatar, update ability
			if( pAvt->IsA( OBJ_USER ) )
			{
				((CObjUSER*)pAvt)->UpdateAbility();
			}

			pAvt->SetChangeWeaponR( 0 );
			pAvt->SetChangeWeaponL( 0 );

			pAvt->SetUpdateMotionFlag( false );
		}
	}
}


//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @brief  : Character handling functions
/// @todo Deselect test code...//if (this->Get_TYPE() != OBJ_MOB )
/// @todo View list Redirect..
//--------------------------------------------------------------------------------

int CObjCHAR::Proc (void)
{
	/*
	*
	* for debugging...
	*
	*/

	//if( m_iCastingStartTime != NULL )
	//{
	//	if( ( g_GameDATA.GetGameTime() - m_iCastingStartTime ) > SKILL_PROC_LIMIT )
	//	{
	//		///Casting_END();
	//		assert( 0 && "Casting time expired" );
	//	}
	//}

#if defined(_GBC)
//--------------------------------------------------------------------------------
		//Park Ji-Ho: If the user on the rear of the cart to cart tandem users update the location.
		if(this->GetPetMode() > -1)
		{
			//If you have any users in the back now...
          	if(m_pObjCART->m_pRideUser)
			{
				float Pos1[3] = { 0 };

				//2 seater brings the value of the location of the pile.
				::getDummyPosition(m_pObjCART->GetZMODEL(),10,Pos1);
				//2 seater user location update
				m_pObjCART->m_pRideUser->Set_CurPOS(D3DXVECTOR3(Pos1[0],Pos1[1],Pos1[2]));
			}
		}
//--------------------------------------------------------------------------------
#endif


	//--------------------------------------------------------------------------------
	/// Object vibe migration update: Let's rock ~ ~ ~ ~
	//--------------------------------------------------------------------------------
    m_ObjVibration.Proc();



	//--------------------------------------------------------------------------------
	/// Updated the external decoration
	//--------------------------------------------------------------------------------
	m_EndurancePack.Update();


	//--------------------------------------------------------------------------------
	/// visibility Change
	/// Upon the death of a character is the motion at all, updates are ... This is associated with the code below? -04/5/25
	//--------------------------------------------------------------------------------
	if( m_pChangeVisibility != NULL )
	{
		/// if expired?
		if( m_pChangeVisibility->ProcessVisibility( g_GameDATA.GetElapsedFrameTime() ) == false )
		{
			delete m_pChangeVisibility;
			m_pChangeVisibility = NULL;

			this->m_lDeadTIME = 0;
			return 0;
		}

		assert( !(this->IsA( OBJ_USER )) && " What invisible why wrongdoing to match oil agent t? " );

		return 1;
	}


	//--------------------------------------------------------------------------------
	/// Update check frame
	//--------------------------------------------------------------------------------
	DWORD dwCurrentTime = g_GameDATA.GetGameTime();
	m_dwFrameElapsedTime = dwCurrentTime - m_dwLastRecoveryUpdateTime;		/// The current flow between the time frame from the previous frame case.
	m_dwLastRecoveryUpdateTime = dwCurrentTime;




	//--------------------------------------------------------------------------------
	/// Lee cover specific handling
	/// 04/5/28 CObjAVT::Proc Go to..
	//--------------------------------------------------------------------------------
	//if( this->Is_AVATAR() )
	//{
	//	switch ( Get_COMMAND() )
	//	{
	//		case CMD_SIT :
	//			this->RecoverHPnMP( RECOVER_STATE_SIT_ON_GROUND );
	//			break;
	//		default:
	//			/// 명령이 sit 에서 바뀌거나.. 연속적인 sit 처리가 아니라면 시간 리셋..
	//			m_dwElapsedTime = 0;
	//			break;
	//	}

	//	//--------------------------------------------------------------------------------
	//	/// Time-dependent action mode processing..
	//	/// if This way to lighten the moving door.. 04/5/28
	//	//--------------------------------------------------------------------------------
	//	///if( this->IsA( OBJ_AVATAR ) || this->IsA( OBJ_USER ) )
	//	{
	//		m_ChangeActionMode.Proc();
	//	}
	//}

#ifdef _DEBUG
	/*
	if( ::getUserData( GetZMODEL() ) )
	{
		_ASSERT( ::getUserData( GetZMODEL() ) );

		const char* pStrName = ::getName( GetZMODEL() );
		D3DVECTOR vec;
		::getPosition( GetZMODEL(), (float*)( &vec ) );
	}
	*/
#endif


	//--------------------------------------------------------------------------------
	/// Once you are done your character all objects on the list..
	/// Mini map output for the list
	//--------------------------------------------------------------------------------
	g_pObjMGR->AddViewObject( m_nIndex );


	//--------------------------------------------------------------------------------
	/// Due to the distance of the object management (register or leave the engine...)
	/// 2004/3/17 Due to the distance from the model of management of the client dimension is unnecessary. On the server I'd let all moderate management.
	/// If necessary, apply the LOD is reflected.
	/// 2004/7/8 InsertToScene/removeFromScene As such, there is no need to get on the server to add and delete..
	//--------------------------------------------------------------------------------
	bool bInViewfrustum = false;
	///int iDistance = CD3DUtil::distance ((int)g_GameDATA.m_PosCENTER.x, (int)g_GameDATA.m_PosCENTER.y, (int)m_PosCUR.x, (int)m_PosCUR.y);
	///if ( iDistance < 8*4*nGRID_SIZE )
	{
		///this->InsertToScene ();

		// char name & chatting message
		if ( ::inViewfrustum( this->GetZMODEL() ) ) // If true, false if the view frustum in
		{
			bInViewfrustum = true;
			if (this->Get_TYPE() != OBJ_MOB )
			{
				g_pViewMSG->AddObjIDX( m_nIndex );
			}
		}
	} /*else
	{
		this->RemoveFromScene ();
	}*/



	//--------------------------------------------------------------------------------
	/// Only those objects in the view frustum, change the height value (because a lot of the load)
	//--------------------------------------------------------------------------------
	if ( bInViewfrustum ) // Only those objects in the view frustum, change the height value (because a lot of the load)
	{
		/// If you have a link to anyone I. .. Not handling conflicts..

#if defined(_GBC)
		// 06. 06. 20 - Kim Joo-Hyun: Valkyrie transformed into effect in the State, the port is applied to it in that issue modification too.
		// EVO There is no State of the transformation.
		//When you turn the County character composition...
		if(m_bDisguise)
		{
			::setScale(this->m_hNodeMODEL, 1.0f, 1.0f, 1.0f);
		}
#endif


		if( this->IsChild() == false)
		{
#ifdef __NPC_COLLISION
          if(!(this->m_bCollisionOnOff  && this->IsNPC()))
			  Adjust_HEIGHT ();
#else
			Adjust_HEIGHT ();
#endif
		}











		if( this->IsChild() == false )
			Adjust_HEIGHT ();

#if defined(_GBC)
		// 06. 06. 20 - Kim Joo-Hyun: Valkyrie transformed into effect in the State, the port is applied to it in that issue modification even.
		// EVO There is no State of the transformation.
		//When you turn the County character composition...
		if(m_bDisguise)
		{
			::setLightingRecursive(this->GetZMODEL(), g_GameDATA.m_hCharacterLight2);
			::setVisibilityRecursive(this->GetZMODEL(), 0.15f);
			::setScale(this->GetZMODEL(), 0.7f, 0.7f, 0.7f);
			::setVisibleGlowRecursive( this->GetZMODEL(), 2, 0.2f, 0.5f, 0.945f );
		}
#endif
	}
	else
	{

#if (1) /// At present, compensation is high at the start of the move, since it is no need to compensate for the height of the.
		::getPosition( this->m_hNodeMODEL, (float*)m_PosCUR );
		//m_PosCUR.z = g_pAVATAR->Get_CurPOS().z; // The height of the protagonist character avatar is not visible at the height of the alignment.

		/// Avatar or (not riding a cart), I-card only..
		///if ( ( this->Is_AVATAR() && ( this->GetPetMode() < 0 ) ) ||
		///	this->IsPET() )
		{
			D3DXVECTOR3 vAvatarPos	= g_pAVATAR->Get_CurPOS();
			float fDistanceSquare2D = (vAvatarPos.x - m_PosCUR.x)*(vAvatarPos.x - m_PosCUR.x) + (vAvatarPos.y - m_PosCUR.y)*(vAvatarPos.y - m_PosCUR.y);
			const float fMinDistanceSquare2D = 4000.0f*4000.0f;
			// If you have only a very short distance height adjustment
			if ( fDistanceSquare2D < fMinDistanceSquare2D )
			{
				Adjust_HEIGHT();
			//	// If you're going to go home, the current position is the position of the formation of civil.
			//	//m_PosCUR.z = g_pTerrain->GetHeightTop(m_PosCUR.x, m_PosCUR.y);
			//	::setPosition (m_hNodeMODEL, m_PosCUR.x, m_PosCUR.y, m_PosCUR.z);
			}
		}
#endif

	}


	//--------------------------------------------------------------------------------
	/// Motion frames over the?
	//--------------------------------------------------------------------------------

	m_bFrameING = this->ProcMotionFrame ();

	//--------------------------------------------------------------------------------
	// Motion loops over.
	//--------------------------------------------------------------------------------
	if ( !m_bFrameING )
	{
		//--------------------------------------------------------------------------------
		/// Always ActionSkillIDX to register DoingSkill at the beginning of the action and the skill at the end of the motion the reset ... (See motion in a frame because of the skill number)
		//--------------------------------------------------------------------------------
		m_nDoingSkillIDX = 0;

//Park Ji-Ho:: cart issues that go to the avatar, then use the skill force at the end of the animation frames
//        Pacifying the avatar.
#if defined(_GBC)
		Set_SitMode();
#endif
		// { added by zho 2003-12-17
		// The sword stuck, stop
		for (short nI=0; nI<2; nI++)
		{
			if ( m_hTRAIL[ nI ] )
			{
				::controlTrail ( m_hTRAIL[ nI ], 3 );	// Gradually disappearing.
			}
		}

		//--------------------------------------------------------------------------------
		/// Equipment is due to be replaced by proper motion personal..
		//--------------------------------------------------------------------------------
		UpdateMotionByChangingEquip();

	} else if ( Get_STATE() & CS_BIT_INT )	// Some of this behavior can't get cancer.
	{
		return 1;
	}

	//--------------------------------------------------------------------------------
	/// Piled in the command queue exhausted...
	/// CanApplyCommand is false, it is impossible to apply.
	/// True, it is only when the current is m_bCastingSTART =.
	//--------------------------------------------------------------------------------
	ProcQueuedCommand();


	//--------------------------------------------------------------------------------
	/// Cast teulbyeolhan for processing
	/// Skill_START The m_bCastingSTART = true.
	/// However, you cannot see the point of the last skill action is over. .. Once
	/// This will depend on the.
	/// 04/5/28 - Skill: (DO_SKILL function) does with refactoring. (No.)
	//--------------------------------------------------------------------------------
	///Casting_END ();




	//--------------------------------------------------------------------------------
	/// Damage checked timeouts
	//--------------------------------------------------------------------------------
	this->ProcDamageTimeOut();

	//--------------------------------------------------------------------------------
	/// Effect_Of_Skill Check timeout
	//--------------------------------------------------------------------------------
	ProcTimeOutEffectedSkill();

	//--------------------------------------------------------------------------------
	/// Received a notice of death from the server...
	//--------------------------------------------------------------------------------
	if ( this->m_lDeadTIME )
	{
		CObjCHAR *pAttacker = g_pObjMGR->Get_CharOBJ( this->m_DeadDAMAGE.m_nTargetObjIDX, true );

		/// 2003. 12. 26 Last modified herbal treatment...
		/// I have been a 10 seconds dead, none If you do not hit me...
		if ( g_GameDATA.GetGameTime() - this->m_lDeadTIME >= 1000 * 5
				|| pAttacker == NULL
				|| pAttacker->m_iServerTarget != g_pObjMGR->Get_ServerObjectIndex( this->Get_INDEX() ) )
		{
			this->Apply_DAMAGE( pAttacker, this->m_DeadDAMAGE.m_wDamage );

			/// If this is a suicide ... Do not show damage..
			if( pAttacker != this )
			{
				uniDAMAGE wDamage;
				wDamage = this->m_DeadDAMAGE.m_wDamage;

				D3DXVECTOR3	pos = this->Get_CurPOS();
				g_UIMed.CreateDamageDigit( wDamage.m_wVALUE, pos.x, pos.y, pos.z + m_fStature, this->IsA( OBJ_USER ) );

				this->m_lDeadTIME = 0;
			}else
			{
				this->m_lDeadTIME = 100;
			}
			return 1;
		}
	}





	/// if current hp <= 0 do CMD die
	if( Get_HP() == 0 )
	{
		_ASSERT( 0 && "/// if current hp <= 0 do CMD die" );
		this->SetCMD_DIE();
	}


	//Park Ji-Ho received the command from the avatar RecvPACKET.cpp: command, the command is run here has been set.
	//Each command functions that will set an animation and effects.
	switch( Get_COMMAND() )
	{
		case CMD_DIE	:
			return this->IsUSER ();

		case CMD_SIT	:
			return ProcCMD_SIT ();

		case CMD_STOP   :
			return ProcCMD_STOP ();

		case CMD_MOVE	   :
			return ProcCMD_MOVE ();

		case CMD_PICK_ITEM :
			return ProcCMD_PICK_ITEM ();

		case CMD_ATTACK :
			return	ProcCMD_ATTACK ();

		case CMD_SKILL2SELF :
			return ProcCMD_Skill2SELF ();

		case CMD_SKILL2POS :
			return ProcCMD_Skill2POSITION ();

		case CMD_SKILL2OBJ :
			return ProcCMD_Skill2OBJECT ();
	}

	return 1;
}


//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// Park Ji-Ho: Set_SitMode()
///       : When avatars attack strikes the cart seat
///
//--------------------------------------------------------------------------------
void CObjCHAR::Set_SitMode(void)
{

	//I sit in the cart if you shot in the form of an avatar motion
	//Change..
	if(this->GetPetMode() > 0)
	{
//		if(this->m_pObjCART->Get_COMMAND() == CMD_ATTACK)
			this->Set_MOTION(this->m_pObjCART->GetRideAniPos());
	}

}


//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// Park Ji-Ho: ProcCMD_Skill2OBJECT_PET()
///       : The skill set of the target State of the pet.
///
//--------------------------------------------------------------------------------
int CObjCHAR::ProcCMD_Skill2OBJECT_PET()
{

	CObjCHAR *pTarget = CSkillManager::GetSkillTarget( m_iServerTarget, (this->m_nToDoSkillIDX)? this->m_nToDoSkillIDX : this->m_nActiveSkillIDX);

	if ( pTarget )
	{
		//-----------------------------------------------------------------------------------------
		/// If you have not yet started to cast the skill..
		//-----------------------------------------------------------------------------------------
		if ( !m_bCastingSTART )
		{
			int iAttackRange = this->Get_AttackRange();

			//The target set..
			m_pObjCART->m_PosGOTO       = pTarget->Get_CurPOS();
			m_pObjCART->Set_TargetIDX(m_iServerTarget);

			//The target should be move up to the.
			if( m_pObjCART->Goto_TARGET( pTarget, iAttackRange ) == false )
			{	m_pObjCART->SetCMD_MOVE(pTarget->Get_CurPOS(),TRUE);	return 1;	}
			else
			{
				//Stop
				m_pObjCART->Set_STATE(CS_STOP);
				m_pObjCART->m_fCurMoveSpeed = 0;

				Set_STATE(CS_STOP);
				m_fCurMoveSpeed = 0;
			}

			//If you can't stop writing skill
			if( !bCanStartSkill() )
			{
				//Pet
				m_pObjCART->Set_STATE(CS_STOP);
				m_pObjCART->m_fCurMoveSpeed = 0;

				//Avatar
				Set_STATE(CS_STOP);
				m_fCurMoveSpeed = 0;
				///this->Set_MOTION( this->GetANI_Casting() );
				return 1;
			}

			//Rotate the view right in the target treatment
			m_pObjCART->Set_ModelDIR( pTarget->m_PosCUR );

		}

		m_pObjCART->Set_ModelDIR( pTarget->m_PosCUR );

		//-----------------------------------------------------------------------------------------
		/// Target abandoned to die...
		//-----------------------------------------------------------------------------------------
		if( pTarget->m_bDead )
		{
			SetEffectedSkillFlag( true );
		}

		//Skill type skills check whether you get the cart.
		//!!In the cart, and the cart in order to write a skill List_Skill.stb avatar skills
		//If you are using a delimiter if you run the code just below the cart is a skill.

		//!!The implementation on the server, not the relationship test.
		int sType = SKILL_TYPE(this->m_nToDoSkillIDX);

		///Cart:: applied in the actual casting/action...
		if(sType == -1)
		{
			m_pObjCART->m_nToDoSkillIDX	= this->m_nToDoSkillIDX;

			if(m_pObjCART->m_SkillActionState == 0)
				m_pObjCART->m_SkillActionState = 1;

			m_pObjCART->Do_SKILL(this->Get_TargetIDX(),pTarget);
		}
		///Avatar: cast/actual behavior apply...
		else
		{ 	this->Do_SKILL(this->Get_TargetIDX(),pTarget);		}

	}

	else
	{
		SetEffectedSkillFlag( true );
		m_nActiveSkillIDX = 0;
		Casting_END();
	}

	return 1;
}


//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// Park Ji-Ho: SetNewCommandAfterSkill_PET()
///       : The skill set of the target State of the pet.
///
//--------------------------------------------------------------------------------
void CObjCHAR::SetNewCommandAfterSkill_PET(int iSkillNO)
{

	//This action brings the mode to be executed.
	switch( SKILL_ACTION_MODE( iSkillNO ) )
	{
		case SA_STOP:
			{
				m_pObjCART->Set_COMMAND(CMD_STOP);
				this->Set_COMMAND(CMD_STOP);

				break;
			}

		case SA_ATTACK:
			{
				CObjCHAR* pTarget = (CObjCHAR*)(this->Get_TargetOBJ());
				//g_pObjMGR->Get_ClientCharOBJ(this->m_iServerTarget,false);

				if(pTarget)
				{
					/// If I lose in a non-PVP zone zone attack command.
					if(this->IsA( OBJ_USER ) && pTarget->IsUSER() )
					{
						if( !g_pTerrain->IsPVPZone() || g_GameDATA.m_iPvPState == PVP_CANT )
						{ m_pObjCART->Set_COMMAND(CMD_STOP); this->Set_COMMAND(CMD_STOP); }
					}
					else
					{
						// Until death attacks !!!
						m_pObjCART->Start_ATTACK(pTarget);
						m_pObjCART->Set_COMMAND(CMD_ATTACK);
						this->Set_COMMAND(CMD_ATTACK);
					}
				}
				else
				{
					m_pObjCART->Set_COMMAND(CMD_STOP);
					this->Set_COMMAND(CMD_STOP);
				}

				break;
			}

		case SA_RESTORE:
			{
				//In the previous attack set
				this->Set_COMMAND(this->Get_BECOMMAND());
				this->Set_BECOMMAND(CMD_STOP);

				//If you switch the cards attack attack.
				if(m_pObjCART->Get_COMMAND() == CMD_ATTACK)
				{
					CObjCHAR* pTarget = (CObjCHAR*)(this->Get_TargetOBJ());
					//g_pObjMGR->Get_ClientCharOBJ(this->m_iServerTarget,false);

					if(pTarget)
					{
						/// If I lose in a non-PVP zone zone attack command.
						if(this->IsA( OBJ_USER ) && pTarget->IsUSER() )
						{
							if( !g_pTerrain->IsPVPZone() || g_GameDATA.m_iPvPState == PVP_CANT )
							{ m_pObjCART->Set_COMMAND(CMD_STOP); this->Set_COMMAND(CMD_STOP); }
						}
						// Or try to attack.
						else
						{
							m_pObjCART->Start_ATTACK(pTarget);
							m_pObjCART->Set_COMMAND(CMD_ATTACK);
							this->Set_COMMAND(CMD_ATTACK);
						}
					}
				}
			}

	}

}


///--------------------------------------------------------------------------------
/// class : CObjCHAR
/// Park Ji-Ho: SetRideUser()
///       : 2-seater.
///
//--------------------------------------------------------------------------------
BOOL CObjCHAR::SetRideUser(WORD irideIDX)
{
	//Pet mode...
	if(this->GetPetMode() < 0)
		return FALSE;

	//Cart creation...
	if(this->m_pObjCART == NULL)
		return FALSE;

	//Switch to the client index.
	m_iRideIDX = g_pObjMGR->Get_ClientObjectIndex(irideIDX);

	CObjAVT* pTarget = g_pObjMGR->Get_CharAVT(m_iRideIDX,false);
		if(pTarget == NULL)
			return FALSE;

	//Pet mode settings
	pTarget->SetPetType(this->GetPetMode());
	pTarget->m_pObjCART = this->m_pObjCART;
	pTarget->m_IsRideUser = TRUE;

	//Cart Board..
	this->m_pObjCART->Create(pTarget);

	pTarget->Set_COMMAND(CMD_STOP);
	pTarget->Set_STATE(CS_STOP);


	//Avatar is set so that the weapon and not the wing
	int iVisibilityPart[3] = {BODY_PART_KNAPSACK, BODY_PART_WEAPON_R, BODY_PART_WEAPON_L};

	for(register int i=0;i<3;i++)
	{
		CMODEL<CCharPART> *pCharPART = pTarget->m_pCharMODEL->GetCharPART(iVisibilityPart[i]);
		if (pCharPART)
		{
			short nI;

			for (nI=0; nI<pCharPART->m_nPartCNT; nI++)
			{
				if (pTarget->m_phPartVIS[iVisibilityPart[ i ] ][ nI ] )
				{
					::setVisibilityRecursive( pTarget->m_phPartVIS[ iVisibilityPart[ i ] ][ nI ], 0.0f );
				}
			}
		}
	}

	return TRUE;

}


///--------------------------------------------------------------------------------
/// class : CObjCHAR
/// Park Ji-Ho: ReleaseRideUser()
///       : 2 seater off.
///
//--------------------------------------------------------------------------------
void CObjCHAR::ReleaseRideUser(void)
{

	if(m_iRideIDX == 0)
		return;

	//Guest brings an object.
	CObjAVT* pTarget = g_pObjMGR->Get_CharAVT(m_iRideIDX,false);
	if(pTarget == NULL)
		return;

	m_iRideIDX = 0;

	//Pet mode settings
	pTarget->SetPetType(-1);
	pTarget->m_pObjCART = NULL;
	pTarget->m_IsRideUser = FALSE;

	pTarget->Set_STATE(CS_STOP);
	pTarget->SetCMD_STOP();


	//Avatar is set so that the arms and wings
	int iVisibilityPart[3] = {BODY_PART_KNAPSACK, BODY_PART_WEAPON_R, BODY_PART_WEAPON_L};

	for(register int i=0;i<3;i++)
	{
		CMODEL<CCharPART> *pCharPART = pTarget->m_pCharMODEL->GetCharPART(iVisibilityPart[i]);
		if (pCharPART)
		{
			short nI;

			for (nI=0; nI<pCharPART->m_nPartCNT; nI++)
			{
				if (pTarget->m_phPartVIS[iVisibilityPart[ i ] ][ nI ] )
				{
					::setVisibilityRecursive( pTarget->m_phPartVIS[ iVisibilityPart[ i ] ][ nI ], 1.0f );
				}
			}
		}
	}

}


///--------------------------------------------------------------------------------
/// class : CObjCHAR
/// Park Ji-Ho: ReleaseRideUser()
///       : If you're making the jump yourself secondary to cart.
///
//--------------------------------------------------------------------------------
void CObjCHAR::ReleaseRideUser(WORD irideIDX)
{

	//m_iRideIDX = g_pObjMGR->Get_ClientObjectIndex(irideIDX);
	CObjAVT* pTarget = g_pObjMGR->Get_CharAVT(irideIDX,false);
		if(pTarget == NULL)
			return;

	pTarget->m_pObjCART->UnLinkChild(1);

	//Parent's disconnect
	pTarget->m_pObjCART->GetParent()->m_iRideIDX	= 0;
	pTarget->m_pObjCART->GetParent()->m_pRideUser	= NULL;
	//Pet mode settings
	pTarget->SetPetType(-1);
	pTarget->m_pObjCART = NULL;
	pTarget->m_IsRideUser = FALSE;

	pTarget->Set_STATE(CS_STOP);
	//pTarget->SetCMD_STOP();

	//Make the jump to set in motion..
	pTarget->Set_MOTION(SKILL_ANI_ACTION_TYPE(27),0,1,false,1);





	//Avatar is set so that the arms and wings
	int iVisibilityPart[3] = {BODY_PART_KNAPSACK, BODY_PART_WEAPON_R, BODY_PART_WEAPON_L};

	for(register int i=0;i<3;i++)
	{
		CMODEL<CCharPART> *pCharPART = pTarget->m_pCharMODEL->GetCharPART(iVisibilityPart[i]);
		if (pCharPART)
		{
			short nI;

			for (nI=0; nI<pCharPART->m_nPartCNT; nI++)
			{
				if (pTarget->m_phPartVIS[iVisibilityPart[ i ] ][ nI ] )
				{
					::setVisibilityRecursive( pTarget->m_phPartVIS[ iVisibilityPart[ i ] ][ nI ], 1.0f );
				}
			}
		}
	}

}


///--------------------------------------------------------------------------------
/// class : CObjCHAR
/// Park Ji-Ho: Ride_Cansel_Motion()
///       : Denied boarding shall set in motion.
///
//--------------------------------------------------------------------------------
void CObjCHAR::Ride_Cansel_Motion(void)
{

	this->Set_COMMAND(CMD_STOP);
	this->SetCMD_STOP();

	this->Set_MOTION(453,0,1,false,1);

}


///--------------------------------------------------------------------------------
/// class : CObjCHAR
/// class : Stop_Cart
/// Park Ji-Ho: stop the cart.
///
//--------------------------------------------------------------------------------
void CObjCHAR::Stop_Cart(void)
{

	if(!m_pObjCART)
		return;

	m_pObjCART->Set_COMMAND(CMD_STOP);
	m_pObjCART->SetCMD_STOP();

}


//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @brief  : queuing the command
///				You can apply the current command?
///				The current command to determine the status of the settings can't La. ..
//--------------------------------------------------------------------------------

bool CObjCHAR::CanApplyCommand()
{
	/// The cast has been launched not applicable..
	/// Also if you result_of_skill ... In the end, all before the end of the current cast command queue with skill.
	/// Already, the results are applied in there because clients should unconditionally skill:.
	if( this->m_bCastingSTART && bCanActionActiveSkill() )
	{
		return false;
	}

	/// Currently, the command to be performed when the queue is empty, you must first carry out the orders of the queue if you do not ...
	if( this->m_CommandQueue.IsEmpty() == false )
	{
		return false;
	}

	return true;
}


//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @brief  : Wrapped commands.
///			  Should be done every frame?
//--------------------------------------------------------------------------------

void CObjCHAR::ProcQueuedCommand()
{
	if( m_CommandQueue.IsEmpty() )
		return;

	/// Be sure to follow the current skill-related commands or skill if you have this setting wait. ..
    if( this->m_bCastingSTART && bCanActionActiveSkill() )
	{
		return;
	}

	/// Once the last whites-only
	bool bSkillCommand = false;
	CObjCommand* pCommand = m_CommandQueue.PopLastCommand( bSkillCommand );

	if( pCommand )
	{
		pCommand->Execute( this );
		Log_String( LOG_NORMAL, "!!The information in the queue run!!\n" );

		/// Skill If you have already received the command and the command Result..
		if( bSkillCommand )
		{
			if( pCommand->bGetResultOfSkil() )
			{
				SetEffectedSkillFlag( true );
				Log_String( LOG_NORMAL, "!!Effectedskillflag from the information in the queue settings!!\n" );
			}
		}
	}
}


//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @brief  :
//--------------------------------------------------------------------------------

/*override*/void CObjCHAR::PushCommandSit()
{
	CObjCommand* pCommand = m_CommandQueue.GetObjCommand( OBJECT_COMMAND_SIT );

	if( pCommand )
	{
		((CObjSitCommand*)pCommand)->SetCMD_SIT( );

		m_CommandQueue.PushCommand( pCommand );
	}else
		assert( 0 && " GetObjCommand failed " );
}


//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @brief  :
//--------------------------------------------------------------------------------

/*override*/void CObjCHAR::PushCommandStand()
{
	CObjCommand* pCommand = m_CommandQueue.GetObjCommand( OBJECT_COMMAND_STAND );

	if( pCommand )
	{
		((CObjStandCommand*)pCommand)->SetCMD_STAND( );

		m_CommandQueue.PushCommand( pCommand );
	}else
		assert( 0 && " GetObjCommand failed " );
}


//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @brief  :
//--------------------------------------------------------------------------------

/*override*/void CObjCHAR::PushCommandStop()
{
	CObjCommand* pCommand = m_CommandQueue.GetObjCommand( OBJECT_COMMAND_STOP );

	if( pCommand )
	{
		((CObjStopCommand*)pCommand)->SetCMD_STOP( );

		m_CommandQueue.PushCommand( pCommand );
	}else
		assert( 0 && " GetObjCommand failed " );
}


//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @brief  :
//--------------------------------------------------------------------------------

/*override*/void CObjCHAR::PushCommandMove( const D3DVECTOR& PosTO, BYTE btRunMODE )
{
	CObjCommand* pCommand = m_CommandQueue.GetObjCommand( OBJECT_COMMAND_MOVE );

	if( pCommand )
	{
		((CObjMoveCommand*)pCommand)->SetCMD_MOVE( PosTO, btRunMODE );

		m_CommandQueue.PushCommand( pCommand );
	}else
		assert( 0 && " GetObjCommand failed " );
}


//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @brief  :
//--------------------------------------------------------------------------------

/*override*/void CObjCHAR::PushCommandMove( WORD wSrvDIST, const D3DVECTOR& PosTO, int iServerTarget )
{
	CObjCommand* pCommand = m_CommandQueue.GetObjCommand( OBJECT_COMMAND_MOVE );

	if( pCommand )
	{
		((CObjMoveCommand*)pCommand)->SetCMD_MOVE( wSrvDIST, PosTO, iServerTarget );

		m_CommandQueue.PushCommand( pCommand );
	}else
		assert( 0 && " GetObjCommand failed " );
}


//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @brief  :
//--------------------------------------------------------------------------------

/*override*/void CObjCHAR::PushCommandAttack( int iServerTarget, WORD wSrvDIST, const D3DVECTOR& PosTO )
{
	CObjCommand* pCommand = m_CommandQueue.GetObjCommand( OBJECT_COMMAND_ATTACK );

	if( pCommand )
	{
		((CObjAttackCommand*)pCommand)->SetCMD_ATTACK( iServerTarget, wSrvDIST, PosTO );

		m_CommandQueue.PushCommand( pCommand );
	}else
		assert( 0 && " GetObjCommand failed " );
}


//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @brief  :
//--------------------------------------------------------------------------------

/*override*/void CObjCHAR::PushCommandDie()
{
	CObjCommand* pCommand = m_CommandQueue.GetObjCommand( OBJECT_COMMAND_DIE );

	if( pCommand )
	{
		((CObjDieCommand*)pCommand)->SetCMD_DIE( );

		m_CommandQueue.PushCommand( pCommand );
	}else
		assert( 0 && " GetObjCommand failed " );
}


//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @brief  :
//--------------------------------------------------------------------------------

/*override*/void CObjCHAR::PushCommandToggle( BYTE btTYPE )
{
	CObjCommand* pCommand = m_CommandQueue.GetObjCommand( OBJECT_COMMAND_TOGGLE );

	if( pCommand )
	{
		((CObjToggleCommand*)pCommand)->SetCMD_TOGGLE( btTYPE );

		m_CommandQueue.PushCommand( pCommand );
	}else
		assert( 0 && " GetObjCommand failed " );
}


//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @brief  :
//--------------------------------------------------------------------------------

/*override*/void CObjCHAR::PushCommandSkill2Self( short nSkillIDX )
{
	CObjCommand* pCommand = m_CommandQueue.GetObjCommand( OBJECT_COMMAND_Skill2SELF );

	if( pCommand )
	{
		((CObjSkill2SelfCommand*)pCommand)->SetCMD_Skill2SELF( nSkillIDX );

		m_CommandQueue.PushCommand( pCommand );
	}else
		assert( 0 && " GetObjCommand failed " );
}


//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @brief  :
//--------------------------------------------------------------------------------

/*override*/void CObjCHAR::PushCommandSkill2Obj( WORD wSrvDIST, const D3DVECTOR& PosTO, int iServerTarget, short nSkillIDX )
{
	CObjCommand* pCommand = m_CommandQueue.GetObjCommand( OBJECT_COMMAND_Skill2OBJ );

	if( pCommand )
	{
		((CObjSkill2ObjCommand*)pCommand)->SetCMD_Skill2OBJ( wSrvDIST, PosTO, iServerTarget, nSkillIDX );

		m_CommandQueue.PushCommand( pCommand );
	}else
		assert( 0 && " GetObjCommand failed " );
}


//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @brief  :
//--------------------------------------------------------------------------------

/*override*/void CObjCHAR::PushCommandSkill2Pos( const D3DVECTOR& PosGOTO, short nSkillIDX )
{
	CObjCommand* pCommand = m_CommandQueue.GetObjCommand( OBJECT_COMMAND_Skill2POS );

	if( pCommand )
	{
		((CObjSkill2PosCommand*)pCommand)->SetCMD_Skill2POS( PosGOTO, nSkillIDX );

		m_CommandQueue.PushCommand( pCommand );
	}else
		assert( 0 && " GetObjCommand failed " );
}


//----------------------------------------------------------------------------------------------------
/// @param
/// @brief Start vibration
//----------------------------------------------------------------------------------------------------

void CObjCHAR::StartVibration()
{
	m_ObjVibration.StartVibration();
}


//--------------------------------------------------------------------------------
/// class : CObjCHAR
/// @brief  :
//--------------------------------------------------------------------------------

void	CObjCHAR::Set_HP (int iHP)
{
	if( iHP == DEAD_HP )
		m_iHP = iHP;
	else
	{
		if( iHP <= 0 )
			m_iHP = 1;
		else
			m_iHP = iHP;
	}
}
void	CObjCHAR::Set_MP (int iMP)
{
	m_iMP = iMP;
}

int	CObjCHAR::Add_HP (int iAdd)
{
	m_iHP += iAdd;

	if( m_iHP <= 0 )
	{
		LogString( LOG_DEBUG,"Caution : HP < 0 @CObjCHAR::Add_HP" );
		m_iHP = 1;
	}

	if( m_iHP > Get_MaxHP() )
		m_iHP = Get_MaxHP();
	return m_iHP;
}

int	CObjCHAR::Sub_HP (int iSub)
{
	m_iHP -= iSub;
	if( m_iHP <= 0 )
		m_iHP = 1;

	return m_iHP;
}


//----------------------------------------------------------------------------------------------------
/// @brief Effects on the characters..
//----------------------------------------------------------------------------------------------------
void CObjCHAR::ShowEffectOnCharByIndex( int iEffectIDX, int iSountIDX, bool bWeatherEffect )
{
	if( iEffectIDX )
	{
		if ( this->IsVisible() )
		{
			CEffect *pEffect = g_pEffectLIST->Add_EffectWithIDX( iEffectIDX, true );
			if( pEffect )
			{
				pEffect->LinkNODE ( this->GetZMODEL() );

				pEffect->SetParentCHAR( this );


				if( !bWeatherEffect )
					this->AddExternalEffect( pEffect );
				else
					this->AddWeatherEffect( pEffect );


				pEffect->InsertToScene ();
			}
		}
	}

	if ( iSountIDX )
	{
		g_pSoundLIST->IDX_PlaySound3D( iSountIDX, Get_CurPOS() );
	}
}

void CObjCHAR::ShowEffectOnCharByHash( int iEffectHash, int iSoundIDX, bool bWeatherEffect )
{
	if( iEffectHash )
	{
		if ( this->IsVisible() )
		{
			CEffect *pEffect = g_pEffectLIST->Add_EFFECT( (t_HASHKEY)iEffectHash, true );
			pEffect->LinkNODE ( this->GetZMODEL() );

			pEffect->SetParentCHAR( this );

			if( !bWeatherEffect )
				this->AddExternalEffect( pEffect );
			else
				this->AddWeatherEffect( pEffect );

			pEffect->InsertToScene ();
		}
	}

	if ( iSoundIDX )
	{
		g_pSoundLIST->IDX_PlaySound3D( iSoundIDX, Get_CurPOS() );
	}
}

void CObjCHAR::AddWeatherEffect( CEffect* pEffect )
{
	m_WeatherEffectLIST.AllocNAppend( pEffect );
}

void CObjCHAR::DeleteWeatherEffect()
{
	classDLLNODE< CEffect* > *pNode;
	pNode = m_WeatherEffectLIST.GetHeadNode ();
	while( pNode )
	{
		/// Delete the only effect of the parents should not handle the effects. Why? I'm a parent..
		g_pEffectLIST->Del_EFFECT( pNode->DATA, false );

		m_WeatherEffectLIST.DeleteNFree( pNode );
		pNode = m_WeatherEffectLIST.GetHeadNode( );
	}

	m_WeatherEffectLIST.ClearList();
}


void CObjCHAR::SetClanMark( WORD wMarkBack, WORD wMarkCenter )
{
	m_wClanMarkBack		= wMarkBack;
	m_wClanMarkCenter   = wMarkCenter;
	if( m_ClanMarkUserDefined )
	{
		m_ClanMarkUserDefined->Release();
		m_ClanMarkUserDefined = NULL;
	}
}

void CObjCHAR::SetClan( DWORD dwClanID, WORD wMarkBack, WORD wMarkCenter, const char* pszName , int iLevel, BYTE btClanPos )
{
	//assert( pszName && dwClanID );
	if( pszName && dwClanID)
	{

		m_dwClanID			= dwClanID;
		m_wClanMarkBack		= wMarkBack;
		m_wClanMarkCenter   = wMarkCenter;
		m_iClanLevel		= iLevel;
		m_strClanName		= pszName;
		m_strClanName.erase( remove( m_strClanName.begin(), m_strClanName.end(),' ' ) , m_strClanName.end() );
		m_btClanPos			= btClanPos;
	}
}

BYTE  CObjCHAR::GetClanPos()
{
	return m_btClanPos;
}

void CObjCHAR::SetClanPos( BYTE btPos )
{
	m_btClanPos = btPos;
}

DWORD CObjCHAR::GetClanID()
{
	return m_dwClanID;
}

WORD CObjCHAR::GetClanMarkBack()
{
	return m_wClanMarkBack;
}

WORD CObjCHAR::GetClanMarkCenter()
{
	return m_wClanMarkCenter;
}

const char* CObjCHAR::GetClanName()
{
	return m_strClanName.c_str();
}

void CObjCHAR::ResetClan()
{
	m_dwClanID					= 0;
	m_btClanPos					= 0;
	m_wClanMarkBack				= 0;
	m_wClanMarkCenter			= 0;

	if( m_ClanMarkUserDefined )
	{
		m_ClanMarkUserDefined->Release();
		m_ClanMarkUserDefined = NULL;
	}
}
void CObjCHAR::SetClanLevel( int iLevel )
{
	m_iClanLevel = iLevel;
}
int CObjCHAR::GetClanLevel()
{
	return m_iClanLevel;
}

void CObjCHAR::SetUserDefinedClanMark( CClanMarkUserDefined* pUserDefinedClanMark )
{
	assert( pUserDefinedClanMark );
	m_ClanMarkUserDefined = pUserDefinedClanMark;
	pUserDefinedClanMark->AddRef();
}

///If the current server and client, and the amount of HP and he saves the value.
void CObjCHAR::SetReviseHP( int hp )
{
	m_ReviseHP = hp;
}

///If the current server and client, and the amount of MP saves the value.
void CObjCHAR::SetReviseMP( int mp )
{
	m_ReviseMP = mp;
}
//-----------------------------------------------------------------------------
/// @brief An additional ability related to the calculation be allure: 2005/7/13-nAvy
//-----------------------------------------------------------------------------
void CObjCHAR::Calc_AruaAddAbility()
{
	if( IsApplyNewVersion() )
	{
		if( m_IsAroa )
			m_AruaAddMoveSpeed   = GetOri_RunSPEED(); // * 0.2;
		else
			m_AruaAddMoveSpeed   = 0;
	}
}
//-----------------------------------------------------------------------------
/// @brief Attack speed
//-----------------------------------------------------------------------------
short CObjCHAR::Get_nAttackSPEED ()
{
	int iR = GetOri_ATKSPEED() + m_EndurancePack.GetStateValue( ING_INC_ATK_SPD )
								- m_EndurancePack.GetStateValue( ING_DEC_ATK_SPD );

	return GetMinStateValue( AT_ATK_SPD, iR );
}
//-----------------------------------------------------------------------------
/// @brief The engine is used to change the speed of the attack methods			: 2005/7/13 - nAvy
//-----------------------------------------------------------------------------
float CObjCHAR::Get_fAttackSPEED ()
{
	return Get_nAttackSPEED()/100.f;



}

//---------------------------------------------------------------------------------------------------------------
/// @brief The engine is used to change the speed of the attack on methods: If the rate increase be revised to Arua : 2005/7/13 - nAvy
//---------------------------------------------------------------------------------------------------------------
float CObjCHAR::Get_MoveSPEED ()
{
	return m_fAdjustSPEED;
}

//-----------------------------------------------------------------------------
/// @brief If the move is to increase the speed of Arua fix : 2005/7/13 - nAvy
//-----------------------------------------------------------------------------

float CObjCHAR::Get_DefaultSPEED ()
{
	if ( !m_bRunMODE && ( m_btMoveMODE <= MOVE_MODE_RUN ) )
	{
		return GetOri_WalkSPEED();
	}

	//PY: removing speed calculation from the client
	/*
	short nR = ( GetOri_RunSPEED() + m_EndurancePack.GetStateValue( ING_INC_MOV_SPD )
									- m_EndurancePack.GetStateValue( ING_DEC_MOV_SPD ) );

	nR += m_AruaAddMoveSpeed;
	*/
	short nR = GetOri_RunSPEED();	//just using this basic version instead

	//20060810 Hong-Geun: a mob of velocity of zero over the move bug.
	if( this->IsA( OBJ_MOB ) && nR !=0 && nR < MIN_SPEED )

	{
		return MIN_SPEED;
	}
#ifdef __CAL_BUF2
	else
	{
		return GetMinStateValue( AT_SPEED, nR );	//all this does is check that nR is not less than a defined minimum value
	}
#else
	else						return nR;
#endif

}
//---------------------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
/// class : CObjMOB
/// @brief  : Mob creation order .. static
//--------------------------------------------------------------------------------

DWORD CObjMOB::m_dwCreateOrder=0;

//--------------------------------------------------------------------------------
/// class : CObjMOB
/// @brief  : constructor
//--------------------------------------------------------------------------------

CObjMOB::CObjMOB()
{
	m_nQuestIDX = 0;


	m_iMobAniSkill = MOB_ANI_CASTION01;
}


//--------------------------------------------------------------------------------
/// class : CObjMOB
/// @brief  : destructor
//--------------------------------------------------------------------------------

CObjMOB::~CObjMOB ()
{
	/*
	if ( m_nQuestIDX ) {
		g_pEventLIST->Del_EVENT (xxx);
	}
	*/
//	LogString (LOG_DEBUG, "   CObjMOB::~CObjMOB ( charNo:%d, %s ), Obj:%d  \n", m_nCharIdx, Get_NAME(), m_nIndex );
}


//--------------------------------------------------------------------------------
/// class : CObjMOB
/// @param CGameOBJ *pSourOBJ
/// @param short nEventIDX
/// @brief  : A mob set event handling in. .. (NPC dialog events, etc.)
//--------------------------------------------------------------------------------

bool CObjMOB::Check_EVENT (CGameOBJ *pSourOBJ, short nEventIDX)
{
	CGameOBJ* pGameObj = pSourOBJ;
	/// Pet If the parent is checked
	if( pSourOBJ->IsPET() )
	{
		pGameObj = (CGameOBJ*)((CObjCART*)pSourOBJ)->GetParent();
	}


	if ( pGameObj->IsA( OBJ_USER ) &&
		 m_nQuestIDX )
	{
		return g_pEventLIST->Run_EVENT( this->Get_INDEX(), m_nQuestIDX, nEventIDX );
	}

	return false;
}


//--------------------------------------------------------------------------------
/// class : CObjMOB
/// @param  : CObjCHAR *pAtk
/// @brief  : The event occurred at the time of death..
//--------------------------------------------------------------------------------

void CObjMOB::Do_DeadEvent	(CObjCHAR *pAtk)
{
	QF_doQuestTrigger( NPC_DESC( m_nCharIdx ) );
}


//--------------------------------------------------------------------------------
/// class : CObjMOB
/// @param  :
/// @brief  : Mob creation
//--------------------------------------------------------------------------------

bool CObjMOB::Create (short nCharIdx, const D3DVECTOR& Position, short nQuestIDX, bool bRunMODE, short nMonLevel, short nMonSize)
{
	char *szName  = CStr::Printf (NAME_MOB_MODEL, NPC_NAME( nCharIdx ), this->m_dwCreateOrder++);

	CCharMODEL *pMODEL = g_MOBandNPC.GetMODEL( nCharIdx );


	m_nCharIdx = nCharIdx;
	if(nMonSize == 0)
	{
		m_fScale = NPC_SCALE( nCharIdx ) / 100.f;
	}
	else
	{
		m_fScale = nMonSize / 100.f;
		//ClientLog (LOG_NORMAL, "CObjMOB::Create set a value of %f for monster size %i", m_fScale, nMonSize );
	}
	if ( CObjCHAR::CreateCHAR (szName, pMODEL, pMODEL->GetPartCNT(), Position) )
	{
		this->m_iHP = NPC_HP( m_nCharIdx );
		this->m_iMaxHP = NPC_HP( m_nCharIdx ) * NPC_LEVEL( m_nCharIdx );

		if ( this->m_iHP < 0 )
			g_pCApp->ErrorBOX ("ERROR:: HP <= 0 !!!", (char*)NPC_NAME( nCharIdx ) );

		this->m_bRunMODE      = bRunMODE;
		this->m_fRunAniSPEED  = 1.0f;
//		this->m_fAtkAniSPEED  = (NPC_ATK_SPEED( m_nCharIdx ) / 100.f);

		//this->Adjust_HEIGHT ();

		// EVENT ...
		m_nQuestIDX = nQuestIDX;

		this->New_EFFECT( BODY_PART_WEAPON_R, NPC_R_WEAPON( m_nCharIdx ) );
		this->New_EFFECT( BODY_PART_WEAPON_L, NPC_L_WEAPON( m_nCharIdx ) );


		//----------------------------------------------------------------------------------------------------
		/// Glow effect
		//----------------------------------------------------------------------------------------------------
		unsigned int iColor = NPC_GLOW_COLOR( nCharIdx );
		if( iColor )
		{
			D3DXCOLOR color = CGameUtil::GetRGBFromString( iColor );
			::setVisibleGlowRecursive( this->GetZMODEL(), 2, color.r, color.g, color.b );
		}

#ifdef __NPC_COLLISION

		this->m_bCollisionOnOff = false;
		this->m_iPreCollisionNPCState = 0;
	    this->m_iCurrentCollisionNPCState = 0;

	    this->m_bCollisionOnOff = NPC_COLLISION_ONOFF(nCharIdx);

		if(this->m_bCollisionOnOff)
		{
			this->m_iCollisionBoneIndex = NPC_COLLISION_BONE_INDEX(nCharIdx);
            this->m_fCollisionBoxLength[0] = 1.0f*NPC_COLLISION_BOX_XLENGTH(nCharIdx);
			this->m_fCollisionBoxLength[1] = 1.0f*NPC_COLLISION_BOX_YLENGTH(nCharIdx);
	        this->m_fCollisionBoxLength[2] = 1.0f*NPC_COLLISION_BOX_ZLENGTH(nCharIdx);

			::SetModelCameraCollisionOnOff(this->GetZMODEL(), true);
			::SetForcedMotionMixOff(this->GetZMODEL(), true);

			if(g_pObjMGR->m_iCurrentCollisionNPCState == 0)
			{
				m_iCurrentCollisionNPCState = 0;
				m_iPreCollisionNPCState = 1;
			}
			else if(g_pObjMGR->m_iCurrentCollisionNPCState == 1)
			{
				m_iCurrentCollisionNPCState = 1;
				m_iPreCollisionNPCState = 0;
			}
			else if(g_pObjMGR->m_iCurrentCollisionNPCState == 2)
			{
				m_iCurrentCollisionNPCState = 2;
				m_iPreCollisionNPCState = 1;
			}

			Set_MOTION(m_iCurrentCollisionNPCState, 0.0f, 1.0f, false, 1);

		}
#endif


		return true;
	}

	LogString (LOG_DEBUG, "MOB Char create failed .. %d: %s \n", nCharIdx, NPC_NAME( nCharIdx ) );

	return false;
}


//--------------------------------------------------------------------------------
/// class : CObjMOB
/// @param  :
/// @brief  : Run away..
//--------------------------------------------------------------------------------

void CObjMOB::Run_AWAY (int iDistance)
{
	D3DVECTOR pos;

	pos.x = RANDOM( iDistance*2 ) - iDistance;
	pos.y = RANDOM( iDistance*2 ) - iDistance;
	pos.x += m_PosBORN.x;
	pos.y += m_PosBORN.y;

	this->SetCMD_MOVE (pos, true);
	this->Set_COMMAND( CMD_RUNAWAY );
}


//--------------------------------------------------------------------------------
/// class : CObjMOB
/// @param  :
/// @brief  : Character change. .. (So do not erase the model node? )
//--------------------------------------------------------------------------------

bool CObjMOB::Change_CHAR (int nCharIDX)
{
	this->DeleteCHAR ();

	D3DVECTOR PosBORN = Get_BornPOSITION ();

	if ( !this->Create (nCharIDX, m_PosCUR, m_nQuestIDX, m_bRunMODE) ) 
	{
		return false;
	}

	m_PosBORN = PosBORN;
	return true;
}


//--------------------------------------------------------------------------------
/// class : CObjMOB
/// @param  :
/// @brief  : No implementation
//--------------------------------------------------------------------------------

bool CObjMOB::Create_PET (int nCharIDX)
{
#ifdef	__VIRTUAL_SERVER
	return ( g_pObjMGR->Add_MobCHAR( 0, nCharIDX, m_PosCUR, 0, true ) ? true : false );
#else
	return false;
#endif
}


//--------------------------------------------------------------------------------
/// class : CObjMOB
/// @param  :
/// @brief  : AI's message..
//--------------------------------------------------------------------------------

void CObjMOB::Say_MESSAGE (char *szMessage)
{
	//AddMsgToChatWND (CStr::Printf("%s> %s", this->Get_NAME (), szMessage ), g_dwBLACK );
	g_UIMed.AddChatMsg( g_pObjMGR->Get_ServerObjectIndex( this->Get_INDEX() ), szMessage, g_dwRED );
}


//--------------------------------------------------------------------------------
/// class : CObjMOB
/// @param  :
/// @brief  : In order to apply a durable change of the figures currently in effect is the ability to figure (passive skill)
//--------------------------------------------------------------------------------

int	CObjMOB::Get_DefaultAbilityValue( int iType )
{
	switch( iType )
	{
		case AT_SPEED:
		case AT_ATK_SPD:
			return 1;

		case AT_MAX_HP:
			{
				return this->m_iMaxHP;
			}
			break;

		case AT_MAX_MP:
			{
				return this->m_iMaxMP;
			}
			break;

		default:
			return 1;
	}

	return 1;
}

/// In the case of NPC go that height is the height of force in the STB.
void CObjMOB::GetScreenPOS ( D3DVECTOR &PosSCR)
{
	float fStature = NPC_HEIGHT( this->m_nCharIdx );

/*
#ifdef _DEBUG
	::InputSceneAxis(m_hNodeMODEL, 500.0f);
#endif
*/
	if( fStature != 0 )
	{
		::worldToScreen( m_PosCUR.x, m_PosCUR.y, getPositionZ(m_hNodeMODEL) + fStature, &PosSCR.x, &PosSCR.y, &PosSCR.z );
		return;
	}

	// m_fStature, fStature This year I would say getting caught!

	// The coordinates of the model plus the name of the location of the output location settings key
	::worldToScreen( m_PosCUR.x, m_PosCUR.y, getPositionZ(m_hNodeMODEL) + m_fStature, &PosSCR.x, &PosSCR.y, &PosSCR.z );
}


//--------------------------------------------------------------------------------
/// class : CObjMOB
/// @param  :
/// @brief  : Attack distance. .. If you are using a skill skill at the table or tables in the NPC would..
//--------------------------------------------------------------------------------

int CObjMOB::Get_AttackRange()
{
	/// If you enter distance attack skill skill or weapon distance distance..
	if ( this->m_nToDoSkillIDX > 0 )
	{
		if( this->m_nToDoSkillIDX < g_SkillList.Get_SkillCNT() )
		{
			if(	SKILL_DISTANCE( this->m_nToDoSkillIDX ) )
			{
				return SKILL_DISTANCE( this->m_nToDoSkillIDX );
			}
		}else
			assert( 0 && "Get_AttackRange Failed[ ToDoSkillIDX is invalid ]" );
	}

	/// If you enter distance attack skill skill street or weapons..
	//if ( this->m_nToDoSkillIDX && SKILL_DISTANCE( this->m_nToDoSkillIDX ) )
	//{
	//	return SKILL_DISTANCE( this->m_nToDoSkillIDX );
	//}

	// Default attack range...
	return ( Def_AttackRange () + ( Get_SCALE() * 120 ) );
}

//------------------------------------------------------------------------------------------------
/// Monster enables sound output..
//------------------------------------------------------------------------------------------------
void CObjMOB::PlayStopSound()
{
	int iIndex = NPC_NORMAL_EFFECT_SOUND( this->Get_CharNO() );
	g_pSoundLIST->IDX_PlaySound3D( iIndex, this->Get_CurPOS() );
}

/*override*/ int CObjMOB::Proc()
{
	int iResult = CObjCHAR::Proc();

	//--------------------------------------------------------------------------------
	// Motion loops over.
	//--------------------------------------------------------------------------------
	if( this->Get_COMMAND() == CMD_STOP )
	{
		if ( !m_bFrameING )
		{
			if( RANDOM( 100 ) < 20 )
				PlayStopSound();
		}
	}

	return iResult;
}


//Base stats + items + passive
int	CObjMOB::Get_AbilityValue_StatItemPas( WORD nType )
{
	switch( nType )
	{
	case AT_SPEED	:		return (int)GetOri_RunSPEED();
	case AT_ATK_SPD :		return GetOri_ATKSPEED();
	}
	return 0;
}

//--------------------------------------------------------------------------------
// 05.05.19 icarus:: WOW way to Qwest exposure system for additional ...
CObjNPC::CObjNPC()
{
	m_nQuestSignal = -1;
}

void CObjNPC::SetEventValue( int iEventValue )
{
	m_iEventSTATUS = iEventValue;
}

int CObjNPC::GetEventValue()
{
	return m_iEventSTATUS;
}


int CObjNPC::Proc ()
{
	if( IsApplyNewVersion() && ( this->m_nQuestSignal < 0 || g_pAVATAR->m_bQuestUpdate ) )
	{
		// The status of the update my avatar's quests, or you have previously determined the status of the quest...
		this->m_nQuestSignal = SC_QF_GetNpcQuestStatus( this->m_nCharIdx );
		_RPT1( _CRT_WARN,"SC_QF_GetNpcQuestStatus :%d \n", m_nQuestSignal );
	}

	return CObjMOB::Proc ();
}



//--------------------------------------------------------------------------------
/// class : CObjAVT
/// @param  :
/// @brief  : constructor
//--------------------------------------------------------------------------------

CObjAVT::CObjAVT()
{
//	m_fScale  = 0.5;
	m_dwSitTIME			= 0;
	m_bIsFemale			= false;
	m_bRunMODE			= true;	// The default avatar is running...
	m_nRunSPEED			= WALK_CmPerSec;

	m_pCharMODEL		= &m_CharMODEL;	// Init ..

	m_iExpression		= 0;

	m_iDoChangeWeaponR	= 0;
	m_iDoChangeWeaponL	= 0;
	m_iCon				= 1;
	m_iRecoverHP		= 1;
	m_iRecoverMP		= 1;


	m_iLevel = 1;

	::ZeroMemory( m_sPartItemIDX, sizeof(tagPartITEM)*MAX_BODY_PART );

	///
	/// Personal store
	///
	m_bPersonalStoreMode	= false;
	m_pObjPersonalStore		= NULL;

	///
	/// pet
	///
	::ZeroMemory( m_sPetPartItemIDX, sizeof(tagPartITEM)*MAX_RIDING_PART );
	//m_iPetType			= 0;
	//m_pObjCART			= NULL;
	m_btWeightRate		= 0;

	m_pWeaponJemEffect	= 0;
	m_nStamina			= 0;///Set the minimum value
}


//--------------------------------------------------------------------------------
/// class : CObjAVT
/// @param  :
/// @brief  : destructor
//--------------------------------------------------------------------------------

CObjAVT::~CObjAVT ()
{
	DeleteCart();

	//--------------------------------------------------------------------------------
	/// Re, re delete the relevant drilling effects.
	//--------------------------------------------------------------------------------
	DeleteGemmingEffect();
	DeleteGreadEffect();

	if( m_pObjPersonalStore )
	{
		delete m_pObjPersonalStore;
		m_pObjPersonalStore = NULL;
	}
}


//--------------------------------------------------------------------------------
/// class : CObjAVT
/// @param  :
/// @brief  : Part piece settings (settings)
//--------------------------------------------------------------------------------

void CObjAVT::SetPartITEM (short nPartIdx, short nItemNo)
{
	this->New_EFFECT( nPartIdx, nItemNo, false );

	m_sPartItemIDX[ nPartIdx ].m_nItemNo = nItemNo;

	/// If you're going to clear all the data items which.
	if( nItemNo == 0 )
	{
		m_sPartItemIDX[ nPartIdx ].m_bHasSocket = false;
		m_sPartItemIDX[ nPartIdx ].m_cGrade = 0;
		m_sPartItemIDX[ nPartIdx ].m_nGEM_OP = 0;
	}

	//g_pSoundLIST->IDX_PlaySound( SOUND_EQUIP_ITEM  );
}


//--------------------------------------------------------------------------------
/// class : CObjAVT
/// @param  :
/// @brief  : ** Should the engine coordinates the ROK;.
///				Generates a Abaza
//--------------------------------------------------------------------------------

bool CObjAVT::Create (const D3DVECTOR &Position, BYTE btCharRACE)
{
	m_btRace = btCharRACE;
	m_bIsFemale = btCharRACE & 0x01;
	m_CharMODEL.Set_SEX( m_bIsFemale );

	for (short nItemIDX, nI=0; nI<MAX_BODY_PART; nI++)
	{
		nItemIDX = m_sPartItemIDX[ nI ].m_nItemNo;

		/// If you replace it with a hat and matching hair hair.
		if ( nI == BODY_PART_HAIR )
		{
			nItemIDX += HELMET_HAIR_TYPE( m_sPartItemIDX[ BODY_PART_HELMET ].m_nItemNo );
		}

		/// If you are replacing the current expression face facial..
		if( nI == BODY_PART_FACE )
		{
			nItemIDX += GetCharExpression();
		}

		this->m_CharMODEL.SetPartMODEL( nI, nItemIDX );
		//this->New_EFFECT( nI, nItemIDX );
	}

	D3DVECTOR charPos = Position;
	charPos.z = g_pTerrain->GetHeightTop(Position.x, Position.y);

	if ( CObjCHAR::CreateCHAR ( (char*)m_Name.c_str(), &m_CharMODEL, MAX_BODY_PART, charPos) ) {
		m_nAtkAniSPEED = Cal_AtkAniSPEED( this->GetPartITEM(BODY_PART_WEAPON_R) );

		m_iHP = 100;

		// The effect generated from the link SetPartITEM
		this->Link_EFFECT ();

		// Characters of the kidney.
		m_fStature = ::getModelHeight (this->m_hNodeMODEL);

		this->New_EFFECT( BODY_PART_WEAPON_R, this->GetPartITEM(BODY_PART_WEAPON_R));
		this->New_EFFECT( BODY_PART_WEAPON_L, this->GetPartITEM(BODY_PART_WEAPON_L));


		CreateGemmingEffect();
		CreateGradeEffect();

		return true;
	}

	return false;
}


//--------------------------------------------------------------------------------
/// class : CObjAVT
/// @param  :
/// @brief  : Class is set inside the avatar information, update.
//--------------------------------------------------------------------------------

void CObjAVT::Update (bool bUpdateBONE )
{
	// 0. Effect unlink  :: DeletePARTS was deleted from the engine automatically unlink??
	this->Unlink_EFFECT ();

	// 1. Clear
	// The ones that addRenderUnit clearRenderUnit (HNODE hVisible); Delete as
	// LoadVisible, that was deleted with unloadVisible ...
	this->DeletePARTS (false);

	//--------------------------------------------------------------------------------
	/// Re, re delete the effects related to the drilling.
	//--------------------------------------------------------------------------------
	DeleteGemmingEffect();
	DeleteGreadEffect();


	if ( bUpdateBONE ) {
		m_pCharMODEL->UnlinkBoneEFFECT( m_ppBoneEFFECT );

		this->UnloadModelNODE ();
		this->LoadModelNODE ( (char*)m_Name.c_str() );
		if ( m_bIsVisible ) {
			::insertToScene( m_hNodeMODEL );
			::setPosition (this->m_hNodeMODEL, m_PosCUR.x, m_PosCUR.y, m_PosCUR.z);
		}

		m_pCharMODEL->LinkBoneEFFECT( m_hNodeMODEL, m_ppBoneEFFECT );
	}

	// 2. Data set.
	for (short nItemIDX, nI=0; nI<MAX_BODY_PART; nI++)
	{
		nItemIDX = m_sPartItemIDX[ nI ].m_nItemNo;

		if ( nI == BODY_PART_HAIR )
		{
			nItemIDX += HELMET_HAIR_TYPE( m_sPartItemIDX[ BODY_PART_HELMET ].m_nItemNo );
		}


		/// If you are a current look face to face replacement..
		if( nI == BODY_PART_FACE )
		{
			nItemIDX += GetCharExpression();
		}


		this->m_CharMODEL.SetPartMODEL( nI, nItemIDX );
	}


	// 4. Model settings.
	this->CreatePARTS ( (char*)m_Name.c_str() );

	this->InsertToScene ();

	// 5. Relink the effects
	this->Link_EFFECT ();


	//----------------------------------------------------------------------------------------------------
	/// @brief Ash, ash creates an effect related to drilling
	//----------------------------------------------------------------------------------------------------

	//2005. 06. 15 Park Ji-Ho
	Set_RareITEM_Glow();

	CreateGemmingEffect();
	CreateGradeEffect();


	// 6. If the motion is wrong the weapon currently in use have been changed ...
	#pragma message ("TODO:: change motion ..." __FILE__)



	// Characters of the kidney. Long referred to as the height of an object above the sword artifacts or other wanted ... InsertToScene we're in. ..
	m_fStature = ::getModelHeight (this->m_hNodeMODEL);



	/// If you are riding in a cart..
	if( GetPetMode() >= 0 )
	{
#ifdef __ITEM_TIME_LIMMIT
		if( !IsRideUser() )
#endif
			this->UpdatePet();
	}

	/// CheckVisibiliey
	m_EndurancePack.UpdateVisibility();
}


//----------------------------------------------------------------------------------------------------
/// @brief Ash, ash Lotus related effects. ..
//----------------------------------------------------------------------------------------------------
const int iWeaponAttachedEffectDummyNO = 2;
void CObjAVT::CreateGemmingEffect()
{
	//----------------------------------------------------------------------------------------------------
	/// @brief The re-re-Ming or drilling-related ... If paste effect. ..
	//----------------------------------------------------------------------------------------------------

	for( int i = BODY_PART_WEAPON_R; i <= BODY_PART_WEAPON_L ; i++ )
	{
		int iPartIdx = i;
		int iItemIDX = m_sPartItemIDX[ iPartIdx ].m_nItemNo;
		if( iItemIDX == 0 )
			continue;

		if( m_sPartItemIDX[ iPartIdx ].m_bHasSocket && m_sPartItemIDX[ iPartIdx ].m_nGEM_OP > 300 )
		{
			int iEffectIDX = 0;

			iEffectIDX = GEMITEM_ATTACK_EFFECT( m_sPartItemIDX[ iPartIdx ].m_nGEM_OP );
			if( iEffectIDX )
			{
				/// List_Effect There are up to four effects can take a. .. Only one ...
				int iStartPointNO = 0;
				switch( iPartIdx )
				{
					case BODY_PART_WEAPON_R:
						iStartPointNO = WEAPON_GEMMING_POSITION( iItemIDX );
						break;

					case BODY_PART_WEAPON_L:
						iStartPointNO = SUBWPN_GEMMING_POSITION( iItemIDX );
						break;
				}

				for( int i = 0; i < 1 ; i++ )
				{
					int iEffect = EFFECT_POINT( iEffectIDX, i );

					m_pWeaponJemEffect = g_pEffectLIST->Add_EffectWithIDX( iEffect );
					if ( m_pWeaponJemEffect )
					{
						CMODEL<CCharPART> *pCharPART = g_DATA.Get_CharPartMODEL( iPartIdx, iItemIDX, this->IsFemale() );

						if( pCharPART &&
							m_phPartVIS[ iPartIdx ] &&
							iStartPointNO < pCharPART->m_nDummyPointCNT &&
							m_phPartVIS[ iPartIdx ][ pCharPART->m_pDummyPoints[ iStartPointNO + i ].m_nParent ] )
						{
							m_pWeaponJemEffect->Rotation( pCharPART->m_pDummyPoints[ iStartPointNO + i ].m_Rotate );
							m_pWeaponJemEffect->Transform( pCharPART->m_pDummyPoints[ iStartPointNO + i ].m_Transform );
							m_pWeaponJemEffect->LinkNODE( m_phPartVIS[ iPartIdx ][ pCharPART->m_pDummyPoints[ iStartPointNO + i ].m_nParent ] );
						}else
						{
							g_pEffectLIST->Del_EFFECT( m_pWeaponJemEffect );
						}
					}
				}

				/*m_pWeaponJemEffect = g_pEffectLIST->Add_EffectWithIDX( iEffectIDX );
				if ( m_pWeaponJemEffect )
				{
					CMODEL<CCharPART> *pCharPART = g_DATA.Get_CharPartMODEL( iPartIdx, iItemIDX, this->IsFemale() );

					if( iWeaponAttachedEffectDummyNO < pCharPART->m_nDummyPointCNT )
					{
						m_pWeaponJemEffect->Transform( pCharPART->m_pDummyPoints[ iWeaponAttachedEffectDummyNO ].m_Transform );
						m_pWeaponJemEffect->Rotation( pCharPART->m_pDummyPoints[ iWeaponAttachedEffectDummyNO ].m_Rotate );
						m_pWeaponJemEffect->LinkNODE( m_phPartVIS[ iPartIdx ][ pCharPART->m_pDummyPoints[ iWeaponAttachedEffectDummyNO ].m_nParent ] );
					}else
					{
						g_pEffectLIST->Del_EFFECT( m_pWeaponJemEffect );
					}
				}*/
			}
		}
	}
}

//----------------------------------------------------------------------------------------------------
/// @brief Ash, ash Lotus related effects. ..
//----------------------------------------------------------------------------------------------------
void CObjAVT::DeleteGemmingEffect()
{
	if( m_pWeaponJemEffect )
	{
		m_pWeaponJemEffect->UnlinkNODE();

		g_pEffectLIST->Del_EFFECT( m_pWeaponJemEffect );
		m_pWeaponJemEffect = NULL;
	}
}


//2005.06. 15 Park Ji-Ho
//----------------------------------------------------------------------------------------------------
/// If the glow effect when the items in case for the rare step 3.
//----------------------------------------------------------------------------------------------------
void CObjAVT::Set_RareITEM_Glow(void)
{

	//Taiwan and the Republic of the Philippines. Fix 2005.6.20 nAvy
	if( !IsApplyNewVersion() )
		return;

	CInventory	m_Inventory;
	short		m_iType = 0;


	for( int i = 1; i < MAX_BODY_PART ; i++ )
	{
		int iItemIDX = m_sPartItemIDX[ i ].m_nItemNo;

		if( iItemIDX == 0 )
			continue;

		//Check whether a rare item.
		m_iType = m_Inventory.GetBodyPartToItemType(i);
			if(m_iType == MAX_BODY_PART)
				continue;


		if(ITEM_RARE_TYPE(m_iType,m_sPartItemIDX[i].m_nItemNo) != 3)
			continue;

		//Step 3 set the Glow effect
		m_sPartItemIDX[i].m_cGrade = 3;
	}

}


//----------------------------------------------------------------------------------------------------
/// @brief Ash, ash Lotus related effects. ..
//----------------------------------------------------------------------------------------------------
void CObjAVT::CreateGradeEffect()
{
	CMODEL<CCharPART> *pCharPART = NULL;

	for( int i = 0; i < MAX_BODY_PART ; i++ )
	{
		int iItemIDX = m_sPartItemIDX[ i ].m_nItemNo;
		if( iItemIDX == 0 )
			continue;

		if( m_sPartItemIDX[ i ].m_cGrade != 0 )
		{
			//----------------------------------------------------------------------------------------------------
			/// Glow effect
			//----------------------------------------------------------------------------------------------------
			#if defined ( _NEWINTERFACEKJH )
				unsigned int iColor = ITEMGRADE_GLOW_COLOR_KRNEW( m_sPartItemIDX[ i ].m_cGrade );
			#else
				unsigned int iColor = ITEMGRADE_GLOW_COLOR( m_sPartItemIDX[ i ].m_cGrade );
			#endif

			if( iColor )
			{
				D3DXCOLOR color = CGameUtil::GetRGBFromString( iColor );

				/// Skinning If you are not the objects..
				if( m_pCharMODEL )
				{
					if( m_pCharMODEL->m_RenderUnitPart[ i ].empty() )
					{
						pCharPART = g_DATA.Get_CharPartMODEL( i, iItemIDX, this->IsFemale() );
						if( pCharPART )
						{
							for( int j = 0; j < pCharPART->m_nPartCNT; j++ )
							{
								if( m_phPartVIS[ i ] && m_phPartVIS[ i ][ j ] )
									::setVisibleGlow( m_phPartVIS[ i ][ j ], ZZ_GLOW_TEXTURE, color.r, color.g, color.b );
							}
						}
					}else
					{
						/// If the object being, ... skin
						std::list< int >::iterator	begin = m_pCharMODEL->m_RenderUnitPart[ i ].begin();
						for( ; begin != m_pCharMODEL->m_RenderUnitPart[ i ].end() ; ++begin )
						{
							int iRenderUnitIndex = *begin;
							::setVisibleRenderUnitGlow( this->GetZMODEL(), iRenderUnitIndex, ZZ_GLOW_TEXTURE, color.r, color.g, color.b );
							//::setVisibleGlow( this->GetZMODEL(), ZZ_GLOW_SIMPLE, 1.0f, 1.0f, 1.0f );
						}
					}
				}
			}
		}
	}
}

//----------------------------------------------------------------------------------------------------
/// @brief Ash, ash Lotus related effects. ..
//----------------------------------------------------------------------------------------------------
void CObjAVT::DeleteGreadEffect()
{
}



//--------------------------------------------------------------------------------
/// class : CObjAVT
/// @param  :
/// @brief  : Save a number of bullets. In the case of long-distance weapons. .. Currently, depending on bullet set with judgment
//--------------------------------------------------------------------------------

/*override*/int	CObjAVT::Get_BulletNO ()
{
	tagITEM sItem;
	sItem.m_cType   = ITEM_TYPE_WEAPON;
	sItem.m_nItemNo = Get_R_WEAPON();

	int iBulletNo = 0;
	int iShotType = sItem.GetShotTYPE();

	/// Do not consume magic bullet weapons
	if( iShotType >= MAX_SHOT_TYPE )
	{
		switch( WEAPON_TYPE( sItem.m_nItemNo ) )
		{
			case WEAPON_ITEM_NOT_USE_BULLET:
				{
					iBulletNo = WEAPON_BULLET_EFFECT( sItem.m_nItemNo );
					return iBulletNo;
				}
				break;
		}
	}

	if( iShotType < MAX_SHOT_TYPE && m_ShotData[iShotType].m_nItemNo )
		iBulletNo = NATURAL_BULLET_NO( m_ShotData[iShotType].m_nItemNo );

	return iBulletNo;
}


//--------------------------------------------------------------------------------
/// class : CObjAVT
/// @param  :
/// @brief  : Saves the current action and weapons fit motion.
//--------------------------------------------------------------------------------

tagMOTION *CObjAVT::Get_MOTION (short nActionIdx)
{
	int iActionIdx = m_ChangeActionMode.GetAdjustedActionIndex( nActionIdx );

	// Depending on the type of weapon in his right hand ...
	short nWeaponTYPE = WEAPON_MOTION_TYPE( this->m_sRWeaponIDX.m_nItemNo );

	short nFileIDX;


	//Error modifying the composition, 2005 11-23 TYPE_MOTION County
	if(iActionIdx >= 0 && iActionIdx < g_TblAniTYPE.m_nDataCnt)
		nFileIDX = FILE_MOTION( nWeaponTYPE, iActionIdx );
	else
		return NULL;

	if ( 0 == nFileIDX ) {
		// The motion does not exist, replace them with bare hands in motion ...
		nFileIDX = FILE_MOTION( 0, nActionIdx );
	}

	tagMOTION *pMOTION = g_MotionFILE.IDX_GetMOTION( nFileIDX, m_bIsFemale );
	if ( pMOTION ) {
		pMOTION->m_nActionIdx = nActionIdx;
	}
	return pMOTION;
}


//--------------------------------------------------------------------------------
/// class : CObjAVT
/// @param  :
/// @brief  : Clerk of the Board sit toggle
//--------------------------------------------------------------------------------

bool CObjAVT::ToggleSitMODE ()
{
	if ( this->Get_COMMAND() == CMD_SIT ) {
		this->SetCMD_STAND ();
	} else {
		// The client unconditionally.
		m_dwSitTIME = 0;
		this->SetCMD_SIT ();
	}
	::setRepeatCount( m_hNodeMODEL, 1 );

	return true;
}


//--------------------------------------------------------------------------------
/// class : CObjAVT
/// @param  : float fAdjRate : The default speed of the speed of the current correction ratio for. ..
/// @brief  : Walk to the run mode toggle
///				m_bRunMODE Depending on the State of the variable speed is the speed calculation: default is wrong.
///             Prior to this function implemented in the inside, but now it's out there ...
//--------------------------------------------------------------------------------

bool CObjAVT::ToggleRunMODE ( float fAdjRate )
{
	if ( Get_STATE() == CS_MOVE )
	{

		Set_AdjustSPEED( this->Get_DefaultSPEED () * fAdjRate );


		this->Set_CurMOTION( this->Get_MOTION( this->GetANI_Move() ) );
#ifdef	_DEBUG
		float fMoveSpeed = this->Get_MoveSPEED();
		_ASSERT( fMoveSpeed >= 0.f && fMoveSpeed < 2000.f );
#endif

		// Speed motion speed set ...
		this->Set_ModelSPEED( this->Get_MoveSPEED() );

		::attachMotion		( this->m_hNodeMODEL, this->Get_ZMOTION()  );
		::setAnimatableSpeed( this->m_hNodeMODEL, ( m_bRunMODE )?this->m_fRunAniSPEED:1.0f );

		return true;
	}

	return false;
}


//--------------------------------------------------------------------------------
/// class : CObjAVT
/// @param  :
/// @brief  : The current fetch mode
//--------------------------------------------------------------------------------

int	CObjAVT::GetPetMode()
{
	if( m_pObjCART )
		return m_pObjCART->GetCartType();
	return -1;
}


//--------------------------------------------------------------------------------
/// class : CObjAVT
/// @param  :
/// @brief  : The current fetch mode
//--------------------------------------------------------------------------------

bool CObjAVT::CanAttackPetMode()
{
	int iPetMode = this->GetPetMode();
	if( iPetMode > 0 )
	{
		/// Pet mode In the event that ...
		/*switch( iPetMode )
		{
			case PET_TYPE_CART01:
				return false;
			case PET_TYPE_CASTLE_GEAR01:
				return true;
		}*/
		//----------------------------------------------------------------------------------------------------
		/// @brief Pet Not the type of attacks meant to determine whether the attack going the distance ...
		//----------------------------------------------------------------------------------------------------

//Park Ji-Ho: in the old cart area is configured to step 4:, cart skill version,
//		   Changed to step 5, check the distance attack weapon parts are currently in cart
//		   You can find out the attack.
#if defined(_GBC)
		if( PAT_ITEM_ATK_RANGE( this->m_sWeaponIDX.m_nItemNo ) <= 0 )
#else
		//06. 06. 28 - Kim Joo-Hyun: should you go cart Kart battle if the attack aboard the disabled
		// m_iPetType, 21 = CART, 31 = CASTLEGEAR
		if(g_pAVATAR->m_iPetType == 21)
			return false;

		if( PAT_ITEM_ATK_RANGE( this->m_sAbilIDX.m_nItemNo ) <= 0 )
#endif
			return false;

	}

	return true;
}


//--------------------------------------------------------------------------------
/// class : CObjAVT
/// @param  :
/// @brief  :
//--------------------------------------------------------------------------------

void CObjAVT::SetCMD_PET_MOVE( const D3DVECTOR& PosTO, BYTE btRunMODE )
{
	if( m_pObjCART )
	{
		m_pObjCART->SetCMD_MOVE( PosTO, btRunMODE );
	}
}


//--------------------------------------------------------------------------------
/// class : CObjAVT
/// @param  :
/// @brief  :
//--------------------------------------------------------------------------------

void CObjAVT::SetCMD_PET_MOVE( WORD wSrvDIST, const D3DVECTOR& PosTO, int iServerTarget)
{
	if( m_pObjCART )
	{
		m_pObjCART->SetCMD_MOVE( wSrvDIST, PosTO, iServerTarget );
	}
}


//--------------------------------------------------------------------------------
/// class : CObjAVT
/// @param  :
/// @brief  :
//--------------------------------------------------------------------------------

void CObjAVT::SetCMD_PET_STOP( void )
{
	if( m_pObjCART )
	{
		m_pObjCART->SetCMD_STOP();
	}
}

//--------------------------------------------------------------------------------
/// class : CObjAVT
/// @param  :
/// @brief  :
//--------------------------------------------------------------------------------

void CObjAVT::SetCMD_PET_ATTACK( int iServerTarget, WORD wSrvDIST, const D3DVECTOR& PosTO )
{
	if( m_pObjCART )
	{
		m_pObjCART->SetCMD_ATTACK( iServerTarget, wSrvDIST, PosTO );
	}
}



//--------------------------------------------------------------------------------
/// class : CObjAVT
/// @param  :
/// @brief  :
//--------------------------------------------------------------------------------

void CObjAVT::RideCartToggle( bool bRide )
{
	if( bRide == false )
	{
		if( GetPetMode() >= 0 )
		{
			/// Unloading
			DeleteCart( true );
		}else
		{
			/// Rides
			CreateCartFromMyData( true );
		}

	}else
	{
		/// Currently only the State riding a cart ride
		if( GetPetMode() < 0 )
			CreateCartFromMyData( true );
	}

	//Buff wipe.
	this->m_EndurancePack.ClearStateByDriveCart();

}


//-------------------------------------------------------------------------------------------------
#define	CHECK_TIME	1500

//--------------------------------------------------------------------------------
/// class : CObjAVT
/// @param  :
/// @brief  : HP and MP Lee Jenn CObjAI:: RecoverHPnMP () are handled in the previous code, this is a delete.	- 2004/02/11 : nAvy
///           An additional 50% recovery rate increases when you're Aroa											- 2005/07/12 : nAvy
//--------------------------------------------------------------------------------

int CObjAVT::Proc ()
{
	m_dwElapsedTime += m_dwFrameElapsedTime;

	//--------------------------------------------------------------------------------
	/// Lee cover specific handling
	//--------------------------------------------------------------------------------
	///if( GetStamina() > 3000 )
	{
#ifdef _NoRecover
	// 06. 06. 22 - Kim Joo-Hyun: auto recovery time was adjusted ... -_-a
	// _NoRecover version that does not use the Kleiner was calculated from _NoRecover when using the app in each second from the server
	// All you need to do is update the value!!
	int	iRecoverStateCheckTime = RECOVER_STATE_CHECK_TIME_EVO;
	int iRecoverStateSitOnGround = RECOVER_STATE_SIT_ON_GROUND_OLD;
	int iRecoverStateStopWalk    = RECOVER_STATE_STOP_OR_WALK_NEW;
#else
		int	iRecoverStateCheckTime = RECOVER_STATE_CHECK_TIME_OLD;
		if( IsApplyNewVersion() )
			iRecoverStateCheckTime = RECOVER_STATE_CHECK_TIME_NEW;

		int iRecoverStateSitOnGround = RECOVER_STATE_SIT_ON_GROUND_OLD;
		if( IsApplyNewVersion() )
			iRecoverStateSitOnGround = RECOVER_STATE_SIT_ON_GROUND_NEW;


		int iRecoverStateStopWalk    = RECOVER_STATE_STOP_OR_WALK_OLD;
		if( IsApplyNewVersion() )
			iRecoverStateStopWalk = RECOVER_STATE_STOP_OR_WALK_NEW;

#endif


		if ( m_dwElapsedTime > iRecoverStateCheckTime )
		{
			//_RPT3( _CRT_WARN,"Update Recover HP/MP ElapsedTime:%d, CheckTime:%d, CurrTime:%d \n", m_dwElapsedTime, iRecoverStateCheckTime, timeGetTime() );
			switch ( Get_COMMAND() )
			{
			case CMD_SIT :
					this->RecoverHP( iRecoverStateSitOnGround );
					this->RecoverMP( iRecoverStateSitOnGround );
				break;
			case CMD_DIE:
				break;
			default:
				/// Castle gear do not recover when boarding
				if( this->GetPetMode() < 0 )
				{
					/// There isn't a seat, only the behavior of the HP recovery
					this->RecoverHP( iRecoverStateStopWalk );
					this->RecoverMP( iRecoverStateStopWalk );
				}
				break;
			}
			m_dwElapsedTime -= iRecoverStateCheckTime;
		}
	}

	/*else

	{
		m_dwElapsedTime = 0;
	}*/

	//--------------------------------------------------------------------------------
	// Park Ji-Ho: pet vibe ration
	// Add m_pObjCART to the end: conditional statement (an error occurred when the cart when fit) 2005/07/31
	//--------------------------------------------------------------------------------
	if(GetPetMode() && m_pObjCART && SetCartVA())
	{
		m_pObjCART->m_ObjVibration.StartVibration();
		SetCartVA() = FALSE;
	}

	if(GetPetMode() && m_pObjCART)
		m_pObjCART->m_ObjVibration.Proc();

	//--------------------------------------------------------------------------------
	/// Time-dependent action mode processing..
	/// If the door is to lighten the moving towards a. .. 04/5/28
	//--------------------------------------------------------------------------------
	///if( this->IsA( OBJ_AVATAR ) || this->IsA( OBJ_USER ) )
	{
		m_ChangeActionMode.Proc();
	}

	return CObjCHAR::Proc ();
}


//--------------------------------------------------------------------------------
/// class : CObjAVT
/// @param  :
/// @brief  : Bullet data
//--------------------------------------------------------------------------------

void CObjAVT::SetShotData( int i, int iItemNo )
{
	if( i < 0 || i >= MAX_SHOT_TYPE )
		return;

	m_ShotData[i].m_nItemNo = iItemNo;
}


//--------------------------------------------------------------------------------
/// class : CObjAVT
/// @param  :
/// @brief  : In order to apply a durable change of the figures currently in effect is the ability to figure (passive skill)
//--------------------------------------------------------------------------------

int	CObjAVT::Get_DefaultAbilityValue( int iType )
{
	switch( iType )
	{
		case AT_SPEED:
			{
				return this->GetOri_WalkSPEED();
			}
			break;

		case AT_ATK_SPD:
			{
				return this->GetOri_ATKSPEED();
			}
			break;

		case AT_MAX_HP:
			{
				return this->m_iMaxHP;
			}
			break;

		case AT_MAX_MP:
			{
				return this->m_iMaxMP;
			}
			break;

		default:
			return 1;
	}

	return 1;
}




/*override*/int CObjAVT::GetANI_Stop()
	{
		if( m_pObjCART )
			return GetANI_Ride();
		return  AVT_ANI_STOP1;
	}
/*override*/int	CObjAVT::GetANI_Move()		{	return  m_bRunMODE ? AVT_ANI_RUN : AVT_ANI_WALK;	}
/*override*/int	CObjAVT::GetANI_Attack()	{	return( AVT_ANI_ATTACK+RANDOM(3) );	}
/*override*/int	CObjAVT::GetANI_Die ()
{
	if( m_bStopDead )
		return AVT_ANI_STOP1;
	return  AVT_ANI_DIE;
}
/*override*/int	CObjAVT::GetANI_Hit()		{	return  AVT_ANI_HIT;				}
/*override*/int	CObjAVT::GetANI_Casting ()	{	return	SKILL_ANI_CASTING(m_nActiveSkillIDX);	}
/*override*/int	CObjAVT::GetANI_CastingRepeat()	{   return SKILL_ANI_CASTING_REPEAT(m_nActiveSkillIDX);	}		/// The behavior of the loop used by the behavior of the cast ...
/*override*/int	CObjAVT::GetANI_Skill ()	{	return	SKILL_ANI_ACTION_TYPE(m_nActiveSkillIDX);	}
/*override*/int	CObjAVT::GetANI_Sitting()	{	return	AVT_ANI_SITTING;			}
/*override*/int	CObjAVT::GetANI_Standing()	{	return	AVT_ANI_STANDUP;			}
/*override*/int	CObjAVT::GetANI_Sit()		{	return	AVT_ANI_SIT;				}
/*override*/int	CObjAVT::GetANI_Ride()		{   return  ( m_pObjCART )? m_pObjCART->GetRideAniPos():0; }
/*override*/int	CObjAVT::GetANI_PickITEM()	{	return  AVT_ANI_PICKITEM; }




//----------------------------------------------------------------------------------------------------
/// @param
/// @brief
//----------------------------------------------------------------------------------------------------
WORD CObjAVT::GetPetState()
{
	if( m_pObjCART )
		return m_pObjCART->Get_STATE();

	return 0;
}

//--------------------------------------------------------------------------------
/// class : CObjAVT
/// @param  :
/// @brief  : The data set in the current internal cart creation
//--------------------------------------------------------------------------------

bool CObjAVT::CreateCartFromMyData( bool bShowEffect )
{

	//----------------------------------------------------------------------------------------------------
	/// Pet type Decision
	/// Items to be replaced by: determine the type, but it cannot know the State received from the server.
	//----------------------------------------------------------------------------------------------------
	/*CItemSlot* pItemSlot = g_pAVATAR->GetItemSlot();
	CItem* pBodyItem = pItemSlot->GetItem( INVENTORY_RIDE_ITEM0 );*/

	if( m_sBodyIDX.m_nItemNo )
	{
		this->SetPetType( PAT_ITEM_PART_TYPE( m_sBodyIDX.m_nItemNo ) );
	}else
		return false; /// The Department does not have the form, not the body ride


#ifdef _GBC
	bool bResult = CreateCart( m_iPetType, m_sEngineIDX.m_nItemNo, m_sBodyIDX.m_nItemNo,
		m_sLegIDX.m_nItemNo, m_sAbilIDX.m_nItemNo, m_sWeaponIDX.m_nItemNo );
#else
	bool bResult = CreateCart( m_iPetType, m_sEngineIDX.m_nItemNo, m_sBodyIDX.m_nItemNo,
		m_sLegIDX.m_nItemNo, m_sAbilIDX.m_nItemNo );
#endif


	/// Not all glass status revocation 2005/7/30 - nAvy
	///
	///this->m_EndurancePack.ClearStateByDriveCart();

	//----------------------------------------------------------------------------------------------------
	/// When you ride a cart should not make weapons, wings
	//----------------------------------------------------------------------------------------------------
	/*if( m_phPartVIS[ BODY_PART_KNAPSACK ] )
		::setVisibilityRecursive( *m_phPartVIS[ BODY_PART_KNAPSACK ], 0.0f );
	if( m_phPartVIS[ BODY_PART_WEAPON_R ] )
		::setVisibility( *m_phPartVIS[ BODY_PART_WEAPON_R ], 0.0f );
	if( m_phPartVIS[ BODY_PART_WEAPON_L ] )
		::setVisibility( *m_phPartVIS[ BODY_PART_WEAPON_L ], 0.0f );*/

	int iVisibilityPart[3] = { BODY_PART_KNAPSACK, BODY_PART_WEAPON_R, BODY_PART_WEAPON_L };
	for( int i = 0; i < 3; i++ )
	{
		CMODEL<CCharPART> *pCharPART = m_pCharMODEL->GetCharPART( iVisibilityPart[ i ] );
		if ( pCharPART  )
		{
			short nI;

			for (nI=0; nI<pCharPART->m_nPartCNT; nI++)
			{
				if ( m_phPartVIS[ iVisibilityPart[ i ] ][ nI ] )
				{
					::setVisibilityRecursive( m_phPartVIS[ iVisibilityPart[ i ] ][ nI ], 0.0f );
				}
			}
		}
	}


	//----------------------------------------------------------------------------------------------------
	/// The effect of the output when
	//----------------------------------------------------------------------------------------------------
	if( bShowEffect )
	{
		int iEffectNO	= PAT_RIDE_EFFECT( m_sBodyIDX.m_nItemNo );
		int iSoundNO	= PAT_RIDE_SOUND( m_sBodyIDX.m_nItemNo );

		this->ShowEffectOnCharByIndex( iEffectNO, iSoundNO );
	}

	return bResult;
}


//--------------------------------------------------------------------------------
/// class : CObjAVT
/// @param  :
/// @brief  : Card creation. Get information on each part as an argument.
//--------------------------------------------------------------------------------
bool CObjAVT::CreateCart( unsigned int iPetType, int iEnginePart, int iBodyPart, int iLegPart, int iAbilIPart,int iWeaponPart )
{
	if( iBodyPart == 0 || iEnginePart == 0 || iLegPart == 0 )
	{
		return false;
	}

	m_ObjVibration.EndVibration();

	m_iPetType = iPetType;

	if( m_pObjCART != NULL )
	{
		delete m_pObjCART;
		m_pObjCART = NULL;
	}

	m_pObjCART = g_pObjMGR->Add_CartCHAR( iPetType, this, 0 );
	if( m_pObjCART == NULL )
	{
		assert( 0 && "Create cart failed" );
		///g_itMGR.OpenMsgBox( "Failed to create cart" );
		return false;
	}

	SetPetParts( RIDE_PART_BODY,	iEnginePart, false );
	SetPetParts( RIDE_PART_ENGINE,	iBodyPart, false );
	SetPetParts( RIDE_PART_LEG,		iLegPart, false );

	///<- 2005/7/25 Modify the additional saitogurimusu cart system : nAvy
	//SetPetParts( RIDE_PART_ARMS,	iArmsIDX, false );
#ifdef _GBC
	SetPetParts( RIDE_PART_ABIL,	iAbilIPart, false );
	SetPetParts( RIDE_PART_ARMS,	iWeaponPart, false );
#else
	SetPetParts( RIDE_PART_ARMS,	iAbilIPart, false );
#endif

	///->

	if( m_pObjCART->Create( this, m_iPetType, this->Get_CurPOS() ) == false )
	{
		g_pObjMGR->Del_Object( m_pObjCART );
		return false;
	}

	D3DXVECTOR3 pos( 0.0f, 0.0f, 0.0f );
	ResetCUR_POS( pos );
	::savePrevPosition( m_hNodeMODEL );

	this->Set_ModelDIR( 0.0f );

	SetCMD_STOP();

	this->m_btMoveMODE = MOVE_MODE_DRIVE;

	return true;
}


//--------------------------------------------------------------------------------
/// class : CObjAVT
/// @param  :
/// @brief  : Remove cart
//--------------------------------------------------------------------------------

void CObjAVT::DeleteCart( bool bShowEffect )
{
	if( m_pObjCART )
	{
		float fDir = ::getModelDirection( m_pObjCART->GetZMODEL() );
		this->Set_ModelDIR( fDir );

//Park Ji-Ho
#if defined(_GBC)
		//Driver: If you're a guest at the back of the Board with me: I've been a guest at the link and
		//Removes a cart.
		if(GetRideUserIndex())
		{
			//20050901 Hong-Keun made a secondary seat of the cart 2 seater if that occupant State to Normal as baggweojunda.
			CObjAVT *oSrc = g_pObjMGR->Get_ClientCharAVT( g_pObjMGR->Get_ServerObjectIndex(GetRideUserIndex()), true );
			if(oSrc)
			{
				if( !strcmp( oSrc->Get_NAME() , g_pAVATAR->Get_NAME()) )
				{
					g_pAVATAR->Set_Block_CartRide( false );
				}
			}

			//Turn off the link
			m_pObjCART->UnLinkChild();

			//The actual object manager in the list, delete the objects in the cart.
			if(m_pObjCART)
			{	g_pObjMGR->Del_Object( m_pObjCART );	m_pObjCART = NULL;	}

			//Stop.
			SetCMD_STOP();

			goto CHAR_VISIBLE;
		}
       //2-seater passenger: I only have 2 seater or a cart.
		else if(IsRideUser())
		{
			if(m_pObjCART)
			{
				//When you pass the user index. Unlink from the inside and jumped down
				//Set the animation.
				ReleaseRideUser(m_pObjCART->GetParent()->GetRideUserIndex());

				//20050901 Hong-Keun made a secondary seat of the cart 2 seater if that occupant State to Normal as baggweojunda.
				if( !strcmp( this->Get_NAME(), g_pAVATAR->Get_NAME()) )
				{
					g_pAVATAR->Set_Block_CartRide( false );
				}

				return;
			}
		}
		//If you're riding alone typically handles disabling.
		else
		{	m_pObjCART->UnLinkChild();		}
#else
		m_pObjCART->UnLinkChild();
#endif

		if(m_pObjCART)
		{
			g_pObjMGR->Del_Object( m_pObjCART );
			m_pObjCART = NULL;
		}

		SetCMD_STOP();

		this->m_btMoveMODE = this->m_bRunMODE;


		//----------------------------------------------------------------------------------------------------
		/// When you ride a cart should not make weapons, wings
		//----------------------------------------------------------------------------------------------------
		/*if( m_phPartVIS[ BODY_PART_KNAPSACK ] )
			::setVisibility( *m_phPartVIS[ BODY_PART_KNAPSACK ], 1.0f );
		if( m_phPartVIS[ BODY_PART_WEAPON_R ] )
			::setVisibility( *m_phPartVIS[ BODY_PART_WEAPON_R ], 1.0f );
		if( m_phPartVIS[ BODY_PART_WEAPON_L ] )
			::setVisibility( *m_phPartVIS[ BODY_PART_WEAPON_L ], 1.0f );*/
CHAR_VISIBLE:
		int iVisibilityPart[3] = { BODY_PART_KNAPSACK, BODY_PART_WEAPON_R, BODY_PART_WEAPON_L };
		for( int i = 0; i < 3; i++ )
		{
			CMODEL<CCharPART> *pCharPART = m_pCharMODEL->GetCharPART( iVisibilityPart[ i ] );
			if ( pCharPART  )
			{
				short nI;

				for (nI=0; nI<pCharPART->m_nPartCNT; nI++)
				{
					if ( m_phPartVIS[ iVisibilityPart[ i ] ][ nI ] )
					{
						::setVisibilityRecursive( m_phPartVIS[ iVisibilityPart[ i ] ][ nI ], 1.0f );
					}
				}
			}
		}

		//----------------------------------------------------------------------------------------------------
		/// The effect of the output when a. ..
		//----------------------------------------------------------------------------------------------------
		if( bShowEffect )
		{
			int iEffectNO	= PAT_GETOFF_EFFECT( m_sBodyIDX.m_nItemNo );
			int iSoundNO	= PAT_GETOFF_SOUND( m_sBodyIDX.m_nItemNo );

			this->ShowEffectOnCharByIndex( iEffectNO, iSoundNO );
		}
	}
}


///--------------------------------------------------------------------------------
/// class : CObjCHAR
/// Park Ji-Ho: Process_JOIN_RIDEUSER()
///       : Driver John Warp: 2-seater passenger.
///
//--------------------------------------------------------------------------------
void CObjAVT::Process_JOIN_RIDEUSER(void)
{

	if(this->GetPetMode() < 0)
		return;

	DeleteCart(true);

}


//--------------------------------------------------------------------------------
/// class : CObjAVT
/// @param  :
/// @brief  : Kart part information settings
//--------------------------------------------------------------------------------

void CObjAVT::SetPetParts( unsigned int iPetPartIDX, unsigned int iItemIDX, bool bJustInfo )
{
	if( iPetPartIDX >= MAX_RIDING_PART )
		return;

	m_sPetPartItemIDX[ iPetPartIDX ].m_nItemNo = iItemIDX;

	if( bJustInfo == false )
	{
		if( m_pObjCART == NULL )
			return;

		m_pObjCART->SetPetParts( iPetPartIDX, iItemIDX );
	}
}

//--------------------------------------------------------------------------------
/// class : CObjAVT
/// @param  :
/// @brief  : Kart parts, such as change in the event of cart updates
//--------------------------------------------------------------------------------

void CObjAVT::UpdatePet()
{
	if( m_pObjCART )
	{
		DeleteCart();
		CreateCartFromMyData();
	}
}

//--------------------------------------------------------------------------------
/// class : CObjAVT
/// @param  : The new possession item weight ratio
//--------------------------------------------------------------------------------

void CObjAVT::SetWeightRate( BYTE btWeightRate )
{
	m_btWeightRate = btWeightRate;
}

//--------------------------------------------------------------------------------
/// class : CObjAVT
/// @return : In the current rate of possession of items
//--------------------------------------------------------------------------------

BYTE CObjAVT::GetWeightRate()
{
	return m_btWeightRate;
}

//-------------------------------------------------------------------------------------------
/// @brief The starting point of every individual opening and closing the store..
/// Private shop-only model with a series of low-tech and low-cost operations.
//-------------------------------------------------------------------------------------------
void CObjAVT::SetPersonalStoreTitle( char* strTitle, int iPersonalStoreType )
{
	if( strTitle == NULL )
	{
		if( m_phPartVIS[ 0 ] == NULL )
			m_pCharMODEL->ClearRenderUnitParts();

		// LoadVisible delete the ones.
		for (short nP=0; nP<MAX_BODY_PART; nP++)
		{
			if( m_phPartVIS[ nP ] == NULL )
				CreateSpecificPART( (char*)m_Name.c_str(), nP );
		}

		this->CreateGemmingEffect();
		this->CreateGradeEffect();

		m_bPersonalStoreMode = false;

		if( m_pObjPersonalStore )
		{
			m_pObjPersonalStore->UnlinkVisibleWorld();
			///m_pObjPersonalStore->RemoveFromScene();
			delete m_pObjPersonalStore;
			m_pObjPersonalStore = NULL;
		}
		return;
	}

	m_pObjPersonalStore = new CObjTREE();

	///D3DXVECTOR3 vPos = this->Get_CurPOS();
	D3DXVECTOR3 vPos( 0.0f, 0.0f, 0.0f );
	const int iPersonalStoreNO = 260;
	if( m_pObjPersonalStore->Create( g_DATA.m_ModelFieldITEM.GetMODEL( iPersonalStoreNO + iPersonalStoreType ), vPos ) )
	{
		//-------------------------------------------------------------------------------------------
		/// Delete all character parts,
		//-------------------------------------------------------------------------------------------
		::clearRenderUnit( this->m_hNodeMODEL );
		for (short nP=0; nP<MAX_BODY_PART; nP++)
		{
			DeleteSpecificPART( nP, m_phPartVIS[ nP ] );
			//m_pCharMODEL->DeletePART( nP, m_phPartVIS[ nP ] );
			m_phPartVIS[ nP ] = NULL;
		}

		m_pObjPersonalStore->LinkToModel( this->GetZMODEL() );
	}else
	{
		delete m_pObjPersonalStore;
		m_pObjPersonalStore = NULL;
	}

	m_bPersonalStoreMode = true;
	m_strPersonalStoreTitle = std::string( strTitle );
}

const char*	CObjAVT::GetPersonalStoreTitle()
{
	return m_strPersonalStoreTitle.c_str();
}

bool CObjAVT::IsPersonalStoreMode()
{
	return m_bPersonalStoreMode;
}

/// Equipment replacement equipment hanging off the property in..
void CObjAVT::ClearRWeaponSkillEffect()
{
	this->m_EndurancePack.ClearRWeaponSkillEffect();
}

void CObjAVT::ClearLWeaponSkillEffect()
{
	this->m_EndurancePack.ClearLWeaponSkillEffect();
}

//----------------------------------------------------------------------------------------------------
/// @brief When Pat returns the coordinates of the Pat mode, you typically return my coordinates
//----------------------------------------------------------------------------------------------------
const D3DVECTOR& CObjAVT::GetWorldPos()
{
	if( GetPetMode() > 0 )
	{
		if( m_pObjCART )
		{
			return m_pObjCART->Get_CurPOS();
		}
	}

	return Get_CurPOS();
}


//----------------------------------------------------------------------------------------------------
/// @brief Besides the renderUnit of registered in the model for judging conflicts. .. (Avatar, the only)
//----------------------------------------------------------------------------------------------------
bool CObjAVT::IsIntersectAccessory( float &fCurDistance )
{
	/// Represent your store model
	if( this->m_pObjPersonalStore )
	{
		return m_pObjPersonalStore->IsIntersectForCamera( fCurDistance );
	}
	return false;
}
//----------------------------------------------------------------------------------------------------
/// @brief The party member's HP recovery and Stamina-related Method for automatic
///		   In the case of CUserDATA: CObjUSER: GetCur_STAMINA. You will notice that the
//----------------------------------------------------------------------------------------------------
short CObjAVT::GetStamina()
{
	return m_nStamina;
}
void CObjAVT::SetStamina( short nStamina )
{
	m_nStamina = nStamina;
}

//----------------------------------------------------------------------------------------------------
/// @brief m_dwSubFLAG Conversion of special large State to the flag
//----------------------------------------------------------------------------------------------------

void CObjAVT::ChangeSpecialState( DWORD dwSubFLAG )
{
	m_dwSubFLAG = dwSubFLAG;

	if( m_dwSubFLAG & FLAG_SUB_HIDE )
	{
		::setVisibilityRecursive( this->GetZMODEL(), 0.0f );
	}
	//Kwok Hong Geun:: transparent
	else if( !(m_dwSubFLAG & FLAG_SUB_HIDE) )
	{
		::setVisibilityRecursive( this->GetZMODEL(), 1.0f );

		::setVisibilityRecursive( this->GetZMODEL(), 1.0f );

		//2005 11-29 cart check the burnt composition Prefecture instructs
		if( this->m_iPetType == 21 )
		{
			int iVisibilityPart[3] = { BODY_PART_KNAPSACK, BODY_PART_WEAPON_R, BODY_PART_WEAPON_L };

			for( int i = 0; i < 3; i++ )
			{
				CMODEL<CCharPART> *pCharPART = m_pCharMODEL->GetCharPART( iVisibilityPart[ i ] );

				if ( pCharPART  )
				{
					short nI;
                    for (nI=0; nI<pCharPART->m_nPartCNT; nI++)
					{
						if ( m_phPartVIS[ iVisibilityPart[ i ] ][ nI ] )
						{
							::setVisibilityRecursive( m_phPartVIS[ iVisibilityPart[ i ] ][ nI ], 0.0f );
						}
					}
				}
			}
		}


	}
}

//----------------------------------------------------------------------------------------------------
/// @brief You can click an object?
//----------------------------------------------------------------------------------------------------
bool CObjAVT::CanClickable()
{
	if( m_dwSubFLAG & FLAG_SUB_HIDE )
		return false;

	if( ::getVisibility( this->GetZMODEL() ) < 0.1 )
		return false;

	return true;
}

//----------------------------------------------------------------------------------------------------
/// @brief virtual From CObjAI   : Add  2005/7/13 - nAvy
//----------------------------------------------------------------------------------------------------
int CObjAVT::GetOri_MaxHP()
{
	return m_iMaxHP;
}

//Base stats + items + passive
int	CObjAVT::Get_AbilityValue_StatItemPas( WORD nType )
{
	switch( nType )
	{
	case AT_SPEED	:		return (int)GetOri_RunSPEED();
	case AT_ATK_SPD :		return GetOri_ATKSPEED();
	case AT_MAX_HP :		return GetOri_MaxHP ();
	}
	return 0;
}
///////////////////////////////////////////////////////////////////////////////////////////
// 2005. 6. 20	Park Ji-Ho
//
// Goddess summoning and directing class
///////////////////////////////////////////////////////////////////////////////////////////


CGoddessMgr goddessMgr;
//Constructor
CGoddessMgr::CGoddessMgr()
{

	Init();

}


//The destructor
CGoddessMgr::~CGoddessMgr()
{

//	Release();

}


//Initialization
BOOL CGoddessMgr::Init(void)
{

	m_IsUse		= FALSE;
	m_IsAvata	= FALSE;

	m_dwAvata	= 0;
	m_Count		= 0;


	return TRUE;

}


//Releasing memory
void CGoddessMgr::Release(void)
{

//List disable
//-----------------------------------------------------------------------------------
	GODDESSSTR* pGds = NULL;
	for(GODLIST itr = m_list.begin(); itr != m_list.end();itr++)
	{
		pGds = &(itr->second);
		 if(pGds == NULL)
			continue;

		 Release_List(pGds);
	}

	m_list.clear();
	m_Count = 0;
//------------------------------------------------------------------------------------

	Init();
}


//Only one list.
void CGoddessMgr::Release_List(GODDESSSTR* pGDS)
{

	delete pGDS->pGODModel;
	delete pGDS->pSprModel;

	for(register int i=0;i<2;i++)
	{
		delete  pGDS->pEffect[i];
		pGDS->pEffect[i]	= NULL;
		pGDS->hParticle[i]  = NULL;
	}

	pGDS->pGODModel		= NULL;
	pGDS->pSprModel		= NULL;

	pGDS->hGoddess		= NULL;
	pGDS->hSprite		= NULL;

}


//Check the allow countries
BOOL CGoddessMgr::Permission_Country(void)
{

	//After checking the results the country settings.
	if( IsApplyNewVersion())
		return TRUE;


	return FALSE;

}


//Goddess, nymph, loads the object.
BOOL CGoddessMgr::Register_God_And_Sprite(void)
{

	if(m_IsUse)
		return TRUE;

	return (m_IsUse = TRUE);

}


//Rendering objects object.
BOOL CGoddessMgr::Register_RenderObj(GODDESSSTR& gds)
{

	//Avatar brings objects.
	CObjAVT *pCHAR = g_pObjMGR->Get_ClientCharAVT(gds.idxMaster,TRUE);
		if(pCHAR == NULL)
			return FALSE;

	//Avatar set state
	pCHAR->SetAroaState() = gds.bEnable;

	//
	pCHAR->Calc_AruaAddAbility();

	//If you own avatar information.
	m_IsAvata = (lstrcmp(pCHAR->Get_NAME(),g_pAVATAR->Get_NAME()) ? FALSE : TRUE);


	char* ParticlName[] =
	{ "3Ddata\\Effect\\_arua_ghost01.eft", "3Ddata\\Effect\\end_effect_02.eft" };

	for(register int i=0;i<2;i++)
	{
		if(gds.pEffect[i]!= NULL)
		{ delete gds.pEffect[i]; gds.pEffect[i] = NULL; }

		DWORD hKey = CStr::GetHASH(ParticlName[i]);

		gds.pEffect[i] = g_pEffectLIST->Add_EFFECT(hKey);
		if(	gds.pEffect[i] == NULL)
		{
			_ASSERT(gds.pEffect[i]);
			return FALSE;
		}

		gds.pEffect[i]->LinkNODE(pCHAR->GetZMODEL());
		gds.hParticle[i] = gds.pEffect[i]->GetZNODE();
	}


	//Goddess road
	if(gds.pGODModel == NULL)
	{
		gds.pGODModel = new CObjMOB;
		if (gds.pGODModel == NULL)
		{
			_ASSERT(gds.pGODModel);
			return FALSE;
		}

		D3DXVECTOR3 cpos = pCHAR->Get_CurPOS();
		if (!gds.pGODModel->Create(939,cpos,0,TRUE))
		{
			LogString (LOG_DEBUG, "Create Goddess Model Data\n");
			delete gds.pGODModel; gds.pGODModel = NULL;
		}

		gds.hGoddess = gds.pGODModel->GetZMODEL();
		::setRepeatCount(gds.hGoddess,1);
		::setAnimatableFrame(gds.hGoddess,0);
		::controlAnimatable(gds.hGoddess,0);
	}


	//Fairy road
	if(gds.pSprModel == NULL)
	{
		gds.pSprModel = new CObjMOB;
		if (gds.pSprModel == NULL)
		{
			_ASSERT(gds.pSprModel);
			return FALSE;
		}

		D3DXVECTOR3 cpos = pCHAR->Get_CurPOS();

		if (!gds.pSprModel->Create(940,cpos,0,FALSE))
		{
			LogString (LOG_DEBUG, "Create Sprite Model Data\n");
			delete gds.pSprModel; gds.pSprModel = NULL;
		}

		gds.hSprite = gds.pSprModel->GetZMODEL();
	}

	::controlAnimatable(gds.hGoddess,0);
	::setVisibilityRecursive(gds.hGoddess,0.0f);
	::setVisibilityRecursive(gds.hSprite,0.0f);

	return TRUE;

}


void CGoddessMgr::Set_GDSData(GODDESSSTR& gds,BOOL bonOff,BOOL bPrograss)
{

	if(bPrograss)
	{
		if(bonOff)
			gds.god_State = GOD_SPRITE_EFFECT;
		else
			gds.god_State = GOD_NONE;
	}
	else
	{
		if(bonOff)
			gds.god_State = GOD_APPEAR_PARTCLE;
		else
			gds.god_State = GOD_END_EFFECT;
	}

}


//Goddess summoning on/off.
BOOL CGoddessMgr::SetProcess(DWORD bonOff,WORD idx,BOOL bPrograss)
{

  //Country code check
	if(!Permission_Country())
		return TRUE;


	GODDESSSTR Gds,*pGds = NULL;

	BOOL bOnOff = bonOff ? 1 : 0;


//Old list
//-------------------------------------------------------------------
	GODLIST itr = m_list.find(idx);

	if(itr != m_list.end())
	{
		itr->second.bEnable		= bOnOff;
		itr->second.sTick		= GetTickCount();

		Set_GDSData(itr->second,bOnOff,bPrograss);

		if(!Register_RenderObj(itr->second))
			return FALSE;

		pGds = &(itr->second);

		goto SET_EFF;
	}
//--------------------------------------------------------------------


//New list
//--------------------------------------------------------------------
	//If you need to create the list OFF in the first place.
	if(bOnOff == FALSE)
		return TRUE;


	Gds.Init();

	Gds.bEnable		= bOnOff;
	Gds.sTick		= GetTickCount();
	Gds.idxMaster	= idx;

	Set_GDSData(Gds,bOnOff,bPrograss);

	if(!Register_RenderObj(Gds))
		return FALSE;

	m_list[idx] = Gds;
	m_Count = m_list.size();

	pGds = &Gds;
//--------------------------------------------------------------------

//Effect rendering settings
SET_EFF:

	//If you save the State of the Arua avatar.
	if(m_IsAvata)
		m_dwAvata = bonOff;


	//If you make it look elfin mode.
	if(pGds->god_State == GOD_SPRITE_EFFECT)
	{
		pGds->fviewSpr = 1.0f;
		::setVisibilityRecursive(pGds->hSprite,1.0f);
	}


	//Setting the particle animation
	if(pGds->god_State == GOD_APPEAR_PARTCLE)
	{
		pGds->pEffect[0]->StartEffect();
		pGds->fviewGODD = 0.0f;
	}
	else
		pGds->pEffect[0]->StopEffect();

	if(pGds->god_State == GOD_END_EFFECT)
	{
		pGds->pEffect[1]->StartEffect();
		pGds->fviewSpr = 1.0f;
	}
	else
		pGds->pEffect[1]->StopEffect();


	return TRUE;

}


void CGoddessMgr::Update(void)
{

	//Country code check
	if(!Permission_Country())
		return;


#define TIME_GOD_ACCEPT				9100   //Rendering the entire tick
#define TIME_APPEAR_GODDESS			3000   //The goddess appears after starting the effect time tick


	if(m_Count == 0)
		return;


	CObjAVT *pCHAR = NULL;
	DWORD curTick=0,tempTick=0;
	D3DXVECTOR3 tPos = D3DXVECTOR3(0,0,0);

	BOOL IsBeginList = FALSE;
	GODLIST preNode;

	float fRot[4] = { 0 };
	int t1=0,t2=0;



	for(GODLIST itr=m_list.begin();itr!=m_list.end();++itr)
	{

		GODDESSSTR* gds = &(itr->second);
			if(gds == NULL)
				continue;


	//	if(gds->god_State == GOD_NONE)
	//		continue;


		//Avatar brings objects.
		pCHAR = g_pObjMGR->Get_ClientCharAVT(gds->idxMaster,TRUE);
			if((pCHAR == NULL) || (gds->god_State == GOD_NONE))
			{
				Release_List(&(itr->second));
				if(itr != m_list.begin())
				{
					preNode = --itr;
					++itr;
					IsBeginList = FALSE;
				}
				else
					IsBeginList = TRUE;


				m_list.erase(itr);

				if(!(m_Count = m_list.size()))
					break;

				//Section of the list is a list Begin if
				if(IsBeginList)
					itr = m_list.begin();
				else
					itr = preNode;

				continue;
			}


		//The coordinates of the avatar
		D3DXVECTOR3 cPos = tPos = pCHAR->Get_CurPOS();

		//Avatar rotation
		::getRotationQuad(pCHAR->GetZMODEL(),fRot);


		//step1 :  Particle effects output
		if(gds->god_State == GOD_APPEAR_PARTCLE)
		{
			curTick  = GetTickCount();
			tempTick = (curTick - gds->sTick);

			//The presence or absence of the goddess character output
			if(tempTick >= TIME_APPEAR_GODDESS)
			{
				if(gds->fviewGODD == 0)
				{
					::setRepeatCount(gds->hGoddess,1);
					::setAnimatableFrame(gds->hGoddess,0);
					::controlAnimatable(gds->hGoddess,1);
				}

				//Goddess character to appear.
				if(ProcessVisible(gds->fviewGODD,0.0008f) == 1)
					gds->god_State = GOD_APPEAR_GODDESS;

				::setRotationQuat(gds->hGoddess,fRot);
				::setPosition(gds->hGoddess,tPos.x,tPos.y,tPos.z);
				::setVisibilityRecursive(gds->hGoddess,gds->fviewGODD);
			}
		}


		//step2 : Particle &amp; goddess effect output
		if(gds->god_State == GOD_APPEAR_GODDESS)
		{
			curTick  = GetTickCount();
			tempTick = (curTick - gds->sTick);


			//At the end of the regular cast effects fairy tracking mode.
			if(tempTick >= TIME_GOD_ACCEPT)
			{
				//Goddess character to appear.
				t1 = ProcessVisible(gds->fviewGODD,-0.0009f);
				t2 = ProcessVisible(gds->fviewSpr,0.001f);

				if((t1 == 0) && (t2 ==    1))
				{
					gds->god_State = GOD_SPRITE_EFFECT;
					::controlAnimatable(gds->hGoddess,0);
				}

				//Fairy
				::setRotationQuat(gds->hSprite,fRot);
				::setPosition(gds->hSprite,tPos.x,tPos.y,tPos.z);
				::setVisibilityRecursive(gds->hSprite,gds->fviewSpr);
			}

			//Goddess
			::setRotationQuat(gds->hGoddess,fRot);
			::setPosition(gds->hGoddess,tPos.x,tPos.y,tPos.z);
			::setVisibilityRecursive(gds->hGoddess,gds->fviewGODD);
		}

		//step3 : Fairy tracking mode
		if(gds->god_State == GOD_SPRITE_EFFECT)
		{
			::setRotationQuat(gds->hSprite,fRot);
			::setPosition(gds->hSprite,tPos.x,tPos.y,tPos.z);

		}

		//step4 ; The fairies disappear.
		if(gds->god_State == GOD_END_EFFECT)
		{

			if(ProcessVisible(gds->fviewSpr,-0.001f) == 0)
				gds->god_State = GOD_NONE;

            			::setRotationQuat(gds->hSprite,fRot);
			::setPosition(gds->hSprite,tPos.x,tPos.y,tPos.z);
			::setVisibilityRecursive(gds->hSprite,gds->fviewSpr);
		}

	}

}


//Visible Processing..
int CGoddessMgr::ProcessVisible(float& fv,float fseq)
{

	float fdelta =  g_GameDATA.GetElapsedFrameTime();

	fv+=(fseq * fdelta);

	if(fv > 1.0f)
	{	fv = 1.0f; return 1; }

	if(fv < 0.0f)
	{	fv = 0.0f; return 0; }


	return -1;

}

