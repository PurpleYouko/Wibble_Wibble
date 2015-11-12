#ifndef _SKILLDLG_
#define _SKILLDLG_


#include "../SlotContainer/CSkillSlot.h"
#include "../../gamecommon/IObserver.h"
#include "subclass/cslot.h"

#include "TDialog.h"

class CZListBox;



/**
* 현재 배운 스킬에 대한 정보를 보여주는 다이얼로그
*
* @Warning		스킬트리창을 여는 버튼은 대만판에서는 제거되어 있다( XML Data )
* @Author		최종진
* @Date			2005/9/14
*/

class CSkillListItem;

class CSkillDLG  : public CTDialog, public IObserver
{
private:

	int				m_iCurrentSkillTab;	

public:
	CSkillDLG( int iType );
	virtual ~CSkillDLG();

	virtual void Draw( );
	virtual void Show( );
	virtual bool Create( const char* IDD );
	virtual bool IsVision();
	virtual unsigned int Process( UINT uiMsg,WPARAM wParam,LPARAM lParam );

	virtual void Update( CObservable* pObservable, CTObject* pObj );
	virtual void SetInterfacePos_After();
	virtual void RefreshDlg();

	POINT		Get_SkillSlot_Pos(int iIndex);

	void		Init_Skill();
	void		Set_Skill4Job(int iJobIndex);
	void		Set_Job_Desc(int iJobIndex);	

	void		Update_LearnSkill();
	enum
	{
		STAT_HIDE,
		STAT_SHOW,
		STAT_DEL,
	};
	void		Set_LearnSkill(int iSkillIndex, int iStatus );
	void		Set_LearnCondition(int iSkillIndex);

	enum
	{
		IID_TABBEDPANE		= 20,

		IID_TAB_BASIC		= 100,
		IID_TAB_BASIC_COMM	= 110,
		
		IID_TAB_000			= 200,

		IID_TAB_PARTS		= 300,
		IID_TAB_PARTS_CART	= 310,
		IID_TAB_PARTS_CASTLEGEAR	= 320,

		IID_TAB_UNIQUE		= 400,
		IID_TAB_UNIQUE_COMM	= 410,


		IID_TAB_111			= 500,
		IID_TAB_111_COMM	= 510,
		IID_TAB_111_ONE		= 520,
		IID_TAB_111_TWO		= 530,
		IID_TAB_121			= 540,
		IID_TAB_122			= 550,

		IID_TAB_211			= 600,
		IID_TAB_211_COMM	= 610,
		IID_TAB_211_ATTACK	= 620,
		IID_TAB_211_SUB		= 630,
		IID_TAB_221			= 640,
		IID_TAB_222			= 650,

		IID_TAB_311			= 700,
		IID_TAB_311_COMM	= 710,
		IID_TAB_311_BOW		= 720,
		IID_TAB_311_MEELE	= 730,
		IID_TAB_321			= 740,
		IID_TAB_322			= 750,
		
		IID_TAB_411			= 800,
		IID_TAB_411_COMM	= 810,
		IID_TAB_411_ATTACK	= 820,
		IID_TAB_411_MAKE	= 830,
		IID_TAB_421			= 840,
		IID_TAB_422			= 850,
		
		IID_BTN_CLOSE		= 900,
		IID_BTN_ICONIZE		= 901,		

		IID_BTN_MILEAGE		= 1000,
		IID_BTN_MILEAGE_1	= 1010,
		IID_BTN_MILEAGE_2	= 1020,
		IID_BTN_MILEAGE_3	= 1030,
		IID_BTN_MILEAGE_4	= 1040,

		IID_SKILLICON		= 300000,

		//100000			//배울 수 있는 스킬 아이콘 번호대.
		//200000			//직업이미지 아이콘 번호대 200000:비지터, 200111 ... ...
		//300000			//스킬 아이콘.

	};

protected:

	bool	On_LButtonUP( unsigned iProcID, WPARAM wParam, LPARAM lParam );
	void	DrawTexts();														
	bool	IsValidLevelUp( int iSkillSlotIdx );								/// 레벨업이 가능한 상태인가?
	void	RemoveSkillListItem( CZListBox* pListBox, int iSkillSlotIndex );
	void	UpdateSkillListBySkillLevelUp( CZListBox* pListBox );			/// 스킬의 레벨업시 스킬의 정보표시 갱신
	void	UpdateSkillLists();												/// 스킬에 대한 소모 능력치및 기타 변경사항 갱신
};


#endif //_SKILLDLG_