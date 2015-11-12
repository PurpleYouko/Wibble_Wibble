#include "stdafx.h"
#include ".\cgame.h"
#include "CGameState.h"
#include "CGameStateNull.h"
#include "CGameStateTitle.h"
#include "CGameStateLogin.h"
#include "GameStateWarning.h"
#include "CGameStateSelectSvr.h"
#include "CGameStatePrepareSelectAvatar.h"
#include "CGameStateSelectAvatar.h"
#include "CGameStateCreateAvatar.h"
#include "CGameStatePrepareMain.h"
#include "CGameStateMain.h"
#include "CGameStateWarp.h"
#include "CGameStateExitMain.h"
#include "CGameStateMoveMain.h"
#include "CGameStateLoginVirtual.h"
#include "GameStateMovePlanet.h"
#include "CGameStateReLogin.h"
#include "SqliteDB.h"

#include "../Country.h"
#include "../Debug.h"
#include "../Network/CNetwork.h"
#include "../IO_Event.h"
#include "../CClientStorage.h"
#include "../IO_Terrain.h"
#include "../CViewMsg.h"
#include "../CCamera.h"
#include "../interface/CTDrawImpl.h"
#include "../interface/CTFontImpl.h"
#include "../interface/CLoading.h"
#include "../interface/CTSoundImpl.h"
#include "../interface/CUIMediator.h"
#include "../interface/IO_ImageRes.h"
#include "../interface/it_mgr.h"
#include "../interface/CDragNDropMgr.h"
#include "../interface/dlgs/CClanDlg.h"
#include "../interface/dlgs/CPatDlg.h"
#include "../interface/dlgs/COptionDlg.h"
#include "../interface/dlgs/CharacterDlg.h"
#include "../interface/dlgs/QuickToolBar.h"
#include "../interface/dlgs/CSkillDlg.h"
#include "../interface/dlgs/SkillTreeDlg.h"
#include "../interface/dlgs/MakeDlg.h"
#include "../interface/dlgs/CItemDlg.h"
#include "../interface/dlgs/AvatarInfoDlg.h"
#include "../interface/cursor/ccursor.h"
#include "../interface/ExternalUI/ExternalUILobby.h"

#include "../Common/IO_Quest.h"
#include "../Common/IO_PAT.h"
#include "../gamedata/cmanufacture.h"
#include "../gamedata/CExchange.h"
#include "../gamedata/CSeparate.h"
#include "../gamedata/cdealdata.h"
#include "../gamedata/CPrivateStore.h"
#include "../gamedata/CClan.h"

#include "../gamecommon/CFilterword.h"
#include "../Game_FUNC.h"
#include "../Game.h"
#include "../Bullet.h"
#include "../CSkyDome.h"
#include "../util/Localizing.h"
#include <resource.h>

#include "SystemProcScript.h"
#include "../ObjectActionProcessor.h"

#include "../GameCommon/StringManager.h"

#include "../Util/Localizing.h"
#include "../Util/classTIME.h"


#include "../Sound/MusicMgr.h"


#include "ResourceMgr.h"
#include "TIme2.h"

#include "util/ijl.h"	

#include "../JCommandState.h"
#include "GameCommon/LngTbl.h"

#include "GameProc/SkillCommandDelay.h"

#include "Tutorial/TutorialEventManager.h"


#include "../sfx/SFXManager.h"
#include "../sfx/SFXFont.h"

#include "../misc/gameutil.h"

#include "GameProc/UseItemDelay.h"
#include "../interface/command/UICommand.h"
#include "../Util/SystemUtil.h"


bool g_bDontDeleteCapFile = false;

CGame::CGame(void)
{
	m_GameStates[ GS_NULL ] = new CGameStateNull( GS_NULL );

	for( int i = GS_TITLE; i < GS_MAX; ++i )
		m_GameStates[ i ]			= NULL;

	m_pCurrState = m_GameStates[ GS_NULL ];

//	m_iCurrentCursorType = 4;
	m_iCheckFrame		 = 0;
	g_bDontDeleteCapFile = false;


	m_ptCurrMouse.x = 0;
	m_ptCurrMouse.y = 0;
	m_iRepairMode		= REPAIR_NONE;
	m_iMakeSocketMode	= MAKESOCKET_NONE;
	m_bLoadedBasicData2 = false;

	m_iJoinChannelNo    = 0;
	m_uiSelectedServerID = 0;
	m_bAutoRun			= false;

	m_UseActiveXConnect = false;
}

CGame::~CGame(void)
{
	for( int i = 0 ; i < GS_MAX; ++i)
	{
		if( m_GameStates[i] )
		{
			delete m_GameStates[i];
			m_GameStates[i] = NULL;
		}
	}
}

CGame& CGame::GetInstance()
{
	static CGame s_Instance;
	return s_Instance;
}

CGameState* CGame::MakeState( int iState )
{
	CGameState* pState = NULL;
	switch( iState )
	{
	case GS_TITLE:
		pState = new CGameStateTitle( GS_TITLE );
		break;
	case GS_LOGIN:
		pState = new CGameStateLogin( GS_LOGIN );
		break;
	case GS_WARNING:
		pState = new CGameStateWarning( GS_WARNING );
		break;
	case GS_SELECTSVR:
		pState = new CGameStateSelectSvr( GS_SELECTSVR );
		break;
	case GS_PREPARESELECTAVATAR:
		pState = new CGameStatePrepareSelectAvatar( GS_PREPARESELECTAVATAR );
		break;
	case GS_SELECTAVATAR:
		pState = new CGameStateSelectAvatar( GS_SELECTAVATAR );
		break;
	case GS_LOGINVIRTUAL:
		pState = new CGameStateLoginVirtual( GS_LOGINVIRTUAL );
		break;
	case GS_CREATEAVATAR:
		pState = new CGameStateCreateAvatar( GS_CREATEAVATAR );
		break;
	case GS_PREPAREMAIN:
		pState = new CGameStatePrepareMain( GS_PREPAREMAIN );
		break;
	case GS_MAIN:
		pState = new CGameStateMain( GS_MAIN );
		break;
	case GS_WARP:
		pState = new CGameStateWarp( GS_WARP );
		break;
	case GS_EXITMAIN:
		pState = new CGameStateExitMain( GS_EXITMAIN );
		break;
	case GS_MOVEMAIN:
		pState = new CGameStateMoveMain( GS_MOVEMAIN );
		break;

	case GS_MOVEPLANET:
		pState = new CGameStateMovePlanet( GS_MOVEPLANET );
		break;

	case GS_RELOGIN:
		pState = new CGameStateReLogin( GS_RELOGIN );
		break;

	default:
		break;
	}
	assert( pState != NULL );
	return pState;
}

void CGame::ChangeState( int newState )
{
	if( m_GameStates[ newState ] == NULL )
		m_GameStates[ newState ] = MakeState( newState );

	m_pCurrState->Leave( newState );
	int oldState = m_pCurrState->GetStateID();
	
	m_pCurrState = m_GameStates[ newState ];
	m_pCurrState->Enter( oldState );
}

void CGame::GameLoop()
{
	if( Init() != 0 )
		return ;	

	ChangeState( GS_TITLE );

	do 
	{
		bool bLostFocus = g_pCApp->GetMessage ();

		g_GameDATA.Update();

		
		g_pNet->Proc ();
		ProcInput();
		
		m_pCurrState->Update( bLostFocus );

		m_MouseTargetEffect.Proc();

		ProcCommand();

	} while( !g_pCApp->IsExitGame() );

	Exit();
}

