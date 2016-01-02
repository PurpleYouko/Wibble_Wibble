#include "stdafx.h"
#include ".\cseparatedlg.h"
#include "subclass/CSeparateDlgStateNormal.h"
#include "subclass/CSeparateDlgStateWait.h"
#include "subclass/CSeparateDlgStateResult.h"
#include "../CDragItem.h"
#include "../icon/ciconitem.h"
#include "../it_mgr.h"
#include "../command/uicommand.h"
#include "../../common/cinventory.h"
#include "../../gamedata/event/cteventseparate.h"
#include "../../gamedata/cseparate.h"
#include "../../gamecommon/item.h"
#include "../../object.h"
#include "../TypeResource.h"
#include "TCaption.h"
#include "TImage.h"

#include "../CToolTipMgr.h"


CSeparateDlg * CSeparateDlg::s_Instance = NULL;

void  CSeparateDlg::OutputItemSlotClear()
{
	
	m_OutputItemSlots.clear();
	this->s_Instance = this;

}

void CSeparateDlg::SetInterfacePos_After()
{
	POINT pt;
	int iGapY = 0;
	CSlot Slot;
	CWinCtrl * pCtrl = NULL;
	CWinCtrl * pCtrl2 = NULL;

	if( pCtrl = Find("ITEM_SLOT_00") )
	{
		pt = pCtrl->GetOffset();		
		m_MaterialItemSlot.SetOffset( pt );
		m_MaterialItemSlot.SetParent( GetDialogType() );
		m_MaterialItemSlot.SetDragAvailable();
		m_MaterialItemSlot.SetDragItem( m_pDragItem );
	}

	
	
	if( pCtrl = Find("ITEM_SLOT_01") )
	{
		pt = pCtrl->GetOffset();

		if( pCtrl2 = Find("ITEM_SLOT_02") )
		{
			iGapY = pCtrl2->GetOffset().y - pCtrl->GetOffset().y;
		}
	}
	
	for( int i = 0; i < 4; ++i )
	{
		Slot.SetOffset( pt );		
		Slot.SetParent( GetDialogType() );

		m_OutputItemSlots.push_back( Slot );
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
		((CTImage*)pCtrl)->SetText(LIST_STRING(789));//생성 재료명
	}
	if( pCtrl = Find("NUM") )
	{
		((CTImage*)pCtrl)->SetAlign(DT_CENTER | DT_VCENTER);
		((CTImage*)pCtrl)->SetText(LIST_STRING(790));//개수
	}
	
	if(pCtrl=Find(IID_BTN_START))
	{
		//분리시작 893
		pCtrl->SetText( LIST_STRING(893) );
	}

	m_pCaption->SetString(LIST_STRING(793));//분리

	MoveWindow(GetPosition());
}

void CSeparateDlg::Show()
{
	
#ifdef _DEBUG
	POINT pt = { GetPosition().x, GetPosition().y };
	Clear();
	Create("DlgSeparate");		
	SetInterfacePos_After();
	MoveWindow(pt);
#endif

	CTDialog::Show();
}



CSeparateDlg::CSeparateDlg(void)
{
	this->s_Instance = this;
	int iType	=	DLG_TYPE_SEPARATE;
	
	///제조할 아이템이 들어갈 Slot
	m_pDragItem = new CDragItem;
	CTCommand* pCmd = new CTCmdTakeOutItemFromSeparateDlg;

	m_pDragItem->AddTarget( iType, NULL );
	m_pDragItem->AddTarget( CDragItem::TARGET_ALL, pCmd );


	

	SetDialogType( iType );

	m_pStates[STATE_NORMAL] = new CSeparateDlgStateNormal(this);
	m_pStates[STATE_WAIT]	= new CSeparateDlgStateWait;
	m_pStates[STATE_RESULT]	= new CSeparateDlgStateResult(this);

	m_pCurrState = m_pStates[STATE_NORMAL];
	iViewItem = 0;
	m_ResultItemSet.clear();
}

