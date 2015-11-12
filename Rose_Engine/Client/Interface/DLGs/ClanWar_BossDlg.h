#ifndef _CCLAN_WAR_BOSSDLG_
#define _CCLAN_WAR_BOSSDLG_

#include "TDialog.h"

struct gsv_cli_CLAN_WAR_BOSS_HP;
class CClanWar_BossDlg : public CTDialog
{
public:
	CClanWar_BossDlg();
	CClanWar_BossDlg( int iType );
	virtual ~CClanWar_BossDlg(void);

	virtual unsigned	Process( unsigned uiMsg, WPARAM wParam, LPARAM lParam );
	virtual void		SetInterfacePos_After();
	virtual void		Update( POINT ptMouse );
	virtual void		Show();
	virtual void		Hide();	
	virtual void		Draw();

	void				Init();
	void				Set_Boss(gsv_cli_CLAN_WAR_BOSS_HP * pBossHp );	
	

	enum
	{
		IID_TEAM_A			 = 6,	//ClanWar Team Type A
		IID_TEAM_B,					//ClanWar Team Type B
		IID_BOSS_HP,				//ClanWar Boss HP
	};


private:

	int			m_iHp;
	int			m_iMaxHp;
	std::string	m_strBossName;

};


#endif