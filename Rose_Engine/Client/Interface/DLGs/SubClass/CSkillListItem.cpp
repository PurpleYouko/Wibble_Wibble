#include "stdafx.h"
#include ".\cskilllistitem.h"
#include "TButton.h"
#include "ResourceMgr.h"
#include "../../Icon/CIconSkill.h"
#include "../../IO_ImageRes.h"
#include "../../interfaceType.h"
#include "../../CToolTipMgr.h"

#include "TImage.h"
#include "TDialog.h"
#include "../../../Object.h"
#include "../../CDragItem.h"
#include "../../command/dragcommand.h"
#include "../../../Network/CNetwork.h"
#include "../../it_mgr.h"
#include "../CSkillDlg.h"
#include "../../../GameCommon/Skill.h"
#include "../../../Country.h"

#include "../interface/dlgs/CItemDlg.h"
#include "../../command/uicommand.h"

CSkillListItem::CSkillListItem(void)
{
	CResourceMgr* pResMgr = CResourceMgr::GetInstance();

	m_pButton		= new CTButton;
	m_pButton->Create( 0,0, 17, 17, 
		pResMgr->GetImageNID( UI_IMAGE_RES_ID,"UI09_BTN_PLUS_NORMAL" ),
		pResMgr->GetImageNID( UI_IMAGE_RES_ID,"UI09_BTN_PLUS_OVER" ),
		pResMgr->GetImageNID( UI_IMAGE_RES_ID,"UI09_BTN_PLUS_DOWN" ),
		UI_IMAGE_RES_ID 
	);

	m_pButton->SetDisableGrahicID( pResMgr->GetImageNID( UI_IMAGE_RES_ID,"UI09_BTN_PLUS_DISABLE" ));
	m_pButton->SetOffset( 40-17, 40-17 );	
	m_pButton->SetControlID( 10 );///����ID Process���� ������ �ޱ� ���ؼ�

	m_Slot.SetOffset( 0, 0 );
	m_Slot.SetParent( DLG_TYPE_SKILL );


	m_pDragItem = new CDragItem;
	CTCommand* pCmd = new CTCmdDragSkill2QuickBar;
	m_pDragItem->AddTarget( DLG_TYPE_QUICKBAR, pCmd );

	pCmd = new CTCmdDragSkill2QuickBar( DLG_TYPE_QUICKBAR_EXT );
	m_pDragItem->AddTarget( DLG_TYPE_QUICKBAR_EXT, pCmd );

	m_Slot.SetDragAvailable();
	m_Slot.SetDragItem( m_pDragItem );

	SetHeight( 44 );

	m_bLearn = true;


	m_pImage = NULL;

}

CSkillListItem::~CSkillListItem(void)
{
	SAFE_DELETE( m_pDragItem );
	SAFE_DELETE( m_pButton );
	SAFE_DELETE( m_pImage );
	m_Slot.DetachIcon();
}

void CSkillListItem::DelButton()
{
	SAFE_DELETE( m_pButton );
}

CIconSkill*	CSkillListItem::GetIcon()
{
	CIcon* pIcon = m_Slot.GetIcon();
	if( pIcon )
		return (CIconSkill*)pIcon;

	return NULL;
}

void CSkillListItem::SetIcon( CIconSkill* pIcon )
{
	m_Slot.DetachIcon();
	m_Slot.AttachIcon( pIcon );
	assert( pIcon );
	if( pIcon )
	{
		CIconSkill* pSkillIcon = (CIconSkill*)pIcon;
		int iSkillSlotIndex = pSkillIcon->GetSkillSlotFromIcon();

		// ��ų ������ 0���� �۰ų�(���� �ִ� ��ų ǥ�� �Ѻκ�.) �нú��� ��� �巡�� ����.
		if( pIcon->GetSkillSlotFromIcon() < 0 ||
			SKILL_TYPE( g_pAVATAR->GetSkillSlot()->GetSkillIndex( iSkillSlotIndex ) ) == SKILL_TYPE_PASSIVE )
		{
			m_Slot.SetDragDisable();
		}
		else 
		{
			m_Slot.SetDragAvailable();
		}

		Update();
	}
}

//--------------------------------------------------------------------------------
/// @brief ��ų�� ����Ǿ����� ȣ��Ǿ� ���� method( ���� ��Ͻ�& ������ )
//--------------------------------------------------------------------------------
void CSkillListItem::Update()
{
	CIcon* pIcon = m_Slot.GetIcon();
	if( NULL != pIcon )
	{	
		CIconSkill* pSkillIcon = (CIconSkill*)pIcon;

		RECT rtDraw = { 50 , 25 , 50 + 100, 25 + 18 };

		CSkill* pSkill = pSkillIcon->GetSkill();
		
		int iSkillNo = pSkill->GetSkillIndex();

		int iUseValue = 0;
	
		std::string strUseAbility;
					
		for( int i = 0; i < SKILL_USE_PROPERTY_CNT; ++i )
		{
			if( SKILL_USE_PROPERTY(iSkillNo, i) )
			{
				iUseValue = g_pAVATAR->Skill_ToUseAbilityVALUE( iSkillNo, i );
				strUseAbility.append( CStr::Printf( "%s:%d", CStringManager::GetSingleton().GetAbility( SKILL_USE_PROPERTY(iSkillNo,i) ), iUseValue ) );
				strUseAbility.append("   ");
			}
		}
		
		if( !strUseAbility.empty() )
		{
			m_strUseAbility.set_string( strUseAbility.c_str(), rtDraw );
		}
		
		
		
	}
}