CSeparateDlg::~CSeparateDlg(void)
{
	SAFE_DELETE( m_pDragItem );
	SAFE_DELETE( m_pStates[STATE_NORMAL] );
	SAFE_DELETE( m_pStates[STATE_WAIT] );
	SAFE_DELETE( m_pStates[STATE_RESULT] );

	m_pCurrState = NULL;

}

void CSeparateDlg::ChangeState( int iID )
{
	assert( iID >= 0 && iID < STATE_MAX );
	if(  iID >= 0 && iID < STATE_MAX   )
	{
		m_pCurrState->Leave();
		m_pCurrState = m_pStates[iID];
		m_pCurrState->Enter();
	}
}

void CSeparateDlg::Update( POINT ptMouse )
{
	if( !IsVision() ) return;
	CTDialog::Update( ptMouse );

	RECT  rcDrawName = { 27, 189, 27  + 88, 189 + 18 };
	if( CSeparate::GetInstance().GetType() == CSeparate::TYPE_NPC )
	{
		WORD wNpcSvrIdx = CSeparate::GetInstance().GetNpcSvrIdx();
		CObjCHAR* pNpc = g_pObjMGR->Get_ClientCharOBJ( wNpcSvrIdx, false );
		if( !( pNpc && g_pAVATAR->IsInRANGE( pNpc, AVT_CLICK_EVENT_RANGE )) )
		{
			Hide();
			CSeparate::GetInstance().RemoveItem();
		}
	}

 	m_pCurrState->Update( ptMouse );

	m_MaterialItemSlot.Update( ptMouse );

	CSeparateDlgStateResult* pSeparateStateResult = CSeparateDlgStateResult::GetInstance();

	m_ResultItemSet = CSeparate::GetInstance().GetResultItemSet();
	if(!pSeparateStateResult->GetStartSeparateState())
	{

	}

	else
	{
		if( pSeparateStateResult == NULL) return;

		if(pSeparateStateResult->GetViewNum() > 4 || pSeparateStateResult->GetViewNum() == NULL)
		{
			iViewItem = 0;
		}
		else
		{
			iViewItem = pSeparateStateResult->GetViewNum();
		}
		
		for( int i = 0, j =  0 ; i < CSeparate::GetInstance().GetResultCnt() - 1 && j <= iViewItem; i++, j++)
		{
			if(!pSeparateStateResult->GetCheckUpdate())
				m_OutputItemSlots[i].Update( ptMouse );
		}
	}

 
    //긴 아이템 이름 툴팁.
	vector<CSinglelineString>::iterator itor = m_vecSingleString.begin();
	while( itor != m_vecSingleString.end() )
	{
		CSinglelineString & sStrBuf = (*itor);

		if( sStrBuf.is_ellipsis() )
		{
			POINT ptPoint = { ptMouse.x - m_sPosition.x , ptMouse.y - m_sPosition.y };
			const RECT& rect_name = rcDrawName;
			if( PtInRect( &rect_name, ptPoint ) )
			{
				sStrBuf.get_rect().left;
				sStrBuf.get_rect().top;
				CToolTipMgr::GetInstance().RegToolTip( rcDrawName.left + m_sPosition.x - 20, rcDrawName.top + m_sPosition.y - 20, sStrBuf.get_string()  );
				return;
			}
		}
		itor++;
	}

}

