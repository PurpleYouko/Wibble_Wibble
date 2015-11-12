#include "stdafx.h"
#include ".\coptiondlg.h"
#include "../../../TGameCtrl/TComboBox.h"
#include "../../../TGameCtrl/TScrollBar.h"
#include "../../Game.h"
#include "../../System/CGame.h"
#include "../../CApplication.h"
#include "../it_mgr.h"
#include "../../Common/IO_STB.h"
#include "../../JCommandState.h"
#include "../Cursor/CCursor.h"
#include "../../Sound/MusicMgr.h"

#include "JContainer.h"
#include "TabbedPane.h"
#include "TComboBox.h"
#include "TScrollBar.h"
#include "TRadioBox.h"
#include "TRadioButton.h"
#include "TCheckBox.h"
#include "TIme2.h"
#include "TControlMgr.h"
#include "TEditBox.h"
#include "TImage.h"
#include "../../IO_Terrain.h"
#include "TriggerInfo.h"

#include "../Command/UICommand.h"

COptionDlg::COptionDlg(void)
{
	ZeroMemory(&m_VideoOption, sizeof( t_OptionVideo ));
	m_iTab = IID_TABVIDEO;
	m_pAdptInfo = NULL;
}

COptionDlg::~COptionDlg(void)
{
	if(m_pAdptInfo)
	{
		TI_ReleaseAdptInfo();
		m_pAdptInfo = NULL;
	}
}

void COptionDlg::Hide()
{
	CTDialog::Hide();
	
	SaveCurrentOption();
}


bool COptionDlg::Create( const char* szIDD )
{
	char*	pszName;
	if( CTDialog::Create( szIDD ) == false )
		return false;

	m_pAdptInfo = TI_ReadAdptInfoFile ();
	int	iAdptIDX = 0;

	SystemResolution *pResolution = m_pAdptInfo->pAdapter[ iAdptIDX ].pResolution;
	int iResCNT = m_pAdptInfo->pAdapter[ iAdptIDX ].nResolution;

	SupportedScreen Screen;

	m_SupportedScreens.clear();
	for( int i = 0; i < iResCNT; ++i )
	{
		AddSupportedScreen( pResolution[ i ].pixel_width,
			pResolution[ i ].pixel_height,
			pResolution[ i ].pixel_color,
			pResolution[ i ].frequency );
	}

	CWinCtrl* pCtrl = NULL;		

	///VIDEO	
	pCtrl = Find( IID_COMBO_RESOLUTION );
	if( pCtrl && pCtrl->GetControlType() == CTRL_COMBOBOX )
	{
		CTComboBox* pComboBox = (CTComboBox*)pCtrl;
		std::vector<SupportedScreen>::iterator iter;
		for( iter = m_SupportedScreens.begin(); iter != m_SupportedScreens.end(); ++iter )
		{
			pComboBox->AddItem( CStr::Printf("%d*%d*%dbit", 
				iter->m_iWidth, iter->m_iHeight, iter->m_iDepth ) );
		}
	}

	pCtrl = Find( IID_LIST_CAMERA );
	assert( pCtrl );
	if( pCtrl && pCtrl->GetControlType() == CTRL_LISTBOX )
	{
		CTListBox* pListBox = (CTListBox*)pCtrl;
		pListBox->SetExtent( 1 );
		for( int i = 0; i < 3; ++i )
		{
			pszName = CAMERA_NAME(i);
			if( pszName )
				pListBox->AppendText( pszName , g_dwWHITE );
		}
	}

	pCtrl = Find( IID_LIST_ZNZIN );
	assert( pCtrl );
	if( pCtrl && pCtrl->GetControlType() == CTRL_LISTBOX )
	{
		CTListBox* pListBox = (CTListBox*)pCtrl;
		pListBox->SetExtent( 1 );
		for( int i = 0; i < 5; ++i )
			pListBox->AppendText( "1" , g_dwWHITE );
	}


	/*	pCtrl = pContainer->Find( IID_LIST_GAMMA );
	assert( pCtrl );
	if( pCtrl && pCtrl->GetControlType() == CTRL_LISTBOX )
	{
	CTListBox* pListBox = (CTListBox*)pCtrl;
	pListBox->SetExtent( 1 );
	for( int i = 0; i < 5; ++i )
	pListBox->AppendText( "1" , g_dwWHITE );
	}*/

	///AUDIO	
	pCtrl = Find( IID_LIST_BGM );
	if( pCtrl && pCtrl->GetControlType() == CTRL_LISTBOX )
	{
		CTListBox* pListBox = (CTListBox*)pCtrl;
		for( int i = 0; i < MAX_BGM_VOLUME ; ++i )
		{
			pListBox->AppendText( "1", g_dwWHITE );
		}
	}


	pCtrl = Find( IID_LIST_EFFECTSOUND );
	if( pCtrl && pCtrl->GetControlType() == CTRL_LISTBOX )
	{
		CTListBox* pListBox = (CTListBox*)pCtrl;
		for( int i = 0; i < MAX_EFFECT_VOLUME; ++i )
		{
			pListBox->AppendText("2", g_dwWHITE );
		}
	}
	
	pCtrl = Find( IID_COMBO_ANTIALIASING );
	if( pCtrl && pCtrl->GetControlType() == CTRL_COMBOBOX )
	{
		short AntiAliasingValues[] = { 0, 2, 4, 8 };
		CTComboBox* pComboBox = (CTComboBox*)pCtrl;
		pComboBox->AddItem("0");
		pComboBox->AddItem("2");
		pComboBox->AddItem("4");
		pComboBox->AddItem("8");
		pComboBox->SetSelectedItem(g_ClientStorage.GetVideoAntiAliasing());
	}
	GetCurrentOption();
	return true;
}


