#ifndef _CGAMESTATEWARP_
#define _CGAMESTATEWARP_

#include "cgamestate.h"
#include "GameProc/LoadingImageManager.h"
/**
* 게임내 워프시의 State Class
*
* @Author	최종진
* @Date		2005/9/14
*/

class CGameStateWarp : public CGameState
{
	int m_iPrevStateID;
	HANDLE m_hThread;
	bool m_bswitchImage;

	/// 이벤트별 로딩 이미지 테이블
	std::vector <std::string>	m_mMapLoadingImageTableByEvent;

public:
	CGameStateWarp(int iID);
	~CGameStateWarp(void);
	virtual int Update( bool bLostFocus );
	virtual int Enter( int iPrevStateID );
	virtual int Leave( int iNextStateID );
	virtual int ProcMouseInput( UINT uiMsg, WPARAM wParam, LPARAM lParam ) { return 0;}
	virtual int ProcKeyboardInput( UINT uiMsg, WPARAM wParam, LPARAM lParam ) { return 0;}

	LoadingImageTableByEvent m_EventLoading;

protected:
	static unsigned __stdcall ThreadFunc( void* pArguments );
	void Draw();

};
#endif
