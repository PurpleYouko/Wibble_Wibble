#include "stdafx.h"
#include ".\cupgradedlg.h"
#include "subclass/CUpgradeDlgStateNormal.h"
#include "subclass/CUpgradeDlgStateWait.h"
#include "subclass/CUpgradeDlgStateResult.h"
#include "../../GameData/Event/CTEventUpgrade.h"
#include "../../GameData/CUpgrade.h"
#include "../../GameCommon/Item.h"
#include "../CDragItem.h"
#include "../command/dragcommand.h"
#include "../../object.h"
#include "TImage.h"
#include <algorithm>
#include "../CToolTipMgr.h"
#include "TCaption.h"


CUpgradeDlg::CUpgradeDlg( int iType )
{
	m_pStates[STATE_NORMAL] = new CUpgradeDlgStateNormal(this);
	m_pStates[STATE_WAIT]	= new CUpgradeDlgStateWait;
	m_pStates[STATE_RESULT]	= new CUpgradeDlgStateResult(this);

	m_pCurrState = m_pStates[STATE_NORMAL];

	
	SetDialogType( iType );

}

CUpgradeDlg::~CUpgradeDlg(void)
{
	SAFE_DELETE( m_pDragItemTarget );
	SAFE_DELETE( m_pDragItemMaterial );


	SAFE_DELETE( m_pStates[STATE_NORMAL] );
	SAFE_DELETE( m_pStates[STATE_WAIT] );
	SAFE_DELETE( m_pStates[STATE_RESULT] );

	m_pCurrState = NULL;
}


void CUpgradeDlg::SetInterfacePos_After()
{
	CWinCtrl * pCtrl = NULL;
	CWinCtrl * pCtrl2 = NULL;
	POINT pt;
	int iGapY = 0;

	if(pCtrl = Find("ITEM_SLOT_00"))
	{
		pt.x = pCtrl->GetOffset().x;
		pt.y = pCtrl->GetOffset().y;
	}	

	m_pDragItemTarget = new CDragItem;
	CTCommand* pCmd = new CTCmdDragTakeoutTargetItemFromUpgradeDlg;
	m_pDragItemTarget->AddTarget( CDragItem::TARGET_ALL, pCmd );
	m_pDragItemTarget->AddTarget( GetDialogType(), NULL );

	m_TargetItemSlot.SetOffset( pt );
	m_TargetItemSlot.SetParent( GetDialogType() );
	m_TargetItemSlot.SetDragAvailable();
	m_TargetItemSlot.SetDragItem( m_pDragItemTarget );


	if(pCtrl = Find("ITEM_SLOT_01"))
	{
		pt.x = pCtrl->GetOffset().x;
		pt.y = pCtrl->GetOffset().y;
		if(pCtrl2 = Find("ITEM_SLOT_02"))
		{
			iGapY = pCtrl2->GetOffset().y - pCtrl->GetOffset().y;
		}
	}	
	
	m_pDragItemMaterial = new CDragItem;;

	pCmd = new CTCmdDragTakeoutMaterialItemFromUpgradeDlg;
	m_pDragItemMaterial->AddTarget( CDragItem::TARGET_ALL, pCmd );
	m_pDragItemMaterial->AddTarget( GetDialogType(), NULL );


	for(int i = 0 ; i < 3; ++i )
	{		
		m_MaterialSlots[i].SetOffset( pt );
		m_MaterialSlots[i].SetParent( GetDialogType() );
		m_MaterialSlots[i].SetDragAvailable();
		m_MaterialSlots[i].SetDragItem( m_pDragItemMaterial );

		pt.y += iGapY;
	}

	if( pCtrl = Find("DEST_ITEM") )
	{
		((CTImage*)pCtrl)->SetAlign(DT_CENTER | DT_VCENTER);
		((CTImage*)pCtrl)->SetText(LIST_STRING(788));//대상 아이템
	}
	if( pCtrl = Find("MATERIAL_NAME") )
	{
		((CTImage*)pCtrl)->SetAlign(DT_CENTER | DT_VCENTER);
		((CTImage*)pCtrl)->SetText(LIST_STRING(798));//재료명
	}
	if( pCtrl = Find("NUM") )
	{
		((CTImage*)pCtrl)->SetAlign(DT_CENTER | DT_VCENTER);
		((CTImage*)pCtrl)->SetText(LIST_STRING(790));//개수
	}	

	if( pCtrl = Find(10) )
	{
		//제련시작 895
		pCtrl->SetText( LIST_STRING(895) );		
	}	

	GetCaption()->SetString(LIST_STRING(794));//제련

	MoveWindow(GetPosition());
}

void CUpgradeDlg::ChangeState( int iID )
{
	assert( iID >= 0 && iID < STATE_MAX );
	if(  iID >= 0 && iID < STATE_MAX   )
	{
		m_pCurrState->Leave();
		m_pCurrState = m_pStates[iID];
		m_pCurrState->Enter();
	}
}

