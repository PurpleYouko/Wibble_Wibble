#include "StdAfx.h"
#include ".\zz_timer2.h"

zz_timer2::zz_timer2(void)
{
}

zz_timer2::~zz_timer2(void)
{
}

void zz_timer2::UpdateDeltaTime()
{
	static bool first_time = true;

	if(first_time)
	{
		prevSecs = (((float)GetTickCount()) / 1000.0F);
        first_time = false;
	}
	
	currSecs = (((float)GetTickCount()) / 1000.0F);
	deltaSecs = currSecs - prevSecs;
	prevSecs = currSecs;

    if(deltaSecs>2.0f)
    deltaSecs = 2.0f;

}