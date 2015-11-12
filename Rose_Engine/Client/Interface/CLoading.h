#ifndef _LOADING_
#define _LOADING_

#include <deque>
#include "../Util/JSingleton.h"
#include "GameProc/LoadingImageManager.h"

/**
* ���� ������ �ε��̹����� �����ִ� Class 
*	- CLoadingImageManager�� stb�� �����Ͽ� �����־���� �̹����� �ٲپ� �ش�.
*
* @Author	������
* @Date		2005/9/5
*/
class CLoading : public CJSingleton< CLoading >
{
	
	HNODE						m_hTexture;

	/// m_LoadingImageManager �� �ʱ�ȭ �Ǿ��°�?
	bool						m_bInitLoadingImageManager;

	bool						m_CustomSet;
	unsigned long				m_LastDayTime;
	std::deque<int>				m_Playlist;


	std::multimap< int, std::string >	m_LoadingImageTableByCustom;
	std::map<int, int>			m_CustomList1;
	std::map<int, int>			m_CustomList2;
	std::map<int, int>			m_CustomList3;

	/// �̺�Ʈ�� �ε� �̹����� ����ؾ��ϴ°�.. ��,�༺,�̺�Ʈ���ȣ�� 0���� ���.
	bool	m_bDisplayEventLoadingImage;

	/// �̺�Ʈ�� �ε��̹����� ī��Ʈ.
	int		m_iCountNPCNO;

	/// list_loading.stb�� npc��ȣ�� �ִ� ���.
	bool	m_UseEventLoading;

	std::vector< std::string >			m_LoadingImageTableDefault;
	/// �༺�� �ε� �̹��� ���̺�
	std::vector< std::string >			m_LoadingImageTableByEvent;

	/// �༺�� �ε� �̹��� ���̺�
	std::multimap< int, std::string >	m_LoadingImageTableByPlanet;    
	/// ���� �ε� �̹��� ���̺�
	std::multimap< int, std::string >	m_LoadingImageTableByZone;

	/// �̺�Ʈ�� �ε��̹��� ���̺�  06.05. 09 - ������
	LoadingImageTableByEvent			m_EventTable;

	bool CheckUseEventLoadingImage();
	HNODE	GetLoadingImageFromTable( std::multimap< int, std::string >& imageTable, int iKey );


public:
	CLoading();
	~CLoading();

	bool			InitLoadingImageManager();

	bool			LoadTexture( int iZoneNo = 9999, int iPlanetNO = 0 );

	/// NPC������ ������ �̺�Ʈ �ε���
	bool			LoadTexture( std::vector < std::string >	m_mMapEventLoadingTable );

	void			Draw();
	void			UnloadTexture();

	HNODE	GetLoadingImageCustom( int tp, int imgID );

	bool	LoadImageTable( char* strSTBName );
	HNODE	GetLoadingImage( int iZoneNO, int iPlanetNO );
	void	ReleaseTexture( HNODE hTexture );

	LoadingImageTableByEvent GetEventLoadingTable() {return m_EventTable;}
};

#define g_Loading CLoading::GetSingleton()

#endif //_LOADING_