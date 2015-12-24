/*
	$Header: /Client/CObjCHAR.h 368   06-11-29 5:49a Choo0219 $
*/
#ifndef	__COBJCHAR_H
#define	__COBJCHAR_H
#include "IO_Model.h"
#include "CModelCHAR.h"
#include "Common\CObjAI.h"
#include "Common\IO_STB.h"
#include "common\IO_AI.h"
#include "Network\NET_Prototype.h"	
#include "Interface/CEnduranceProperty.h"
#include "GameProc/CDayNNightProc.h"
#include "GameProc/Objvibration.h"

#include "ObjectCommand/ObjCommand.h"

#include <VECTOR>
#include <numeric>


#ifdef	__VIRTUAL_SERVER
#include "Common\\CRegenAREA.h"
#endif


#ifndef	__SERVER
#include "GameProc\changeactionmode.h"
#endif


#define	QUEST_EVENT_ON_DEAD		0


#define MIN_ATK			30
#define MIN_DEF			30
#define MIN_HIT			30
#define MIN_AVOID		30
#define MIN_ATK_SPD		30
#define MIN_RES			10
#define MIN_CRITICAL	10

#ifdef __CAL_BUF2
#define MIN_SPEED		100
#else
#define MIN_SPEED		200
#endif


const int INVALID_DUMMY_POINT_NUM = 999;

//struct uniDAMAGE;

///
/// Damage class
///
struct tagDAMAGE {
	short		m_nTargetObjIDX;
	uniDAMAGE	m_wDamage;			//Honggeun: Max Damage modification
} ;


class CChangeVisibility;
class CObjCHAR_Collision;
class CObjFIXED;
class CClanMarkUserDefined;
//Bakjiho 
class CObjCART;
class CObjAVT;

#ifdef __NPC_COLLISION
class CObjCHAR_Collision2NPC;
#endif

///
///	CObjCHAR class inherited from COjbAI
/// This is base class of AVATA, MOB class.
///

class CObjCHAR : public CObjAI
{

protected:
	int							m_iHP;
	int							m_iMP;
	int							m_iMaxHP;
	int							m_iMaxMP;


	CChangeVisibility*			m_pChangeVisibility;
	bool						m_bStopDead;			/// Stop State of death. (Ie there is no conversion of the motion to die.)
    

public:
	float						m_fHeightOfGround;		// Center character in the default terrain height (relative height)
	bool						m_bDead;				/// Dead damage from the server to receive?
	/// Set on the height (the height of flying units)
	float						m_fAltitude;


/*
	// 06. 04. 18 - Kim, Joo - Hyun: Cart ride / exchange / friend add / modify an overflow error message party for invited members of the relevant variables.
	   FindMsgBox / IsOpenedMsgBox 've've Done to check;; The first window is not active in the game and letting the message, the rush
	   Check to fail. So, make a separate check for the variable;
*/
	bool						m_bCartInvitationWnd;	   //Cart-related messages is enabled there?

	int							GetMinStateValue( int iIng_Type, int iValue );	//Honggeun: Apply a minimum value of Springerville.

	void						SetCartInvitationWnd(bool binvitation)  { m_bCartInvitationWnd = binvitation; }
	bool						GetCartInvitationWnd() { return m_bCartInvitationWnd; }


    bool						m_bExChangeWnd;  // There is active exchange-related messages?

	void						SetExChangeWnd(bool bExchange)  { m_bExChangeWnd = bExchange; }
	bool						GetExChangeWnd() { return m_bExChangeWnd; }


	bool						m_bAddFriendWnd;  // There is active and relevant message Add to friends?

	void						SetAddFriendWnd(bool bAddFriend)  { m_bAddFriendWnd = bAddFriend; }
	bool						GetAddFriendWnd() { return m_bAddFriendWnd; }

	
	bool						m_bPartyInvitationWnd; // Related party invitation message is active there?
	void						SetPartyInvitationWnd(bool bParty)  { m_bPartyInvitationWnd = bParty; }
	bool						GetPartyInvitationWnd() { return m_bPartyInvitationWnd; }

	float						GetStature();

#ifdef __NPC_COLLISION

	bool                        m_bCollisionOnOff;
	int                         m_iCollisionBoneIndex;
	float                       m_fCollisionBoxLength[3];
    int                         m_iPreCollisionNPCState;
	int                         m_iCurrentCollisionNPCState;

#endif 

protected :
	CObjCHAR_Collision			* m_pCollision; // Conflicting information

#ifdef __NPC_COLLISION	
	
   CObjCHAR_Collision2NPC      *m_pCollision2NPC; //NPC Conflict
	
#endif 






	float						m_fStature;				// Height (back)
	float						m_fScale;
	HNODE						m_hNodeGround; // The engine handles objects in pursuing

	bool						m_bUseResetPosZ; // Whether the new Z value to be updated.
	float 						m_fResetPosZ; // New updates to the Z value. Then calculate the height, this value should be updated.
	float						m_fModelSPEED; // Model transfer speed.

	/// Team.
	int							m_iTeamInfo;			// Team.


	void						Add_EFFECT(CMODEL<CCharPART> *pCharPART, short nPartIDX, short nPointIDX, t_HASHKEY HashEffectFILE, bool bLinkNODE=true );
	void						LoadTRAIL   (CMODEL<CCharPART> *pCharPART, short nPartIDX, bool bLinkBONE, int iColorNO, int iDuration, int iBaseDummyIdx = 0 );
	void						UnloadTRAIL (short nPartIDX);

	CAI_OBJ*					AI_FindFirstOBJ( int iDistance );
	CAI_OBJ*					AI_FindNextOBJ ();

	bool						m_bFrameING;



	
protected:
	CCharMODEL					*m_pCharMODEL;

	CEffect						**m_ppBoneEFFECT;
	short						m_nEffectPointCNT[ MAX_BODY_PART ];
	CEffect						**m_pppEFFECT[ MAX_BODY_PART ];	// The default effect.

	int							m_iLastDummyIDX;

	HNODE						m_hNodeMODEL;
	/// Model nodes..
	
	union 
	{
		HNODE					*m_phPartVIS[ MAX_BODY_PART ];
		struct 
		{
			// So ... in order t_CharPART
			HNODE				*m_phFaceVIS;
			HNODE				*m_phHairVIS;
			HNODE				*m_phHelmetVIS;

			HNODE				*m_phArmorVIS;
			HNODE				*m_phGauntletVIS;
			HNODE				*m_phBootsVIS;

			HNODE				*m_phGoggleVIS;
			HNODE				*m_phKnapsackVIS;

			HNODE				*m_phRWeaponVIS;
			HNODE				*m_phLWeaponVIS;
		} ;
	} ;

	/// Nodes of Trailes..
	union 
	{
		HNODE					m_hTRAIL[ 2 ];	// right=0, left=1
		struct 
		{
			HNODE				m_hR_TRAIL;
			HNODE				m_hL_TRAIL;
		} ;
	} ;


	union 
	{
		short					m_nCharIdx;		// NPC, MOB Number
		bool					m_bIsFemale;	// Avatar: The Man 0, female 1
		BYTE					m_btRace;		// Race ...
	} ;

	

	int							GetCurrentFrame ()						{	return ::getMotionFrame ( this->m_hNodeMODEL );		}
	void						ActionEVENT (int iActionIDX);
	
//--------------------------------------------------------------------------------
/// NPC  For dealing with conflict effects for special effects
	void                        ActionSpecialEVENT(int iIndex);
//--------------------------------------------------------------------------------
	
	DWORD						m_dwLastRecoveryUpdateTime;	///HP and MP will automatically recover the previous time been
	DWORD						m_dwFrameElapsedTime;		/// Hours before treatment. Flows.
	DWORD						m_dwElapsedTime;			/// Hours before treatment. Flows.

public:
	void						ClearTimer();

	CObjCHAR_Collision*			GetCollisionUtility(){ return m_pCollision; }
   
	virtual int					Get_AbilityValue_StatItemPas( WORD nType ){	return 0;	}
    
private:

	///
	///	Process action frame..
	///
	void						ActionSpecialFrame( int iActionIDX );

	void						ActionInFighting( int iActionIDX );
	void						ActionBow( int iActionIDX );
	void						ActionGun( int iActionIDX );
	void						ActionSkill( int iActionIDX );
	void						ActionImmediateSkill( int iActionIDX );

	void						FireEffectBullet( CObjCHAR *pTarget, int iBulletIDX, bool bDummy, int iSkillIDX );

