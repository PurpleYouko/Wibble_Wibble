#ifndef _CJContainer_
#define _CJContainer_

#include "tobject.h"
#include <vector>

using namespace std;

class CWinCtrl;

//------------------------------------------------------------------------------------------------------------------------
///	CWinCtrl을 child로 가지는 일반적인 컨테이너 클래스
///
/// @Author				최종진
///
/// @Date				2005/08/23
//------------------------------------------------------------------------------------------------------------------------

typedef vector< CWinCtrl* >::reverse_iterator	riter_CWinCtrl;
typedef vector< CWinCtrl* >::iterator			iter_CWinCtrl;

class TGAMECTRL_API CJContainer :	public CTObject
{
public:
	CJContainer(void);
	virtual ~CJContainer(void);

	void Draw();
	void Update( POINT ptMouse );
	unsigned Process( unsigned uiMsg, WPARAM wParam, LPARAM lParam );
	void MoveWindow( POINT pt );
	void Show();
	void Hide();

	CWinCtrl*	Find( unsigned index );
	CWinCtrl*	Find( const char * szName );

	void		Add( CWinCtrl* pCtrl );
	void		Remove( unsigned index );
	void		RemoveAll();
	void		SetChildEnable( unsigned iID, bool bEnable );

	vector<CWinCtrl*> & GetChild();
	CWinCtrl * GetChild(int iIndex);

protected:
	void		MoveCtrl2ListEnd( int iID );
protected:

	vector< CWinCtrl* >	m_Children;	

};


#endif