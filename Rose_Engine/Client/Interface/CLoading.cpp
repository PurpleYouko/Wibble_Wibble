
#include "stdafx.h"
#include "CLoading.h"
#include "Game.h"
#include <algorithm>

using std::random_shuffle;

CLoading _Loading;

CLoading::CLoading()
{
	m_bInitLoadingImageManager	= false;
	m_hTexture					= NULL;
	m_CustomSet					= true;
	m_LastDayTime				= 0;
	m_bDisplayEventLoadingImage = false;
	m_UseEventLoading = false;
	m_iCountNPCNO = 0;
}

CLoading::~CLoading()
{
	// Destruct
	m_LoadingImageTableByZone.clear();
	m_LoadingImageTableByPlanet.clear();
	m_LoadingImageTableByEvent.clear();
	m_LoadingImageTableDefault.clear();
}

bool CLoading::InitLoadingImageManager()
{
	CGameSTB fSTB;
	int iImageZoneNO = 0;
	int ivariableFirstNO = 0;
	ClientLog(LOG_DEBUG,"Image Manager: Loading custom_ls.rst. (Custom Playlist)");
	if ( fSTB.Open ( "cdat\\custom_ls.rst", _LOAD_NORMAL ) ) 
	{
		int iImageCNT = fSTB.GetRowCount();
		ClientLog(LOG_DEBUG,"Image Manager: Image Count = %i",iImageCNT);
		for ( int i=0 ; i<iImageCNT; i++) 
		{			
			iImageZoneNO = fSTB.GetInteger( 1, i );
			ivariableFirstNO = fSTB.GetInteger(3, i);
			const char* szFileName = fSTB.GetString( 0, i );
			if ( szFileName ) 
			{
				/// If the output image for the event.
				ClientLog(LOG_DEBUG,"Image Manager: szFileName = $s Image Zone # = %i",szFileName, iImageZoneNO);
				if( iImageZoneNO > 13000 )
				{
					m_LoadingImageTableByCustom.insert( std::make_pair< int, std::string >( iImageZoneNO - 10000,  szFileName ) );
					m_CustomList3.insert( std::make_pair<int, int>( iImageZoneNO - 10000, ivariableFirstNO ) );
				}
				else if( iImageZoneNO > 12000 )
				{
					m_LoadingImageTableByCustom.insert( std::make_pair< int, std::string >( iImageZoneNO - 10000,  szFileName ) );
					m_CustomList2.insert( std::make_pair<int, int>( iImageZoneNO - 10000, ivariableFirstNO ) );
				}
				else if( iImageZoneNO > 11000 )
				{
					m_LoadingImageTableByCustom.insert( std::make_pair< int, std::string >( iImageZoneNO - 10000,  szFileName ) );
					m_CustomList1.insert( std::make_pair<int, int>( iImageZoneNO - 10000, ivariableFirstNO ) );
				}
			}
			else
			{
				assert( 0 && "Invalid loading image zone NO" );
			}
		}
		ClientLog(LOG_DEBUG,"Image Manager. custom_ls.rst");
		fSTB.Close ();
		ClientLog(LOG_DEBUG,"Image Manager. Finished processing file and closed fSTB");
	}
	else
	{
		ClientLog(LOG_DEBUG,"Image Manager. custom_ls.rst doesn't exist. Setting custom playlist to False");
		m_CustomSet = false;
	}


	if( LoadImageTable( "3DData\\STB\\LIST_LOADING.STB" ) )
	{
		m_bInitLoadingImageManager = true;
		return true;
	}


	return false;
}

