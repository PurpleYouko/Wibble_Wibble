#include "stdafx.h"
#include ".\cclandlg.h"
#include "../../Game.h"
#include "../CTDrawImpl.h"
#include "../IO_ImageRes.h"
#include "../it_mgr.h"
#include "../TypeResource.h"
#include "../../GameData/Event/CTEventClan.h"
#include "../../GameData/CClan.h"
#include "subclass/ClanMemberItem.h"
#include "subclass/CClanSkillListItem.h"
#include "../icon/ciconskillclan.h"
#include "../GameCommon/StringManager.h"
#include "../Network/CNetwork.h"
#include "../JCommandState.h"
#include "../Command/UICommand.h"
#include "../../IO_Terrain.h"
#include "../ClanMarkTransfer.h"
#include "../../Country.h"
#include "../misc/gameutil.h"

#include "TEditBox.h"
#include "TButton.h"
#include "TabbedPane.h"
#include "ZListBox.h"
#include "JContainer.h"
#include "ResourceMgr.h"
#include "../CClanMarkView.h"
#include "TListBox.h"



CClanDlg::CClanDlg(void)
{
	m_iTab = TAB_INFO;
	m_pListBox_Slogan = NULL;
}

CClanDlg::~CClanDlg(void)
{

}

void CClanDlg::Show()
{

#ifdef _DEBUG

/*	POINT pt = { GetPosition().x, GetPosition().y };
	Clear();
	Create("DlgClan");		
	SetInterfacePos_After();
	MoveWindow(pt);
	g_pNet->Send_cli_CLAN_COMMAND( GCMD_ROSTER, NULL );
	m_iTab = TAB_INFO;

	CClan::GetInstance().SetSlogan( CClan::GetInstance().GetSlogan() );
*/
#endif

	CTDialog::Show();
	///내가 마스터 이상이고 등록된 마크 이미지를 사용한다면
	SYSTEMTIME clanmark_regtime = CClan::GetInstance().GetClanMarkRegTime();
	if( g_pAVATAR && g_pAVATAR->GetClanPos() >= CClan::CLAN_MASTER && g_pAVATAR->GetClanMarkBack() == 0 && clanmark_regtime.wYear == 0 )
		g_pNet->Send_cli_CLANMARK_REG_TIME();
}

void CClanDlg::Draw()
{
	if( !IsVision() ) return;
	CTDialog::Draw();

	D3DVECTOR vCharPos = m_vCharPos;
	vCharPos.x += (float)GetPosition().x;
	vCharPos.y += (float)GetPosition().y;

	switch( m_iTab )
	{
	case TAB_INFO:		
		CClanMarkView::Draw( g_pAVATAR, vCharPos );
		DrawInfomation2();		
		m_ClanMarkPreView.Draw( vCharPos );
		break;

	case TAB_MEMBER:	
		CClanMarkView::Draw( g_pAVATAR, vCharPos );		
		DrawMembers2();		
		break;

	case TAB_SKILL:		
		if( !IsApplyNewVersion() )
		{
			CClanMarkView::Draw( g_pAVATAR, vCharPos );		
			DrawSkills2();			
		}
		break;
	case TAB_NOTICE:	
		CClanMarkView::Draw( g_pAVATAR, vCharPos );		
		DrawNotice2();		
		break;
	default:
		break;
	}
}

unsigned CClanDlg::Process( unsigned uiMsg, WPARAM wParam, LPARAM lParam )
{
	if( !IsVision() ) return 0;
	if( unsigned uiProcID = CTDialog::Process( uiMsg, wParam, lParam ))
	{
		switch( uiMsg )
		{
		case WM_LBUTTONDOWN:
			OnLButtonDown( uiProcID, wParam , lParam );
			break;
		case WM_LBUTTONUP:
			OnLButtonUp( uiProcID, wParam, lParam );
			break;
		default:
			break;
		}
		return uiMsg;
	}
	return 0;
}

void CClanDlg::OnLButtonDown( unsigned uiProcID, WPARAM wParam, LPARAM lParam )
{
	switch( uiProcID )
	{

	case IID_BTN_TAB_INFO:
		m_iTab = TAB_INFO;
		break;
	case IID_BTN_TAB_MEMBER:
		m_iTab = TAB_MEMBER;
		break;
	case IID_BTN_TAB_SKILL:
		m_iTab = TAB_SKILL;
		break;
	case IID_BTN_TAB_NOTICE:
		m_iTab = TAB_NOTICE;
		break;
	default:
		break;
	}
}