	/// 44,64,75, 45,65,75 Treatment.
	void						ShowCastingEffect( int iActionIDX );
	void						ActionSkillCastingFrame( int iCastingOrder );
	/// Battered attack effects (dummy.)
	void						RepeatedlyHitEffect( int iHitOrder );


	enum	
	{
		LEFT_WEAPON = 0,
		RIGHT_WEAPON,
	};

public:
	void						Adjust_HEIGHT (void);
	void						ResetCUR_POS( D3DXVECTOR3& Pos );

	/// @brief Value received from the server the next time the height Adjust_Height () updates the value of the height settings.
	/// @param bUseResetPosZ Whether to update the values ??for the height. When you set the height value to true, false, when you enter the initialization.
	/// @param fPosZ New height values ??to be updated.
	/// @see GetResetPosZ()
	void						SetResetPosZ( bool bUseResetPosZ, float fPosZ )
	{
		if (m_bUseResetPosZ = bUseResetPosZ) { // = Not a typo.
			m_fResetPosZ = fPosZ;
		}
	}

	/// @brief Received from the server whether there is a high value and a high value are obtained.
	/// @param fResetPosZ Change the value of a new high
	/// @return Valid, true, returns false if not valid
	/// @see SetResetPosZ()
	bool						GetResetPosZ( float& fResetPosZ )
	{
		if (m_bUseResetPosZ) {
			fResetPosZ = m_fResetPosZ;
			return true;
		}
		return false;
	}

	bool						LoadModelNODE (char *szName);
	void						UnloadModelNODE ();

	void						Link_EFFECT (void);
	void						Unlink_EFFECT (void);

	void						New_EFFECT(short nPartIdx, short nItemNo, bool bLinkNODE=true);

	void						CreateSpecificPART( char *szName, int iPartIDX );
	void						DeleteSpecificPART( short nPartIdx, HNODE *pVIS );

	void						CreatePARTS (char *szName);
	void						DeletePARTS (bool bDelEFFECT=true);

	bool						CreateCHAR (char *szName, CCharMODEL *pMODEL, short nCharPartCNT, const D3DVECTOR &Position);
	void						DeleteCHAR (void);				// avatar, mob Deletion


	void						SetTeamInfo( int iTeamInfo ){ m_iTeamInfo = iTeamInfo; }

	
	//////////////////////////////////////////////////////////////////////////////////////////	
	/// < Inherited from CGameObj virtual functions
	/// <

	/*override*/virtual bool	IsInViewFrustum()						{ return ( 0 != ::inViewfrustum( GetZMODEL() ) );	}	
	/*override*/bool			IsIntersect (float &fCurDistance)			{	return	CGameOBJ::IsIntersect(m_hNodeMODEL, fCurDistance);	}
	/*override*/void			InsertToScene (void);
	/*override*/void			RemoveFromScene (bool bIncludeEFFECT=true);

	/*override*/D3DXVECTOR3&	Get_CurPOS ();
	/// <
	/// < End
	//////////////////////////////////////////////////////////////////////////////////////////




	//////////////////////////////////////////////////////////////////////////////////////////	
	/// < Inherited from CObjAI virtual functions
	/// <

	/*override*/ void			SetEffectedSkillFlag( bool bResult );
	/*override*/ bool			bCanActionActiveSkill();
	/*override*/ void			SetStartSkill( bool bResult ){ m_bCanStartSkill = bResult; }
	/*override*/ bool			bCanStartSkill(){ return m_bCanStartSkill; }					/// Casting Start packets from the server receive?

	/*override*/ virtual int	Get_MaxHP ()					   {   return 1;			}
	/*override*/ virtual int	Get_MaxMP ()                       {   return 1;			}

	/*override*/ virtual int	Get_HP ()						{	return m_iHP;			}
	/*override*/ virtual int	Get_MP ()                       {   return m_iMP;			}

	/*override*/virtual void	Set_HP (int iHP);
	/*override*/virtual void	Set_MP (int iMP);

	/*override*/ bool			SetCMD_ATTACK ( int iServerTarget ){ return CObjAI::SetCMD_ATTACK( iServerTarget ); }

	/*override*/bool			IsInRANGE (CObjCHAR *pT, int iRange)		{	return CD3DUtil::distance ((int)m_PosCUR.x, (int)m_PosCUR.y, (int)pT->m_PosCUR.x, (int)pT->m_PosCUR.y ) <= iRange;	}

	/*override*/void			Adj_MoveSPEED ( WORD wSrvDIST, const D3DVECTOR& PosGOTO );
	/*override*/void			Adj_AniSPEED ( float fAniSpeed );
	/*override*/virtual int		Get_AttackRange ();
	/*override*/void			Set_MOTION (short nActionIdx, float fMoveSpeed=0, float fAniSpeed=1.0f, bool bAttackMotion=false, int iRepeatCnt = 0 );
	/*override*/void			Move_COMPLETED ();

	/*override*/void			Reset_Position ();

	///
	/// concerned command
	///
	/*override*/virtual void	SetCMD_ATTACK (int iServerTarget, WORD wSrvDIST, const D3DVECTOR& PosGOTO);	
	/*override*/bool			Attack_START (CObjCHAR *pTarget);
	/*override*/bool			Skill_START (CObjCHAR *pTarget);
	/*override*/bool			Casting_START( CObjCHAR *pTarget );
	/*override*/void			Casting_END();
	/*override*/void			Attack_END (bool bStopTrail=true);
	/*override*/void			MoveStart ();


	///*override*/void			SetCMD_SIT (void);
	///*override*/void			SetCMD_STAND (void);
	///*override*/void			SetCMD_MOVE (tPOINTF &PosFROM, tPOINTF &PosTO, int iServerTarget);
	///*override*/void			SetCMD_DIE ();
	///*override*/bool			SetCMD_TOGGLE (BYTE btTYPE);

	//
	///*override*/void			SetCMD_STOP ();
	///*override*/void			SetCMD_MOVE (float fPosX, float fPosY, BYTE btRunMODE);
	///*override*/void			SetCMD_RUNnATTACK(int iTargetObjTAG)

	///*override*/void			SetCMD_Skill2SELF (short nSkillIDX);
	///*override*/bool			SetCMD_Skill2OBJ (int iServerTarget, short nSkillIDX);
	///*override*/void			SetCMD_Skill2POS (tPOINTF &PosGOTO, short nSkillIDX);



	/// Look to see where the target rotation model.
	/*override*/void			Set_ModelDIR (t_POSITION &PosToView, bool bImmediate = false );
	/*override*/void			Set_ModelDIR ( float fAngleDegree, bool bImmediate = false );


	/// The moving speed of the model is changed.
	/*override*/void			Set_ModelSPEED (float fMoveSpeed)
	{
		m_fModelSPEED = fMoveSpeed;
		::setModelMoveVelocity( m_hNodeMODEL, m_fModelSPEED );
	}
	/// Retrieves the movement speed of the model.
	/*override*/float			Get_ModelSPEED ()
	{
		return m_fModelSPEED;
	}


	
	/*
	Each layer between the object types are Get_WalkSPEED (), GetOri_RunSPEED () through the implementation of the moving speed is determined.!
	Eventually Get_WalkSPEED (), GetOri_RunSPEED () Write the neural implementation of the inheritance.
	*/
	/*override*/ virtual float			Get_MoveSPEED ();
				 virtual float			Get_DefaultSPEED ();

	/*override*/ virtual short			Get_nAttackSPEED ();
	/*override*/ virtual float			Get_fAttackSPEED ();

	/*override*/ DWORD			GetIngDurationStateFLAG(){ return m_EndurancePack.GetStateFlag(); }; /// Get the current value of the persistent state flags.
	/*override*/ short			GetIngDurationStateSKILL( eING_TYPE eTYPE ){ return 0; };	/// Number skills are applied to the steady state

	/// RECOVER_STATE_CHECK_TIME Get_RecoverHP intervals obtained from HP, Get_ReocverMP obtained from adding to the MP.
	/*override*/ virtual void	RecoverHP( short nRecoverMODE );	
	/*override*/ virtual void	RecoverMP( short nRecoverMODE );	

	/*override*/virtual void	Do_DeadEvent	(CObjCHAR *pTarget){};	

	/*override*/virtual void	SetMotionRepeatCount( int iRepeatCount ) { ::setRepeatCount( GetZMODEL(), iRepeatCount ); };
					


protected:
	CChangeActionMode			m_ChangeActionMode;

public:
	/*override*/void			ChangeActionMode( int iActionMode ){ return m_ChangeActionMode.ChangeActionMode( iActionMode ); }


	/*override*/ virtual int	GetPetMode(){ return -1; }			

