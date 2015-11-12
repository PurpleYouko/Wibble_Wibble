#ifndef	_TLISTBOX_
#define _TLISTBOX_
#include "winctrl.h"
#include "IScrollModel.h"
#include <deque>

using namespace std;

typedef struct {
	char		m_szTxt[MAX_PATH];
	D3DCOLOR	m_dwColor;
	bool		m_bDrawn;
}t_list_item;


typedef deque<t_list_item>						li_item_vec;
typedef deque<t_list_item>::iterator			li_item_vec_itor;
typedef deque<t_list_item>::const_iterator		li_item_vec_const_itor;

class CTScrollBar;
class ITFont;

/**
* ListBox Class : t_list_item by item Can have as a ListBox
*	- ScrollBar So that ?butilsu? IScrollModel by Implement that
*
* @Author	√÷¡æ¡¯
*
* @Date		2003/11/26
*/
class TGAMECTRL_API CTListBox :	public CWinCtrl, public IScrollModel
{
public:
	CTListBox(void);
	virtual ~CTListBox(void);

	bool					Create( int iScrX, int iScrY, int iWidth, int iHeight ,int iExtent,int iCharWidth, int iCharHeight );
	virtual unsigned int	Process( UINT uiMsg,WPARAM wParam,LPARAM lParam );
	virtual	void			Update( POINT ptMouse );
	virtual void			Draw();

	/// IScrollModel Its implementation
	virtual int GetValue();
	virtual int GetExtent();
	virtual int GetMaximum();
	virtual int GetMinimum();

	virtual void SetValue( int i );
	virtual void SetExtent( int i );
	virtual void SetMaximum( int i );
	virtual void SetMinimum( int i );

	virtual RECT GetWindowRect();
	virtual bool IsLastItemDrawn();
	/// Add or delete text
	void	AppendText(const char* szTxt,D3DCOLOR dwColor,bool bAutoIncValue = true );
	void	SetText( int index, const char* text, D3DCOLOR dwColor );
	void	DeleteText(int iLineNo);
	const char*	GetText(int iLineNo);
	t_list_item	GetItem( int iLineNo );
	void    ClearText();///Item to delete all.
	///
	const   li_item_vec&	GetList(){ return m_ITM; }

	///Select an item
	bool	IsSelectable(){ return m_bSelectable; }
	void	SetSelectable( bool b ){ m_bSelectable = b; }
	short   GetSelectedItemID(){ return m_iSelectedItem; }
	const char* GetSelectedItem();
	void    SetSelectedItem( short i){ m_iSelectedItem = i; }
	///Line space (leading)
	void    SetLineSpace( short i ){ m_iLineSpace = i; }
	short   GetLineSpace(){ return m_iLineSpace; }
	///
	void	SetMaxSize( int i ){ m_iMaxSize = i; }
	int		GetMaxSize(){ return m_iMaxSize; }
	///Font Size
	void    SetCharHeight( short i ){ m_iCharHeight = i; }
	short   GetCharHeight(){ return m_iCharHeight; }

	void    SetCharWidth( short i );
	short   GetCharWidth(){ return m_iCharWidth; }

	void	SetFont( int iFont );

	int		GetLineHeight();

protected:
	///A list of selectable options, the message processing method
	bool	ProcessSelectable(UINT uiMsg,WPARAM wParam,LPARAM lParam);

protected:
	li_item_vec			m_ITM;			///List an item
	short				m_nPutLnNum;	///You are taking the first line number
	short				m_nMaxLnCnt;	///Maximum number of lines
	short				m_nMaxPutChar;	///The maximum number of characters in the line
	short				m_iSelectedItem;///Selected items
	bool				m_bSelectable;  ///Select items are available?
	short				m_iLineSpace;   ///Space between lines
	short				m_iCharHeight;  ///Font height
	short				m_iCharWidth;   ///font width
	int					m_iMaxSize;		///Lists the maximum number of items can be added
	ITFont*				m_pFontMgr;
	int					m_iFont;

};
#endif ///_TLISTBOX_