void COptionDlg::OnLButtonDown( unsigned iProcID )
{
	switch( iProcID )
	{
	case IID_COMBO_RESOLUTION:
		{
			CWinCtrl* pCtrl = NULL;				

			pCtrl = Find( IID_COMBO_RESOLUTION );
			if( pCtrl && pCtrl->GetControlType() == CTRL_COMBOBOX )
			{
				CTComboBox* pComboBox = (CTComboBox*)pCtrl;
				short nResolutionIndex = pComboBox->GetSelectedItemID();				

				pCtrl = Find( IID_COMBO_FREQUENCY );
				if( pCtrl && pCtrl->GetControlType() == CTRL_COMBOBOX )
				{
					CTComboBox* pComboBox = (CTComboBox*)pCtrl;
					std::vector< int >::iterator iter;
					short iIndex = 0;
					short iSelectIndex = 0;
					pComboBox->ClearItem();
					for( iter = m_SupportedScreens[nResolutionIndex].m_Frequencys.begin();
						iter != m_SupportedScreens[nResolutionIndex].m_Frequencys.end();
						++iter, ++iIndex )
					{
						pComboBox->AddItem( CStr::Printf("%dHz",*iter ) );
						if( *iter == m_VideoOption.tResolution.iFrequency )
							iSelectIndex = iIndex;
					}
					pComboBox->SetSelectedItem( iSelectIndex );
				}
			}

			ChangeVideoOption();

			break;
		}
	case IID_COMBO_FREQUENCY:
		ChangeVideoOption();
		break;
	case IID_COMBO_ANTIALIASING:
		ChangeVideoOption();
		break;
	case IID_RADIOBUTTON_DEFAULTCONTROL:
	case IID_RADIOBUTTON_SEVENHEARTSCONTROL:
		ChangePlayOption();
		break;
	case IID_RADIOBTN_ENTERCHATTING:
	case IID_RADIOBTN_NORMALCHATTING:
		ChangeKeyboardOption();
		break;

	case IID_RADIOBOX_WHISPER:
	case IID_RADIOBOX_ADDFRIEND:
	case IID_RADIOBOX_EXCHANGE:
	case IID_RADIOBOX_PARTY:
	case IID_RADIOBOX_MESSANGER:
	case IID_RADIOBOX_PICKUP:
	case IID_RADIOBUTTON_SHORTCUT_SHOW:
	case IID_RADIOBUTTON_SHORTCUT_HIDE:
		ChangeCommunityOption();
		break;
	case IID_SCROLLBAR_BGM:
	case IID_SCROLLBAR_EFFECTSOUND:
		ChangeAudioOption();
		break;

	default:
		break;
	}
}


void COptionDlg::OnLButtonUp( unsigned iProcID )
{
	switch( iProcID )
	{
	case IID_BTN_ICONIZE:
		g_itMGR.AddDialogIcon( 	GetDialogType() );
		break;
	case IID_BTN_INITIALIZE:
		g_itMGR.InitInterfacePos();
		InitOption();
		break;
	case IID_TABBUTTON_VIDEO:
		m_iTab = IID_TABVIDEO;
		break;
	case IID_TABBUTTON_AUDIO:
		m_iTab = IID_TABAUDIO;
		break;
	case IID_TABBUTTON_PLAY:
		m_iTab = IID_TABPLAY;
		break;
	case IID_TABBUTTON_COMMUNITY:
		m_iTab = IID_TABCOMMUNITY;
		break;
	case IID_TABBUTTON_KEYBOARD:
		m_iTab = IID_TABKEYBOARD;
		break;
	case IID_BTN_CONFIRM:
		ChangeOption();
		SaveCurrentOption();
		Hide();
		break;	
	case IID_BTN_CLOSE:
		DoCancel();
		Hide();		
		break;


	case IID_SCROLLBAR_CAMERA:
		ChangeVideoOption();
		break;
	case IID_SCROLLBAR_PERFORMANCE:
		ChangeVideoOption();
		break;

	case IID_SCROLLBAR_BGM:		
	case IID_SCROLLBAR_EFFECTSOUND:
		ChangeAudioOption();
		break;

	default:
		break;
	}
}

unsigned int COptionDlg::Process(UINT uiMsg,WPARAM wParam,LPARAM lParam)
{
	if(!IsVision()) return 0;
	
	unsigned iProcID = 0;
	if( iProcID = CTDialog::Process(uiMsg,wParam,lParam)) 
	{
		switch(uiMsg)
		{
		case WM_LBUTTONDOWN:
			OnLButtonDown( iProcID );
			break;
		case WM_LBUTTONUP:
			OnLButtonUp( iProcID );
			break;
		default:
			break;
		}
		return uiMsg;
	}
	return 0;
}

void COptionDlg::Show()
{


#ifdef _DEBUG

	POINT pt = { GetPosition().x, GetPosition().y };
	Clear();
	Create("DlgOption");		
	SetInterfacePos_After();
	MoveWindow(pt);

#endif


	GetCurrentOption();
	CTDialog::Show();
}
void COptionDlg::Update( POINT ptMouse )
{
	if( !IsVision() ) return;
	CTDialog::Update( ptMouse );

	///실시간으로 볼륨 조정
	switch( m_iTab )
	{
	case IID_TABAUDIO:
		{
			CWinCtrl* pCtrl = Find( IID_TABBEDPANE );
			assert( pCtrl && pCtrl->GetControlType() == CTRL_TABBEDPANE );
			if( pCtrl && pCtrl->GetControlType() == CTRL_TABBEDPANE )
			{
				CTabbedPane* pPane = (CTabbedPane*)pCtrl;
				CJContainer* pContainer = pPane->GetTabContainer( IID_TABAUDIO );
				assert( pContainer );
				if( pContainer == NULL ) 
					return;


				pCtrl = pContainer->Find( IID_LIST_BGM );
				assert( pCtrl && pCtrl->GetControlType() == CTRL_LISTBOX );
				if( pCtrl && pCtrl->GetControlType() == CTRL_LISTBOX )
				{
					CTListBox* pListBox = ( CTListBox* )pCtrl;
					CMusicMgr::GetSingleton().SetVolume( g_ClientStorage.GetBgmVolumeByIndex( pListBox->GetValue() ) );
				}

				pCtrl = pContainer->Find( IID_LIST_EFFECTSOUND );
				assert( pCtrl && pCtrl->GetControlType() == CTRL_LISTBOX );
				if( pCtrl && pCtrl->GetControlType() == CTRL_LISTBOX )
				{
					CTListBox* pListBox = ( CTListBox* )pCtrl;					
					g_pSoundLIST->SetVolume( g_ClientStorage.GetEffectVolumeByIndex( pListBox->GetValue() ) );
				}
			}
			break;
		}
	case IID_TABVIDEO:
		{
			/*CWinCtrl* pCtrl = Find( IID_TABBEDPANE );
			assert( pCtrl && pCtrl->GetControlType() == CTRL_TABBEDPANE );
			if( pCtrl && pCtrl->GetControlType() == CTRL_TABBEDPANE )
			{
				CTabbedPane* pPane = (CTabbedPane*)pCtrl;
				CJContainer* pContainer = pPane->GetTabContainer( IID_TABVIDEO );
				assert( pContainer );
				if( pContainer == NULL ) 
					return;


				pCtrl = pContainer->Find( IID_LIST_GAMMA );
				assert( pCtrl && pCtrl->GetControlType() == CTRL_LISTBOX );
				if( pCtrl && pCtrl->GetControlType() == CTRL_LISTBOX )
				{
					CTListBox* pListBox = ( CTListBox* )pCtrl;
					setGammaValue( c_GammaValues[ pListBox->GetValue() ] );		
				}
			}					*/
			break;
		}
	default:
		break;
	}
}