int CGame::Init()
{
	/// Get current code page

	//PY let's try initializing the LUA way up here
	//LUA initialized here. InitSystem is a boolean so why aren't we using it that way?
	CSystemProcScript::GetSingleton().InitSystem();	//Load SystemFunc.lua functions (SystemProcScript.cpp)
	ClientLog(LOG_DEBUG,"LUA Initialized" );


	( CLocalizing::GetSingleton() ).GetCurrentCodePage();
	if( !(CStringManager::GetSingleton()).LoadTables( ( CLocalizing::GetSingleton() ).GetCurrentCharSet() ) ) //Loading STLs
		return false;

#ifdef _USE_BG
	g_CrashMsg.assign( STR_CRASH_MSG );
#endif

	::activateLog (true);

#ifdef	_DEBUG
	CLOG::SetLogMODE ( LOG_NORMAL,	LOG_SCR );
	CLOG::SetLogMODE ( LOG_NORMAL,	LOG_DBGOUT );
#else
	CLOG::SetLogMODE ( 0,			LOG_SCR );
	CLOG::SetLogMODE ( 0,			LOG_DBGOUT );
#endif
	CLOG::SetLogMODE ( 0,	LOG_FILE );

	LogString (LOG_DEBUG, "tagBasicINFO: %d, tagBasicAbility: %d, tagGrowAbility: %d, tagSkillAbility: %d, tagQuestData: %d, CInventory: %d = %d \n",
			sizeof(tagBasicINFO),
			sizeof(tagBasicAbility),
			sizeof(tagGrowAbility),
			sizeof(tagSkillAbility),
			sizeof(tagQuestData),
			sizeof(CInventory),
			sizeof(tagBasicINFO) + sizeof(tagBasicAbility) + sizeof(tagGrowAbility) + sizeof(tagSkillAbility) + sizeof(tagQuestData) + sizeof(CInventory) );

	//g_GameDATA.m_nScrWidth  = getScreenWidth();
    //g_GameDATA.m_nScrHeight = getScreenHeight();

	g_GameDATA.m_bWireMode = false;

	if ( g_pNet == NULL ) 
	{
		g_pCApp->ErrorBOX( "init socket failed", "error", MB_OK);
		return 1;
	}


	g_pEventLIST = new CEventLIST;
	g_pBltMGR  = CBulletMANAGER::Instance();
	g_pViewMSG = CViewMSG::Instance ();
	g_pObjMGR = CObjectMANAGER::Instance ();

	g_pObjMGR->ReserverViewObjectList();

	g_pTerrain = CTERRAIN::Instance ();


	it_Init( g_pCApp->GetHWND(), &g_DrawImpl,&g_SoundImpl ,&g_FontImpl );

	//--------------------------------------------------------------------------------
	/// If you get a new current code page will tell you to always TGameCtrl
	//--------------------------------------------------------------------------------
	CLocalizing::GetSingleton().GetCurrentCodePage();
	it_SetCodePage( CLocalizing::GetSingleton().GetCurrentCodePageNO() );

	SIZE sizeScreen = { g_pCApp->GetWIDTH(), g_pCApp->GetHEIGHT() };
	CTIme::GetInstance().Initialize( sizeScreen );
	//--------------------------------------------------------------------------------


	//--------------------------------------------------------------------------------
	/// for processing userdata registed engine
	//--------------------------------------------------------------------------------
	CObjectActionProcessor::GetSingleton().MakeChain();


	//--------------------------------------------------------------------------------
	// Music init
	//--------------------------------------------------------------------------------
	CMusicMgr& musicMgr = CMusicMgr::GetSingleton();		
	if( musicMgr.Init( OGG_MUSIC_TYPE ) )
	{				
		//musicMgr.Play ( "Sound\\BGM\\Prologue.mp3" );
		musicMgr.Play ( "Sound\\BGM\\Prologue.ogg" );
		musicMgr.SetVolume( g_ClientStorage.GetBgmVolumeByIndex( g_ClientStorage.GetBgmVolumeIndex() ));		
	}else
	{
		ClientLog(LOG_DEBUG,"There is a problem with the sound card.");
		//MessageBox( g_pCApp->GetHWND(), "There is a problem with the sound card.", "ERROR", MB_OK );
	}

	//--------------------------------------------------------------------------------
	/// sound
	//--------------------------------------------------------------------------------
	g_pSoundLIST->SetVolume( -10000 );





	//--------------------------------------------------------------------------------
	/// Loading class
	MakeFont();	
//	LoadUserCursor();
	
//	SetUserCursor(CURSOR_DEFAULT);
#ifdef	__VIRTUAL_SERVER
	g_pCApp->SetStatus( AS_MAIN_GAME );
#endif	
	g_pCApp->Show ();


	g_TblString.Load				( "3DDATA\\STB\\LIST_STRING.STB",		true, false );
//	if( CreateAllStringTables( &g_TblString ) == false )
//		return false;
	//ClientLog(LOG_DEBUG,"LIST_STRINGS.STB Complete" );

	//PY: this section temporaily moved down a few lines to see if it helps.
	//LUA initialized here
	//CSystemProcScript::GetSingleton().InitSystem();	//Load SystemFunc.lua functions (SystemProcScript.cpp)
	//ClientLog(LOG_DEBUG,"InitSystem OK" );
	
	
	CCursor::GetInstance().Initialize();            //Sets up cursors
	//ClientLog(LOG_DEBUG,"Initialize OK" );
	CCursor::GetInstance().SetCursorType( CCursor::CURSOR_DEFAULT );
	//ClientLog(LOG_DEBUG,"Set Cursor Type OK" );


	/// skill dealy
	CSkillCommandDelay::GetSingleton().Init();
	ClientLog(LOG_DEBUG,"Skill command delay init OK" );

	//LUA initialized here. InitSystem is a boolean so why aren't we using it that way?
	//CSystemProcScript::GetSingleton().InitSystem();	//Load SystemFunc.lua functions (SystemProcScript.cpp)
	//ClientLog(LOG_DEBUG,"InitSystem OK" );

	/// Tutorial Event
	
	// PY: Loading this function crashes the client when using VFS_NORMAL mode. Maybe it doesn't load the lua properly?
	if( CTutorialEventManager::GetSingleton().Init() == false )
	{
		MessageBox( NULL, "Load tutorial failed", "ERROR", MB_OK );	
		ClientLog(LOG_DEBUG,"Load tutorial failed" );
		return 0;
	}
	ClientLog(LOG_DEBUG,"Tutorial loaded" );
	

	//CSqliteDB::GetInstance()
	if( CSqliteDB::GetInstance().Open( "RoseOnline.db") == 0 )
	{
		const char* sqlCreateMemoTable = "create table memo ( name string, sender string, content string, time int )";
		CSqliteDB::GetInstance().Exec( sqlCreateMemoTable, NULL );
	}

	ClientLog(LOG_DEBUG,"get instance OK" );
	CSFXFont* sfx_font = new CSFXFont;
	CSFXManager::GetSingleton().AddSFX( sfx_font );
	ClientLog(LOG_DEBUG,"Fonts OK" );


	///The client needed to create a directory at run time.
	CreateDirectory( CClanMarkUserDefined::ClanMarkFileDirectory.c_str(), NULL );
	CreateDirectory( "Chat", NULL );

#ifdef _DEBUG
	m_ClientFileVersion.GetVersionInfo( "TRose.exe" );
#else
	m_ClientFileVersion.GetVersionInfo( "TRose.exe" );
#endif

	m_ClientFileVersion.m_strFileVersion = CStr::Printf("%d.%d.%d.%d",
		m_ClientFileVersion.m_iMajorVersion, 
		m_ClientFileVersion.m_MinorVersion, 
		m_ClientFileVersion.m_BuildNumber, 
		m_ClientFileVersion.m_RevisionNumber );

	m_ClientFileVersion.m_strFileVersion.append("_DE");

#ifdef __ITEM_MAX
	m_ClientFileVersion.m_strFileVersion.append("_IM");
#endif
	ClientLog(LOG_DEBUG,"Initialization complete" );
	return 0;
}

