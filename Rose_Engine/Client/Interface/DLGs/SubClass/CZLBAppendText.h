#ifndef _CZLBAppendText_
#define _CZLBAppendText_
#include "winctrl.h"
#include "JStringParser.h"
/**
* ZListBox에 태그들을 적용해서 Append 하기 위한 데이터 클래스.
*
* @Author		김주현
* @Date			2006/10/24
*/
class CZLBAppendText :	public CWinCtrl
{
public:
	CZLBAppendText();

	CZLBAppendText( const char* pszText,int iWidth, DWORD dwColor = D3DCOLOR_ARGB( 255, 255, 255, 255));
	CZLBAppendText( CJString ptJString, int iWidth, DWORD dwColor = D3DCOLOR_ARGB( 255, 255, 255, 255));

	~CZLBAppendText(void);

	void Draw();

protected: 

	std::string		m_pszText;
	int				m_iLineHeight;

	int				m_iBorderWidth;

	CJStringParser	m_jString;

	CJString*		m_pJString;	
};
#endif