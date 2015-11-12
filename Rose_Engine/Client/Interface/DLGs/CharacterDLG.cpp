#include "StdAfx.h"
#include "CharacterDLG.h"
#include "../it_mgr.h"
#include "../CTDrawImpl.h"
#include "../CToolTipMgr.h"
#include "../CDragItem.h"
#include "../IO_ImageRes.h"
#include "../TypeResource.h"

#include "../icon/ciconitem.h"

#include "../../System/CGame.h"
#include "../../OBJECT.h"
#include "../../Network/CNetWork.h"
#include "../../gamecommon/item.h"
#include "../../GameData/Event/CTEventItem.h"
#include "../../GameData/CClan.h"

#include "TabbedPane.h"
#include "TGuage.h"
#include "JContainer.h"
#include "ResourceMgr.h"
#include "TImage.h"
#include "../CClanMarkView.h"
#include "../misc/gameutil.h"


CCharacterDLG::CCharacterDLG( int iType )
{
	m_iGuageBlueGID = 0;
	m_iGuageYellowGID = 0;
	m_iGuageRedGID = 0;

	m_iTab = BASIC_INFO;
	SetDialogType( iType );
}

CCharacterDLG::~CCharacterDLG()
{
}

void CCharacterDLG::Show()
{
#ifdef _DEBUG

	POINT pt = { GetPosition().x, GetPosition().y };
	Clear();
	Create("DlgAvata");		
	SetInterfacePos_After();
	MoveWindow(pt);

#endif

	CTDialog::Show();
}

void CCharacterDLG::Draw()
{
	if(!IsVision()) return ;

	CTDialog::Draw();

	DrawBasicInfo2();	
	DrawAbilityInfo2();	
	DrawUnionInfo2();
}