	/// <
	/// < End
	//////////////////////////////////////////////////////////////////////////////////////////




	//////////////////////////////////////////////////////////////////////////////////////////	
	/// < Inherited from CAI_OBJ virtual functions
	/// <
	
	/*override*/int				Get_TeamNO ()						{	return m_iTeamInfo;}	/// Number received from the server team
	/*override*/BYTE			Is_DAY()							{	return g_DayNNightProc.IsDay();}			/// Check the time zone smaller than the evening?
	/*override*/int				Get_ZoneTIME()							{	return g_DayNNightProc.GetZoneTime();}			/// Check the time zone smaller than the evening?
	

	/*override*/float			Get_CurXPOS ()						{	return m_PosCUR.x;	}
	/*override*/float			Get_CurYPOS ()						{	return m_PosCUR.y;	}
	/*override*/float			Get_CurZPOS ()						{	return m_PosCUR.z;}
	/*override*/float			Get_BornXPOS ()						{	return m_PosBORN.x;	}
	/*override*/float			Get_BornYPOS ()						{	return m_PosBORN.y;	}	


	/*override*/int				Get_RANDOM (int iMod)				{	return  (rand()%iMod);												}
	/*override*/float			Get_MoveDISTANCE ()					{	return	CD3DUtil::distance(m_PosBORN, m_PosCUR);					}
	/*override*/float			Get_DISTANCE (CAI_OBJ *pObj)		{	return	CD3DUtil::distance(m_PosCUR, ((CObjCHAR*)pObj)->m_PosCUR);	}	
	/*override*/void			Special_ATTACK ();
	/*override*/void			Set_EMOTION(short nEmotionIDX);

	
	/// <
	/// < End
	//////////////////////////////////////////////////////////////////////////////////////////







	/// Clean all internal list
	void							ClearAllEntityList();



	/////////////////////////////////////////////////////////////////////////////////
	/// Given to the effect that from the outside.
	classDLLIST< CEffect* >				m_ExternalEffectLIST;

	classDLLIST< CEffect* >				m_WeatherEffectLIST;
public:
	void								AddExternalEffect( CEffect* pEffect );
	void								DeleteExternalEffect( CEffect* pEffect );

	void								ClearExternalEffect();


	/// Weather
	void								AddWeatherEffect( CEffect* pEffect );
	void								DeleteWeatherEffect();

	/////////////////////////////////////////////////////////////////////////////////



	/////////////////////////////////////////////////////////////////////////////////
	/// Skill result effect Registration list
private:

	struct stEFFECT_OF_SKILL
	{
		DWORD				m_dwCreateTime;
		int					iSkillIDX;
		bool				bDamageOfSkill;
		int					iCasterINT;						/// The user's INT calculations required

		gsv_DAMAGE_OF_SKILL	EffectOfSkill;					/// damage_of_skill This effect_of_skill Been inherited, so shift to
	};

	std::vector< stEFFECT_OF_SKILL >	m_EffectedSkillList;	
	int									m_iEffectedSkillIndex;
	bool								m_bProcEffectedSkill;
	bool								m_bCanStartSkill;

public:	
	/*void								SetEffectedSkillIndex( int iEffectedSkillIdx )
	{ 
		m_iEffectedSkillIndex = iEffectedSkillIdx; 
		m_bProcEffectedSkill = true;
	}*/
	void								PushEffectedSkillToList( int iSkillIDX, gsv_DAMAGE_OF_SKILL EffectedSkill, int iCasterINT, bool bDamageOfSkill = false );
	void								ProcTimeOutEffectedSkill();
	bool								ProcEffectedSkill();	
	void								ProcOneEffectedSkill( stEFFECT_OF_SKILL*	pEffectOfSkill );

	void								ProcEffectOfSkillInDamageOfSkill( int iSkillIDX, int iObjIDX, CObjCHAR* pChar, stEFFECT_OF_SKILL*	pEffectOfSkill );
	void								ApplyEffectOfSkill( int iSkillIDX, int iObjIDX, CObjCHAR* pChar, stEFFECT_OF_SKILL*	pEffectOfSkill );
	/////////////////////////////////////////////////////////////////////////////////




	/////////////////////////////////////////////////////////////////////////////////
	/// Damage Registration list
private:
	struct stAPPLY_DAMAGE
	{
		DWORD			m_dwCreateTime;
		int				m_iAttacker;
		uniDAMAGE		m_Damage;
	};

	std::vector< stAPPLY_DAMAGE >		m_DamageList;

public:
	void								PushDamageToList( int iAttacker, uniDAMAGE wDamage );


	uniDAMAGE							PopCurrentAttackerDamage( int iAttacker );
	uniDAMAGE							PopTotalDamageFromList( int& iMaxDamage );	

	/// All Daisy processing (dead or something like that situation .. clean ..)
	void								ClearAllDamage();

	/// Timeout processing
	void								ProcDamageTimeOut();

	/// Tagyeokchi appear immediately
	void								CreateImmediateDigitEffect( DWORD wDamage  );

	/// Damage_of_Skill => Switch to save the normal damage.
	void								ConvertDamageOfSkillToDamage( gsv_DAMAGE_OF_SKILL stDamageOfSkill );
	/////////////////////////////////////////////////////////////////////////////////



	/////////////////////////////////////////////////////////////////////////////////
	/// Registration list field item drops (If you do not know to drop several at a time as a vector)
private:
	std::vector< tag_DROPITEM >			m_FieldItemList;

public:
	void								PushFieldItemToList( tag_DROPITEM& wDamage ){ m_FieldItemList.push_back( wDamage ); }
	void								DropFieldItemFromList( );	
	/////////////////////////////////////////////////////////////////////////////////



	/////////////////////////////////////////////////////////////////////////////////
	/// Persistence Entity Registration
public:
	CEndurancePack						m_EndurancePack;				/// Please use the initialization list.

public:	
	bool								AddEnduranceEntity( int iEntityIDX, int iStatusSTBNO, int iEnduranceTime, int iEntityType )
	{
		return m_EndurancePack.AddEntity( iEntityIDX, iStatusSTBNO, iEnduranceTime, iEntityType );
	}

	int									GetEnduranceEntityCount(){ return m_EndurancePack.GetEntityCount(); }
	int									GetEnduranceStateSTBNO( int Index )
	{
		CEnduranceProperty* pEntity = m_EndurancePack.GetEntity( Index );
		if( pEntity == NULL )
			return 0;

		return pEntity->GetStateSTBNO();
	}		
	
	/// Termination of the glass state, called the state revocation.
	int									ProcFlushStateSkill( int iStateTableIndex ){ return m_EndurancePack.ProcFlushStateSkill( iStateTableIndex ); }
	/////////////////////////////////////////////////////////////////////////////////




	/////////////////////////////////////////////////////////////////////////////////
	/// Summon monsters related
	/// Save to put the action frame to fit.
private:
	gsv_MOB_CHAR						m_SummonMob;
	bool								m_bHaveSummonedMob;				/// Should be summoned monsters are there?

public:
	void								SetSummonMobInfo( gsv_MOB_CHAR& MobInfo );
	void								SummonMob();

	/////////////////////////////////////////////////////////////////////////////////




	//////////////////////////////////////////////////////////////////////////////////////////////////
	/// queuing the command 
	//////////////////////////////////////////////////////////////////////////////////////////////////
	

	CObjCommandManager			m_CommandQueue;

	/// Applicable to the current command do?
	/*override*/virtual bool	CanApplyCommand();
	void						ProcQueuedCommand();

	void						ClearAllCommand(){ m_CommandQueue.ClearCommand(); }


	/*override*/virtual void	PushCommandSit();
	/*override*/virtual void	PushCommandStand();
	/*override*/virtual void	PushCommandStop();
	/*override*/virtual void	PushCommandMove( const D3DVECTOR& PosTo, BYTE btRunMODE );
	/*override*/virtual void	PushCommandMove( WORD wSrvDIST, const D3DVECTOR& PosTO, int iServerTarget );
	/*override*/virtual void	PushCommandAttack( int iServerTarget, WORD wSrvDIST, const D3DVECTOR &PosTO );
	/*override*/virtual void	PushCommandDie();
	/*override*/virtual void	PushCommandToggle( BYTE btTYPE );
	/*override*/virtual void	PushCommandSkill2Self( short nSkillIDX );
	/*override*/virtual void	PushCommandSkill2Obj( WORD wSrvDIST, const D3DVECTOR& PosTO, int iServerTarget, short nSkillIDX );
	/*override*/virtual void	PushCommandSkill2Pos( const D3DVECTOR &PosGOTO, short nSkillIDX );


