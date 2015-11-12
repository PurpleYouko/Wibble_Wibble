#ifndef _CITStateMakeSocket_
#define _CITStateMakeSocket_
#include "citstate.h"


/**
* ���ϻ���(����� item �� ) Interface(IT_MGR) State
*
* @Author		������
* @Date			2005/10/29
*
*/


class CITStateMakeSocket : public CITState
{
public:
	CITStateMakeSocket(void);
	virtual ~CITStateMakeSocket(void);

	virtual void Enter();
	virtual void Leave();
	virtual unsigned Process( unsigned uiMsg, WPARAM wParam, LPARAM lParam );
	virtual void Update( POINT ptMouse ){}
};
#endif