unsigned int  CCharacterDLG::Process(UINT uiMsg,WPARAM wParam,LPARAM lParam)
{
	if( g_pAVATAR == NULL )
		return 0;


	if(!IsVision()) return 0;

	unsigned iProcID = 0;
	if( iProcID =  CTDialog::Process(uiMsg,wParam,lParam)) 
	{
		switch(uiMsg)
		{
		case WM_LBUTTONDOWN:
			On_LButtonDN( iProcID, wParam, lParam );
			break;
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


bool CCharacterDLG::On_LButtonUP( unsigned iProcID, WPARAM wParam, LPARAM lParam )
{
	switch( iProcID )
	{
	case IID_BTN_DIALOG2ICON:
		g_itMGR.AddDialogIcon( 	GetDialogType() );
		break;
	case IID_BTN_CLOSE:
		Hide();
		break;

	default:
		return true;
	}
	return false;
}

bool CCharacterDLG::On_LButtonDN( unsigned iProcID, WPARAM wParam, LPARAM lParam )
{

	short nNeedPoint = 0;	
	switch( iProcID )
	{
	case IID_TAB_BASICINFO_BTN:
		m_iTab = BASIC_INFO;
		break;
	case IID_TAB_ABILITY_BTN:
		m_iTab = ABILITY_INFO;
		break;
	case IID_TAB_UNION_BTN:
		m_iTab = UNION_INFO;
		break;
	case IID_BTN_UP_STR:
		nNeedPoint = g_pAVATAR->Get_NeedPoint2AbilityUP( BA_STR );
		if( ( g_pAVATAR->Get_BonusPOINT() - nNeedPoint ) >= 0 )
		{
			//			g_pAVATAR->Set_BonusPOINT( g_pAVATAR->Get_BonusPOINT() - nNeedPoint );
			g_pNet->Send_cli_USE_BPOINT_REQ( BA_STR );
		}			
		break;
	case IID_BTN_UP_DEX:
		nNeedPoint = g_pAVATAR->Get_NeedPoint2AbilityUP( BA_DEX );
		if( ( g_pAVATAR->Get_BonusPOINT() - nNeedPoint ) >= 0 )
		{
			//			g_pAVATAR->Set_BonusPOINT( g_pAVATAR->Get_BonusPOINT() - nNeedPoint );
			g_pNet->Send_cli_USE_BPOINT_REQ( BA_DEX );
		}
		break;
	case IID_BTN_UP_INT:
		nNeedPoint = g_pAVATAR->Get_NeedPoint2AbilityUP( BA_INT );
		if( ( g_pAVATAR->Get_BonusPOINT() - nNeedPoint ) >= 0 )
		{
			//			g_pAVATAR->Set_BonusPOINT( g_pAVATAR->Get_BonusPOINT() - nNeedPoint );
			g_pNet->Send_cli_USE_BPOINT_REQ( BA_INT );
		}
		break;
	case IID_BTN_UP_CON:
		nNeedPoint = g_pAVATAR->Get_NeedPoint2AbilityUP( BA_CON );
		if( ( g_pAVATAR->Get_BonusPOINT() - nNeedPoint ) >= 0 )
		{
			//			g_pAVATAR->Set_BonusPOINT( g_pAVATAR->Get_BonusPOINT() - nNeedPoint );
			g_pNet->Send_cli_USE_BPOINT_REQ( BA_CON );
		}
		break;
	case IID_BTN_UP_CHARM:
		nNeedPoint = g_pAVATAR->Get_NeedPoint2AbilityUP( BA_CHARM );
		if( ( g_pAVATAR->Get_BonusPOINT() - nNeedPoint ) >= 0 )
		{
			//			g_pAVATAR->Set_BonusPOINT( g_pAVATAR->Get_BonusPOINT() - nNeedPoint );
			g_pNet->Send_cli_USE_BPOINT_REQ( BA_CHARM );
		}
		break;
	case IID_BTN_UP_SENSE:
		nNeedPoint = g_pAVATAR->Get_NeedPoint2AbilityUP( BA_SENSE );
		if( ( g_pAVATAR->Get_BonusPOINT() - nNeedPoint ) >= 0 )
		{
			//			g_pAVATAR->Set_BonusPOINT( g_pAVATAR->Get_BonusPOINT() - nNeedPoint );
			g_pNet->Send_cli_USE_BPOINT_REQ( BA_SENSE );
		}
		break;
	default:
		break;
	}
	return false;
}



void CCharacterDLG::Update( POINT ptMouse )
{
	if( !IsVision() ) return;

	CTDialog::Update( ptMouse );
	CWinCtrl * pCtrl = NULL;	

	pCtrl = Find( IID_GUAGE_STAMINA );
	if( pCtrl && pCtrl->GetControlType() == CTRL_GUAGE )
	{
		CTGuage* pGuage = (CTGuage*)pCtrl;
		int iValue = g_pAVATAR->GetCur_STAMINA() * pGuage->GetValueUnitSize() / g_pAVATAR->GetCur_MaxSTAMINA();

		pGuage->SetFont( FONT_NORMAL_OUTLINE );
		pGuage->SetAlign( DT_CENTER || DT_VCENTER );

		pGuage->SetValue( iValue );
		pGuage->SetText( CStr::Printf("%d / %d", g_pAVATAR->GetCur_STAMINA(), g_pAVATAR->GetCur_MaxSTAMINA () ) );		
	}

	int   iPer		= 0;	
	int   iValue	= 0;
	int	  iMax		= 1;
	char*	pszBuf;
	CTGuage* pGuage = (CTGuage*)Find( "HP_GUAGE" );
	if( pGuage && g_pAVATAR )
	{
		int iValue = g_pAVATAR->Get_HP();
		if ( iValue < 0 ) 	iValue = 0;

		iMax = g_pAVATAR->Get_MaxHP();
		if( iMax != 0 )
		{
			iPer = (int)((float) iValue * pGuage->GetValueUnitSize()  / iMax);
			pGuage->SetValue( iPer );
			pGuage->SetFont( FONT_NORMAL_OUTLINE );
			pGuage->SetAlign( DT_CENTER || DT_VCENTER );

			pszBuf = CStr::Printf( "%d/%d", iValue, iMax);
			pGuage->SetText( pszBuf );		
		}
	}


	pGuage = (CTGuage*)Find( "MP_GUAGE" );
	if( pGuage )
	{
		int iValue = g_pAVATAR->Get_MP();
		if ( iValue < 0 ) 	iValue = 0;

		iMax = g_pAVATAR->Get_MaxMP();
		if( iMax != 0 )
		{
			iPer = (int)((float)(iValue *pGuage->GetValueUnitSize()) / iMax);
			pGuage->SetValue( iPer );
			pGuage->SetFont( FONT_NORMAL_OUTLINE );
			pGuage->SetAlign( DT_CENTER || DT_VCENTER );
			pszBuf = CStr::Printf( "%d/%d", iValue, iMax);
			pGuage->SetText( pszBuf );
		}
	}

	__int64 i64Per = 0;
	pGuage = (CTGuage*)Find( "EXP_GUAGE" );
	if( pGuage )
	{
		__int64 i64Value = g_pAVATAR->Get_EXP();
		if ( i64Value < 0 ) 	i64Value = 0;

		__int64 i64Max = g_pAVATAR->Get_NeedEXP( g_pAVATAR->Get_LEVEL() );
		if( i64Max != 0 )
		{
			i64Per = i64Value * pGuage->GetValueUnitSize() / i64Max;

			pGuage->SetValue( (int)i64Per );
			pGuage->SetFont( FONT_NORMAL_OUTLINE );
			pGuage->SetAlign( DT_CENTER || DT_VCENTER );

			char szValue[128];		g_GameUtil.Convert2String(i64Value, szValue );
			char szMax[128];		g_GameUtil.Convert2String(i64Max, szMax);
			pszBuf = CStr::Printf( "%s/%s", szValue, szMax );

			pGuage->SetText( pszBuf );
		}
	}		

	//ToolTip	
	for( int i=IID_TXT_REQ_STR; i<=IID_TXT_REQ_SEN; i++)
	{
		if( pCtrl = Find(i) )
		{
			if( pCtrl->IsInside( ptMouse.x, ptMouse.y ) )
			{
				goto ToolTip;
			}
		}
	}
	return;

ToolTip:
	//06. 10. 13 - ������ : �ɸ��� ����â�� �ʿ� ��ų����Ʈ�� �ʿ� ��������Ʈ�� ��Ʈ�� ����.
#ifdef __NEWUPDATEPARTYBUFF
	CToolTipMgr::GetInstance().RegToolTip( ptMouse.x, ptMouse.y, STR_ENOUGH_STAT_POINT );				
#else
	CToolTipMgr::GetInstance().RegToolTip( ptMouse.x, ptMouse.y, LIST_STRING(506) );
#endif
	return;



	switch( m_iTab )
	{
	case BASIC_INFO:
		{
			CWinCtrl* pCtrl = Find( IID_TABBEDPANE );
			if( pCtrl && pCtrl->GetControlType() == CTRL_TABBEDPANE )
			{
				CTabbedPane* pPane = (CTabbedPane*)pCtrl;

				CJContainer* pContainer = pPane->GetTabContainer( IID_TABBASIC );
				assert( pContainer );
				if( pContainer )
				{
					pCtrl = pContainer->Find( IID_GUAGE_STAMINA );
					if( pCtrl && pCtrl->GetControlType() == CTRL_GUAGE )
					{
						CTGuage* pGuage = (CTGuage*)pCtrl;
						int iValue = g_pAVATAR->GetCur_STAMINA() * 100 / g_pAVATAR->GetCur_MaxSTAMINA();


						///if( iValue != pGuage->GetValue() )
						{
							pGuage->SetValue( iValue );
							pGuage->SetText( CStr::Printf("%d / %d", g_pAVATAR->GetCur_STAMINA(), g_pAVATAR->GetCur_MaxSTAMINA () ) );
							if( g_pAVATAR->GetCur_STAMINA() >= g_pAVATAR->GetCur_MaxSTAMINA () )
								pGuage->SetGuageImageID( m_iGuageBlueGID );
							else
								pGuage->SetGuageImageID( m_iGuageYellowGID );
						}
					}
				}
			}
			break;
		}
	case ABILITY_INFO:
		{
			///���� ���� ǥ��
			CTDialog* pDlg = CTDialog::GetProcessMouseOverDialog();
			if( pDlg && pDlg != this )
				return ;

			CWinCtrl* pCtrl = CWinCtrl::GetProcessMouseOverCtrl();
			if( pCtrl )
			{
				switch( pCtrl->GetControlID() )
				{
				case IID_BTN_UP_STR:
					CToolTipMgr::GetInstance().RegToolTip( ptMouse.x, ptMouse.y, 
						CStr::Printf("%s %d",STR_REQUIRE_POINT, g_pAVATAR->Get_NeedPoint2AbilityUP( BA_STR ) ));
					break;
				case IID_BTN_UP_DEX:
					CToolTipMgr::GetInstance().RegToolTip( ptMouse.x, ptMouse.y, 
						CStr::Printf("%s %d",STR_REQUIRE_POINT,g_pAVATAR->Get_NeedPoint2AbilityUP( BA_DEX ) ));
					break;
				case IID_BTN_UP_INT:
					CToolTipMgr::GetInstance().RegToolTip( ptMouse.x, ptMouse.y, 
						CStr::Printf("%s %d",STR_REQUIRE_POINT,g_pAVATAR->Get_NeedPoint2AbilityUP( BA_INT ) ));
					break;
				case IID_BTN_UP_CON:
					CToolTipMgr::GetInstance().RegToolTip( ptMouse.x, ptMouse.y, 
						CStr::Printf("%s %d",STR_REQUIRE_POINT,g_pAVATAR->Get_NeedPoint2AbilityUP( BA_CON ) ));
					break;
				case IID_BTN_UP_CHARM:
					CToolTipMgr::GetInstance().RegToolTip( ptMouse.x, ptMouse.y, 
						CStr::Printf("%s %d",STR_REQUIRE_POINT,g_pAVATAR->Get_NeedPoint2AbilityUP( BA_CHARM ) ));
					break;
				case IID_BTN_UP_SENSE:
					CToolTipMgr::GetInstance().RegToolTip( ptMouse.x, ptMouse.y, 
						CStr::Printf("%s %d",STR_REQUIRE_POINT, g_pAVATAR->Get_NeedPoint2AbilityUP( BA_SENSE ) ));
					break;
				default:
					break;
				}
			}
			break;
		}
	default:
		break;
	}
}

bool CCharacterDLG::IsVision()
{
	if( g_pAVATAR == NULL )
		return CTDialog::IsVision();
	else
		return( CTDialog::IsVision() && g_pAVATAR->Get_HP() > 0 );

	return false;
}

void CCharacterDLG::DrawBasicInfo()
{
	D3DXMATRIX mat;	
	D3DXMatrixTranslation( &mat, (float)m_sPosition.x, (float)m_sPosition.y,0.0f);
	::setTransformSprite( mat );

	///�̸�
	::drawFont( g_GameDATA.m_hFONT[ FONT_NORMAL_BOLD ], true, 59,67, D3DCOLOR_XRGB( 0, 255, 42 ),  g_pAVATAR->Get_NAME() );		
	///Ŭ����(����)
	::drawFont( g_GameDATA.m_hFONT[ FONT_NORMAL ], true, 59,88, D3DCOLOR_XRGB( 0, 210, 255), CStringManager::GetSingleton().GetJobName( g_pAVATAR->Get_JOB() ) );		

	if( CClan::GetInstance().GetName() )
		::drawFont( g_GameDATA.m_hFONT[ FONT_NORMAL ], true, 59,109, g_dwWHITE, CClan::GetInstance().GetName() );		

	//����	
	::drawFontf( g_GameDATA.m_hFONT[ FONT_NORMAL_BOLD ], true, 59,172, D3DCOLOR_XRGB( 255, 255, 0 ), "%d", g_pAVATAR->Get_LEVEL());		
	//����ġ	
	::drawFontf( g_GameDATA.m_hFONT[ FONT_NORMAL ], true, 59,193, D3DCOLOR_XRGB( 0, 210, 255), "%d / %d", g_pAVATAR->Get_EXP(), g_pAVATAR->Get_NeedEXP(g_pAVATAR->Get_LEVEL()) );
}

void CCharacterDLG::DrawBasicInfo2()
{
	///�̸�
	CWinCtrl * pCtrl = NULL;

	if( pCtrl = Find("NAME") )
	{
		((CTImage*)pCtrl)->SetText( g_pAVATAR->Get_NAME() );		
	}
	if( pCtrl = Find("LV") )
	{
		((CTImage*)pCtrl)->SetText( CStr::Printf("%d",	g_pAVATAR->Get_LEVEL() ) );	
	}
	if( pCtrl = Find("JOB") )
	{
		((CTImage*)pCtrl)->SetText( CStr::Printf("%s", CStringManager::GetSingleton().GetJobName( g_pAVATAR->Get_JOB() ) ) );	
	}

	if( pCtrl = Find("CLAN_MARK") )
	{
		D3DVECTOR vCharPos = { (float)pCtrl->GetPosition().x,  (float)pCtrl->GetPosition().y,  0 };
		CClanMarkView::Draw( g_pAVATAR, vCharPos );
	}	

	if( pCtrl = Find("CLAN_NAME") )
	{
		if( CClan::GetInstance().GetName() )
		{
			((CTImage*)pCtrl)->SetText( CClan::GetInstance().GetName() );
		}		
	}
	
}


void CCharacterDLG::DrawAbilityInfo()
{
	D3DXMATRIX mat;	
	D3DXMatrixTranslation( &mat, (float)m_sPosition.x, (float)m_sPosition.y,0.0f);
	::setTransformSprite( mat );

	///��
	::drawFontf( g_GameDATA.m_hFONT[ FONT_NORMAL_BOLD ], true, 58, 67, D3DCOLOR_XRGB( 0, 255, 42), "%d",g_pAVATAR->Get_STR());		
	///��ø
	::drawFontf( g_GameDATA.m_hFONT[ FONT_NORMAL_BOLD ], true, 58, 88, D3DCOLOR_XRGB( 0, 255, 42), "%d",g_pAVATAR->Get_DEX());		
	///����
	::drawFontf( g_GameDATA.m_hFONT[ FONT_NORMAL_BOLD ], true, 58,109, D3DCOLOR_XRGB( 0, 255, 42), "%d",g_pAVATAR->Get_INT());		
	///����
	::drawFontf( g_GameDATA.m_hFONT[ FONT_NORMAL_BOLD ], true, 58,130, D3DCOLOR_XRGB( 0, 255, 42), "%d",g_pAVATAR->Get_CON());
	///�ŷ�
	::drawFontf( g_GameDATA.m_hFONT[ FONT_NORMAL_BOLD ], true, 58,151, D3DCOLOR_XRGB( 0, 255, 42), "%d",g_pAVATAR->Get_CHARM());
	///����
	::drawFontf( g_GameDATA.m_hFONT[ FONT_NORMAL_BOLD ], true, 58,172, D3DCOLOR_XRGB( 0, 255, 42), "%d",g_pAVATAR->Get_SENSE());


	///���ݷ�
	::drawFontf( g_GameDATA.m_hFONT[ FONT_NORMAL_BOLD ], true, 171,  67, g_dwWHITE, "%d",g_pAVATAR->Get_ATK() );				
	///����
	::drawFontf( g_GameDATA.m_hFONT[ FONT_NORMAL_BOLD ], true, 171,  88, g_dwWHITE, "%d",g_pAVATAR->Get_DEF() );
	///��������
	::drawFontf( g_GameDATA.m_hFONT[ FONT_NORMAL_BOLD ], true, 171, 109, g_dwWHITE, "%d",g_pAVATAR->Get_RES() );
	///���߷�
	::drawFontf( g_GameDATA.m_hFONT[ FONT_NORMAL_BOLD ], true, 171, 130, g_dwWHITE, "%d",g_pAVATAR->Get_HIT() );
	///ũ��Ƽ��
	::drawFontf( g_GameDATA.m_hFONT[ FONT_NORMAL_BOLD ], true, 171, 151, g_dwWHITE, "%d",g_pAVATAR->Get_CRITICAL() );
	///ȸ�Ƿ�
	::drawFontf( g_GameDATA.m_hFONT[ FONT_NORMAL_BOLD ], true, 171, 172, g_dwWHITE, "%d",g_pAVATAR->Get_AVOID() );
	///���ݼӵ�
	::drawFontf( g_GameDATA.m_hFONT[ FONT_NORMAL_BOLD ], true, 171, 193, g_dwWHITE, "%d",g_pAVATAR->GetCur_ATK_SPD() );
	///�̵��ӵ�
	::drawFontf( g_GameDATA.m_hFONT[ FONT_NORMAL_BOLD ], true, 171, 214, g_dwWHITE, "%.0f",g_pAVATAR->GetCur_MOVE_SPEED() );


	///����Ʈ
	::drawFontf( g_GameDATA.m_hFONT[ FONT_NORMAL_BOLD ], true, 69,211, D3DCOLOR_XRGB( 0, 255, 42),"%d", g_pAVATAR->Get_BonusPOINT());			
}

void CCharacterDLG::DrawAbilityInfo2()
{
	CWinCtrl * pCtrl00 = Find( "ATK" );
	CWinCtrl * pCtrl01 = Find( "ATK_VALUE" );
	CWinCtrl * pCtrl02 = Find( "STR" );
	CWinCtrl * pCtrl03 = Find( "STR_VALUE" );
	CWinCtrl * pCtrl10 = Find( "DEF" );
	CWinCtrl * pCtrlPoint = Find( "POINT" );

	if( !pCtrl00 || !pCtrl01 || !pCtrl02 || !pCtrl03 || !pCtrl10 || !pCtrlPoint )
	{
		return;
	}	

	RECT rt;
	D3DXMATRIX mat;	
	D3DXMatrixTranslation( &mat, (float)m_sPosition.x, (float)m_sPosition.y,0.0f);
	::setTransformSprite( mat );

	int iPosX = 0;
	int iPosY = pCtrl00->GetOffset().y;
	int iPosGapY = pCtrl10->GetOffset().y - pCtrl00->GetOffset().y;



	///���ݷ�	
	SetRect( &rt, pCtrl01->GetOffset().x, iPosY, pCtrl01->GetOffset().x + pCtrl01->GetWidth(), iPosY + pCtrl01->GetHeight() );
	::drawFontf( g_GameDATA.m_hFONT[ FONT_NORMAL ], true, &rt, g_dwWHITE, DT_RIGHT, "%d", g_pAVATAR->Get_ATK() );

	::drawFontf(	g_GameDATA.m_hFONT[ FONT_NORMAL ], true,
		pCtrl00->GetOffset().x, iPosY, D3DCOLOR_XRGB( 255, 255, 255),
		"%s", CStringManager::GetSingleton().GetString( 735 ) );

	///����
	SetRect( &rt, pCtrl01->GetOffset().x, iPosY+=iPosGapY, pCtrl01->GetOffset().x + pCtrl01->GetWidth(), iPosY + pCtrl01->GetHeight() );
	::drawFontf( g_GameDATA.m_hFONT[ FONT_NORMAL ], true, &rt, g_dwWHITE, DT_RIGHT, "%d", g_pAVATAR->Get_DEF() );

	::drawFontf(	g_GameDATA.m_hFONT[ FONT_NORMAL ], true,
		pCtrl00->GetOffset().x, iPosY, D3DCOLOR_XRGB( 255, 255, 255),
		"%s", CStringManager::GetSingleton().GetString( 736 ) );

	///��������
	SetRect( &rt, pCtrl01->GetOffset().x, iPosY+=iPosGapY, pCtrl01->GetOffset().x + pCtrl01->GetWidth(), iPosY + pCtrl01->GetHeight() );
	::drawFontf( g_GameDATA.m_hFONT[ FONT_NORMAL ], true, &rt, g_dwWHITE, DT_RIGHT, "%d",g_pAVATAR->Get_RES() );

	::drawFontf(	g_GameDATA.m_hFONT[ FONT_NORMAL ], true,
		pCtrl00->GetOffset().x, iPosY, D3DCOLOR_XRGB( 255, 255, 255),
		"%s", CStringManager::GetSingleton().GetString( 737 ) );

	///���߷�
	SetRect( &rt, pCtrl01->GetOffset().x, iPosY+=iPosGapY, pCtrl01->GetOffset().x + pCtrl01->GetWidth(), iPosY + pCtrl01->GetHeight() );
	::drawFontf( g_GameDATA.m_hFONT[ FONT_NORMAL ], true, &rt, g_dwWHITE, DT_RIGHT, "%d",g_pAVATAR->Get_HIT() );

	::drawFontf(	g_GameDATA.m_hFONT[ FONT_NORMAL ], true,
		pCtrl00->GetOffset().x, iPosY, D3DCOLOR_XRGB( 255, 255, 255),
		"%s", CStringManager::GetSingleton().GetString( 743 ) );

	///ũ��Ƽ��
	SetRect( &rt, pCtrl01->GetOffset().x, iPosY+=iPosGapY, pCtrl01->GetOffset().x + pCtrl01->GetWidth(), iPosY + pCtrl01->GetHeight() );
	::drawFontf( g_GameDATA.m_hFONT[ FONT_NORMAL ], true, &rt, g_dwWHITE, DT_RIGHT, "%d",g_pAVATAR->Get_CRITICAL() );

	::drawFontf(	g_GameDATA.m_hFONT[ FONT_NORMAL ], true,
		pCtrl00->GetOffset().x, iPosY, D3DCOLOR_XRGB( 255, 255, 255),
		"%s", CStringManager::GetSingleton().GetString( 741 ) );

	///ȸ�Ƿ�
	SetRect( &rt, pCtrl01->GetOffset().x, iPosY+=iPosGapY, pCtrl01->GetOffset().x + pCtrl01->GetWidth(), iPosY + pCtrl01->GetHeight() );
	::drawFontf( g_GameDATA.m_hFONT[ FONT_NORMAL ], true, &rt, g_dwWHITE, DT_RIGHT, "%d",g_pAVATAR->Get_AVOID() );

	::drawFontf(	g_GameDATA.m_hFONT[ FONT_NORMAL ], true,
		pCtrl00->GetOffset().x, iPosY, D3DCOLOR_XRGB( 255, 255, 255),
		"%s", CStringManager::GetSingleton().GetString( 742 ) );

	///���ݼӵ�
	SetRect( &rt, pCtrl01->GetOffset().x, iPosY+=iPosGapY, pCtrl01->GetOffset().x + pCtrl01->GetWidth(), iPosY + pCtrl01->GetHeight() );
	::drawFontf( g_GameDATA.m_hFONT[ FONT_NORMAL ], true, &rt, g_dwWHITE, DT_RIGHT, "%d",g_pAVATAR->GetCur_ATK_SPD() );

	::drawFontf(	g_GameDATA.m_hFONT[ FONT_NORMAL ], true,
		pCtrl00->GetOffset().x, iPosY, D3DCOLOR_XRGB( 255, 255, 255),
		"%s", CStringManager::GetSingleton().GetString( 739 ) );

	///�̵��ӵ�
	SetRect( &rt, pCtrl01->GetOffset().x, iPosY+=iPosGapY, pCtrl01->GetOffset().x + pCtrl01->GetWidth(), iPosY + pCtrl01->GetHeight() );
#ifdef __CAL_BUF2
	::drawFontf( g_GameDATA.m_hFONT[ FONT_NORMAL ], true, &rt, g_dwWHITE, DT_RIGHT, "%.0f", g_pAVATAR->Get_DefaultSPEED() );	
#else
	::drawFontf( g_GameDATA.m_hFONT[ FONT_NORMAL ], true, &rt, g_dwWHITE, DT_RIGHT, "%.0f", g_pAVATAR->GetCur_MOVE_SPEED() );	
#endif
	::drawFontf(	g_GameDATA.m_hFONT[ FONT_NORMAL ], true,
		pCtrl00->GetOffset().x, iPosY, D3DCOLOR_XRGB( 255, 255, 255),
		"%s", CStringManager::GetSingleton().GetString( 740 ) );



	iPosX = pCtrl03->GetOffset().x;
	iPosY = pCtrl03->GetOffset().y;
	int iWidth = pCtrl03->GetWidth();
	int iHeight = pCtrl03->GetHeight();	
	
	///��
	SetRect( &rt, iPosX, iPosY, iPosX + iWidth, iPosY + iHeight+1 );
	::drawFontf( g_GameDATA.m_hFONT[ FONT_NORMAL ], true, &rt, g_dwWHITE, DT_RIGHT|DT_VCENTER, "%d",g_pAVATAR->Get_STR() );	

	///��ø
	SetRect( &rt, iPosX, iPosY+=iPosGapY, iPosX + iWidth, iPosY + iHeight+1 );
	::drawFontf( g_GameDATA.m_hFONT[ FONT_NORMAL ], true, &rt, g_dwWHITE, DT_RIGHT|DT_VCENTER, "%d",g_pAVATAR->Get_DEX() );

	///����
	SetRect( &rt, iPosX, iPosY+=iPosGapY, iPosX + iWidth, iPosY + iHeight+1 );
	::drawFontf( g_GameDATA.m_hFONT[ FONT_NORMAL ], true, &rt, g_dwWHITE, DT_RIGHT|DT_VCENTER, "%d",g_pAVATAR->Get_INT() );

	///����
	SetRect( &rt, iPosX, iPosY+=iPosGapY, iPosX + iWidth, iPosY + iHeight+1 );
	::drawFontf( g_GameDATA.m_hFONT[ FONT_NORMAL ], true, &rt, g_dwWHITE, DT_RIGHT|DT_VCENTER, "%d",g_pAVATAR->Get_CON() );

	///�ŷ�
	SetRect( &rt, iPosX, iPosY+=iPosGapY, iPosX + iWidth, iPosY + iHeight+1 );
	::drawFontf( g_GameDATA.m_hFONT[ FONT_NORMAL ], true, &rt, g_dwWHITE, DT_RIGHT|DT_VCENTER, "%d",g_pAVATAR->Get_CHARM() );

	///����
	SetRect( &rt, iPosX, iPosY+=iPosGapY, iPosX + iWidth, iPosY + iHeight+1 );
	::drawFontf( g_GameDATA.m_hFONT[ FONT_NORMAL ], true, &rt, g_dwWHITE, DT_RIGHT|DT_VCENTER, "%d",g_pAVATAR->Get_SENSE() );


	//+��ư �������� �Һ�Ǵ� sp
	short nNeedPoint = 0;

	if( pCtrl03 = Find("STR_POINT") )
	{
		nNeedPoint = g_pAVATAR->Get_NeedPoint2AbilityUP( BA_STR );
		pCtrl03->SetText( CStr::Printf("%d", nNeedPoint) );
		pCtrl03->SetAlign( DT_RIGHT|DT_VCENTER );
		pCtrl03->SetTextColor( g_dwYELLOW );
	}
	if( pCtrl03 = Find("DEX_POINT") )
	{
		nNeedPoint = g_pAVATAR->Get_NeedPoint2AbilityUP( BA_DEX );
		pCtrl03->SetText( CStr::Printf("%d", nNeedPoint) );
		pCtrl03->SetAlign( DT_RIGHT|DT_VCENTER );
		pCtrl03->SetTextColor( g_dwYELLOW );
	}
	if( pCtrl03 = Find("INT_POINT") )
	{
		nNeedPoint = g_pAVATAR->Get_NeedPoint2AbilityUP( BA_INT );
		pCtrl03->SetText( CStr::Printf("%d", nNeedPoint) );
		pCtrl03->SetAlign( DT_RIGHT|DT_VCENTER );
		pCtrl03->SetTextColor( g_dwYELLOW );
	}
	if( pCtrl03 = Find("CON_POINT") )
	{
		nNeedPoint = g_pAVATAR->Get_NeedPoint2AbilityUP( BA_CON );
		pCtrl03->SetText( CStr::Printf("%d", nNeedPoint) );
		pCtrl03->SetAlign( DT_RIGHT|DT_VCENTER );
		pCtrl03->SetTextColor( g_dwYELLOW );
	}
	if( pCtrl03 = Find("CRM_POINT") )
	{
		nNeedPoint = g_pAVATAR->Get_NeedPoint2AbilityUP( BA_CHARM );
		pCtrl03->SetText( CStr::Printf("%d", nNeedPoint) );
		pCtrl03->SetAlign( DT_RIGHT|DT_VCENTER );
		pCtrl03->SetTextColor( g_dwYELLOW );
	}
	if( pCtrl03 = Find("SEN_POINT") )
	{
		nNeedPoint = g_pAVATAR->Get_NeedPoint2AbilityUP( BA_SENSE );
		pCtrl03->SetText( CStr::Printf("%d", nNeedPoint) );
		pCtrl03->SetAlign( DT_RIGHT|DT_VCENTER );
		pCtrl03->SetTextColor( g_dwYELLOW );
	}


	iPosX = pCtrl02->GetOffset().x;
	iPosY = pCtrl02->GetOffset().y;
	///��
	::drawFontf(	g_GameDATA.m_hFONT[ FONT_NORMAL ], true,
		iPosX, iPosY, D3DCOLOR_XRGB( 255, 255, 255),
		"%s", CStringManager::GetSingleton().GetString( 744 ) );

	///��ø
	::drawFontf(	g_GameDATA.m_hFONT[ FONT_NORMAL ], true,
		iPosX, iPosY+=iPosGapY, D3DCOLOR_XRGB( 255, 255, 255),
		"%s", CStringManager::GetSingleton().GetString( 745 ) );
	///����
	::drawFontf(	g_GameDATA.m_hFONT[ FONT_NORMAL ], true,
		iPosX, iPosY+=iPosGapY, D3DCOLOR_XRGB( 255, 255, 255),
		"%s", CStringManager::GetSingleton().GetString( 746 ) );
	///����
	::drawFontf(	g_GameDATA.m_hFONT[ FONT_NORMAL ], true,
		iPosX, iPosY+=iPosGapY, D3DCOLOR_XRGB( 255, 255, 255),
		"%s", CStringManager::GetSingleton().GetString( 747 ) );
	///�ŷ�
	::drawFontf(	g_GameDATA.m_hFONT[ FONT_NORMAL ], true,
		iPosX, iPosY+=iPosGapY, D3DCOLOR_XRGB( 255, 255, 255),
		"%s", CStringManager::GetSingleton().GetString( 748 ) );
	///����
	::drawFontf(	g_GameDATA.m_hFONT[ FONT_NORMAL ], true,
		iPosX, iPosY+=iPosGapY, D3DCOLOR_XRGB( 255, 255, 255),
		"%s", CStringManager::GetSingleton().GetString( 749 ) );
	
	if( pCtrl00 = Find("POINT") )
	{
		pCtrl00->SetText( CStr::Printf( "%d", g_pAVATAR->Get_BonusPOINT() ) );
	}
}


void CCharacterDLG::DrawUnionInfo()
{
	D3DXMATRIX mat;	
	D3DXMatrixTranslation( &mat, (float)m_sPosition.x, (float)m_sPosition.y,0.0f);
	::setTransformSprite( mat );

	///Ŭ��
	::drawFont( g_GameDATA.m_hFONT[ FONT_NORMAL_BOLD ], true, 90,67, g_dwCOLOR[ UNION_COLOR(g_pAVATAR->Get_UNION()) ],  UNION_NAME(g_pAVATAR->Get_UNION()) );

	int iAbility = AT_UNION_POINT1;
	POINT pt;
	for( int i = 0 ; i < 2; ++i )
	{
		pt.x = 50 + i * 113;
		for( int j = 0 ; j < 5; ++j )
		{
			pt.y = 130 + j * 21;
			::drawFontf( g_GameDATA.m_hFONT[ FONT_NORMAL_BOLD ], true, pt.x, pt.y, g_dwWHITE, "%d",g_pAVATAR->Get_AbilityValue( iAbility ) );		
			++iAbility;
		}
	}
}


void CCharacterDLG::DrawUnionInfo2()
{

	CWinCtrl * pCtrl = NULL;
	
	if( pCtrl = Find( "UNION_NAME" ) )
	{		
		((CTImage*)pCtrl)->SetTextColor( g_dwWHITE );
		((CTImage*)pCtrl)->SetText( UNION_NAME(g_pAVATAR->Get_UNION()) );
	}
	if( pCtrl = Find( "UNION_FAME_VALUE" ) )
	{		
		((CTImage*)pCtrl)->SetTextColor( g_dwWHITE );
		((CTImage*)pCtrl)->SetText( CStr::Printf( "%d", g_pAVATAR->Get_FAME() ) );
	}	


	for( int i=0; i< 5; i++ )
	{

		if( pCtrl = Find( CStr::Printf( "MARKVALUE_%02d", i ) ) )
		{		
			((CTImage*)pCtrl)->SetTextColor( g_dwWHITE );
			pCtrl->SetAlign( DT_VCENTER|DT_CENTER );
			((CTImage*)pCtrl)->SetText( CStr::Printf( "%d", g_pAVATAR->Get_AbilityValue( AT_UNION_POINT1 + i ) ) );
		}
	}	
	
}
bool CCharacterDLG::Create( const char* IDD )
{
	if( CTDialog::Create( IDD ) )
	{	
		return true;
	}
	return false;
}

void CCharacterDLG::SetInterfacePos_After()
{
	CWinCtrl * pCtrl = NULL;	

	if( pCtrl = Find("BG_00") )
	{
		pCtrl->SetSizeFit(true);
	}	

	if( pCtrl = Find("NAME_TXT") )
	{
		//�̸� 775
		((CTImage*)pCtrl)->SetText( LIST_STRING(755) );
	}
	if( pCtrl = Find("JOB_TXT") )
	{
		//���� 813
		((CTImage*)pCtrl)->SetText( LIST_STRING(813) );
	}
	if( pCtrl = Find("CLAN_TXT") )
	{
		((CTImage*)pCtrl)->SetText( STR_CLAN_NAME );
	}
	if( pCtrl = Find("LV_TXT") )
	{
		//���� 313
		((CTImage*)pCtrl)->SetText( LIST_STRING(313) );
		((CTImage*)pCtrl)->SetFont( FONT_NORMAL );				
	}


	if( pCtrl = Find("HP") )
	{
		//hp
		((CTImage*)pCtrl)->SetText( LIST_STRING(833) );
		((CTImage*)pCtrl)->SetFont( FONT_NORMAL );
		((CTImage*)pCtrl)->SetTextColor( g_dwWHITE );
	}
	if( pCtrl = Find("MP") )
	{
		//mp
		((CTImage*)pCtrl)->SetText( LIST_STRING(834) );
		((CTImage*)pCtrl)->SetFont( FONT_NORMAL );
		((CTImage*)pCtrl)->SetTextColor( g_dwWHITE );
	}
	if( pCtrl = Find("EXP") )
	{
		//������ 814
		((CTImage*)pCtrl)->SetText( LIST_STRING(814) );
		((CTImage*)pCtrl)->SetFont( FONT_NORMAL );		
	}

	if( pCtrl = Find("NAME") )
	{		
		((CTImage*)pCtrl)->SetFont( FONT_NORMAL );
		((CTImage*)pCtrl)->SetTextColor( g_dwYELLOW );
	}
	if( pCtrl = Find("LV_JOB") )
	{		
		((CTImage*)pCtrl)->SetFont( FONT_NORMAL );
		((CTImage*)pCtrl)->SetTextColor( D3DCOLOR_XRGB( 251, 255, 151) );
	}

	if( pCtrl = Find("CLAN_NAME") )
	{		
		((CTImage*)pCtrl)->SetFont( FONT_NORMAL );
		((CTImage*)pCtrl)->SetTextColor( g_dwWHITE );
	}
	if( pCtrl = Find("EXP_VALUE") )
	{
		((CTImage*)pCtrl)->SetFont( FONT_NORMAL );
		((CTImage*)pCtrl)->SetTextColor( D3DCOLOR_XRGB( 251, 255, 151) );
		((CTImage*)pCtrl)->SetAlign( DT_VCENTER | DT_RIGHT );
	}

	if( pCtrl = Find("POINT_TXT") )
	{
		((CTImage*)pCtrl)->SetText( CStringManager::GetSingleton().GetString( 750 ) );		
		((CTImage*)pCtrl)->SetFont( FONT_SMALL );
		((CTImage*)pCtrl)->SetTextColor( D3DCOLOR_XRGB( 253, 156, 2) );		
	}

	if( pCtrl = Find("POINT") )
	{	
		((CTImage*)pCtrl)->SetAlign( DT_VCENTER | DT_CENTER );		
	}


	if( pCtrl = Find("BASIC_INFO_TXT") )
	{
		//�⺻���� 835
		((CTImage*)pCtrl)->SetText( LIST_STRING(835) );
		((CTImage*)pCtrl)->SetAlign( DT_VCENTER | DT_LEFT );		
	}
	if( pCtrl = Find("STATUS_INFO_TXT") )
	{
		//�ƹ�Ÿ �ɷ�ġ 836
		((CTImage*)pCtrl)->SetText( LIST_STRING(836) );
		((CTImage*)pCtrl)->SetAlign( DT_VCENTER | DT_LEFT );		
	}
	if( pCtrl = Find("UNION_INFO_TXT") )
	{
		//�������� 837
		((CTImage*)pCtrl)->SetText( LIST_STRING(837) );
		((CTImage*)pCtrl)->SetAlign( DT_VCENTER | DT_LEFT );		
	}

	
	

	if( pCtrl = Find("MARK_00") )
	{
		((CTImage*)pCtrl)->SetImage( 91, IMAGE_RES_STATE_ICON );
	}
	if( pCtrl = Find("MARK_01") )
	{
		((CTImage*)pCtrl)->SetImage( 92, IMAGE_RES_STATE_ICON );
	}
	if( pCtrl = Find("MARK_02") )
	{
		((CTImage*)pCtrl)->SetImage( 93, IMAGE_RES_STATE_ICON );
	}
	if( pCtrl = Find("MARK_03") )
	{
		((CTImage*)pCtrl)->SetImage( 94, IMAGE_RES_STATE_ICON );
	}
	if( pCtrl = Find("MARK_04") )
	{
		((CTImage*)pCtrl)->SetImage( 95, IMAGE_RES_STATE_ICON );
	}	
	if( pCtrl = Find( "UNION" ) )
	{
		((CTImage*)pCtrl)->SetTextColor( g_dwWHITE );
		((CTImage*)pCtrl)->SetText( LIST_STRING(762) );//�Ҽ� ����
	}
	if( pCtrl = Find( "UNION_FAME" ) )
	{
		((CTImage*)pCtrl)->SetTextColor( g_dwWHITE );
		((CTImage*)pCtrl)->SetText( LIST_STRING(763) );//���� ��ġ
	}

	//ĳ�������� 825
	GetCaption()->SetString( LIST_STRING(825) );

	MoveWindow( GetPosition() );
}
