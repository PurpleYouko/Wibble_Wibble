#ifndef _CSeparate_
#define _CSeparate_

#include "../GameCommon/CObservable.h"
#include "../GameCommon/IObserver.h"
#include "../Common/CItem.h"

#include "../interface/dlgs/subclass/CSlot.h"
#include "../interface/command/ctcmdopendlg.h"

#include "event/CTEventSeparate.h"
#include "../Network/Net_Prototype.h"
#include <vector>

class CItemFragment;
class CItem;



/**
* 분리/분해시 사용되는 Data Class
*
* @Author		최종진
* @Date			2005/9/15
*/
class CSeparate : public CObservable, public IObserver
{
	CSeparate(void);
	~CSeparate(void);

public:
	static CSeparate& GetInstance();

	virtual void Update( CObservable* pObservable, CTObject* pObj );

	void	SetItem( CItem* pItem );
	void	RemoveItem();

	int		GetRequireMp();
	int		GetRequireMoney();

//--------------------------------------------------------
//분해한 결과를 서버에서 받은 다음에 처리에 필요한 함수들.
	void ClearResultItemSet();
	void AddResultItemSet( tag_SET_INVITEM& Item );
	void ApplyResultItemSet();
	int  GetResultCnt();
//-------------------------------------------------------

	enum{
		TYPE_NONE			= 0,
		TYPE_SEPARATE		= 1,///분리 - 제밍
		TYPE_DECOMPOSITION	= 2,///분해 - 재조된 혹은 재료번호가가 있는 아이템
	};
	
	enum{
		TYPE_SKILL,
		TYPE_NPC
	};


	enum{
		ALL_OK,
		NOTENOUGH_MP,		/// MP 부족.
		NOTENOUGH_MATERIAL, /// 재료아이템의 개수 부족.
		INVALID_MATERIAL,   /// 필요한 재료아이템이 없다.
		INVALID_TARGETITEM, /// 분해할 아이템이 없거나 잘못된 아이템이다.
		NOTENOUGH_MONEY,
		INVALID_TYPE,
	};

	bool Send_CRAFT_BREAKUP_REQ();
	void SetSkillSlotIdx( int iSkillSlotIdx );
	void SetNpcSvrIdx( WORD wNpcSvrIdx );
	WORD GetNpcSvrIdx();
	int	 GetType();

	void SetResult( BYTE btResult ); // event를 result로 변경하고 게이지 그리기 모드로 전환.

	BYTE GetResult(); // 현재 상태

	bool SendPacketSeparate();

	int  GetMaterialCount();

	void RunSettingOutputItem(); // 서버 분해 성공했다고 보내온 아이템들의 출력 목록.

	void SetOutputItem();
	
	std::vector<tag_SET_INVITEM> CSeparate::GetResultItemSet();

	bool m_bWaitServerReply;

	int  m_iNumMat; //분해를 했을때 생성되는 아이템의 갯수.

	int GetOutputItemQuantity(int number);

	CreateMsgBoxData GetCreateMsgBoxData();

	CItemFragment* GetTargetItem();

private:
	int	GetQuantity( tagITEM& Item, int iRequireQuantity );

	int	IsValidSeparateCondition();

	CreateMsgBoxData m_MsgBoxData;

private:
	CTEventSeparate			m_Event;
	CItemFragment*			m_pMaterialItem;///분리 하고자 하는 아이템
	std::vector< CItem* >	m_pItems;		///분리후 생성될 아이템들
	int						m_iType;
	int						m_iRequireMp;
	int						m_iRequireMoney;
	int						m_iSkillSlotIdx;
	WORD					m_wNpcSvrIdx;

	BYTE					m_btResult;

	// 06. 08. 24 - 김주현 : 분해를 2회 이상해서 같은 아이템이 분해 결과로 나왔을 경우에 마지막에 온 아이템의 숫자가 최종숫자이다,
    // 인터페이스에 표현하기 위해서 저장공간을 2개로 만든다.
	std::vector<tag_SET_INVITEM>		m_ResultItemSet;		// 보여 주기 위해서 만든 아이템 목록


	int						m_listQuantity[10];
};
#endif