void CClanDlg::OnLButtonUp( unsigned uiProcID, WPARAM wParam, LPARAM lParam )
{
	switch( uiProcID )
	{
	case IID_BTN_PREVIEW:
		{
			m_ClanMarkPreView.Free();
			///Mark.bmp를 로드해서 CClanMarkUserDefined에 넣는다.
			///CClanMarkTransfer::RegisterMarkToServer()와 비슷한 체크루틴이 필요하다.
			HANDLE hBmpFile = CreateFile( CClanMarkUserDefined::NewClanMarkFileName.c_str(), GENERIC_READ,FILE_SHARE_READ, NULL,OPEN_EXISTING,FILE_ATTRIBUTE_READONLY,NULL );
			if( hBmpFile == INVALID_HANDLE_VALUE )
			{
				//DWORD dwErr = GetLastError();
				//_RPTF1( _CRT_ASSERT,"CreateFile Error : Bmp File - Invalid_handle_value(%d)", dwErr );
				g_itMGR.OpenMsgBox( CStr::Printf("%s %s",STR_CLANMARK_FILE_NOTFOUND , STR_CLANMARK_HELP_HOMEPAGE) );
				return;
			}

			if( false == CClanMarkTransfer::GetSingleton().Check_BmpFile( hBmpFile, CClanMarkUserDefined::ClanMarkSize ) )
			{
				//g_itMGR.OpenMsgBox( CStr::Printf("%s %s",STR_CLANMARK_INVALID_FORMAT , STR_CLANMARK_HELP_HOMEPAGE) );			
				CloseHandle( hBmpFile );
				break;
			}

			CloseHandle( hBmpFile );
			HNODE hNode = loadTexture( CClanMarkUserDefined::NewClanMarkFileName.c_str(), CClanMarkUserDefined::NewClanMarkFileName.c_str(), 1,  0 );
			m_ClanMarkPreView.SetNode( hNode );
		}
		break;
	case IID_BTN_REGIST_CLANMARK:
		if( CClan::GetInstance().GetClanNo() )
		{
			if(  CClan::GetInstance().GetClass() >= CClan::CLAN_MASTER )
			{
				//CClanMarkTransfer::GetSingleton().RegisterMarkToServer( g_pAVATAR->GetClanID(), CClanMarkUserDefined::NewClanMarkFileName.c_str() );
				CTCommand* pCmdOk = new CTCmdRegisterClanMark( g_pAVATAR->GetClanID(), CClanMarkUserDefined::NewClanMarkFileName.c_str() );
				g_itMGR.OpenMsgBox( CStr::Printf("%s %s",STR_QUERY_CHANGE_CLANMARK,STR_CLANMARK_UPDATE_ERROR), CMsgBox::BT_OK | CMsgBox::BT_CANCEL, true, 0, pCmdOk, 0 );
			}
			else
				g_itMGR.OpenMsgBox( STR_CLAN_NO_RIGHT );
		}
		break;
	case IID_BTN_CLOSE:
		Hide();
		break;
	case IID_BTN_ICONIZE:
		g_itMGR.AddDialogIcon( 	GetDialogType() );
		break;
	case IID_BTN_REGIST_NOTICE:
		{
			CClan& Clan = CClan::GetInstance();
			if( Clan.GetClanNo() )
			{
				///클랜 마스터일경우에만
				if( CClan::GetInstance().GetClass() >= CClan::CLAN_MASTER )
					g_itMGR.OpenDialog( DLG_TYPE_CLAN_NOTICE );
				else
					g_itMGR.OpenMsgBox( STR_CLAN_NO_RIGHT );
			}
		}
		break;
	case IID_BTN_DELETE_NOTICE:
		///클랜 마스터일경우에만
		break;
	case IID_BTN_WITHDRAWAL:
		{
			if( g_pTerrain->IsAgitZone() )
			{
				g_itMGR.OpenMsgBox( STR_CANT_SKILL_IN_AGIT );
				break;
			}

			//20061206 홍근 : 클랜전에서 클랜강퇴, 클랜탈퇴 비활성화.
			if( g_pTerrain->GetZoneNO()>=101 && g_pTerrain->GetZoneNO()<=150 )
			{
				break;
			}

			CClan& Clan = CClan::GetInstance();
			if( Clan.GetClanNo() && !g_pTerrain->IsPVPZone() )
			{
				if( CClan::GetInstance().GetClass() < CClan::CLAN_MASTER )
				{
					// 클랜 마스터가 아닐경우에만
					CTCmdClanCommand* pCmd = new CTCmdClanCommand( GCMD_QUIT, NULL );
					g_itMGR.OpenMsgBox( STR_CLAN_QUERY_BAN_S, CMsgBox::BT_OK | CMsgBox::BT_CANCEL, true, 0, pCmd );
					// RESULT_CLAN_QUIT packet 에서 처리
					//g_pNet->Send_cli_CLAN_COMMAND( GCMD_QUIT , NULL );
					//CClan::GetInstance().Clear();
					//g_pAVATAR->ResetClan();
				}
				else
				{
					g_itMGR.OpenMsgBox( STR_CLAN_CANT_QUIT_CLANMASTER );
				}
			}
		}
		break;
	case IID_BTN_ENTRUST:
		{
#ifdef __NEWUPDATEPARTYBUFF
			//홍근 : 클랜전 신청상태에서 위임 금지. 오후 3:24 2006-10-25
			//홍근 : 6번 체크 삭제 20070508
			if( g_pAVATAR->m_Quests.m_nClanWarVAR[0] > 0 
				/*|| g_pAVATAR->m_Quests.m_nClanWarVAR[6] > 0*/ )
			{
				//g_itMGR.OpenMsgBox( "클랜전 신청중에는 위임을 할 수 없습니다." );
				break;
			}
#endif
			CClan& Clan = CClan::GetInstance();
			if( Clan.GetClanNo() && !g_pTerrain->IsPVPZone() )
			{
				if( CClan::GetInstance().GetClass() >= CClan::CLAN_MASTER )
				{
					CClanMemberItem* pMember = GetSelectedMember();
					if( pMember != NULL && _strcmpi( pMember->GetName(), g_pAVATAR->Get_NAME() )) 
					{
						if( pMember->GetClass() >= CClan::CLAN_COMMANDER )
						{
							CTCmdClanCommand* pCmd = new CTCmdClanCommand( GCMD_LEADER, pMember->GetName() );

							g_itMGR.OpenMsgBox( CStr::Printf( STR_CLAN_QUERY_ENTRUST, pMember->GetName() ), 
								CMsgBox::BT_OK | CMsgBox::BT_CANCEL, true, 0, pCmd );

						}
						else
						{
							g_itMGR.OpenMsgBox( STR_CLAN_CANT_ENTRUST_TO_LOW_CLASS );
						}
					}
				}
				else
				{
					g_itMGR.OpenMsgBox( STR_CLAN_NO_RIGHT );
				}
			}
		}
		break;
	case IID_BTN_BAN:
		{
			if( g_pTerrain->IsAgitZone() )
			{
				g_itMGR.OpenMsgBox( STR_CANT_SKILL_IN_AGIT );
				break;
			}

			//20061206 홍근 : 클랜전에서 클랜강퇴, 클랜탈퇴 비활성화.
			if( g_pTerrain->GetZoneNO()>=101 && g_pTerrain->GetZoneNO()<=150 )
			{
				break;
			}

			CClan& Clan = CClan::GetInstance();
			if( Clan.GetClanNo() && !g_pTerrain->IsPVPZone() )
			{
				if( CClan::GetInstance().GetClass() >= CClan::CLAN_MASTER )
				{
					CClanMemberItem* pMember = GetSelectedMember();
					if( pMember != NULL &&  _strcmpi( pMember->GetName(), g_pAVATAR->Get_NAME() ))
					{
						CTCmdClanCommand* pCmd = new CTCmdClanCommand( GCMD_REMOVE, pMember->GetName() );
						g_itMGR.OpenMsgBox( CStr::Printf( STR_CLAN_QUERY_BAN_M, pMember->GetName() ), 
							CMsgBox::BT_OK | CMsgBox::BT_CANCEL, true, 0, pCmd );
					}
				}
				else
				{
					g_itMGR.OpenMsgBox( STR_CLAN_NO_RIGHT );
				}
			}
		}
		break;
	case IID_BTN_CLASS_UP:
		{
			CClanMemberItem* pMember = GetSelectedMember();
			if( pMember != NULL &&  _strcmpi( pMember->GetName(), g_pAVATAR->Get_NAME() ) && !g_pTerrain->IsPVPZone() )
			{
				if( CClan::GetInstance().GetClass() >= CClan::CLAN_MASTER )
				{
					if( CClan::GetInstance().IsValidClassUp( pMember->GetClass() ) )
					{
						CTCmdClanCommand* pCmd = new CTCmdClanCommand( GCMD_PROMOTE, pMember->GetName() );
						g_itMGR.OpenMsgBox( CStr::Printf( STR_CLAN_QUERY_PROMOTE, pMember->GetName() ), 
							CMsgBox::BT_OK | CMsgBox::BT_CANCEL, true, 0, pCmd );
					}
					else
					{
						g_itMGR.OpenMsgBox( 
							CStr::Printf( STR_CLAN_CANT_PROMOTE, CStringManager::GetSingleton().GetClanClass( pMember->GetClass() + 1 ) )
						);
					}
				}
				else
				{
					g_itMGR.OpenMsgBox( STR_CLAN_NO_RIGHT );
				}
			}
		}
		break;
	case IID_BTN_CLASS_DOWN:
		{
			CClanMemberItem* pMember = GetSelectedMember();
			if( pMember &&  _strcmpi( pMember->GetName(), g_pAVATAR->Get_NAME() ) && !g_pTerrain->IsPVPZone() )
			{
				if( CClan::GetInstance().GetClass() >= CClan::CLAN_MASTER )
				{
					if( pMember->GetClass() >= 0 )
					{
						CTCmdClanCommand* pCmd = new CTCmdClanCommand( GCMD_DEMOTE, pMember->GetName() );
						g_itMGR.OpenMsgBox( CStr::Printf( STR_CLAN_QUERY_DEMOTE, pMember->GetName() ), 
							CMsgBox::BT_OK | CMsgBox::BT_CANCEL, true, 0, pCmd );
					}
				}
				else
				{
					g_itMGR.OpenMsgBox( STR_CLAN_NO_RIGHT );
				}
			}
		}
		break;
	case IID_BTN_REQJOIN:
		{

			CClan& Clan = CClan::GetInstance();
			
			if( Clan.GetClanNo() && !g_pTerrain->IsPVPZone() )
			{
				if( Clan.GetClass() >= CClan::CLAN_SUB_MASTER )
				{
					CObjAVT* pAvt = g_pObjMGR->Get_ClientCharAVT( 
						g_pObjMGR->Get_ServerObjectIndex(g_UserInputSystem.GetCurrentTarget()), 
						false 
					);

					if( pAvt )
					{
						if( Clan.IsValidJoinMember(pAvt->Get_NAME()))  //클랜 정원수와 내클랜에 이미 가입된 유저여부판단. (허재영 2005.11.14)
							g_pNet->Send_cli_CLAN_COMMAND( GCMD_INVITE , pAvt->Get_NAME() );
						else
							g_itMGR.OpenMsgBox( STR_CLAN_CANT_MORE_JOINMEMBER );
					}
				}
				else
				{
					g_itMGR.OpenMsgBox( STR_CLAN_RESULT_CLAN_JOIN_NO_RIGHT );
				}
			}
		}
		break;
	default:
		break;
	}
}

