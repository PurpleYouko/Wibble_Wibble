#include "StdAfx.h"
#include ".\jcontainer.h"
#include "WinCtrl.h"
#include <algorithm> 



CJContainer::CJContainer(void)
{
}

CJContainer::~CJContainer(void)
{

}

void CJContainer::Draw()
{
	
	iter_CWinCtrl iter;
	int iCnt = 0;
	for( iter =	m_Children.begin(); iter != m_Children.end(); ++iter)
	{
		iCnt++;
		(*iter)->Draw();
	}	
	
}

void CJContainer::Update( POINT ptMouse )
{
	for_each( m_Children.begin(), m_Children.end(), bind2nd( mem_fun( &CWinCtrl::Update ), ptMouse ) );
}

unsigned CJContainer::Process( unsigned uiMsg, WPARAM wParam, LPARAM lParam )
{
	unsigned uiProcID = 0;

	riter_CWinCtrl riter;

	for( riter = m_Children.rbegin(); riter != m_Children.rend(); ++riter)
	{
		uiProcID = (*riter)->Process( uiMsg, wParam, lParam );
		if( uiProcID )
		{
			CWinCtrl* pCtrl = *riter;
			return uiProcID;
		}
	}
	return 0;
}

void CJContainer::MoveCtrl2ListEnd( int iID )
{
	iter_CWinCtrl iter;

	for( iter = m_Children.begin(); iter != m_Children.end(); ++iter )
	{
		if( (int)(*iter)->GetControlID() == iID )
		{
			CWinCtrl* pCtrl = *iter;
			m_Children.erase( iter );
			m_Children.push_back( pCtrl );
			return;
		}
	}
}

void CJContainer::MoveWindow( POINT pt )
{
	iter_CWinCtrl iter;
	for( iter =	m_Children.begin(); iter != m_Children.end(); ++iter)
		(*iter)->MoveWindow( pt );
}

void CJContainer::Show()
{
	iter_CWinCtrl iter;
	for( iter =	m_Children.begin(); iter != m_Children.end(); ++iter)
		(*iter)->Show();
}
void CJContainer::Hide()
{
	iter_CWinCtrl iter;
	for( iter =	m_Children.begin(); iter != m_Children.end(); ++iter)
		(*iter)->Hide();
}

CWinCtrl*	CJContainer::Find( unsigned index )
{
	iter_CWinCtrl iter;
	CWinCtrl*	pCtrl = NULL;
	for( iter =	m_Children.begin(); iter != m_Children.end(); ++iter)
	{
		pCtrl = *iter;
		if( (*iter)->GetControlID() == index )
			return *iter;

		if( pCtrl = (*iter)->Find( index ) )
		{
			return pCtrl;
		}
	}
	return NULL;
}

CWinCtrl*	CJContainer::Find( const char * szName )
{
	iter_CWinCtrl iter;
	
	for( iter =	m_Children.begin(); iter != m_Children.end(); ++iter)
	{
		CWinCtrl * ctrl = *iter;
		if( strcmp( ctrl->GetControlName(), szName ) == 0 )		
			return *iter;

		if( ctrl = (*iter)->Find( szName) )
		{
			return ctrl;
		}
	}
	return NULL;
}

void CJContainer::SetChildEnable( unsigned iID, bool bEnable )
{
	CWinCtrl* pCtrl = Find( iID );
	if( pCtrl )
		pCtrl->SetEnable( bEnable );
}
void CJContainer::Add( CWinCtrl* pCtrl )
{
	_ASSERT( pCtrl );
	m_Children.push_back( pCtrl );
}

void CJContainer::Remove( unsigned index )
{
	iter_CWinCtrl iter;
	CWinCtrl*	pCtrl = NULL;
	for( iter =	m_Children.begin(); iter != m_Children.end(); ++iter)
	{
		if( (*iter)->GetControlID() == index )
		{
			pCtrl = *iter;
			iter = m_Children.erase( iter );			
			SAFE_DELETE(pCtrl);
			return;
		}
	}
}

void CJContainer::RemoveAll()
{
	iter_CWinCtrl iter;
	CWinCtrl*	pCtrl = NULL;
	for( iter =	m_Children.begin(); iter != m_Children.end(); )
	{
		pCtrl = *iter;
		iter = m_Children.erase( iter );
		delete pCtrl;
	}
}

vector<CWinCtrl*> & CJContainer::GetChild()
{
	return m_Children;
}
CWinCtrl * CJContainer::GetChild(int iIndex)
{
	if( iIndex > 0 && iIndex < m_Children.size() )
	{
		return m_Children[iIndex];
	}
	return m_Children[0];
}