void CUpgradeDlg::Update( POINT ptMouse )
{
	if( !IsVision() ) return;
	CTDialog::Update( ptMouse );


    //긴 아이템 이름 툴팁.
	vector<CSinglelineString>::iterator itor = m_vecSingleString.begin();
	while( itor != m_vecSingleString.end() )
	{
		CSinglelineString & sStrBuf = (*itor);

		if( sStrBuf.is_ellipsis() )
		{
			POINT ptPoint = { ptMouse.x - m_sPosition.x , ptMouse.y - m_sPosition.y };
			const RECT& rect_name = sStrBuf.get_rect();
			if( PtInRect( &rect_name, ptPoint ) )
			{
				sStrBuf.get_rect().left;
				sStrBuf.get_rect().top;
				CToolTipMgr::GetInstance().RegToolTip( sStrBuf.get_rect().left + m_sPosition.x - 20, sStrBuf.get_rect().top + m_sPosition.y - 20, sStrBuf.get_string() );
				return;
			}
		}
		itor++;
	}


	if( CUpgrade::GetInstance().GetType() == CUpgrade::TYPE_NPC )
	{
		WORD wNpcSvrIdx = CUpgrade::GetInstance().GetNpcSvrIdx();
		CObjCHAR* pNpc = g_pObjMGR->Get_ClientCharOBJ( wNpcSvrIdx, false );
		if( !( pNpc && g_pAVATAR->IsInRANGE( pNpc, AVT_CLICK_EVENT_RANGE )) )
		{
			CUpgrade::GetInstance().RemoveTargetItem();
			Hide();
			return;
		}
	}

	m_pCurrState->Update( ptMouse );

	m_TargetItemSlot.Update( ptMouse );
	for( int i = 0 ; i < 3; ++i )
		m_MaterialSlots[i].Update( ptMouse );

}

void CUpgradeDlg::Draw()
{
	if( !IsVision() ) return;
	CWinCtrl * pCtrl = NULL;
	CTDialog::Draw();
	m_pCurrState->Draw();

	m_TargetItemSlot.Draw();
	for( int i = 0 ; i < 3; ++i )
		m_MaterialSlots[i].Draw();


	D3DXMATRIX mat;	
	D3DXMatrixTranslation( &mat, (float)m_sPosition.x, (float)m_sPosition.y,0.0f);
	::setTransformSprite( mat );


	CUpgrade& Upgrade = CUpgrade::GetInstance();
	if( Upgrade.GetTargetItemName()  )
	{
		if( pCtrl = Find("DEST_ITEM_NAME") )
		{
			RECT rcTargetItemName = {
				pCtrl->GetOffset().x,
				pCtrl->GetOffset().y,
				pCtrl->GetOffset().x + pCtrl->GetWidth(),
				pCtrl->GetOffset().y + pCtrl->GetHeight() };
				
			drawFont( g_GameDATA.m_hFONT[ FONT_NORMAL ], true, &rcTargetItemName ,g_dwWHITE,DT_CENTER, Upgrade.GetTargetItemName() );
		}		
	}

	RECT  rcDraw = { 27, 189, 27  + 88, 189 + 18 };
	RECT  rcDrawCount = { 115, 189, 145, 205 };
	int iGapY = 0;

	CWinCtrl * pCtrl2 = NULL;

	if( pCtrl = Find("MATERIAL_00"))
	{
		SetRect( &rcDraw, 
			pCtrl->GetOffset().x,
			pCtrl->GetOffset().y,
			pCtrl->GetOffset().x + pCtrl->GetWidth(),
			pCtrl->GetOffset().y + pCtrl->GetHeight() );

		if( pCtrl2 = Find("MATERIAL_01"))
		{
			iGapY = pCtrl2->GetOffset().y - pCtrl2->GetOffset().y;
		}
	}

	if( pCtrl = Find("MATERIAL_NUM_00"))
	{
		SetRect( &rcDrawCount, 
			pCtrl->GetOffset().x,
			pCtrl->GetOffset().y,
			pCtrl->GetOffset().x + pCtrl->GetWidth(),
			pCtrl->GetOffset().y + pCtrl->GetHeight() );
	}

	//05.11.10 : 김주현 - 제련 UI 필요 아이템 이름 표시 방법 변경.
	//제련에 필요한 아이템 명의 길이가 너무 길 경우  ..을 붙이고
	//마우스 포인터를 올렸을경우 풀네임이 보이게 변경.

	CSinglelineString sStrBuf;
	m_vecSingleString.clear();

	for( int i = 0; i < 3; ++i )
	{
		if( Upgrade.GetRequireMaterialName( i ) )
		{
			sStrBuf.set_string( Upgrade.GetRequireMaterialName(i), rcDraw );
			sStrBuf.set_format( DT_CENTER | DT_VCENTER );
			sStrBuf.draw();		
			if( sStrBuf.is_ellipsis() )
			{
				m_vecSingleString.push_back( sStrBuf );
			}	

			drawFontf( g_GameDATA.m_hFONT[ FONT_NORMAL ], true, &rcDrawCount ,DT_CENTER | DT_VCENTER, "%d", Upgrade.GetRequireMaterialCount( i ) );
			
		}
		rcDraw.top		+= iGapY;
		rcDraw.bottom	+= iGapY;
		rcDrawCount.top += iGapY;
		rcDrawCount.bottom += iGapY;
	}

	if( pCtrl = Find("REQ_MP_NUM") )
	{		
		SetRect( &rcDraw, 
			pCtrl->GetOffset().x,
			pCtrl->GetOffset().y,
			pCtrl->GetOffset().x + pCtrl->GetWidth(),
			pCtrl->GetOffset().y + pCtrl->GetHeight() );
	}

	
	switch( Upgrade.GetType() )
	{
	case CUpgrade::TYPE_SKILL:
		if( pCtrl = Find("REQ_MONEY") )
		{
			//소모 MP 791
			((CTImage*)pCtrl)->SetAlign(DT_CENTER | DT_VCENTER);
			((CTImage*)pCtrl)->SetText( LIST_STRING(791) );
		}
		drawFontf( g_GameDATA.m_hFONT[ FONT_NORMAL ], true, &rcDraw ,g_dwWHITE,DT_RIGHT,"%d", Upgrade.GetRequireMp() );
		break;
	case CUpgrade::TYPE_NPC:
		if( pCtrl = Find("REQ_MONEY") )
		{
			//비용 574
			((CTImage*)pCtrl)->SetAlign(DT_CENTER | DT_VCENTER);
			((CTImage*)pCtrl)->SetText(LIST_STRING(574));
		}
		drawFontf( g_GameDATA.m_hFONT[ FONT_NORMAL ], true, &rcDraw ,g_dwWHITE,DT_RIGHT,"%d", Upgrade.GetRequireMoney() );
		break;
	default:
		break;
	}
	
}

