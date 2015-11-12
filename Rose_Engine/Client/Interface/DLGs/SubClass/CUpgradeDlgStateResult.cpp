#include "stdafx.h"
#include ".\cupgradedlgstateresult.h"
#include "../CUpgradeDlg.h"
#include "../../../Network/Net_prototype.h"
#include "CGuage.h"
#include "../../../GameData/CUpgrade.h"
#include "../../../GameCommon/Item.h"
#include "../../../Network/CNetwork.h"
#include "../../../Game.h"
#include "../../command/uicommand.h"
#include "../../it_mgr.h"
#include "../../io_imageres.h"
#include "../../TypeResource.h"
#include "../../../Object.h"
#include "../../../Game_Func.h"
#include "ResourceMgr.h"

CUpgradeDlgStateResult::CUpgradeDlgStateResult( CUpgradeDlg* pParent )
{
	m_pParent = pParent;

	m_iRedGuageImageID = CResourceMgr::GetInstance()->GetImageNID( UI_IMAGE_RES_ID,"ID_GUAGE_MAKE_RED" );
	m_iGreenGuageImageID  = CResourceMgr::GetInstance()->GetImageNID( UI_IMAGE_RES_ID,"ID_GUAGE_MAKE_GREEN" );
	int iModuleID	    = UI_IMAGE_RES_ID;
	int iGuageWidth		= 88;

#ifdef _NEWINTERFACEKJH
	POINT	ptOffset	= { 23 , 268 };
#else
	POINT	ptOffset	= { 27,187 + 47 * 3 };
#endif
	
	m_pResultGuage = new CGuage(m_iRedGuageImageID, 0, iModuleID , iGuageWidth );
	m_pResultGuage->SetOffset( ptOffset );
	m_pResultGuage->SetAutoIncrement( 50, 100 );
}

CUpgradeDlgStateResult::~CUpgradeDlgStateResult(void)
{
	SAFE_DELETE( m_pResultGuage );
}
void CUpgradeDlgStateResult::Leave()
{
	m_bWaitUserinput = false;
	BYTE btResult = CUpgrade::GetInstance().GetResult();
	switch( btResult )
	{
	case CRAFT_UPGRADE_SUCCESS:
		{
#ifdef __NEWUPGRADESYSTEM
			if(CUpgrade::GetInstance().GetMaterialItemIsEmpty())
			{
				CUpgrade::GetInstance().RemoveOnlyMaterialItem();
			}
			else
			{
				CUpgrade::GetInstance().RemoveTargetItem();
				CUpgrade::GetInstance().ApplyResultItemSet();
			}
#else
			CUpgrade::GetInstance().RemoveTargetItem();
			CUpgrade::GetInstance().ApplyResultItemSet();
#endif
		}
		break;
	case CRAFT_UPGRADE_FAILED:
		CUpgrade::GetInstance().RemoveTargetItem();
		CUpgrade::GetInstance().ApplyResultItemSet();
		//g_itMGR.OpenMsgBox(STR_CRAFT_UPGRADE_FAILED);
		break;

	case 0x99:	// 07. 03. 08 - 김주현 : 재련이 실패해서 아이템의 등급이 하락.
		{
			if(CUpgrade::GetInstance().GetMaterialItemIsEmpty())
			{
				CUpgrade::GetInstance().RemoveOnlyMaterialItem();
			}
			else
			{
				CUpgrade::GetInstance().RemoveTargetItem();
				CUpgrade::GetInstance().ApplyResultItemSet();
			}
		}
		break;

	case CRAFT_UPGRADE_INVALID_MAT:
		g_itMGR.OpenMsgBox( STR_CRAFT_UPGRADE_INVALID_MAT);
		break;

	default:
		break;
	}
}

void CUpgradeDlgStateResult::Enter()
{
	m_bWaitUserinput = false;
	BYTE btResult = CUpgrade::GetInstance().GetResult();
	m_pResultGuage->SetValue(0);
#if defined( _NEWINTERFACEKJH )
	m_pResultGuage->SetGuageImageID(m_iGreenGuageImageID);
#else
	m_pResultGuage->SetGuageImageID(m_iRedGuageImageID);
#endif


	switch( btResult )
	{	
	case CRAFT_UPGRADE_SUCCESS:
		{
		#if defined( _NEWINTERFACEKJH )
			m_pResultGuage->SetAutoIncrementMaxValue( 120 );
		#else
			int iSuccess = CUpgrade::GetInstance().GetResultSuccessProb() / 10;
			if( iSuccess <= 30 ) iSuccess = 31;
			if( iSuccess >= 100 ) iSuccess = 100;

			m_iVirtualSuccessPoint = iSuccess - ( rand() % 30 );
			m_pResultGuage->SetAutoIncrementMaxValue( iSuccess  );
		#endif
			break;
		}
	case CRAFT_UPGRADE_FAILED:
		{
		#if defined( _NEWINTERFACEKJH )
			m_pResultGuage->SetAutoIncrementMaxValue( 120 );
		#else
			m_iVirtualSuccessPoint = rand() % 50 + 30;

			int iSuccess = m_iVirtualSuccessPoint - ( rand() % 20 );
			m_pResultGuage->SetAutoIncrementMaxValue( iSuccess  );			
		#endif
			break;
		}

	case 0x99:	// 07. 03. 08 - 김주현 : 재련이 실패해서 아이템의 등급이 하락.
		{
#if defined( _NEWINTERFACEKJH )
			m_pResultGuage->SetAutoIncrementMaxValue( 120 );
#else
			m_iVirtualSuccessPoint = rand() % 50 + 30;

			int iSuccess = m_iVirtualSuccessPoint - ( rand() % 20 );
			m_pResultGuage->SetAutoIncrementMaxValue( iSuccess  );			
#endif
			break;
		}


	case CRAFT_UPGRADE_INVALID_MAT:
		m_pParent->ChangeState( CUpgradeDlg::STATE_NORMAL );
		break;

	default:
		break;
	}
	m_dwPrevTime = g_GameDATA.GetGameTime();
}

