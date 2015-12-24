#ifndef _DELAYED_EXP_
#define _DELAYED_EXP_

#include <list>
#include "../Util/JSingleton.h"


struct EXP_DATA
{
	int iOwnerIndex;
	//int iAddExp;			//PY: should be a long otherwise we are passing a long into an int when we store exp values
	long iAddExp;
	//int iExp;				//Same for this one
	long iExp;
	//int iGetTime;			/// Received from the server time. This also should be long since the function returns a DWORD
	long iGetTime;

	int iStamina;			/// Stamina is the current stamina come away from the server..
};


//------------------------------------------------------------------------------------------
///
/// class CDelayedExp
/// Synchronizing display time of getting exp with real mob dead time
///
//------------------------------------------------------------------------------------------

class CDelayedExp : public CJSingleton< CDelayedExp >
{
private:
	std::list< EXP_DATA >		m_ExpData;


public:
	CDelayedExp(void);
	~CDelayedExp(void);

	void						PushEXPData( int iOwnerIndex, long iAddExp, long iEXP, int iStamina );
	void						GetExp( int iOwnerIndex );

	void						Proc();
};

#endif //_DELAYED_EXP_
