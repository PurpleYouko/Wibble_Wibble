#include "stdafx.h"
#include "cgamedatacreateavatar.h"

#include "../CJustModelAVT.h"
#include "../Game.h"
#include "../Interface/it_mgr.h"
#include "../Interface/CToolTipMgr.h"
#include "../Interface/TypeResource.h"
#include "../System/SystemProcScript.h"

#include "../Country.h"


CGameDataCreateAvatar::CGameDataCreateAvatar(void)
{
	//조성현 2005 11 - 21 생성된 아바타
	m_CreateAvatar = NULL;

	c_AvatarPositions[0][0] = -4890.37f + 520000.0f; c_AvatarPositions[0][1] = 135.333f + 520000.0f;c_AvatarPositions[0][2] = 5497.79f;
	c_AvatarRotation[0][0] = 0.0f; c_AvatarRotation[0][1] = 0.0f; c_AvatarRotation[0][2] = 0.67353f; c_AvatarRotation[0][3] = 0.73916f; 

	c_AvatarPositions[1][0] = -4841.4f + 520000.0f; c_AvatarPositions[1][1] = 261.975f + 520000.0f; c_AvatarPositions[1][2] = 5497.79f;
	c_AvatarRotation[1][0] = 0.0f; c_AvatarRotation[1][1] = 0.0f; c_AvatarRotation[1][2] = 0.455356f; c_AvatarRotation[1][3] = 0.89031f; 

	c_AvatarPositions[2][0] = -4735.52f + 520000.0f; c_AvatarPositions[2][1] = 347.595f + 520000.0f; c_AvatarPositions[2][2] = 5497.79f;
	c_AvatarRotation[2][0] = 0.0f; c_AvatarRotation[2][1] = 0.0f; c_AvatarRotation[2][2] = 0.21768f; c_AvatarRotation[2][3] = 0.97602f; 

	c_AvatarPositions[3][0] = -4602.94f + 520000.0f; c_AvatarPositions[3][1] = 368.719f + 520000.0f; c_AvatarPositions[3][2] = 5497.79f;
	c_AvatarRotation[3][0] = 0.0f;c_AvatarRotation[3][1] = 0.0f; c_AvatarRotation[3][2] = -0.0587484f; c_AvatarRotation[3][3] = 0.998273f; 


	c_AvatarPositions[4][0] = -4475.84f + 520000.0f; c_AvatarPositions[4][1] = 319.976f + 520000.0f; c_AvatarPositions[4][2] = 5497.79f;
	c_AvatarRotation[4][0] = 0.0f; c_AvatarRotation[4][1] = 0.0f; c_AvatarRotation[4][2] = -0.303677f; c_AvatarRotation[4][3] = 0.952775f;
}

CGameDataCreateAvatar::~CGameDataCreateAvatar(void)
{

}

CGameDataCreateAvatar& CGameDataCreateAvatar::GetInstance()
{
	static CGameDataCreateAvatar s_Instance;
	return s_Instance;
}

void CGameDataCreateAvatar::Clear()
{
	std::multimap< int , CJustModelAVT* >::iterator iter;

	for( iter = m_avatars_platinum.begin(); iter != m_avatars_platinum.end(); )
	{
		iter->second->RemoveFromScene();
		iter->second->UnloadModelVisible();

		delete iter->second;
		iter = m_avatars_platinum.erase( iter );
	}

	for( iter = m_avatars_premium.begin(); iter != m_avatars_premium.end(); )
	{
		iter->second->RemoveFromScene();
		iter->second->UnloadModelVisible();

		delete iter->second;
		iter = m_avatars_premium.erase( iter );
	}

	ClearSelectedAvatar();
}

void CGameDataCreateAvatar::AddCreateAvatar( CJustModelAVT* pAvatar )
{
	assert( pAvatar );

	std::multimap< int , CJustModelAVT* >::reverse_iterator riter;
	int count = 0;
	if( pAvatar->IsPlatinum() )
	{
		int platinum = m_avatars_platinum.size();
		int premium = m_avatars_premium.size();

		pAvatar->SetIndex( m_avatars_platinum.size() + m_avatars_premium.size() );
		m_avatars_platinum.insert( make_pair( pAvatar->GetLevel(), pAvatar ) );
	}
	else
	{
		pAvatar->SetIndex( m_avatars_platinum.size() + m_avatars_premium.size() );
		m_avatars_premium.insert( make_pair( pAvatar->GetLevel(), pAvatar ) );
	}


	// 홍근 : 플래티넘 캐릭터 정렬.
	count = 0;
	for( riter = m_avatars_platinum.rbegin(); riter != m_avatars_platinum.rend();++riter,++count )
	{
		setPosition( riter->second->GetModelNode(), 
			c_AvatarPositions[count+3][0], 
			c_AvatarPositions[count+3][1], 
			c_AvatarPositions[count+3][2] 
			);

			setRotationQuat( riter->second->GetModelNode(), c_AvatarRotation[count+3]); 

			//조성현 캐릭터 선택화면 Index 첨가..
			riter->second->m_iSelectPositionIndex = count + 3;

	}

	// 홍근 : 프리미엄 캐릭터 정렬.
	count = 0;
	for( riter = m_avatars_premium.rbegin(); riter != m_avatars_premium.rend();++riter,++count )
	{
		setPosition( riter->second->GetModelNode(), 
			c_AvatarPositions[count][0], 
			c_AvatarPositions[count][1], 
			c_AvatarPositions[count][2] 
			);

			setRotationQuat( riter->second->GetModelNode(), c_AvatarRotation[count]); 

			riter->second->m_iSelectPositionIndex = count;
	}
	


}

