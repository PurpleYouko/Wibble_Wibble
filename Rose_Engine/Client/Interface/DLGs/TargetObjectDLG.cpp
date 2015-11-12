#include "stdafx.h"
#include "TargetObjectDLG.h"
#include "../CTDrawImpl.h"
#include "../it_Mgr.h"
#include "../IO_ImageRes.h"
#include "../../OBJECT.h"
#include "../../Game.h"

#include "../../JCommandState.h"
#include "../../../TGameCtrl/ResourceMgr.h"

//TGameCtrl
#include "TGuage.h"




CTargetObjectDLG::CTargetObjectDLG()
{
	m_TargetObj = NULL;
	m_pGuage = NULL;
}

CTargetObjectDLG::~CTargetObjectDLG()
{

}

void CTargetObjectDLG::Show()
{

/*#ifdef _DEBUG

	POINT pt = { GetPosition().x, GetPosition().y };
	Clear();
	Create("DlgTarget");
	MoveWindow(pt);

#endif*/

	CTDialog::Show();
}

void CTargetObjectDLG::Draw()
{
	if( !IsVision() ) return;	

	CTDialog::Draw();

	/// 선택된 타겟 출력.
	if ( g_UserInputSystem.GetCurrentTarget() ) 
	{
		CObjCHAR *pObj = (CObjCHAR*)g_pObjMGR->Get_CharOBJ( g_UserInputSystem.GetCurrentTarget(), true );
		/// 유효하지 않은 타겟이다.. 마우스 컴맨드 초기화
		if( pObj == NULL )
		{	
			GetCaption()->SetString( "" );
			m_pGuage->SetValue( 0 );
			return;	
		}

		CResourceMgr*   pResourceMgr = CResourceMgr::GetInstance();
		CTDrawImpl* pDrawObj = g_itMGR.GetDrawImplPtr();
		if( pDrawObj == NULL )	return;		
		
		D3DCOLOR d3Color;

		switch( pObj->Get_TYPE() ) 
		{
			case OBJ_NPC :
				{
					d3Color = g_dwGREEN;
				}				
				break;
				
			default :
				if( pObj->Get_TARGET() && g_pAVATAR->Is_ALLIED( pObj->Get_TARGET() ) )
				{
					d3Color = g_dwRED;
				}
				else
				{
					d3Color = g_dwWHITE;
				}				
				break;
		}	

		GetCaption()->SetString( pObj->Get_NAME(), FONT_NORMAL );
		GetCaption()->SetStringColor( d3Color );			
		
		if( m_pGuage )
		{
			if( pObj->Get_TYPE() == OBJ_MOB ) 
			{	
				m_iValue = pObj->Get_HP();
				if ( m_iValue < 0 ) 	m_iValue = 0;

				m_iMax = pObj->Get_MaxHP();
				m_iPer = 0;
				if( m_iMax != 0 )
				{
					m_iPer = (int)((float) m_iValue * 1000  / m_iMax);
					m_pGuage->SetValue( m_iPer );
				}			
			}
			else
			{
				m_pGuage->SetValue( 0 );
			}
		}
		else
		{
			m_pGuage = (CTGuage*)Find( "HP_GUAGE" );
		}
		

		return;

		/// 캐릭터 얼굴 아이콘..
		//if( ( pObj->Get_TYPE() == OBJ_NPC || pObj->Get_TYPE() == OBJ_MOB )  ) 
		//{
		//	short nFaceIcon = NPC_FACE_ICON( pObj->Get_CharNO() );
		//	pDrawObj->Draw( m_sPosition.x - 20, m_sPosition.y - 24 ,IMAGE_RES_FACE, nFaceIcon );
		//}


		D3DXMATRIX mat;	
		D3DXMatrixTranslation( &mat, (float)m_sPosition.x - 20, (float)m_sPosition.y - 24,0.0f);
		::setTransformSprite( mat );

		

		
		
		
		
		
		RECT rt;
		SetRect( &rt,	85, 
						34, 
						85 + 148, 
						34 + 30 );


		int iTargetLevel = pObj->Get_LEVEL();

		if( 0 )
		{			
			if( pObj->Get_TYPE() == OBJ_AVATAR )
				::drawFontf(g_GameDATA.m_hFONT[ FONT_NORMAL_BOLD ], true, &rt, d3Color,DT_CENTER | DT_VCENTER,"%d/%d", pObj->Get_HP(), pObj->Get_MaxHP() );
			else
				::drawFontf(g_GameDATA.m_hFONT[ FONT_NORMAL_BOLD ], true, &rt, d3Color,DT_CENTER | DT_VCENTER,"%s(%d)", pObj->Get_NAME(), iTargetLevel );
		}
		else
		{		
			::drawFontf(g_GameDATA.m_hFONT[ FONT_NORMAL_BOLD ], true, &rt, d3Color,DT_CENTER | DT_VCENTER,"%s", pObj->Get_NAME() );
		}

		if( pObj->Get_TYPE() == OBJ_MOB ) 
		{
			int iCurrentHP = pObj->Get_HP();
			int iMaxHP = pObj->Get_MaxHP();
			
			if( iCurrentHP > iMaxHP )
			{
				iCurrentHP = iMaxHP;
				pObj->Set_HP( iCurrentHP );
			}

			if( iMaxHP != 0 )
			{
				short nWidth = ( 151 * iCurrentHP ) / iMaxHP;			
//				if( nWidth > 151 )
//					nWidth = 151;
				pDrawObj->Draw( m_sPosition.x + 60, m_sPosition.y + 33, nWidth, UI_IMAGE_RES_ID, pResourceMgr->GetImageNID( UI_IMAGE_RES_ID, "ID_GUAGE_RED") );	
			}
		}	
	}
	else
	{
		GetCaption()->SetString( "Target Dialog.", FONT_NORMAL );

		if(m_pGuage)	m_pGuage->SetValue( 0 );	
	}	
}

unsigned int CTargetObjectDLG::Process(UINT uiMsg,WPARAM wParam,LPARAM lParam)
{
	if( !IsVision()  ) return 0;

	unsigned iProcID = 0;
	if( iProcID = CTDialog::Process(uiMsg,wParam,lParam)) 
	{
		switch(uiMsg)
		{
		case WM_LBUTTONUP:
			On_LButtonUP( iProcID, wParam, lParam );			
			break;
		default:
			break;
		}
		return uiMsg;
	}
	return 0;
}
bool CTargetObjectDLG::On_LButtonUP( unsigned iProcID , WPARAM wParam, LPARAM lParam )
{
	switch( iProcID )
	{
	
	case IID_BTN_CLOSE:
		Hide();
		break;
	
	default:
		break;
	}

	return true;
}
