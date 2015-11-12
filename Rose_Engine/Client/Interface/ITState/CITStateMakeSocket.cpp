#include "stdafx.h"
#include ".\citstatemakesocket.h"
#include "../it_mgr.h"
#include "../../System/CGame.h"
#include "../dlgs/CItemDlg.h"




CITStateMakeSocket::CITStateMakeSocket(void)
{
		m_iID = IT_MGR::STATE_MAKESOCKET;
}

CITStateMakeSocket::~CITStateMakeSocket(void)
{

}

void CITStateMakeSocket::Enter()
{
	if( CTDialog* pDlg = g_itMGR.FindDlg( DLG_TYPE_ITEM ) )
	{
		CItemDlg* pItemDlg = ( CItemDlg*)pDlg;
		pItemDlg->AddActionEventListener2Slots();				/// ������â ���Կ����� ������ Ŭ���� ����(���� ����)�� �ٲپ� �ش�.
	}
}
void CITStateMakeSocket::Leave()
{
		if( CTDialog* pDlg = g_itMGR.FindDlg( DLG_TYPE_ITEM ) )
	{
		CItemDlg* pItemDlg = ( CItemDlg*)pDlg;
		pItemDlg->RemoveActionEventListener2Slots();			/// Enter()���� �߰��� ����(���� ����)�� �����.
	}
}

unsigned CITStateMakeSocket::Process( unsigned uiMsg, WPARAM wParam, LPARAM lParam )
{
	UINT uiRet = 0;

	list_dlgs_ritor ritorDlgs;
	CTDialog*		pDlg = NULL;
	int				iProcessDialogType = 0;

	for( ritorDlgs = g_itMGR.m_Dlgs.rbegin(); ritorDlgs != g_itMGR.m_Dlgs.rend(); ++ritorDlgs )
	{
		pDlg = *ritorDlgs;
		if( pDlg->Process( uiMsg, wParam,lParam ) )
		{
			if( uiMsg == WM_LBUTTONDOWN )
				g_itMGR.MoveDlg2ListEnd( pDlg );///iterator�� �ı��ɼ� �ִ� �׻� loop�� �����

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

	switch( uiMsg )
	{
	case WM_LBUTTONDOWN:
		{
			if( iProcessDialogType != DLG_TYPE_ITEM /*&& iProcessDialogType != DLG_TYPE_DIALOG*/ )
				CGame::GetInstance().EndMakeSocket();
			break;
		}
	case WM_RBUTTONDOWN:
		CGame::GetInstance().EndMakeSocket();
		break;
	case WM_LBUTTONUP:
		CWinCtrl::SetMouseExclusiveCtrl( NULL );
	default:
		break;
	}

	return uiRet;
}