#ifndef _LOADING_IMAGE_MANAGER_
#define _LOADING_IMAGE_MANAGER_

#include <map>
#include <vector>

/**
* �� ���� Ȥ�� �༺��.. ��Ȳ�� ���� �ε��̹��� ��ü ����..
*
* @Author		�̵���
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