#ifndef _CGAMESTATEPREPAREMAIN_
#define _CGAMESTATEPREPAREMAIN_

#include "cgamestate.h"
#include "GameProc/LoadingImageManager.h"
/**
* 캐릭터 선택후 CGameStateMain전 처리를 하는 State Class
*	- CGameStateWarp와 비슷하지만 다른 부분이 있어 분리되었다.
*
* @Author		최종진
* @Date			2005/9/15
*/
class CGameStatePrepareMain : public CGameState
{
	int m_iPrevStateID;
	HANDLE m_hThread;

	bool m_bswitchImage;

	/// 이벤트별 로딩 이미지 테이블
	std::vector <std::string>	m_mMapLoadingImageTableByEvent;

public:
	CGameStatePrepareMain(int iID);
	~CGameStatePrepareMain(void);
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