bool CLoading::LoadTexture( int iZoneNo, int iPlanetNO )
{
	if( !m_bInitLoadingImageManager )
	{
		if( InitLoadingImageManager() == false )
		{			
			return false;
		}
	}

	if( m_hTexture )
		UnloadTexture();

	setDelayedLoad(0);


	if (!m_CustomList1.empty() || !m_CustomList2.empty() || !m_CustomList3.empty())
	{
		if (!m_CustomSet)
		{
			static const unsigned long playlistResetInterval = (60 * 60 * 1000);
			unsigned long curDayHours = ::timeGetTime();

			// timeGetTime returns millisecs
			if ((curDayHours - m_LastDayTime) >= playlistResetInterval)
			{
				m_Playlist.clear();
				m_CustomSet = true;
			}
		}
	}
	else
	{
		m_Playlist.clear();
		m_CustomSet = false;
	}

	if (m_CustomSet)
	{
		if (m_Playlist.empty())
		{
			std::deque<int> tmpList;
			std::map<int, int>::const_iterator isl;
			int iis;
			for (isl = m_CustomList1.begin(); isl != m_CustomList1.end(); ++isl)
			{
				for (iis = 0; iis < (*isl).second; ++iis)
				{
					tmpList.push_back((*isl).first);
				}
			}
			std::random_shuffle(tmpList.begin(), tmpList.end());
			std::random_shuffle(tmpList.begin(), tmpList.end());
			m_Playlist.insert(m_Playlist.end(), tmpList.begin(), tmpList.end());
			tmpList.clear();
			for (isl = m_CustomList2.begin(); isl != m_CustomList2.end(); ++isl)
			{
				for (iis = 0; iis < (*isl).second; ++iis)
				{
					tmpList.push_back((*isl).first);
				}
			}
			std::random_shuffle(tmpList.begin(), tmpList.end());
			std::random_shuffle(tmpList.begin(), tmpList.end());
			m_Playlist.insert(m_Playlist.end(), tmpList.begin(), tmpList.end());
			tmpList.clear();
			for (isl = m_CustomList3.begin(); isl != m_CustomList3.end(); ++isl)
			{
				for (iis = 0; iis < (*isl).second; ++iis)
				{
					tmpList.push_back((*isl).first);
				}
			}
			std::random_shuffle(tmpList.begin(), tmpList.end());
			std::random_shuffle(tmpList.begin(), tmpList.end());
			m_Playlist.insert(m_Playlist.end(), tmpList.begin(), tmpList.end());
		}
	}

	if( iZoneNo != 9999 )
	{
		if (m_CustomSet && !m_Playlist.empty())
		{
			m_hTexture = this->GetLoadingImageCustom( 0, m_Playlist.front() );
			m_Playlist.pop_front();

			if (m_Playlist.empty())
			{
				m_CustomSet = false;
				m_LastDayTime = ::timeGetTime();
			}
		}
		else
		{
			m_hTexture = GetLoadingImage( iZoneNo, iPlanetNO );
		}
	}

	if( m_hTexture == 0 )
	{
		m_hTexture = loadTexture ( "3DData\\Control\\Res\\Loading.dds", 
								"3DData\\Control\\Res\\Loading.dds",
								1, 
								0 );
	}

	setDelayedLoad(1);
	if( m_hTexture == NULL )
	{
#ifdef _DEBUG
			g_pCApp->ErrorBOX( "Loading image load failed !!", "File open error" );
#endif
			//Gives less reason for the failure.
			return false;
	}    

	return true;
}

bool	CLoading::LoadTexture( std::vector < std::string >	m_mMapEventLoadingTable )
{
	if( !m_bInitLoadingImageManager )
	{
		if( InitLoadingImageManager() == false )
		{			
			return false;
		}
	}

	if( m_hTexture )
		UnloadTexture();

	setDelayedLoad(0);

	//------------------------------------------------------------------------------
	/// 2006/05/11 NPC variables can be changed to modify the reference load image.
	//------------------------------------------------------------------------------

	int iRefNO = 0;

	if(m_mMapEventLoadingTable.size() > 1 )
	{
		iRefNO = RANDOM( m_mMapEventLoadingTable.size() );		
	}

	m_hTexture = loadTexture(m_mMapEventLoadingTable[iRefNO].c_str() , m_mMapEventLoadingTable[iRefNO].c_str(), 1, 0);

	if( m_hTexture == 0 )
	{
		m_hTexture = loadTexture ( "3DData\\Control\\Res\\Loading.dds", 
			"3DData\\Control\\Res\\Loading.dds",
			1, 
			0 );
	}

	setDelayedLoad(1);
	if( m_hTexture == NULL )
	{
#ifdef _DEBUG
		g_pCApp->ErrorBOX( "Loading image load failed !!", "File open error" );
#endif
		//Gives less reason for the failure.
		return false;
	}    

	return true;
}




