#include "stdafx.h"
#include ".\citstatenormal.h"
#include "../it_mgr.h"
#include "../CDragNDropMgr.h"
#include "../Dlgs/ChattingDLG.h"
#include "../Dlgs/CMinimapDLG.h"

#include "../Icon/CIconDialog.h"
#include "../../System/CGame.h"

#include "TEditBox.h"
#include "TControlMgr.h"

CITStateNormal::CITStateNormal(void)
{
	m_iID = IT_MGR::STATE_NORMAL;
}

CITStateNormal::~CITStateNormal(void)
{

}

void CITStateNormal::Enter()
{
//ȫ�� : �����߿� Ű���� �޼��� ����. �����߿� ���� ��� ���ϰ�.	
	CGame::GetInstance().ClearWindowMsgQ();
}

unsigned CITStateNormal::Process( unsigned uiMsg, WPARAM wParam, LPARAM lParam )
{
	UINT uiRet = 0;

	/// Ctrl���� ����ó���� ���� Setting
	if( uiMsg == WM_LBUTTONUP )
		CWinCtrl::SetMouseExclusiveCtrl( NULL );


	list_dlgs_ritor ritorDlgs;
	CTDialog*		pDlg = NULL;
	int				iProcessDialogType = 0;

	/// �Ϲ����� ���̾�α� ó��
	for( ritorDlgs = g_itMGR.m_Dlgs.rbegin(); ritorDlgs != g_itMGR.m_Dlgs.rend(); ++ritorDlgs )
	{
		pDlg = *ritorDlgs;
		if( pDlg->Process( uiMsg, wParam,lParam ) )
		{
			if( uiMsg == WM_LBUTTONDOWN )
			{
				g_itMGR.MoveDlg2ListEnd( pDlg );///iterator�� �ı��ɼ� �ִ� �׻� loop�� �����								
			}

			uiRet = uiMsg;
			iProcessDialogType = pDlg->GetDialogType();
			break;
		}

		///��� ���̾�α��� ���� ���� ���̾�α׸� ó�����ʿ䰡 ����.
		if( pDlg->IsVision() && pDlg->IsModal() )
		{
			DWORD dwDialgType = pDlg->GetDialogType();		
			return 1;
		} 
	}

	
	
	

	/// ������ȭ�� ���̾�α� ó��
	std::list< CIconDialog* >::reverse_iterator riterIcons;
	for( riterIcons = g_itMGR.m_Icons.rbegin(); riterIcons != g_itMGR.m_Icons.rend(); ++riterIcons )
	{
		if( (*riterIcons)->Process( uiMsg, wParam, lParam ) )
		{
			if( uiMsg == WM_LBUTTONDOWN )
				g_itMGR.MoveDlgIcon2ListEnd( *riterIcons );

			uiRet = uiMsg;
			break;
		}
	}

	// ����Ű ó�� 
	switch( it_GetKeyboardInputType() )
	{
	case CTControlMgr::INPUTTYPE_AUTOENTER:
		{
			if( !ProcessHotKeyAutoEnter( uiMsg, wParam, lParam ) )
				uiRet = 0;
		}
		break;
	case CTControlMgr::INPUTTYPE_NORMAL:
		{
			if( !ProcessHotKeyNormalInput( uiMsg, wParam, lParam ) )
				uiRet = 0;


		}
		break;
	default:
		break;
	}

	switch(uiMsg)
	{
	case WM_KEYDOWN:
		// quick ����ó�� 
		uiRet = uiMsg;
		switch( wParam)
		{
		case 91:///Window Key
			//g_itMGR.OpenDialog( DLG_TYPE_MENU );
			/// ������ ���۸޴��� ���۵Ǹ鼭 ȭ����ȯ�� �ǹ�����. ȭ���� �� ������쿡�� 
			/// ��ŷ�ؼ� ���ƹ�����? �ϴ� ����
			break;
		
		case VK_ESCAPE:
			{
#ifdef __NEWUPGRADESYSTEM
				//07. 1. 15 - ������ : ���� or ���� or ����â�� ���µǾ������� escŰ�� �⺻â�� ������°��� Ȱ��ȭ��Ű�� �ʴ´�.
				if(!g_itMGR.IsDlgOpened(DLG_TYPE_SEPARATE) && !g_itMGR.IsDlgOpened(DLG_TYPE_UPGRADE) && !g_itMGR.IsDlgOpened(DLG_TYPE_MAKE))
#endif
					g_itMGR.ProcessEscKeyDown();
				break;
			}
		default:
			uiRet = 0;
			break;
		}
		break;
	case WM_LBUTTONUP:
		// �巡�׾ص�� ó�� 		
//		g_DragNDrop.DropToWindow();
		CDragNDropMgr::GetInstance().DragEnd( iProcessDialogType );
		g_itMGR.DelDialogiconFromMenu();
		break;
	}
	
	///��ȯ���ϰ�� �̵��� ���´�.
	if( (pDlg = g_itMGR.FindDlg( DLG_TYPE_EXCHANGE )) && pDlg->IsVision() )
		return uiMsg;

	return uiRet;
}

