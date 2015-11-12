#include "StdAfx.h"
#include ".\timage.h"
#include "ITDraw.h"
#include "ITFont.h"
#include "TControlMgr.h"
#include <mmsystem.h>
CTImage::CTImage(void)
{
	m_pDraw			= NULL;
	m_iGraphicID	= 0;
	m_bBlink		= true;
	m_bBlinkEnable  = false;

	m_dwBlinkSwapTime = 1000;
	m_dwBlinkPrevSwapTime = 0;
	m_dwBlinkCurrentGID = 0;
	m_dwBlinkCurrentMID = 0;
	
	m_dwTextColor	= D3DCOLOR_ARGB(255, 255, 255, 255);	
	m_iFont			= 0;	
	m_dwAlgin		= DT_CENTER|DT_VCENTER;
	
}

CTImage::~CTImage(void)
{	
}

CTImage * CTImage::Clone()
{
	CTImage * pImage = new CTImage();

	if( !pImage->Create( m_sPosition.x, m_sPosition.y, m_iWidth, m_iHeight, m_iGraphicID, m_iModuleID ) )
	{
		return NULL;
	}	
		
	pImage->m_iBlinkGraphicID		= m_iBlinkGraphicID;
	pImage->m_iBlinkModuleID		= m_iBlinkModuleID;

	pImage->m_bBlink				= m_bBlink;					
	pImage->m_bBlinkEnable			= m_bBlinkEnable;

	pImage->m_dwBlinkSwapTime		= m_dwBlinkSwapTime;
	pImage->m_dwBlinkPrevSwapTime	= m_dwBlinkPrevSwapTime;
	pImage->m_dwBlinkCurrentGID		= m_dwBlinkCurrentGID;
	pImage->m_dwBlinkCurrentMID		= m_dwBlinkCurrentMID;

	pImage->m_pFontImpl				= m_pFontImpl;
	pImage->m_iFont					= m_iFont;
	pImage->m_strText				= m_strText;
	pImage->m_dwAlgin				= m_dwAlgin;
	pImage->m_dwTextColor			= m_dwTextColor;

	return pImage;

}

bool CTImage::Create(int iScrX, int iScrY, int iWidth, int iHeight, int iGraphicID,int iModuleID )
{
	Init( CTRL_IMAGE, iScrX, iScrY, iWidth, iHeight );
	m_iGraphicID	= iGraphicID;
	m_iModuleID		= iModuleID;
	m_pDraw			= CTControlMgr::GetInstance()->GetDrawMgr();
	m_pFontImpl		= CTControlMgr::GetInstance()->GetFontMgr();
	return true;
}

unsigned int CTImage::Process( UINT uiMsg,WPARAM wParam,LPARAM lParam )
{
	return 0;
}

void CTImage::Draw()
{
	if( !IsVision() ) return;

	if( m_pDraw && m_iGraphicID >= 0)
	{
		int module_id = m_iModuleID;
		int graphic_id = m_iGraphicID;

		if( m_bBlinkEnable && m_bBlink )
		{
			module_id	= m_dwBlinkCurrentMID;
			graphic_id	= m_dwBlinkCurrentGID;
		}

		if( GetSizeFit() )
		{
			m_pDraw->DrawFit(	(int)m_sPosition.x, (int)m_sPosition.y,
								module_id, graphic_id,
								GetWidth(), GetHeight(),
								D3DCOLOR_ARGB(255, 255, 255, 255) );
		}
		else if( m_btAlphaValue == 0 && m_fScaleWidth == 0 && m_fScaleHeight == 0 )
		{
			m_pDraw->Draw( m_sPosition.x, m_sPosition.y, module_id, graphic_id );
		}
		else if( m_btAlphaValue && m_fScaleWidth )
		{
			m_pDraw->Draw( m_sPosition.x, m_sPosition.y, module_id, graphic_id , 
				m_fScaleWidth, 1,
				D3DCOLOR_RGBA( 255, 255, 255, m_btAlphaValue ));
		}
		else if( m_btAlphaValue && m_fScaleHeight)
		{
			m_pDraw->Draw( m_sPosition.x, m_sPosition.y, module_id, graphic_id, 
				1, m_fScaleHeight,
				D3DCOLOR_RGBA( 255, 255, 255, m_btAlphaValue ));
		}
		else if( m_btAlphaValue )
		{
			m_pDraw->Draw( m_sPosition.x, m_sPosition.y, module_id, graphic_id, 
				D3DCOLOR_RGBA( 255, 255, 255, m_btAlphaValue ));
		}

		else if( m_fScaleWidth && m_fScaleHeight )
		{
			m_pDraw->Draw( m_sPosition.x, m_sPosition.y, module_id, graphic_id, 
				m_fScaleWidth, m_fScaleHeight,
				D3DCOLOR_RGBA( 255, 255, 255, 255 ));
		}
		else if( m_fScaleWidth )
		{
			m_pDraw->Draw( m_sPosition.x, m_sPosition.y, module_id, graphic_id, 
				m_fScaleWidth,1,
				D3DCOLOR_RGBA( 255, 255, 255, 255 ));
		}
		else if( m_fScaleHeight )
		{
			m_pDraw->Draw( m_sPosition.x, m_sPosition.y, module_id, graphic_id, 
				1,m_fScaleHeight,
				D3DCOLOR_RGBA( 255, 255, 255, 255 ));
		}
		else
		{
			m_pDraw->Draw( m_sPosition.x, m_sPosition.y, module_id, graphic_id );
		}

		
	}

	if( m_pFontImpl )
	{
		if( !m_strText.empty())
		{
			RECT rcDraw;			

			SetRect(
				&rcDraw,
				0,
				0,
				m_iWidth,
				m_iHeight+1 
				);
			m_pFontImpl->SetTransformSprite( m_sPosition.x, m_sPosition.y );
			m_pFontImpl->Draw( m_iFont, true, &rcDraw, m_dwTextColor, m_dwAlgin, m_strText.c_str());
		}
	}

}

