 #include "stdafx.h"
#include ".\CZLBAppendText.h"

#include "../../it_mgr.h"
#include "../../TypeResource.h"
#include "../../IO_ImageRes.h"
#include "../../CTDrawImpl.h"

#include "ZListBox.h"

CZLBAppendText::CZLBAppendText( const char* pszText, int iWidth, DWORD dwColor)
{
	SetWidth( iWidth );

	if( pszText )
	{
		m_jString.SetSplitType( CJStringParser::SPLIT_WORD );
		m_jString.SetDefaultColor( dwColor );

		m_jString.SetString( pszText, iWidth - 10 );

		if( m_jString.GetStringCount() )
			SetHeight( m_jString.GetString(0)->GetStringHeight() * m_jString.GetStringCount() + 3 );
	}
}

CZLBAppendText::CZLBAppendText( CJString ptJString, int iWidth, DWORD dwColor)
{
	SetWidth( iWidth );

	if(!ptJString.IsEmpty())
	{
		m_jString.Push_Back(ptJString);
		m_jString.SetDefaultColor( dwColor );

		if( m_jString.GetStringCount() )
			SetHeight( m_jString.GetString(0)->GetStringHeight() * m_jString.GetStringCount() + 3 );
	}
}

CZLBAppendText::~CZLBAppendText(void)
{

}

void CZLBAppendText::Draw()
{

	if( int iLineCount = m_jString.GetStringCount() )
	{
		POINT ptDraw = m_sPosition;


		for( int i = 0; i < iLineCount; ++i )
		{
			if( m_pJString = m_jString.GetString( i ) )
			{
				m_pJString->Draw( ptDraw.x , ptDraw.y ,  false, 0, true );
				ptDraw.y += m_pJString->GetStringHeight();


			}
		}
	}	
}
