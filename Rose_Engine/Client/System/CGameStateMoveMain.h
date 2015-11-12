#ifndef _CGAMESTATEMOVEMAIN_
#define _CGAMESTATEMOVEMAIN_

#include "CGameState.h"
#include "GameProc/LoadingImageManager.h"

/**
* 캐릭터 선택후 게임에 들어가기전 카메라 이동시의 State Class
*	- Next State : CGameStatePrepareMain
*
* @Author		최종진
* @Date			2005/9/15
*/
class CGameStateMoveMain : public CGameState
{
	bool m_bswitchImage;

	/// 이벤트별 로딩 이미지 테이블
	std::vector <std::string>	m_mMapLoadingImageTableByEvent;

public:
	CGameStateMoveMain(int iID);
	~CGameStateMoveMain(void);

	virtual int	Update( bool bLostFocus );
	virtual int	Enter( int iPrevStateID );
	virtual int	Leave( int iNextStateID );

	///CGame의 Msg Queue에 넣기 전에 즉시 처리해야할 Msg를 처리하는 Method
	///각 State는 Method를 Overriding하여 처리해야할 Message를 처리한다
	virtual int ProcWndMsgInstant( unsigned uiMsg, WPARAM wParam, LPARAM lParam );

	virtual void ServerDisconnected();

	virtual int ProcMouseInput( UINT uiMsg, WPARAM wParam, LPARAM lParam );
	virtual int ProcKeyboardInput( UINT uiMsg, WPARAM wParam, LPARAM lParam );

	LoadingImageTableByEvent m_EventLoading;

	//조성현 2005 12 - 25 FadeInOut 넣기..
	void CheckInterruptTime();
	DWORD InputInterruptTime;
	bool InputOnOff; 
	int m_iMoveMainState;
};
#endif