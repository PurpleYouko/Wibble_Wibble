#include "stdafx.h"
#include ".\partyoptiondlg.h"
#include "../../GameData/CParty.h"
#include "../../GameData/Event/CTEventParty.h"
#include "../interface/it_mgr.h"
#include "../interface/TypeResource.h"
#include "../../GameCommon/StringManager.h"
#include "../../CClientStorage.h"


#include "TImage.h"
#include "TButton.h"
#include "TRadioBox.h"
#include "TCheckBox.h"
CPartyOptionDlg::CPartyOptionDlg(void)
{
	m_party_rule = 0;
}

CPartyOptionDlg::~CPartyOptionDlg(void)
{
}

unsigned CPartyOptionDlg::Process( unsigned uiMsg, WPARAM wParam, LPARAM lParam )
{
	if( !IsVision() ) return 0;

	if( unsigned uiProcID = CTDialog::Process( uiMsg, wParam, lParam ) )
	{
		switch( uiMsg )
		{
		case WM_LBUTTONUP:
			OnLButtonUp( uiProcID );
			break;
		default:
			break;
		}
		return uiMsg;
	}
	return 0;
}

void CPartyOptionDlg::OnLButtonUp( unsigned uiProcID )
{
	switch( uiProcID )
	{
	case IID_BTN_CLOSE:
		Hide();
		break;
	case IID_BTN_CONFIRM:
		{
			CWinCtrl* pCtrl = Find( IID_CHECKBOX_SHOW_PARTYMEMBER_HPGUAGE );
			if( pCtrl && pCtrl->GetControlType() == CTRL_CHECKBOX )
			{
				CTCheckBox* p =(CTCheckBox*)pCtrl;
				if( p->IsCheck() )
					g_ClientStorage.SetShowPartyMemberHpGuage( true );
				else
					g_ClientStorage.SetShowPartyMemberHpGuage( false );
			}

			if( !CParty::GetInstance().IsPartyLeader() )///파티장이 아닐경우에는 다른 옵션 변경은 무시( 정상적으로는 변하지도 않는다. );
			{
				Hide();			
				break;
			}

			BYTE btPartyRule = 0;
			pCtrl = Find( IID_RADIOBOX_ITEM );
			assert( pCtrl );
			if( pCtrl == NULL ) break;
			if( pCtrl->GetControlType() != CTRL_RADIOBOX ) break;

			CTRadioBox* pRadioBox = (CTRadioBox*)pCtrl;
			UINT uiPressedBtn = pRadioBox->GetPressedButtonID();
			switch( uiPressedBtn )
			{
			case IID_RADIOBUTTON_ITEM_PICK:
				break;
			case IID_RADIOBUTTON_ITEM_SEQUENCE:
				btPartyRule |= BIT_PARTY_RULE_ITEM_TO_ORDER;
				break;
			default:
				assert( 0 && "Invalid Selected RadioButton" );
				break;
			}

			///유저가 입력한 값을 이전 값으로 돌리고 갱신이 될경우에는 서버에서 받아서 바꾸도록한다.
			if( m_party_rule & BIT_PARTY_RULE_ITEM_TO_ORDER )
				pRadioBox->SetPressedButton( IID_RADIOBUTTON_ITEM_SEQUENCE );
			else
				pRadioBox->SetPressedButton( IID_RADIOBUTTON_ITEM_PICK );


			
			pCtrl = Find( IID_RADIOBOX_EXP );
			assert( pCtrl );
			if( pCtrl == NULL ) break;
			if( pCtrl->GetControlType() != CTRL_RADIOBOX ) break;

			pRadioBox = (CTRadioBox*)pCtrl;
			uiPressedBtn = pRadioBox->GetPressedButtonID();
			switch( uiPressedBtn )
			{
			case IID_RADIOBOX_EXP_EQUALITY:
				break;
			case IID_RADIOBOX_EXP_RATIO_LEVEL:
				btPartyRule |= BIT_PARTY_RULE_EXP_PER_PLAYER;
				break;
			default:
				assert( 0 && "Invalid Selected RadioButton" );
				break;
			}

			///유저가 입력한 값을 이전 값으로 돌리고 갱신이 될경우에는 서버에서 받아서 바꾸도록한다.
			if( m_party_rule & BIT_PARTY_RULE_EXP_PER_PLAYER )
				pRadioBox->SetPressedButton( IID_RADIOBOX_EXP_RATIO_LEVEL );
			else
				pRadioBox->SetPressedButton( IID_RADIOBOX_EXP_EQUALITY );


			CParty::GetInstance().SendChangePartyRule( btPartyRule );
			Hide();
		}
		break;
	default:
		break;
	}
}
void CPartyOptionDlg::Update( CObservable* pObservable, CTObject* pObj )
{
	assert( pObservable );
	assert( pObj );
	if( pObj == NULL )
		return;

	if( strcmp( pObj->toString(), "Party" ) )
	{
		assert( 0 && "Event Type is Invalid @PartyDlg" );	
		return;
	}

	CTEventParty* pEvent = (CTEventParty*)pObj;
	CParty& Party = CParty::GetInstance();
	int iObjSvrIdx = pEvent->GetObjSvrIdx();
	switch( pEvent->GetID() )
	{
	case CTEventParty::EID_CHANGE_RULE:
		{
			BYTE btRule		= pEvent->GetRule();
			m_party_rule	= btRule;
			g_itMGR.AppendChatMsg( STR_CHANGED_PARTY_CONFIG, IT_MGR::CHAT_TYPE_SYSTEM );

			CWinCtrl* pCtrl = Find( IID_RADIOBOX_ITEM );
			assert( pCtrl );
			if( pCtrl == NULL ) break;
			if( pCtrl->GetControlType() != CTRL_RADIOBOX ) break;

			CTRadioBox* pRadioBox = (CTRadioBox*)pCtrl;
			
			if( btRule & BIT_PARTY_RULE_ITEM_TO_ORDER )
			{
				g_itMGR.AppendChatMsg(CStr::Printf("%s: %s", STR_ITEM_PICKUP_RIGHT, STR_PARTY_ITEM_PICKUP_SEQENCE) , IT_MGR::CHAT_TYPE_SYSTEM  );
				pRadioBox->SetPressedButton( IID_RADIOBUTTON_ITEM_SEQUENCE );
			}
			else
			{
				g_itMGR.AppendChatMsg(CStr::Printf( "%s: %s",STR_ITEM_PICKUP_RIGHT, STR_PARTY_ITEM_PICKUP_RIGHT_ALL), IT_MGR::CHAT_TYPE_SYSTEM );
				pRadioBox->SetPressedButton( IID_RADIOBUTTON_ITEM_PICK );
			}

			pCtrl = Find( IID_RADIOBOX_EXP );
			assert( pCtrl );
			if( pCtrl == NULL ) break;
			if( pCtrl->GetControlType() != CTRL_RADIOBOX ) break;
			pRadioBox = (CTRadioBox*)pCtrl;

			if( btRule & BIT_PARTY_RULE_EXP_PER_PLAYER )
			{
				g_itMGR.AppendChatMsg(CStr::Printf("%s: %s", CStringManager::GetSingleton().GetAbility( AT_EXP ), STR_PARTY_EXP_LEVELRATE), IT_MGR::CHAT_TYPE_SYSTEM );
				pRadioBox->SetPressedButton( IID_RADIOBOX_EXP_RATIO_LEVEL );
			}
			else
			{
				g_itMGR.AppendChatMsg(CStr::Printf("%s: %s", 
						CStringManager::GetSingleton().GetAbility( AT_EXP ),
						STR_PARTY_EXP_EQUALITY), 
						IT_MGR::CHAT_TYPE_SYSTEM 
				);
				pRadioBox->SetPressedButton( IID_RADIOBOX_EXP_EQUALITY );
			}
		}
		break;
	case CTEventParty::EID_CHANGE_LEADER:
		{
			if( Party.IsPartyLeader() )
			{
				SetEnableChild( IID_RADIOBUTTON_ITEM_PICK, true );
				SetEnableChild( IID_RADIOBUTTON_ITEM_SEQUENCE, true );
				SetEnableChild( IID_RADIOBOX_EXP_EQUALITY, true );
				SetEnableChild( IID_RADIOBOX_EXP_RATIO_LEVEL, true );
			}
			else
			{
				SetEnableChild( IID_RADIOBUTTON_ITEM_PICK, false );
				SetEnableChild( IID_RADIOBUTTON_ITEM_SEQUENCE, false );
				SetEnableChild( IID_RADIOBOX_EXP_EQUALITY, false );
				SetEnableChild( IID_RADIOBOX_EXP_RATIO_LEVEL, false );
			}
		}
		break;
	default:
		break;
	}

}

