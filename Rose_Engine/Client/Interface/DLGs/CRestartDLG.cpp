#include "stdafx.h"
#include "CRestartDLG.h"
#include "..\\..\\NetWork\\CNetwork.h"
#include "../../Country.h"
#include "../../IO_Terrain.h"
#include "TListBox.h"

CRestartDLG::CRestartDLG()
{
	// Construct
}

CRestartDLG::~CRestartDLG()
{
	// Destruct0
}
 
unsigned int CRestartDLG::Process(UINT uiMsg,WPARAM wParam,LPARAM lParam)
{
	unsigned iProcID = 0;
	if( iProcID = CTDialog::Process(uiMsg,wParam,lParam)) 
	{
		switch(uiMsg)
		{
		case WM_LBUTTONUP:
			if(On_LButtonUP( iProcID ))
				return uiMsg;
			break;
		default:
			break;
		}
		return uiMsg;
	}
	return 0;
}

bool CRestartDLG::On_LButtonUP( unsigned iProcID )
{
	switch( iProcID )
	{
	case BTN_RESTART_SAVE_POSITION:
	case BTN_RESTART_DUNGEON_GATE:
		g_pNet->Send_cli_REVIVE_REQ( REVIVE_TYPE_SAVE_POS );
		Hide();
		break;
	case BTN_RESTART_CURRENT_FIELD:
		g_pNet->Send_cli_REVIVE_REQ( REVIVE_TYPE_REVIVE_POS );
		Hide();
		break;
	}
	

#ifdef __VIRTUAL_SERVER
	g_pAVATAR->Set_HP( g_pAVATAR->Get_MaxHP() );
#endif

	return true;
}


void CRestartDLG::Draw()
{
	if( !IsVision() ) return;
	CTDialog::Draw();
}

void CRestartDLG::Show()
{

#ifdef _DEBUG

	POINT pt = { GetPosition().x, GetPosition().y };
	Clear();
	Create("DlgRestart");		
	SetInterfacePos_After();
	MoveWindow(pt);
	
#endif

	CTDialog::Show();


	CWinCtrl* pCtrl = NULL;

#ifdef __KOREA
	//홍근 : 클랜 필드에서는 저장된 장소 부활만 가능.
	if( g_pTerrain->GetZoneNO() >= 11 && g_pTerrain->GetZoneNO() <= 13 )
	{
		if( pCtrl = Find( BTN_RESTART_CURRENT_FIELD ) )
			pCtrl->Hide();
	}
#endif

	if( IsApplyNewVersion() )
	{
		//홍근 : 클랜 필드에서는 저장된 장소 부활만 가능.
		if( pCtrl = Find( BTN_RESTART_CURRENT_FIELD ) )
			pCtrl->Show();
		
		if( ZONE_IS_UNDERGROUND( g_pTerrain->GetZoneNO() ) )
		{
			if( pCtrl = Find( BTN_RESTART_SAVE_POSITION ) )
				pCtrl->Hide();
			if( pCtrl = Find( BTN_RESTART_DUNGEON_GATE ) )
				pCtrl->Show();
		}
		else
		{
			if( pCtrl = Find( BTN_RESTART_SAVE_POSITION ) )
				pCtrl->Show();
			if( pCtrl = Find( BTN_RESTART_DUNGEON_GATE ) )
				pCtrl->Hide();

			//홍근 : 클랜 필드에서는 저장된 장소 부활만 가능.
			if( g_pTerrain->GetZoneNO() >= 11 && g_pTerrain->GetZoneNO() <= 13 )
			{
				if( pCtrl = Find( BTN_RESTART_CURRENT_FIELD ) )
					pCtrl->Hide();
			}
			//20060810 홍근 : 클랜전 필드에서는 저장된 장소 부활만 가능.
			if( g_pTerrain->GetZoneNO() >= 101 && g_pTerrain->GetZoneNO() <= 150 )
			{
				if( pCtrl = Find( BTN_RESTART_SAVE_POSITION ) )
					pCtrl->Hide();
			}
		}
	}

}


void CRestartDLG::SetInterfacePos_After()
{
	CWinCtrl * pCtrl = NULL;

	//문자열 추가
	if( pCtrl = Find(BTN_RESTART_SAVE_POSITION) )
	{
		//저장된 마을 900
		pCtrl->SetText( LIST_STRING(900) );
	}
	if( pCtrl = Find(BTN_RESTART_CURRENT_FIELD) )
	{
		//현 재 필 드 901
		pCtrl->SetText( LIST_STRING(901) );
	}
	if( pCtrl = Find(BTN_RESTART_DUNGEON_GATE) )
	{
		//던전 입구로 902
		pCtrl->SetText( LIST_STRING(902) );
	}
	if( pCtrl = Find("LISTBOX") )
	{
		//어느 시작 위치에서 다시 시작하겠습니까? 903
		((CTListBox*)pCtrl)->ClearText();
		((CTListBox*)pCtrl)->AppendText( LIST_STRING(903) , g_dwGREEN );
	}
	//부활 위치 904
	GetCaption()->SetString( LIST_STRING(904) );
}