void CGameDataCreateAvatar::RemoveCreateAvatar( const char* pszName )
{
	assert( pszName );
	std::multimap< int, CJustModelAVT* >::iterator iter;

	for(iter = m_avatars_platinum.begin(); iter != m_avatars_platinum.end(); ++iter)
	{
		if( iter->second->GetName().compare( pszName ) == 0 )
		{
			iter->second->RemoveFromScene();
			iter->second->UnloadModelVisible();
			delete iter->second;
			iter = m_avatars_platinum.erase( iter );		
			return;
		}
	}

	for(iter = m_avatars_premium.begin(); iter != m_avatars_premium.end(); ++iter)
	{
		if( iter->second->GetName().compare( pszName ) == 0 )
		{
			iter->second->RemoveFromScene();
			iter->second->UnloadModelVisible();
			delete iter->second;
			iter = m_avatars_premium.erase( iter );		
			return;
		}
	}



}
HNODE CGameDataCreateAvatar::GetModelNode( const char* pszName )
{
	assert( pszName );
	std::multimap< int, CJustModelAVT* >::iterator iter;

	for(iter = m_avatars_platinum.begin(); iter != m_avatars_platinum.end(); ++iter)
	{
		if( iter->second->GetName().compare( pszName ) == 0 )
			return iter->second->GetModelNode();
	}

	for(iter = m_avatars_premium.begin(); iter != m_avatars_premium.end(); ++iter)
	{
		if( iter->second->GetName().compare( pszName ) == 0 )
			return iter->second->GetModelNode();
	}


	return 0;
}

CJustModelAVT*	CGameDataCreateAvatar::FindAvatar( const char* pszName )
{
	if( pszName == NULL ) return NULL;
	std::multimap< int, CJustModelAVT* >::iterator iter;


	for(iter = m_avatars_platinum.begin(); iter != m_avatars_platinum.end(); ++iter)
	{
		if( iter->second->GetName().compare( pszName ) == 0 )
			return iter->second;
	}

	for(iter = m_avatars_premium.begin(); iter != m_avatars_premium.end(); ++iter)
	{
		if( iter->second->GetName().compare( pszName ) == 0 )
			return iter->second;
	}



	return NULL;
}

const char* CGameDataCreateAvatar::Pick_AVATAR( long x, long y )
{

	D3DXVECTOR3 RayOrig;
	D3DXVECTOR3 RayDir;
	D3DXVECTOR3 PickPosition;
	float		fPickDistance;

	::getRay (x, y, &RayOrig.x, &RayOrig.y,	&RayOrig.z, &RayDir.x, &RayDir.y,	&RayDir.z );

	std::multimap< int, CJustModelAVT* >::reverse_iterator riter;

	float fContactPointX;
	float fContactPointY;
	float fContactPointZ;
	float fDistance;
	HNODE hNode = pickNode ( x, y, &fContactPointX, &fContactPointY, &fContactPointZ, &fDistance );
	if( hNode )
	{
		for(riter = m_avatars_platinum.rbegin(); riter != m_avatars_platinum.rend(); ++riter)
		{
			if( riter->second->GetModelNode() == hNode )
				return riter->second->GetName().c_str();
		}


		for(riter = m_avatars_premium.rbegin(); riter != m_avatars_premium.rend(); ++riter)
		{
			if( riter->second->GetModelNode() == hNode )
				return riter->second->GetName().c_str();
		}


	}

	CJustModelAVT* pAvt = NULL;
	for(riter = m_avatars_platinum.rbegin(); riter != m_avatars_platinum.rend(); ++riter)
	{

		if ( ::intersectRay( riter->second->GetModelNode(),
			RayOrig.x,	RayOrig.y,	RayOrig.z,
			RayDir.x,	RayDir.y,	RayDir.z,
			&PickPosition.x,		&PickPosition.y,		&PickPosition.z,		&fPickDistance) )
		{
			pAvt = riter->second;
			return pAvt->GetName().c_str();
		}
	}

	for(riter = m_avatars_premium.rbegin(); riter != m_avatars_premium.rend(); ++riter)
	{

		if ( ::intersectRay( riter->second->GetModelNode(),
			RayOrig.x,	RayOrig.y,	RayOrig.z,
			RayDir.x,	RayDir.y,	RayDir.z,
			&PickPosition.x,		&PickPosition.y,		&PickPosition.z,		&fPickDistance) )
		{
			pAvt = riter->second;
			return pAvt->GetName().c_str();
		}
	}


	return NULL;
}

