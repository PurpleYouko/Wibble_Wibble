#include "stdafx.h"
#include ".\cseparatedlgstateresult.h"
#include ".\cseparatedlgstatenormal.h"
#include "../CSeparateDlg.h"
#include "../../../Network/Net_prototype.h"
#include "CGuage.h"
#include "../../../GameData/CSeparate.h"
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

CSeparateDlgStateResult * CSeparateDlgStateResult::s_Instance = NULL;

CSeparateDlgStateResult::CSeparateDlgStateResult( CSeparateDlg* pParent )
{
	this->s_Instance = this;
	m_pParent = pParent;
	m_iRedGuageImageID = CResourceMgr::GetInstance()->GetImageNID( UI_IMAGE_RES_ID,"ID_GUAGE_MAKE_RED" );
	m_iGreenGuageImageID  = CResourceMgr::GetInstance()->GetImageNID( UI_IMAGE_RES_ID,"ID_GUAGE_MAKE_GREEN" );
	int iModuleID	    = UI_IMAGE_RES_ID;
	int iGuageWidth		= 88;

	POINT	ptOffset	= {23,113};

	for( int i = 0; i < iMaxCountMaterial; ++i , ptOffset.y += 52 )
	{
		pGuage = new CGuage(m_iRedGuageImageID, 0, iModuleID , iGuageWidth );
		pGuage->SetOffset( ptOffset );
		pGuage->SetAutoIncrement( 50, 100 );
		m_listGuage.push_back( pGuage );
	}

	m_bCheckUpdate = false;
	m_bStartSeparate = false;
}

CSeparateDlgStateResult::~CSeparateDlgStateResult(void)
{
	std::vector<CGuage*>::iterator iter;
	CGuage* pGuage = NULL;
	for( iter =	m_listGuage.begin(); iter != m_listGuage.end();)
	{
		pGuage = *iter;
		iter = m_listGuage.erase( iter );
		delete pGuage;
		pGuage = NULL;
	}
	s_Instance = NULL;
}
void CSeparateDlgStateResult::Leave()
{
	m_viewitemnum = 0;
	m_bWaitState = false;

	m_bCheckUpdate = false;

	BYTE btResult = CSeparate::GetInstance().GetResult();

	m_bStartSeparate = false;
	this->s_Instance = this;

	switch( btResult )
	{
	case CRAFE_BREAKUP_SUCCESS:
//		CSeparate::GetInstance().RemoveItem();
		CSeparate::GetInstance().ApplyResultItemSet();

//		g_itMGR.OpenMsgBox(STR_CRAFT_UPGRADE_SUCCESS);///해당 아이템을 보여주자.
		break;
	case CRAFT_UPGRADE_INVALID_MAT:
		g_itMGR.OpenMsgBox( STR_CRAFT_UPGRADE_INVALID_MAT);
		break;
	default:
		break;
	}
}

void CSeparateDlgStateResult::Enter()
{
	m_viewitemnum = -1;
	m_bWaitState = false;

	CSeparate::GetInstance().RunSettingOutputItem();

	m_bStartSeparate = true;  

	this->s_Instance = this;



	std::vector<CGuage*>::iterator iter;
	for( iter =	m_listGuage.begin(); iter != m_listGuage.end(); ++iter )
	{
		(*iter)->SetValue(0);
		(*iter)->SetGuageImageID(m_iRedGuageImageID);
	}

	switch( CSeparate::GetInstance().GetResult() )
	{
	case CRAFE_BREAKUP_SUCCESS:		// 성공 :Animation진행
		{
			int a = CSeparate::GetInstance().GetMaterialCount();
			for( int i = 0; i < CSeparate::GetInstance().GetMaterialCount(); ++i )
			m_listGuage[i]->SetAutoIncrementMaxValue( 120 );
			break;
		}
	default:
		{
//			CTCmdChangeState* pCmd = new CTCmdChangeState( CSeparateDlg::STATE_NORMAL );
//			m_bWaitState = true;		
//			CSeparateDlg::GetInstance().ChangeState( CSeparateDlg::STATE_NORMAL );			
//			g_itMGR.OpenMsgBox("",CMsgBox::BT_OK, CMsgBox::TDS_MODAL, m_pParent->GetControlID(), pCmd, NULL );
			break;
		}
	}
	m_dwPrevTime = g_GameDATA.GetGameTime();

}