void CGame::Exit()
{
	CSFXManager::GetSingleton().Clear();

	CSqliteDB::GetInstance().Close();

	CCursor::GetInstance().Finalize();

	m_MouseTargetEffect.Clear();

	CSkyDOME::Free ();

	g_itMGR.Free();

	CImageResManager::GetSingletonPtr()->ReleaseResources();
	CResourceMgr::GetInstance()->UnLoadResource(IMAGE_RES_EXUI);
	
	CResourceMgr::GetInstance()->UnLoadResource(IMAGE_RES_NEWUI);


	CResourceMgr::GetInstance()->UnLoadResource(IMAGE_RES_ITEM);

	for( int i =0; i < MAX_FONT ; i++ )
	{
		if( g_GameDATA.m_hFONT[ i ] != NULL ) 
		{
			::unloadFont( g_GameDATA.m_hFONT[ i ] );
			g_GameDATA.m_hFONT[ i ] = NULL;
		}
	}	

	
	//----------------------------------------------------------------------------------------
	/// clear music resource
	//----------------------------------------------------------------------------------------
	if( CMusicMgr::GetSingleton().bIsReady() )
		CMusicMgr::GetSingleton().Clear();


	
	g_pTerrain->Destroy ();
	g_pObjMGR->Destroy ();
	g_pViewMSG->Destroy ();

	Free_BasicDATA ();
	it_Destroy();
	SAFE_DELETE( g_pEventLIST );
	CObjectMANAGER::Destroy();

	CSystemProcScript::GetSingleton().ClearSystem();

	CTIme::GetInstance().Finalize();

	CSkillCommandDelay::GetSingleton().Release();

	/// Tutorial event
	CTutorialEventManager::GetSingleton().Release();

}
void CGame::Load_NewVersionData()
{
	/// NPC FACE TABLE( Local variables are automatically turned off )
	STBDATA	stbNPCFACE;
	stbNPCFACE.Load2("3DData\\STB\\LIST_NPCFACE.STB", false, false );

	if( stbNPCFACE.m_nDataCnt < 1 || stbNPCFACE.m_nColCnt < 2 )
	{
		_RPT0( _CRT_WARN,"LIST_NPCFACE STB's schima mismatch\n");
		return;
	}

	std::string directory;
	
	if( stbNPCFACE.m_ppVALUE[0][1].GetSTR() )
		directory = stbNPCFACE.m_ppVALUE[0][1].GetSTR();

	std::string file;
	for( int row = 1 ; row < stbNPCFACE.m_nDataCnt; row++ )
	{
		file = directory;
		if( stbNPCFACE.m_ppVALUE[row][1].GetSTR() )
		{
			file.append( stbNPCFACE.m_ppVALUE[row][1].GetSTR() );
			CImageResManager::GetSingleton().Add_NpcFaceFileInfo( 
				stbNPCFACE.m_ppVALUE[row][0].GetSHORT(),
				file.c_str() );
		}
	}

	//stbNPCFACE
	//if ( nCol >= m_nColCnt || nRow >= m_nDataCnt )
	//	return NULL;

	//
	//
	//
	//return m_ppVALUE[ nRow ][ nCol ].GetSTR();		




}
bool CGame::Load_BasicDATA2()
{
	if( m_bLoadedBasicData2 ) 
		return true;

	ClientLog(LOG_DEBUG,"Loading BasicDATA2");
	m_bLoadedBasicData2 = true;

	DWORD dwStartTime = timeGetTime();
	g_SkillList.LoadSkillTable	( "3DData\\STB\\LIST_SKILL.STB" );

	///NEW VERSION Loading data for
	//if( IsApplyNewVersion() )
	if( IsApplyNewVersion() )
	{
		Load_NewVersionData();
	}

	/// NPC table
	g_TblNPC.Load2				( "3DDATA\\STB\\LIST_NPC.STB",		true, false, _LOAD_NORMAL );
	g_MOBandNPC.Load_MOBorNPC	( "3DDATA\\NPC\\LIST_NPC.CHR");
	g_QuestList.LoadQuestTable	(  "3DDATA\\STB\\LIST_QUEST.STB", "3DDATA\\STB\\LIST_QuestDATA.STB" );


	g_TblFACEITEM.Load	( "3DDATA\\STB\\LIST_FACEITEM.STB",	true, true	);	
////g_TblHELMET.Load	( "3DDATA\\STB\\LIST_Cap.STB",		true, true  );=>Load_BasicDATA()This, select the table you need to make avatars
	g_TblARMOR.Load		( "3DDATA\\STB\\LIST_Body.STB",		true, true  );
	g_TblGAUNTLET.Load	( "3DDATA\\STB\\LIST_Arms.STB",		true, true  );
	g_TblBOOTS.Load		( "3DDATA\\STB\\LIST_Foot.STB",		true, true  );

	g_TblBACKITEM.Load  ( "3DDATA\\STB\\LIST_BACK.STB",		true, true	);	
	g_TblJEWELITEM.Load ( "3DDATA\\STB\\LIST_JEWEL.STB",	true, true	);	
	g_TblWEAPON.Load	( "3DDATA\\STB\\LIST_Weapon.STB",	true, true  );
	g_TblSUBWPN.Load	( "3DDATA\\STB\\LIST_SUBWPN.STB",	true, true  );
	g_TblUSEITEM.Load   ( "3DDATA\\STB\\LIST_USEITEM.STB",	true, true	);	
	g_TblGEMITEM.Load	( "3DDATA\\STB\\LIST_JEMITEM.STB",	true, true	);	
	g_TblNATUAL.Load	( "3DDATA\\STB\\LIST_NATURAL.STB",	true, true	);
	g_TblQUESTITEM.Load ( "3DDATA\\STB\\LIST_QUESTITEM.STB",	true, true	);	
	if ( !g_PatITEM.LoadPatITEM( "3DDATA\\STB\\LIST_PAT.STB") )
		return false;

	g_pTblSTBs[ ITEM_TYPE_FACE_ITEM ] = &g_TblFACEITEM;
	g_pTblSTBs[ ITEM_TYPE_HELMET	] = &g_TblHELMET;
	g_pTblSTBs[ ITEM_TYPE_ARMOR		] =	&g_TblARMOR;
	g_pTblSTBs[ ITEM_TYPE_GAUNTLET	] = &g_TblGAUNTLET;
	g_pTblSTBs[ ITEM_TYPE_BOOTS		] = &g_TblBOOTS;
	g_pTblSTBs[ ITEM_TYPE_KNAPSACK	] = &g_TblBACKITEM;
	g_pTblSTBs[ ITEM_TYPE_JEWEL		] = &g_TblJEWELITEM;
	g_pTblSTBs[ ITEM_TYPE_WEAPON	] = &g_TblWEAPON;
	g_pTblSTBs[ ITEM_TYPE_SUBWPN	] = &g_TblSUBWPN;
	g_pTblSTBs[ ITEM_TYPE_USE		] = &g_TblUSEITEM;
	g_pTblSTBs[ ITEM_TYPE_GEM		] = &g_TblGEMITEM;
	g_pTblSTBs[ ITEM_TYPE_NATURAL	] = &g_TblNATUAL;
	g_pTblSTBs[ ITEM_TYPE_QUEST     ] = &g_TblQUESTITEM;
	g_pTblSTBs[ ITEM_TYPE_RIDE_PART ] = &g_PatITEM.m_ItemDATA;

	_RPT1(_CRT_WARN,"Loading Time Basic Data2 %d \n", timeGetTime() - dwStartTime );

	return true;
}
/// 2004 /3/ 29:nAvy - Client, in order to reduce execution time Load_BasicDATA2 first separated Loading
/// CGameLoadSelectAvatarState Loading from the rest.
/// * Now Skill, Weapon The Loading 5 seconds each time to move on.
/// * Be careful not to load or STB 2 Load the STB is already back in the Load stop yours.
/// * Prototype Pattern Use shall we??
bool CGame::Load_BasicDATA()
{
	//---------------------------------------------------------------------
	/// string table load
	//---------------------------------------------------------------------


	DWORD dwStartTime = timeGetTime();

	CObjITEM::SetMotion( "3DData\\Motion\\item_ani.ZMO" );

	
	int iCurrentLang = CStringManager::GetSingleton().GetLanguageIDInGame( CLocalizing::GetSingleton().GetCurrentCharSet() );

	g_AI_LIST.Load		( NULL, "3DDATA\\STB\\FILE_AI.STB", "3DDATA\\AI\\AI_S.stb", iCurrentLang );
	g_pEffectLIST = new CEffectLIST( "3ddata\\stb\\FILE_EFFECT.stb" );	// shader The set should be loaded.

	if ( !g_DATA.Load3DDATA () )		// shader The set should be loaded.
		return false;

	g_TblHAIR.Load		( "3DDATA\\STB\\LIST_Hair.STB",		true, false );
	g_TblFACE.Load		( "3DDATA\\STB\\LIST_Face.STB",		true, false );
	g_TblEFFECT.Load	( "3DDATA\\STB\\LIST_EFFECT.STB",	false,false );
#ifdef __VIRTUAL_SERVER
	//06. 08. 09 - Kim, Joo - Hyun: Item is not used to load the drop Climb!
	//__VIRTUAL_SERVER - Activate a virtual server, the server is built on the old jijigo bokgi made ??for himself before the preprocessor climb.
	g_TblDropITEM.Load	( "3DDATA\\STB\\ITEM_DROP(NEW).STB",			false, false);
#endif
	// *-------------------------------------------------------------------* //
	g_TblPRODUCT.Load	( "3DDATA\\STB\\LIST_PRODUCT.STB",	true, false );///2004 / 2 /4 :nAvy Crystal PRODUCT.STB => LIST_PRODUCT.STB
#if defined ( _NEWBREAK )
	g_TblBREAK.Load		( "3DDATA\\STB\\LIST_Break.STB",	true, false  );///05/11/03 : Loading more relevant STB Kim, Joo - Hyun degradation.
#endif
	g_TblStore.Load2	( "3DDATA\\STB\\LIST_SELL.STB",	false, false);
	g_TblAniTYPE.Load	( "3DDATA\\STB\\TYPE_MOTION.STB",	false, false);
	g_TblEVENT.Load2	( "3DDATA\\STB\\LIST_EVENT.STB",	false, true );
	g_TblSKY.Load2		( "3DDATA\\STB\\LIST_Sky.STB",		false, false);
	g_TblWARP.Load2		( "3DDATA\\STB\\WARP.STB",			true, false);
	g_TblZONE.Load2		( "3DDATA\\STB\\LIST_ZONE.STB",		true, false );
	
	g_TblHitSound.Load	( "3DDATA\\STB\\LIST_HITSOUND.STB",	false, false );	
	g_TblAVATAR.Load	( "3DDATA\\STB\\INIT_AVATAR.STB",	false, false );
	g_TblRangeSet.Load	( "3DDATA\\STB\\RangeSet.STB",		false, false );

	g_TblSTATE.Load2	( "3DData\\STB\\LIST_STATUS.STB",	false, false );	
	g_TblUnion.Load2	( "3DDATA\\STB\\LIST_UNION.STB",	false, false );
	g_TblClass.Load2	( "3DDATA\\STB\\LIST_CLASS.STB",	false, false );
	
	g_TblStepSound.Load	( "3DDATA\\STB\\LIST_STEPSOUND.STB");
	g_TblItemGRADE.Load	( "3DDATA\\STB\\LIST_GRADE.STB");		
	g_TblHELMET.Load	( "3DDATA\\STB\\LIST_Cap.STB",		true, true  );

	g_TblFoulWord.Load2	( "3DDATA\\STB\\BADWORDS.STB",		false, false );
	g_TblBadNames.Load2	( "3DDATA\\STB\\BADNAMES.STB",		false, false );

	if( g_MotionFILE.Load	( "3DDATA\\STB\\FILE_MOTION.stb" ) == false )
		return false;

	if( g_LngTBL.Load ( iCurrentLang ) == false )
		return false;
	ClientLog(LOG_DEBUG,"Finished loading STBs in LoadBasicData" );

	MakeBadWordTable();
	ClientLog(LOG_DEBUG,"Bad Word table ok" );
	InitScript ();
	ClientLog(LOG_DEBUG,"InitScript ok" );
	g_UserInputSystem.Init();
	ClientLog(LOG_DEBUG,"g_UserInputSystem.Init ok" );

	_RPT1(_CRT_WARN,"Loading Time Basic Data %d \n", timeGetTime() - dwStartTime );
	return true;
}


