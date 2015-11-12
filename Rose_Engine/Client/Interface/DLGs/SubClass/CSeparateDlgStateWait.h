#ifndef _CSeparateDlgStateWait_
#define _CSeparateDlgStateWait_

#include "cseparatedlgstate.h"


/**
* 분해인터페이스 창의 State Class : 사용자가 서버에게 분해을 요청하고 응답을 대기하는 상태
*	
* @Warning		이상태의 경우 모든 유저의 입력은 무시한다.
* @Author		김주현
* @Date			2005/11/04
*/
class CSeparateDlgStateWait : public CSeparateDlgState
{
public:
	CSeparateDlgStateWait(void);
	virtual ~CSeparateDlgStateWait(void);

	virtual void Enter(){}
	virtual void Leave(){}
	virtual void Update( POINT ptMouse ){}
	virtual void Draw(){}
	virtual unsigned Process( unsigned uiMsg, WPARAM wParam, LPARAM lParam ){ return 1; }

};
#endif