void COptionDlg::ChangeResolution( t_OptionResolution Resolution )
{

	int iWidth  = Resolution.iWidth;
	int iHeight = Resolution.iHeight;
	int iDepth  = Resolution.iDepth;
	int iFrequency = Resolution.iFrequency;

	///변경된 사항이 없다면 바꾸지 않는다.
	if( iWidth == m_VideoOption.tResolution.iWidth &&
		iHeight == m_VideoOption.tResolution.iHeight &&
		iDepth == m_VideoOption.tResolution.iDepth &&
		iFrequency == m_VideoOption.tResolution.iFrequency
		)
		return;


	if( iWidth != m_VideoOption.tResolution.iWidth ||
		iHeight != m_VideoOption.tResolution.iHeight ||
		iDepth != m_VideoOption.tResolution.iDepth )
	{
		g_pCApp->ResizeWindowByClientSize( iWidth, iHeight, iDepth, true );
		m_VideoOption.tResolution.iWidth  = iWidth;
		m_VideoOption.tResolution.iHeight = iHeight;
		m_VideoOption.tResolution.iDepth  = iDepth;
		g_ClientStorage.SetVideoOption( m_VideoOption );
	}
	
	if( iFrequency != m_VideoOption.tResolution.iFrequency )
	{
		setMonitorRefreshRate( iFrequency );
		resetScreen();
	}

	//resetScreen();
	
	CCursor::GetInstance().ReloadCursor();


//	g_pCApp->SetWIDTH( g_GameDATA.m_nScrWidth );
//	g_pCApp->SetHEIGHT( g_GameDATA.m_nScrHeight );

	SIZE sizeScreen = { g_pCApp->GetWIDTH(), g_pCApp->GetHEIGHT() };
	CTIme::GetInstance().ChangeScreenSize( sizeScreen );
	_RPT2( _CRT_WARN,"Screen Width %d, Height %d\n", g_pCApp->GetWIDTH(), g_pCApp->GetHEIGHT() );
	g_itMGR.InitInterfacePos();
}

void COptionDlg::DoCancel()
{
	CMusicMgr::GetSingleton().SetVolume( g_ClientStorage.GetBgmVolumeByIndex( m_SoundOption.iBgmVolume ) );	
	g_pSoundLIST->SetVolume(g_ClientStorage.GetEffectVolumeByIndex( m_SoundOption.iEffectVolume ) );

}


void COptionDlg::ChangeKeyboardOption()
{
	CWinCtrl * pCtrl = Find( IID_RADIOBOX_KEYBOARD );
	assert( pCtrl );
	if( pCtrl && pCtrl->GetControlType() == CTRL_RADIOBOX )
	{
		CTRadioBox* pRadioBox = (CTRadioBox*)pCtrl;
		UINT uiPressedBtn = pRadioBox->GetPressedButtonID();
		switch( uiPressedBtn )
		{
		case IID_RADIOBTN_ENTERCHATTING:
			{
				m_KeyboardOption.iChattingMode = 0;
				it_SetKeyboardInputType( CTControlMgr::INPUTTYPE_NORMAL );
				break;
			}
		case IID_RADIOBTN_NORMALCHATTING:
			{
				m_KeyboardOption.iChattingMode = 1;
				it_SetKeyboardInputType( CTControlMgr::INPUTTYPE_AUTOENTER );
				///현재 포커스를 가진 EditBox가 없을경우 채팅창 EditBox에 강제로 포커스를 준다.
				CTDialog* pDlg = g_itMGR.FindDlg( DLG_TYPE_CHAT );
				if( CTEditBox::s_pFocusEdit == NULL && pDlg != NULL  )
					pDlg->Show();

				break;
			}
		default:
			break;
		}
	}
}


void COptionDlg::ChangeVideoOption()
{	
	CWinCtrl * pCtrl = NULL;

	pCtrl = Find( IID_LIST_CAMERA );
	assert( pCtrl );
	if( pCtrl && pCtrl->GetControlType() == CTRL_LISTBOX )
	{
		CTListBox* pListBox = (CTListBox*)pCtrl;
		short iID = pListBox->GetValue();
		if( m_VideoOption.iCamera != iID )
		{
			g_ClientStorage.ApplyCameraOption( iID );
			m_VideoOption.iCamera = iID;
		}
	}

	pCtrl = Find( IID_LIST_ZNZIN );
	assert( pCtrl );
	if( pCtrl && pCtrl->GetControlType() == CTRL_LISTBOX )
	{
		CTListBox* pListBox = (CTListBox*)pCtrl;
		short iID = pListBox->GetValue();
		if( m_VideoOption.iPerformance != iID )
		{
			setDisplayQualityLevel( c_iPeformances[iID] );				
			m_VideoOption.iPerformance = iID;				
		}
	}

	pCtrl = Find( IID_COMBO_RESOLUTION );
	if( pCtrl && pCtrl->GetControlType() == CTRL_COMBOBOX )
	{
		CTComboBox* pComboBox = (CTComboBox*)pCtrl;
		short iID = pComboBox->GetSelectedItemID();

		assert( iID >= 0 && iID < (int)m_SupportedScreens.size() );
		if( iID >= 0 && iID < (int)m_SupportedScreens.size() )
		{
			pCtrl = Find( IID_COMBO_FREQUENCY );
			if( pCtrl && pCtrl->GetControlType() == CTRL_COMBOBOX )
			{
				CTComboBox* pComboBox = (CTComboBox*)pCtrl;
				int iFrequencyIndex = pComboBox->GetSelectedItemID();
				assert( iFrequencyIndex >= 0 && iFrequencyIndex < (int)m_SupportedScreens[iID].m_Frequencys.size() );

				if( iFrequencyIndex >= 0 && iFrequencyIndex < (int)m_SupportedScreens[iID].m_Frequencys.size() )
				{
					t_OptionResolution Resolution;
					Resolution.iWidth      = m_SupportedScreens[ iID ].m_iWidth;
					Resolution.iHeight     = m_SupportedScreens[ iID ].m_iHeight;
					Resolution.iDepth      = m_SupportedScreens[ iID ].m_iDepth;
					Resolution.iFrequency  = m_SupportedScreens[ iID ].m_Frequencys[ iFrequencyIndex];

					ChangeResolution( Resolution );

					m_VideoOption.tResolution = Resolution;
				}
			}
		}
	}

	// Anti Aliasing Option
	pCtrl = Find( IID_COMBO_ANTIALIASING );
	if( pCtrl && pCtrl->GetControlType() == CTRL_COMBOBOX )
	{
		CTComboBox* pComboBox = (CTComboBox*)pCtrl;
		short iID = pComboBox->GetSelectedItemID();
		if( m_VideoOption.iAntiAliasing != iID )
		{
			g_ClientStorage.ApplyAntiAliasingOption( iID );
			m_VideoOption.iAntiAliasing = iID;
			resetScreen();
		}
	}
	g_itMGR.RefreshDlg();
}