void CSeparateDlg::Update( CObservable* pObservable, CTObject* pObj )
{
	assert( pObservable );
	assert( pObj && strcmp( pObj->toString() ,"Separate" ) == 0 );
	if( pObj == NULL || strcmp( pObj->toString() ,"Separate" ) ) return;

	CTEventSeparate* pEvent = (CTEventSeparate*)pObj;
	switch( pEvent->GetID() )
	{
	case CTEventSeparate::EID_REMOVE_MATERIAL_ITEM:
		m_MaterialItemSlot.DetachIcon();
		break;
	case CTEventSeparate::EID_SET_MATERIAL_ITEM:
		{
			CItem* pItem = pEvent->GetItem();
			assert( pItem );
			if( pItem  )
				m_MaterialItemSlot.AttachIcon( pItem->CreateItemIcon() );
		
			break;
		}
	case CTEventSeparate::EID_REMOVE_OUTPUT_ITEM:
		{
			int iIndex = pEvent->GetIndex();
			assert( iIndex >= 0 && iIndex < (int)m_OutputItemSlots.size() );
			if( iIndex  >= 0 && iIndex < (int)m_OutputItemSlots.size() )
				m_OutputItemSlots[iIndex].DetachIcon();	
			break;
		}
	case CTEventSeparate::EID_SET_OUTPUT_ITEM:
		{
			int e = m_OutputItemSlots.size();
			int iIndex = pEvent->GetIndex();
			assert( iIndex >= 0 && iIndex < (int)m_OutputItemSlots.size() );
			if( iIndex  >= 0 && iIndex < (int)m_OutputItemSlots.size() )
			{
				CItem* pItem = pEvent->GetItem();

				assert( pItem && m_OutputItemSlots[iIndex].GetIcon() == NULL );


				if( pItem && m_OutputItemSlots[iIndex].GetIcon() == NULL )
				{
					m_OutputItemSlots[iIndex].AttachIcon( pItem->CreateItemIcon() );
				}
				else if( pItem && m_OutputItemSlots[iIndex].GetIcon() != NULL)
				{
					m_OutputItemSlots[iIndex].DetachIcon();	
					m_OutputItemSlots[iIndex].AttachIcon( pItem->CreateItemIcon() );
				}
				else
				{
					assert(" pItem && m_OutputItemSlots[iIndex].GetIcon() == NULL ?? ");
				}
			}
			break;
		}
	case CTEventSeparate::EID_RECEIVE_RESULT:
		{
			ChangeState( STATE_RESULT );
			break;
		}
	default:
		assert( 0 && "Invalid Separate Event Type" );
		break;
	}

}

unsigned CSeparateDlg::Process( unsigned uiMsg, WPARAM wParam, LPARAM lParam )
{
	if( !IsVision() ) return 0;
		return m_pCurrState->Process( uiMsg, wParam, lParam );
}

bool CSeparateDlg::CheckInvEmptySlot()
{

	CSeparate& Separate = CSeparate::GetInstance();
	CIcon* pIcon = m_MaterialItemSlot.GetIcon();

	if( pIcon == NULL )///재료가 없다면 Disable
	{
		g_itMGR.OpenMsgBox(STR_SEPARATE_EMPTY_TARGETITEM);
		return false;
	}

	int iCount = 0;

	int totalcount = 0;
	std::vector< CSlot >::iterator iterOutput;
	for( iterOutput = m_OutputItemSlots.begin(); iterOutput != m_OutputItemSlots.end(); ++iterOutput )
	{
		++totalcount;
		if( iterOutput->GetIcon() )
		++iCount;
	}
	if( iCount <= 0 )
	{
		g_itMGR.OpenMsgBox( STR_CANT_SEPARATE );
		return false;
	}

	CIconItem* pItem = (CIconItem*)pIcon;


	if( !HasEnoughInventoryEmptySlot( pItem,m_OutputItemSlots ) )
	{
		g_itMGR.OpenMsgBox(STR_NOTENOUGH_EMPTY_INVENTORYSLOT);
		return false;
	}

	return true;
}

