#include "StdAfx.h"
#include "Hasher.h"
#include <WinCrypt.h>
int sha1HashByte( void *pData, unsigned int dataLength, void *pHashOut, unsigned int *pHashLength )
{
	HCRYPTPROV hProv = 0;
	HCRYPTHASH hHash = 0;

	DWORD dwCryptLength = sizeof( DWORD );
	DWORD dwHashLength = 0;

	if ( 0 == CryptAcquireContext( &hProv, NULL, NULL, PROV_RSA_FULL, 0 ) )
	{
		if ( 0 == CryptAcquireContext( &hProv, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT ) )
		{
			if ( 0 == CryptAcquireContext( &hProv, NULL, NULL, PROV_RSA_FULL, CRYPT_NEWKEYSET ) )
			{
				return 0;
			}
		}
	}

	if ( 0 == CryptCreateHash( hProv, CALG_SHA1, 0, 0, &hHash ) )
	{
		CryptReleaseContext( hProv, 0 );
		return 0;
	}
	
	if ( 0 == CryptHashData( hHash, reinterpret_cast<const LPBYTE>( pData ), dataLength, 0 ) )
	{
		CryptDestroyHash( hHash );
		CryptReleaseContext( hProv, 0 );
		return 0;
	}
	else
	{
		if ( 0 == CryptGetHashParam( hHash, HP_HASHSIZE, ( PBYTE ) &dwHashLength, &dwCryptLength, 0 ) )
		{
			*pHashLength = 0;
			CryptDestroyHash( hHash );
			CryptReleaseContext( hProv, 0 );
			return 0;
		}
		else
		{
			*pHashLength = dwHashLength;
		}

		if (0 == CryptGetHashParam( hHash, HP_HASHVAL,  reinterpret_cast<const LPBYTE>( pHashOut ), &dwHashLength, 0 ) )
		{
			pHashOut = 0;
			CryptDestroyHash( hHash );
			CryptReleaseContext( hProv, 0 );
			return 0;
		}

		CryptDestroyHash( hHash );
		CryptReleaseContext( hProv, 0 );
		return 1;

	}

	
}

int sha1HashFile( LPTSTR szFileName, void *pHashOut, unsigned int *pHashLength )
{
	HANDLE hFile = 0;

	DWORD fileSizeLow = 0;
	DWORD fileSizeHigh = 0;

	LPBYTE pData = NULL;
	BOOL bSuccess = FALSE;
	DWORD dwRead = 0;
	DWORD errCode = 0;

	
	hFile = CreateFile( szFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
	errCode = GetLastError();

	if ( INVALID_HANDLE_VALUE == hFile )
	{
		return 0;
	}
	
	fileSizeLow = GetFileSize( hFile, &fileSizeHigh );

	if ( fileSizeHigh > 0 )
	{
		return 0;
	}

	pData = reinterpret_cast<LPBYTE>( malloc( fileSizeLow ) );
	if ( !ReadFile( hFile, pData, fileSizeLow, &dwRead, NULL ) )
	{
		CloseHandle( hFile );
		return 0;
	}
	
	bSuccess = sha1HashByte( pData, fileSizeLow, pHashOut, pHashLength );

	CloseHandle( hFile );

	return bSuccess;
}