void CSkillListItem::Draw()
{
	if( !IsVision() ) return;

	/// ������ ������¸� �����ֱ� ���ؼ�..
	::endSprite();
	::beginSprite( D3DXSPRITE_ALPHABLEND ); 

	m_Slot.Draw();		

	CIcon* pIcon = m_Slot.GetIcon();
	if( pIcon )
	{
		CIconSkill* pSkillIcon = (CIconSkill*)pIcon;
		CSkill* pSkill = pSkillIcon->GetSkill();
		
		assert( pSkill );
		
		if( pSkill )
		{
			int iSkillNo = pSkill->GetSkillIndex();
			int iLevel = pSkill->GetSkillLevel();

			CSinglelineString sStrBuf;

			RECT	rtDraw = { 5, 25, 5+110, 55+18 };

			if( pIcon->GetName() )
			{
				if( iLevel <= 0 )
				{
				}
				else
				{
					if ( IsLearn() )
					{
						sStrBuf.set_string( CStr::Printf("%d", iLevel ), rtDraw );
						sStrBuf.set_font( FONT_NORMAL_OUTLINE );
					}					

					sStrBuf.draw();					
				}
			}
			


			//�⺻���� ���� ��ų�� ��ư�� ���� ����!
			if( SKILL_TAB_TYPE(iSkillNo) == SKILL_TAB_COMMON )
			{
				SAFE_DELETE( m_pButton );
			}			
		}
	}

	if( m_pImage )
	{
		m_pImage->Draw();
	}

	if( m_pButton )
	{
		m_pButton->Draw();
	}
}

void CSkillListItem::MoveWindow( POINT pt )
{
	CWinCtrl::MoveWindow( pt );
	m_Slot.MoveWindow( m_sPosition );

	if( m_pButton )
		m_pButton->MoveWindow( m_sPosition );

	if( m_pImage )
		m_pImage->MoveWindow( pt );
}

void CSkillListItem::Update( POINT ptMouse )
{
	if( !IsVision() ) return;

	m_Slot.Update( ptMouse );

	if( m_pButton )
	{
		m_pButton->Update( ptMouse );
		CIcon* pIcon = m_Slot.GetIcon();
		if( pIcon )
		{
			CTDialog* pDlg = CTDialog::GetProcessMouseOverDialog();
			if( pDlg && pDlg->GetDialogType() != DLG_TYPE_SKILL)
				return;

			//��ų�� ���� �������� ��ư ǥ��.
			CIconSkill* pSkillIcon = (CIconSkill*)pIcon;
			if( pSkillIcon )
			{
				if( IsValidLevelUp( pSkillIcon->GetSkillSlotFromIcon() )==false )
				{
					if( !GetImage() )
					{
						m_pButton->Hide();
					}
				}				
				else
				{
					m_pButton->Show();
				}
			}
		}
	}
}

void CSkillListItem::Show()
{
	CWinCtrl::Show();
	m_Slot.Show();
	if( m_pButton )
		m_pButton->Show();

	if( m_pImage )
	{
		m_pImage->Show();
	}
}

void CSkillListItem::Hide()
{
	CWinCtrl::Hide();
	m_Slot.Hide();
	if( m_pButton )
		m_pButton->Hide();

	if( m_pImage )
	{
		m_pImage->Hide();
	}
}