	//////////////////////////////////////////////////////////////////////////////////////////////////





	//////////////////////////////////////////////////////////////////////////////////////////////////
	/// Object Vibration
	//////////////////////////////////////////////////////////////////////////////////////////////////
public:
	CObjvibration				m_ObjVibration;

public:
	void						StartVibration();

	//////////////////////////////////////////////////////////////////////////////////////////////////





#ifdef	__VIRTUAL_SERVER
	CRegenPOINT					*m_pRegenPOINT;
	classDLLNODE< CObjCHAR* >	*m_pRegenListNODE;
#endif
	
	
	//////////////////////////////////////////////////////////////////////////////////////////	
	/// > CObjCHAR virtual functions
	/// >

	// default npc skeleton
	virtual HNODE				Get_SKELETON ()	{	return m_pCharMODEL->GetZSKEL ();	}

		
	virtual bool				Check_EVENT (CGameOBJ *pSourOBJ, short nEventIDX=-1)		{	return false;	}

	virtual bool				IsFemale ()		=0;
	virtual short				IsMagicDAMAGE()	=0;

	virtual tagMOTION*			Get_MOTION (short nActionIdx)=0{ *(int*)0 = 10; };

	virtual int					Add_HP (int iAdd);
	virtual int					Add_MP (int iAdd)				{	return (m_iMP+=iAdd);	}

	virtual int					Sub_HP (int iSub);
	virtual int					Sub_MP (int iSub)				{	return (m_iMP-=iSub);	}



	virtual void				Add_EXP (short nExp)=0{ *(int*)0 = 10; };	
	virtual short				GetOri_WalkSPEED ()=0{ *(int*)0 = 10; };
	virtual short				GetOri_ATKSPEED()=0{ *(int*)0 = 10; };
	virtual short				GetOri_RunSPEED()=0{ *(int*)0 = 10; };


	/// The current consumption of the avatar you are affected by a bullet.
	virtual int					Get_BulletNO ();

	/// Go to a specific coordinate the height correction. The height of the objects are placed on the highest point coordinates
	virtual void				DropFromSky ( float fX, float fY );

	virtual int					Get_INT(){ return 0; }						/// !!!Cal CObjCHAR members of the class is such that only this approach.


	/// Apply the changes persisted to the current applied to the numerical ability levels (including passive skills)
	virtual int					Get_DefaultAbilityValue( int iType ){ return 1; };



	///
	/// Even if the turning state of the loop according to the state of this flag is reset motion (STOP, MOVE ...)
	///
	virtual void				SetUpdateMotionFlag( bool bUpdate ){}
	virtual bool				GetUpdateMotionFlag(){return false;} 


	///
	/// Did someone link me?
	///
	virtual bool				IsChild(){ return false; }


	//----------------------------------------------------------------------------------------------------	
	/// @brief Additional information for each process for each object type can have a little bit.
	//----------------------------------------------------------------------------------------------------
	virtual int					Proc (void);


	//----------------------------------------------------------------------------------------------------	
	/// @brief Effect on the character tags.
	//----------------------------------------------------------------------------------------------------
	virtual void				ShowEffectOnCharByIndex( int iEffectIDX, int iSoundIDX = 0, bool bWeatherEffect = false );
	virtual void				ShowEffectOnCharByHash( int iHash, int iSoundIDX = 0, bool bWeatherEffect = false ); 


	/// >
	/// > End
	//////////////////////////////////////////////////////////////////////////////////////////

	

	/// Look to see where the target rotation model.
	void						Add_ModelDIR( float fAngleDegree );
	
	D3DXVECTOR3					Get_GotoPOSITION ()					{	return m_PosGOTO;	}
	D3DXVECTOR3					Get_BornPOSITION ()					{	return m_PosBORN;	}

	float						Get_DISTANCE (t_POSITION PosBASE)	{	return	CD3DUtil::distance(m_PosCUR, PosBASE);						}

	

public:
	/// not overrided function
	int							Get_RWeapon(){ return Get_R_WEAPON(); }	
	int							Get_LWeapon(){ return Get_L_WEAPON(); }	

	CObjCHAR ();
	virtual ~CObjCHAR ();

	HNODE						GetZMODEL()									{	return m_hNodeMODEL;	}
	CMODEL<CCharPART> *			GetPartMODEL(short nPartIDX)				{	return m_pCharMODEL->GetCharPART( nPartIDX );	}
	int							Get_CharNO ()								{	return	m_nCharIdx;				}	

	bool						LinkDummy(HNODE hLinkNODE, short nDummyIDX);
	bool						Link2LastDummy(HNODE hLinkNODE);
	int							GetLastDummyIDX()							{	return m_iLastDummyIDX;	}

	

	bool						IsIntersect (HNODE hModel)					{	return ( ::intersectNode( m_hNodeMODEL, hModel)!=0 );		}



	
	void						SetSCALE (float fScale)						{	m_fScale=fScale, ::setScale(m_hNodeMODEL, fScale, fScale, fScale);					}
	float						Get_SCALE(){ return m_fScale; }

	void						Rotate (D3DXQUATERNION &Rotate)				{	::setQuaternion( m_hNodeMODEL, Rotate.w, Rotate.x, Rotate.y, Rotate.z );			}
	/// NPC of the STB is in the high force that the height is used.
	virtual void				GetScreenPOS ( D3DVECTOR &PosSCR);

	bool						LinkEffectToPOINT( CEffect *pEffect, short nPartIDX, short nPointIDX);
	bool						LinkEffectToDUMMY( CEffect *pEffect, short nDummyIDX );

	void						Add_EFFECT(short nPartIDX, short nPointIDX, t_HASHKEY HashEffectFILE );
	void						Del_EFFECT(short nPartIDX);

	
	void						Warp_POSITION (float fX, float fY);

		
	void						Dead ();
	bool						ProcessSkillHit( CObjCHAR *pFromOBJ );
	bool						Hitted (CObjCHAR *pFromOBJ, int iEffectIDX, int iSkillIDX = 0, bool bIsSkillEffect = false, bool bIsBulletHit = false, bool bJustEffect = false );
	
	
	void						Set_UserMOITON ( t_HASHKEY HashMOTION, float fMoveSpeed, int iRepeatCnt = 0 );
	void						Set_UserMOITON (tagMOTION *pMotion, float fMoveSpeed, int iRepeatCnt = 0 );


	void						SetCollisionGroundNode ( HNODE hNodeGroundIn ) { m_hNodeGround = hNodeGroundIn; }
	HNODE						GetCollisionGroundNode () { return m_hNodeGround; } // Located at the bottom of the object returns the handle to the engine. Or branched, and water are excluded. 0 if not.

public:
	tagDAMAGE					m_DeadDAMAGE;
	long						m_lDeadTIME;
	void						Apply_DAMAGE (CObjCHAR *pAtkOBJ, uniDAMAGE sDamage);	//Honggeun: Max Damage modification


	//----------------------------------------------------------------------------------------------------
	/// @brief When replacing equipment in the middle of a particular action (especially arms) Motion-sensitive changes in the equipment replacement
	//----------------------------------------------------------------------------------------------------
	void						UpdateMotionByChangingEquip();


	//----------------------------------------------------------------------------------------------------
	/// @brief Weapons when I paetil
	//----------------------------------------------------------------------------------------------------
	void						FireBulletWithPetWeapon( CObjCHAR* pTarget );


private:
	int							m_iLastCastingSkill;
public:
	void						SetLastCastingSkill( int iSkillIDX ){ m_iLastCastingSkill = iSkillIDX; }
	int							GetLastCastingSkill(){ return m_iLastCastingSkill; }



	//----------------------------------------------------------------------------------------------------
	/// @brief You can click on an object is?
	//----------------------------------------------------------------------------------------------------
	virtual bool				CanClickable(){ return true; }



private:
	WORD					m_dwClanID;
	WORD					m_wClanMarkBack;
	WORD					m_wClanMarkCenter;
	int						m_iClanLevel;
	std::string				m_strClanName;
	BYTE					m_btClanPos;

	CClanMarkUserDefined*	m_ClanMarkUserDefined;
public:
	void					SetClan( DWORD dwClanID, WORD wMarkBack, WORD wMarkCenter, const char* pszName , int iLevel , BYTE btClanPos );
	DWORD					GetClanID();
	WORD					GetClanMarkBack();
	WORD					GetClanMarkCenter();
	const char*				GetClanName();
	BYTE					GetClanPos();
	void					SetClanPos( BYTE btPos );
	void					ResetClan();
	void					SetClanLevel( int iLevel );
	int						GetClanLevel();
	void					SetClanMark( WORD wMarkBack, WORD wMarkCenter );