//*-------------------------------------------------------------------/
/// 분리/분해후 아이템이 들어갈 자리가 충분히 있는가?
//*-------------------------------------------------------------------/
bool CSeparateDlg::HasEnoughInventoryEmptySlot( CIconItem* pItemIcon, std::vector<CSlot>& OutputItems )
{
#ifdef _NEWBREAK
	//06. 08. 24 - 김주현 : 지금 분해는 재료만 나오게 되어있다. 재료탭만 체크한다.
	short nEmptySlotCount[ MAX_INV_TYPE ];

	CInventory* pInven = g_pAVATAR->GetInventory();
	assert( pInven );

	nEmptySlotCount[INV_ETC] = pInven->GetEmptyInvenSlotCount( (t_InvTYPE)INV_ETC );

	if(nEmptySlotCount[INV_ETC] < 4)
		return false;

	return true;
#else
	assert( pItemIcon );
	if( pItemIcon == NULL ) return false;

	///일단 모든 타입의 빈 슬롯을 구한다.
	short nEmptySlotCount[ MAX_INV_TYPE ];

	CInventory* pInven = g_pAVATAR->GetInventory();
	assert( pInven );

	for( int i = INV_WEAPON; i < MAX_INV_TYPE; ++i )
		nEmptySlotCount[i] = pInven->GetEmptyInvenSlotCount( (t_InvTYPE)i );

	/// 분리/분해할 아이템의 자리를 빈슬롯으로 계산한다( 개수가 없거나 개수가 있지만 1개일경우)
	tagITEM& Item = pItemIcon->GetItem();
	if( (Item.IsEnableDupCNT() && Item.GetQuantity() == 1 ) || !Item.IsEnableDupCNT() )
		nEmptySlotCount[ CInventory::GetInvPageTYPE( Item ) ]++;


	CIcon*		pIcon = NULL;
	CIconItem*	pOutputItemIcon = NULL;
	std::vector<CSlot>::iterator iter;

	for( iter = OutputItems.begin(); iter != OutputItems.end(); ++iter )
	{
		if( pIcon = iter->GetIcon() )
		{
			pOutputItemIcon = ( CIconItem* )pIcon;
			nEmptySlotCount[ CInventory::GetInvPageTYPE( pOutputItemIcon->GetItem() ) ]--;
			if( nEmptySlotCount[ CInventory::GetInvPageTYPE( pOutputItemIcon->GetItem() ) ] < 0 )
				return false;
		}
	}
	return true;
#endif

}

