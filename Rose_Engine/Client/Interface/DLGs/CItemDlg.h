#ifndef _CItemDlg_
#define _CItemDlg_
#include "tdialog.h"

#include <map>
#include "subclass/CSlot.h"
#include "../../gamecommon/IObserver.h"
#include "../../CClientStorage.h"

#include "IActionListener.h"
class CDragItem;
class CTCmdNumberInput;
/**
* 보유한 아이템및 장착된 아이템의 정보를 보여주는 다이얼로그
*	- 서버와는 별도로 클라이언트에서만 인벤토리의 아이템의 위치를 이동시킬수 있으면 저장되어 진다.( 당연히 이동된 아이템의 위치는 같은 PC에서만 적용된다 )
*	- Observable : CItemSlot
*
* @Author		최종진
* @Date			2005/9/12
*/


// 06. 03. 10 - 김주현 : 들어갈수 없는 인벤의 탭에 아이템이 들어가는 버그를 수정.
// 좀더 깔끔하게 다듬을려면.. Drop 커맨드에서.. 그 아이템이 들어갈수 있는지 없는지
// 판단하는게 깔끔할듯...

static int		m_iInventoryStorge; // 바뀌기 전의 탭 번호 저장.
static int      iprevPane; // 현재 탭의 번호를 저장.
class CItemDlg : public CTDialog, public IObserver, public IActionListener
{
public:
	CItemDlg( int iType );
	virtual ~CItemDlg(void);

	virtual void Draw();
	virtual void Update( POINT ptMouse );
	virtual void Show();
	virtual void MoveWindow( POINT pt );
	virtual unsigned	Process( unsigned uiMsg, WPARAM wParam, LPARAM lParam );


	virtual void		Update( CObservable* pObservable, CTObject* pObj );



	virtual unsigned	ActionPerformed( CActionEvent* e );
	
	CSlot		* SearchItem_Inventory( int iItemIndex );

	bool		IsInsideInven( POINT pt );											/// 좌표가 인벤토리 슬롯 영역	위인가?
	bool		IsInsideEquip( POINT pt );											/// 좌표가 장착 슬롯 영역 위인가?

	
	int			GetEquipSlot( POINT pt );											/// 좌표로 장비 장착창 슬롯 번호를 구한다.
	CSlot*		GetInvenSlot( POINT pt );											/// 좌표로 인벤토리 슬롯 번호를 구한다.

	void		ApplySavedVirtualInventory( std::list<S_InventoryData>& list );
	void		GetVirtualInventory( std::list<S_InventoryData>& list );

	void		AddActionEventListener2Slots( );
	void		RemoveActionEventListener2Slots( );


	void		SetInterfacePos_After();

	CSlot*		GetInvenSlotByRealIndex( int iIndex );								/// 실제 인벤토리 인덱스로 슬롯 인덱스를 구한다.

	void		SortItems();

private:
	void		SwitchIcon( int iReal, int iVirtual );								/// 실제 인벤토리 인덱스와 가상 인벤토리인덱스로 아이콘 위치 이동
	
	
	void		OnLButtonDown( unsigned uiProcID, WPARAM wParam, LPARAM lParam );	
	void		OnLButtonUp( unsigned uiProcID, WPARAM wParam, LPARAM lParam );		
	
	bool		ProcessSlots( unsigned uiMsg, WPARAM wParam, LPARAM lParam );		/// 슬롯들의 CTDialog::Process처리

	CWinCtrl*	FindChildInPane( unsigned uiPaneID, unsigned uiChildID );
	void		Minimize();															/// 최소화 처리
	void		Maximize();															/// 최대화 처리


	bool		IsAvailableRepair(  CIcon* pIcon  );								/// 수리 가능한 아이템인가?
	bool		IsAvailableMakeSocket( CIcon* pIcon );								/// 아이템의 소켓 생성 가능 여부

	bool		IsInside4Move(int x, int y);

	void		SetEquipTab(int iIndex);


private:
	enum{
		IID_BTN_CLOSE				= 10,
		IID_BTN_ICONIZE				= 11,
		IID_BTN_MONEY				= 12,
		IID_EQUIP_PAB				= 20,
		IID_BTN_EQUIP_PAT			= 23,
		IID_BTN_EQUIP_AVATAR		= 33,
		IID_TABBEDPANE_INVEN_ITEM	= 50,
		IID_BTN_INVEN_EQUIP			= 53,
		IID_BTN_INVEN_USE			= 63,
		IID_BTN_INVEN_ETC			= 73,
		IID_BTN_INVEN_MILEAGE		= 83,

		IID_BTN_PAT_EQUIP			= 103,
		IID_BTN_PAT_ETC				= 113,
		IID_BTN_PAT_MILEAGE			= 123,

		IID_TABBEDPANE_INVEN_PAT	= 100,
		IID_PANE_EQUIP				= 200,
		IID_BTN_MINIMIZE			= 213,
		IID_BTN_MAXIMIZE			= 214,
		IID_PANE_INVEN				= 300,
		IID_RBTN_EQUIP				= 400,
	};

	enum
	{
		INVEN_ITEM = 0,
		INVEN_PAT,
	};
	int		m_iEquipTab;													/// 0 ~ 1
	int		m_iInventoryTab;												/// 0 ~ 3
	int		m_iPatTab;														/// 0 ~ 2

	CSlot	m_AvatarEquipSlots[MAX_EQUIP_IDX-1];							/// 캐릭터 아이템 장착 슬롯
	CSlot	m_BulletEquipSlots[MAX_SHOT_TYPE];								/// 소모탄 아이콘 슬롯

	CSlot	m_PatEquipSlots[MAX_RIDING_PART];								/// PAT 아이템 장착 슬롯

	CSlot	m_ItemSlots[MAX_INV_TYPE][INVENTORY_PAGE_SIZE];					/// 모든 인벤토리 아이템 슬롯


	CDragItem*		m_pInvenDragItem;										/// 인벤토리의 아이템 아이콘 드래그 아이템
	CDragItem*		m_pEquipDragItem;										/// 장착중인 아이템 아이콘 드래그 아이템

	CTCmdNumberInput*	m_pCmdDropMoney;
	CTCmdNumberInput*	m_pCmdAddMyMoney2Exchange;

	bool				m_bMinimize;										/// 최소화 or 최대화

};
#endif