unsigned CSkillListItem::Process( unsigned uiMsg, WPARAM wParam, LPARAM lParam )
{
	if( !IsVision() ) return 0;


	if( m_pButton && m_pButton->Process( uiMsg, wParam, lParam ) )
	{
		if( uiMsg == WM_LBUTTONUP )///������ Level Up�� ��û�Ѵ�.������ �������� �������� �ѹ��� üũ�Ұ�
		{
			CIcon* pIcon = m_Slot.GetIcon();
			if( pIcon )
			{
				CIconSkill* pSkillIcon = (CIconSkill*)pIcon;	
				

				//��ų�� ������.(��ų)
				if( pSkillIcon->GetSkillSlotFromIcon() >= 0 )
				{
					if( IsValidLevelUp( pSkillIcon->GetSkillSlotFromIcon() )  )
					{
						g_pNet->Send_cli_SKILL_LEVELUP_REQ( (BYTE)pSkillIcon->GetSkillSlotFromIcon() );
					}
					else
					{
						g_itMGR.AppendChatMsg( m_error_msg.c_str(), IT_MGR::CHAT_TYPE_SYSTEM );
					}
				}
				//��ų�� ������.(����� �ϴ� ��ų. �κ����� å�� �ֳ� Ȯ���غ���, ���� ���� �����.)
				else
				{
					if( g_pAVATAR->Skill_FindLearnedSlot( pSkillIcon->GetSkill()->GetClanSkillSlot() ) < 0 )
					{
						//20060623 ��ų������ ��ų ��ﶧ Ȯ�� �޼��� �߰�.

						CTCmdLearnSkill * pCmd = new CTCmdLearnSkill( pSkillIcon );
						pCmd->Exec(NULL);
						SAFE_DELETE( pCmd );

					}
				}
			}
		}
		return GetControlID();
	}

	if( m_Slot.Process( uiMsg, wParam, lParam ) )
		return GetControlID();

	return 0;
}

bool CSkillListItem::IsHasNextLevel( int iSkillSlotIdx )
{
	if( iSkillSlotIdx  < 0 ) 
		return false;

	short nSkillIDX = g_pAVATAR->m_Skills.m_nSkillINDEX[iSkillSlotIdx];

	///��ų������ 10�̻��̰ų� �ʿ� ����Ʈ���� ���� ����Ʈ�� ���ٸ� false
	if ( SKILL_1LEV_INDEX( nSkillIDX) != SKILL_1LEV_INDEX( nSkillIDX + 1 ) ||
		 SKILL_LEVEL( nSkillIDX ) + 1   != SKILL_LEVEL( nSkillIDX + 1) ) 
	{
		//g_itMGR.AppendChatMsg( STR_CANT_LEVELUP, IT_MGR::CHAT_TYPE_SYSTEM  );		
		m_error_msg = STR_CANT_LEVELUP;
		return false;
	}
	return true;
}




bool CSkillListItem::IsValidLevelUp( int iSkillSlotIdx )
{
	//��ų�� ����� �ʾҴ�.
	if( iSkillSlotIdx  < 0 ) 
	{
		return false;	
	}

	short nSkillIDX = g_pAVATAR->m_Skills.m_nSkillINDEX[iSkillSlotIdx];

	///��ų������ 10�̻��̰ų� �ʿ� ����Ʈ���� ���� ����Ʈ�� ���ٸ� false
	if ( SKILL_1LEV_INDEX( nSkillIDX) != SKILL_1LEV_INDEX( nSkillIDX + 1 ) ||
		 SKILL_LEVEL( nSkillIDX ) + 1   != SKILL_LEVEL( nSkillIDX + 1) ) 
	{
		m_error_msg = STR_CANT_LEVELUP;
		return false;
	}

	if( g_pAVATAR->Get_NeedPoint2SkillUP( iSkillSlotIdx ) > g_pAVATAR->GetCur_SkillPOINT() )
	{
		m_error_msg = STR_NOT_ENOUGH_SKILLPOINT;
		return false;
	}
	
	int nNextSkillIDX = nSkillIDX + 1;
	int iNeedAbilityType = 0;
	for( int i = 0; i < SKILL_NEED_ABILITY_TYPE_CNT; ++i)
	{
		iNeedAbilityType  = SKILL_NEED_ABILITY_TYPE( nNextSkillIDX, i );
		if( iNeedAbilityType )
		{
			if( g_pAVATAR->Get_AbilityValue( iNeedAbilityType ) < SKILL_NEED_ABILITY_VALUE( nNextSkillIDX, i ))
			{
				m_error_msg = CStr::Printf(STR_NEED_ABILITY, 
					CStringManager::GetSingleton().GetAbility( iNeedAbilityType ),
					SKILL_NEED_ABILITY_VALUE( nNextSkillIDX, i ) );
				return false;
			}
		}
	}
/*
	int iNeedAbilityType = 0;
	for( int i = 0; i < SKILL_NEED_ABILITY_TYPE_CNT; ++i)
	{
		iNeedAbilityType  = SKILL_NEED_ABILITY_TYPE( nSkillIDX, i );
		if( iNeedAbilityType )
		{
			if( g_pAVATAR->Get_AbilityValue( iNeedAbilityType ) < SKILL_NEED_ABILITY_VALUE( nSkillIDX, i ))
			{
				m_error_msg = CStr::Printf(STR_NEED_ABILITY, 
					CStringManager::GetSingleton().GetAbility( iNeedAbilityType ),
					SKILL_NEED_ABILITY_VALUE( nSkillIDX, i ) );
				return false;
			}
		}
	}
*/
	

	return true;
}

CTButton * CSkillListItem::GetButton()
{
	return m_pButton;
}

void CSkillListItem::SetNotLearnSkill( bool bLearn )
{
	m_bLearn = bLearn;
}