bool CGame::Load_DataNotUseThread()///Should not read the thread by using the Load Data and Method: The first order of need.
{
	g_pCamera = CCamera::Instance ();

	::doScript( "scripts/load.lua" );

	g_pCamera->Init ( findNode( "motion_camera" ) );
	g_GameDATA.m_nSightRange = 30;
	//g_ClientStorage.ApplyCameraOption( g_TblCamera.m_nDataCnt - 1 );

	g_GameDATA.m_hShader_nolit_skin		= ::findNode( "shader_nolit_skin" );	
	g_GameDATA.m_hShader_nolit			= ::findNode( "shader_nolit" );		
	g_GameDATA.m_hShader_terrain		= ::findNode( "shader_terrain" );	
	g_GameDATA.m_hShader_terrain_Rough	= ::findNode( "shader_terrain_rough" );	
	g_GameDATA.m_hShader_lightmap		= ::findNode( "shader_lightmap_nolit" );	
	g_GameDATA.m_hShader_ocean			= ::findNode( "shader_ocean" );
	g_GameDATA.m_hShader_specular		= ::findNode( "shader_specular" );	
	g_GameDATA.m_hShader_specular_skin	= ::findNode( "shader_specular_skin" );
	g_GameDATA.m_hShader_lit_skin		= ::findNode( "shader_lit_skin" );	
	g_GameDATA.m_hShader_lit			= ::findNode( "shader_lit" );		
	g_GameDATA.m_hShader_sky			= ::findNode( "shader_sky" );
		
	g_GameDATA.m_hLight					= ::findNode( "light_01" );
	g_GameDATA.m_hCharacterLight		= ::findNode( "light_02" );
	g_GameDATA.m_hCharacterLight2		= ::findNode( "light_03" );

	g_GameDATA.m_TrailTexture.Set( "3DDATA\\Effect\\trail.dds" );

	if( g_UIMed.InitMediator() == false )
	{
		g_pCApp->ErrorBOX( "Load UIMED failed", "ERROR" );
		return false;	
	}

	CreateScreenSpaceText();
	
	return true;
}


#include "Util/FileVersionInfo.h"
void CGame::MakeFont()
{
	//CFileVersionInfo versionInfo;
	//versionInfo.GetVersionInfo( "d_client.exe" );

	for( int i =0; i < MAX_FONT ; i++ )
	{		
		g_GameDATA.m_hFONT[ i ] = NULL;
	}

	int iCharSet = CLocalizing::GetSingleton().GetCurrentCharSet();
	const char* szFont = CStringManager::GetSingleton().GetFontNameByCharSet( iCharSet );	

	//const char* szFont = "Verdana";

	int iFontSize = 9;
	if( iCharSet == 0 )
		iFontSize = 11;

	/// Normal
	g_GameDATA.m_hFONT[ FONT_NORMAL ] = ::loadFont( "Normal_Font",
													szFont,
													CLocalizing::GetSingleton().GetCurrentCharSet(),
													iFontSize,
													0,
													0,
													255,
													255,
													255,
													255 );

	/// Normal bold
	g_GameDATA.m_hFONT[ FONT_NORMAL_BOLD ] = ::loadFont( "Normal_Font_B",
													szFont,
													CLocalizing::GetSingleton().GetCurrentCharSet(),
													iFontSize,
													1,
													0,
													255,
													255,
													255,
													255 );


	/// Small
	iFontSize = 8;
	if( iCharSet == 0 )
		iFontSize = 10;

	g_GameDATA.m_hFONT[ FONT_SMALL ] = ::loadFont( 	"Small_Font",
													szFont,
													CLocalizing::GetSingleton().GetCurrentCharSet(),
													iFontSize,
													0,
													0,
													255,
													255,
													255,
													255 );

	/// Small bold
	g_GameDATA.m_hFONT[ FONT_SMALL_BOLD ] = ::loadFont( 	"Small_Font_B",
													szFont,
													CLocalizing::GetSingleton().GetCurrentCharSet(),
													iFontSize,
													1,
													0,
													255,
													255,
													255,
													255 );	



	/// Large
	g_GameDATA.m_hFONT[ FONT_LARGE ] = ::loadFont( 	"Large_Font",
													szFont,
													CLocalizing::GetSingleton().GetCurrentCharSet(),
													12,
													0,
													0,
													255,
													255,
													255,
													255 );

	/// Large bold
	g_GameDATA.m_hFONT[ FONT_LARGE_BOLD ] = ::loadFont( 	"Large_Font_B",
													szFont,
													CLocalizing::GetSingleton().GetCurrentCharSet(),
													12,
													1,
													0,
													255,
													255,
													255,
													255 );


	
	g_GameDATA.m_hFONT[ FONT_OUTLINE_18_BOLD ] = ::loadFontOutline( "FONT_OUTLINE_18",
													szFont,
													CLocalizing::GetSingleton().GetCurrentCharSet(),
													18,
													1,
													0,
													1,
													D3DCOLOR_ARGB( 255,255,255,255 ),
													D3DCOLOR_ARGB( 255,0,0,0 ) );

	g_GameDATA.m_hFONT[ FONT_OUTLINE_14_BOLD ] = ::loadFontOutline( 	"FONT_OUTLINE_14",
													szFont,
													CLocalizing::GetSingleton().GetCurrentCharSet(),
													14,
													1,
													0,
													1,
													D3DCOLOR_ARGB( 255,255,255,255 ),
													D3DCOLOR_ARGB( 255,0,0,0 ) );



	g_GameDATA.m_hFONT[ FONT_OUTLINE_24_BOLD ] = ::loadFontOutline( "FONT_OUTLINE_24",
													szFont,
													CLocalizing::GetSingleton().GetCurrentCharSet(),
													24,
													1,
													0,
													1,
													D3DCOLOR_ARGB( 255,255,255,255 ),
													D3DCOLOR_ARGB( 255,0,0,0 ) );

	g_GameDATA.m_hFONT[ FONT_OUTLINE_16_BOLD ] = ::loadFontOutline( 	"FONT_OUTLINE_16",
													szFont,
													CLocalizing::GetSingleton().GetCurrentCharSet(),
													16,
													1,
													0,
													1,
													D3DCOLOR_ARGB( 255,255,255,255 ),
													D3DCOLOR_ARGB( 255,0,0,0 ) );

	iFontSize = 9;
	if(  0 == iCharSet )
		iFontSize = 11;
	g_GameDATA.m_hFONT[ FONT_NORMAL_OUTLINE ] = loadFontOutline( "Normal_Font_Outline",
													szFont,
													CLocalizing::GetSingleton().GetCurrentCharSet(),
													iFontSize,
													0,
													0,
													1,
													D3DCOLOR_ARGB( 255,255,255,255 ),
													D3DCOLOR_ARGB( 255,0,0,0 ) );


	g_GameDATA.m_hFONT[ FONT_OUTLINE_11_BOLD ] = loadFontOutline( "FONT_OUTLINE_11_BOLD",
													szFont,
													CLocalizing::GetSingleton().GetCurrentCharSet(),
													11,
													1,
													0,
													1,
													D3DCOLOR_ARGB( 255,255,255,255 ),
													D3DCOLOR_ARGB( 255,0,0,0 ) );

	g_GameDATA.m_hFONT[ FONT_12_BOLD ]       = ::loadFont( 	"Font_12_B",
													szFont,
													CLocalizing::GetSingleton().GetCurrentCharSet(),
													12,
													1,
													0,
													255,
													255,
													255,
													255 );

	g_GameDATA.m_hFONT[ FONT_14_BOLD ]       = ::loadFont( 	"Font_14_B",
													szFont,
													CLocalizing::GetSingleton().GetCurrentCharSet(),
													14,
													1,
													0,
													255,
													255,
													255,
													255 );
}