void CPartyOptionDlg::Draw()
{
	if( !IsVision() ) return;
	CTDialog::Draw();
}

void CPartyOptionDlg::Show()
{


#ifdef _DEBUG

	POINT pt = { GetPosition().x, GetPosition().y };
	Clear();
	Create("DlgPartyOption");		
	SetInterfacePos_After();
	MoveWindow(pt);

#endif

	CWinCtrl * pCtrl = NULL;
	
	if( pCtrl = Find(IID_RADIOBOX_ITEM) )
	{
		CTRadioBox* pRadioBox = (CTRadioBox*)pCtrl;
		if( m_party_rule & BIT_PARTY_RULE_ITEM_TO_ORDER )
		{
			pRadioBox->SetPressedButton( IID_RADIOBUTTON_ITEM_SEQUENCE );
		}
		else
		{
			pRadioBox->SetPressedButton( IID_RADIOBUTTON_ITEM_PICK );
		}
	}
	
	if( pCtrl = Find(IID_RADIOBOX_EXP) )
	{
		CTRadioBox* pRadioBox = (CTRadioBox*)pCtrl;
		if( m_party_rule & BIT_PARTY_RULE_EXP_PER_PLAYER )
		{
			pRadioBox->SetPressedButton( IID_RADIOBOX_EXP_RATIO_LEVEL );
		}
		else
		{
			pRadioBox->SetPressedButton( IID_RADIOBOX_EXP_EQUALITY );
		}
	}


	CTDialog::Show();
	if( pCtrl = Find( IID_CHECKBOX_SHOW_PARTYMEMBER_HPGUAGE ) )
	{
		if( pCtrl->GetControlType() == CTRL_CHECKBOX )
		{
			CTCheckBox* p =(CTCheckBox*)pCtrl;
			if( g_ClientStorage.IsShowPartyMemberHpGuage() )
				p->SetCheck();
			else
				p->SetUncheck();
		}
	}
}
	
