#ifndef _CSeparateDlgStateWait_
#define _CSeparateDlgStateWait_

#include "cseparatedlgstate.h"


/**
* �����������̽� â�� State Class : ����ڰ� �������� ������ ��û�ϰ� ������ ����ϴ� ����
*	
* @Warning		�̻����� ��� ��� ������ �Է��� �����Ѵ�.
* @Author		������
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