void CSeparateDlgStateResult::Update( POINT ptMouse )
{
	if( m_bWaitState ) return;
	DWORD dwCurrTime = g_GameDATA.GetGameTime();

	///너무 작은 값이 넘어가서 증가값이 1이하가 생길수 있다.
	if( dwCurrTime - m_dwPrevTime < 33 ) 
		return;

	CItemSlot* pItemSlot = g_pAVATAR->GetItemSlot();
	CItem*	   pItem	 = NULL;
	///결과 Animation 처리

	int a = CSeparate::GetInstance().GetResultCnt();
	for( int i = 0; i < CSeparate::GetInstance().GetResultCnt(); ++i )
	{
		if( m_listGuage[i]->Update( ptMouse, m_dwPrevTime, dwCurrTime ) == CGuage::UPDATE_END )
		{

			if( CSeparate::GetInstance().GetResult() == CRAFE_BREAKUP_SUCCESS )
			{
					if(m_viewitemnum <= i - 1 )
						m_viewitemnum = i;
				this->s_Instance = this;

				if( i == CSeparate::GetInstance().GetResultCnt() - 2)///맨밑의 게이지 갱신이 끝까지 갔다
				{
				m_bWaitState = true;		
				CTCmdChangeStateSeparateDlg* pCmd = new CTCmdChangeStateSeparateDlg(CSeparateDlg::STATE_NORMAL );				
//				g_itMGR.OpenMsgBox(STR_CRAFE_BREAKUP_SUCCESS,CMsgBox::BT_OK, true, m_pParent->GetDialogType(), pCmd, NULL );
				g_itMGR.OpenMsgBox2(CSeparate::GetInstance().GetCreateMsgBoxData());
				m_bCheckUpdate = true;
				this->s_Instance = this;
				
				int itemtype;
				int itemno;

				if( CItemFragment* pTargetItem = CSeparate::GetInstance().GetTargetItem() )
				{
					itemtype = pTargetItem->GetItem().GetTYPE();
					itemno = pTargetItem->GetItem().GetItemNO();
				}

				g_pNet->Send_cli_ITEM_RESULT_REPORT( REPORT_ITEM_CREATE_SUCCESS, itemtype, itemno );
				break;
				}
			}	
			continue;
		}

		if( m_listGuage[i]->GetValue() >= 120 )
			m_listGuage[i]->SetGuageImageID(m_iGreenGuageImageID);

		break;
	}
	m_dwPrevTime = dwCurrTime;

}

void CSeparateDlgStateResult::Draw()
{
	for( int i = 0; i < CSeparate::GetInstance().GetMaterialCount(); ++i )
		m_listGuage[i]->Draw();
}
void CSeparateDlgStateResult::MoveWindow( POINT ptPosition )
{
	std::vector<CGuage*>::iterator iter;
	for( iter =	m_listGuage.begin(); iter != m_listGuage.end(); ++iter )
		(*iter)->MoveWindow( ptPosition );
}

bool CSeparateDlgStateResult::IsModal()
{
	return true;
}

int CSeparateDlgStateResult::GetViewNum()
{
	return m_viewitemnum;
}

CSeparateDlgStateResult* CSeparateDlgStateResult::GetInstance()
{
	return s_Instance;
}

bool CSeparateDlgStateResult::GetStartSeparateState()
{
	return m_bStartSeparate;
}

bool CSeparateDlgStateResult::GetCheckUpdate()
{
	return m_bCheckUpdate;
}