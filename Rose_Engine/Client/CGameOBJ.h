/*
	$Header: /Client/CGameOBJ.h 22    04-07-14 5:08p Jeddli $
*/
#ifndef	__COBJECT_H
#define	__COBJECT_H



/// 
/// Game object class
/// All games within the top-level object
///

class CGameOBJ 
{
private:
	static	float			m_fPickDistance;		///< distance from Rayorigin to collision point 
	static	D3DXVECTOR3		m_PickPosition;			///< Collision detection is used as a temporary variable. (Collision point)

protected:
	static  bool			m_bStepOnObject;
	bool					m_bIsVisible;

	///
	/// for special userdata
	///
	int						m_iSpecialUserData;

	D3DXVECTOR3				m_PosCUR;						///< Current position

public :
	short					m_nIndex;						///< Object slot number	
	classDLLNODE< int >	*	m_pListNODE;


	///
	/// for special userdata
	///
	void					SetSpecialUserNumericData( int iData ){ m_iSpecialUserData = iData; }
	int						GetSpecialUserNumericData(){ return m_iSpecialUserData; }


	CGameOBJ ();
	virtual ~CGameOBJ ();

	/// Slot number of the object.
	short					Get_INDEX ()						{	return	m_nIndex;		}
	static const			D3DXVECTOR3& Get_PickPOSITION ()	{	return	m_PickPosition;	}

	/// You are here
	virtual D3DXVECTOR3&	Get_CurPOS ()						{	return	m_PosCUR;		}
	void					Set_CurPOS (D3DXVECTOR3& PosCUR)	{	m_PosCUR = PosCUR;		}


	/// When picking picking over a given length positioning
	static void				AdjustPickPOSITION (float fMaxDistanceFromAvatar);


	bool					IsA (t_ObjTAG Obj_TYPE)	{	return ( Get_TYPE() == Obj_TYPE		);					}
	bool					IsCHAR ()				{	return ( Get_TYPE() >= OBJ_NPC );	}
	bool					IsNPC  ()				{	return ( Get_TYPE()>=OBJ_NPC && Get_TYPE()<=OBJ_MOB );	}
	bool					IsUSER ()				{	return ( Get_TYPE() == OBJ_AVATAR || Get_TYPE() == OBJ_USER ); }
	bool					IsITEM ()				{	return ( Get_TYPE() == OBJ_ITEM		);					}
	bool					IsPET()					{   return ( Get_TYPE() == OBJ_CART || Get_TYPE() == OBJ_CGEAR || Get_TYPE() == OBJ_NRIDE ); }


	static bool				IsIntersect (HNODE hNODE, float &fCurDistance);

	bool					IsVisible ()			{	return m_bIsVisible;					}




	//////////////////////////////////////////////////////////////////////////////////////////
	/// > GameObj virtual functions
	

	/// There are in the view frustum?
	virtual bool		IsInViewFrustum() = 0{ *(int*)0 = 10; };
	virtual bool		IsIntersect (float &fCurDistance)=0{ *(int*)0 = 10; };
	/// Between the camera and the object will need different treatment.
	virtual bool		IsIntersectForCamera (float& fCurDis){ return false; };
	/// Insert objects in the scene.
	virtual void		InsertToScene (void)=0{ *(int*)0 = 10; };
	/// Remove the object from the scene
	virtual void		RemoveFromScene (bool bIncludeEFFECT=true)=0{ *(int*)0 = 10; };

	virtual int			Get_TYPE ()=0{ *(int*)0 = 10; };
	virtual char*		Get_NAME ()			{	return NULL;	}
	virtual int			Proc (void)			{	return 0;		}

	/// > End
	//////////////////////////////////////////////////////////////////////////////////////////





} ;

//-------------------------------------------------------------------------------------------------
#endif
