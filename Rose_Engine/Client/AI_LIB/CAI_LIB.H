#ifndef	__CAI_OBJ_H
#define	__CAI_OBJ_H

///</
///</ Hercules intelligent virtual objects ...
///</
class STBDATA;
class CAI_OBJ 
{
private :
	ULONG			 m_ulAICheckTIME[ 2 ];

public  :
	CAI_OBJ ()		{	::ZeroMemory(m_ulAICheckTIME,sizeof(ULONG)*2);		}

	unsigned long	 Get_AICheckTIME(int iIDX)								{	return m_ulAICheckTIME[iIDX];		}
	void			 Set_AICheckTIME(int iIDX, unsigned long lCheckTIME)	{	m_ulAICheckTIME[iIDX] = lCheckTIME;	}

	virtual float	 Get_CurXPOS() = 0{ *(int*)0 = 10; }					///< Current X-axis position
	virtual float	 Get_CurYPOS() = 0{ *(int*)0 = 10; }					///< Current Y-axis position
#ifndef	__SERVER
	virtual float	 Get_CurZPOS() = 0{ *(int*)0 = 10; }					///< Current Z-axis position
#endif
	virtual float	 Get_BornXPOS() = 0{ *(int*)0 = 10; }					///< Origin X-axis position
	virtual float	 Get_BornYPOS() = 0{ *(int*)0 = 10; }					///< Origin Y-axis position

	virtual int		 Get_TAG() = 0{ *(int*)0 = 10; }						///< Does not overlap with other objects, the only value. (Usually slot index)
//	virtual int		 Get_ObjTYPE() = 0{ *(int*)0 = 10; }					///< The type of the object is obtained.
//	virtual bool	 Is_SameTYPE(int iType) = 0{ *(int*)0 = 10; }			///< IType is the type of the object is the same?
//	virtual bool	 Is_AVATAR() = 0{ *(int*)0 = 10; }						///< Avatar is the type of object?

	virtual int		 Get_PercentHP() = 0{ *(int*)0 = 10; }
	virtual int		 Get_HP() = 0{ *(int*)0 = 10; }						///< Vitality of the object is obtained.
	virtual int		 Get_CharNO() = 0{ *(int*)0 = 10; }					///< Character code of the object is obtained.

	virtual int		 Get_LEVEL ()	=0;
	virtual int		 Get_ATK ()		=0;
	virtual int		 Get_DEF ()		=0;
	virtual int		 Get_RES ()		=0;
	virtual int		 Get_HIT ()		=0;	
	virtual int		 Get_CHARM ()	=0;
	virtual int		 Get_AVOID ()	=0;
	virtual int		 Get_SENSE ()	=0;
	virtual int		 Get_GiveEXP ()	=0;
	virtual int		 Get_CRITICAL()	=0;	

	virtual CAI_OBJ *Get_TARGET() = 0{ *(int*)0 = 10; }					///< Targets is obtained.
	virtual CAI_OBJ *Get_OWNER()						{	return NULL;	}

//	virtual void	 Set_TARGET (CAI_OBJ *pAIObj) = 0{ *(int*)0 = 10; }	///< Targets are set.
	virtual float	 Get_DISTANCE(CAI_OBJ *pAIObj) = 0{ *(int*)0 = 10; }	///< The distance to the target is determined.
	virtual float	 Get_MoveDISTANCE() = 0{ *(int*)0 = 10; }				///< Calculate the distance of the current position from the origin position.

	virtual int		 Get_RANDOM(int iMod) = 0{ *(int*)0 = 10; }			///< The value is determined depending on the object rendeom.

	virtual bool	 Change_CHAR(int /*iCharIDX*/)			{	return false;	}	///< The object is converted into iCharIDX.
	virtual bool	 Create_PET (int /*iCharIDX*/, float /*fPosX*/, float /*fPosY*/, int /*iRange*/)	{	return false;	}	///< Summon the object of iCharIDX.

	virtual void	 Say_MESSAGE (char* /*szMessage*/)		{	;	}		///< And outputs a conversation.
	virtual void	 Run_AWAY(int /*iDistance*/)			{	;	}
	virtual void	 Drop_ITEM(short /*nDropITEM*/)			{	;	}

	// command interface function
	virtual void Set_EMOTION(short /*nEmotionIDX*/)			{	;	}		///< Emotional expression behavior are set.

	virtual bool SetCMD_STOP() = 0{ *(int*)0 = 10; }										///< Stop command to set the object.
	virtual bool SetCMD_MOVE2D (float fPosToX, float fPosToY, BYTE btRunMODE) = 0{ *(int*)0 = 10; }///< Sets a command to move the object.
#ifndef	__SERVER
	virtual bool SetCMD_MOVE (const D3DVECTOR& PosTo, BYTE btRunMODE) = 0{ *(int*)0 = 10; }///< Set the command to move objects.
#endif
	virtual void SetCMD_RUNnATTACK(int iTargetObjTAG) = 0{ *(int*)0 = 10; }				///< Set the command to attack objects.
	virtual void Special_ATTACK() = 0{ *(int*)0 = 10; }									///< Set the object of special attack commands.

