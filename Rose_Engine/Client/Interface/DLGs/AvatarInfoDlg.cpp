#include "stdafx.h"
#include "avatarinfodlg.h"

#include "TGuage.h"
#include "TCheckBox.h"
#include "TStatic.h"
#include "TImage.h"
#include "ResourceMgr.h"

#include "../it_mgr.h"
#include "../CToolTipMgr.h"
#include "../CTDrawImpl.h"
#include "../IO_ImageRes.h"
#include "../../Object.h"
#include "../../System/CGame.h"
#include "../../JCommandState.h"

#include "../icon/ciconitem.h"
#include "../../gamecommon/item.h"
#include "../../gamedata/event/cteventitem.h"

#include "../Misc/GameUtil.h"
#include "CHelpMgr.h"
#include "CMinimapDLG.h"
#include "Network\CNetwork.h"
#include "../Command/UICommand.h"
#include ".\cclientstorage.h"

///장착된 원거리무기에 적합한 소모탄을 장착하지 않았을경우 대신 아이콘 표시할 아이템 번호
const int c_iRepresentBulletItemNo[MAX_SHOT_TYPE] = { 301, 321, 341 };

CAvatarInfoDlg::CAvatarInfoDlg( int iType )
{
	SetDialogType( iType );
	POINT ptOffset = { 186, 36 };

	m_Slot.SetHeight( 40 );
	m_Slot.SetWidth( 40 );
	m_Slot.SetParent( iType );
	m_Slot.SetOffset( ptOffset );

	m_pRepresentBullet = new CItem;
}

CAvatarInfoDlg::~CAvatarInfoDlg(void)
{
	m_Slot.DetachIcon();
	SAFE_DELETE( m_pRepresentBullet );
}

bool CAvatarInfoDlg::Create ( const char* IDD )
{
	if( CTDialog::Create( IDD ) )
	{
		RefreshDlg();
		return true;
	}
	return false;
}

void CAvatarInfoDlg::MoveWindow( POINT pt )
{
	CTDialog::MoveWindow( pt );
	m_Slot.MoveWindow( m_sPosition );
}

void CAvatarInfoDlg::Draw()
{
	if( !IsVision() ) return;

	CTDialog::Draw();

	m_Slot.Draw();


	D3DXMATRIX mat;	
	D3DXMatrixTranslation( &mat, (float)m_sPosition.x, (float)m_sPosition.y,0.0f);
	::setTransformSprite( mat );

	// 이름	
	RECT rt;
	SetRect( &rt, 28, 6, 150, 25 );
	::drawFontf( g_GameDATA.m_hFONT[ FONT_NORMAL_BOLD ], true, &rt, D3DCOLOR_XRGB( 255, 255, 255 ), DT_TOP|DT_LEFT, "%s", g_pAVATAR->Get_NAME() );

	// Level, job, weight, exp, zuly.	``
	const int money_buffer_size = 64;
	char money_buffer[ money_buffer_size ];
	CGameUtil::ConvertMoney2String( g_pAVATAR->Get_MONEY(), money_buffer, money_buffer_size );	
	float fExp = ( (float)g_pAVATAR->Get_EXP() / g_pAVATAR->Get_NeedEXP(g_pAVATAR->Get_LEVEL()) ) * 100;

	CWinCtrl * ctrl = NULL;

	//문자열 추가.
	if(ctrl = Find("INFO_LEV"))
	{
		//레벨
		((CTImage*)ctrl)->SetText( CStr::Printf(	"%s %d", LIST_STRING(313),  g_pAVATAR->Get_LEVEL() ) );
		ctrl->SetAlign(DT_LEFT);
	}
	if(ctrl = Find("INFO_JOB"))
	{	
		//직업
		((CTImage*)ctrl)->SetText( CStr::Printf(	"%s %s", LIST_STRING(813),  CStringManager::GetSingleton().GetJobName( g_pAVATAR->Get_JOB() ) ) );
		ctrl->SetAlign(DT_LEFT);		
	}
	if(ctrl = Find("INFO_WEIGHT"))
	{	//무게
		((CTImage*)ctrl)->SetText( CStr::Printf(	"%s %d/%d", LIST_STRING(107), g_pAVATAR->GetCur_WEIGHT(), g_pAVATAR->GetCur_MaxWEIGHT() ) );
		ctrl->SetAlign(DT_LEFT);
	}
	if(ctrl = Find("INFO_EXP"))
	{	//경험치
		((CTImage*)ctrl)->SetText( CStr::Printf(	"%s %d/%d (%.2f%%)", LIST_STRING(814), g_pAVATAR->Get_EXP(), g_pAVATAR->Get_NeedEXP(g_pAVATAR->Get_LEVEL()), fExp ) );
		ctrl->SetAlign(DT_LEFT);
	}
	if(ctrl = Find("INFO_ZULIE"))
	{		
		//줄루
		((CTImage*)ctrl)->SetText( CStr::Printf(	"%s %s", LIST_STRING(815), money_buffer ) );
		ctrl->SetAlign(DT_LEFT);
	}
}

