#ifndef _COUNTRY_
#define _COUNTRY_

#include "Util/JSingleton.h"

//---------------------------------------------------------------------------------------
///
/// The information provided by each country.
///
//---------------------------------------------------------------------------------------
class CCountry : public CJSingleton< CCountry >
{
public:
	CCountry(void);
	~CCountry(void);

	bool CheckCountry();
	bool IsUseItemDelayNewVersion();
	

	DWORD GetCountryCode();


	bool IsPhilipplines();
	bool IsJapan();
	bool IsKorea();
	bool IsCountry( int country );
	bool IsUsa();

	enum COUNTRY_TYPE
	{
		COUNTRY_KOR		= 0,
		COUNTRY_IROSE	= 1,
		COUNTRY_JPN		= 2,
		COUNTRY_CHA		= 3,
		COUNTRY_CHA_TW	= 4,
		COUNTRY_PH		= 5,
		COUNTRY_USA		= 6,
		COUNTRY_EU		= 7,
		COUNTRY_MAX,
	};

private:
	/*
	���� ����� ���Ǵ� ���ڴ� Ŭ���̾�Ʈ�� Ʋ����
	LANGUAGE_KOR = 0,
	LANGUAGE_USA = 1,
	LANGUAGE_JPN = 2,
	LANGUAGE_CHA_TRADITIONAL = 3,
	LANGUAGE_CHA_SIMPLE = 4,
	�������� ȥ��.. ���Ŀ� ������.. ������ �����ڵ尡 �븸�� 4������ �Ǿ��ִ�.. �׷��� ��� ��Ʈ�� ���̺��� 3����.. (translation failed below)
	Confused with the server.. Fix to a future..  The country code of Taiwan is 4.. However, all string table 3 times
	*/

	bool		m_bIsThatCountry[ COUNTRY_MAX ];
	DWORD		m_dwCountryCode;


};

bool IsApplyNewVersion();


#endif //_COUNTRY_