void CGameDataCreateAvatar::SelectAvatar( const char* pszName )
{
	assert( pszName );
	if( FindAvatar( pszName ))
		m_strSelectedAvatarName = pszName;
	else
		_RPT1(_CRT_WARN,"Not Found Avatar(%s) @CGameDataCreateAvatar::SelectAvatar\n", pszName );
}

void CGameDataCreateAvatar::SelectAvatar( int iIndex )
{
	std::multimap< int, CJustModelAVT* >::reverse_iterator riter;
	int iCount = 0 ;

	for( riter = m_avatars_premium.rbegin(); riter != m_avatars_premium.rend(); ++riter,++iCount )
	{
		if( iCount == iIndex )
		{
			SelectAvatar( riter->second->GetName().c_str() );
			break;
		}
	}

	iCount = 3;
	for( riter = m_avatars_platinum.rbegin(); riter != m_avatars_platinum.rend(); ++riter,++iCount )
	{
		if( iCount == iIndex )
		{
			SelectAvatar( riter->second->GetName().c_str() );
			break;
		}
	}

}

void CGameDataCreateAvatar::ClearSelectedAvatar()
{
	m_strSelectedAvatarName.clear();
}

const char*	CGameDataCreateAvatar::GetSelectedAvatarName()
{
	return m_strSelectedAvatarName.c_str();
}

int CGameDataCreateAvatar::GetSelectedAvatarIndex()
{
	if( m_strSelectedAvatarName.empty() )
		return -1;

	std::multimap< int, CJustModelAVT* >::iterator iter;


	for( iter = m_avatars_premium.begin(); iter != m_avatars_premium.end(); ++iter )
	{
		if( iter->second->GetName().compare( m_strSelectedAvatarName.c_str() ) == 0 )
			return iter->second->GetIndex();
	}


	for( iter = m_avatars_platinum.begin(); iter != m_avatars_platinum.end(); ++iter )
	{
		if( iter->second->GetName().compare( m_strSelectedAvatarName.c_str() ) == 0 )
			return iter->second->GetIndex();
	}

	return -1;
}

CJustModelAVT* CGameDataCreateAvatar::GetSelectedAvatar()
{
	std::multimap< int, CJustModelAVT* >::iterator iter;

	for( iter = m_avatars_platinum.begin(); iter != m_avatars_platinum.end(); ++iter )
	{
		if( iter->second->GetName().compare( m_strSelectedAvatarName.c_str() ) == 0 )
			return iter->second;
	}

	for( iter = m_avatars_premium.begin(); iter != m_avatars_premium.end(); ++iter )
	{
		if( iter->second->GetName().compare( m_strSelectedAvatarName.c_str() ) == 0 )
			return iter->second;
	}

	return NULL;
}

