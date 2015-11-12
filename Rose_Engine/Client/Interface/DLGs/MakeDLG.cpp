#include "StdAfx.h"
#include "MakeDLG.h"
#include "../../../TGameCtrl/ResourceMgr.h"
#include "../../../TGameCtrl/JComboBox.h"
#include "../../../TGameCtrl/JListBox.h"
#include "../../../TGameCtrl/TContainer.h"
#include "../../../TGameCtrl/TListCellRenderer.h"
#include "TImage.h"
#include "../../Common/IO_Skill.h"
#include "../../Common/CEconomy.h"
#include "../../Network/CNetwork.h"
#include "TCaption.h"
#include "../../Game.h"
#include "../../Object.h"

#include "../IO_ImageRes.h"
#include "../CTDrawImpl.h"
#include "../IT_MGR.h"
#include "../CDragItem.h"
#include "../command/uicommand.h"

#include "SubClass/CMakeStateNormal.h"
#include "SubClass/CMakeStateWait.h"
#include "SubClass/CMakeStateResult.h"
#include "../Icon/CIconItem.h"

#include "../../GameData/Event/CTEventManufacture.h"

#include <algorithm>
#include "../CToolTipMgr.h"


CMakeComboItem::CMakeComboItem( tagITEM& Item )
{
	if( !Item.IsEmpty() )
	{
		SetIdentify( ITEM_NAME( Item.GetTYPE(), Item.GetItemNO() ) );
		m_Item = Item;
	}
	else
	{
		assert( 0 && "Item is Empty @CMakeComboItem::Constructor" );
	}
}


tagITEM& CMakeComboItem::GetItem()
{
	return m_Item;
}

CMakeComboClass::CMakeComboClass( int iClass )
{
	m_iClass = iClass;
	if( CManufacture::GetInstance().GetMakeType() == CManufacture::GEM_TYPE )
	{
		int iLineNo = iClass;	
		const char* pszName = ITEM_NAME( ITEM_TYPE_GEM, iLineNo );
		if( pszName )
		{
			std::string strName = pszName;
			
			std::string::size_type sizeType = strName.find_first_of(' ', 0 );
			if( sizeType != string::npos && sizeType != strName.size() )
			{
				std::string strSubName = strName.substr( 0, sizeType );
				SetIdentify( strSubName.c_str() );
			}
			else
			{
				SetIdentify( strName.c_str() );
			}
		}
		else
		{
			SetIdentify( "NoName");
		}
	}
	else
		SetIdentify( CStringManager::GetSingleton().GetItemType(iClass) );
}

int CMakeComboClass::GetClass()
{
	return m_iClass;
}
//*----------------------------------------------------------------------------------*/
/// Wait State와 Result State는 Modal인것처럼 처리한다.(IsModal에서 항상 return true );
/// Normal State에서는 return CTDialog::IsModal()
//*-----------------------------------------------------------------------------------*/
CMakeDLG::CMakeDLG( int iType )
{
	m_pMakeState[STATE_NORMAL]	= NULL;
	m_pMakeState[STATE_WAIT]	= NULL;
	m_pMakeState[STATE_RESULT]	= NULL;
	m_pCurrState				= NULL;

	m_pDragItem = new CDragItem;
	CTCommand* pCmd = new CTCmdTakeOutItemFromMakeDlg;

	m_pDragItem->AddTarget( iType, NULL );
	m_pDragItem->AddTarget( CDragItem::TARGET_ALL, pCmd );
	

	SetDialogType( iType );
}

CMakeDLG::~CMakeDLG()
{
	for( int i = 0; i < STATE_MAX; ++i )
		delete m_pMakeState[i];

	m_MakeItemSlot.DetachIcon();
	for_each( m_listMaterialSlot.begin(), m_listMaterialSlot.end(), mem_fun_ref( &CSlot::DetachIcon) );
	m_pCurrState = NULL;

	SAFE_DELETE( m_pDragItem );
}