void CClanDlg::DrawInfomation()
{
	CClan& Clan = CClan::GetInstance();
	
	if( Clan.GetClanNo() )
	{
		D3DXMATRIX mat;	
		D3DXMatrixTranslation( &mat, (float)m_sPosition.x, (float)m_sPosition.y,0.0f);
		::setTransformSprite( mat );
	
		::drawFont( g_GameDATA.m_hFONT[ FONT_NORMAL ], true, 15, 73, g_dwBLACK, STR_CLAN_NAME);
		::drawFont( g_GameDATA.m_hFONT[ FONT_NORMAL ], true, 88, 73, g_dwWHITE, Clan.GetName());

		::drawFont( g_GameDATA.m_hFONT[ FONT_NORMAL ], true, 15, 94, g_dwBLACK, STR_CLAN_LEV);
		::drawFontf( g_GameDATA.m_hFONT[ FONT_NORMAL ], true, 88, 94, g_dwWHITE, "%d" ,Clan.GetLevel() );

		::drawFont( g_GameDATA.m_hFONT[ FONT_NORMAL ], true, 15,115, g_dwBLACK, STR_CLAN_POINT);
		::drawFontf( g_GameDATA.m_hFONT[ FONT_NORMAL ], true, 88,115, g_dwWHITE, "%d", Clan.GetPoint());		

		::drawFont( g_GameDATA.m_hFONT[ FONT_NORMAL ], true, 15, 135, g_dwBLACK, STR_CLAN_SLOGAN);

		RECT rcDraw = { 88, 135, 210, 180 };
		::drawFont( g_GameDATA.m_hFONT[ FONT_NORMAL ], true, &rcDraw, g_dwWHITE, DT_WORDBREAK,Clan.GetSlogan() );		

		::drawFont( g_GameDATA.m_hFONT[ FONT_NORMAL ], true, 15, 187, g_dwBLACK, STR_CLAN_MONEY );
		::drawFontf( g_GameDATA.m_hFONT[ FONT_NORMAL ], true, 88, 187, g_dwWHITE,"%d", Clan.GetMoney());




		::drawFont( g_GameDATA.m_hFONT[ FONT_NORMAL ], true, 15, 208, g_dwBLACK, STR_CLAN_MEMBER_COUNT);
		SetRect( &rcDraw, 88, 208, 210, 230 );
		::drawFontf( g_GameDATA.m_hFONT[ FONT_NORMAL ], true, &rcDraw, g_dwWHITE, DT_CENTER, "%d / %d", Clan.GetMemberCount(), Clan.GetMemberMaxCount() );		


		::drawFont( g_GameDATA.m_hFONT[ FONT_NORMAL ], true, 15, 229, g_dwBLACK, STR_CLANMARK_REGISTER_TIME);
		if( Clan.GetClanNo() && Clan.GetClass() >= CClan::CLAN_MASTER && g_pAVATAR->GetClanMarkBack() == 0 )
		{
			SYSTEMTIME clanmark_regtime = Clan.GetClanMarkRegTime();
			::drawFontf( g_GameDATA.m_hFONT[ FONT_NORMAL ], true, 88, 229, g_dwWHITE, "%d/%d/%d %d:%02d", 
				clanmark_regtime.wYear,clanmark_regtime.wMonth, clanmark_regtime.wDay, clanmark_regtime.wHour, clanmark_regtime.wMinute );
		}


		::drawFont( g_GameDATA.m_hFONT[ FONT_NORMAL ], true, 15,248, g_dwBLACK, STR_CLAN_ALLYED);
		::drawFont( g_GameDATA.m_hFONT[ FONT_NORMAL ], true, 88,248, g_dwWHITE, Clan.GetAllyName(0) );
//		::drawFont( g_GameDATA.m_hFONT[ FONT_NORMAL ], true, 71,229, g_dwWHITE, Clan.GetAllyName(1) );
//		::drawFont( g_GameDATA.m_hFONT[ FONT_NORMAL ], true, 71,250, g_dwWHITE, Clan.GetAllyName(2) );



		DWORD dwColor = g_dwWHITE;
		if( CClan::GetInstance().GetClass() <= CClan::CLAN_PENALTY )
			dwColor = g_dwRED;

		::drawFont( g_GameDATA.m_hFONT[ FONT_NORMAL ], true, 84,288, dwColor, CStringManager::GetSingleton().GetClanClass( CClan::GetInstance().GetClass() ) );///자신의 직위
		::drawFontf( g_GameDATA.m_hFONT[ FONT_NORMAL ], true, 84,309, g_dwWHITE, "%d",Clan.GetMyClanPoint() );


		g_DrawImpl.Draw(  m_sPosition.x + 229, m_sPosition.y + 283, UI_IMAGE_RES_ID, m_iClassMarkImageNo[ Clan.GetClass() ] );
	}

}