void CGame::Free_BasicDATA ()
{
	DeleteScreenSpaceText();

	CObjITEM::ClearMotion( );

	g_UIMed.FreeResource();
	CImageResManager::GetSingletonPtr()->ReleaseResources();

	FreeScript ();

	g_TblBadNames.Free();
	g_TblFoulWord.Free();
	g_TblNPC.Free ();
	g_TblWEAPON.Free ();
	g_TblHELMET.Free ();
	g_TblBOOTS.Free ();
	g_TblGAUNTLET.Free ();
	g_TblARMOR.Free ();
	g_TblFACE.Free ();
	g_TblHAIR.Free ();

	// *----------------------* //
#ifdef __VIRTUAL_SERVER
	g_TblDropITEM.Free ();
#endif
	g_TblPRODUCT.Free();
	g_TblBREAK.Free ();
	g_TblNATUAL.Free();
	g_TblFACEITEM.Free();
	g_TblUSEITEM.Free();
	g_TblBACKITEM.Free();
	g_TblGEMITEM.Free();
	g_TblJEWELITEM.Free();
	g_TblStore.Free ();
	// *----------------------* //
	g_TblClass.Free();
	g_TblUnion.Free();
	g_TblSKY.Free ();
	g_TblEVENT.Free ();
	g_TblWARP.Free ();
	g_TblAniTYPE.Free ();
	g_TblZONE.Free ();

	g_MatFILE.Free ();
	g_MeshFILE.Free ();
	g_MotionFILE.Free ();
	
	g_MOBandNPC.Free ();
	g_PatITEM.Free();

	g_DATA.Free3DDATA ();

	SAFE_DELETE( g_pEffectLIST );

	g_pCamera->Destroy ();

	g_UserInputSystem.Clear();
}

bool CGame::AddWndMsgQ( UINT uiMsg, WPARAM wParam, LPARAM lParam )
{
	if( uiMsg == WM_MOUSEMOVE )
	{
		m_ptPrevMouse	= m_ptCurrMouse;

		m_ptCurrMouse.x = LOWORD( lParam );
		m_ptCurrMouse.y = HIWORD( lParam );
		
		//POINT ptWindow = m_ptCurrMouse;
		//ClientToScreen( g_pCApp->GetHWND(), &ptWindow );
		//setCursorPosition ( m_hDxCursor[m_iCurrentCursorType], ptWindow.x, ptWindow.y );

	}
	m_pCurrState->ProcWndMsgInstant( uiMsg, wParam, lParam );
	return m_WndMsgQ.AddMsgToQ( uiMsg, wParam, lParam );
}




void CGame::CreateSelectedAvata()
{

	strcpy( g_MsgBuf, m_strAvataName.c_str() );
	g_pAVATAR = (CObjUSER*)g_pObjMGR->New_UserCHAR (0, g_MsgBuf );

//	g_pAVATAR->SetAllPARTS( m_SelectedAvataInfo.m_nPartItemIDX );
	g_pAVATAR->SetAllPARTS( m_SelectedAvataInfo.m_PartITEM );

	/// Add Observer to CItemSlot<-
	CItemSlot* pItemSlot = g_pAVATAR->GetItemSlot();
	assert( pItemSlot );

	CTDialog* pDlg = g_itMGR.FindDlg( DLG_TYPE_ITEM );
	if( pDlg )
	{
		CItemDlg* pItemDlg = ( CItemDlg* ) pDlg;
		pItemSlot->AddObserver( pItemDlg );
	}

	/*pDlg = g_itMGR.FindDlg( DLG_TYPE_PAT );
	if( pDlg )
	{
		CPatDlg* pPatDlg = ( CPatDlg* ) pDlg;
		pItemSlot->AddObserver( pPatDlg );
	}*/
	/*
	pDlg = g_itMGR.FindDlg( DLG_TYPE_CHAR );
	if( pDlg )
	{
		CCharacterDLG* pCharDlg = ( CCharacterDLG*) pDlg;
		pItemSlot->AddObserver( pCharDlg );
	}*/

	pDlg = g_itMGR.FindDlg( DLG_TYPE_QUICKBAR );
	if( pDlg )
	{
		CQuickBAR* pQuickBar = ( CQuickBAR*) pDlg;
		pItemSlot->AddObserver( pQuickBar );
	}

	pDlg = g_itMGR.FindDlg( DLG_TYPE_QUICKBAR_EXT );
	if( pDlg )
	{
		CQuickBAR* pQuickBar = ( CQuickBAR*) pDlg;
		pItemSlot->AddObserver( pQuickBar );
	}
	

	pDlg = g_itMGR.FindDlg( DLG_TYPE_INFO );
	if( pDlg )
	{
		CAvatarInfoDlg* pInfoDlg = (CAvatarInfoDlg*)pDlg;
		pItemSlot->AddObserver( pInfoDlg );
	}
	
	pItemSlot->AddObserver( &(CManufacture::GetInstance()) );
	pItemSlot->AddObserver( &(CDragNDropMgr::GetInstance()) );
	pItemSlot->AddObserver( &(CDealData::GetInstance()) );
	pItemSlot->AddObserver( &(CExchange::GetInstance()) );
	pItemSlot->AddObserver( &(CPrivateStore::GetInstance() ));
	pItemSlot->AddObserver( &(CSeparate::GetInstance() ) );


	pDlg = g_itMGR.FindDlg( DLG_TYPE_MAKE );
	CMakeDLG* pMakeDlg = ( CMakeDLG*)pDlg;
	CManufacture::GetInstance().AddObserver( pMakeDlg );
	///Add Observer to CSkillSlot<-

	CSkillSlot* pSkillSlot = g_pAVATAR->GetSkillSlot();

	pDlg = g_itMGR.FindDlg( DLG_TYPE_SKILL );
	CSkillDLG* pSkillDlg = (CSkillDLG*)pDlg;
	pSkillSlot->AddObserver( pSkillDlg );

	g_pAVATAR->m_nReviveZoneNO = m_SelectedAvataInfo.m_nReviveZoneNO;
	::CopyMemory ( &g_pAVATAR->m_BasicINFO,		&m_SelectedAvataInfo.m_BasicINFO,		sizeof(tagBasicINFO)	);
	::CopyMemory ( &g_pAVATAR->m_BasicAbility,	&m_SelectedAvataInfo.m_BasicAbility,	sizeof(tagBasicAbility)	);
	::CopyMemory ( &g_pAVATAR->m_BasicAbility,	&m_SelectedAvataInfo.m_BasicAbility,	sizeof(tagBasicAbility)	);
	::CopyMemory ( &g_pAVATAR->m_GrowAbility,	&m_SelectedAvataInfo.m_GrowAbility,		sizeof(tagGrowAbility)	);
	
	::CopyMemory ( &g_pAVATAR->m_Skills,		&m_SelectedAvataInfo.m_Skill,			sizeof(tagSkillAbility)	);
	
	::CopyMemory ( &g_pAVATAR->m_Quests,		&m_QuestData.m_Quests,					sizeof(tagQuestData)	);
	::CopyMemory ( &g_pAVATAR->m_WishLIST,		&m_QuestData.m_WishLIST,				sizeof(tagWishLIST)		);
	
	
	memcpy( g_pAVATAR->m_Inventory.m_ItemLIST,
			m_SelectedAvataINV.m_INV.m_ItemLIST, 			
			sizeof( g_pAVATAR->m_Inventory.m_ItemLIST ) );	

	for( int i = 0 ; i < MAX_WISH_ITEMS; ++i )
		CPrivateStore::GetInstance().AddItemWishList( g_pAVATAR->m_WishLIST.m_WishITEM[i], false ,i );

	// Initialization of a passive skill stats ...
	g_pAVATAR->InitPassiveSkill ();

	//g_pAVATAR->m_Inventory = m_SelectedAvataINV.m_INV;
	g_pAVATAR->Set_SEX( m_SelectedAvataInfo.m_btCharRACE );
	g_pAVATAR->UpdateAbility ();


	/// skill slot setting : UpdateAbility Go back

	// Seukilchang after setting the career.
	{
		CTCmdSetSkillDlg Cmd( g_pAVATAR->Get_JOB() );
		Cmd.Exec(NULL);
	}


	// Put seukilchang stored in DB skills.
	SetSkill();

	g_ClientStorage.SetSelectAvatarName(m_strAvataName.c_str());
	g_ClientStorage.LoadAvatarData();

	///Add Item to ItemSlot<-
	g_pAVATAR->m_Inventory.Clear();
	for( int iInvenIdx = 0; iInvenIdx < INVENTORY_TOTAL_SIZE ; ++iInvenIdx )
	{
		if( m_SelectedAvataINV.m_INV.m_ItemLIST[iInvenIdx].IsEmpty() )
			continue;

		g_pAVATAR->Set_ITEM( iInvenIdx, m_SelectedAvataINV.m_INV.m_ItemLIST[iInvenIdx] );

	}

	g_pAVATAR->m_Inventory.MakeItemIndexList();

	/// The effect of the client and used to synchronize the motion parameters and Setting : 2004 / 2 / 23 
	/// CObjAVT :: m_ShotData motion is synchronized with other avatars without actually
	/// Only CObjUSER CInventory::m_ItemSHOTSuch as using two synchronized with the motion of the server and the client.
	g_pAVATAR->SetShotData(SHOT_TYPE_ARROW , g_pAVATAR->CUserDATA::m_Inventory.m_ItemSHOT[SHOT_TYPE_ARROW].GetItemNO() );
	g_pAVATAR->SetShotData(SHOT_TYPE_BULLET, g_pAVATAR->CUserDATA::m_Inventory.m_ItemSHOT[SHOT_TYPE_BULLET].GetItemNO() );
	g_pAVATAR->SetShotData(SHOT_TYPE_THROW , g_pAVATAR->CUserDATA::m_Inventory.m_ItemSHOT[SHOT_TYPE_THROW].GetItemNO() );


	//----------------------------------------------------------------------------------------------------	
	/// Updated information set + CQuickDlg slot slot (Inventory and Skill must receive information first.)
	///::CopyMemory ( &g_pAVATAR->m_HotICONS,		&m_SelectedAvataInfo.m_HotICONS,			sizeof(CHotICONS)	);	
	//----------------------------------------------------------------------------------------------------	
	(g_pAVATAR->GetHotIconSlot())->SetHotIcons( &m_SelectedAvataInfo.m_HotICONS );


	g_pAVATAR->SetCur_MONEY ( m_SelectedAvataINV.m_INV.m_i64Money );
	g_pAVATAR->SetUniqueTag( m_SelectedAvataInfo.m_dwUniqueTAG );


	g_pAVATAR->UpdateAbility ();

	char* pszName = NULL;
	SYSTEMTIME	stCurrTime;
	DWORD		dwABS = 0;
	for( int i = 0 ; i < MAX_MAINTAIN_STATUS; ++i )
	{
		if( dwABS = g_pAVATAR->m_GrowAbility.m_STATUS[i].m_dwExpiredSEC )
		{
#ifdef __EXPERIENCE_ITEM // Put a dummy value in distinguishing the type of Springerville.
			if(i > 3)
			{
				pszName =  CStringManager::GetSingleton().GetAbility( g_pAVATAR->m_GrowAbility.m_STATUS[i].m_nDummy + i + AT_MAINTAIN_ABILITY );
			}
			else
			{
				pszName =  CStringManager::GetSingleton().GetAbility( i + AT_MAINTAIN_ABILITY );
			}			
#else
			pszName =  CStringManager::GetSingleton().GetAbility( i + AT_MAINTAIN_ABILITY );
#endif
			classTIME::AbsSecondToSystem ( dwABS, stCurrTime );

			g_itMGR.AppendChatMsg( CStr::Printf(STR_ITEM_TIME_OUT, pszName, stCurrTime.wYear, stCurrTime.wMonth, stCurrTime.wDay, stCurrTime.wHour, stCurrTime.wMinute ), IT_MGR::CHAT_TYPE_NOTICE);
		} 

	}

	g_pAVATAR->Set_RareITEM_Glow();
	g_pAVATAR->CreateGradeEffect();
	
	//Skills stored on the server.

	//The current time.
	DWORD dwCurrentTime = classTIME::GetCurrentAbsSecond();
	int iDelayType = 0;
	float iDelayTick = 0;
	int iSkill = 0;

	for( int i = 0; i < MAX_LEARNED_SKILL_CNT; i++ )
	{
		iSkill = g_pAVATAR->m_Skills.m_nSkillINDEX[ i ];
		if( iSkill != 0 )
		{
			iDelayType = SKILL_RELOAD_TYPE( iSkill);
			iDelayTick = g_SkillList.GetDelayTickCount( iSkill );

			if(iDelayType && m_SelectedAvataInfo.m_dwCoolTime[iDelayType] > 0)
			{
				//m_SelectedAvataInfo.m_dwCoolTime[iDelayType] : iDelayTypeTime with the skill.

				//Remains a skill delay.
				if( ( iDelayTick) > (dwCurrentTime - m_SelectedAvataInfo.m_dwCoolTime[iDelayType])*1000 ) 
				{
					g_UseSkillDelay.SetUseItemDelay( iDelayType, (dwCurrentTime - m_SelectedAvataInfo.m_dwCoolTime[iDelayType])*1000 );
					g_CurSkillDelayTick.SetUseItemDelay( iDelayType, iDelayTick );
				}
			}
		}
	}

	/// Apply the skill cooldown drive : 2005/7/27 - nAvy
	/// If there is no cooldown g_pAVATAR->m_GrowAbility.m_dwPatCoolTIME == 0

	if( g_SkillList.Get_Cart_SkillIndex()>0 )
	{
		g_SoloSkillDelayTick.SetUseItemDelay( g_SkillList.Get_Cart_SkillIndex(), (float)g_pAVATAR->GetCur_PatCoolTIME() );
	}
	

}