void CGameDataCreateAvatar::Update()
{
	std::multimap< int, CJustModelAVT* >::iterator iter;

	for( iter =	m_avatars_premium.begin(); iter != m_avatars_premium.end(); ++iter )
	{
		iter->second->Update();

		if(getAnimatableState(iter->second->GetModelNode()) == 0)
		{
			if(iter->second->m_iCurrentMotionState == 2 || iter->second->m_iCurrentMotionState == 0)
			{
				iter->second->SetMotion(iter->second->m_iMotionIDX[1], 0);
				iter->second->m_iCurrentMotionState = 1;
			}
		}
	}

	for( iter =	m_avatars_platinum.begin(); iter != m_avatars_platinum.end(); ++iter )
	{	
		iter->second->Update();

		if(getAnimatableState(iter->second->GetModelNode()) == 0)
		{			
			if(iter->second->m_iCurrentMotionState == 2 || iter->second->m_iCurrentMotionState == 0)
			{
				iter->second->SetMotion(iter->second->m_iMotionIDX[1], 0);
				iter->second->m_iCurrentMotionState = 1;
			}
		}
	}

	if( m_strSelectedAvatarName.empty() )
		return;

	if( CJustModelAVT* pAVT = FindAvatar( m_strSelectedAvatarName.c_str() ) )
	{
		float fPositionXYZ[3];
		float fScreenXYZ[3];
		if( getPosition ( pAVT->GetModelNode(),  fPositionXYZ ) )
		{
			::worldToScreen( fPositionXYZ[0], fPositionXYZ[1], fPositionXYZ[2] , &fScreenXYZ[0], &fScreenXYZ[1], &fScreenXYZ[2] );


			CInfo Info;
			Info.Clear();
			POINT pt = { (LONG)fScreenXYZ[0] - 40, (LONG)fScreenXYZ[1] - 250};

			Info.AddString( pAVT->GetName().c_str() ,g_dwYELLOW, g_GameDATA.m_hFONT[ FONT_NORMAL ], DT_CENTER );
			Info.AddString( CStr::Printf( "%s: %d",CStringManager::GetSingleton().GetAbility( AT_LEVEL ), pAVT->GetLevel() ));
			Info.AddString( CStr::Printf( "%s: %s",CStringManager::GetSingleton().GetAbility( AT_CLASS ), CStringManager::GetSingleton().GetJobName( pAVT->GetJob() )));
		

			if( pAVT->IsPlatinum() )
			{
				Info.AddString( STR_PLATINUM, g_dwYELLOW );
			}

			Info.SetImage( 3, "GID_IMG_AVATARINFO" ); // IMAGE_RES_EXUI = 3
			Info.SetInfoType( CInfo::INFOTYPE_ONEIMG );
			Info.SetPositionText( 27,  35 );			
			
			pt.x = ( g_pCApp->GetWIDTH() - Info.GetWidth() ) / 2;
			pt.y = g_pCApp->GetHEIGHT() - 136;

			Info.SetPosition( pt );
			CToolTipMgr::GetInstance().RegistInfo( Info );

			CToolTipMgr::GetInstance().Draw();

			Info.Clear();

			if( pAVT->GetDeleteRemainSEC() )
			{
				Info.AddString( CStr::Printf( "%s: %3.1f%s",
					STR_REMAIN_REMOVE_WAITTIME,
					pAVT->GetDeleteRemainSEC() / 3600.f, STR_HOUR ),
					g_dwWHITE );

				pt.x = ( g_pCApp->GetWIDTH() - Info.GetWidth() ) / 2 + 180;
				pt.y = g_pCApp->GetHEIGHT() - 73;

				Info.SetImage( 3, "GID_IMG_REMOVETIME" ); // IMAGE_RES_EXUI = 3
				Info.SetInfoType( CInfo::INFOTYPE_ONEIMG );
				Info.SetPositionText( 10, 17 );	
				Info.SetPosition( pt );

				CToolTipMgr::GetInstance().RegistInfo( Info );
			}

			CToolTipMgr::GetInstance().Draw();		
		}
	}
}
int	CGameDataCreateAvatar::GetAvatarCount()
{
	return (int)( m_avatars_premium.size() + m_avatars_platinum.size() );
}



void CGameDataCreateAvatar::InputSceneAvatar()
{

	std::multimap< int , CJustModelAVT* >::reverse_iterator riter;
	for(riter = m_avatars_premium.rbegin(); riter != m_avatars_premium.rend(); ++riter)
	{
		::InputSceneOBBNode(riter->second->GetModelNode());
	}
}

//조성현 2005 12 -14 캐릭터 선택화면..
CJustModelAVT* CGameDataCreateAvatar::FindAvatarCount(int count)
{
	std::multimap< int, CJustModelAVT* >::iterator iter;

	for(iter = m_avatars_platinum.begin(); iter != m_avatars_platinum.end(); ++iter)
	{
		if( iter->second->m_iSelectPositionIndex == count )
			return iter->second;
	}

	for(iter = m_avatars_premium.begin(); iter != m_avatars_premium.end(); ++iter)
	{
		if( iter->second->m_iSelectPositionIndex == count )
			return iter->second;
	}

	return NULL;
}

CJustModelAVT*CGameDataCreateAvatar::FindAvatarCurrentState( int iState )
{
	std::multimap< int, CJustModelAVT* >::iterator iter;

	for(iter = m_avatars_platinum.begin(); iter != m_avatars_platinum.end(); ++iter)
	{
		if( iter->second->m_iCurrentMotionState == iState )
			return iter->second;
	}

	for(iter = m_avatars_premium.begin(); iter != m_avatars_premium.end(); ++iter)
	{
		if( iter->second->m_iCurrentMotionState == iState )
			return iter->second;
	}

	return NULL;
}