void COptionDlg::ChangeAudioOption()
{
	CWinCtrl * pCtrl = Find( IID_LIST_BGM );
	if( pCtrl && pCtrl->GetControlType() == CTRL_LISTBOX )
	{
		CTListBox* pListBox = (CTListBox*)pCtrl;
		m_SoundOption.iBgmVolume = pListBox->GetValue();

		CMusicMgr::GetSingleton().SetVolume( g_ClientStorage.GetBgmVolumeByIndex(pListBox->GetValue()) );
		
	}
	else
	{
		assert( 0 && "Not Found Bgm Volume List" );
	}

	pCtrl = Find( IID_LIST_EFFECTSOUND );
	if( pCtrl && pCtrl->GetControlType() == CTRL_LISTBOX )
	{
		CTListBox* pListBox = (CTListBox*)pCtrl;
		m_SoundOption.iEffectVolume = pListBox->GetValue();
		g_pSoundLIST->SetVolume( g_ClientStorage.GetEffectVolumeByIndex(pListBox->GetValue()) );		
	}
	else
	{
		assert( 0 && "Not Found Effect Volume List" );
	}
}


void COptionDlg::ChangePlayOption()
{
	CWinCtrl * pCtrl = NULL;
	pCtrl = Find( IID_RADIOBOX_CONTROL );

	assert( pCtrl && pCtrl->GetControlType() == CTRL_RADIOBOX );
	if( pCtrl && pCtrl->GetControlType() == CTRL_RADIOBOX )
	{
		CTRadioBox* pRadioBox = (CTRadioBox*)pCtrl;
		switch( pRadioBox->GetPressedButtonID() )
		{
		case IID_RADIOBUTTON_DEFAULTCONTROL:
			{
				g_UserInputSystem.ChangeUserInputStyle( DEFAULT_USER_INPUT_STYLE );
				m_PlayOption.uiControlType = 0;					
				CTCmdSetOption Cmd( "CTRL", "DEFAULTCONTROL" );
				Cmd.Exec(NULL);
			}
			break;
		case IID_RADIOBUTTON_SEVENHEARTSCONTROL:
			{
				g_UserInputSystem.ChangeUserInputStyle( SEVENHEARTS_USER_INPUT_STYLE );
				m_PlayOption.uiControlType = 1;
				CTCmdSetOption Cmd( "CTRL", "SEVENHEARTSCONTROL" );
				Cmd.Exec(NULL);
			}				
			break;
		default:
			break;
		}
	}	

	m_PlayOption.iShowNpcName = 1;
}


void COptionDlg::ChangeOption()
{

	switch( m_iTab )
	{
	case IID_TABVIDEO:
		ChangeVideoOption();
		break;
	case IID_TABAUDIO:
		ChangeAudioOption();
		break;
	case IID_TABPLAY:
		ChangePlayOption();
		break;
	case IID_TABCOMMUNITY:
		ChangeCommunityOption();
		break;
	case IID_TABKEYBOARD:
		ChangeKeyboardOption();
		break;
	default:
		break;
	}
}

void COptionDlg::InitOption()
{
	switch( m_iTab )
	{
	case IID_TABVIDEO:
		ChangeVideoOption();
		break;
	case IID_TABAUDIO:
		ChangeAudioOption();
		break;
	case IID_TABPLAY:
		ChangePlayOption();
		break;
	case IID_TABCOMMUNITY:
		ChangeCommunityOption();
		break;
	default:
		break;
	}
}

void COptionDlg::SaveCurrentOption()
{
	CWinCtrl* pCtrl = Find( IID_TABBEDPANE );
	if( pCtrl && pCtrl->GetControlType() == CTRL_TABBEDPANE )
	{
		CTabbedPane* pPane = (CTabbedPane*)pCtrl;
		CJContainer* pContainer = pPane->GetTabContainer( IID_TABVIDEO );
		assert( pContainer );

		pCtrl = pContainer->Find( IID_LIST_BGM );
		if( pCtrl && pCtrl->GetControlType() == CTRL_LISTBOX )
		{
			CTListBox* pListBox = (CTListBox*)pCtrl;
			m_SoundOption.iBgmVolume = pListBox->GetValue();

			CMusicMgr::GetSingleton().SetVolume( g_ClientStorage.GetBgmVolumeByIndex( m_SoundOption.iBgmVolume ) );			
		}

		pCtrl = pContainer->Find( IID_LIST_EFFECTSOUND );
		if( pCtrl && pCtrl->GetControlType() == CTRL_LISTBOX )
		{
			CTListBox* pListBox = (CTListBox*)pCtrl;
			m_SoundOption.iEffectVolume = pListBox->GetValue();
			g_pSoundLIST->SetVolume( g_ClientStorage.GetEffectVolumeByIndex( m_SoundOption.iEffectVolume ) );		
		}
	}	

	g_ClientStorage.SetKeyboardOption( m_KeyboardOption );
	g_ClientStorage.SetVideoOption( m_VideoOption );
	g_ClientStorage.SetSoundOption( m_SoundOption );
	g_ClientStorage.SetPlayOption( m_PlayOption );
	g_ClientStorage.SetCommunityOption( m_CommunityOption );
	g_ClientStorage.SetShortCutOption( m_ShortCutOption );
	g_ClientStorage.Save();
}