void CGame::SetSkill()
{	
	for( int i = 0; i < MAX_LEARNED_SKILL_CNT; i++ )
	{
		if( g_pAVATAR->m_Skills.m_nSkillINDEX[ i ] != 0 )
		{
			g_pAVATAR->AddNewSkill( g_pAVATAR->m_Skills.m_nSkillINDEX[ i ], i );
		}
		
	}
}

void CGame::ProcWndMsg( UINT uiMsg, WPARAM wParam, LPARAM lParam )
{
	switch( uiMsg )
	{
	case WM_CLOSE:
		if( m_pCurrState->GetStateID() == GS_MAIN )
			g_itMGR.OpenDialog( DLG_TYPE_SYSTEM );
		else
			g_pCApp->SetExitGame();
		break;
	case WM_USER_CLOSE_MSGBOX:
		if( m_pCurrState->GetStateID() == GS_LOGIN || m_pCurrState->GetStateID() == GS_SELECTAVATAR )
			g_EUILobby.HideMsgBox();

		break;
	case WM_USER_SERVER_DISCONNECTED:
		if( !g_pCApp->IsExitGame() )
			m_pCurrState->ServerDisconnected();
		break;
	case WM_USER_WORLDSERVER_DISCONNECTED:
		if( !g_pCApp->IsExitGame() )
			m_pCurrState->WorldServerDisconnected();
		break;
	default:
		break;
	}
}
int CGame::GetCurrStateID()
{
	return m_pCurrState->GetStateID();
}

/// BMP file to clear the screen capture at the end Would ... Would
//bool g_bDontDeleteCapFile = false;

bool CGame::IJL_BMP2JPEG (const char *szBMPFile, char *szJPGFile)
{
	FILE* fpBMP;

	fpBMP = fopen( szBMPFile, "rb" );

	JPEG_CORE_PROPERTIES jcprops;
	ijlInit( &jcprops );

	BITMAPFILEHEADER bmfh;
	BITMAPINFOHEADER bmih;

	fread( &bmfh,	1,	sizeof(BITMAPFILEHEADER),	fpBMP);
	fread( &bmih,	1,	sizeof(BITMAPINFOHEADER),	fpBMP);

	long DIBLineSize = (bmih.biWidth * 4 ) / 4 * 4;

	jcprops.DIBChannels = 4;
	jcprops.DIBColor	= IJL_RGB;
	jcprops.DIBHeight	= -bmih.biHeight;
	jcprops.DIBWidth	= bmih.biWidth;
	jcprops.DIBPadBytes = IJL_DIB_PAD_BYTES( jcprops.DIBWidth, jcprops.DIBChannels );

	jcprops.JPGHeight = bmih.biHeight;
	jcprops.JPGWidth  = bmih.biWidth;
	jcprops.jquality  = 100;

	jcprops.JPGFile = szJPGFile;

	unsigned char* DIBBuffer = new unsigned char[ DIBLineSize * bmih.biHeight ];
	fread( DIBBuffer,	1,	DIBLineSize * bmih.biHeight,	fpBMP );

	BYTE btBlue;
	for (int iP=0; iP<DIBLineSize * bmih.biHeight; iP+=4) {
		btBlue = DIBBuffer[ iP ];
		DIBBuffer[ iP ] = DIBBuffer[ iP+2 ];
		DIBBuffer[ iP+2 ]  = btBlue;
	}
	
	jcprops.DIBBytes = DIBBuffer;

	ijlWrite (&jcprops, IJL_JFILE_WRITEWHOLEIMAGE );
	ijlFree( &jcprops );

	delete[] DIBBuffer;

	fclose( fpBMP );

if( !g_bDontDeleteCapFile )
	::DeleteFile( szBMPFile );

	return true;
}

/*
Direct3D9: (ERROR) :RenderTargets are not lockable unless application specifies TRUE for the Lockable parameter
 at CreateRenderTarget or D3DPRESENTFLAG_LOCKABLE_BACKBUFFER at CreateDevice, 
 CreateAdditionalSwapChain or Reset. Lockable render targets incur a performance cost on some graphics hardware.
*/

