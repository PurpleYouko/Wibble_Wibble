#ifndef __HASHER_H_INCLUDED__
#define __HASHER_H_INCLUDED__
#include <Windows.h>
#include <tchar.h>


extern int sha1HashByte( void *pData, unsigned int dataLength, void *pHashOut, unsigned int *pHashLength );
extern int sha1HashFile( LPTSTR szFileName, void *pHashOut, unsigned int *pHashLength );

#endif //__HASHER_H_INCLUDED__