unsigned CUpgradeDlg::Process( unsigned uiMsg, WPARAM wParam, LPARAM lParam )
{
	if( !IsVision() ) return 0;
	return m_pCurrState->Process( uiMsg, wParam, lParam );
}

void CUpgradeDlg::Update( CObservable* pObservable, CTObject* pObj )
{
	assert( pObservable );
	assert( pObj );
	if( pObj == NULL ) return;

	assert( strcmp(pObj->toString(),"Upgrade") == 0 );
	if( strcmp(pObj->toString(),"Upgrade") ) return;

	CTEventUpgrade* pEvent = (CTEventUpgrade*)pObj;
	switch( pEvent->GetID() )
	{
	case CTEventUpgrade::EID_REMOVE_MATERIALITEM:
		{
			int iIndex = pEvent->GetIndex();
			assert( iIndex >= 0 && iIndex < 3 );
			if( iIndex >= 0 && iIndex < 3 )
			{
				assert( m_MaterialSlots[iIndex].GetIcon() );
				m_MaterialSlots[iIndex].DetachIcon();
			}
			break;
		}
	case CTEventUpgrade::EID_REMOVE_TARGETITEM:
		{
			assert( m_TargetItemSlot.GetIcon() );
			m_TargetItemSlot.DetachIcon();
			break;
		}
	case CTEventUpgrade::EID_SET_MATERIALiTEM:
		{
			int iIndex = pEvent->GetIndex();
			assert( iIndex >= 0 && iIndex < 3 );
			if( iIndex >= 0 && iIndex < 3 )
			{
				assert( m_MaterialSlots[iIndex].GetIcon() == NULL );
				m_MaterialSlots[iIndex].AttachIcon( pEvent->GetItem()->CreateItemIcon() );
			}
			break;
		}
	case  CTEventUpgrade::EID_SET_TARGETITEM:
		{
			assert( m_TargetItemSlot.GetIcon() == NULL );
			if( m_TargetItemSlot.GetIcon() == NULL )
				m_TargetItemSlot.AttachIcon( pEvent->GetItem()->CreateItemIcon() );

			break;
		}
	case CTEventUpgrade::EID_RECEIVE_RESULT:
		{
			ChangeState( STATE_RESULT );
			break;
		}
	default:
		break;
	}
}
void CUpgradeDlg::MoveWindow( POINT pt )
{
	CTDialog::MoveWindow( pt );
	m_TargetItemSlot.MoveWindow( m_sPosition );
	int i = 0;
	for( i = 0 ; i < 3; ++i )
		m_MaterialSlots[i].MoveWindow( m_sPosition );

	for( i = 0 ; i < STATE_MAX; ++i )
		m_pStates[i]->MoveWindow( m_sPosition );;
}

void CUpgradeDlg::Show()
{


#ifdef _DEBUG

	POINT pt = { GetPosition().x, GetPosition().y };
	Clear();
	Create("DlgUpgrade");		
	SetInterfacePos_After();
	MoveWindow(pt);

#endif


	CTDialog::Show();
	switch( CUpgrade::GetInstance().GetType() )
	{
	case CUpgrade::TYPE_SKILL:
		HideChild( IID_TEXT_COST );
		break;
	default:
		break;
	}
}

void CUpgradeDlg::Hide()
{
	CTDialog::Hide();
	CUpgrade::GetInstance().RemoveTargetItem();
}
