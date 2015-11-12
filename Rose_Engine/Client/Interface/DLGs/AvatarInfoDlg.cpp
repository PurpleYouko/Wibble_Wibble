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

///������ ���Ÿ����⿡ ������ �Ҹ�ź�� �������� �ʾ������ ��� ������ ǥ���� ������ ��ȣ
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

	// �̸�	
	RECT rt;
	SetRect( &rt, 28, 6, 150, 25 );
	::drawFontf( g_GameDATA.m_hFONT[ FONT_NORMAL_BOLD ], true, &rt, D3DCOLOR_XRGB( 255, 255, 255 ), DT_TOP|DT_LEFT, "%s", g_pAVATAR->Get_NAME() );

	// Level, job, weight, exp, zuly.	``
	const int money_buffer_size = 64;
	char money_buffer[ money_buffer_size ];
	CGameUtil::ConvertMoney2String( g_pAVATAR->Get_MONEY(), money_buffer, money_buffer_size );	
	float fExp = ( (float)g_pAVATAR->Get_EXP() / g_pAVATAR->Get_NeedEXP(g_pAVATAR->Get_LEVEL()) ) * 100;

	CWinCtrl * ctrl = NULL;

	//���ڿ� �߰�.
	if(ctrl = Find("INFO_LEV"))
	{
		//����
		((CTImage*)ctrl)->SetText( CStr::Printf(	"%s %d", LIST_STRING(313),  g_pAVATAR->Get_LEVEL() ) );
		ctrl->SetAlign(DT_LEFT);
	}
	if(ctrl = Find("INFO_JOB"))
	{	
		//����
		((CTImage*)ctrl)->SetText( CStr::Printf(	"%s %s", LIST_STRING(813),  CStringManager::GetSingleton().GetJobName( g_pAVATAR->Get_JOB() ) ) );
		ctrl->SetAlign(DT_LEFT);		
	}
	if(ctrl = Find("INFO_WEIGHT"))
	{	//����
		((CTImage*)ctrl)->SetText( CStr::Printf(	"%s %d/%d", LIST_STRING(107), g_pAVATAR->GetCur_WEIGHT(), g_pAVATAR->GetCur_MaxWEIGHT() ) );
		ctrl->SetAlign(DT_LEFT);
	}
	if(ctrl = Find("INFO_EXP"))
	{	//����ġ
		((CTImage*)ctrl)->SetText( CStr::Printf(	"%s %d/%d (%.2f%%)", LIST_STRING(814), g_pAVATAR->Get_EXP(), g_pAVATAR->Get_NeedEXP(g_pAVATAR->Get_LEVEL()), fExp ) );
		ctrl->SetAlign(DT_LEFT);
	}
	if(ctrl = Find("INFO_ZULIE"))
	{		
		//�ٷ�
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
			
			// ȫ�� : ��Ʈ�� �������� �Դ´�.
			pGuage->SetText( "" );
		}
	}


	//exp ������ ������ �Ѹ�
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
	

	///���� ���� ǥ��
	CTDialog* pDlg = CTDialog::GetProcessMouseOverDialog();
	if( pDlg && pDlg != this )
			return ;

	CWinCtrl* pCtrl = Find( IID_BTN_SELFTARGET );

	//���ڿ� �߰�
	//{"�ѹ� Ŭ�� ON & OFF", "���� ON & OFF", "���� ä�� ON & OFF", "���� ���� ON & OFF", };
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
			//����ġ
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
		case CTEventItem::EID_ADD_ITEM:																			/// �κ��丮�� �������� �߰�/���� �Ǿ�����
			{
				CItem* pAddItem = pEvent->GetItem();
				if( iIndex == EQUIP_IDX_WEAPON_R )																/// ������ ���Ⱑ �����Ǿ������
				{
					m_Slot.DetachIcon();
					tagITEM& AddItem = pAddItem->GetItem();
					if( AddItem.GetShotTYPE() == MAX_SHOT_TYPE )												/// �Ҹ�ź�� ����ϴ� ���Ⱑ �ƴҰ��
					{
						m_Slot.AttachIcon( pAddItem->CreateItemIcon() );
					}
					else///�Ҹ�ź�� �����Ǿ� �ִٸ� �װ��� �׸��� �ƴ϶�� ��ǥ�̹����� �׸���.
					{
						CItemSlot* pItemSlot = g_pAVATAR->GetItemSlot();
						CItem* pBullet = pItemSlot->GetItem( INVENTORY_SHOT_ITEM0 + AddItem.GetShotTYPE() );
						if( pBullet )/// �ش� �Ҹ�ź�� �����Ǿ� �������
						{
							m_Slot.AttachIcon( pBullet->CreateItemIcon() );
						}
						else		/// �ش� �Ҹ�ź�� �����Ǿ� ���� ������� : ��ǥ �̹���
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
				else if( iIndex >= INVENTORY_SHOT_ITEM0 && iIndex < INVENTORY_SHOT_ITEM0 + MAX_SHOT_TYPE )		/// �Ҹ�ź�� �����Ǿ��� ���
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
		case CTEventItem::EID_DEL_ITEM:/// �κ��丮����  �������� �����ų� Ż���Ͽ�����
			{
				if( iIndex == EQUIP_IDX_WEAPON_R )/// ������ ������ ���
				{
					m_Slot.DetachIcon();
				}
				else if( iIndex >= INVENTORY_SHOT_ITEM0 && iIndex < INVENTORY_SHOT_ITEM0 + MAX_SHOT_TYPE )///�Ҹ�ź�ϰ��
				{
					CIcon* pIcon = m_Slot.GetIcon();
					if( pIcon )
					{
						CIconItem* pItemIcon = (CIconItem*)pIcon;
						
						
						if( pItemIcon->GetIndex() == EQUIP_IDX_WEAPON_R )					///�����ȹ��Ⱑ �������
						{
							tagITEM& EquipItem = pItemIcon->GetItem();
							if( EquipItem.GetShotTYPE() == iIndex - INVENTORY_SHOT_ITEM0 )	/// �����Ǿ� �ִ� ������ �Ҹ�ź�ϰ��
							{
								tagITEM TempBullet;
								TempBullet.Clear();
								TempBullet.m_cType = ITEM_TYPE_NATURAL;
								TempBullet.m_nItemNo = c_iRepresentBulletItemNo[ EquipItem.GetShotTYPE() ];

								m_pRepresentBullet->SetIndex( EquipItem.GetShotTYPE() );
								m_pRepresentBullet->SetItem( TempBullet );
								
								m_Slot.DetachIcon();///���� �������� ����
								m_Slot.AttachIcon( m_pRepresentBullet->CreateItemIcon() );	///��ǥ Item�� Icon�� Attach�Ѵ�.
								
							}
						}
						else if( pItemIcon->GetIndex() == iIndex )						  	/// �Ҹ�ź�� Ż���Ǿ� ���� ��� �ѹ��� üũ
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
					///�������� ����
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

		//exp ���� ��ġ ����
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
		ctrl->SetScaleWidth( 200 );		//����� ũ��.		
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