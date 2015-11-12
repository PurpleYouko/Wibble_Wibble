#ifndef _BGM_MANAGER_
#define _BGM_MANAGER_
#include "../util/JSingleton.h"


//---------------------------------------------------------------------------------------
///
/// class CBGManager
/// Background music player management
///
//---------------------------------------------------------------------------------------
class CBGMManager : public CJSingleton< CBGMManager >
{
private:
	int				m_iCurrentVolumeIdx;
	int				m_iStartVolumeIdx;
	bool			m_bToFadeOut;

	DWORD			m_dwTransitionPrevTime;
	DWORD			m_dwElapsedTime;


	bool			m_bEnableTransition;
	bool			m_bToNight;	
	
	int				m_iZoneNO;


public:
	CBGMManager(void);
	~CBGMManager(void);

	/// Background music transition due to day and night
	void			ChangeBGMMusicWithDayAndNight( int iZoneNO, bool bToNight );

	void			EndTransition();
	void			Proc();
};

#endif //_BGM_MANAGER_