	CClanMarkUserDefined*	GetUserDefinedClanMark(){ return m_ClanMarkUserDefined; }
	void					SetUserDefinedClanMark( CClanMarkUserDefined* pUserDefinedClanMark );//{ m_ClanMarkUserDefined = pUserDefinedClanMark; }

	/// Server and the HP / MP difference for correcting variables / functions
protected:
	int						m_ReviseHP;
	int						m_ReviseMP;
	int						m_AruaAddMoveSpeed;		/// Al Oh Goddess movement speed increased, the state

public:
	void					SetReviseHP( int hp );
	void					SetReviseMP( int mp );

	
	void			SHOW()
	{	::setVisibilityRecursive(GetZMODEL(),TRUE); 	}
	void			HIDE()
	{	::setVisibilityRecursive(GetZMODEL(),FALSE); 	}
	
//-----------------------------------------------------------------------------------
	//Cho, Sung - Hyun
	 bool            m_bDisguise;


//------------------------------------------------------------------------------------	
	//Bakjiho
	//Variables cart ...
	CObjCART*			m_pObjCART;					//Cart keulraeseo 
	CObjCHAR*			m_pRideUser;				//2 seater player 

	int					m_iPetType;					//Pet type (21: cart) 
	short				m_skCartIDX;				//Cart skill effects (currently in use)
	WORD				m_iRideIDX;					//2 seater user index 
	
	float				m_saveSpeed;				//Spit stored variables 

	BOOL				m_bUseCartSkill;			//Cart skill use (not currently used)	
	BOOL				m_IsRideUser;				//2 seater Hvis boarding 

	//Oh Loa goddess status 
	BOOL				m_IsAroa;					//Oh Loa Hvis set 
	BOOL				m_IsCartVA;					//Cart vibe ration (shake)

	//Oh Oh, and cart with skill effects. 
	void ChangeHittedEffect(CObjCHAR* pObj,BOOL bA,int& hitIDX);
	//After using the skill set the behavior of the cart. 
	void SetNewCommandAfterSkill_PET(int iSkillNO);
	//The cart stops. 
	void Stop_Cart(void);
	//Denied boarding, setting in motion. 
	void Ride_Cansel_Motion(void);
	
	//2 seater itself. 
	void ReleaseRideUser(void);
	//The guests themselves off the cart. 
	void ReleaseRideUser(WORD irideIDX);
	//Cart since avatar is always a skill. 
	void Set_SitMode(void);		

	//Cart skill. 
	int  ProcCMD_Skill2OBJECT_PET();
	
	//2-seater. 
	BOOL SetRideUser(WORD irideIDX=0);

	//Cart skill index. 
	short&		GetCartSKIDX(void)	
	{	return	m_skCartIDX;							}
	
	//Do not use the 
	BOOL&		GetUseCartSkill(void)	
	{	return	m_bUseCartSkill;						}

	//I did check your opponent's Board to cart (TRUE: boarding, FALSE:)
	BOOL&		IsRideUser(void)
	{	return m_IsRideUser;							}

	//I have checked upon boarding the other side of the cart (0:, number: boarding)
	WORD		GetRideUserIndex(void)
	{	return m_iRideIDX;								}

	//Oh goddess as set state function 
	BOOL&			SetAroaState(void)
	{	return m_IsAroa;								}
	//Cart migration using a vibe (not currently used)
	BOOL&			SetCartVA(void)
	{	return m_IsCartVA;								}
	//Save current speed (not currently used)  
	float& SaveSpeed(void)
	{	return m_saveSpeed;								}
//------------------------------------------------------------------------------------

	/// If the final is the addition of the goddess Arua: stats calculation function
	virtual		void Calc_AruaAddAbility();
} ;




///
///	MOB class inherited from COjbCHAR
///

class CObjMOB : public CObjCHAR
{
private:
	short						m_nQuestIDX;
	int							m_iMobAniSkill;

public :
	static DWORD				m_dwCreateOrder;
	
	CObjMOB();
	virtual ~CObjMOB ();



	//////////////////////////////////////////////////////////////////////////////////////////	
	/// < Inherited from CGameObj virtual functions
	/// <

	/*override*/virtual int		Get_TYPE()							{	return OBJ_MOB;					}
	/*override*/char*			Get_NAME ()							{	return (char*)NPC_NAME( m_nCharIdx );	}

	/// <
	/// < End
	//////////////////////////////////////////////////////////////////////////////////////////
	
	
	//////////////////////////////////////////////////////////////////////////////////////////	
	/// < Inherited from CAI_OBJ virtual functions
	/// <

	/*override*/virtual void		Say_MESSAGE (char *szMessage);
	/*override*/virtual bool		Change_CHAR (int nCharIDX);
	/*override*/virtual bool		Create_PET  (int nCharIDX);
	/*override*/virtual void		Run_AWAY (int iDistance);

	/*override*/virtual int			Get_LEVEL ()						{	return NPC_LEVEL( m_nCharIdx );				}
	/*override*/virtual int			Get_ATK ()							{	return NPC_ATK  ( m_nCharIdx );				}
	/*override*/virtual int			Get_DEF ()							{	return NPC_DEF	( m_nCharIdx );				}
	/*override*/virtual int			Get_RES ()							{	return NPC_RES	( m_nCharIdx );				}
	/*override*/virtual int			Get_HIT ()							{	return NPC_HIT	( m_nCharIdx );				}	
	/*override*/virtual int			Get_CHARM ()						{	return 0;		}
	/*override*/virtual int			Get_AVOID ()						{	return NPC_AVOID( m_nCharIdx );				}
	/*override*/virtual int			Get_SENSE ()						{	return Get_LEVEL();							}	// Replaced with LEVEL!
	/*override*/virtual int			Get_GiveEXP ()						{	return NPC_GIVE_EXP( m_nCharIdx );			}
	/*override*/virtual void		Add_EXP (short nExp)				{	;	/* nop */	}
	/*override*/virtual int			Get_CRITICAL()						{	return (int)(this->Get_LEVEL() * 0.8 );		}
	
	virtual int						Get_AbilityValue_StatItemPas( WORD nType );//Base stats + items + passive
	/// <
	/// < End
	//////////////////////////////////////////////////////////////////////////////////////////
	

	//////////////////////////////////////////////////////////////////////////////////////////	
	/// < Inherited from CObjAI virtual functions
	/// <

	/*override*/int				Def_AttackRange()					{	return NPC_ATK_RANGE( m_nCharIdx );			}

	/*override*/int				GetANI_Stop()						{	return MOB_ANI_STOP;	}
	/*override*/int				GetANI_Move()						{	return ( m_bRunMODE ) ? MOB_ANI_RUN : MOB_ANI_MOVE;	}
	/*override*/int				GetANI_Attack()						{	return MOB_ANI_ATTACK;	}
	/*override*/int				GetANI_Die ()
	{	
		if( m_bStopDead )
			return MOB_ANI_STOP;
		return  MOB_ANI_DIE;				
	}
	/*override*/int				GetANI_Hit()						{	return MOB_ANI_HIT;		}
	/*override*/int				GetANI_Casting()					{   return m_iMobAniSkill;	}		/// Casting operation
	/*override*/int				GetANI_CastingRepeat()				{   return m_iMobAniSkill;	}		/// The behavior of the loop used by the behavior of the cast ...
	/*override*/int				GetANI_Skill()						{	return m_iMobAniSkill + 1;	}

	/*override*/int				Get_MP ()							{	return 32767;			}
	/*override*/int				Get_R_WEAPON ()						{	return NPC_R_WEAPON( m_nCharIdx );			}
	/*override*/int				Get_L_WEAPON ()						{	return 0;				}
	/*override*/ virtual void	RecoverHPnMP( short nRecoverMODE )	{};	


	/// 최대 생명력	
	void						Set_MaxHP( int iMaxHP )				{ m_iMaxHP = iMaxHP; }
	/*override*/int				Get_MaxHP()							{	return m_iMaxHP;			}
	/// 최대 마나
	/*override*/int				Get_MaxMP()							{	return 100;								}


	/*override*/int				Get_AttackRange ();


