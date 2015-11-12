#ifndef _TARGET_MANAGER_
#define _TARGET_MANAGER_

#include "../Util/JSingleton.h"


class CTargetManager  : public CJSingleton< CTargetManager >
{
private:
	int					m_iCurrentMouseTargetObject;			/// The mouse is currently pointing to objects moving mouse	

public:
	CTargetManager(void);
	~CTargetManager(void);


	void		SetMouseTargetObject( int iObjectIDX );


	void		Proc();
	void		Draw();
};


#endif //_TARGET_MANAGER_