void CClanDlg::DrawMembers()
{

	if( !m_strSelectedMemberView.empty() )
	{
		D3DXMATRIX mat;	
		D3DXMatrixTranslation( &mat, (float)m_sPosition.x + 20, (float)m_sPosition.y + m_iHeight - 28,0.0f);
		::setTransformSprite( mat );

		::drawFont( g_GameDATA.m_hFONT[ FONT_NORMAL ], true, 0,0 , g_dwWHITE, m_strSelectedMemberView.c_str() );
	}
}

void CClanDlg::DrawSkills()
{

}

void CClanDlg::DrawNotice()
{
}


void CClanDlg::ClearClanData()
{
	CWinCtrl * pCtrl = NULL;
	if( pCtrl = Find("NAME") ){		pCtrl->SetText(""); }
	if( pCtrl = Find("LEV") ){		pCtrl->SetText( "" );		}
	if( pCtrl = Find("POINT") ){	pCtrl->SetText( "" );	}
	if( pCtrl = Find("CLAN_RANK") )	{		pCtrl->SetText( "" );	}
	if( pCtrl = Find("CLAN_BONUS") ){		pCtrl->SetText( "" );	}

#ifdef __EDIT_CLANDLG	
	if(m_pListBox_Slogan){	m_pListBox_Slogan->ClearText();	}
#else
	if( pCtrl = Find("SLOGAN") ){	pCtrl->SetText( "" );	}
#endif

	if( pCtrl = Find("MONEY") )	{	pCtrl->SetText( "" );	}
	if( pCtrl = Find("ALLY") ){		pCtrl->SetText("" );	}	
	if( pCtrl = Find("CLASS") )	{	pCtrl->SetText( "" );	}	
	if( pCtrl = Find("CLAN_POINT") ){	pCtrl->SetText( "" );	}	
	if(pCtrl = Find("MEMBER_CNT") )	{	pCtrl->SetText( "" );	}

}

void CClanDlg::SetClanData()
{
	CClan& Clan = CClan::GetInstance();
	DWORD dwColor = g_dwWHITE;

	if( Clan.GetClanNo() )
	{
		CWinCtrl * pCtrl = NULL;

		if( pCtrl = Find("NAME") )
		{
			pCtrl->SetText( Clan.GetName() );
		}
		if( pCtrl = Find("LEV") )
		{
			pCtrl->SetText( CStr::Printf("%d", Clan.GetLevel()) );
		}
		if( pCtrl = Find("POINT") )
		{
			pCtrl->SetText( CStr::Printf("%d", Clan.GetPoint()) );
		}
		if( pCtrl = Find("CLAN_RANK") )
		{
			pCtrl->SetText( CStr::Printf("%d", Clan.GetRankPoint()) );
		}
		if( pCtrl = Find("CLAN_BONUS") )
		{
			pCtrl->SetText( CStr::Printf("%d", Clan.GetRewardPoint()) );
		}

#ifdef __EDIT_CLANDLG		
#else
		if( pCtrl = Find("SLOGAN") )
		{
			pCtrl->SetText( Clan.GetSlogan() );
		}
#endif

		if( pCtrl = Find("MONEY") )
		{
			const int money_buffer_size = 64;
			char money_buffer[ money_buffer_size ];
			CGameUtil::ConvertMoney2String( Clan.GetMoney(), money_buffer, money_buffer_size );
			pCtrl->SetText( money_buffer );
		}
		if( pCtrl = Find("ALLY") )
		{
			pCtrl->SetText( Clan.GetAllyName(0) );
		}
		if( pCtrl = Find("CLASS") )
		{
			pCtrl->SetTextColor(dwColor);
			pCtrl->SetAlign(DT_VCENTER);
			pCtrl->SetText( CStringManager::GetSingleton().GetClanClass( CClan::GetInstance().GetClass() ) );
		}	
		if( pCtrl = Find("CLAN_POINT") )
		{
			pCtrl->SetAlign(DT_VCENTER);
			pCtrl->SetText( CStr::Printf("%d", Clan.GetMyClanPoint()) );
		}	
	}
}