void CSeparateDlg::Draw()
{
	if( !IsVision() ) return;

	CTDialog::Draw();

	m_pCurrState->Draw();


	m_MaterialItemSlot.Draw();

	CIcon* pIcon = NULL;

	CWinCtrl * pCtrl = NULL;
	CWinCtrl * pCtrl2 = NULL;

	RECT rcDraw, rcDrawCount;
	int		iGapY = 0;

	///필요 MP or 줄리
	D3DXMATRIX mat;	
	D3DXMatrixTranslation( &mat, (float)m_sPosition.x, (float)m_sPosition.y,0.0f);
	::setTransformSprite( mat );

	if( pIcon = m_MaterialItemSlot.GetIcon() )
	{
		if( pCtrl = Find("DEST_ITEM_NAME") )
		{
			((CTImage*)pCtrl)->SetAlign(DT_CENTER | DT_VCENTER);
			((CTImage*)pCtrl)->SetText(pIcon->GetName());//대상 아이템
		}
	}
//05. 12. 23 - 김주현 : 분해가.. 기본스킬에 등록됨에 따라 기존에 뺏어오던 돈이랑 mp는 삭제되었다
//하지만 UI 작업이 아직 안된 관계로 찍는 부분만 삭제하고 나중에 UI 수정 작업을 해줘야 한다.
/*
	if(pCtrl = Find("REQ_MP_NUM"))
	{
		SetRect( &rcDraw,
			pCtrl->GetOffset().x, 
			pCtrl->GetOffset().y, 
			pCtrl->GetOffset().x+pCtrl->GetWidth(), 
			pCtrl->GetOffset().y+pCtrl->GetHeight() );

		CSeparate& Separate = CSeparate::GetInstance();
		switch( Separate.GetType() )
		{
		case CSeparate::TYPE_SKILL:
			drawFontf( g_GameDATA.m_hFONT[ FONT_NORMAL ], true, &rcDraw ,g_dwWHITE,DT_RIGHT,"%d", Separate.GetRequireMp() );
			break;
		case CSeparate::TYPE_NPC:
			drawFontf( g_GameDATA.m_hFONT[ FONT_NORMAL ], true, &rcDraw ,g_dwWHITE,DT_RIGHT,"%d", Separate.GetRequireMoney() );
			break;
		default:
			break;

		}		
	}
*/
	if(pCtrl = Find( "MATERIAL_00" ))
	{
		SetRect( &rcDraw,
			pCtrl->GetOffset().x,
			pCtrl->GetOffset().y + 2,
			pCtrl->GetOffset().x + pCtrl->GetWidth(),
			pCtrl->GetOffset().y + pCtrl->GetHeight() + 2);
		if( pCtrl2 = Find( "MATERIAL_01" ) )
		{
			iGapY = pCtrl2->GetPosition().y - pCtrl->GetPosition().y;
		}

	}
	if(pCtrl = Find( "MATERIAL_NUM_00" ))
	{
		SetRect( &rcDrawCount,
			pCtrl->GetOffset().x,
			pCtrl->GetOffset().y,
			pCtrl->GetOffset().x + pCtrl->GetWidth(),
			pCtrl->GetOffset().y + pCtrl->GetHeight() );
	}


	int j= 0;

	m_ResultItemSet = CSeparate::GetInstance().GetResultItemSet();

	std::vector< CSlot >::iterator iter;

	CSeparateDlgStateResult* pSeparateStateResult = CSeparateDlgStateResult::GetInstance();

	if(!pSeparateStateResult->GetStartSeparateState())
	{
		m_vecSingleString.clear();
	}
	else
	{

		if( pSeparateStateResult == NULL) return;

		if(pSeparateStateResult->GetViewNum() > 4)
		{
			iViewItem = -1;
		}
		else
		{
			iViewItem = pSeparateStateResult->GetViewNum();
		}




		for( int i = 0; i < CSeparate::GetInstance().GetResultCnt() - 1 && i <= iViewItem; i++)
		{
			m_OutputItemSlots[i].Draw();

			D3DXMatrixTranslation( &mat, (float)m_sPosition.x, (float)m_sPosition.y,0.0f);
			::setTransformSprite( mat );

			if( pIcon = m_OutputItemSlots[i].GetIcon() )
			{
				CSinglelineString sStrBuf;
				sStrBuf.set_color(g_dwBLACK);
				sStrBuf.set_format(DT_CENTER | DT_VCENTER);
				sStrBuf.set_string( pIcon->GetName() , rcDraw );
				sStrBuf.draw();

				if( sStrBuf.is_ellipsis() )
				{
					m_vecSingleString.push_back( sStrBuf );
				}			

				char Temp[125] = "";
				sprintf( Temp, "%d",CSeparate::GetInstance().GetOutputItemQuantity(i) );

				drawFont( g_GameDATA.m_hFONT[ FONT_NORMAL ], true, &rcDrawCount, g_dwWHITE, DT_CENTER , Temp );
			}
			rcDraw.top		+= iGapY;
			rcDraw.bottom	+= iGapY;
			rcDrawCount.top += iGapY;
			rcDrawCount.bottom += iGapY;

		}

	}
}




CSeparateDlg* CSeparateDlg::GetInstance()
{
	return s_Instance;
}

CSlot CSeparateDlg::GetMaterialItemSlot()
{	
	return	m_MaterialItemSlot;
}

void CSeparateDlg::MoveWindow( POINT pt )
{
	CTDialog::MoveWindow( pt );

	m_MaterialItemSlot.MoveWindow( m_sPosition );
	std::vector< CSlot >::iterator iter;
	for( iter = m_OutputItemSlots.begin(); iter != m_OutputItemSlots.end(); ++iter )
		iter->MoveWindow( m_sPosition );

	for( int i = 0 ; i < STATE_MAX; ++i )
		m_pStates[i]->MoveWindow( m_sPosition );;
		
}


void CSeparateDlg::Hide()
{
	CTDialog::Hide();
	CSeparate::GetInstance().RemoveItem();
}
