#ifndef _CSeparateDlgStateNormal_
#define _CSeparateDlgStateNormal_

#include "cseparatedlgstate.h"

class CSeparateDlg;


/**
* �����������̽� â�� State Class : ����ڰ� �������� �غ��ϴ� ����
*
* @Author		������
* @Date			2005/11/04
*/
class CSeparateDlgStateNormal : public CSeparateDlgState
{
public:
	CSeparateDlgStateNormal( CSeparateDlg* pParent );
	virtual ~CSeparateDlgStateNormal(void);

	virtual void Enter();
	virtual void Leave();
	virtual void Update( POINT ptMouse );
	virtual void Draw();
	virtual unsigned Process( unsigned uiMsg, WPARAM wParam, LPARAM lParam );

private:
	enum{
		IID_BTN_START	= 10,
		IID_BTN_CLOSE	= 11
	};

	CSeparateDlg*	m_pParent;

};

#endif