#ifndef _CTPUSHBUTTON_
#define _CTPUSHBUTTON_
#include "winctrl.h"
#include "SinglelineString.h"
/**
* 체크 버튼처럼 해당버튼을 클릭시 Normal과 Down이 반복되는 Button
*	- client에서 1:1거래창에서 사용중( 2005/8/30)
*
* @Author	최종진
*
* @Data		2005/8/30
*/
class CTPushButton : public CWinCtrl
{
public:
	CTPushButton(void);
	virtual ~CTPushButton(void);

	bool					Create( int iScrX, int iScrY, int iWidth, int iHeight, int iNormalGraphicID, int iOverGraphicID, int iDownGraphicID ,int iModuleID );
	virtual unsigned int	Process( UINT uiMsg,WPARAM wParam,LPARAM lParam );
	virtual	void			Update( POINT ptMouse );
	virtual void			Draw();	

	void					SetButtonState( int iState ){ m_iButtonState = iState; }
	int						GetButtonState(){ return m_iButtonState; }
	
	void					SetSoundOverID( int i ){ m_iOverSoundID = i; }
	void					SetSoundClickID( int i ){ m_iClickSoundID = i; }

	enum
	{
		BS_NORMAL,
		BS_MOUSEOVER,
		BS_CLICKED,
	};

protected:	

	int				m_iButtonState;
	int				m_iModuleID;
	int				m_iNormalGraphicID;
	int				m_iOverGraphicID;
	int				m_iDownGraphicID;

	int				m_iOverSoundID;
	int				m_iClickSoundID;
};
#endif