void CClanDlg::DrawInfomation2()
{
	CClan& Clan = CClan::GetInstance();
	DWORD dwColor = g_dwWHITE;

	if( Clan.GetClanNo() )
	{
		CWinCtrl * pCtrl = NULL;

		D3DXMATRIX mat;	
		D3DXMatrixTranslation( &mat, (float)m_sPosition.x, (float)m_sPosition.y,0.0f);
		::setTransformSprite( mat );

		
		if( CClan::GetInstance().GetClass() <= CClan::CLAN_PENALTY )
			dwColor = g_dwRED;

		if(pCtrl = Find("CLASSMARK_POS"))
		{
			g_DrawImpl.Draw(	pCtrl->GetPosition().x,
								pCtrl->GetPosition().y,
								UI_IMAGE_RES_ID,
								m_iClassMarkImageNo[ Clan.GetClass() ] );
		}		
	}

}

void CClanDlg::DrawMembers2()
{	
	CClan& Clan = CClan::GetInstance();
	CWinCtrl * pCtrl = NULL;
	if(pCtrl = Find("MEMBER_CNT") )
	{
		pCtrl->SetText( CStr::Printf("%d / %d", Clan.GetMemberCount(), Clan.GetMemberMaxCount()) );
	}
}

void CClanDlg::DrawSkills2()
{

}

void CClanDlg::DrawNotice2()
{
#ifdef __PRIVATECHAT2//클랜 공지 버그 수정. 
#else

	D3DXMATRIX mat;	
	D3DXMatrixTranslation( &mat, (float)m_sPosition.x, (float)m_sPosition.y, 0.0f );
	::setTransformSprite( mat );
	
	RECT rcDraw = { 12, 45, 198, 230 };

	drawFont( g_GameDATA.m_hFONT[ FONT_NORMAL ],
		true,
		&rcDraw,
		g_dwWHITE, 
		DT_WORDBREAK, 
		CClan::GetInstance().GetNotice() );
#endif
}

