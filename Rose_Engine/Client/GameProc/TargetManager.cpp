#include "stdafx.h"
#include ".\targetmanager.h"
#include "../CViewMSG.h"

#include "../Interface/CTDrawImpl.h"
#include "../Interface/IO_ImageRes.h"
#include "../Interface/it_Mgr.h"
#include "../OBJECT.h"

#include "../JCommandState.h"
#include "../../TGameCtrl/ResourceMgr.h"
#include "../Interface/CNameBox.h"


CTargetManager _TargetManager;


CTargetManager::CTargetManager(void)
{
}

CTargetManager::~CTargetManager(void)
{
}

void CTargetManager::SetMouseTargetObject( int iObjectIDX )
{ 
	m_iCurrentMouseTargetObject = iObjectIDX; 	
}

void CTargetManager::Proc()
{
	if( m_iCurrentMouseTargetObject )
	{
		g_pViewMSG->AddObjIDX( m_iCurrentMouseTargetObject );
	}

	/// The currently selected object
	if ( g_UserInputSystem.GetCurrentTarget() ) 
	{
		/// If it's Mob
		CObjCHAR *pObj = (CObjCHAR*)g_pObjMGR->Get_CharOBJ( g_UserInputSystem.GetCurrentTarget(), true );
		/// Target is not valid. Initialize mouse com mandrel
		if( pObj != NULL )
		{		
			if( pObj->IsA( OBJ_MOB ) )
			{
				g_pViewMSG->AddObjIDX( g_UserInputSystem.GetCurrentTarget() );
			}
		}
	}
}

void CTargetManager::Draw()
{	
}