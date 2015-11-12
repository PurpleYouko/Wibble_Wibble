#ifndef _CSeparateDlgStateResult_
#define _CSeparateDlgStateResult_

#include "CSeparateDlgState.h"

class CGuage;
class CSeparateDlg;

static int	m_viewitemnum;

/**
* �����������̽� â�� State Class : �����κ��� ���� ��û�� ���� ������ �ް� ����ϴ� ����
*
* @Author		������
* @Date			2005/9/15
*/
const int		iMaxCountMaterial = 4;

class CSeparateDlgStateResult : public CSeparateDlgState
{
public:
	static CSeparateDlgStateResult* GetInstance();

	CSeparateDlgStateResult( CSeparateDlg* pParent );

	static int GetViewNum();
	virtual ~CSeparateDlgStateResult(void);

	virtual void Enter();
	virtual void Leave();
	virtual void Update( POINT ptMouse );
	virtual void Draw();
	virtual unsigned Process( unsigned uiMsg, WPARAM wParam, LPARAM lParam ){ return 0; }
	virtual void MoveWindow( POINT ptMouse );
	virtual bool IsModal();

	bool GetStartSeparateState();

	bool GetCheckUpdate();
private:
	static CSeparateDlgStateResult* s_Instance;
	CSeparateDlg*	m_pParent;
	CGuage*		m_pResultGuage;
	CGuage*		pGuage ;
	std::vector<CGuage*>	m_listGuage;
	int			m_iRedGuageImageID;
	int			m_iGreenGuageImageID;
	int			m_dwPrevTime;

	bool		m_bWaitUserinput;
	bool		m_bWaitState;///�޼��� �ڽ��� ���������� ��ٸ���.

	bool		m_bCheckUpdate;		//���� �Ϸ���� �����з��Ǽ� ���� ��ϵ��� �ߴ� ���̾�α�â�� Ȱ��ȭ�Ǹ� ���� ����â�� ��ο� / ������Ʈ�� X �ϰ�.
	bool		m_bStartSeparate;	//����â�� ǥ�õ� �������� ���� üũ true : ���صǾ ���� ����� �����ܸ� ǥ��.

};
#endif