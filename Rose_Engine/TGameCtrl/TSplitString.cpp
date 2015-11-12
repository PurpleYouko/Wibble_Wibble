#include "StdAfx.h"
#include ".\tsplitstring.h"
#include "ITFont.h"
#include "TControlMgr.h"


CTSplitString::CTSplitString(void)
{
	m_strNull = "";
	m_iMaxLine = 256;
}

CTSplitString::~CTSplitString(void)
{
}

CTSplitString::CTSplitString( int iFont, const char* pszStr, int iDisplayWidth ,unsigned uiCodePage )
{
	Split( iFont, pszStr, iDisplayWidth, uiCodePage );
}


int	CTSplitString::GetLineCount()
{
	return m_StringList.size();
}
const char* CTSplitString::GetString( int iIndex )
{
	if( iIndex < 0 ||  iIndex >= (int)m_StringList.size() )
		return m_strNull.c_str();

	return m_StringList[ iIndex ].c_str();	
}



bool CTSplitString::Split( int iFont, const char* pszStr, int iDisplayWidth ,unsigned uiCodePage )
{
	if( pszStr == NULL )
		return false;	

	m_StringList.clear();

	std::string					strTemp;

	const char * pCurrPos = pszStr;

	const char* pNextPos	= NULL;
	const char* pPrevPos	= NULL;
	const char* pStartPos = pCurrPos;
	const char* pCurrPosBuf = NULL;
	SIZE sizeString;


	ITFont* pFont = CTControlMgr::GetInstance()->GetFontMgr();

	pNextPos = CharNextExA( (WORD)uiCodePage, pCurrPos, 0 );
	while( pCurrPos != pNextPos )
	{
//홍근 : 오전 10:09 2006-11-07 : 리턴이 들어 있음 다음 줄로 내린다.

		if(*pCurrPos==VK_RETURN )
		{			
			strTemp.push_back('\n');
			m_StringList.push_back( strTemp );
			strTemp.clear();				
			pStartPos = ++pCurrPos;
			pNextPos = CharNextExA( uiCodePage, pCurrPos, 0 );
			continue;
		}

		for( ; pCurrPos < pNextPos; ++pCurrPos )
		{				
			strTemp.push_back( *pCurrPos );
		}

		sizeString = pFont->GetFontTextExtent( iFont, strTemp.c_str() );

		if( sizeString.cx > iDisplayWidth )
		{
			//hong geun.
			pCurrPosBuf = pCurrPos;			

			if( uiCodePage == 1252 )
			{
				while(true)
				{
					pPrevPos = CharPrevExA( uiCodePage, pStartPos, pCurrPosBuf, 0 );				
					pCurrPosBuf = pPrevPos;				
					
					if(	pPrevPos[0] == ' ' )
					{
						break;
					}

					if( strTemp.size() < (pCurrPos-pPrevPos) * 3 )
					{
						pPrevPos = CharPrevExA( uiCodePage, pStartPos, pCurrPos, 0 );
						break;
					}								
				}				
			}
			else
			{
				pPrevPos = CharPrevExA( uiCodePage, pStartPos, pCurrPos, 0 );				
			}			

			for( ;pCurrPos > pPrevPos; --pCurrPos )
			{
				strTemp = strTemp.erase( strTemp.size() - 1, 1 );
			}
			
			
			m_StringList.push_back( strTemp );
			

			strTemp.clear();
			pStartPos = pCurrPos;
		}

		pNextPos = CharNextExA( uiCodePage, pCurrPos, 0 );
	}

	if( !strTemp.empty() )
		m_StringList.push_back( strTemp );
	

	return true;
}

void CTSplitString::Clear()
{
	m_StringList.clear();
}

SIZE & CTSplitString::GetSizeText( int iFont, const char * szStr )
{		
	ITFont* pFont = CTControlMgr::GetInstance()->GetFontMgr();
	if( pFont )
	{
		m_szStrSize = pFont->GetFontTextExtent( iFont, szStr );		
	}	
	return m_szStrSize;
}

SIZE CTSplitString::GetColmnRow(int iPos)
{
	SIZE sz = {0 ,0};
	int iLen = 0;
	int iCnt = 0;
	for(int i=0; i<m_StringList.size(); i++)
	{
		iLen = m_StringList[i].length();
		iCnt += iLen;

		sz.cx = iLen-(iCnt-iPos);
		sz.cy = i;

		if(iCnt > iPos)
		{			
			break;
		}
	}
	return sz;
}