void CClanDlg::Update( CObservable* pObservable, CTObject* pObj )
{
	assert( pObservable );
	assert( pObj );

	if( pObservable == NULL || pObj == NULL ) return;

	if( strcmp( pObj->toString(), "CTEventClan") == 0 )
	{

		CTEventClan* pEvent = (CTEventClan*)pObj;

		switch( pEvent->GetID() )
		{
#ifdef __EDIT_CLANDLG
		case CTEventClan::EID_SET_SLOGAN:
		{
			if(m_pListBox_Slogan)
			{
                m_pListBox_Slogan->ClearText();
				m_pListBox_Slogan->AppendText( CClan::GetInstance().GetSlogan(), g_dwWHITE );
				m_pListBox_Slogan->SetValue(0);
			}

            break;
		}
#endif
			
#ifdef __PRIVATECHAT2//클랜 공지 버그 수정. 
		case CTEventClan::EID_SET_NOTICE:
			{				
				CWinCtrl* pCtrl = Find( IID_ZLIST_NOTICE_CONTENT );
				if( pCtrl && pCtrl->GetControlType() == CTRL_EDITBOX )
				{
					CTEditBox * pEditBox = (CTEditBox*)pCtrl;
					pEditBox->SetText( CClan::GetInstance().GetNotice() );
				}
			}
			break;
#endif
		
		case CTEventClan::EID_SET_JOB:
			{
				CWinCtrl* pCtrl = Find( IID_TABBEDPANE );
				if( pCtrl && pCtrl->GetControlType() == CTRL_TABBEDPANE )
				{
					CTabbedPane* pPane = (CTabbedPane*)pCtrl;
					pCtrl = FindCtrlInTabbedPane( pPane, TAB_MEMBER, IID_ZLIST_MEMBER );
					assert( pCtrl );
					if( pCtrl && pCtrl->GetControlType() == CTRL_ZLISTBOX )
					{
						CZListBox* pList = (CZListBox*) pCtrl;
						CClanMemberItem* pItem	= NULL;
						for( int i = 0 ; i < pList->GetSize(); ++i )
						{
							if( pCtrl = pList->GetItem( i ) )
							{
								CClanMemberItem* pItem = (CClanMemberItem*)pCtrl;
								if( _strcmpi( pItem->GetName(), pEvent->GetName() ) == 0 )
								{
									pItem->SetJob( pEvent->GetJob() );
									break;
								}
							}
						}
					}
				}			
			}
			break;
		case CTEventClan::EID_SET_LEVEL:
			{
				CWinCtrl* pCtrl = Find( IID_TABBEDPANE );
				if( pCtrl && pCtrl->GetControlType() == CTRL_TABBEDPANE )
				{
					CTabbedPane* pPane = (CTabbedPane*)pCtrl;
					pCtrl = FindCtrlInTabbedPane( pPane, TAB_MEMBER, IID_ZLIST_MEMBER );
					assert( pCtrl );
					if( pCtrl && pCtrl->GetControlType() == CTRL_ZLISTBOX )
					{
						CZListBox* pList = (CZListBox*) pCtrl;
						CClanMemberItem* pItem	= NULL;
						for( int i = 0 ; i < pList->GetSize(); ++i )
						{
							if( pCtrl = pList->GetItem( i ) )
							{
								CClanMemberItem* pItem = (CClanMemberItem*)pCtrl;
								if( _strcmpi( pItem->GetName(), pEvent->GetName() ) == 0 )
								{
									pItem->SetLevel( pEvent->GetLevel() );
									break;
								}
							}
						}
					}
				}			
			}
			break;
		case CTEventClan::EID_ADD_MEMBER:
			{
				CWinCtrl* pCtrl = Find( IID_TABBEDPANE );
				if( pCtrl && pCtrl->GetControlType() == CTRL_TABBEDPANE )
				{
					CTabbedPane* pPane = (CTabbedPane*)pCtrl;
					pCtrl = FindCtrlInTabbedPane( pPane, TAB_MEMBER, IID_ZLIST_MEMBER );
					assert( pCtrl );
					if( pCtrl && pCtrl->GetControlType() == CTRL_ZLISTBOX )
					{
						CZListBox* pList = (CZListBox*) pCtrl;
						CClanMemberItem* pItem = new CClanMemberItem( pEvent->GetName(),  pEvent->GetClass() , pEvent->GetPoint(), pEvent->GetChannelNo() ,pEvent->GetLevel(), pEvent->GetJob());
						pList->Add( pItem );

						if( pList->GetSize() > 1 )
							SortMemberlistByClass();
					}
				}			
			}
			break;
		case CTEventClan::EID_REMOVE_MEMBER:
			{
				CWinCtrl* pCtrl = Find( IID_TABBEDPANE );
				if( pCtrl && pCtrl->GetControlType() == CTRL_TABBEDPANE )
				{
					CTabbedPane* pPane = (CTabbedPane*)pCtrl;
					pCtrl = FindCtrlInTabbedPane( pPane, TAB_MEMBER, IID_ZLIST_MEMBER );
					assert( pCtrl );
					if( pCtrl && pCtrl->GetControlType() == CTRL_ZLISTBOX )
					{
						CZListBox* pList = (CZListBox*) pCtrl;
						CClanMemberItem* pItem	= NULL;
						for( int i = 0 ; i < pList->GetSize(); ++i )
						{
							if( pCtrl = pList->GetItem( i ) )
							{
								CClanMemberItem* pItem = (CClanMemberItem*)pCtrl;
								if( _strcmpi( pItem->GetName(), pEvent->GetName() ) == 0 )
								{
									pList->DelItem( i );
									break;
								}
							}
						}
					}
				}			
			}
			break;
		case CTEventClan::EID_CHANGE_CLASS:
			{
				CWinCtrl* pCtrl = Find( IID_TABBEDPANE );
				if( pCtrl && pCtrl->GetControlType() == CTRL_TABBEDPANE )
				{
					CTabbedPane* pPane = (CTabbedPane*)pCtrl;
					pCtrl = FindCtrlInTabbedPane( pPane, TAB_MEMBER, IID_ZLIST_MEMBER );
					assert( pCtrl );
					if( pCtrl && pCtrl->GetControlType() == CTRL_ZLISTBOX )
					{
						CZListBox* pList = (CZListBox*) pCtrl;
						CClanMemberItem* pItem	= NULL;
						for( int i = 0 ; i < pList->GetSize(); ++i )
						{
							if( pCtrl = pList->GetItem( i ) )
							{
								CClanMemberItem* pItem = (CClanMemberItem*)pCtrl;
								if( _strcmpi( pItem->GetName(), pEvent->GetName() ) == 0 )
								{
									pItem->SetClass( pEvent->GetClass() );
									SortMemberlistByClass();
									break;
								}
							}
						}
					}
				}			
			}
			break;
		case CTEventClan::EID_CHANGE_CONNECT_STATUS:
			{
				CWinCtrl* pCtrl = Find( IID_TABBEDPANE );
				if( pCtrl && pCtrl->GetControlType() == CTRL_TABBEDPANE )
				{
					CTabbedPane* pPane = (CTabbedPane*)pCtrl;
					pCtrl = FindCtrlInTabbedPane( pPane, TAB_MEMBER, IID_ZLIST_MEMBER );
					assert( pCtrl );
					if( pCtrl && pCtrl->GetControlType() == CTRL_ZLISTBOX )
					{
						CZListBox* pList = (CZListBox*) pCtrl;
						CClanMemberItem* pItem	= NULL;
						for( int i = 0 ; i < pList->GetSize(); ++i )
						{
							if( pCtrl = pList->GetItem( i ) )
							{
								CClanMemberItem* pItem = (CClanMemberItem*)pCtrl;
								if( _strcmpi( pItem->GetName(), pEvent->GetName() ) == 0 )
								{
									pItem->SetConnectStatus( pEvent->GetChannelNo() );
									break;
								}
							}
						}
					}
				}			
			}
			break;
			case CTEventClan::EID_SET_CLEAR_DATA:
			{
                ClearClanData();
			}
			break;
		default:
			break;
		}

		//클랜 정보가 바뀌면 Dlg Control 내부 텍스트 교체.
		SetClanData();
	}
	else if( strcmp( pObj->toString(), "ETClanSkill") == 0 )
	{
		CTEventClanSkill* pEvent = (CTEventClanSkill*)pObj;
		switch( pEvent->GetID() )
		{
		case CTEventClanSkill::EID_ADD_SKILL:
			{
				CWinCtrl* pCtrl = Find( IID_TABBEDPANE );
				if( pCtrl && pCtrl->GetControlType() == CTRL_TABBEDPANE )
				{
					CTabbedPane* pPane = (CTabbedPane*)pCtrl;
					pCtrl = FindCtrlInTabbedPane( pPane, TAB_SKILL, IID_ZLIST_SKILL );
					assert( pCtrl );
					if( pCtrl && pCtrl->GetControlType() == CTRL_ZLISTBOX )
					{
						CZListBox* pList = (CZListBox*)pCtrl;
						
						CIconSkillClan* pIcon		= new CIconSkillClan( pEvent->GetIndex() );
						CClanSkillListItem* pItem	= new CClanSkillListItem;
						pItem->Show();
						pItem->SetIcon( pIcon );
						pList->Add( pItem );
					}
				}			
			}
			break;
		case CTEventClanSkill::EID_DEL_SKILL:
			{
				CWinCtrl* pCtrl = Find( IID_TABBEDPANE );
				if( pCtrl && pCtrl->GetControlType() == CTRL_TABBEDPANE )
				{
					CTabbedPane* pPane = (CTabbedPane*)pCtrl;
					pCtrl = FindCtrlInTabbedPane( pPane, TAB_SKILL, IID_ZLIST_SKILL );
					assert( pCtrl );
					if( pCtrl && pCtrl->GetControlType() == CTRL_ZLISTBOX )
					{
						CZListBox* pList = (CZListBox*)pCtrl;
						RemoveClanSkillListItem( pList, pEvent->GetIndex() );
					}
				}			
			}
			break;
		default:
			break;
		}
	}
}