	/*override*/void			Do_StopAI (void)					{	g_AI_LIST.AI_Stop( NPC_AI_TYPE(m_nCharIdx), this );							}
	/*override*/void			Do_AttackMoveAI (CObjCHAR *pTarget)	{	g_AI_LIST.AI_AttackMove( NPC_AI_TYPE(m_nCharIdx), this, pTarget );			}
	/*override*/void			Do_DamagedAI (CObjCHAR *pTarget, int iDamage )	
	{	
		if ( !( CMD_BIT_INT & this->Get_COMMAND() ) )
			g_AI_LIST.AI_Damaged( NPC_AI_TYPE(m_nCharIdx), this, pTarget, iDamage );
	}
	/*override*/void			Do_KillAI (CObjCHAR *pTarget, int iDamage)		{	g_AI_LIST.AI_Kill( NPC_AI_TYPE( m_nCharIdx ), this, pTarget, iDamage );		}
	/*override*/void			Do_DeadAI (CObjCHAR *pTarget, int iDamage)		{	g_AI_LIST.AI_Dead( NPC_AI_TYPE( m_nCharIdx ), this, pTarget, iDamage );		}

	/*override*/virtual void	Do_DeadEvent	(CObjCHAR *pAtk);	


	/// <
	/// < End
	//////////////////////////////////////////////////////////////////////////////////////////




	//////////////////////////////////////////////////////////////////////////////////////////	
	/// < Inherited from CObjCHAR virtual functions
	/// <

	/*override*/bool			Check_EVENT (CGameOBJ *pSourOBJ, short nEventIDX);

	/*override*/ bool			IsFemale ()			{	return false;	}
	/*override*/ short			IsMagicDAMAGE()		{	return NPC_IS_MAGIC_DAMAGE( m_nCharIdx );	}

	/*override*/tagMOTION *		Get_MOTION (short nActionIdx=MOB_ANI_STOP)	{	return m_pCharMODEL->GetMOTION( nActionIdx );	}

	

	/*override*/ short			GetOri_WalkSPEED()					{	return NPC_WALK_SPEED( m_nCharIdx );		}
	/*override*/ short			GetOri_RunSPEED()					{	return NPC_RUN_SPEED( m_nCharIdx );			}
	/*override*/ short			GetOri_ATKSPEED ()					{	return NPC_ATK_SPEED( m_nCharIdx );			}


	/// In order to apply a durable change of the figures currently in effect is the ability to figure (passive skill)
	virtual int					Get_DefaultAbilityValue( int iType );

	/// In the case of NPC go that height is the height of force in the STB.
	virtual void				GetScreenPOS ( D3DVECTOR &PosSCR);

	/// <
	/// < End
	//////////////////////////////////////////////////////////////////////////////////////////



	bool	Create (short nCharIdx, const D3DVECTOR& Position, short nQuestIDX, bool bRunMODE);

	void	SetMobAniSkill( int iMobAniSkill = MOB_ANI_CASTION01 ){ m_iMobAniSkill = iMobAniSkill; }


	//------------------------------------------------------------------------------------------------
	/// Monster enables sound output ...
	//------------------------------------------------------------------------------------------------
	void	PlayStopSound();


	virtual int	Proc();
	
} ;



///
///	Npc class inherited from COjbMOB
///

class CObjNPC : public CObjMOB
{
private:
	int				m_iEventSTATUS;

public :

	//////////////////////////////////////////////////////////////////////////////////////////	
	/// < Inherited from CGameObj virtual functions
	/// <

	CObjNPC();

	virtual int		Get_TYPE()							{	return OBJ_NPC;	}
	virtual int		Proc();
	/// <
	/// < end	
	//////////////////////////////////////////////////////////////////////////////////////////	


	void			SetEventValue( int iEventValue );
	int				GetEventValue();


	//-------------------------------------------------------------------------------------------------
	// 05.05.19 icarus:: Wow-style NPC quest exposure ...
public :
	// Add the packet to the default value when creating a server under the npc-1, 
	short			m_nQuestSignal;
//	int				Proc (void);

} ;



class CObjCART;
///
///	Avata class
///

class CObjAVT : public CObjCHAR
{
protected:
	CCharMODEL		m_CharMODEL;

	/// Character expression
	int				m_iExpression;

	/// 캐릭터 파츠
	union {
		tagPartITEM	m_sPartItemIDX[ MAX_BODY_PART ];
		// short	m_nPartItemIDX[ MAX_BODY_PART ];
		struct {
			// Depending on the order of t_CharPART ...
			tagPartITEM m_sFaceIDX;			//short m_nFaceIDX;
			tagPartITEM m_sHairIDX;			//short m_nHairIDX;
			tagPartITEM m_sHelmetIDX;		//short m_nHelmetIDX;
			
			tagPartITEM m_sArmorIDX;		//short m_nArmorIDX;
			tagPartITEM m_sGauntletIDX;		//short m_nGauntletIDX;
			tagPartITEM m_sBootsIDX;		//short m_nBootsIDX;
			
			tagPartITEM m_sGoggleIDX;		//short m_nGoggleIDX;
			tagPartITEM m_sKnapsackIDX;		//short m_nKnapsackIDX;
			
			tagPartITEM m_sRWeaponIDX;		//short m_nRWeaponIDX;
			tagPartITEM m_sLWeaponIDX;		//short m_nLWeaponIDX;
		} ;
	} ;

		
protected:
	std::string				m_Name;
	int						m_iLevel;


	DWORD					m_dwSitTIME;
	short					m_nPsvAtkSPEED;

	short					m_nRunSPEED;
	short					m_nAtkAniSPEED;


	/// If the weapon is interchangeable settings. .. The motion is changing after ...
	int						m_iDoChangeWeaponR;
	int						m_iDoChangeWeaponL;
	bool					m_bUpdateMotion;	

	/// The bullet data that is currently in use ...
	tagShotDATA				m_ShotData[MAX_SHOT_TYPE];
	int						m_iCon;
	int						m_iRecoverHP;
	int						m_iRecoverMP;

	///Possession of the item weight ratio: 
	BYTE					m_btWeightRate;

	/// In Mina			
	short					m_nStamina;

	

public:
	CObjAVT ();
	virtual ~CObjAVT ();


	void					SetShotData( int i, int iItemNo );

	
	void					Set_CON( int iCon ){	m_iCon = iCon; }
	void					Set_AddRecoverHP( int iRecoverHP ){ m_iRecoverHP = iRecoverHP; }
	void					Set_AddRecoverMP( int iRecoverMP ){ m_iRecoverMP = iRecoverMP; }

public :
	
	/// Character expression
	void					SetCharExpression( int iFaceNo ){ m_iExpression = iFaceNo; }
	int						GetCharExpression(){ return m_iExpression; }

	/// If the weapon is interchangeable settings. .. The motion is changing after ...
	void					SetChangeWeaponR( int iWeaponNO ){ m_iDoChangeWeaponR = iWeaponNO;}
	void					SetChangeWeaponL( int iWeaponNO ){ m_iDoChangeWeaponL = iWeaponNO;}
	int						GetChangeWeaponR( ){ return m_iDoChangeWeaponR;}
	int						GetChangeWeaponL( ){ return m_iDoChangeWeaponL;}


	/// Equipment replacement equipment hanging on the property off. ..
	void					ClearRWeaponSkillEffect();
	void					ClearLWeaponSkillEffect();


	///
	/// Turn the loop depending on the status of this flag, even if the State of motion reset (STOP, MOVE ...)
	///
	void						SetUpdateMotionFlag( bool bUpdate ){ m_bUpdateMotion = bUpdate; }
	bool						GetUpdateMotionFlag(){ return m_bUpdateMotion; } 






	/// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
	/// > virtual function of CObjAVT
	/// >

	virtual void				Update_SPEED ()			//PY: I think this is just for animation purposes
	{	
		m_nAtkAniSPEED = Cal_AtkAniSPEED( this->GetPartITEM(BODY_PART_WEAPON_R) );
		m_fRunAniSPEED = Cal_RunAniSPEED( m_nRunSPEED );
	}
	/// Virtual function
	virtual void				Update (bool bUpdateBONE=false);

	virtual void				Set_SEX ( BYTE btRace )		
	{	
		m_btRace = btRace;
		m_bIsFemale = btRace & 0x01;
		m_CharMODEL.Set_SEX( m_bIsFemale );	
	}

	/// >
	/// > end	
	/// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>







	//////////////////////////////////////////////////////////////////////////////////////////	
	/// < Inherited from CGameOBJ virtual functions
	/// <

	/*override*/virtual int	Get_TYPE()					{	return OBJ_AVATAR;				}
	char*					Get_NAME ()					{	return (char*)m_Name.c_str();				}

	/// <
	/// < end	
	//////////////////////////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////////////////////////////	
	/// < Inherited from CAI_OBJ virtual functions
	/// <