void CLoading::UnloadTexture()
{
	if( m_hTexture != NULL )
	{
		unloadTexture( m_hTexture );
		m_hTexture = NULL;
	}
}

void CLoading::Draw()
{
	if( m_hTexture == NULL )
		return;

	// Transform	
	D3DXMATRIX mat;	
	D3DXMatrixTranslation( &mat, 0,	0, 0.0f );
	int iWidth, iHeight;
	D3DXVECTOR3 vpos(0,0,0);
	
	::getTextureSize( m_hTexture, iWidth, iHeight );
	
	if( iWidth != 0 && iHeight != 0 )
	{
		float scr_ar = (float)g_pCApp->GetWIDTH()/(float)g_pCApp->GetHEIGHT();
		float tex_ar = (float)iWidth/(float)iHeight;
		if (fabs(scr_ar - tex_ar) > 0.0001f)
		{
			D3DXMatrixScaling( &mat, (float)g_pCApp->GetWIDTH()/(float)iWidth, 
				((float)iHeight/(float)iWidth)*(float)g_pCApp->GetWIDTH()/(float)iHeight, 1.0f );
			vpos = D3DXVECTOR3(0.0f, 
				(((float)iHeight * (float)g_pCApp->GetWIDTH() / (float)iWidth) - (float)g_pCApp->GetHEIGHT()) * (float)iWidth/(float)g_pCApp->GetWIDTH() * -0.5f, 
				0.0f);
		}
		else
		{
			D3DXMatrixScaling( &mat, (float)g_pCApp->GetWIDTH()/(float)iWidth, (float)g_pCApp->GetHEIGHT()/(float)iHeight, 1.0f );
		}
	}
	
	::setTransformSprite( mat );
	::drawSprite( m_hTexture, 
					NULL,
					NULL,
					&vpos,								
					D3DCOLOR_RGBA( 255, 255, 255, 255 ) );

}


HNODE CLoading::GetLoadingImageCustom( int tp, int iCustomID )
{
	std::multimap<int, std::string>::iterator iil;
	iil = m_LoadingImageTableByCustom.find( iCustomID );
	if( iil != m_LoadingImageTableByCustom.end() )
	{
		return ::loadTexture( iil->second.c_str(), iil->second.c_str(), 1, 0 ); 
	}
	return 0;
}

//------------------------------------------------------------------------
/// List_Loading.sTB Generates a table from an image.
//------------------------------------------------------------------------
bool CLoading::LoadImageTable( char* strSTBName )
{
	CGameSTB fSTB;
	int iImageZoneNO = 0;
	int iNpcNO = 0;
	int ivariableFirstNO = 0;
	int ivariableSecondNO = 0;

	if ( fSTB.Open ( strSTBName ) ) 
	{
		int iImageCNT = fSTB.GetRowCount();

		/// Loading image to the output for the event should you do?
		int iIsEventLoading = fSTB.GetInteger( 1, 0 );
		if( iIsEventLoading == 0 && fSTB.GetString( 0, 0 ) )
		{
			m_bDisplayEventLoadingImage = true;
		}
				
		for ( int i=0 ; i<iImageCNT; i++) 
		{			
			iImageZoneNO = fSTB.GetInteger( 1, i );
			iNpcNO = fSTB.GetInteger(2, i);
			ivariableFirstNO = fSTB.GetInteger(3, i);
			ivariableSecondNO = fSTB.GetInteger(4, i);
			const char* szFileName = fSTB.GetString( 0, i );
			if ( szFileName ) 
			{
				/// If the output image for the event.
				if( m_bDisplayEventLoadingImage )
				{
					m_LoadingImageTableByEvent.push_back( szFileName );
				}else
				{
					if(iNpcNO > 0)
					{
						m_EventTable.EventTable[m_iCountNPCNO].NpcNO = iNpcNO;
						m_EventTable.EventTable[m_iCountNPCNO].FirstNO = ivariableFirstNO;
						m_EventTable.EventTable[m_iCountNPCNO].SecondNO = ivariableSecondNO;
						m_EventTable.EventTable[m_iCountNPCNO].sFileName = szFileName;
						m_iCountNPCNO++;
						m_UseEventLoading = true;
					}
					else if (iImageZoneNO == 1000)
					{
						m_LoadingImageTableDefault.push_back( szFileName);
					}
					else if (iImageZoneNO > 1000)
					{
					}
					else if (iImageZoneNO > 500)
					{
						m_LoadingImageTableByPlanet.insert( std::make_pair< int, std::string >( iImageZoneNO - 500,  szFileName ) );
					}
					else if (iImageZoneNO > 0)
					{
						m_LoadingImageTableByZone.insert( std::make_pair< int, std::string >( iImageZoneNO,  szFileName ) );
					}
				}
			}else
			{
				assert( 0 && "Invalid loading image zone NO" );
			}
		}
		fSTB.Close ();
	
		m_EventTable.EventLoadingCount = m_iCountNPCNO;
		return true;
	}

	return false;
}