void CClanDlg::RemoveClanSkillListItem( CZListBox* pListBox, int iSkillSlotIndex )
{
	CClanSkillListItem*	pItem	= NULL;
	CWinCtrl* pCtrl				= NULL;
	CIconSkillClan* pSkillIcon	= NULL;
	for( int i = 0 ; i < pListBox->GetSize(); ++i )
	{
		pCtrl = pListBox->GetItem( i );
		assert( pCtrl );
		if( pCtrl )
		{
			pItem = (CClanSkillListItem*)pCtrl;
			pSkillIcon = pItem->GetIcon();
			assert( pSkillIcon );
			if( pSkillIcon )
			{
				if( pSkillIcon->GetSkillSlotFromIcon() == iSkillSlotIndex )
				{
					pListBox->DelItem( i );
					break;
				}
			}
		}
	}
}
CWinCtrl* CClanDlg::FindCtrlInTabbedPane( CTabbedPane* pPane, int iTabID, int iCtrlID )
{
	CWinCtrl* pCtrl = NULL;
	CJContainer* pContainer = pPane->GetTabContainer( iTabID );
	assert( pContainer );
	if( pContainer )
		return pContainer->Find( iCtrlID );

	return NULL;
}

CClanMemberItem* CClanDlg::GetSelectedMember()
{
	CWinCtrl* pCtrl = Find( IID_TABBEDPANE );
	if( pCtrl && pCtrl->GetControlType() == CTRL_TABBEDPANE )
	{
		CTabbedPane* pPane = (CTabbedPane*)pCtrl;
		pCtrl = FindCtrlInTabbedPane( pPane, TAB_MEMBER, IID_ZLIST_MEMBER );
		assert( pCtrl );
		if( pCtrl && pCtrl->GetControlType() == CTRL_ZLISTBOX )
		{
			CZListBox* pList = (CZListBox*) pCtrl;
			int iSelectedIndex = pList->GetSelectedItemIndex();
			if( iSelectedIndex >= 0 )
			{
				if( pCtrl = pList->GetItem( iSelectedIndex ) )
				{
					return (CClanMemberItem*)pCtrl;
				}
			}
		}
	}			
	return NULL;
}

void CClanDlg::SortMemberlistByClass()
{
	std::multimap< int , CClanMemberItem* > TempMap;
	std::multimap< int , CClanMemberItem* >::reverse_iterator riterMap;

	CClanMemberItem* pItem = NULL;
	CClanMemberItem* pNewItem = NULL;
	CWinCtrl* pCtrl = Find( IID_TABBEDPANE );
	if( pCtrl && pCtrl->GetControlType() == CTRL_TABBEDPANE )
	{
		CTabbedPane* pPane = (CTabbedPane*)pCtrl;
		pCtrl = FindCtrlInTabbedPane( pPane, TAB_MEMBER, IID_ZLIST_MEMBER );
		assert( pCtrl );
		if( pCtrl && pCtrl->GetControlType() == CTRL_ZLISTBOX )
		{
			CZListBox* pList = (CZListBox*) pCtrl;
			for( int i = 0 ; i < pList->GetSize(); ++i )
			{

				pItem = (CClanMemberItem*)pList->GetItem( i );
				pNewItem = new CClanMemberItem( pItem->GetName(), pItem->GetClass() , pItem->GetClanPoint(), pItem->GetChannelNo(), pItem->GetLevel(), pItem->GetJob() );

				TempMap.insert( make_pair( pItem->GetClass() , pNewItem ) );
			}

			pList->Clear();
			for( riterMap = TempMap.rbegin(); riterMap != TempMap.rend(); ++riterMap )
				pList->Add( riterMap->second );

			TempMap.clear();
		}
	}
}

bool CClanDlg::Create( const char* IDD )
{
	bool bRet = CTDialog::Create( IDD );
	if( bRet )
	{
		m_iClassMarkImageNo[0] = CResourceMgr::GetInstance()->GetImageNID( UI_IMAGE_RES_ID, "CLAN01_MARK_JUNIOR" );
		m_iClassMarkImageNo[1] = CResourceMgr::GetInstance()->GetImageNID( UI_IMAGE_RES_ID, "CLAN01_MARK_JUNIOR" );
		m_iClassMarkImageNo[2] = CResourceMgr::GetInstance()->GetImageNID( UI_IMAGE_RES_ID, "CLAN01_MARK_SENIOR" );
		m_iClassMarkImageNo[3] = CResourceMgr::GetInstance()->GetImageNID( UI_IMAGE_RES_ID, "CLAN01_MARK_LANDER" );
		m_iClassMarkImageNo[4] = CResourceMgr::GetInstance()->GetImageNID( UI_IMAGE_RES_ID, "CLAN01_MARK_COMMANDER" );
		m_iClassMarkImageNo[5] = CResourceMgr::GetInstance()->GetImageNID( UI_IMAGE_RES_ID, "CLAN01_MARK_SUBMASTER" );
		m_iClassMarkImageNo[6] = CResourceMgr::GetInstance()->GetImageNID( UI_IMAGE_RES_ID, "CLAN01_MARK_MASTER" );
	}
	return bRet;
}

