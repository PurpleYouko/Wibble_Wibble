
#ifndef _CHARACTER_DLG_H
#define _CHARACTER_DLG_H

#include "../../../TGameCtrl/TDialog.h"
#include "../../Common/CItem.h"
#include "../../Common/DataType.h"

/**
* ĳ���� ������ �����ֱ� ���� ���̾�α� 
*
* @Warning		����(2005/9/12) �븸(New)�������� ���� ������ �������� �ʴ´�( XML���� �ǹ�ư�� ���ּ� ���� �̵��Ҽ� ������ �Ǿ� �ִ� )
* @Author		������
* @Date			2005/9/12
*/
class CCharacterDLG : public CTDialog
{

public:
	CCharacterDLG( int iType );
	virtual ~CCharacterDLG();
	virtual void			Draw();
	virtual unsigned int	Process(UINT uiMsg,WPARAM wParam,LPARAM lParam);
	virtual void			Update( POINT ptMouse );
	virtual bool			IsVision();
	virtual bool			Create( const char* IDD );
	virtual void			Show();


	enum{
		IID_BTN_CLOSE = 10,
		IID_BTN_DIALOG2ICON,
		IID_TABBEDPANE			= 20,
		IID_TAB_BASICINFO		= 21,
		IID_TAB_BASICINFO_BG,
		IID_TAB_BASICINFO_BTN,
		IID_GUAGE_STAMINA		= 24,
		IID_TAB_ABILITY			= 31,
		IID_TAB_ABILITY_BG,
		IID_TAB_ABILITY_BTN,
		IID_BTN_UP_STR,
		IID_BTN_UP_DEX,
		IID_BTN_UP_INT,
		IID_BTN_UP_CON,
		IID_BTN_UP_CHARM,
		IID_BTN_UP_SENSE,
		IID_TAB_UNION			= 41,
		IID_TAB_UNION_BG,
		IID_TAB_UNION_BTN,

		IID_TXT_REQ_STR = 100,		
		IID_TXT_REQ_DEX,
		IID_TXT_REQ_INT,
		IID_TXT_REQ_CON,
		IID_TXT_REQ_CRM,
		IID_TXT_REQ_SEN,
	};
protected:

	enum{
		IID_TABABLITY = 0,
		IID_TABUNION  = 1,
		IID_TABBASIC = 2
	};
	enum{
		BASIC_INFO,
		ABILITY_INFO,
		UNION_INFO
	};
	bool On_LButtonUP( unsigned iProcID, WPARAM wParam, LPARAM lParam );
	bool On_LButtonDN( unsigned iProcID, WPARAM wParam, LPARAM lParam );


	void DrawBasicInfo();						/// �⺻���� Draw
	void DrawAbilityInfo();						/// �ɷ�ġ Draw
	void DrawUnionInfo();						/// �������� Draw

	void DrawBasicInfo2();						/// �⺻���� Draw
	void DrawAbilityInfo2();						/// �ɷ�ġ Draw
	void DrawUnionInfo2();						/// �������� Draw


	void	SetInterfacePos_After();


private:
	int			m_iTab;							/// �Ǳ���
	int			m_iGuageBlueGID;				/// ���� �������� �׷��� ID
	int			m_iGuageYellowGID;				/// ���׹̳� �������� �׷��� ID
	int			m_iGuageRedGID;					/// ü�� �������� �׷��� ID
};




#endif