void COptionDlg::GetCurrentOption()
{
	g_ClientStorage.GetKeyboardOption( m_KeyboardOption );
	g_ClientStorage.GetCommunityOption( m_CommunityOption );
	g_ClientStorage.GetVideoOption( m_VideoOption );
	g_ClientStorage.GetSoundOption( m_SoundOption );
	g_ClientStorage.GetPlayOption( m_PlayOption );
	g_ClientStorage.GetShortCutOption( m_ShortCutOption );

	CWinCtrl* pCtrl = NULL;	
	///커뮤니티			
	pCtrl = Find( IID_RADIOBOX_WHISPER );
	assert( pCtrl );
	if( pCtrl && pCtrl->GetControlType() == CTRL_CHECKBOX )
	{
		CTCheckBox* pCheckBox = (CTCheckBox*)pCtrl;
		pCheckBox->SetCheck( !m_CommunityOption.iWhisper );					
	}

	pCtrl = Find( IID_RADIOBOX_ADDFRIEND );
	assert( pCtrl );
	if( pCtrl && pCtrl->GetControlType() == CTRL_CHECKBOX )
	{
		CTCheckBox* pCheckBox = (CTCheckBox*)pCtrl;
		pCheckBox->SetCheck( !m_CommunityOption.iAddFriend );					
	}


	pCtrl = Find( IID_RADIOBOX_EXCHANGE );
	assert( pCtrl );
	if( pCtrl && pCtrl->GetControlType() == CTRL_CHECKBOX )
	{
		CTCheckBox* pCheckBox = (CTCheckBox*)pCtrl;
		pCheckBox->SetCheck( !m_CommunityOption.iExchange );					
	}

	pCtrl = Find( IID_RADIOBOX_PARTY );
	assert( pCtrl );
	if( pCtrl && pCtrl->GetControlType() == CTRL_CHECKBOX )
	{
		CTCheckBox* pCheckBox = (CTCheckBox*)pCtrl;
		pCheckBox->SetCheck( !m_CommunityOption.iParty );					
	}


	pCtrl = Find( IID_RADIOBOX_MESSANGER );
	assert( pCtrl );
	if( pCtrl && pCtrl->GetControlType() == CTRL_CHECKBOX )
	{
		CTCheckBox* pCheckBox = (CTCheckBox*)pCtrl;
		pCheckBox->SetCheck( !m_CommunityOption.iMessanger );					
	}

	/*pCtrl = Find( IID_RADIOBOX_PICKUP );
	assert( pCtrl );
	if( pCtrl && pCtrl->GetControlType() == CTRL_CHECKBOX )
	{
		CTCheckBox* pCheckBox = (CTCheckBox*)pCtrl;
		pCheckBox->SetCheck( !m_CommunityOption.iAutoPickup );					
	}*/


	///비디오
	int iResolutionIndex = GetCurrentResolutionIndex();
	pCtrl = Find( IID_COMBO_RESOLUTION );
	if( pCtrl && pCtrl->GetControlType() == CTRL_COMBOBOX )
	{
		CTComboBox* pComboBox = (CTComboBox*)pCtrl;
		pComboBox->SetSelectedItem( iResolutionIndex );
	}


	pCtrl = Find( IID_COMBO_FREQUENCY );
	if( pCtrl && pCtrl->GetControlType() == CTRL_COMBOBOX )
	{
		CTComboBox* pComboBox = (CTComboBox*)pCtrl;
		std::vector< int >::iterator iter;
		short iIndex = 0;
		short iSelectIndex = 0;
		pComboBox->ClearItem();
		for( iter = m_SupportedScreens[iResolutionIndex].m_Frequencys.begin();
			iter != m_SupportedScreens[iResolutionIndex].m_Frequencys.end();
			++iter, ++iIndex )
		{
			pComboBox->AddItem( CStr::Printf("%dHz",*iter ) );
			if( *iter == m_VideoOption.tResolution.iFrequency )
				iSelectIndex = iIndex;

		}
		pComboBox->SetSelectedItem( iSelectIndex );
	}


	/// 그리고 해당 해상도에 맞는 주파수를 넣어주고 저장된 주파수를 선택한다.
	pCtrl = Find( IID_SCROLLBAR_CAMERA );
	assert( pCtrl );
	if( pCtrl && pCtrl->GetControlType() == CTRL_SCROLLBAR )
	{
		CTScrollBar* pScrollBar = (CTScrollBar*)pCtrl;
		pScrollBar->SetValue( m_VideoOption.iCamera );
	}

	pCtrl = Find( IID_SCROLLBAR_PERFORMANCE );
	assert( pCtrl );
	if( pCtrl && pCtrl->GetControlType() == CTRL_SCROLLBAR )
	{
		CTScrollBar* pScrollBar = (CTScrollBar*)pCtrl;
		pScrollBar->SetValue( m_VideoOption.iPerformance );
	}





	///사운드 볼륨
	pCtrl = Find( IID_SCROLLBAR_BGM );
	if( pCtrl && pCtrl->GetControlType() == CTRL_SCROLLBAR )
	{
		CTScrollBar* pScrollBar = (CTScrollBar*)pCtrl;
		if( m_SoundOption.iBgmVolume < 0 || m_SoundOption.iBgmVolume >= MAX_BGM_VOLUME )
			m_SoundOption.iBgmVolume = DEFAULT_BGM_VOLUME ;

		pScrollBar->SetValue( m_SoundOption.iBgmVolume );
	}

	pCtrl = Find( IID_SCROLLBAR_EFFECTSOUND );
	if( pCtrl && pCtrl->GetControlType() == CTRL_SCROLLBAR )
	{
		CTScrollBar* pScrollBar = (CTScrollBar*)pCtrl;
		if( m_SoundOption.iEffectVolume < 0 || m_SoundOption.iEffectVolume >= MAX_EFFECT_VOLUME )
			m_SoundOption.iEffectVolume = DEFAULT_EFFECT_VOLUME ;

		pScrollBar->SetValue( m_SoundOption.iEffectVolume );
	}

	////Play Option
	pCtrl = Find( IID_RADIOBOX_CONTROL );
	assert( pCtrl );
	if( pCtrl && pCtrl->GetControlType() == CTRL_RADIOBOX )
	{
		CTRadioBox* pRadioBox = (CTRadioBox*)pCtrl;
		if( m_PlayOption.uiControlType )
			pRadioBox->SetPressedButton( IID_RADIOBUTTON_SEVENHEARTSCONTROL );
		else
			pRadioBox->SetPressedButton( IID_RADIOBUTTON_DEFAULTCONTROL );
	}
	
	///Keyboard
	{
		pCtrl = Find( IID_RADIOBOX_KEYBOARD );
		assert( pCtrl );
		if( pCtrl && pCtrl->GetControlType() == CTRL_RADIOBOX )
		{
			CTRadioBox* pRadioBox = (CTRadioBox*)pCtrl;
			if( m_KeyboardOption.iChattingMode )
				pRadioBox->SetPressedButton( IID_RADIOBTN_NORMALCHATTING );
			else
				pRadioBox->SetPressedButton( IID_RADIOBTN_ENTERCHATTING );
		}
	}


	///ShortCut View.
	{
		pCtrl = Find( IID_RADIOBOX_SHORTCUT );
		assert( pCtrl );
		if( pCtrl && pCtrl->GetControlType() == CTRL_RADIOBOX )
		{
			CTRadioBox* pRadioBox = (CTRadioBox*)pCtrl;
			if( m_ShortCutOption.iShortCutView )
				pRadioBox->SetPressedButton( IID_RADIOBUTTON_SHORTCUT_SHOW );
			else
				pRadioBox->SetPressedButton( IID_RADIOBUTTON_SHORTCUT_HIDE );
		}
	}
}