void CGame::ScreenCAPTURE ()
{
	LPDIRECT3DSURFACE9 pSUR;
	
	if ( D3D_OK == reinterpret_cast<LPD3DDEVICE>( getDevice () )->GetRenderTarget (0, &pSUR) ) {
		D3DSURFACE_DESC sDesc;
		if ( D3D_OK != pSUR->GetDesc( &sDesc ) )
			return;
		
		::CreateDirectory( "capture", NULL );

		WORD	wDumpNo=0;
		char   *pStr;
		char *pTempBMP;
		
		do 
		{
			wDumpNo++;
			pStr = CStr::Printf ("capture\\screen%03d.jpg", wDumpNo);
			pTempBMP = CStr::Printf ("capture\\screen%03d.bmp", wDumpNo);
		} while ( CUtil::Is_FileExist( pStr ) && wDumpNo <= 999 ) ;		


		
		if ( D3D_OK == D3DXSaveSurfaceToFile( pTempBMP, D3DXIFF_BMP, pSUR, NULL,	NULL) ) 
		{
			LogString (LOG_NORMAL, "Capture Screen %s", pStr);		

			IJL_BMP2JPEG( pTempBMP, pStr );
		} else {
			LogString (LOG_NORMAL, "Capture Screen Failed");
		}
		

		SAFE_RELEASE(pSUR);
	}
}


void CGame::ScreenCAPTURE (SYSTEMTIME SystemTime)
{
	if ( ::beginScene() )
	{
		::beginSprite( D3DXSPRITE_ALPHABLEND );	
		// Text Put in place

		SYSTEMTIME SystemTime;
		GetLocalTime( &SystemTime );			

		::drawFontf( g_GameDATA.m_hFONT[ FONT_NORMAL_OUTLINE ], true, 820,720, D3DCOLOR_XRGB( 255, 255, 255),		
			"%s \n%s \n%s %d, %d \n%d.%d.%d %d:%d:%d",
			this->GetServerInfo().strServerName.c_str(),	// The server name

			g_pAVATAR ? g_pAVATAR->Get_NAME() : "UNKNOWN",							// Carrick Name

			ZONE_NAME( g_pTerrain->GetZoneNO() ),			// Location.
			g_pAVATAR ? (int)g_pAVATAR->Get_CurPOS().x/1000  : 0,
			g_pAVATAR ? (int)g_pAVATAR->Get_CurPOS().y/1000  : 0,

			SystemTime.wYear,								// Time.
			SystemTime.wMonth,
			SystemTime.wDay,
			SystemTime.wHour,
			SystemTime.wMinute,
			SystemTime.wSecond);

		::endSprite();
		::endScene();		
	}

	LPDIRECT3DSURFACE9 pSUR;

	if ( D3D_OK == reinterpret_cast<LPD3DDEVICE>( getDevice () )->GetRenderTarget (0, &pSUR) ) {
		D3DSURFACE_DESC sDesc;
		if ( D3D_OK != pSUR->GetDesc( &sDesc ) )
			return;

		::CreateDirectory( "capture", NULL );

		WORD	wDumpNo=0;
		char   *pStr;
		char *pTempBMP;

		do 
		{
			wDumpNo++;
			pStr = CStr::Printf ("capture\\screen%03d.jpg", wDumpNo);
			pTempBMP = CStr::Printf ("capture\\screen%03d.bmp", wDumpNo);
		} while ( CUtil::Is_FileExist( pStr ) && wDumpNo <= 999 ) ;		



		/*D3DLOCKED_RECT LockRect;
		pSUR->LockRect( &LockRect, NULL, D3DLOCK_READONLY );

		if (IJL_EncodeToJPEGFile ( pStr, sDesc.Width, sDesc.Height, (WORD*)LockRect.pBits, sDesc.Format ) ) 
		{
		LogString (LOG_NORMAL, "Capture Screen %s", pStr);		
		}*/


		if ( D3D_OK == D3DXSaveSurfaceToFile( pTempBMP, D3DXIFF_BMP, pSUR, NULL,	NULL) ) 
		{
			LogString (LOG_NORMAL, "Capture Screen %s", pStr);		

			IJL_BMP2JPEG( pTempBMP, pStr );
		} else {
			LogString (LOG_NORMAL, "Capture Screen Failed");
		}


		SAFE_RELEASE(pSUR);
	}	
}


void CGame::ProcInput()
{
	tagWNDMSG Msg;
	while( CGame::GetInstance().m_WndMsgQ.GetMouseMsgFromQ( Msg ) )
	{
		if( !m_pCurrState->ProcMouseInput( Msg.uiMsg, Msg.wParam, Msg.lParam ) )
		{

		}
	}
	while( CGame::GetInstance().m_WndMsgQ.GetKeyboardMsgFromQ( Msg ) )
	{
		switch( Msg.uiMsg )
		{
		case WM_SYSKEYDOWN:
			{
				unsigned int oemScan = int( Msg.lParam & (0xff << 16) ) >> 16; 
				UINT vk = MapVirtualKey( oemScan, 1 );
				if( vk == VK_F4 )
				{
                    // Remove the use of ALT + F4 shortcut.
					//g_pCApp->SetExitGame();
					//return;
				}
			}
			break;
		}

		m_pCurrState->ProcKeyboardInput( Msg.uiMsg, Msg.wParam, Msg.lParam );
		
		switch( Msg.uiMsg )
		{
		case WM_KEYUP:
			if ( Msg.wParam == VK_SNAPSHOT )
			{
#ifdef _SCREENCAPTURE
				g_pNet->Send_cli_SCREEN_SHOT_TIME();
#else
				ScreenCAPTURE ();
#endif
			}
			
			break;
		case WM_SYSKEYDOWN :
			switch( Msg.wParam )
			{
			case '0':
				CCursor::GetInstance().ChangeNextState();
				//CGame::GetInstance().ToggleUserMouseFlag();
				//CGame::GetInstance().SetUserCursor( CGame::CURSOR_DEFAULT );
				break;
			case '/':
				g_bDontDeleteCapFile = !g_bDontDeleteCapFile;
				break;
			case VK_RETURN:
				{
					ChangeScreenMode();
					CTDialog* pDlg = g_itMGR.FindDlg( DLG_TYPE_OPTION );
					if( pDlg )
					{
						COptionDlg* pOptionDlg = (COptionDlg*)pDlg;
						pOptionDlg->ChangeScreenModeByHotKey();
					}
					break;
				}
			default:
				break;
			}
		default:
			break;
		}

	}
}
void	CGame::ChangeScreenMode()
{
	t_OptionResolution Resolution = g_ClientStorage.GetResolution();
	int iWidth  = Resolution.iWidth;
	int iHeight = Resolution.iHeight;
	int iDepth  = Resolution.iDepth;


	if( g_pCApp->IsFullScreenMode() )
		g_pCApp->SetFullscreenMode( false );
	else
		g_pCApp->SetFullscreenMode( true );

	g_pCApp->ResizeWindowByClientSize( iWidth, iHeight, iDepth , true);

	CCursor::GetInstance().ReloadCursor();
}

short	CGame::Get_XPos ()
{ 
	return (short)m_ptCurrMouse.x; 
}

short	CGame::Get_YPos ()
{ 
	return (short)m_ptCurrMouse.y; 
}

void	CGame::BeginRepair( int iType )
{
	CCursor::GetInstance().SetCursorType( CCursor::CURSOR_REPAIR );
	m_iRepairMode = iType;
	g_itMGR.OpenDialog( DLG_TYPE_ITEM, false );
	g_itMGR.BeginRepair();
}

void	CGame::EndRepair()
{
	m_iRepairMode = REPAIR_NONE;
	g_itMGR.EndRepair();
}

int CGame::GetRepairMode()
{
	return m_iRepairMode;
}

void CGame::ResetRepairMode()
{
	m_iRepairMode = REPAIR_NONE;
}


void CGame::SetUsingRepairItemInvenIdx( int iItemInvenIdx )
{
	m_iUsingRepairItemInvenIdx = iItemInvenIdx;
}
int CGame::GetUsingRepairItemInvenIdx()
{
	return m_iUsingRepairItemInvenIdx;
}
void CGame::SetRepairNpcSvrIdx( WORD wNpcSvrIdx )
{
	m_wRepairNpc = wNpcSvrIdx;
}
WORD CGame::GetRepairNpcSvrIdx()
{
	return m_wRepairNpc;
}

void CGame::BeginMakeSocket(int iType)
{
#if defined( _NEWINTERFACEKJH )
	CCursor::GetInstance().SetCursorType( CCursor::CURSOR_MAKESOCKET );
#else
	CCursor::GetInstance().SetCursorType( CCursor::CURSOR_REPAIR );
#endif
	m_iMakeSocketMode = iType;
	g_itMGR.OpenDialog( DLG_TYPE_ITEM, false );
	g_itMGR.BeginMakeSocket();
}

void CGame::EndMakeSocket()
{
	m_iMakeSocketMode = MAKESOCKET_NONE;
	g_itMGR.EndMakeSocket();
}


int CGame::GetMakeSocketMode()
{
	return m_iMakeSocketMode;
}

void CGame::ResetMakeSocketMode()
{
	m_iMakeSocketMode = MAKESOCKET_NONE;
}