	/*override*/virtual bool	Is_SameTYPE(int ObjTYPE)	{	return ( ObjTYPE==OBJ_AVATAR ) || ( ObjTYPE==OBJ_USER );	}

	/*override*/virtual int		Get_LEVEL ()						{	return m_iLevel;}
	/*override*/virtual int		Get_ATK ()							{	return 1;		}
	/*override*/virtual int		Get_DEF ()							{	return 1;		}
	/*override*/virtual int		Get_RES ()							{	return 1;		}		
	/*override*/virtual int		Get_HIT ()							{	return 1;		}
	/*override*/virtual int		Get_CHARM ()						{	return 1;		}
	/*override*/virtual int		Get_AVOID ()						{	return 1;		}
	/*override*/virtual int		Get_SENSE ()						{	return 1;		}			
	/*override*/virtual int		Get_GiveEXP ()						{	return 1;		}
	/*override*/virtual void	Add_EXP (short nExp)				{	;	/* nop */	}
	/*override*/virtual int		Get_CRITICAL()						{	return 1;		}
	/// <
	/// < end	
	//////////////////////////////////////////////////////////////////////////////////////////




	//////////////////////////////////////////////////////////////////////////////////////////	
	/// < Inherited from CObjAI virtual functions
	/// <

	/*override*/virtual int		Get_CON()				{	return m_iCon;	}	// If you are a user, return pAVATAR->GetCur_CON();
	/*override*/virtual int		GetAdd_RecoverHP()		{	return m_iRecoverHP;	}	// If you are a user, return pAVATAR->m_btRecoverHP;
	/*override*/virtual int		GetAdd_RecoverMP()		{	return m_iRecoverMP;	}	// If you are a user, return pAVATAR->m_btRecoverHP;

	/*override*/virtual short	GetPsv_ATKSPEED(float fCurSpeed, short nRightWeaponItemNo)		{	return m_nPsvAtkSPEED;				}
	/*override*/int				Def_AttackRange()		{	return WEAPON_ATTACK_RANGE( 0 );							}


	/*override*/int				GetANI_Stop();				
	/*override*/int				GetANI_Move();
	/*override*/int				GetANI_Attack();
	/*override*/int				GetANI_Die();			
	/*override*/int				GetANI_Hit();
	/*override*/int				GetANI_Casting ();
	/*override*/int				GetANI_CastingRepeat();
	/*override*/int				GetANI_Skill();
	/*override*/int				GetANI_Sitting();
	/*override*/int				GetANI_Standing();
	/*override*/int				GetANI_Sit();
	/*override*/int				GetANI_Ride();
	/*override*/int				GetANI_PickITEM();


//	/*override*/int				Get_R_WEAPON ()			{	return this->m_nRWeaponIDX;				}
//	/*override*/int				Get_L_WEAPON ()			{	return this->m_nLWeaponIDX;				}
	/*override*/int				Get_R_WEAPON ()			{	return this->m_sRWeaponIDX.m_nItemNo;	}
	/*override*/int				Get_L_WEAPON ()			{	return this->m_sLWeaponIDX.m_nItemNo;	}

	/*override*/int				Get_MaxHP()
	{
		//return m_iMaxHP + m_EndurancePack.GetStateValue( ING_INC_MAX_HP );	//PY: let's remove the added stuff from this so that only the value sent from the server matters
		return m_iMaxHP;
	}		
	/*override*/int				Get_MaxMP()				
	{
		//return m_iMaxMP + m_EndurancePack.GetStateValue( ING_INC_MAX_MP );	//PY: let's remove the added stuff from this so that only the value sent from the server matters
		return m_iMaxMP;
	}

	virtual int					Get_AbilityValue_StatItemPas( WORD nType );//Base stats + items + passive

	/*override*/bool			ToggleRunMODE ( float fAdjRate );
	/*override*/bool			ToggleSitMODE ();

	

	///
	/// Check PET MODE
	/// if mode < 0, this char is normal mode
	/*override*/ virtual int	GetPetMode();
	/*override*/ virtual bool	CanAttackPetMode();
	/*override*/ virtual void	SetCMD_PET_MOVE( const D3DVECTOR& PosTO, BYTE btRunMODE );	
	/*override*/ virtual void	SetCMD_PET_MOVE( WORD wSrvDIST, const D3DVECTOR& PosTO, int iServerTarget);
	/*override*/ virtual void	SetCMD_PET_STOP( void );
	/*override*/ virtual void	SetCMD_PET_ATTACK (int iServerTarget, WORD wSrvDIST, const D3DVECTOR& PosTO);

	/*override*/ virtual void	RideCartToggle( bool bRide );		

	/// <
	/// < end	
	//////////////////////////////////////////////////////////////////////////////////////////



	//////////////////////////////////////////////////////////////////////////////////////////	
	/// < Inherited from CObjCHAR virtual functions
	/// <

private:
	/*override*/HNODE			Get_SKELETON ()		{	return g_DATA.Get_SKELETON( this->m_bIsFemale );	}

public:
	/*override*/bool			IsFemale ()				{	return m_bIsFemale;					}
	/*override*/short			IsMagicDAMAGE()			{	return WEAPON_IS_MAGIC_DAMAGE( Get_R_WEAPON() );			}

	/*override*/tagMOTION*		Get_MOTION (short nActionIdx=AVT_ANI_STOP1);




	/*override*/short			GetOri_ATKSPEED()					{	return m_nAtkAniSPEED;	}
	/*override*/short			GetOri_WalkSPEED()					{	return WALK_CmPerSec;			}
	
	/*override*/short			GetOri_RunSPEED()
	{
		return m_nRunSPEED;				
	}

	virtual		int				GetOri_MaxHP();

	/*override*/ virtual int	Get_BulletNO ();

	
	/// In order to apply a durable change of the figures currently in effect is the ability to figure (passive skill)
	/*override*/ virtual int	Get_DefaultAbilityValue( int iType );

	///
	/// I have a link to someone?
	///
	/*override*/ virtual bool	IsChild(){ return ( ( m_pObjCART == NULL )? false:true ); }

	//----------------------------------------------------------------------------------------------------
	/// CObjCHAR Do some additional work to do.
	//----------------------------------------------------------------------------------------------------
	/*override*/int							Proc ();
		
	/// <
	/// < end	
	//////////////////////////////////////////////////////////////////////////////////////////



	
	void						SetOri_RunSPEED(short nRunSpeed)	
	{
		m_nRunSPEED = nRunSpeed;		
	}
	void						SetPsv_AtkSPEED(short nPsvAtkSpeed)	{	m_nPsvAtkSPEED = nPsvAtkSpeed;	}
	
	
	/// Max vitality	
	void						Set_MaxHP( int iMaxHP )			{ m_iMaxHP = iMaxHP; }
	//added by PY
	void						Set_MaxMP( int iMaxMP )			{ m_iMaxMP = iMaxMP; }
	
	

	

	short						GetPartITEM(short nPartIdx)						{	return m_sPartItemIDX[ nPartIdx ].m_nItemNo;	}
	void						SetPartITEM(short nPartIdx, tagPartITEM &sITEM)	{	m_sPartItemIDX[ nPartIdx ] = sITEM;				}
	void						SetPartITEM(short nPartIdx, tagITEM &sITEM)		
	{	
		tagPartITEM sPartITEM;
		sPartITEM.m_nItemNo = sITEM.GetItemNO();
		sPartITEM.m_bHasSocket = sITEM.m_bHasSocket;
		sPartITEM.m_cGrade = sITEM.GetGrade();
		sPartITEM.m_nGEM_OP = sITEM.GetGemNO();
		m_sPartItemIDX[ nPartIdx ] = sPartITEM;
	}
	void						SetPartITEM(short nPartIdx, short nItemNo);
	void						SetAvataName (char *szName)						{	m_Name = szName;							}
	void						SetAllPARTS (tagPartITEM *pPartItemIDX)			{	::CopyMemory( m_sPartItemIDX, pPartItemIDX, sizeof(tagPartITEM)*MAX_BODY_PART);			}
	void						SetAllPetPARTS (tagPartITEM *pPetPartItemIDX)	{	::CopyMemory( m_sPetPartItemIDX, pPetPartItemIDX, sizeof(tagPartITEM)*MAX_RIDING_PART);	}

	void						SetAvtLevel( int iLevel ){ m_iLevel = iLevel; }

	bool						Create (const D3DVECTOR& Position, BYTE btCharRACE);	