//*-----------------------------------------------------------------------------------------//
/// @brief ä���� �ѹ� �Ҷ����� Enter �� �Է� �ؾ� �ϴ� ���
//*-----------------------------------------------------------------------------------------//
bool CITStateNormal::ProcessHotKeyNormalInput( unsigned uiMsg, WPARAM wParam, LPARAM lParam )
{
	if( CTEditBox::s_pFocusEdit == NULL )///�Է� ��Ŀ���� ���°��
	{
		switch(uiMsg)
		{
		case WM_KEYDOWN:
			{
				unsigned int oemScan = int( lParam & (0xff << 16) ) >> 16; 
				UINT vk = MapVirtualKey( oemScan, 1 );

				switch( vk )
				{
				case 0x41:
					g_itMGR.OpenDialog( DLG_TYPE_CHAR);
					return true;
				case 0x43:
					g_itMGR.OpenDialog( DLG_TYPE_COMMUNITY );
					return true;
				case 0x48:
					return true;

				case 0x49:
				case 0x56:
					g_itMGR.OpenDialog( DLG_TYPE_ITEM );
					return true;
				case 0x53:
					g_itMGR.OpenDialog( DLG_TYPE_SKILL );
					return true;
				case 0x51:
					g_itMGR.OpenDialog( DLG_TYPE_QUEST );
					return true;
				case 0x4d:
					{
						CMinimapDLG* pDlg = ( CMinimapDLG*)g_itMGR.FindDlg( DLG_TYPE_MINIMAP );
						pDlg->ToggleShowMinimap();
						return true;
					}
				case 0x4c:
					{
						CMinimapDLG* pDlg = ( CMinimapDLG*)g_itMGR.FindDlg( DLG_TYPE_MINIMAP );
						pDlg->ToggleZoomMinimap();
						return true;
					}
				case 0x4e:
					{
						g_itMGR.OpenDialog( DLG_TYPE_CLAN );
						return true;
					}
			/*	case 0x58:
					g_itMGR.OpenDialog( DLG_TYPE_SYSTEM );
					return true;*/
				case 0x4f:
					g_itMGR.OpenDialog( DLG_TYPE_OPTION );
					return true;
				default:
					break;
				}
			}
			return false;
		default:
			break;
			
		}
	}
	return ProcessHotKeyAutoEnter( uiMsg, wParam, lParam );
}
//*-----------------------------------------------------------------------------------------//
/// @brief �׻� �Է�â�� ��Ŀ���� ��ġ�Ͽ� ���͸� ĥ �ʿ䰡 ������ HotKeyó��
/// @warning NormalInput�ϰ�쿡�� AutoEnter()�� ó���ϰ� �ִ�.
//*-----------------------------------------------------------------------------------------//
bool CITStateNormal::ProcessHotKeyAutoEnter( unsigned uiMsg, WPARAM wParam, LPARAM lParam )
{
	switch(uiMsg)
	{
	case WM_SYSKEYDOWN:
		{
			unsigned int oemScan = int( lParam & (0xff << 16) ) >> 16; 
			UINT vk = MapVirtualKey( oemScan, 1 );

			switch(vk)
			{
			case 0x41: // 'a'
				//ĳ����â�� ���� 
				g_itMGR.OpenDialog( DLG_TYPE_CHAR);
				return true;
			case 0x43:// 'c'
				g_itMGR.OpenDialog( DLG_TYPE_COMMUNITY );
				return true;
			case 0x48://h
				return true;
			case 0x49: // 'i'
			case 0x56: // 'v' /// 2004 / 1 / 26 / Navy /�߰�( SYSTEM + I�� �Ѽ����� ������ ����ٴ� �ǰ����� )				
#ifdef __TEST // �κ��丮 ����Ű �߰�
			case 0x45: // 'e' /// ���� 9:52 2006-10-17 ȫ��.
#endif
				//�κ��丮�� ���� 
				g_itMGR.OpenDialog( DLG_TYPE_ITEM );
				return true;
			///��ųâ
			case 0x53: // 's'
				g_itMGR.OpenDialog( DLG_TYPE_SKILL );
				return true;
			/// ����Ʈâ
			case 0x51: // 'q'
				g_itMGR.OpenDialog( DLG_TYPE_QUEST );
				return true;
			///�̴ϸ� ���̱� / �����
			case 0x4d: // 'm'	
				{
					CMinimapDLG* pDlg = ( CMinimapDLG*)g_itMGR.FindDlg( DLG_TYPE_MINIMAP );
					pDlg->ToggleShowMinimap();
					return true;
				}
			///�̴ϸ� Ȯ�� / ���
			case 0x4c: // 'l'
				{
					CMinimapDLG* pDlg = ( CMinimapDLG*)g_itMGR.FindDlg( DLG_TYPE_MINIMAP );
					pDlg->ToggleZoomMinimap();
					return true;
				}
			case 0x4e: //n
				{
					g_itMGR.OpenDialog( DLG_TYPE_CLAN );
					return true;
				}
			case 0x58: //'x'
				g_itMGR.OpenDialog( DLG_TYPE_SYSTEM );
				return true;
			case 0x4f: //'o'
				g_itMGR.OpenDialog( DLG_TYPE_OPTION );
				return true;
			default:
				break;
			}
		}
		return false;
		break;
	default:
		break;
	}
	return true;
}
