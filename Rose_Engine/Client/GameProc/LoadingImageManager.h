#ifndef _LOADING_IMAGE_MANAGER_
#define _LOADING_IMAGE_MANAGER_

#include <map>
#include <vector>

/**
* 각 존별 혹은 행성별.. 상황에 따른 로딩이미지 교체 관리..
*
* @Author		이동현
* @Date			2005/9/5
*/ 

#define MAX_EVENTLOADING 128

struct EventLoading{
	int NpcNO;
	int FirstNO;
	int SecondNO;
	std::string sFileName;
};

struct LoadingImageTableByEvent
{
	EventLoading EventTable[MAX_EVENTLOADING];
	int			 EventLoadingCount;
};

#endif //_LOADING_IMAGE_MANAGER_