CTComboBox*	COptionDlg::GetComboBox( int ComboBoxID )
{
	CWinCtrl* pCtrl = Find( ComboBoxID );
	if( pCtrl && pCtrl->GetControlType() == CTRL_COMBOBOX )
		return (CTComboBox*)pCtrl;

	return NULL;
}

///Alt+Enter로 화면모드 전환시 처리할 사항
void COptionDlg::ChangeScreenModeByHotKey()
{
	CWinCtrl* pCtrl = Find( IID_TABBEDPANE );
	if( pCtrl && pCtrl->GetControlType() == CTRL_TABBEDPANE )
	{
		CTabbedPane* pPane = (CTabbedPane*)pCtrl;
		CJContainer* pContainer = pPane->GetTabContainer( IID_TABVIDEO );
		assert( pContainer );

		pCtrl = pContainer->Find( IID_RADIOBOX_FULLSCREEN );
		assert( pCtrl );
		if( pCtrl && pCtrl->GetControlType() == CTRL_RADIOBOX )
		{
			CTRadioBox* pRadioBox = (CTRadioBox*)pCtrl;
			if( g_pCApp->IsFullScreenMode() )
			{
				pRadioBox->SetPressedButton( IID_RADIOBUTTON_FULLSCREEN );
				m_VideoOption.iFullScreen = 1;
				g_ClientStorage.SetVideoFullScreen( 1 );
			}
			else
			{
				pRadioBox->SetPressedButton( IID_RADIOBUTTON_WINDOWMODE );
				m_VideoOption.iFullScreen = 0;
				g_ClientStorage.SetVideoFullScreen( 0 );
			}
		}
	}
}

void COptionDlg::AddSupportedScreen( int iWidth, int iHeight, int iDepth, int iFrequency )
{
	std::vector<SupportedScreen>::iterator iter;
	for( iter = m_SupportedScreens.begin(); iter != m_SupportedScreens.end(); ++iter )
	{
		if( iter->IsEqual( iWidth, iHeight, iDepth ) )
		{
			iter->m_Frequencys.push_back( iFrequency );
			return;
		}
	}

	SupportedScreen Screen;
	Screen.m_iWidth = iWidth;
	Screen.m_iHeight = iHeight;
	Screen.m_iDepth = iDepth;
	Screen.m_Frequencys.push_back( iFrequency );
	m_SupportedScreens.push_back( Screen );
}

int  COptionDlg::GetCurrentResolutionIndex()
{
	std::vector<SupportedScreen>::iterator iter;
	int iCount = 0;
	for( iter = m_SupportedScreens.begin(); iter != m_SupportedScreens.end(); ++iter, ++iCount )
	{
		if( iter->IsEqual( m_VideoOption.tResolution.iWidth,
			m_VideoOption.tResolution.iHeight,
			m_VideoOption.tResolution.iDepth ) )
		{
			return iCount;
		}
	}
	assert( 0 && "Invalid Save Resolution Value");
	return 0;
}