void CGame::SetUsingMakeSocketItemInvenIdx( int iItemInvenIdx )
{
	m_iUsingMakeSocketItemInvenIdx = iItemInvenIdx;
}


int CGame::GetUsingMakeSocketItemInvenIdx()
{
	return m_iUsingMakeSocketItemInvenIdx;
}




DWORD CGame::GetRight()
{
	return m_dwRight;
}

void CGame::SetRight( DWORD dwRight )
{
	m_dwRight = dwRight;
}

void CGame::MakeBadWordTable()
{
	CFilterWord& refFilter = CFilterWord::GetInstance();

	if( g_TblFoulWord.m_ppVALUE[0][1].GetSTR() && strlen( g_TblFoulWord.m_ppVALUE[0][1].GetSTR() ) >= 1 )
		refFilter.SetChangeWord( g_TblFoulWord.m_ppVALUE[0][1].GetSTR() );

	for( int i = 1 ; i < g_TblFoulWord.m_nDataCnt; ++i )
		refFilter.AddWord( g_TblFoulWord.m_ppVALUE[i][0].GetSTR(), g_TblFoulWord.m_ppVALUE[i][1].GetSTR() );
	for( int i = 0 ; i < g_TblBadNames.m_nDataCnt; ++i )
		refFilter.AddName( g_TblBadNames.m_ppVALUE[i][0].GetSTR() );

}

void CGame::SetAppraisalNpcSvrIdx( WORD wNpcSvrIdx )
{
	m_wAppraisalNpc = wNpcSvrIdx;
}

WORD CGame::GetAppraisalNpcSvrIdx()
{
	return m_wAppraisalNpc;
}

void CGame::BeginAppraisal()
{
	CCursor::GetInstance().SetCursorType( CCursor::CURSOR_APPRAISAL );
	m_iAppraisalMode = APPRAISAL_NPC;
	g_itMGR.BeginAppraisal();
}

void CGame::EndAppraisal()
{
	m_iAppraisalMode = APPRAISAL_NONE;
	g_itMGR.EndAppraisal();

}

__int64 CGame::GetAppraisalCost()
{
	return m_i64AppraisalCost;
}

void	CGame::SetAppraisalCost( __int64 i64Cost )
{
	m_i64AppraisalCost = i64Cost;
}

void	CGame::AcceptedConnectLoginSvr()
{
	m_pCurrState->AcceptedConnectLoginSvr();
}

void CGame::UpdateCurrentState()
{
	m_pCurrState->Update( g_pCApp->GetMessage () );
}


void CGame::SetJoinChannelNo( int iNo )
{
	m_iJoinChannelNo = iNo;
}
int CGame::GetJoinChannelNo()
{
	return m_iJoinChannelNo;
}

bool CGame::AddTCommand( CTCommand* pCmd )
{
	assert( pCmd );
	if( pCmd )
	{
		m_CommandQ.Push_Back( pCmd );
		return true;
	}
	return false;
}

void CGame::ProcCommand()
{
	CTCommand* pCmd = m_CommandQ.GetCommand();
	if( pCmd )
	{
		if( pCmd->Exec( NULL ) )
			delete pCmd;
		else
		{
			if( pCmd->IsWaitExecSucceed() )
				m_CommandQ.Push_Front( pCmd );
			else
				m_CommandQ.Push_Back( pCmd );
		}
	}
}

void CGame::SetPayType( WORD paytype )
{
	m_paytype = paytype;
}


WORD CGame::GetPayType()
{
	
//Honggeun: Japan Billing modification
#ifdef __JAPAN_BILL
	return m_paytype;
#else
	//EXT 
	if( m_paytype & PLAY_FLAG_EXTRA_CHAR 
		&& m_paytype & PLAY_FLAG_EXTRA_STOCK  )
	{
		return PAY_PLATINUM;
	}
	else if( m_paytype & PLAY_FLAG_KOREA_DEFAULT )
	{
		return PAY_PREMIUM;
	}
	//NO EXT
	else
	{		
		switch( m_paytype )
		{
		case BILLING_MSG_PAY_FU:// FU	Log in with your user ID indicates free.
			return PAY_FREE;
		case BILLING_MSG_PAY_FQ:// FQ	Quantify the individual user logged-in user (premium) indicates.
		case BILLING_MSG_PAY_FA:// FA	Log in with your personal cum user (s) indicates.
			return PAY_PREMIUM;
		case BILLING_MSG_PAY_GU:// GU	Games logged in user is the user (platinum) indicates.
		case BILLING_MSG_PAY_GQ:// GQ	(Platinum), but I try to login in Games Games of the billing period (time) has expired, the log indicates that the individual determination.
		case BILLING_MSG_PAY_IQ:// IQ	(Platinum) Games already signed in, but I try to login using both the IP number and log it indicates that the individual determination.
		case BILLING_MSG_PAY_FAP:// FAP	User logged in user a private sperm (Platinum) shows that it
			return PAY_PLATINUM;	

		default:
			break;
		}
	}
	return PAY_FREE;
#endif
	
}

WORD CGame::GetDetailPayType()
{
	return m_paytype;
}


void CGame::SetExpireTime( const char* time )
{
	assert( time );
	if( time )
	{
		m_time_setexpiretime = g_GameDATA.GetGameTime();
		m_expireTime = time;
	}
}

void CGame::SetExpireTimeExt( int type, const char* time )
{	
	assert( time );

	if( time && type>0 && type<PAY_FLAG_JP_SIZE )
	{
		m_time_setexpiretimeExt[type] = g_GameDATA.GetGameTime();
		m_expireTimeExt[type] = time;
	}
}

const std::string& CGame::GetExpireTime()
{
	return m_expireTime;
}

const std::string& CGame::GetExpireTimeExt( int type )
{
	if(type>0 && type<PAY_FLAG_JP_SIZE)
	{
		return m_expireTimeExt[type];		
	}
	return "";
}

DWORD CGame::GetSetExpireTime()
{
	return m_time_setexpiretime;
}

DWORD CGame::GetSetExpireTimeExt(int type)
{
	if(type>0 && type<PAY_FLAG_JP_SIZE)
	{
		return m_time_setexpiretimeExt[type];
	}
	return 0;
}

/// 1: date, 2: minutes, 0: no
void CGame::SetExpireTimeType( int type )
{
	m_expiretime_type = type;
}

void CGame::SetExpireTimeTypeExt( int type, int time )
{
	if(type>0 && type<PAY_FLAG_JP_SIZE)
	{
		m_expiretime_typeExt[type] = type;
	}
}


int CGame::GetExpireTimeType()
{
	return m_expiretime_type;
}

int CGame::GetExpireTimeTypeExt(int type)
{
	if(type>0 && type<PAY_FLAG_JP_SIZE)
	{
		return m_expiretime_typeExt[type];
	}
	return 0;
}


void CGame::SetSelectedServerID( unsigned int id )
{
	m_uiSelectedServerID = id;
}

unsigned int CGame::GetSelectedServerID()
{
	return m_uiSelectedServerID;
}
bool	CGame::IsAutoRun()
{
	return m_bAutoRun;
}
void	CGame::ToggleAutoRun()
{
	if( m_bAutoRun )
		m_bAutoRun = false;
	else
		m_bAutoRun = true;
}

void	CGame::ResetAutoRun()
{
	m_bAutoRun = false;
}

//-------------------------------------------------------------------------------------------
/// @brief Japanese partner for four minutes will activate the combo box?
///    - NHN Japan, yet only when it is not active.
//-------------------------------------------------------------------------------------------
bool CGame::IsActiveRouteComboBox()
{
	if( CCountry::GetSingleton().IsCountry( CCountry::COUNTRY_JPN ) && !g_GameDATA.m_is_NHN_JAPAN )
		return true;

	return false;
}
void CGame::SetJapanPartnerString( const char* str )
{
	if( NULL == str )
		return ;

	m_JapanPartnerString = str;
}


//07. 06. 14 - Kim, Joo - Hyun: ActiveX server when using the character selection window, select the window to move the display-connect to fix the problem
//ActiveX In order to determine whether to use more.
void CGame::SetActiveXConnect(bool bn)
{	
	m_UseActiveXConnect = bn;
}

bool CGame::GetActiveXConnect()
{
	return m_UseActiveXConnect;
}


const std::string& CGame::GetJapanPartnerString()
{
	return m_JapanPartnerString;
}


std::string& CGame::GetClientFileVersion()
{
	return m_ClientFileVersion.m_strFileVersion;
}

void CGame::SetCurVersion(std::string szString)
{
	m_CurVersion = szString;
}

std::string& CGame::GetCurVersion()
{
	return m_CurVersion;
}

void CGame::ClearWindowMsgQ()
{
	MSG m_Message;
	while ( ::PeekMessage (&m_Message, NULL, 0, 0, PM_REMOVE) )
	{
		::TranslateMessage (&m_Message);
		::DispatchMessage  (&m_Message);
	}
	ClearWndMsgQ();
}