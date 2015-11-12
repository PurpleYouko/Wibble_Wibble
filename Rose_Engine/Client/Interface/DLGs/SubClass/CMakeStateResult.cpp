#include "stdafx.h"
#include ".\cmakestateresult.h"
#include "CGuage.h"
#include "../../../../TGameCtrl/ResourceMgr.h"
#include "../../IO_ImageRes.h"
#include "../MakeDLG.h"
#include "../../../Network/CNetwork.h"
#include "../../it_mgr.h"
#include "../../Command/CTCmdHotExec.h"
#include "../../Icon/CIconItem.h"
#include "../../../Game_Func.h"
#include "TGuage.h"

CMakeStateResult::CMakeStateResult( CMakeDLG* pParent )
{ 
	m_pParent = pParent;
	CGuage* pGuage ;
	CTGuage * pTGuage = NULL;

	for( int i = 0; i < g_iMaxCountMaterial; ++i )
	{		
		char * szBuf = CStr::Printf("GUAGE_%02d", i);
		if( pTGuage = (CTGuage*)pParent->Find( CStr::Printf("GUAGE_%02d", i) ) )
		{
			pGuage = new CGuage(pTGuage->GetGuageImageID(), 0, pTGuage->GetModuleID() , pTGuage->GetWidth() );
			pGuage->SetAutoIncrement( 50, 100 );
			pGuage->SetOffset( pTGuage->GetOffset() );
			m_listGuage.push_back( pGuage );
		}		
	}

	m_bWaitState = false;
}

CMakeStateResult::~CMakeStateResult(void)
{
	std::vector<CGuage*>::iterator iter;
	CGuage* pGuage = NULL;
	for( iter =	m_listGuage.begin(); iter != m_listGuage.end();)
	{
		pGuage = *iter;
		iter = m_listGuage.erase( iter );
		delete pGuage;
	}
}

void CMakeStateResult::MoveWindow(POINT ptPos)
{
	std::vector<CGuage*>::iterator iter;
	for( iter =	m_listGuage.begin(); iter != m_listGuage.end(); ++iter )
		(*iter)->MoveWindow( ptPos );
}

void CMakeStateResult::Draw()
{
	for( int i = 0; i < CManufacture::GetInstance().GetMaterialCount(); ++i )
		m_listGuage[i]->Draw();
}
///�������� ������� �����ִ� �������� Update�� �����ڿ��� Update���� �ִ�.
///
void CMakeStateResult::Update( POINT ptMouse)
{
	if( m_bWaitState ) return;
	DWORD dwCurrTime = g_GameDATA.GetGameTime();

	///�ʹ� ���� ���� �Ѿ�� �������� 1���ϰ� ����� �ִ�.
	if( dwCurrTime - m_dwPrevTime < 33 ) 
		return;

	CItemSlot* pItemSlot = g_pAVATAR->GetItemSlot();
	CItem*	   pItem	 = NULL;
	///��� Animation ó��


	for( int i = 0; i < CManufacture::GetInstance().GetMaterialCount(); ++i )
	{
		if( m_listGuage[i]->Update( ptMouse, m_dwPrevTime, dwCurrTime ) == CGuage::UPDATE_END )
		{
			if( m_pParent->m_btRESULT == RESULT_CREATE_ITEM_SUCCESS )
			{
				if( i == CManufacture::GetInstance().GetMaterialCount() - 1 )///�ǹ��� ������ ������ ������ ����
				{

					CManufacture::GetInstance().SubItemsAfterRecvResult( CManufacture::GetInstance().GetMaterialCount() );
					g_pAVATAR->Add_ITEM(m_pParent->m_nStepORInvIDX, m_pParent->m_CreateITEM );
					m_bWaitState = true;

					CTCmdChangeState* pCmd = new CTCmdChangeState( CMakeDLG::STATE_NORMAL );
					//CIcon* pIcon = m_pParent->m_MakeItemSlot.GetIcon();
					//if( pIcon )
					//{

					//	//m_pParent->m_MakeItem.DeleteItemIcon( pIcon );
					//	//
					//	//pItem = pItemSlot->GetItem( m_pParent->m_nStepORInvIDX );
					//	//assert( pItem );

					//	//pIcon = pItem->CreateItemIcon();
					//	//m_pParent->m_MakeItemSlot.AttachIcon( pIcon );
					//	//
					//	//pIcon->SetEnable();

					//}
					g_itMGR.OpenMsgBox(STR_SUCCESS_MAKE_ITEM,CMsgBox::BT_OK, true, m_pParent->GetDialogType(), pCmd, NULL );

					int itemtype = 0;
					int itemno   = 0;
					if( CItem* pItem = CManufacture::GetInstance().GetMakeItem() )
					{
						itemtype = pItem->GetItem().GetTYPE();
						itemno   = pItem->GetItem().GetItemNO();
					}
					
					g_pNet->Send_cli_ITEM_RESULT_REPORT( REPORT_ITEM_CREATE_SUCCESS, itemtype, itemno );

					break;
				}
			}
			else if( m_pParent->m_btRESULT == RESULT_CREATE_ITEM_FAILED )
			{
				if( i == m_pParent->m_nStepORInvIDX )
				{
					CManufacture::GetInstance().SubItemsAfterRecvResult( m_pParent->m_nStepORInvIDX + 1);
					m_bWaitState = true;

					CTCmdChangeState* pCmd = new CTCmdChangeState( CMakeDLG::STATE_NORMAL );
					g_itMGR.OpenMsgBox(STR_FAIL_MAKE_ITEM,CMsgBox::BT_OK, true, m_pParent->GetDialogType(), pCmd, NULL );

					int itemtype = 0;
					int itemno   = 0;
					if( CItem* pItem = CManufacture::GetInstance().GetMakeItem() )
					{
						itemtype = pItem->GetItem().GetTYPE();
						itemno   = pItem->GetItem().GetItemNO();
					}
					g_pNet->Send_cli_ITEM_RESULT_REPORT( REPORT_ITEM_CREATE_FAILED, itemtype, itemno );
					break;
				}
			}
			continue;
		}

		break;
	}

	m_dwPrevTime = dwCurrTime;
}