void CTImage::Draw( POINT ptDraw )
{
	if( !IsVision() ) return;

	if( m_pDraw && m_iGraphicID >= 0 )
	{
		int module_id = m_iModuleID;
		int graphic_id = m_iGraphicID;

		if( m_bBlinkEnable && m_bBlink )
		{
			module_id	= m_dwBlinkCurrentMID;
			graphic_id	= m_dwBlinkCurrentGID;
		}


		if( m_btAlphaValue == 0 && m_fScaleWidth == 0 && m_fScaleHeight == 0 )
		{
			m_pDraw->Draw( ptDraw.x, ptDraw.y, module_id, graphic_id );
		}
		else if( m_btAlphaValue && m_fScaleWidth )
		{
			m_pDraw->Draw( ptDraw.x, ptDraw.y, module_id, graphic_id, 
				m_fScaleWidth, 1,
				D3DCOLOR_RGBA( 255, 255, 255, m_btAlphaValue ));
		}
		else if( m_btAlphaValue && m_fScaleHeight)
		{
			m_pDraw->Draw( ptDraw.x, ptDraw.y, module_id, graphic_id, 
				1, m_fScaleHeight,
				D3DCOLOR_RGBA( 255, 255, 255, m_btAlphaValue ));
		}
		else if( m_btAlphaValue )
		{
			m_pDraw->Draw( ptDraw.x, ptDraw.y, module_id, graphic_id, 
				D3DCOLOR_RGBA( 255, 255, 255, 255 ));
		}

		else if( m_fScaleWidth && m_fScaleHeight )
		{
			m_pDraw->Draw( ptDraw.x, ptDraw.y, module_id, graphic_id, 
				m_fScaleWidth, m_fScaleHeight,
				D3DCOLOR_RGBA( 255, 255, 255, 255 ));
		}
		else if( m_fScaleWidth )
		{
			m_pDraw->Draw( ptDraw.x, ptDraw.y, module_id, graphic_id, 
				m_fScaleWidth,1,
				D3DCOLOR_RGBA( 255, 255, 255, 255 ));
		}
		else if( m_fScaleHeight )
		{
			m_pDraw->Draw( ptDraw.x, ptDraw.y, module_id, graphic_id, 
				1,m_fScaleHeight,
				D3DCOLOR_RGBA( 255, 255, 255, 255 ));
		}
		else
		{
			m_pDraw->Draw( ptDraw.x, ptDraw.y, module_id, graphic_id );
		}
	}

}

void CTImage::Update( POINT ptMouse)
{
	if( m_bBlinkEnable && m_bBlink && m_iGraphicID >= 0 )
	{
		DWORD dwCurrentTime = timeGetTime();

		if( dwCurrentTime - m_dwBlinkPrevSwapTime > m_dwBlinkSwapTime )
		{
			m_dwBlinkPrevSwapTime = dwCurrentTime;

			if( m_dwBlinkCurrentGID == m_iGraphicID )
				m_dwBlinkCurrentGID = m_iBlinkGraphicID;
			else
				m_dwBlinkCurrentGID = m_iGraphicID;

			if( m_dwBlinkCurrentMID == m_iModuleID )
				m_dwBlinkCurrentMID = m_iBlinkModuleID;
			else
				m_dwBlinkCurrentMID = m_iModuleID;
		}
	}
}

void CTImage::SetImage( int iGraphicID, int iModuleID )
{
	m_iModuleID  = iModuleID;
	m_iGraphicID = iGraphicID;

}

void CTImage::SetBlinkImage( int iGraphicID, int iModuleID )
{
	m_iBlinkGraphicID	= iGraphicID;
	m_iBlinkModuleID	= iModuleID;
	m_bBlinkEnable		= true;
}

void CTImage::SetBlink()
{
	m_bBlink = true;
}

void CTImage::SetUnblink()
{
	m_bBlink = false;
	m_dwBlinkPrevSwapTime = 0;
	m_dwBlinkCurrentGID = 0;
	m_dwBlinkCurrentMID = 0;
}

void CTImage::SetBlinkSwapTime( DWORD swap_time )
{
	m_dwBlinkSwapTime = swap_time;
}

void CTImage::SetFont(int iFont)
{
	m_iFont = iFont;
}
void CTImage::SetText(const char * szText)
{
	m_strText = szText;
}
void CTImage::SetAlign(DWORD dwAlign)
{
	m_dwAlgin = dwAlign;
}
void CTImage::SetTextColor(DWORD dwColor)
{
	m_dwTextColor = dwColor;
}
