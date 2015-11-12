#ifndef _CSeparateDlg_
#define _CSeparateDlg_
#include "tdialog.h"
#include "../../GameCommon/IObserver.h"
#include "subclass/CSlot.h"
#include <vector>
#include ".\cseparatedlg.h"
#include "subclass/CSeparateDlgStateNormal.h"
#include "subclass/CSeparateDlgStateWait.h"
#include "subclass/CSeparateDlgStateResult.h"
#include "../Network/Net_Prototype.h"

#include "singlelinestring.h"

class CIconItem;
class CDragItem;

class CSeparateDlgState;
class CSeparateDlgStateResult;
class CSeparateDlgStatWait;
class CSeparateDlgNormal;

/**
* �и�/���ؿ� ���Ǵ� ���̾�α�
*	- Observable : CSeparate
* 
* @Author		������
* @Date			2005/9/14
*/
class CSeparateDlg : public CTDialog, public IObserver
{
	friend CSeparateDlg;
	friend CSeparateDlgStateNormal;
	friend CSeparateDlgStateWait;
	friend CSeparateDlgStateResult;

public:
	CSeparateDlg(void);
	virtual ~CSeparateDlg(void);

	virtual void Update( POINT ptMouse );
	virtual void Show();
	virtual void Hide();
	virtual void Draw();
	virtual void MoveWindow( POINT pt );
	virtual unsigned Process( unsigned uiMsg, WPARAM wParam, LPARAM lParam );
	virtual void SetInterfacePos_After();
	virtual void Update( CObservable* pObservable, CTObject* pObj );

	void ChangeState( int iID );

	static CSeparateDlg* GetInstance();
	static CSeparateDlg* s_Instance;

	enum STATE_TYPE{
		STATE_NORMAL,
		STATE_WAIT,
		STATE_RESULT,
		STATE_MAX
	};

	bool CheckInvEmptySlot();

	CSlot					m_MaterialItemSlot;/// �и�/������ �������� �� ����
	CSlot GetMaterialItemSlot();

	void  OutputItemSlotClear();

	std::vector< CSlot >	m_OutputItemSlots; /// �и�/������ �������� �� ����

	std::vector<tag_SET_INVITEM>  m_ResultItemSet;
private:
	vector<CSinglelineString>	m_vecSingleString; /// ������ �̸��� ���� �������� ���.

protected:
	bool	HasEnoughInventoryEmptySlot( CIconItem* pItemIcon, std::vector<CSlot>& OutputItems );

protected:
	enum{
		IID_TEXT_COST = 5,
		IID_BTN_START = 10,
		IID_BTN_CLOSE = 11,
	};


	CDragItem*				m_pDragItem;

	CSeparateDlgState*			m_pCurrState;
	CSeparateDlgState*			m_pStates[STATE_MAX];
	int	iViewItem;
};
#endif
