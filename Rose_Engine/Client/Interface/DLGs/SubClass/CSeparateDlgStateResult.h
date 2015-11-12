#ifndef _CSeparateDlgStateResult_
#define _CSeparateDlgStateResult_

#include "CSeparateDlgState.h"

class CGuage;
class CSeparateDlg;

static int	m_viewitemnum;

/**
* 제련인터페이스 창의 State Class : 서버로부터 분해 요청에 대한 응답을 받고 출력하는 상태
*
* @Author		최종진
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
	bool		m_bWaitState;///메세지 박스가 눌릴때까지 기다린다.

	bool		m_bCheckUpdate;		//분해 완료된후 최종분류되서 나온 목록들이 뜨는 다이얼로그창이 활성화되면 기존 분해창은 드로우 / 업데이트를 X 하게.
	bool		m_bStartSeparate;	//분해창에 표시될 아이콘의 상태 체크 true : 분해되어서 나온 재료의 아이콘만 표시.

};
#endif