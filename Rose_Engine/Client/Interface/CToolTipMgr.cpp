#include "stdafx.h"
#include "ctooltipmgr.h"
#include "cdragndropmgr.h"
#include "interfacetype.h"
#include "typeresource.h"

#include "Dlgs/CQuestDlg.h"
#include "Dlgs/CTargetMenu.h"
#include "Dlgs/CPartyDlg.h"
#include "Dlgs/CharacterDlg.h"
#include "Dlgs/QuickToolBar.h"
#include "Dlgs/CMinimapDlg.h"
#include "Dlgs/ChattingDlg.h"
#include "Dlgs/MakeDlg.h"
#include "Dlgs/CSkillDlg.h"
#include "it_mgr.h"
#include "../object.h"
#include "../gamecommon/Skill.h"

CToolTipMgr::CToolTipMgr(void)
: m_HasDrawInfo(false)
, m_Show(true)
, m_InfoDetails(true)
{
}

CToolTipMgr::~CToolTipMgr(void)
{

}

CToolTipMgr& CToolTipMgr::GetInstance()
{
	static CToolTipMgr s_Instance;
	return s_Instance;
}

void CToolTipMgr::RegToolTip( int x, int y, const char* pszMsg )
{
	m_Info.Clear();
	m_HasDrawInfo = true;
	POINT pt  = { x + 20, y };
	m_Info.AddString( pszMsg );
	m_Info.SetPosition( pt );
}

void CToolTipMgr::RegUIInfo( const int x, const int y, const DWORD dwDlgType, const int iToolTipID )
{
	m_Info.Clear();
	m_HasDrawInfo = true;
	POINT pt  = { x + 20, y };

	switch( dwDlgType )
	{
	case DLG_TYPE_PARTY:
		break;
	case DLG_TYPE_CHAR:
		break;
	case DLG_TYPE_QUEST:
		{
			if( iToolTipID == CQuestDlg::IID_BTN_ABANDON )
				m_Info.AddString(TT_QUEST_BTN_ABANDON);

			break;
		}
	case DLG_TYPE_SKILL:
		break;
	case DLG_TYPE_QUICKBAR:
		break;
	case DLG_TYPE_MINIMAP:
		break;

	
	default:
		break;
	}

	m_Info.SetPosition( pt );
}

void  CToolTipMgr::Draw()
{
	if( m_HasDrawInfo && IsShow() )
	{
		if( !CDragNDropMgr::GetInstance().IsDraging() )
		{
			m_Info.Draw();
		}
		m_HasDrawInfo = false;
	}
}

void  CToolTipMgr::RegistInfo( const CInfo& Info )
{
	m_Info.Clear();
	m_Info = Info;
	m_HasDrawInfo = true;
}

void CToolTipMgr::Show( bool bShow )
{
	m_Show = bShow;
}