void CUpgradeDlgStateResult::Update( POINT ptMouse )
{
	if( m_bWaitUserinput ) return;
	DWORD dwCurrTime = g_GameDATA.GetGameTime();

	CUpgrade& Upgrade = CUpgrade::GetInstance();
	BYTE btResult = CUpgrade::GetInstance().GetResult();
	if( m_pResultGuage->Update( ptMouse, m_dwPrevTime, dwCurrTime ) == CGuage::UPDATE_END )
	{
		m_bWaitUserinput = true;

		
		switch( btResult )
		{
		case CRAFT_UPGRADE_SUCCESS:
			{
				CTCmdChangeStateUpgradeDlg* pCmd = new CTCmdChangeStateUpgradeDlg(CUpgradeDlg::STATE_NORMAL );
				g_itMGR.OpenMsgBox(STR_CRAFT_UPGRADE_SUCCESS, CMsgBox::BT_OK , true, 0 , pCmd );///해당 아이템을 보여주자.

				int itemtype = 0;
				int itemno   = 0;
				if( CItemFragment* pTargetItem = CUpgrade::GetInstance().GetTargetItem() )
				{
					itemtype = pTargetItem->GetItem().GetTYPE();
					itemtype = pTargetItem->GetItem().GetItemNO();
				}

				g_pNet->Send_cli_ITEM_RESULT_REPORT( REPORT_ITEM_UPGRADE_SUCCESS, itemtype, itemno );
				break;
			}
		case CRAFT_UPGRADE_FAILED:
			{
				CTCmdChangeStateUpgradeDlg* pCmd = new CTCmdChangeStateUpgradeDlg(CUpgradeDlg::STATE_NORMAL );
#ifdef _NEWINTERFACEKJH
				g_itMGR.OpenMsgBox(STR_CRAFT_FAILED, CMsgBox::BT_OK , true, 0 , pCmd );
#else
				g_itMGR.OpenMsgBox(STR_CRAFT_UPGRADE_FAILED, CMsgBox::BT_OK , true, 0 , pCmd );
#endif			

				int itemtype = 0;
				int itemno   = 0;
				if( CItemFragment* pTargetItem = CUpgrade::GetInstance().GetTargetItem() )
				{
					itemtype = pTargetItem->GetItem().GetTYPE();
					itemtype = pTargetItem->GetItem().GetItemNO();
				}
				g_pNet->Send_cli_ITEM_RESULT_REPORT( REPORT_ITEM_UPGRADE_FAILED, itemtype, itemno );
				break;
			}
		case 0x99:// 07. 03. 08 - 김주현 : 재련이 실패해서 아이템의 등급이 하락.
			{
				CTCmdChangeStateUpgradeDlg* pCmd = new CTCmdChangeStateUpgradeDlg(CUpgradeDlg::STATE_NORMAL );

				g_itMGR.OpenMsgBox(STR_CRAFT_UPGRADE_FAILED, CMsgBox::BT_OK , true, 0 , pCmd );

				int itemtype = 0;
				int itemno   = 0;
				if( CItemFragment* pTargetItem = CUpgrade::GetInstance().GetTargetItem() )
				{
					itemtype = pTargetItem->GetItem().GetTYPE();
					itemtype = pTargetItem->GetItem().GetItemNO();
				}
				g_pNet->Send_cli_ITEM_RESULT_REPORT( REPORT_ITEM_UPGRADE_FAILED, itemtype, itemno );
				break;
			}
		default:
			m_pParent->ChangeState( CUpgradeDlg::STATE_NORMAL );
			break;
		}
	}
		
	#if defined( _NEWINTERFACEKJH )
	if( m_pResultGuage->GetValue() >=  120 )
			m_pResultGuage->SetGuageImageID(m_iGreenGuageImageID);
	#else
    if( m_pResultGuage->GetValue() >=  m_iVirtualSuccessPoint )
		m_pResultGuage->SetGuageImageID(m_iGreenGuageImageID);
	#endif
	m_dwPrevTime = dwCurrTime;
}

void CUpgradeDlgStateResult::Draw()
{
	m_pResultGuage->Draw();
}
void CUpgradeDlgStateResult::MoveWindow( POINT ptPosition )
{
	m_pResultGuage->MoveWindow( ptPosition );
}