void CClanDlg::SetSelectedMember( int iClanPoint )
{
	m_strSelectedMemberView.clear();
	m_strSelectedMemberView.append( STR_CLAN_MEMBER_CONTRIB );
	m_strSelectedMemberView.append( CStr::Printf(" : %d", iClanPoint ) );
}

void CClanDlg::Hide()
{
	CTDialog::Hide();
	m_ClanMarkPreView.Free();
}


void CClanDlg::SetInterfacePos_After()
{

	CWinCtrl * pCtrl = NULL;

	if( pCtrl = Find("MARK_POS"))
	{
		m_vCharPos.x = (float)pCtrl->GetOffset().x;
		m_vCharPos.y = (float)pCtrl->GetOffset().y;
		m_vCharPos.z = 0;
	}
	else
	{
		m_vCharPos.x = 238;
		m_vCharPos.y = 102;
		m_vCharPos.z = 0;
	}	 

	//문자열 추가
    //클랜 36
	GetCaption()->SetString( LIST_STRING(36) );

	if( pCtrl = Find(IID_BTN_TAB_INFO) )
	{
		//클랜정보 867
		pCtrl->SetText( LIST_STRING(867) );
	}
	if( pCtrl = Find(IID_BTN_TAB_MEMBER) )
	{
		//맴버정보 868
		pCtrl->SetText( LIST_STRING(868) );
	}
	if( pCtrl = Find(IID_BTN_TAB_SKILL) )
	{
		//클랜스킬 869
		pCtrl->SetText( LIST_STRING(869) );
	}
	if( pCtrl = Find(IID_BTN_TAB_NOTICE) )
	{
		//클랜공지 870
		pCtrl->SetText( LIST_STRING(870) );
	}

	if( pCtrl = Find("NAME") )
	{
		pCtrl->SetAlign(DT_VCENTER);
	}
	if( pCtrl = Find("LEV") )
	{
		pCtrl->SetAlign(DT_VCENTER);
	}
	if( pCtrl = Find("POINT") )
	{
		pCtrl->SetAlign(DT_VCENTER);
	}

#ifdef __EDIT_CLANDLG
#else
	if( pCtrl = Find("SLOGAN") )
	{
		pCtrl->SetAlign(DT_VCENTER);
	}
#endif	

	if( pCtrl = Find("SLOGAN") )
	{
		m_pListBox_Slogan = ( CTListBox* )pCtrl;
	}	

	if( pCtrl = Find("MONEY") )
	{
		pCtrl->SetAlign(DT_VCENTER);
	}
	if( pCtrl = Find("ALLY") )
	{
		pCtrl->SetAlign(DT_VCENTER);
	}	

	if( pCtrl = Find("NAME_TXT") )
	{	
		//클랜명
		pCtrl->SetText( LIST_STRING(861) );
	}
	if( pCtrl = Find("LEV_TXT") )
	{
		//클랜등급
		pCtrl->SetText( LIST_STRING(45) );
	}
	if( pCtrl = Find("POINT_TXT") )
	{
		//클랜점수
		pCtrl->SetText( LIST_STRING(46) );
	}
	if( pCtrl = Find("SLOGAN_TXT") )
	{
		//슬로건
		pCtrl->SetText( LIST_STRING(47) );
	}
	if( pCtrl = Find("MONEY_TXT") )
	{
		//클랜머니
		pCtrl->SetText( LIST_STRING(48) );
	}
	if( pCtrl = Find("ALLY_TXT") )
	{
		//연합
		pCtrl->SetText( STR_CLAN_ALLYED );
	}	

	if( pCtrl = Find(IID_BTN_PREVIEW) )
	{
		//미리보기 871
		pCtrl->SetText( LIST_STRING(871) );
	}	
	if( pCtrl = Find(IID_BTN_REGIST_CLANMARK) )
	{
		//마크등록 872
		pCtrl->SetText( LIST_STRING(872) );
	}

	if( pCtrl = Find(IID_BTN_ENTRUST) )
	{
		//위임하기 873
		pCtrl->SetText( LIST_STRING(873) );
	}	
	if( pCtrl = Find(IID_BTN_BAN) )
	{ 
		//강제탈퇴 874
		pCtrl->SetText( LIST_STRING(874) );
	}
	if( pCtrl = Find(IID_BTN_CLASS_UP) )
	{
		//직위승급 875
		pCtrl->SetText( LIST_STRING(875) );
	}	
	if( pCtrl = Find(IID_BTN_CLASS_DOWN) )
	{
		//직위강등 876
		pCtrl->SetText( LIST_STRING(876) );
	}
	if( pCtrl = Find(IID_BTN_REQJOIN) )
	{
		//가입요청 877
		pCtrl->SetText( LIST_STRING(877) );
	}
	if( pCtrl = Find(IID_BTN_WITHDRAWAL) )
	{
		//탈퇴하기 878
		pCtrl->SetText( LIST_STRING(878) );
	}
	if( pCtrl = Find(IID_BTN_REGIST_NOTICE) )
	{
		//공지등록 879
		pCtrl->SetText( LIST_STRING(879) );
	}
	//문자열 추가
	if(pCtrl = Find("PRIVATE_GRADE_POS"))
	{
		//개인직위 50
		pCtrl->SetText(LIST_STRING(50));		
	}	
	if(pCtrl = Find("MYCLAN_POINT_POS"))
	{
		//기인기어도 51
		pCtrl->SetText(LIST_STRING(51));		
	}	
	if(pCtrl = Find("CLAN_RANK_TXT"))
	{
		//랭킹포인트
		pCtrl->SetText(LIST_STRING(954));
	}	
	if(pCtrl = Find("CLAN_RANK"))
	{		
		pCtrl->SetAlign(DT_VCENTER);
	}

	if(pCtrl = Find("CLAN_BONUS"))
	{		
		pCtrl->SetAlign(DT_VCENTER);
	}
	if(pCtrl = Find("CLAN_BONUS_TXT"))
	{		
		//보상포인트
		pCtrl->SetText(LIST_STRING(955));	
	}	

}