	virtual CAI_OBJ* AI_FindFirstOBJ( int iDistance ) = 0{ *(int*)0 = 10; }
	virtual CAI_OBJ* AI_FindNextOBJ () = 0{ *(int*)0 = 10; }

	virtual int		Get_EconomyVAR(short /*nVarIDX*/)		{	return 0;	}
	virtual int		Get_WorldVAR  (short /*nVarIDX*/)		{	return 0;	}

	virtual int		Get_TeamNO () = 0{ *(int*)0 = 10; }

			///PVP With this judgment should not be the only zone. 2005/07/14 - nAvy
			bool	Is_ALLIED( CAI_OBJ *pDestOBJ )
			{
			#ifdef	__SERVER
				if ( this->Get_OWNER() ) {
					if ( this->Get_OWNER() == pDestOBJ || this->Get_OWNER() == pDestOBJ->Get_OWNER() )
						return true;
				}
			#endif
				if ( -1 == pDestOBJ->Get_TeamNO() )		// Team number -1 unconditionally
					return false;

				if ( this->Get_TeamNO() == pDestOBJ->Get_TeamNO() ||
					 (__int64)(this->Get_TeamNO()) * (__int64)(pDestOBJ->Get_TeamNO()) <= 100 )
					return true;

				return false;
			}
			bool	Is_SameTEAM( CAI_OBJ *pDestOBJ )
			{
			#ifdef	__SERVER
				if ( this->Get_OWNER() ) {
					if ( this->Get_OWNER() == pDestOBJ || this->Get_OWNER() == pDestOBJ->Get_OWNER() )
						return true;
				}
			#endif
				if ( -1 == pDestOBJ->Get_TeamNO() )		// Team number -1 unconditionally
					return false;

				return ( this->Get_TeamNO() == pDestOBJ->Get_TeamNO() );
			}

	virtual BYTE	Is_DAY (void) = 0{ *(int*)0 = 10; }						// Returns whether day or night ... day 1 night 0
	virtual int		Get_ZoneTIME (void) = 0{ *(int*)0 = 10; }				// Returns the current time zone ...
	virtual int		Get_WorldTIME(void) = 0{ *(int*)0 = 10; }				// Returns the current time World ...
	virtual void	Set_TRIGGER (t_HASHKEY /*HashKEY*/)		{	;	}
#ifdef	__SERVER
	virtual DWORD	Get_MagicSTATUS() = 0{ *(int*)0 = 10; }
	virtual CAI_OBJ*Find_LocalNPC(int iNpcNO) = 0{ *(int*)0 = 10; }

	virtual void	Set_EconomyVAR(short nVarIDX, int iValue) = 0{ *(int*)0 = 10; }
	virtual void	Set_WorldVAR  (short nVarIDX, int iValue) = 0{ *(int*)0 = 10; }

	virtual void	Add_DAMAGE ( WORD wDamage ) = 0{ *(int*)0 = 10; }

	virtual bool	Send_gsv_CHAT ( char *szMsg ) = 0{ *(int*)0 = 10; }
	virtual bool	Send_gsv_SHOUT( char *szMsg ) = 0{ *(int*)0 = 10; }
	virtual void	Send_gsv_ANNOUNCE_CHAT( char *szMsg ) = 0{ *(int*)0 = 10; }

	virtual bool	SetCMD_Skill2SELF (short /*nSkillIDX*/, short /*nMotion*/)							{	return true;	}
	virtual bool	SetCMD_Skill2OBJ (int /*iTargetObjIDX*/, short /*nSkillIDX*/, short /*nMotion*/)	{	return true;	}
#endif
} ;


///<-------------------------------------------------------------------------------------------------
///
///	Hercules AI files.
///

class CAI_PATTERN;
class CAI_FILE
{
private:
	CAI_PATTERN	 *m_pPattern;
	int			  m_iPatternCNT;
	unsigned long m_ulCheckStopAI;
	int			  m_iRateDamagedAI;

	void AI_Check (WORD wPattern, CAI_OBJ *pSourCHAR, CAI_OBJ *pDestCHAR=NULL, int iDamage=0);
public :
	CAI_FILE ();
	~CAI_FILE ();

	bool Load (char *szFileName, STBDATA *pSTB, int iLangCol);

	/// 0		When first created,
	void AI_WhenCREATED (CAI_OBJ *pSourCHAR);

	/// 1		Idle
	void AI_WhenSTOP (CAI_OBJ *pSourCHAR);

	/// 2		Attack move
	void AI_WhenAttackMOVE (CAI_OBJ *pSourCHAR, CAI_OBJ *pDestCHAR);

	/// 3		When struck in combat strikes
	void AI_WhenDAMAGED (CAI_OBJ *pSourCHAR, CAI_OBJ *pDestCHAR, int iDamage);

	/// 4		When he killed the other
	void AI_WhenKILL (CAI_OBJ *pSourCHAR, CAI_OBJ *pDestCHAR, int iDamage);

	/// 5		When they die,
	void AI_WhenDEAD (CAI_OBJ *pSourCHAR, CAI_OBJ *pDestCHAR, int iDamage);
} ;


///<-------------------------------------------------------------------------------------------------
///
/// The object referenced by the environment, artificial intelligence function ...
///

extern int		AI_SysRANDOM( int iMod );

///<-------------------------------------------------------------------------------------------------
#endif