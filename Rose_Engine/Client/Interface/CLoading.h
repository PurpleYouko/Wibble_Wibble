#ifndef _LOADING_
#define _LOADING_

#include <deque>
#include "../Util/JSingleton.h"
#include "GameProc/LoadingImageManager.h"

/**
* 존간 워프시 로딩이미지를 보여주는 Class 
*	- CLoadingImageManager가 stb를 참조하여 보여주어야할 이미지를 바꾸어 준다.
*
* @Author	최종진
* @Date		2005/9/5
*/
class CLoading : public CJSingleton< CLoading >
{
	
	HNODE						m_hTexture;

	/// m_LoadingImageManager 가 초기화 되었는가?
	bool						m_bInitLoadingImageManager;

	bool						m_CustomSet;
	unsigned long				m_LastDayTime;
	std::deque<int>				m_Playlist;


	std::multimap< int, std::string >	m_LoadingImageTableByCustom;
	std::map<int, int>			m_CustomList1;
	std::map<int, int>			m_CustomList2;
	std::map<int, int>			m_CustomList3;

	/// 이벤트용 로딩 이미지를 출력해야하는가.. 존,행성,이벤트용번호가 0번인 경우.
	bool	m_bDisplayEventLoadingImage;

	/// 이벤트용 로딩이미지의 카운트.
	int		m_iCountNPCNO;

	/// list_loading.stb에 npc번호가 있는 경우.
	bool	m_UseEventLoading;

	std::vector< std::string >			m_LoadingImageTableDefault;
	/// 행성별 로딩 이미지 테이블
	std::vector< std::string >			m_LoadingImageTableByEvent;

	/// 행성별 로딩 이미지 테이블
	std::multimap< int, std::string >	m_LoadingImageTableByPlanet;    
	/// 존별 로딩 이미지 테이블
	std::multimap< int, std::string >	m_LoadingImageTableByZone;

	/// 이벤트별 로딩이미지 테이블  06.05. 09 - 김주현
	LoadingImageTableByEvent			m_EventTable;

	bool CheckUseEventLoadingImage();
	HNODE	GetLoadingImageFromTable( std::multimap< int, std::string >& imageTable, int iKey );


public:
	CLoading();
	~CLoading();

	bool			InitLoadingImageManager();

	bool			LoadTexture( int iZoneNo = 9999, int iPlanetNO = 0 );

	/// NPC변수를 참고한 이벤트 로딩용
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