bool CPartyOptionDlg::Create( const char* IDD )
{
	if( CTDialog::Create( IDD ) )
	{
		return true;
	}
	return false;
}

void CPartyOptionDlg::SetInterfacePos_After()
{
	CWinCtrl * pCtrl = NULL;
	
    if( pCtrl = Find("EXP_DISTRIBUTE"))
	{
		((CTImage*)pCtrl)->SetText(STR_PARTYEXP_DISTRIBUTE_FORM);
		((CTImage*)pCtrl)->SetAlign( DT_VCENTER | DT_LEFT );
	}
	if( pCtrl = Find("EXP_EQUALITY"))
	{
		((CTImage*)pCtrl)->SetText(STR_PARTY_EXP_EQUALITY);
		((CTImage*)pCtrl)->SetAlign( DT_VCENTER | DT_LEFT );
	}
	if( pCtrl = Find("EXP_LEVELRATE"))
	{
		((CTImage*)pCtrl)->SetText(STR_PARTY_EXP_LEVELRATE);
		((CTImage*)pCtrl)->SetAlign( DT_VCENTER | DT_LEFT );
	}
	if( pCtrl = Find("PICKUP_RIGHT"))
	{
		((CTImage*)pCtrl)->SetText(STR_ITEM_PICKUP_RIGHT);
		((CTImage*)pCtrl)->SetAlign( DT_VCENTER | DT_LEFT );
	}
	if( pCtrl = Find("PICKUP_RIGHT_ALL"))
	{
		((CTImage*)pCtrl)->SetText(STR_PARTY_ITEM_PICKUP_RIGHT_ALL);
		((CTImage*)pCtrl)->SetAlign( DT_VCENTER | DT_LEFT );
	}
	if( pCtrl = Find("PICKUP_RIGHT_SEQENCE"))
	{
		((CTImage*)pCtrl)->SetText(STR_PARTY_ITEM_PICKUP_SEQENCE);
		((CTImage*)pCtrl)->SetAlign( DT_VCENTER | DT_LEFT );
	}
	if( pCtrl = Find("PARTY_HPGUAGE"))
	{
		((CTImage*)pCtrl)->SetText(STR_SHOW_PARTYMEMBER_HPGUAGE);
		((CTImage*)pCtrl)->SetAlign( DT_VCENTER | DT_LEFT );
	}
	//문자열 추가.
	if( pCtrl = Find(IID_BTN_CONFIRM))
	{
		//확인 858
		((CTImage*)pCtrl)->SetText( LIST_STRING(858) );
	}
}