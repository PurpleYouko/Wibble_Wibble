#ifndef	_TIMAGE_
#define _TIMAGE_
#include "winctrl.h"

/**
* Image Class
*
* @Author	최종진
*
* @Date		2005/8/30
*/
class TGAMECTRL_API CTImage : public CWinCtrl
{
public:
	CTImage(void);
	virtual ~CTImage(void);

	bool					Create( int iScrX, int iScrY, int iWidth, int iHeight, int iGraphicID ,int iModuleID );
	virtual unsigned int	Process( UINT uiMsg,WPARAM wParam,LPARAM lParam );
	virtual	void			Update( POINT ptMouse);
	virtual void			Draw();
	virtual void			Draw( POINT ptDraw );

	void					SetImage( int iGraphicID, int iModuleID );
	void					SetBlinkImage( int iGraphicID, int iModuleID );
	
	void					SetBlink();
	void					SetUnblink();
	void					SetBlinkSwapTime( DWORD swap_time );

	//font
	void					SetFont(int iFont);
	void					SetText(const char * szText);
	void					SetAlign(DWORD dwAlign);
	void					SetTextColor(DWORD swColor);

	CTImage					* Clone();


protected:
	int		m_iModuleID;				// 보통상태의 모듈 ID
	int		m_iGraphicID;				// 보통상태의 그래픽 ID

	int		m_iBlinkGraphicID;			// 깜박일때의 그래픽 ID
	int		m_iBlinkModuleID;			// 깜박일때의 모듈 ID

	bool	m_bBlink;					
	bool	m_bBlinkEnable;

	DWORD   m_dwBlinkSwapTime;
	DWORD	m_dwBlinkPrevSwapTime;
	int		m_dwBlinkCurrentGID;
	int		m_dwBlinkCurrentMID;

	ITFont*			m_pFontImpl;
	int				m_iFont;
	std::string		m_strText;
	DWORD			m_dwAlgin;
	DWORD			m_dwTextColor;
};

#endif ///_TIMAGE_
