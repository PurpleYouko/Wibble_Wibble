#ifndef _CClanDlg_
#define _CClanDlg_
#include "tdialog.h"
#include "../../gamecommon/IObserver.h"
#include "../CClanMarkUserDefined.h"

class CClanMemberItem;
class CTabbedPane;
class CZListBox;
class CTListBox;

/**
* Ŭ�������� �����ִ� ���̾�α�
*	- Observable : CClan
*
* @Author		������
* @Date			2005/9/12
*/
class CClanDlg : public CTDialog, public IObserver
{
public:
	CClanDlg(void);
	virtual ~CClanDlg(void);
	
	virtual void		Draw();
	virtual void		Show();
	virtual void		Hide();
	virtual bool		Create( const char* IDD );
	virtual unsigned	Process( unsigned uiMsg, WPARAM wParam, LPARAM lParam );

	virtual void		Update( CObservable* pObservable, CTObject* pObj );
			
			void		SetSelectedMember( int iClanPoint );
	
	virtual void		SetInterfacePos_After();
private:
	enum{
		IID_BTN_CLOSE				= 10,
		IID_BTN_ICONIZE				= 11,
		IID_TABBEDPANE				= 20,
		IID_BTN_TAB_INFO			= 25,
		IID_BTN_PREVIEW				= 28,
		IID_BTN_REGIST_CLANMARK		= 29,
		IID_BTN_TAB_MEMBER			= 35,
		IID_ZLIST_MEMBER			= 36,
		IID_BTN_ENTRUST				= 41,
		IID_BTN_BAN					= 42,
		IID_BTN_CLASS_UP			= 43,
		IID_BTN_CLASS_DOWN			= 44,
		IID_BTN_REQJOIN				= 45,
		IID_BTN_WITHDRAWAL			= 46,
		IID_BTN_TAB_SKILL			= 55,
		IID_ZLIST_SKILL				= 56,
		IID_BTN_TAB_NOTICE			= 70,
		IID_ZLIST_NOTICE			= 75,
		IID_ZLIST_NOTICE_CONTENT	= 77,
		IID_BTN_REGIST_NOTICE		= 80,
		IID_BTN_DELETE_NOTICE		= 81,
		IID_LIST_SLOGAN				= 82,
		IID_SCROLLBAR_SLOGAN		= 83,
		IID_SCROLLBAR_SLOGAN_PBTN	= 1,	//��ũ�ѹ� ��ư�� ��ȣ�� �̿������� �ʰ�, üũ�� �Ǹ� �ȴ�.
		IID_SCROLLBAR_SLOGAN_NBTN	= 1,
	};

	enum{
		TAB_MEMBER	= 0,
		TAB_SKILL	= 1,
		TAB_NOTICE	= 2,
		TAB_INFO	= 3
	};

	void		ClearClanData();					/// �⺻���� ���ڿ� ����.
	void		SetClanData();						/// �⺻���� ���ڿ� ����.

	void		DrawInfomation();					/// �⺻���� �� Draw
	void		DrawMembers();						/// ������� �� Draw
	void		DrawSkills();						/// Ŭ����ų �� Draw
	void		DrawNotice();						/// �������� �� Draw
	
	void		DrawInfomation2();					/// �⺻���� �� Draw
	

	void		DrawMembers2();						/// ������� �� Draw
	void		DrawSkills2();						/// Ŭ����ų �� Draw
	void		DrawNotice2();						/// �������� �� Draw

	void		OnLButtonDown( unsigned uiProcID, WPARAM wParam, LPARAM lParam );
	void		OnLButtonUp( unsigned uiProcID, WPARAM wParam, LPARAM lParam );

	void		SortMemberlistByClass();			/// ��޺��� ��Ʈ

	CWinCtrl*	FindCtrlInTabbedPane( CTabbedPane* pPane, int iTabID, int iCtrlID );

	void		RemoveClanSkillListItem( CZListBox* pListBox, int iSkillSlotIndex );

	CClanMemberItem* GetSelectedMember();



	int		m_iTab;									/// �� ����
	int		m_iClassMarkImageNo[ 7 ];				/// Ŭ�� ����� ��޿� ���� �̹��� GraphicID ����
	

	std::string m_strSelectedMemberView;			/// ��������ǿ��� ���õ� ���ǥ�ýÿ� ���̴� ���� ����

	CClanMarkUserDefined	m_ClanMarkPreView;		/// Ŭ����ũ View Class
	D3DVECTOR				m_vCharPos;				/// Ŭ����ũ ��ġ

	CTListBox				*m_pListBox_Slogan;
};
#endif