	//----------------------------------------------------------------------------------------------------		
	/// @brief Ash, ash Lotus related effects. ..
	//----------------------------------------------------------------------------------------------------
	void						CreateGemmingEffect();
	void						DeleteGemmingEffect();


//--------------------------------------------------------------------------------------------------------
	//2005. 06.15 박 지호 
	//In the case of rare items, step 3 glow effect. 
	void						Set_RareITEM_Glow(void);
	//When cart John warp drive.  
	void						Process_JOIN_RIDEUSER(void);
//--------------------------------------------------------------------------------------------------------

	void						CreateGradeEffect();
	void						DeleteGreadEffect();




	//----------------------------------------------------------------------------------------------------	
	/// The current coordinate (m_PosCUR.x, m_PosCUR.y) returns the height of the
	/// @PReachable location is reached. It is possible to reach from the location of the previous frame, false if the value is set.
	/// @Return height value. It is impossible to reach the position, the highest returns the position of the object.
	//----------------------------------------------------------------------------------------------------
	float						GetHeight (bool * pReachable = NULL);



	//////////////////////////////////////////////////////////////////////////////////////////
	///
	/// Private shop-related data
	///
	//////////////////////////////////////////////////////////////////////////////////////////
private:

	std::string					m_strPersonalStoreTitle;

public:
	void						SetPersonalStoreTitle( char* strTitle, int iPersonalStoreType = 0 );
	const char*					GetPersonalStoreTitle();
	bool						IsPersonalStoreMode();

	CObjFIXED*					m_pObjPersonalStore;
	bool						m_bPersonalStoreMode;


	//////////////////////////////////////////////////////////////////////////////////////////
	///
	/// Pat system-related data. ..
	///
	//////////////////////////////////////////////////////////////////////////////////////////

public:

	friend CObjCART;
	/// Pat saitogurimusu. ..
	/// Of course.. CObjPET exists for this data are inside a. .. This is a convenience for management once.



//Park Ji-Ho:: saitogurimusu modify cart 
//It's an older version, a four-step version of the partition settings, your cart cart set up to step 5 of the skill 
//The attack in the streets of m_nWeaponIDX, non-treatment 
#if defined(_GBC)
	union 
	{
		//short				m_nPetPartItemIDX[ MAX_RIDING_PART ];
		tagPartITEM		m_sPetPartItemIDX[ MAX_RIDING_PART ];
		struct 
		{
			// The order is t_CharPART에 따라서...
			tagPartITEM	m_sEngineIDX;	// short			m_nEngineIDX;
			tagPartITEM	m_sBodyIDX;		// short			m_nBodyIDX;
			tagPartITEM	m_sLegIDX;		// short			m_nLegIDX;
			tagPartITEM	m_sAbilIDX;		// short			m_nAbilIDX;	
			tagPartITEM	m_sWeaponIDX;	// short			m_nWeaponIDX;	
		} ;
	} ;
#else 
	union 
	{
		//short				m_nPetPartItemIDX[ MAX_RIDING_PART ];
		tagPartITEM		m_sPetPartItemIDX[ MAX_RIDING_PART ];
		struct 
		{
			// Depending on the order of t_CharPART ...
			tagPartITEM	m_sEngineIDX;	// short			m_nEngineIDX;
			tagPartITEM	m_sBodyIDX;		// short			m_nBodyIDX;
			tagPartITEM	m_sLegIDX;		// short			m_nLegIDX;
			tagPartITEM	m_sAbilIDX;		// short			m_nAbilIDX;	
		} ;
	} ;
#endif 




//	int						m_iPetType;
//	CObjCART*				m_pObjCART;

	CEffect*				m_pWeaponJemEffect;

public:		
	void					SetPetType( int iPetType ){ m_iPetType = iPetType; }
	WORD					GetPetState();

	bool					CreateCartFromMyData( bool bShowEffect = false );
	bool					CreateCart( unsigned int iPetType, int iEnginePart, int iBodyPart, int iLegPart, int iAbilIPart,int iWeaponPart=0 );
	void					DeleteCart( bool bShowEffect = false );

	void					SetPetParts( unsigned int iPetPartIDX, unsigned int iItemIDX, bool bJustInfo = true );
	void					UpdatePet();
	
	void					SetWeightRate( BYTE btWeightRate );
	BYTE					GetWeightRate();
	//////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////

	//----------------------------------------------------------------------------------------------------	
	/// @brief When Pat returns the coordinates of the Pat mode, you typically return my coordinates
	//----------------------------------------------------------------------------------------------------
	const D3DVECTOR&		GetWorldPos();



	//----------------------------------------------------------------------------------------------------	
	/// @brief Besides the renderUnit of registered in the model for judging conflicts. .. (Avatar, the only)
	//----------------------------------------------------------------------------------------------------
	bool					IsIntersectAccessory( float &fCurDistance );


	virtual short			GetStamina();
	void					SetStamina( short nStamina );


	//--------------------------------------------------------------------------------------
	/// Avatar's special status
	//--------------------------------------------------------------------------------------
private:
	DWORD					m_dwSubFLAG;

public:
	DWORD					GetSpecialStateFLAG(){ return m_dwSubFLAG; }
	void					ChangeSpecialState( DWORD dwSubFLAG );

	//----------------------------------------------------------------------------------------------------
	/// @brief You can click an object?
	//----------------------------------------------------------------------------------------------------
	virtual bool			CanClickable();
	
} ;

//-------------------------------------------------------------------------------------------------





///////////////////////////////////////////////////////////////////////////////////////////	
// 2005. 6. 20	Park Ji-Ho 
//
// Goddess summoning and directing class 
///////////////////////////////////////////////////////////////////////////////////////////	

using namespace std;

//Arua step-by-step Visual constants 
typedef enum 
{	
	GOD_NONE=0,GOD_APPEAR_PARTCLE, GOD_APPEAR_GODDESS, 
	GOD_SPRITE_EFFECT,GOD_END_EFFECT, MAX_GOD_EFFECT 

} GOD_STATE;


//아루아 구조체 
typedef struct _tagGODDESSSTR 
{

	BOOL		bEnable;		//GOD SetUp Status 
	GOD_STATE	god_State;	 	//GOD Action status
								
	HNODE		hGoddess;
	HNODE		hSprite;

	WORD		idxMaster;		//The character that will be associated with the object 
	DWORD		sTick,eTick;	//A tick on the output
	D3DXVECTOR3	cpos;			//Output location 

	CObjMOB		*pGODModel;		//Goddess &amp; fairy o beub project
	CObjMOB		*pSprModel;
	
	CEffect*	pEffect[2];
	HNODE		hParticle[2];

	float		frame;			//Frame

	float		fviewGODD;		//Business blog	
	float		fviewSpr;		
	
	
	//Struct initialization 
	void Init(void)
	{
		bEnable		= FALSE;
		god_State	= GOD_NONE; 
		
		pGODModel	= NULL;
		pSprModel	= NULL;
	
		hGoddess	= NULL;
		hSprite		= NULL;
		
		for(register int i=0;i<2;i++)
		{
			pEffect[i]		= NULL;
			hParticle[i]	= NULL;
		}

		idxMaster	= 0; 
		frame		= 0;
		fviewGODD	= 0;
		fviewSpr	= 0;

		cpos		= D3DXVECTOR3(0,0,0);

		sTick = eTick = 0;
		
	}
	
}GODDESSSTR,*LPGODDESSSTR;





typedef map<WORD,GODDESSSTR>::iterator GODLIST;
class CGoddessMgr
{

	BOOL		m_IsUse;				//Arua use 
	DWORD		m_Count;				//List count 
	map<WORD,GODDESSSTR>	m_list;		//< Server index, arua struct >

	DWORD		m_dwAvata;				//The avatar's own On/Off
	BOOL		m_IsAvata;				//The current existence of the object is to be the avatar set

public:
	CGoddessMgr();
	~CGoddessMgr();

	BOOL  Init(void);
	void  Release(void); 
	void  Release_List(GODDESSSTR* pGDS);
	BOOL  Permission_Country(void);

	BOOL Register_God_And_Sprite(void);
	BOOL Register_RenderObj(GODDESSSTR& gds);
	BOOL SetProcess(DWORD bonOff,WORD idx,BOOL bPrograss=FALSE);
	
	void Set_GDSData(GODDESSSTR& gds,BOOL bonOff,BOOL bPrograss);
	void Update(void);
	int  ProcessVisible(float& fv,float fseq);

	BOOL& IsAvataAroa(void)  	{ return m_IsAvata;	}
	BOOL& IsGetUse(void)		{ return m_IsUse;		}
	DWORD IsAvataState(void)	{ return m_dwAvata;	} 


};


extern CGoddessMgr goddessMgr;


#endif


