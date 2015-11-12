#include "StdAfx.h"
#include ".\tguage.h"
#include "TControlMgr.h"

CTGuage::~CTGuage(void)
{
	m_iModuleID				 = 0;
	m_iBGImageID			 = 0;
	m_iGuageImageID			 = 0;
	m_bSizeFit				= false;
	m_pDrawImpl = NULL;	
}

CTGuage::CTGuage()
{
	m_iModuleID				 = 0;
	m_iBGImageID			 = 0;
	m_iGuageImageID			 = 0;
	m_bSizeFit				= false;
	m_pDrawImpl = NULL;
	m_iValueUnitSize		= 1000;
}

bool CTGuage::Create( int iScrX, int iScrY, int iWidth, int iHeight, int iGraphicID,int iBGraphicID ,int iModuleID )
{
	Init( CTRL_GUAGE, iScrX, iScrY, iWidth, iHeight );
	m_pDrawImpl = CTControlMgr::GetInstance()->GetDrawMgr();
	m_iValue				 = 0;
	m_iWidth				 = iWidth;
	m_iHeight				 = 15;	
	m_iModuleID				 = iModuleID;
	m_iBGImageID			 = iBGraphicID;
	m_iGuageImageID			 = iGraphicID;
	return true;
}

void	CTGuage::Draw()
{
	if( !IsVision() ) return;

	if( m_iBGImageID )
		m_pDrawImpl->Draw( (int)m_sPosition.x, (int)m_sPosition.y, m_iModuleID , m_iBGImageID);

	int iDrawGuageWidth = m_iWidth * m_iValue / m_iValueUnitSize;

	if( iDrawGuageWidth > m_iWidth )
		iDrawGuageWidth = m_iWidth;


	if( m_bSizeFit )
	{
		m_pDrawImpl->DrawFitW(	(int)m_sPosition.x, (int)m_sPosition.y,
								m_iModuleID, m_iGuageImageID,
								iDrawGuageWidth,
								D3DCOLOR_ARGB(255, 255, 255, 255) );
	}
	else if( m_fScaleWidth > 0 && m_fScaleHeight > 0)
	{
		m_pDrawImpl->Draw( (int)m_sPosition.x, (int)m_sPosition.y, m_iModuleID, m_iGuageImageID, iDrawGuageWidth, m_fScaleWidth, m_fScaleHeight, D3DCOLOR_ARGB(255, 255, 255, 255) );
	}
	else if( m_fScaleWidth > 0 )
	{
		m_pDrawImpl->Draw( (int)m_sPosition.x, (int)m_sPosition.y, m_iModuleID, m_iGuageImageID, iDrawGuageWidth, m_fScaleWidth, 1, D3DCOLOR_ARGB(255, 255, 255, 255) );
	}
	else if( m_fScaleHeight > 0)
	{
		m_pDrawImpl->Draw( (int)m_sPosition.x, (int)m_sPosition.y, m_iModuleID, m_iGuageImageID, iDrawGuageWidth, 1, m_fScaleHeight, D3DCOLOR_ARGB(255, 255, 255, 255) );
	}
	else
	{
		m_pDrawImpl->Draw( (int)m_sPosition.x, (int)m_sPosition.y, iDrawGuageWidth, m_iModuleID, m_iGuageImageID );	
	}	
	
	CWinCtrl::Draw_Font();

}

void	CTGuage::SetValue( int iValue )
{
	m_iValue = iValue;
}

int		CTGuage::GetValue()
{
	return m_iValue;
}

void	CTGuage::SetGuageImageID( int iImageID )
{
	m_iGuageImageID = iImageID;
}

int		CTGuage::GetGuageImageID()
{
	return m_iGuageImageID;
}

int		CTGuage::GetModuleID()
{
	return m_iModuleID;
}

/*
void    CGuage::SetAutoIncrementMaxValue( int iAutoIncrementMaxValue )
{
	m_iAutoIncrementMaxValue = iAutoIncrementMaxValue;
}
*/
void	CTGuage::SetSizeFit( bool bFit )
{
	m_bSizeFit = bFit;
}
bool	CTGuage::GetSizeFit()
{
	return m_bSizeFit;
}
int		CTGuage::GetValueUnitSize()
{
	return m_iValueUnitSize;
}