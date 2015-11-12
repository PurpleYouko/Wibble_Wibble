#ifndef _ZZ_TIMER2_H__
#define _ZZ_TIMER2_H__

//#include <d3d9.h>
//#include <d3dx9.h>
//#include <d3dx9math.h>

class zz_timer2
{
public:
	zz_timer2(void);
	~zz_timer2(void);

	float prevSecs, currSecs, deltaSecs;
    
	void UpdateDeltaTime();



};

#endif