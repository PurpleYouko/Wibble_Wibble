#include "stdafx.h"
#include "ClanWar_BossDlg.h"

#include "it_mgr.h"
#include "./Network/CNetwork.h"
#include "TGuage.h"

CClanWar_BossDlg::CClanWar_BossDlg()
{
	SetDialogType( DLG_TYPE_CLANWAR_BOSS );
	Init();
}


CClanWar_BossDlg::CClanWar_BossDlg( int iDlgType )
{
	SetDialogType( iDlgType );
	Init();
}

CClanWar_BossDlg::~CClanWar_BossDlg(void)
{
}

void CClanWar_BossDlg::Init()
{
	m_iHp = 0;
	m_iMaxHp = 0;

	g_itMGR.AppendDlg( GetDialogType(), this, GetControlID() );
	Clear();
	Create("DlgClanWarBoss");
	SetInterfacePos_After();
	g_itMGR.InitInterfacePos(this);
}

void CClanWar_BossDlg::Show()
{
	if( IsVision() )
	{
		return;
	}	
	CTDialog::Show();
}

void CClanWar_BossDlg::Hide()
{
	g_itMGR.DeleteDlg(this);
}

void CClanWar_BossDlg::SetInterfacePos_After()
{
	CTDialog::SetInterfacePos_After();
	
	CTGuage* pGuage = (CTGuage*)Find( "BOSS_HP" );
	if( pGuage )
	{
		pGuage->SetSizeFit(true);
	}

	CWinCtrl * pCtrl = NULL;
}

void CClanWar_BossDlg::Set_Boss(gsv_cli_CLAN_WAR_BOSS_HP * pBossHp)
{
	CWinCtrl * pCtrl = NULL;
	
	if(pBossHp==NULL)
	{
		return;
	}
	
	if( g_pAVATAR->Get_TeamNO() == 1000 )
	{
		m_iHp		= pBossHp->m_A_Boss_HP;
		m_iMaxHp	= pBossHp->m_A_Boss_Max_HP;

		CObjCHAR *pCHAR = (CObjCHAR*)g_pObjMGR->Get_ClientOBJECT( pBossHp->m_A_nCharIdx );
		if( pCHAR )
		{
			m_strBossName = pCHAR->Get_NAME();
		}
		
		if(pCtrl = Find("BOSS_HEAD_B"))
		{
			pCtrl->Hide();
		}		
	}
	else if( g_pAVATAR->Get_TeamNO() == 2000 )
	{
		m_iHp		= pBossHp->m_B_Boss_HP;
		m_iMaxHp	= pBossHp->m_B_Boss_Max_HP;
		
		CObjCHAR *pCHAR = (CObjCHAR*)g_pObjMGR->Get_ClientOBJECT( pBossHp->m_B_nCharIdx );
		if( pCHAR )
		{
			m_strBossName = pCHAR->Get_NAME();
		}
		if(pCtrl = Find("BOSS_HEAD_A"))
		{
			pCtrl->Hide();
		}
	}

	

	if(pCtrl = Find("BOSS_NAME_TXT"))		
	{
		pCtrl->SetText(m_strBossName.c_str());
		pCtrl->SetAlign(DT_VCENTER | DT_LEFT);
	}

	//º¸½º¸÷ÀÇ hp°¡ 0 ÀÌ¸é Ã¢ ´Ý±â.
	if(m_iHp == 0)
	{
		Hide();
		return;
	}

}

void CClanWar_BossDlg::Update( POINT ptMouse )
{
	if( !IsVision() ) return;

	int   iPer		= 0;	
	int   iValue	= 0;
	int	  iMax		= 1;
	char*	pszBuf;

	CTGuage* pGuage = (CTGuage*)Find( "BOSS_HP" );
	if( pGuage )
	{
		if ( m_iHp < 0 ) 	m_iHp = 0;
		
		if( m_iMaxHp != 0 )
		{
			iPer = (int)((float) m_iHp * 1000  / m_iMaxHp);
			pGuage->SetValue( iPer );

			/*
			pszBuf = CStr::Printf( "%d/%d", iValue, iMax);
			pGuage->SetText( pszBuf );
			pGuage->SetFont( FONT_NORMAL_OUTLINE );
			pGuage->SetAlign( DT_CENTER || DT_VCENTER );
			*/
		}
	}
}


unsigned CClanWar_BossDlg::Process( unsigned uiMsg, WPARAM wParam, LPARAM lParam )
{
	if( !IsVision() ) return 0;
	
	return 0;

	if( unsigned uiProcID = CTDialog::Process( uiMsg, wParam, lParam ) )
	{
		switch( uiMsg )
		{
		case WM_LBUTTONUP:
			{
				switch( uiProcID )
				{
				case 10:
					Hide();
					break;
				default:
					break;
				}
				break;
			}
		default:
			break;
		}
		return uiMsg;
	}
	return 0;
}


void CClanWar_BossDlg::Draw()
{
	if( !IsVision() ) return;
	CTDialog::Draw();
}

