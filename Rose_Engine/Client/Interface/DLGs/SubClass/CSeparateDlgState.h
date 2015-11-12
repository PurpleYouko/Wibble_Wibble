#ifndef _CSeparateDlgState_
#define _CSeparateDlgState_

#include "../subclass/CSlot.h"
#include <vector>
/**
* �����������̽� â�� State Base Class : 
*
* @Author		������
* @Date			2005/11/04
*/
class CSeparateDlgState
{
public:
	CSeparateDlgState(void);
	virtual ~CSeparateDlgState(void);
	virtual void Enter() = 0;
	virtual void Leave() = 0;
	virtual void Update( POINT ptMouse ) = 0;
	virtual void Draw() = 0;
	virtual void MoveWindow( POINT ptPosition ){}
	virtual unsigned Process( unsigned uiMsg, WPARAM wParam, LPARAM lParam ) = 0;
	std::vector< CSlot >::iterator iter;

};
#endif