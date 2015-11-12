#ifndef _MAKE_DLG_H
#define _MAKE_DLG_H

#include "../../../TGameCtrl/TDialog.h"
#include "../../../TGameCtrl/TContainerItem.h"
#include "../../Common/CItem.h"
#include "../../gamedata/cmanufacture.h"
#include "../../Common/DataType.h"
#include "../../gamecommon/item.h"
#include "../../gamecommon/iobserver.h"
#include "SubClass/CSlot.h"
#include "singlelinestring.h"


class	CMakeState; 
class	CMakeStateNormal;
class	CMakeStateWait;
class	CMakeComboItem;
class	CMakeComboClass;

struct t_PACKET;
class CDragItem;
/**
* ?�조?�에 ?�용?�는 ?�이?�로�?
*
* @Author		최종�?
* @Date			2005/9/14
*/
class CMakeDLG : public CTDialog, public IObserver
{
	///State
	friend class	CMakeStateNormal;
	friend class	CMakeStateWait;
	friend class    CMakeStateResult;
	///Command
	friend class	CTCmdTakeInItem2MakeDlg;
	friend class	CTCmdTakeOutItemFromMakeDlg;
public:

	CMakeDLG( int iType );
	virtual ~CMakeDLG();	
	virtual void Draw();
	virtual void Update( POINT ptMouse);
	virtual void Show();
	virtual void Hide();
	virtual unsigned int Process(UINT uiMsg,WPARAM wParam,LPARAM lParam );
	virtual void MoveWindow( POINT pt );
	virtual int	 IsInValidShow();
	virtual bool IsVision();
	virtual bool Create( const char* IDD );
	virtual bool IsModal();

	virtual void Update( CObservable* pObservable, CTObject* pObj );
	virtual void SetInterfacePos_After();


	enum CTRL_ID{
		
		IID_BTN_START	= 10,
		IID_BTN_CLOSE	= 11,
		IID_COMBOBOX_CLASS	= 25,
		IID_COMBOBOX_ITEM	= 20
	};

	void	RecvResult( t_PACKET * pRecvPacket );
	void	ChangeState( int iState );

private:
	enum STATE_TYPE{
		STATE_NORMAL,
		STATE_WAIT,
		STATE_RESULT,
		STATE_MAX
	};


	///첫번�??�롯???�이?�이 ?�록?�후 계산?�다.
	///만약 첫번�??�이?�이 ?�다�??�상?�률??계산?�수 ?�다( fMatQuality );
	void	DrawSuccessPoints();
	void	ReloadItemList();

private:

	BYTE    m_btRESULT;
	short	m_nStepORInvIDX;
	tagITEM	m_CreateITEM ;
	short	m_nPRO_POINT[g_iMaxCountMaterial];

	CDragItem*			m_pDragItem;
	CMakeState*			m_pCurrState;
	CMakeState*			m_pMakeState[STATE_MAX];

	CSlot						m_MakeItemSlot;			///?�조???�이???�이콘이 ?�어�??�롯
	std::vector<CSlot>			m_listMaterialSlot;		///?�조창에 ?�려�??�이?�의 ?�이콘이 ?�리???�롯

	vector<CSinglelineString>	m_vecSingleString;
};

/**
* ?�조 Dialog?�서 Combo???�어지??item Class
*/
class CMakeComboItem : public CTContainerItem
{
public:
	CMakeComboItem( tagITEM& Item );
	virtual ~CMakeComboItem(void){}
	tagITEM&	GetItem();
protected:
	tagITEM m_Item;
};

class CMakeComboClass : public CTContainerItem
{
public:
	CMakeComboClass( int iClass );
	virtual ~CMakeComboClass(void){}
	int			GetClass();
protected:
	int			m_iClass;
	std::string m_strName;
};

#endif