void COptionDlg::ChangeCommunityOption()
{	
	CWinCtrl * pCtrl = Find( IID_RADIOBOX_WHISPER );
	assert( pCtrl && pCtrl->GetControlType() == CTRL_CHECKBOX );
	if( pCtrl && pCtrl->GetControlType() == CTRL_CHECKBOX )
	{
		CTCheckBox* pCheckbox = (CTCheckBox*)pCtrl;
		m_CommunityOption.iWhisper = !pCheckbox->IsCheck();
	}

	pCtrl = Find( IID_RADIOBOX_ADDFRIEND );
	assert( pCtrl && pCtrl->GetControlType() == CTRL_CHECKBOX );
	if( pCtrl && pCtrl->GetControlType() == CTRL_CHECKBOX )
	{
		CTCheckBox* pCheckbox = (CTCheckBox*)pCtrl;
		m_CommunityOption.iAddFriend = !pCheckbox->IsCheck();
	}

	pCtrl = Find( IID_RADIOBOX_EXCHANGE );
	assert( pCtrl && pCtrl->GetControlType() == CTRL_CHECKBOX );
	if( pCtrl && pCtrl->GetControlType() == CTRL_CHECKBOX )
	{
		CTCheckBox* pCheckbox = (CTCheckBox*)pCtrl;
		m_CommunityOption.iExchange = !pCheckbox->IsCheck();			
	}

	pCtrl = Find( IID_RADIOBOX_PARTY );
	assert( pCtrl && pCtrl->GetControlType() == CTRL_CHECKBOX );
	if( pCtrl && pCtrl->GetControlType() == CTRL_CHECKBOX )
	{
		CTCheckBox* pCheckbox = (CTCheckBox*)pCtrl;
		m_CommunityOption.iParty = !pCheckbox->IsCheck();
	}

	pCtrl = Find( IID_RADIOBOX_MESSANGER );
	assert( pCtrl && pCtrl->GetControlType() == CTRL_CHECKBOX );
	if( pCtrl && pCtrl->GetControlType() == CTRL_CHECKBOX )
	{
		CTCheckBox* pCheckbox = (CTCheckBox*)pCtrl;
		m_CommunityOption.iMessanger = !pCheckbox->IsCheck();
	}

	pCtrl = Find( IID_RADIOBOX_PICKUP );
	assert( pCtrl && pCtrl->GetControlType() == CTRL_CHECKBOX );
	if( pCtrl && pCtrl->GetControlType() == CTRL_CHECKBOX )
	{
		CTCheckBox* pCheckbox = (CTCheckBox*)pCtrl;
		m_CommunityOption.iAutoPickup = !pCheckbox->IsCheck();
	}

	g_ClientStorage.SetCommunityOption( m_CommunityOption );

	pCtrl = Find( IID_RADIOBOX_SHORTCUT );
	assert( pCtrl && pCtrl->GetControlType() == CTRL_RADIOBOX );
	if( pCtrl && pCtrl->GetControlType() == CTRL_RADIOBOX )
	{
		CTRadioBox* pRadioBox = (CTRadioBox*)pCtrl;
		switch( pRadioBox->GetPressedButtonID() )
		{
		case IID_RADIOBUTTON_SHORTCUT_SHOW:
			{
				m_ShortCutOption.iShortCutView = 1;
				if( g_itMGR.IsDlgOpened(DLG_TYPE_QUICKBAR))
				{
					break;					
				}
				g_itMGR.OpenDialog( DLG_TYPE_QUICKBAR );
				g_itMGR.OpenDialog( DLG_TYPE_QUICKBAR_EXT );
			}			
			break;
		case IID_RADIOBUTTON_SHORTCUT_HIDE:
			{
				m_ShortCutOption.iShortCutView = 0;
				g_itMGR.CloseDialog( DLG_TYPE_QUICKBAR );
				g_itMGR.CloseDialog( DLG_TYPE_QUICKBAR_EXT );
			}
			
			break;
		default:
			break;
		}
	}    
}

void COptionDlg::InitVideoOption()
{

}

void COptionDlg::InitAudioOption()
{
	
}

void COptionDlg::InitPlayOption()
{
	
}

void COptionDlg::InitCommunityOption()
{
	
}
void COptionDlg::Draw()
{
	if( !IsVision() ) return;
	CTDialog::Draw();
	if( IID_TABKEYBOARD == m_iTab )
	{
		D3DXMATRIX mat;	
		D3DXMatrixTranslation( &mat, (float)m_sPosition.x, (float)m_sPosition.y,0.0f);
		::setTransformSprite( mat );
		
		
		::drawFont( g_GameDATA.m_hFONT[ FONT_NORMAL_BOLD ], true, 75,  97, g_dwBLACK,  STR_ENTERCHATTING_MODE );
		::drawFont( g_GameDATA.m_hFONT[ FONT_NORMAL_BOLD ], true, 75, 136, g_dwBLACK,  STR_NORMALCHATTING_MODE );
		
	}
}

t_OptionPlay & COptionDlg::GetOptionPlay()
{
	return m_PlayOption;	
}

void COptionDlg::SetOptionStatus(const char * pszOption, const char * pszStatus)
{
	////Play Option
	CJContainer* pContainer = NULL;
	CWinCtrl * pCtrl = NULL;
	CTabbedPane* pPane = NULL;

	if( strcmp(pszOption, "CTRL")==0 )
	{
		pCtrl = Find( IID_RADIOBOX_CONTROL );
		assert( pCtrl );

		if( pCtrl && pCtrl->GetControlType() == CTRL_RADIOBOX )
		{
			CTRadioBox* pRadioBox = (CTRadioBox*)pCtrl;

			if( strcmp(pszStatus, "SEVENHEARTSCONTROL")==0 )
			{
				pRadioBox->SetPressedButton( IID_RADIOBUTTON_SEVENHEARTSCONTROL );
				m_PlayOption.uiControlType = 1;
				g_UserInputSystem.ChangeUserInputStyle( SEVENHEARTS_USER_INPUT_STYLE );
			}
			else
			{
				pRadioBox->SetPressedButton( IID_RADIOBUTTON_DEFAULTCONTROL );
				m_PlayOption.uiControlType = 0;
				g_UserInputSystem.ChangeUserInputStyle( DEFAULT_USER_INPUT_STYLE );
			}
		}	
		g_ClientStorage.SetPlayOption( m_PlayOption );
	}
	else if( strcmp(pszOption, "CHAT")==0 )
	{
		CWinCtrl * pCtrl = Find( IID_RADIOBOX_KEYBOARD );
		assert( pCtrl );

		if( pCtrl && pCtrl->GetControlType() == CTRL_RADIOBOX )
		{
			CTRadioBox* pRadioBox = (CTRadioBox*)pCtrl;
			
			if( strcmp(pszStatus, "INPUTTYPE_NORMAL")==0 )
			{
				m_KeyboardOption.iChattingMode = 0;				
				it_SetKeyboardInputType( CTControlMgr::INPUTTYPE_NORMAL );				
				pRadioBox->SetPressedButton( IID_RADIOBTN_ENTERCHATTING );				
			}
			else
			{
				m_KeyboardOption.iChattingMode = 1;
				CTDialog* pDlg = g_itMGR.FindDlg( DLG_TYPE_CHAT );
				if( CTEditBox::s_pFocusEdit == NULL && pDlg != NULL  )
					pDlg->Show();
				it_SetKeyboardInputType( CTControlMgr::INPUTTYPE_AUTOENTER );
				pRadioBox->SetPressedButton( IID_RADIOBTN_NORMALCHATTING );
			}		
			g_ClientStorage.SetKeyboardOption( m_KeyboardOption );
		}
	}
	else if( strcmp(pszOption, "SOUND")==0 )
	{
		if( strcmp(pszStatus, "OFF")==0 )
		{			
			m_SoundOption.iSoundOff = 1;			
		}
		else
		{
			m_SoundOption.iSoundOff = 0;			
		}	

		g_ClientStorage.SetSoundOption( m_SoundOption );

		CMusicMgr::GetSingleton().SetVolume( g_ClientStorage.GetBgmVolumeByIndex( m_SoundOption.iBgmVolume ) );	
		g_pSoundLIST->SetVolume(g_ClientStorage.GetEffectVolumeByIndex( m_SoundOption.iEffectVolume ) );
	}
}

