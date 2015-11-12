#include "stdafx.h"
#include ".\country.h"


unsigned long ReadRegistryKeyValue (HKEY hRootKey , const char * Path, const char * KeyName )
{
	HKEY hKey;
	LPBYTE lpbVALUE ;
	DWORD buflen, ret;
	char szZeroEndedPath[ 1024 ];

	sprintf ( szZeroEndedPath, "%s", Path );

    if (!RegOpenKeyEx(HKEY_LOCAL_MACHINE, szZeroEndedPath, 0, KEY_READ, &hKey)) 
	{
        lpbVALUE = 0;
        buflen = sizeof( lpbVALUE );
        ret = RegQueryValueEx(hKey, KeyName, NULL, NULL, (LPBYTE) &lpbVALUE, &buflen);
		
		RegCloseKey ( hKey );
		if (ret == ERROR_SUCCESS) 
			return (unsigned long)lpbVALUE;
    }

	return 0;
}

CCountry _country;

CCountry::CCountry(void)
{
	m_dwCountryCode = 0;
}

CCountry::~CCountry(void)
{
}

bool CCountry::CheckCountry()
{

	unsigned long dwValue = 0;
	//05.01.05 - Kim, Joo - Hyun : Set language code in the registry.
	dwValue = 4;

	memset( m_bIsThatCountry, 0, sizeof( m_bIsThatCountry ) );	
	m_dwCountryCode = 0;

	if( dwValue < COUNTRY_MAX )
	{
		m_dwCountryCode				= dwValue;
		m_bIsThatCountry[ dwValue ] = true;
		return true;
	}

	return false;
}


//*-----------------------------------------------------------------------------------------/
///UseItem Delay�� ���Ӱ� ����� ����üũ (Applied to check the new state of UseItem Delay
//*-----------------------------------------------------------------------------------------/
bool CCountry::IsUseItemDelayNewVersion()
{
	return IsApplyNewVersion();
}

DWORD CCountry::GetCountryCode()
{ 
	return m_dwCountryCode; 
}

bool CCountry::IsPhilipplines()
{ 	
#if defined(_PHILIPPIN) || defined(_TALA)
	return true;	
#endif
	return false;
}

bool CCountry::IsJapan()
{
#if defined(__JAPAN) || defined(__JAPAN_NEW)
	return true;	
#endif
	return false;
}

bool CCountry::IsKorea()
{
#if defined(__KOREA) || defined(__KOREA_NEW) || defined ( __ARUA ) || defined ( __HEBAN )
	return true;	
#endif
	return false;
}

bool CCountry::IsUsa()
{
#if defined(_USA) || defined(_USA_EVO)
	return true;	
#endif
	return false;
}


//---------------------------------------------------------------------------------------
/// @brief Ŭ���̾�Ʈ ����� �ε�� ������Ʈ������ ���ڷ� �Ѿ�� �����ڵ尡 ������ �Ǵ��Ѵ�.
/// @briefThe The client runtime loaded on the registry is equal is equal to the argument passed is the country code)
//---------------------------------------------------------------------------------------
bool CCountry::IsCountry( int country )
{
	/// �迭 ���� üũ (Checking array range)
	assert( country >= 0 && country < COUNTRY_MAX );
	if( country < 0 && country >= COUNTRY_MAX ) return false;

	return m_bIsThatCountry[ country ];
}


//*-----------------------------------------------------------------------------------------/
/// ���ο� �ý����� ������Ѿ� �ϴ°�? (Do you need to apply the new system?)
//*-----------------------------------------------------------------------------------------/
bool IsApplyNewVersion()
{	
	return true;
}
