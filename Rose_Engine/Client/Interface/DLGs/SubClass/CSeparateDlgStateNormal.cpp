#include "stdafx.h"
#include ".\cseparatedlgstatenormal.h"
#include "../cseparateDlg.h"
#include "../../../GameData/cseparate.h"
#include "../../it_mgr.h"
#include "../../../object.h"



CSeparateDlgStateNormal::CSeparateDlgStateNormal(CSeparateDlg* pParent)
{
	m_pParent = pParent;
}

CSeparateDlgStateNormal::~CSeparateDlgStateNormal(void)
{

}
void CSeparateDlgStateNormal::Enter()
{

}

void CSeparateDlgStateNormal::Leave()
{

}

void CSeparateDlgStateNormal::Update( POINT ptMouse )
{

}

void CSeparateDlgStateNormal::Draw()
{

}

unsigned CSeparateDlgStateNormal::Process( unsigned uiMsg, WPARAM wParam, LPARAM lParam )
{

	if(	m_pParent->m_MaterialItemSlot.Process( uiMsg, wParam, lParam ) )
		return uiMsg;
	
	for( iter = m_pParent->m_OutputItemSlots.begin(); iter != m_pParent->m_OutputItemSlots.end(); ++iter )
	{
		if(iter->Process( uiMsg, wParam, lParam ))
		return uiMsg;
	}

	if( unsigned uiProcID = m_pParent->CTDialog::Process( uiMsg, wParam, lParam ) )
	{
		if( uiMsg == WM_LBUTTONUP )
		{
			switch(	uiProcID )
			{
			case IID_BTN_START:

				if( g_pAVATAR )
				{
				
					if( g_pAVATAR->Get_STATE() != CS_STOP )
					{
						g_itMGR.AppendChatMsg( STR_ACTION_COMMAND_STOP_STATE_FAILED, IT_MGR::CHAT_TYPE_SYSTEM );
						break;
					}

					if( g_pAVATAR->GetPetMode() >= 0 )
					{
						if( ( g_pAVATAR->GetPetState() != CS_STOP ) )
						{
							g_itMGR.AppendChatMsg( STR_ACTION_COMMAND_STOP_STATE_FAILED, IT_MGR::CHAT_TYPE_SYSTEM );
							break;
						}
					}
				}
				if(m_pParent->CheckInvEmptySlot())
				{
				if( CSeparate::GetInstance().SendPacketSeparate() )
					{
						m_pParent->ChangeState( CSeparateDlg::STATE_WAIT );
					}
				}
				break;

			case IID_BTN_CLOSE:
				m_pParent->Hide();
				break;
			default:
				break;
			}
		}
		return uiMsg;
	}
	return 0;
}
