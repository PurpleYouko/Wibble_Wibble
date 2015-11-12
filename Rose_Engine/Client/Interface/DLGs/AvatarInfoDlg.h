#ifndef _CAVATARINFODLG_
#define _CAVATARINFODLG_

#include "../../tgamectrl/tdialog.h"
#include "../gamecommon/IObserver.h"
#include "subclass/CSlot.h"

class CItem;

/**
* ȭ�� ������ �� ��Ÿ�ٿ� ���� ������ ǥ���ϴ� �������̽�
*	- Observable : CItemSlot
*
* @Author		������
* @Date			2005/9/6
*/
class CAvatarInfoDlg : public CTDialog, public IObserver
{
public:
	CAvatarInfoDlg( int iType );
	virtual ~CAvatarInfoDlg(void);
	virtual bool Create ( const char* IDD );
	virtual void Draw();
	virtual void Update( POINT ptMouse );
	virtual void MoveWindow( POINT pt );
	virtual unsigned Process(UINT uiMsg,WPARAM wParam,LPARAM lParam);
	virtual void Update( CObservable* pObservable, CTObject* pObj );
	
	virtual void Show();
	virtual void Hide();
	virtual void RefreshDlg();
	virtual void SetInterfacePos_After();

	void		SetMaxView(bool bIsMax = true);
	void		SetMinView();
	bool		GetIsMaxView();

	void		SetMiniMapPos();
	void		SetOptionStatus(const char * pszOption, const char * pszStatus);

	enum
	{
		IID_GUAGE_HP = 6,					/// ü�°�����
		IID_GUAGE_MP,						/// ����������
		IID_GUAGE_EXP,						/// ����ġ������
		IID_BTN_SELFTARGET = 10,			/// Ŭ���� �ڽż��� - NOIMAGE 
		IID_BTN_MENU,						/// �޴�â�� ���� ��ư
		IID_BTN_DIALOG2ICON,				/// ��� Dialog Iconȭ ��ư, Iconȭ �Ұ����� Dialog�� ��� ���� ���Ѵ�.(TGameCtrl���� ó���Ұ�, ����� iconȭ �������� Setting�Ұ� )
		IID_BTN_SCREENSHOT,					/// ��ũ���� ��� ��ư
		IID_BTN_SETSIZE,

		IID_BTN_CTRL		= 100,
		IID_BTN_TIP			= 101,
		IID_BTN_CHAT		= 102,
		IID_BTN_SOUND		= 103,
		IID_BTN_SHORTCUT_SIZE		= 4,
	};
	

private:
	CSlot	m_Slot;							/// ������ ���⿡ ���� �������� �� ����
	CItem*	m_pRepresentBullet;				/// ������ ���⿡ ���� �Ҹ�ź
	bool	m_bIsMaxView;					/// Ȯ�� or ���

};
#endif