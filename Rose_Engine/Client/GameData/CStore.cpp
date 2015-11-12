#include "stdafx.h"
#include ".\cstore.h"
//#include "../gamecommon/CItemEquip.h"
//#include "../gamecommon/CItemEtc.h"
//#include "../gamecommon/CItemUse.h"
//#include "../gamecommon/CItemPat.h"
#include "../gamecommon/Item.h"
#include "../object.h"
#include "../gamecommon/itemcommand.h"
#include "../interface/command/CTCmdNumberInput.h"
#include "../interface/command/UICOMMAND.h"
#include "../interface/it_mgr.h"

int CStore::CLAN_STORE_INDEX = 11;	//LIST_NPC.STB 20번 컬럼 값 11인 경우 클랜 상점.

CStore::CStore(void)
{
	m_iUnionNo					= 0;
	m_pCmdOpenNumberInputDlg	= new CTCmdOpenNumberInputDlg;
	m_pCmdAddItem2DealFromStore = new CTCmdAddItem2DealFromStore;

	m_pCmdOpenNumberInputDlg->SetCommand( m_pCmdAddItem2DealFromStore );
}

CStore::~CStore(void)
{
	SAFE_DELETE( m_pCmdAddItem2DealFromStore );
	SAFE_DELETE( m_pCmdOpenNumberInputDlg );
}

CStore& CStore::GetInstance()
{
	static CStore s_Instance;
	return s_Instance;
}

bool CStore::ChangeStore( int iNpcObjIndex, bool bSpecialTab )
{
	assert( g_pObjMGR->m_pOBJECTS[ iNpcObjIndex ] );

	if(  g_pObjMGR->m_pOBJECTS[ iNpcObjIndex ]->IsA( OBJ_NPC ) == false )
		return false;

	m_iNpcObjIndex = iNpcObjIndex;
	
	int iNpcNo = ((CObjCHAR*)g_pObjMGR->m_pOBJECTS[ iNpcObjIndex ])->Get_CharNO();

	m_iUnionNo = NPC_UNION_NO( iNpcNo );

	if( m_iUnionNo > 0 )
	{
#ifdef __CLAN_STORE //홍근 오전 10:53 2007-01-12 클랜 상점 기능 추가.
					//상점열게 하는 부분에서 클랜 유무 체크는 CON에서 한다.
					//유니온 상점과 클랜 상점은 같은 20번 컬럼을 쓰므로 혼용 할 수 없다.
		if( NPC_UNION_NO( iNpcNo ) == CLAN_STORE_INDEX )
		{			
		}
		else
#endif
		if( m_iUnionNo != g_pAVATAR->Get_UNION() )
		{
			g_itMGR.OpenMsgBox( STR_CANT_OPEN_UNIONSTORE );		
			return false;
		}
	}
	

	short nDeal[ 4 ] = { 0, 0, 0, 0 };

	nDeal[ 0 ] = NPC_SELL_TAB0(iNpcNo);
	nDeal[ 1 ] = NPC_SELL_TAB1(iNpcNo);
	nDeal[ 2 ] = NPC_SELL_TAB2(iNpcNo);

	if(bSpecialTab) 
		nDeal[ 3 ] = NPC_SELL_TAB3(iNpcNo);
	else
		nDeal[ 3 ] = 0;

	tagITEM sITEM;
	sITEM.Clear();

	for( int iTab = 0; iTab < 4; ++iTab )
		m_strTabName[iTab].clear();

	std::map<int, CItem*>::iterator iter;
	for( iter = m_pItems.begin(); iter != m_pItems.end(); )
	{
		iter->second->SetItem( sITEM );
		SetChanged();
		NotifyObservers( iter->second );
		iter = m_pItems.erase( iter );
	}

	

	//상점에 아이템을 등록해준다 
	short	nI;
	short	nItemNo;


	CItem* pItem = NULL;
	for( int i = 0; i < 4 ; i++ )
	{
		if( nDeal[ i ] ) 
		{
			if( STORE_NAME( nDeal[i] ) )
			{
				m_strTabName[i]= STORE_NAME( nDeal[ i ] );

				for( nI = 0 ; nI < c_iSlotCountPerTab ; nI++ ) 
				{
					nItemNo = STORE_ITEM( nDeal[ i ], nI );
					if( nItemNo )
					{
						sITEM.Init( nItemNo );
						pItem =	new CItem;
						if( pItem )
						{
							pItem->SetIndexType( CItem::IT_STORE );
							pItem->SetIndex( i * c_iSlotCountPerTab + nI );
							
							if( sITEM.IsEnableDupCNT() )
								sITEM.m_uiQuantity = 999;

							pItem->SetItem( sITEM );
							pItem->SetCommand( m_pCmdOpenNumberInputDlg );
							
							m_pItems.insert(std::map<int, CItem*>::value_type( i * c_iSlotCountPerTab + nI , pItem ));
						
							SetChanged();
							NotifyObservers( pItem );
						}
					}
				}
			}
			else
			{
				assert( 0 && "STORE_NAME is NULL@CStore::ChangeStore" );
			}
		}
	}
	
	return true;
}

int CStore::GetNpcObjIndex()
{
	return m_iNpcObjIndex;
}
std::string& CStore::GetTabName( int i )
{
	if( i < 0 || i >= 4 )
	{
		assert( 0 && CStr::Printf("Invalid Index	(%d)", i ) );
		return m_strTabName[0];
	}
	return m_strTabName[i];
}

CItem* CStore::GetItem( int iIndex )
{
	std::map<int, CItem*>::iterator iter;
	iter = m_pItems.find( iIndex );
	if( iter != m_pItems.end() )
		return iter->second;

	return NULL;
}

int CStore::GetUnionNo()
{
	return m_iUnionNo;
}

bool CStore::IsUnionStore()
{
	return ( m_iUnionNo > 0 && m_iUnionNo <= 10 ) ? true : false;
}

bool CStore::IsClanStore()
{
	return (m_iUnionNo == CLAN_STORE_INDEX) ? true : false;
}