void CMakeStateResult::Show()
{
}

void CMakeStateResult::Hide()
{
}

void CMakeStateResult::Init()
{
	std::vector<CGuage*>::iterator iter;
	for( iter =	m_listGuage.begin(); iter != m_listGuage.end(); ++iter )
	{
		(*iter)->SetValue(0);	
	}

	m_dwPrevTime = g_GameDATA.GetGameTime();
	m_bWaitState = false;
	///m_pParent�� ����� ������� �����ͼ� ó���Ѵ�.

	switch( m_pParent->m_btRESULT )
	{
	case RESULT_CREATE_ITEM_SUCCESS:		// ���� :Animation����
		{
			for( int i = 0; i < CManufacture::GetInstance().GetMaterialCount(); ++i )
				m_listGuage[i]->SetAutoIncrementMaxValue( m_pParent->m_nPRO_POINT[i] / 10 );
			
			break;
		}
	case RESULT_CREATE_ITEM_FAILED:			// ���� :Animation����
		{
			for( int i = 0; i < CManufacture::GetInstance().GetMaterialCount(); ++i )
				m_listGuage[i]->SetAutoIncrementMaxValue( m_pParent->m_nPRO_POINT[i] / 10 );
			
			break;
		}
	case RESULT_CREATE_ITEM_INVALID_CONDITION:		// ������ �����ϴ�.
		{
			m_bWaitState = true;		
			CTCmdChangeState* pCmd = new CTCmdChangeState( CMakeDLG::STATE_NORMAL );
			g_itMGR.OpenMsgBox(STR_NOT_ENOUGH_MANA,CMsgBox::BT_OK, true, m_pParent->GetDialogType(), pCmd, NULL );
			break;
		}
	case RESULT_CREATE_ITEM_NEED_ITEM:		// �Ҹ�� �������� �����ϴ�
		{
			m_bWaitState = true;		
			CTCmdChangeState* pCmd = new CTCmdChangeState( CMakeDLG::STATE_NORMAL );
			g_itMGR.OpenMsgBox(STR_NOT_ENOUGH_MATERIAL,CMsgBox::BT_OK, true, m_pParent->GetDialogType(), pCmd, NULL );
			CManufacture::GetInstance().SubItemsAfterRecvResult( m_pParent->m_nStepORInvIDX );
			break;
		}
	case RESULT_CREATE_ITEM_INVALID_ITEM:	// �Ҹ�� �������� �������ϴ�
		{
			m_bWaitState = true;		
			CTCmdChangeState* pCmd = new CTCmdChangeState( CMakeDLG::STATE_NORMAL );
			g_itMGR.OpenMsgBox(STR_NOT_EXIST_MATERIAL,CMsgBox::BT_OK, true, m_pParent->GetDialogType(), pCmd, NULL );
			CManufacture::GetInstance().SubItemsAfterRecvResult( m_pParent->m_nStepORInvIDX );
			break;
		}
	case RESULT_CREATE_ITEM_NEED_SKILL_LEV:	// ���� ��ų ������ �����ϴ�
		{
			m_bWaitState = true;		
			CTCmdChangeState* pCmd = new CTCmdChangeState( CMakeDLG::STATE_NORMAL );
			g_itMGR.OpenMsgBox(STR_NOT_ENOUGH_MAKE_SKILL_LEVEL,CMsgBox::BT_OK, true, m_pParent->GetDialogType(), pCmd, NULL );
			break;
		}
	default:
		{
			m_bWaitState = true;		
			CTCmdChangeState* pCmd = new CTCmdChangeState( CMakeDLG::STATE_NORMAL );
//			g_itMGR.OpenMsgBox("",CMsgBox::BT_OK, CMsgBox::TDS_MODAL, m_pParent->GetControlID(), pCmd, NULL );
			break;
		}
	}
}

unsigned int CMakeStateResult::Process(UINT uiMsg,WPARAM wParam,LPARAM lParam )
{
	return 0;
}

bool CMakeStateResult::IsModal()
{
	return true;
}