//------------------------------------------------------------------------
/// Output from that zone to determine the need to return the loaded image.
//------------------------------------------------------------------------
HNODE CLoading::GetLoadingImage( int iZoneNO, int iPlanetNO )
{
	HNODE hTexture = 0;

	/// Should output images for an event?
	if( m_bDisplayEventLoadingImage )
	{
		int iImageCount = m_LoadingImageTableByEvent.size();
		int iRefNO = RANDOM( iImageCount );

		hTexture = ::loadTexture(	m_LoadingImageTableByEvent[ iRefNO ].c_str(), 
									m_LoadingImageTableByEvent[ iRefNO ].c_str(), 
									1,
									0 ); 

		return hTexture;
	}
	
	/// First, make a table look for John.
	if( m_LoadingImageTableByZone.find( iZoneNO ) != m_LoadingImageTableByZone.end() )
	{
		hTexture = GetLoadingImageFromTable( m_LoadingImageTableByZone, iZoneNO );
		return hTexture;
	}
/*
	/// Not in the existing table table to find the planet.
	if( m_LoadingImageTableByPlanet.find( iPlanetNO ) != m_LoadingImageTableByPlanet.end() )
	{
		hTexture = GetLoadingImageFromTable( m_LoadingImageTableByPlanet, iPlanetNO );
		return hTexture;
	}
*/
	int iImageCount = m_LoadingImageTableDefault.size();
	int iRefNO = RANDOM( iImageCount );
	hTexture = ::loadTexture(	m_LoadingImageTableDefault[ iRefNO ].c_str(), 
								m_LoadingImageTableDefault[ iRefNO ].c_str(), 
								1,
								0 ); 

	return hTexture;
}

HNODE CLoading::GetLoadingImageFromTable( std::multimap< int, std::string >& imageTable, int iKey )
{
	std::multimap< int, std::string >::iterator itorLow, itorUpper;

	itorLow		= imageTable.lower_bound( iKey );
	itorUpper	= imageTable.upper_bound( iKey );

	int iDistance = std::distance( itorLow, itorUpper );
	if( iDistance > 1 )
	{
		int iRefNO = RANDOM( iDistance );
		std::advance( itorLow, iRefNO );			
	}

	HNODE hTexture = ::loadTexture( itorLow->second.c_str(), itorLow->second.c_str(), 1, 0 ); 
	return hTexture;
}

//------------------------------------------------------------------------
///
//------------------------------------------------------------------------
void CLoading::ReleaseTexture( HNODE hTexture )
{
	if( hTexture )
	{
		::unloadTexture( hTexture );
		hTexture = 0;
	}
}

bool CLoading::CheckUseEventLoadingImage()
{

	return false;
}