void CAvatarInfoDlg::Update( POINT ptMouse )
{
	if( !IsVision() ) return;
	CTDialog::Update( ptMouse );


	int   iPer		= 0;	
	int   iValue	= 0;
	int	  iMax		= 1;
	char*	pszBuf;
	CTGuage* pGuage = (CTGuage*)Find( IID_GUAGE_HP );
	if( pGuage && g_pAVATAR )
	{
		int iValue = g_pAVATAR->Get_HP();
		if ( iValue < 0 ) 	iValue = 0;

		iMax = g_pAVATAR->Get_MaxHP();
		if( iMax != 0 )
		{
			iPer = (int)((float) iValue * 1000  / iMax);
			pGuage->SetValue( iPer );
			pszBuf = CStr::Printf( "%d/%d", iValue, iMax);
			pGuage->SetText( pszBuf );
			pGuage->SetFont( FONT_NORMAL_OUTLINE );
			pGuage->SetAlign( DT_CENTER || DT_VCENTER );

			// HP%
			CWinCtrl * ctrl = Find("HP_PERCENT");
			if( ctrl )
			{
				((CTImage*)ctrl)->SetText( CStr::Printf("%d%%", iPer/10) );
			}
		}
	}


	pGuage = (CTGuage*)Find( IID_GUAGE_MP );
	if( pGuage )
	{
		int iValue = g_pAVATAR->Get_MP();
		if ( iValue < 0 ) 	iValue = 0;

		iMax = g_pAVATAR->Get_MaxMP();
		if( iMax != 0 )
		{
			iPer = (int)((float)(iValue *1000) / iMax);
			pGuage->SetValue( iPer );
			pszBuf = CStr::Printf( "%d/%d", iValue, iMax);
			pGuage->SetText( pszBuf );
			pGuage->SetFont( FONT_NORMAL_OUTLINE );
			pGuage->SetAlign( DT_CENTER || DT_VCENTER );

			// MP%
			CWinCtrl * ctrl = Find("MP_PERCENT");
			if( ctrl )
			{
				((CTImage*)ctrl)->SetText( CStr::Printf("%d%%", iPer/10) );
			}
		}
	}

	__int64 i64Per = 0;
	pGuage = (CTGuage*)Find( IID_GUAGE_EXP );
	if( pGuage )
	{
		__int64 i64Value = g_pAVATAR->Get_EXP();
		if ( i64Value < 0 ) 	i64Value = 0;

		__int64 i64Max = g_pAVATAR->Get_NeedEXP(g_pAVATAR->Get_LEVEL());
		if( i64Max != 0 )
		{	

			i64Per = i64Value * 1000 / i64Max;
			pGuage->SetValue( (int)i64Per );
			
			// 홍근 : 폰트가 스케일이 먹는다.
			pGuage->SetText( "" );
		}
	}


	//exp 열개로 나누어 뿌림
	if( GetIsMaxView() )
	{
		int i=0;
		CTGuage * pGuageLast = NULL;
		for( i=0; i< 10; i++)
		{
			pGuage = (CTGuage*)Find( CStr::Printf( "EXP%02d", i ) );
			if( pGuage )
			{
				if( (i64Per/100) >= i )
				{
					pGuage->Show();
					pGuage->SetValue( 1000 );
					pGuageLast = pGuage;                    
				}
				else
				{
					pGuage->Hide();
				}
			}
		}
		if( pGuageLast )
		{
			pGuageLast->SetValue( i64Per%100 * 10 );
		}
	}
	

	///이하 툴팁 표시
	CTDialog* pDlg = CTDialog::GetProcessMouseOverDialog();
	if( pDlg && pDlg != this )
			return ;

	CWinCtrl* pCtrl = Find( IID_BTN_SELFTARGET );

	//문자열 추가
	//{"한번 클릭 ON & OFF", "사운드 ON & OFF", "엔터 채팅 ON & OFF", "게임 정보 ON & OFF", };
	std::string strBuf[] = { LIST_STRING(816), LIST_STRING(819), LIST_STRING(818), LIST_STRING(817) };

	for( int i=0; i<IID_BTN_SHORTCUT_SIZE; i++ )
	{
		if( pCtrl  = Find( IID_BTN_CTRL+i ) )
		{
			if( pCtrl->IsInside( ptMouse.x, ptMouse.y ) )
			{				
				CToolTipMgr::GetInstance().RegToolTip( ptMouse.x-20, ptMouse.y+20, strBuf[i].c_str() );
				break;
			}
		}
	}
	
	float fExp = ( (float)g_pAVATAR->Get_EXP() / g_pAVATAR->Get_NeedEXP(g_pAVATAR->Get_LEVEL()) ) * 100;
	std::string strBuf2 = "";
	if( pCtrl  = Find( "Exp_BG" ) )
	{
		if( pCtrl->IsVision() && pCtrl->IsInside( ptMouse.x, ptMouse.y ) )
		{				
			//경험치
			CToolTipMgr::GetInstance().RegToolTip( ptMouse.x-20, ptMouse.y-20,
				 CStr::Printf(	"%s %d/%d (%.2f%%)", LIST_STRING(814), g_pAVATAR->Get_EXP(), g_pAVATAR->Get_NeedEXP(g_pAVATAR->Get_LEVEL()), fExp ) );
		}
	}
	

		
}

