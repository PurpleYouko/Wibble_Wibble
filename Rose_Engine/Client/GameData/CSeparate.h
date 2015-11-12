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
* �и�/���ؽ� ���Ǵ� Data Class
*
* @Author		������
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
//������ ����� �������� ���� ������ ó���� �ʿ��� �Լ���.
	void ClearResultItemSet();
	void AddResultItemSet( tag_SET_INVITEM& Item );
	void ApplyResultItemSet();
	int  GetResultCnt();
//-------------------------------------------------------

	enum{
		TYPE_NONE			= 0,
		TYPE_SEPARATE		= 1,///�и� - ����
		TYPE_DECOMPOSITION	= 2,///���� - ������ Ȥ�� ����ȣ���� �ִ� ������
	};
	
	enum{
		TYPE_SKILL,
		TYPE_NPC
	};


	enum{
		ALL_OK,
		NOTENOUGH_MP,		/// MP ����.
		NOTENOUGH_MATERIAL, /// ���������� ���� ����.
		INVALID_MATERIAL,   /// �ʿ��� ���������� ����.
		INVALID_TARGETITEM, /// ������ �������� ���ų� �߸��� �������̴�.
		NOTENOUGH_MONEY,
		INVALID_TYPE,
	};

	bool Send_CRAFT_BREAKUP_REQ();
	void SetSkillSlotIdx( int iSkillSlotIdx );
	void SetNpcSvrIdx( WORD wNpcSvrIdx );
	WORD GetNpcSvrIdx();
	int	 GetType();

	void SetResult( BYTE btResult ); // event�� result�� �����ϰ� ������ �׸��� ���� ��ȯ.

	BYTE GetResult(); // ���� ����

	bool SendPacketSeparate();

	int  GetMaterialCount();

	void RunSettingOutputItem(); // ���� ���� �����ߴٰ� ������ �����۵��� ��� ���.

	void SetOutputItem();
	
	std::vector<tag_SET_INVITEM> CSeparate::GetResultItemSet();

	bool m_bWaitServerReply;

	int  m_iNumMat; //���ظ� ������ �����Ǵ� �������� ����.

	int GetOutputItemQuantity(int number);

	CreateMsgBoxData GetCreateMsgBoxData();

	CItemFragment* GetTargetItem();

private:
	int	GetQuantity( tagITEM& Item, int iRequireQuantity );

	int	IsValidSeparateCondition();

	CreateMsgBoxData m_MsgBoxData;

private:
	CTEventSeparate			m_Event;
	CItemFragment*			m_pMaterialItem;///�и� �ϰ��� �ϴ� ������
	std::vector< CItem* >	m_pItems;		///�и��� ������ �����۵�
	int						m_iType;
	int						m_iRequireMp;
	int						m_iRequireMoney;
	int						m_iSkillSlotIdx;
	WORD					m_wNpcSvrIdx;

	BYTE					m_btResult;

	// 06. 08. 24 - ������ : ���ظ� 2ȸ �̻��ؼ� ���� �������� ���� ����� ������ ��쿡 �������� �� �������� ���ڰ� ���������̴�,
    // �������̽��� ǥ���ϱ� ���ؼ� ��������� 2���� �����.
	std::vector<tag_SET_INVITEM>		m_ResultItemSet;		// ���� �ֱ� ���ؼ� ���� ������ ���


	int						m_listQuantity[10];
};
#endif