void COptionDlg::SetInterfacePos_After()
{
	CWinCtrl * pCtrl = NULL;

	if( pCtrl = Find( "MAIN_BG" ) )
	{
		pCtrl->SetSizeFit(true);
	}

	if( pCtrl = Find( "BGM_POS" ) )
	{
		((CTImage*)pCtrl)->SetText(LIST_STRING(764));//배경음
	}
	if( pCtrl = Find( "SOUND_POS" ) )
	{
		((CTImage*)pCtrl)->SetText(LIST_STRING(765));//효과음
	}
	if( pCtrl = Find( "RESOLUTION_POS" ) )
	{
		((CTImage*)pCtrl)->SetText(LIST_STRING(766));//해상도
	}
	if( pCtrl = Find( "REFLUSH_POS" ) )
	{
		((CTImage*)pCtrl)->SetText(LIST_STRING(767));//주사율
	}
	if( pCtrl = Find( "BRIGHT_POS" ) )
	{
		((CTImage*)pCtrl)->SetText(LIST_STRING(768));//화면밝기
	}
	if( pCtrl = Find( "SIGHT_POS" ) )
	{
		((CTImage*)pCtrl)->SetText(LIST_STRING(769));//시야
	}
	if( pCtrl = Find( "SPEED_POS" ) )
	{
		((CTImage*)pCtrl)->SetText(LIST_STRING(770));//성능우선
	}
	if( pCtrl = Find( "QUALITY_POS" ) )
	{
		((CTImage*)pCtrl)->SetText(LIST_STRING(771));//품질우선
	}
	if( pCtrl = Find( "CTRL_TYPE_POS" ) )
	{
		((CTImage*)pCtrl)->SetText(LIST_STRING(772));//컨트롤타입
	}
	if( pCtrl = Find( "CTRL_TYPE_00_POS" ) )
	{
		((CTImage*)pCtrl)->SetText(LIST_STRING(773));//"한번클릭:타겟팅"
	}
	if( pCtrl = Find( "CTRL_TYPE_01_POS" ) )
	{
		((CTImage*)pCtrl)->SetText(LIST_STRING(774));//"더블클릭:공격시작"
	}
	if( pCtrl = Find( "CTRL_TYPE_02_POS" ) )
	{
		((CTImage*)pCtrl)->SetText(LIST_STRING(775));//"한번클릭:타겟팅+공격"
	}
	if( pCtrl = Find( "CHAT_TYPE" ) )
	{
		((CTImage*)pCtrl)->SetText(LIST_STRING(776));//"채팅타입"
	}
	if( pCtrl = Find( "CHAT_ENTER_POS" ) )
	{
		((CTImage*)pCtrl)->SetText(LIST_STRING(777));//"엔터후 메세지입력"
	}
	if( pCtrl = Find( "CHAT_ALLWAYS_POS" ) )
	{
		((CTImage*)pCtrl)->SetText(LIST_STRING(778));//"상시 메세지입력"
	}
	if( pCtrl = Find( "WHISPER_POS" ) )
	{
		((CTImage*)pCtrl)->SetText(LIST_STRING(779));//"귓속말"
	}
	if( pCtrl = Find( "ADD_FRIEND_POS" ) )
	{
		((CTImage*)pCtrl)->SetText(LIST_STRING(780));//"친구추가"
	}
	if( pCtrl = Find( "TRADE_POS" ) )
	{
		((CTImage*)pCtrl)->SetText(LIST_STRING(781));//"거래신청"
	}
	if( pCtrl = Find( "PARTY_POS" ) )
	{
		((CTImage*)pCtrl)->SetText(LIST_STRING(782));//"파티신청"
	}if( pCtrl = Find( "MESSANGER_POS" ) )
	{
		((CTImage*)pCtrl)->SetText(LIST_STRING(783));//"메신져"
	}
	if( pCtrl = Find( "REJECT_00_POS" ) )
	{
		((CTImage*)pCtrl)->SetText(LIST_STRING(784));//"거부"
	}
	if( pCtrl = Find( "REJECT_01_POS" ) )
	{
		((CTImage*)pCtrl)->SetText(LIST_STRING(784));
	}
	if( pCtrl = Find( "REJECT_02_POS" ) )
	{
		((CTImage*)pCtrl)->SetText(LIST_STRING(784));
	}
	if( pCtrl = Find( "REJECT_03_POS" ) )
	{
		((CTImage*)pCtrl)->SetText(LIST_STRING(784));
	}
	if( pCtrl = Find( "REJECT_04_POS" ) )
	{
		((CTImage*)pCtrl)->SetText(LIST_STRING(784));
	}
	if( pCtrl = Find( "SHORTCUTDLG_POS" ) )
	{
		((CTImage*)pCtrl)->SetText(LIST_STRING(785));//단축키창
	}
	if( pCtrl = Find( "SHOW_POS" ) )
	{
		((CTImage*)pCtrl)->SetText(LIST_STRING(786));//보이기
	}
	if( pCtrl = Find( "HIDE_POS" ) )
	{
		((CTImage*)pCtrl)->SetText(LIST_STRING(787));//감추기
	}

	//문자열 추가
	if( pCtrl = Find( "VIDEO_TXT" ) )
	{
		//비디오
		pCtrl->SetText( LIST_STRING(820) );
	}
	if( pCtrl = Find( "AUDIO_TXT" ) )
	{
		//오디오
		pCtrl->SetText( LIST_STRING(821) );
	}
	if( pCtrl = Find( "PLAY_TXT" ) )
	{
		//플레이
		pCtrl->SetText( LIST_STRING(822) );
	}
	if( pCtrl = Find( IID_BTN_RESET ) )
	{
		//인터페이스 리셋
		pCtrl->SetText( LIST_STRING(823) );
	}
	if( pCtrl = Find( "ANTIALIASING_POS" ) )
	{
		((CTImage*)pCtrl)->SetText(LIST_STRING(480));
	}
	if( pCtrl = Find( "AUTO_PICKUP_POS" ) )
	{
		((CTImage*)pCtrl)->SetText(LIST_STRING(478));//
	}
	if( pCtrl = Find( "SELECT_PICKUP_POS" ) )
	{
		((CTImage*)pCtrl)->SetText(LIST_STRING(479));//
	}

	//옵션
	GetCaption()->SetString( LIST_STRING(26) );

}
