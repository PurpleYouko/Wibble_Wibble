#include "stdafx.h"
#include "CSystemDLG.h"
#include "../../Network/CNetwork.h"
#include "../../System/CGame.h"
#include "../../GameData/CParty.h"
#include "../Command/CTCmdHotExec.h"
#include "../it_mgr.h"

CSystemDLG::CSystemDLG()
{

}

CSystemDLG::~CSystemDLG()
{

}


unsigned int CSystemDLG::Process(UINT uiMsg,WPARAM wParam,LPARAM lParam)
{
	if(!IsVision()) return 0;	

	unsigned iProcID = 0;
	if( iProcID = CTDialog::Process(uiMsg,wParam,lParam)) 
	{
		switch(uiMsg)
		{
		case WM_LBUTTONUP:
			if(On_LButtonUP( iProcID  ))
				return uiMsg;
			break;
		}
		return uiMsg;
	}
	return 0;
}


bool CSystemDLG::On_LButtonUP( unsigned iProcID)
{
	switch( iProcID )
	{
		case IID_BTN_EXIT:
			{
				Hide();

				if( g_GameDATA.GetGameTime() - g_pAVATAR->GetLastBattleTime() <= 10 * 1000 )
				{
					g_itMGR.AppendChatMsg( STR_CANT_EXITGAME, IT_MGR::CHAT_TYPE_SYSTEM );
				}
				else
				{
					g_itMGR.SetWaitDisconnectType( 0 );
					g_pNet->Send_cli_LOGOUT_REQ();
				}
			}
			break;
		case IID_BTN_CONTINUE:
		case IID_BTN_CLOSE:
			Hide();
			break;
		case IID_BTN_GOTO_SELECTAVT:
			Hide();
			if( g_GameDATA.GetGameTime() - g_pAVATAR->GetLastBattleTime() <= 10 * 1000 )
			{
				g_itMGR.AppendChatMsg( STR_CANT_EXITGAME, IT_MGR::CHAT_TYPE_SYSTEM );
			}
			else
			{
				g_itMGR.SetWaitDisconnectType( 1 );
				g_pNet->Send_cli_CHAR_CHANGE();
			}
			break;
		case IID_BTN_OPTION:
			{
				Hide();
				g_itMGR.OpenDialog( DLG_TYPE_OPTION );
			}
			break;
		default:
			break;
	}
	return true;
}


void CSystemDLG::SetInterfacePos_After()
{
	CWinCtrl * pCtrl = NULL;

	//문자열 추가.
	if( pCtrl = Find(IID_BTN_CONTINUE) )
	{
		//게임으로 돌아가기 905
        pCtrl->SetText(LIST_STRING(905)  );
		pCtrl->SetAlign(DT_VCENTER | DT_CENTER);
	}
	if( pCtrl = Find(IID_BTN_GOTO_SELECTAVT) )
	{
		//캐릭터 선택화면가기 906
		pCtrl->SetText( LIST_STRING(906) );
		pCtrl->SetAlign(DT_VCENTER | DT_CENTER);
	}
	if( pCtrl = Find(IID_BTN_EXIT) )
	{
		//게임 종료하기 907
		pCtrl->SetText( LIST_STRING(907) );
		pCtrl->SetAlign(DT_VCENTER | DT_CENTER);
	}
	if( pCtrl = Find(IID_BTN_OPTION) )
	{
		//게 임 옵 션 908
		pCtrl->SetText( LIST_STRING(908) );
		pCtrl->SetAlign(DT_VCENTER | DT_CENTER);
	}
	//시스템 831
	GetCaption()->SetString( LIST_STRING(831) );
}

