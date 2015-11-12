#ifndef _CTPane_
#define _CTPane_
#include "winctrl.h"
#include "JContainer.h"

/**
* Dialog내에서의 영역별로 child control들을 구분하기 위한 Container class
*   - 클라이언트의 CChatRoomDlg에서 위/아래 영역이 이 Container class로 구분되어 있다.
*
* @Author		최종진
* @Data			2005/8/30
*/

class TGAMECTRL_API CTPane : public CWinCtrl
{
public:
	CTPane(void);
	virtual ~CTPane(void);

	virtual void Update( POINT ptMouse );
	virtual void Draw();
	virtual unsigned Process( unsigned uiMsg, WPARAM wParam, LPARAM lParam );
	virtual void MoveWindow( POINT pt );
	virtual void Show();
	virtual void Hide();

	virtual void		AddChild( CWinCtrl* pCtrl );
	virtual void		DelChildAll();
	virtual void		DelChild( CWinCtrl* pCtrl );
	virtual CWinCtrl*	FindChild( unsigned iID );
	virtual CWinCtrl*	Find( const char * szName );
	virtual CWinCtrl*	Find( int iID );

	vector<CWinCtrl*> & GetChild()
	{
		return m_Children.GetChild();
	}

	CWinCtrl * GetChild( int iIndex )
	{
		return m_Children.GetChild( iIndex );
	}

protected:
	CJContainer		m_Children;
};
#endif