void CMakeDLG::ChangeState( int iState )
{
	if( iState >= 0 && iState < STATE_MAX )
	{
		m_pCurrState = m_pMakeState[ iState ];
		m_pCurrState->Init();
	}
}

void CMakeDLG::MoveWindow( POINT pt )
{
	CTDialog::MoveWindow( pt );

	m_MakeItemSlot.MoveWindow( pt );	
	for_each( m_listMaterialSlot.begin(), m_listMaterialSlot.end(), bind2nd( mem_fun_ref( &CSlot::MoveWindow), pt ));

	for( int i = 0; i < STATE_MAX; ++i )
		m_pMakeState[i]->MoveWindow( pt );
}


void CMakeDLG::Draw()
{
	if( !IsVision() ) return;
	CTDialog::Draw();
	
	CManufacture& MakeData = CManufacture::GetInstance();
	
	/// SubSystem Draw
	m_pCurrState->Draw();

	/// 제조할 아이템 Draw
	m_MakeItemSlot.Draw();

	/// 인벤토리에서 옮긴 재료아이템 Draw
	for_each( m_listMaterialSlot.begin(), m_listMaterialSlot.end(), mem_fun_ref(&CSlot::Draw) );

	/// 필요 재료 Draw
	D3DXMATRIX mat;	
	D3DXMatrixTranslation( &mat, (float)m_sPosition.x, (float)m_sPosition.y,0.0f);
	::setTransformSprite( mat );


	CWinCtrl * pCtrl = NULL;
	CWinCtrl * pCtrl2 = NULL;
	
	RECT	rcDraw, rcDrawCount;
	int		iGapY = 0;
	if(pCtrl = Find( "MATERIAL_00" ))
	{
		SetRect( &rcDraw,
				pCtrl->GetOffset().x,
				pCtrl->GetOffset().y,
				pCtrl->GetOffset().x + pCtrl->GetWidth(),
				pCtrl->GetOffset().y + pCtrl->GetHeight() );
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


	m_vecSingleString.clear();
	for( int i = 0; i < g_iMaxCountMaterial; ++i )
	{
		if( !MakeData.GetRequireMaterial(i).IsEmpty() )
		{			
			CSinglelineString sStrBuf;
			sStrBuf.set_string( MakeData.GetRequireMaterial(i).GetName(), rcDraw );
			sStrBuf.draw();

			if( sStrBuf.is_ellipsis() )
			{
				m_vecSingleString.push_back( sStrBuf );
			}			

			drawFontf( g_GameDATA.m_hFONT[ FONT_NORMAL ], true, &rcDrawCount ,DT_RIGHT, "%d", MakeData.GetRequireMaterial(i).GetRequireCount() );			
		}
		rcDraw.top		+= iGapY;
		rcDraw.bottom	+= iGapY;
		rcDrawCount.top += iGapY;
		rcDrawCount.bottom += iGapY;
	}


	if(pCtrl = Find("REQ_MP_NUM"))
	{
		SetRect( &rcDraw,
				pCtrl->GetOffset().x, 
				pCtrl->GetOffset().y, 
				pCtrl->GetOffset().x+pCtrl->GetWidth(), 
				pCtrl->GetOffset().y+pCtrl->GetHeight() );
		drawFontf( g_GameDATA.m_hFONT[ FONT_NORMAL ], true, &rcDraw ,g_dwWHITE,DT_RIGHT,"%d", MakeData.GetCosumeMP() );
	}	

	///콤보 Draw
	pCtrl = Find( IID_COMBOBOX_CLASS );
	if( pCtrl )
		pCtrl->OwnerDraw();

	pCtrl = Find( IID_COMBOBOX_ITEM );
	if( pCtrl )
		pCtrl->OwnerDraw();
}



void CMakeDLG::DrawSuccessPoints()
{
	CManufacture& MakeData = CManufacture::GetInstance();

	int iAddPosX = 0;
	int iWidth   = 88;
	int iMaxSuccessPoint = 1000;
	POINT ptDraw = { 27 - 4, 204 };
	int iImageIdx = CResourceMgr::GetInstance()->GetImageNID( UI_IMAGE_RES_ID,"ID_UPARROW_RED" );

	for( int i = 0; i < g_iMaxCountMaterial; ++i, ptDraw.y += 47 )
	{
		if( !MakeData.GetRequireMaterial(i).IsEmpty() )
		{
			iAddPosX = iWidth * MakeData.GetSuccessPoint( i ) / iMaxSuccessPoint;
			g_DrawImpl.Draw( m_sPosition.x + ptDraw.x + iAddPosX, m_sPosition.y + ptDraw.y, UI_IMAGE_RES_ID, iImageIdx );
		}
	}
}

void CMakeDLG::Update( POINT ptMouse)
{
	if( !IsVision() ) return;
	CTDialog::Update( ptMouse );

	m_MakeItemSlot.Update( ptMouse );
	for_each( m_listMaterialSlot.begin(), m_listMaterialSlot.end(), bind2nd ( mem_fun_ref( &CSlot::Update ), ptMouse ) );
	
	m_pCurrState->Update( ptMouse );


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
}

void CMakeDLG::Show()
{

#ifdef _DEBUG

	POINT pt = { GetPosition().x, GetPosition().y };
	Clear();
	Create("DlgMake");		
	SetInterfacePos_After();
	MoveWindow(pt);

#endif

	if( IsVision() )
		return;
	//*------------------------------------------------------------------------------------
	/// 클래스들 로딩
	CWinCtrl* pComponent = Find( IID_COMBOBOX_CLASS );
	assert( pComponent );
	if( pComponent == NULL ) return;
	CJComboBox* pCombo = (CJComboBox*)pComponent;
	pCombo->ClearItem();

	const std::list<int>& MakableClasses  = CManufacture::GetInstance().GetMakableClasses();

	if( MakableClasses.empty() )
		return;

	CTObject* pObj = NULL;

	std::list< int >::const_iterator iter;
	for( iter = MakableClasses.begin(); iter != MakableClasses.end(); ++iter )
	{
		pObj = new CMakeComboClass( *iter );
		pCombo->AddItem( pObj );
	}

	pCombo->SelectItem( 1 );

	//*------------------------------------------------------------------------------------
	ReloadItemList();

	CTDialog::Show();
	m_pCurrState = m_pMakeState[STATE_NORMAL];

}
bool CMakeDLG::Create( const char* IDD )
{
	CTDialog::Create( IDD );

	SAFE_DELETE(m_pMakeState[STATE_NORMAL]);
	m_pMakeState[STATE_NORMAL]	= new CMakeStateNormal( this );
	
	SAFE_DELETE(m_pMakeState[STATE_WAIT]);
	m_pMakeState[STATE_WAIT]	= new CMakeStateWait( this );

	SAFE_DELETE(m_pMakeState[STATE_RESULT]);
	m_pMakeState[STATE_RESULT]	= new CMakeStateResult( this );

	m_pCurrState				= m_pMakeState[STATE_NORMAL];

	CWinCtrl* pComponent = Find( IID_COMBOBOX_ITEM );
	if( pComponent && pComponent->GetControlType() == CTRL_JCOMBOBOX )
	{
		CJComboBox* pCombo = (CJComboBox*)pComponent;
		CJListBox* pJListBox = pCombo->GetListBoxPtr();
		if( pJListBox )
		{
			pJListBox->SetContainer( new CTContainer );
			pJListBox->SetCellRenderer( new CTListCellRenderer );
		}
	}

	

	pComponent = Find( IID_COMBOBOX_CLASS );
	if( pComponent && pComponent->GetControlType() == CTRL_JCOMBOBOX )
	{
		CJComboBox* pCombo = (CJComboBox*)pComponent;
		CJListBox* pJListBox = pCombo->GetListBoxPtr();
		if( pJListBox )
		{
			pJListBox->SetContainer( new CTContainer );
			pJListBox->SetCellRenderer( new CTListCellRenderer );
		}
	}

	return true;
}

void CMakeDLG::Hide()
{
	m_pCurrState->Hide();
	CTDialog::Hide();
	CManufacture::GetInstance().Clear();
}

unsigned int  CMakeDLG::Process(UINT uiMsg,WPARAM wParam,LPARAM lParam )
{
	if( !IsVision() )
		return 0;

	return m_pCurrState->Process( uiMsg, wParam, lParam );
}


void CMakeDLG::RecvResult( t_PACKET * pRecvPacket )
{
	///임시로 저장한다.
	m_btRESULT = pRecvPacket->m_gsv_CREATE_ITEM_REPLY.m_btRESULT;
	m_nStepORInvIDX = pRecvPacket->m_gsv_CREATE_ITEM_REPLY.m_nStepORInvIDX;
	m_CreateITEM	= pRecvPacket->m_gsv_CREATE_ITEM_REPLY.m_CreatedITEM;

	/////성공이나 실패시 추가
	if( m_btRESULT == RESULT_CREATE_ITEM_SUCCESS || m_btRESULT == RESULT_CREATE_ITEM_FAILED )
		memcpy( m_nPRO_POINT, pRecvPacket->m_gsv_CREATE_ITEM_REPLY.m_nPRO_POINT, sizeof( short ) * g_iMaxCountMaterial );


	ChangeState( STATE_RESULT );
}


int CMakeDLG::IsInValidShow()
{
	if( g_pAVATAR && g_pAVATAR->Get_HP() <= 0 )
		return 99;

	if( g_itMGR.IsDlgOpened( DLG_TYPE_DEAL ) )
		return DLG_TYPE_DEAL;

	if( g_itMGR.IsDlgOpened( DLG_TYPE_EXCHANGE ) )
		return DLG_TYPE_EXCHANGE;

	if( g_itMGR.IsDlgOpened( DLG_TYPE_DIALOG ))
		return DLG_TYPE_DIALOG;

	return 0;
}

bool CMakeDLG::IsVision()
{
	if( g_pAVATAR == NULL )
		return CTDialog::IsVision();
	else
		return( CTDialog::IsVision() && g_pAVATAR->Get_HP() > 0 );

	return false;
}

void CMakeDLG::Update( CObservable* pObservable, CTObject* pObj )
{
	assert( pObservable );
	if( pObj && strcmp( pObj->toString(), "CTEventMake") == 0 )
	{
		CTEventManufacture* pEvent = (CTEventManufacture*)pObj;
		int iIndex = pEvent->GetIndex();

		switch( pEvent->GetID() )
		{
		case CTEventManufacture::EID_ADD_MATERIAL:
			{
				if( iIndex >= 0 && iIndex < g_iMaxCountMaterial )
				{
					if( m_listMaterialSlot[iIndex].GetIcon() )
					{
						assert( 0 && "CIcon이 있는 Slot에 Overwrite 합니다@CMakeDLG::Update");
					}
					else
					{
						CItem* pItem = pEvent->GetItem();
						m_listMaterialSlot[iIndex].AttachIcon( pItem->CreateItemIcon() );
					}
				}
				else
				{
					assert( 0 && "Index is Invalid @CMakeDlg::Update@EID_ADD_MATERIAL");
				}
				break;
			}
		case CTEventManufacture::EID_REMOVE_MATERIAL:
			{

//				CIcon*		pIcon		= NULL;
//				CIconItem*	pItemIcon	= NULL;

				if( iIndex >= 0 || iIndex < (int)m_listMaterialSlot.size() )
				{
					assert( m_listMaterialSlot[iIndex].GetIcon() && "Detach Icon From Empty Slot" );
					m_listMaterialSlot[iIndex].DetachIcon();
				}
				else
				{
					assert( 0 && "Index is Invalid @CMakeDlg::Update@EID_REMOVE_MATERIAL" );
				}
				break;
			}
		case CTEventManufacture::EID_CHANGE_TARGETITEM:
			{
				CItem* pItem = 	pEvent->GetItem();
				m_MakeItemSlot.DetachIcon();
				if( pItem )
					m_MakeItemSlot.AttachIcon( pItem->CreateItemIcon() );
				break;
			}
		case CTEventManufacture::EID_CHANGE_ITEMLIST:
			{
				ReloadItemList();	
				break;
			}
		default:
			break;
		}
	}
	else
	{
		assert( pObj && "pObj is NULL or Invalid Type @CMakeDLG::Update" );
	}
}

bool CMakeDLG::IsModal()
{
	assert( m_pCurrState );
	return m_pCurrState->IsModal();
}

void CMakeDLG::ReloadItemList()
{
	/// 해당 클래스들의 
	CWinCtrl* pComponent = Find( IID_COMBOBOX_ITEM );
	if( pComponent == NULL )
	{
		assert( pComponent && "Combo Box 가 없네요" );
		return;
	}


	CJComboBox* pCombo = (CJComboBox*)pComponent;
	pCombo->ClearItem();

	const std::list<tagITEM>& MakableItems  = CManufacture::GetInstance().GetMakableItems();

	tagITEM Item;
	std::list<tagITEM>::const_iterator iterItem;
	CTObject* pObj = NULL;
	for( iterItem = MakableItems.begin(); iterItem != MakableItems.end(); ++iterItem )
	{
		Item = *iterItem;
		pObj = new CMakeComboItem( Item );
		pCombo->AddItem( pObj );
	}

	if( MakableItems.size() >= 1 )
	{
		pCombo->SelectItem( 1 );
		iterItem = MakableItems.begin();
		Item = *iterItem;
		CManufacture::GetInstance().SetMakeItem( Item );
	}
}


void CMakeDLG::SetInterfacePos_After()
{
	CWinCtrl * pCtrl = NULL;
	CWinCtrl * pCtrl2 = NULL;
 
	CSlot Slot;

	if(pCtrl = Find("ITEM_SLOT_00"))
	{			
		m_MakeItemSlot.SetOffset( pCtrl->GetOffset() );
		m_MakeItemSlot.SetParent( GetControlType() );
	}

	int iGapY = 0;
	if(pCtrl = Find("ITEM_SLOT_01"))
	{
		if(pCtrl2 = Find("ITEM_SLOT_02"))
		{
			iGapY = pCtrl2->GetOffset().y - pCtrl->GetOffset().y;
		}

		m_listMaterialSlot.reserve(g_iMaxCountMaterial);		///제조창에 올려진 아이템의 아이콘이 자리할 슬롯

		///제조할 아이템의 재료가 들어갈 Slot * 4
		for( int i = 0; i < g_iMaxCountMaterial; ++i )
		{
			Slot.SetOffset( pCtrl->GetOffset().x, pCtrl->GetOffset().y + iGapY*i );		
			Slot.SetParent( GetControlType() );
			Slot.SetDragAvailable();
			Slot.SetDragItem( m_pDragItem );

			m_listMaterialSlot.push_back( Slot );			
		}	
	}

	if(pCtrl = Find("BRANCH"))
	{	
        ((CTImage*)pCtrl)->SetText( LIST_STRING(797) );//분류
	}
	if(pCtrl = Find("MATERIAL_NAME"))
	{	
		((CTImage*)pCtrl)->SetText( LIST_STRING(798) );//재료명
	}
	if(pCtrl = Find("NUM"))
	{	
		((CTImage*)pCtrl)->SetText( LIST_STRING(790) );//개수
	}
	if(pCtrl = Find("REQ_MP"))
	{	
		((CTImage*)pCtrl)->SetText( LIST_STRING(791) );//소모 MP
	}

	if(pCtrl = Find(IID_BTN_START))
	{	
		//제조시작 894
		((CTImage*)pCtrl)->SetText( LIST_STRING(894) );
	}


	

	GetCaption()->SetString( LIST_STRING(796) ); //제조
	
	MoveWindow(GetPosition());

}