unsigned CAvatarInfoDlg::Process(UINT uiMsg,WPARAM wParam,LPARAM lParam)
{
	unsigned iProcID = 0;
	if( iProcID = CTDialog::Process( uiMsg, wParam, lParam ) )
	{
		switch( uiMsg )
		{
		case WM_LBUTTONDOWN:
			{
				switch(iProcID)
				{
				case IID_BTN_CTRL:
					{
						CWinCtrl * pCtrl = NULL;
						pCtrl = Find( iProcID );
						if(((CTCheckBox*)pCtrl)->IsCheck())
						{
							CTCmdSetOption Cmd( "CTRL", "SEVENHEARTSCONTROL" );
							Cmd.Exec(NULL);
						}
						else
						{
							CTCmdSetOption Cmd( "CTRL", "DEFAULTCONTROL" );
							Cmd.Exec(NULL);
						}										
					}
					break;
				case IID_BTN_TIP:
					{
						CWinCtrl * pCtrl = NULL;
						pCtrl = Find( iProcID );
						if(((CTCheckBox*)pCtrl)->IsCheck())
						{
							CTCmdSetOption Cmd( "TIP", "HIDE" );
							Cmd.Exec(NULL);
						}
						else
						{
							CTCmdSetOption Cmd( "TIP", "SHOW" );
							Cmd.Exec(NULL);
						}										
					}
					break;
				case IID_BTN_CHAT:
					{
						CWinCtrl * pCtrl = NULL;
						pCtrl = Find( iProcID );
						if(((CTCheckBox*)pCtrl)->IsCheck())
						{
							CTCmdSetOption Cmd( "CHAT", "INPUTTYPE_AUTOENTER" );
							Cmd.Exec(NULL);
						}
						else
						{
							CTCmdSetOption Cmd( "CHAT", "INPUTTYPE_NORMAL" );
							Cmd.Exec(NULL);
						}										
					}
					break;
				case IID_BTN_SOUND:
					{
						CWinCtrl * pCtrl = NULL;
						pCtrl = Find( iProcID );
						if(((CTCheckBox*)pCtrl)->IsCheck())
						{
							CTCmdSetOption Cmd( "SOUND", "OFF" );
							Cmd.Exec(NULL);
						}
						else
						{
							CTCmdSetOption Cmd( "SOUND", "ON" );
							Cmd.Exec(NULL);
						}										
					}
					break;
				}		
			}
			break;

		case WM_LBUTTONUP:
			{
				switch( iProcID )
				{
				case IID_BTN_SELFTARGET:
					g_UserInputSystem.SetTargetSelf();					
					break;

				case IID_BTN_SETSIZE:
					{
						CWinCtrl * pCtrl = Find( "WindowMode" );
						if(pCtrl)
						{
							pCtrl->Hide();
						}
						pCtrl = Find( "FullMode" );
						if(pCtrl)
						{
							pCtrl->Show();
						}
						SetMinView();
					}
					break;

				case IID_BTN_MENU:
					{
						CWinCtrl * pCtrl = Find( "WindowMode" );
						if(pCtrl)
						{
							pCtrl->Show();
						}
						pCtrl = Find( "FullMode" );
						if(pCtrl)
						{
							pCtrl->Hide();
						}
						SetMaxView();
					}	
					break;
				case IID_BTN_DIALOG2ICON:
					
					break;
				case IID_BTN_SCREENSHOT:
					{
#ifdef _SCREENCAPTURE
						g_pNet->Send_cli_SCREEN_SHOT_TIME();
#else
						CGame::GetInstance().ScreenCAPTURE();
#endif

						
					
					}					
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

void CAvatarInfoDlg::Update( CObservable* pObservable, CTObject* pObj )
{
	assert( pObservable );
	if( pObj && strcmp( pObj->toString(), "CTEventItem" ) == 0 )
	{
		CTEventItem* pEvent = (CTEventItem*)pObj;
		int iIndex = pEvent->GetIndex();
		
		switch( pEvent->GetID() )
		{
		case CTEventItem::EID_ADD_ITEM:																			/// 인벤토리에 아이템이 추가/장착 되었을때
			{
				CItem* pAddItem = pEvent->GetItem();
				if( iIndex == EQUIP_IDX_WEAPON_R )																/// 오른손 무기가 장착되었을경우
				{
					m_Slot.DetachIcon();
					tagITEM& AddItem = pAddItem->GetItem();
					if( AddItem.GetShotTYPE() == MAX_SHOT_TYPE )												/// 소모탄을 사용하는 무기가 아닐경우
					{
						m_Slot.AttachIcon( pAddItem->CreateItemIcon() );
					}
					else///소모탄이 장착되어 있다면 그것을 그리고 아니라면 대표이미지를 그린다.
					{
						CItemSlot* pItemSlot = g_pAVATAR->GetItemSlot();
						CItem* pBullet = pItemSlot->GetItem( INVENTORY_SHOT_ITEM0 + AddItem.GetShotTYPE() );
						if( pBullet )/// 해당 소모탄이 장착되어 있을경우
						{
							m_Slot.AttachIcon( pBullet->CreateItemIcon() );
						}
						else		/// 해당 소모탄이 장착되어 있지 않을경우 : 대표 이미지
						{
							tagITEM TempBullet;
							TempBullet.Clear();
							TempBullet.m_cType = ITEM_TYPE_NATURAL;
							TempBullet.m_nItemNo = c_iRepresentBulletItemNo[ AddItem.GetShotTYPE() ];
							m_pRepresentBullet->SetIndex( AddItem.GetShotTYPE() );
							m_pRepresentBullet->SetItem( TempBullet );

							m_Slot.AttachIcon( m_pRepresentBullet->CreateItemIcon() );
						}
					}
				}
				else if( iIndex >= INVENTORY_SHOT_ITEM0 && iIndex < INVENTORY_SHOT_ITEM0 + MAX_SHOT_TYPE )		/// 소모탄이 장착되었을 경우
				{
					CIcon* pPrevIcon = m_Slot.GetIcon();
					if( pPrevIcon )
					{
						CIconItem* pPrevItemIcon = (CIconItem*)pPrevIcon;
						switch( pPrevItemIcon->GetIndex() )
						{
						case SHOT_TYPE_ARROW:
						case SHOT_TYPE_BULLET:
						case SHOT_TYPE_THROW:
							if( pPrevItemIcon->GetIndex() == iIndex - INVENTORY_SHOT_ITEM0 )
							{
								m_Slot.DetachIcon();
								m_Slot.AttachIcon( pAddItem->CreateItemIcon() );
							}
							break;						
						case EQUIP_IDX_WEAPON_R:
							{
								tagITEM& PrevItem = pPrevItemIcon->GetItem();
								if( PrevItem.GetShotTYPE() == iIndex - INVENTORY_SHOT_ITEM0 )
								{
									m_Slot.DetachIcon();
									m_Slot.AttachIcon( pAddItem->CreateItemIcon() );
								}
								break;
							}
						default:
							break;
						}
					}
				}	
				break;
			}
		case CTEventItem::EID_DEL_ITEM:/// 인벤토리에서  아이템을 버리거나 탈착하였을때
			{
				if( iIndex == EQUIP_IDX_WEAPON_R )/// 오른손 무기의 경우
				{
					m_Slot.DetachIcon();
				}
				else if( iIndex >= INVENTORY_SHOT_ITEM0 && iIndex < INVENTORY_SHOT_ITEM0 + MAX_SHOT_TYPE )///소모탄일경우
				{
					CIcon* pIcon = m_Slot.GetIcon();
					if( pIcon )
					{
						CIconItem* pItemIcon = (CIconItem*)pIcon;
						
						
						if( pItemIcon->GetIndex() == EQUIP_IDX_WEAPON_R )					///장착된무기가 있을경우
						{
							tagITEM& EquipItem = pItemIcon->GetItem();
							if( EquipItem.GetShotTYPE() == iIndex - INVENTORY_SHOT_ITEM0 )	/// 장착되어 있는 무기의 소모탄일경우
							{
								tagITEM TempBullet;
								TempBullet.Clear();
								TempBullet.m_cType = ITEM_TYPE_NATURAL;
								TempBullet.m_nItemNo = c_iRepresentBulletItemNo[ EquipItem.GetShotTYPE() ];

								m_pRepresentBullet->SetIndex( EquipItem.GetShotTYPE() );
								m_pRepresentBullet->SetItem( TempBullet );
								
								m_Slot.DetachIcon();///기존 아이콘을 빼고
								m_Slot.AttachIcon( m_pRepresentBullet->CreateItemIcon() );	///대표 Item의 Icon을 Attach한다.
								
							}
						}
						else if( pItemIcon->GetIndex() == iIndex )						  	/// 소모탄이 탈착되어 지는 경우 한번더 체크
						{
							CItemSlot* pItemSlot = g_pAVATAR->GetItemSlot();
							CItem* pEquipItem = pItemSlot->GetItem( EQUIP_IDX_WEAPON_R );
							m_Slot.DetachIcon();
							if( pEquipItem )
							{
								tagITEM& EquipItem = pEquipItem->GetItem();
								if( EquipItem.GetShotTYPE() == MAX_SHOT_TYPE )
								{
									m_Slot.AttachIcon( pEquipItem->CreateItemIcon() );
								}
								else
								{
									tagITEM TempBullet;
									TempBullet.Clear();
									TempBullet.m_cType = ITEM_TYPE_NATURAL;
									TempBullet.m_nItemNo = c_iRepresentBulletItemNo[ EquipItem.GetShotTYPE() ];

									m_pRepresentBullet->SetIndex( EquipItem.GetShotTYPE() );
									m_pRepresentBullet->SetItem( TempBullet );
									
									m_Slot.AttachIcon( m_pRepresentBullet->CreateItemIcon() );
								}
							}
						}
					}
				}	
				else
				{
					///나머지는 무시
				}
				break;
			}
		default:
			break;
		}
	}
	else
	{
		assert( 0 && "CTEvent is NULL or Invalid Type @CAvatarInfoDlg::Update" );
	}
}


void CAvatarInfoDlg::RefreshDlg()
{
	CTGuage* pGuage = (CTGuage*)Find( IID_GUAGE_EXP );
	if( pGuage )
	{			
		pGuage->SetSizeFit(true);
		pGuage->SetWidth( getScreenWidth() - pGuage->GetPosition().x );
	}

	CWinCtrl * ctrl = Find( "EXP" );
	if( ctrl )
	{					
		ctrl->Hide();

		//exp 열개 위치 세팅
		int i=0;		
		CTGuage * pGuage = NULL;
		float fPosUnit = (float)(getScreenWidth() - ctrl->GetPosition().x)/10.0f;
		int iPosGap = 1;

		for( i=0; i< 10; i++)
		{
			pGuage = (CTGuage*)Find( CStr::Printf( "EXP%02d", i ) );
			if( pGuage )
			{
				pGuage->SetWidth( fPosUnit );
				pGuage->SetOffsetX( (fPosUnit+iPosGap) * i );				
				pGuage->SetSizeFit( true );
			}
		}		
	}

	ctrl = Find("Exp_BG");
	if( ctrl )
	{
		ctrl->SetSizeFit(true);
		ctrl->SetWidth( g_pCApp->GetWIDTH() );		
	}		
	ctrl = Find("Info_bg");
	if( ctrl )
	{
		ctrl->SetScaleWidth( 200 );		//충분히 크게.		
	}

	
	POINT pt;
	pt.y = 0;

	ctrl = Find("CTRL");
	if( ctrl )
	{		
		pt.x = getScreenWidth()-93;		
		pt.y = ctrl->GetOffset().y;
		ctrl->SetOffset( pt );
	}
	ctrl = Find("TIP");
	if( ctrl )
	{
		pt.x = getScreenWidth()-70;
		pt.y = ctrl->GetOffset().y;
		ctrl->SetOffset( pt );
	}
	ctrl = Find("CHAT");
	if( ctrl )
	{
		pt.x = getScreenWidth()-47;
		pt.y = ctrl->GetOffset().y;
		ctrl->SetOffset( pt );
	}
	ctrl = Find("SOUND");
	if( ctrl )
	{
		pt.x = getScreenWidth()-24;
		pt.y = ctrl->GetOffset().y;
		ctrl->SetOffset( pt );
	}

	ctrl = Find("HP_PERCENT");
	if( ctrl )
	{
		((CTImage*)ctrl)->SetFont( FONT_SMALL );
		((CTImage*)ctrl)->SetTextColor( D3DCOLOR_ARGB( 255,0,0,0 ) );
		((CTImage*)ctrl)->SetAlign( DT_VCENTER | DT_CENTER );

	}
	ctrl = Find("MP_PERCENT");
	if( ctrl )
	{
		((CTImage*)ctrl)->SetFont( FONT_SMALL );
		((CTImage*)ctrl)->SetTextColor( D3DCOLOR_ARGB( 255,0,0,0 ) );
		((CTImage*)ctrl)->SetAlign( DT_VCENTER | DT_CENTER );
	}

	MoveWindow( GetPosition() );
}

void CAvatarInfoDlg::Show()
{
	CTDialog::Show();
	
	CWinCtrl * ctrl = Find("FullMode");
	if( ctrl )
	{
		ctrl->Hide();
	}	

	t_OptionKeyboard	KeyboardOption;
	t_OptionPlay		PlayOption;
	g_ClientStorage.GetKeyboardOption( KeyboardOption );
	g_ClientStorage.GetPlayOption( PlayOption );

	ctrl = Find("CHAT");
	if( ctrl )
	{
		((CTCheckBox*)ctrl)->SetCheck(KeyboardOption.iChattingMode);		
	}
	ctrl = Find("CTRL");
	if( ctrl )
	{
		((CTCheckBox*)ctrl)->SetCheck( PlayOption.uiControlType );		
	}
	
	SetMaxView();

	SetMiniMapPos();
}

void CAvatarInfoDlg::Hide()
{
	CTDialog::Hide();
}

void CAvatarInfoDlg::SetMaxView( bool bIsMax )
{
	CWinCtrl * ctrl = NULL;

	ctrl = Find("DETAIL_PAN");
	if( ctrl )	{	if(bIsMax)	{	ctrl->Show();	}	else {	ctrl->Hide();	}	}
	
	ctrl = Find("HP_PERCENT");
	if( ctrl )	{	if(bIsMax)	{	ctrl->Show();	}	else {	ctrl->Hide();	}	}
	
	ctrl = Find("MP_PERCENT");
	if( ctrl )	{	if(bIsMax)	{	ctrl->Show();	}	else {	ctrl->Hide();	}	}

	ctrl = Find("Caption");
	if( ctrl )
	{	
		if(bIsMax)
		{
			this->SetWidth( 1600 );
		}	
		else
		{
			this->SetWidth( ctrl->GetWidth() );			
		}	
	}

	g_HelpMgr.SetShowTip( bIsMax );
	RefreshDlg();
	m_bIsMaxView = bIsMax;

	SetMiniMapPos();	
}

void CAvatarInfoDlg::SetMinView()
{
	SetMaxView(false);
	
}

bool CAvatarInfoDlg::GetIsMaxView()
{
	return m_bIsMaxView;
}

void CAvatarInfoDlg::SetInterfacePos_After()
{
	SetMiniMapPos();	
	MoveWindow(GetPosition());
}

void CAvatarInfoDlg::SetMiniMapPos()
{
	//SET MINIMAP POSY
	CMinimapDLG * pDLG = (CMinimapDLG*)g_itMGR.FindDlg( DLG_TYPE_MINIMAP );
	if( pDLG )
	{
		if( GetIsMaxView() )
		{		
			CWinCtrl * ctrl = Find( "EXP" );
			if( ctrl )
			{					
				pDLG->SetAvataInfoPos( ctrl->GetPosition().y + 7 );
			}			
		}
		else
		{			            
			pDLG->SetAvataInfoPos( 0 );
		}		
	}
}

void CAvatarInfoDlg::SetOptionStatus(const char * pszOption, const char * pszStatus)
{
	CWinCtrl * pCtrl = NULL;

	if( pCtrl = Find(pszOption) )
	{
		if( strcmp( pszStatus, "SEVENHEARTSCONTROL" )==0 
			|| strcmp( pszStatus, "INPUTTYPE_AUTOENTER" )==0 )
		{
			((CTCheckBox*)pCtrl)->SetCheck();
		}
		else if( strcmp( pszStatus, "DEFAULTCONTROL" )==0 
			|| strcmp( pszStatus, "INPUTTYPE_NORMAL" )==0 )
		{
			((CTCheckBox*)pCtrl)->SetUncheck();
		}
	}	
}