
#include "StdAfx.h"
#include "CCheat.h"
#include <shlwapi.h>

#include "..\OBJECT.h"
#include "..\\Game.h"

#include "IO_Terrain.h"
#include "CCamera.h"
#include "../Event/Quest_FUNC.h"
#include "../Interface/it_mgr.h"
#include "../Interface/dlgs/ChattingDlg.h"
#include "../Interface/cursor/CCursor.h"
#include "../System/CGame.h"
#include "../Network/CNetwork.h"
#include "System/System_FUNC.h"
#pragma comment(lib,"shlwapi.lib")

classPARSESTR g_Cheat;

bool DoCheatFunc(char* pStr)
{
	return g_Cheat.DoCheat(pStr);
}
char *s_pCheatStr[] = {
	"/SET",			//0
	"/ADD",			//1
	"/FULL",		//2
	"/SKILL",		//3
	"/ITEM",		//4
	"/MONEY",		//5
	"/SER",			//6
	"/FAME",		//7
	"/MEET",		//8
	"/FLY_MODE",	//9
	"/WARP",		//10
	"/WARP_M",		//11
	"/EFFECT",		//12
	"/SOUND",		//13
	"/진행수치",	//14
	"/조합명성",	//15
	"/QV",			//16
	"/직업",		//17
	"/정보",		//18
	"/NOUI",		//19
	"/PVP",			//20   Special cheat..
	"/CQC",			//21   Check Quest Condition..
	"/FREE",		//22
	"/CLAN",		//23
	"/PAYMENT",		//24
	"/QVSET",		//25
	"/TRANSLATE",	//26
	NULL
} ;	

enum _CHEAT_CODE {
	CHEAT_SET,			//0
	CHEAT_ADD,			//1
	CHEAT_FULL,			//2
	CHEAT_SKILL,		//3
	CHEAT_ITEM,			//4
	CHEAT_MONEY,		//5
	CHEAT_SER,			//6
	CHEAT_FAME,			//7
	CHEAT_MEET,			//8
	CHEAT_FLY_MODE,		//9
	CHEAT_WARP,			//10
	CHEAT_WARP_M,		//11
	CHEAT_EFFECT,		//12
	CHEAT_SOUND,		//13
	CHEAT_PROC_VAL,		//14
	CHEAT_UNION_FAME,	//15
	CHEAT_QUEST_VAL,	//16
	CHEAT_JOB,			//17
	CHEAT_DEBUG_INFO,	//18
	CHEAT_NO_UI,		//19
	CHEAT_SPECIAL_PVP,  //20
	CHEAT_CHECK_QUEST_CONDITION, //21
	CHEAT_FREE,			//22
	CHEAT_CLAN,			//23
	CHEAT_FEE,			//24
	CHEAT_QUEST_VAL_UPDOWN,  //25
	CHEAT_TRANSLATE,	//26
	CHEAT_MAX_CNT		
};


// SET
char* s_SET[] = {
	"STR",	//0
	"DEX",	//1
	"INT",  //2
	"CON",  //3
	"CHA",  //4
	"SEN",	//5
	"LEV",	//6
	"JOB"	//7
};

// ADD
char* s_ADD[] = {
	"EXP",		//0
	"POINT",	//1
	"HP",		//2
	"MP",		//3
	"ITEM",		//4
	"MONEY"		//5
};

// FULL
char* s_FULL[] = {
	"HP",		//0
	"MP",		//1
	"ITEM"		//2
};

// SKILL
char* s_SKILL[] = {
	"SKILL",	//0
	"ITEM",		//1
	"MONEY"		//2
};

// NPC
char* s_NPC[] = {
	"FAME",		//0
	"MEET"		//1
};

// ETC
char* s_ETC[] = {
	"FLY_MODE",	//0
	"WARP",		//1
	"WARP_M",	//2
	"EFFECT",	//3
	"SOUND"		//4
};

/// Quest ..
char* s_QuestInfo[] = {
	"VAL",
	"SWITCH",
	"TIME",
	"ID",
};
/// FREE
const char* s_Free[] = {
	"CHAT",
	"WEIGHT"
};

char* s_QuestUpDown[] = {
	"UP",
	"DOWN"
};

classPARSESTR::classPARSESTR()
{
	
}

classPARSESTR::~classPARSESTR()
{

}

void classPARSESTR::CheatInit()
{
	
}

char* classPARSESTR::ParseString(char* pStr,short nStrNo)
{
	int   iStrLen = strlen(pStr);
	short nSpaceCNT = 0;
	short nCNT = 0;
	ZeroMemory(m_szBUF,sizeof(char)*MAX_PATH);
	
	for(int iStr=0;iStr<iStrLen;iStr++) {
		if(pStr[iStr]==' ') { //공백이면 
			if(nSpaceCNT==nStrNo) {
				return m_szBUF;
			}
			nSpaceCNT++;
			continue;
		}
		if(nSpaceCNT==nStrNo) {
			m_szBUF[nCNT] = pStr[iStr];
			nCNT++;
		}
	}

	return m_szBUF;
}

short classPARSESTR::ParseCheatCodeIdx(char* pStr)
{
	for(short nI=0;nI<CHEAT_MAX_CNT;nI++) {
		if(!_strcmpi(pStr,s_pCheatStr[nI])) 
			return nI;
	}
	return -1;
}

//special 치트실행
int  classPARSESTR::DoSpecialCheat(char* pCheatCode, bool bServerCheate )
{
	char *pStr		= ParseString(pCheatCode,0);
	short nCodeIdx	= ParseCheatCodeIdx(pStr);

	if( bServerCheate )
	{
		switch(nCodeIdx)
		{
			case CHEAT_SPECIAL_PVP:
				if( DoSpecial( pCheatCode ) )
					return CHEAT_RESULT_SERVER;
				break;
		}
	}
	return CHEAR_RESULT_FAIL;
}


int classPARSESTR::DoCheat(char* pCheatCode, bool bServerCheate )
{
	char *pStr		= ParseString(pCheatCode,0);
	short nCodeIdx	= ParseCheatCodeIdx(pStr);

	if( bServerCheate )
	{
		DWORD dRight = CGame::GetInstance().GetRight();
#ifdef _DEBUG
		char Temp[128];
		sprintf(Temp, "Right = %d", dRight);
		g_itMGR.AppendChatMsg( Temp, IT_MGR::CHAT_TYPE_SYSTEM );
#endif
		switch(nCodeIdx)
		{
			case CHEAT_SET:
				if( DoSET(pCheatCode) )
					return CHEAT_RESULT_SERVER;
				break;
			case CHEAT_ADD:
				if( DoADD(pCheatCode) )
					return CHEAT_RESULT_SERVER;				
				break;
			case CHEAT_FULL:
				if( DoFULL(pCheatCode) )
					return CHEAT_RESULT_SERVER;				
				break;
			case CHEAT_SKILL:
				if( DoSKILL(pCheatCode) )
					return CHEAT_RESULT_SERVER;				
				break;
			case CHEAT_ITEM:
				if( DoITEM(pCheatCode) )
					return CHEAT_RESULT_SERVER;				
				break;
			case CHEAT_MONEY:
				if( DoMONEY(pCheatCode) )
					return CHEAT_RESULT_SERVER;				
				break;
			case CHEAT_SER:
				if( DoSER(pCheatCode) )
					return CHEAT_RESULT_SERVER;				
				break;
			case CHEAT_FAME:
				if( DoFAME(pCheatCode) )
					return CHEAT_RESULT_SERVER;				
				break;
			case CHEAT_MEET:
				if( DoMEET(pCheatCode) )
					return CHEAT_RESULT_SERVER;				
				break;
			case CHEAT_FLY_MODE:
				if( DoFLY_MODE(pCheatCode) )
					return CHEAT_RESULT_SERVER;				
				break;
			case CHEAT_WARP:
				if( DoWARP(pCheatCode) )
					return CHEAT_RESULT_SERVER;				
				break;
			case CHEAT_WARP_M:
				if( DoWARP_M(pCheatCode) )
					return CHEAT_RESULT_SERVER;				
				break;
			case CHEAT_SOUND:
				if( DoSOUND(pCheatCode) )
					return CHEAT_RESULT_SERVER;
				break;


		}
	}else
	{
		//05.11.17 - 김주현 : 128 이하의 권한이 가진 사람이 클라이언트에서 발동하는
		//치트키를 사용할려고 할경우 패쓰.
			DWORD dRight = CGame::GetInstance().GetRight();
#ifdef _DEBUG
			char Temp[128];
			sprintf(Temp, "Right = %d", dRight);
			g_itMGR.AppendChatMsg( Temp, IT_MGR::CHAT_TYPE_SYSTEM );
#endif
			switch(nCodeIdx)
				{
				case CHEAT_FEE:
					DoFee();
					return CHEAT_RESULT_CLIENT;
					break;
				case CHEAT_CLAN:
					if(	dRight < 128) return CHEAT_RESULT_CLIENT;
					if( DoClan( pCheatCode ) )
						return CHEAT_RESULT_CLIENT;
					break;
				case CHEAT_PROC_VAL:
					if(	dRight < 128) return CHEAT_RESULT_CLIENT;
					if( DoProcVal( pCheatCode ) )
						return CHEAT_RESULT_CLIENT;
					break;
				case CHEAT_UNION_FAME:
					if(	dRight < 128) return CHEAT_RESULT_CLIENT;
					if( DoUnionFame( pCheatCode ) )
						return CHEAT_RESULT_CLIENT;
					break;				
				case CHEAT_QUEST_VAL:
					if(	dRight < 128) return CHEAT_RESULT_CLIENT;
					if( DoQuestVal( pCheatCode ) )
						return CHEAT_RESULT_CLIENT;
					break;				
				case CHEAT_JOB:
					if(	dRight < 128) return CHEAT_RESULT_CLIENT;
					if( DoJob( pCheatCode ) )
						return CHEAT_RESULT_CLIENT;
					break;				
				case CHEAT_EFFECT:
					if(	dRight < 128) return CHEAT_RESULT_CLIENT;
					if( DoEFFECT( pCheatCode ) )
						return CHEAT_RESULT_CLIENT;
					break;				
				case CHEAT_DEBUG_INFO:
					if(	dRight < 128) return CHEAT_RESULT_CLIENT;
					{
						if( CGame::GetInstance().GetRight() )
						{
							g_GameDATA.m_bDisplayDebugInfo = !g_GameDATA.m_bDisplayDebugInfo;	
						}

						return CHEAT_RESULT_CLIENT;
					}
					break;
				case CHEAT_NO_UI:
					{
						g_GameDATA.m_bNoUI = !g_GameDATA.m_bNoUI;

						if( g_GameDATA.m_bNoUI )
							CCursor::GetInstance().SetCursorType( CCursor::CURSOR_NOUI );
						else
							CCursor::GetInstance().SetCursorType( CCursor::CURSOR_DEFAULT );

						return CHEAT_RESULT_CLIENT;
					}
					break;
				case CHEAT_FREE:
					{
					if(	dRight < 128) return CHEAT_RESULT_CLIENT;
						if( DoFree( pCheatCode ) )
							return CHEAT_RESULT_CLIENT;
						break;
					}

				case CHEAT_CHECK_QUEST_CONDITION:
					{
					if(	dRight < 128) return CHEAT_RESULT_CLIENT;
						DoCheckQuestCondition( pCheatCode );
						return CHEAT_RESULT_CLIENT;
					}
					break;
				case CHEAT_QUEST_VAL_UPDOWN:
					{
						//에피소드 퀘스트 변수 증가 / 감소 치트.
#ifdef _DEBUG
					if(	dRight < 512) return CHEAT_RESULT_CLIENT;
					if( DoQuestValUpDown(pCheatCode))
						return CHEAT_RESULT_CLIENT;
#endif
					}
					break;

				case CHEAT_TRANSLATE:
					{
						g_GameDATA.m_bTranslate = !g_GameDATA.m_bTranslate;
						
						char* pARG1 = ParseString(pStr,1);
						if(pARG1 != NULL)
						{
							int iVAL  = StrToInt(pARG1);
							g_GameDATA.m_iLangIDX = iVAL;
						}
						
						return CHEAT_RESULT_CLIENT;
					}
					break;

		}
	}
	return CHEAR_RESULT_FAIL;
}

bool classPARSESTR::DoSET(char* pStr)
{
	//명령 조건 : /SET SRT 숫자 

	char* pARG1 = ParseString(pStr,1);
	char* pARG2;
	int   iVAL;
	short nCodeIdx = GetSET_CodeIdx(pARG1);
	
	pARG2 = ParseString(pStr,2);
	iVAL  = StrToInt(pARG2);

	switch(nCodeIdx)
	{
	case 0: //srt
		g_pAVATAR->Set_STR( iVAL );
		g_itMGR.AppendChatMsg( "srt", IT_MGR::CHAT_TYPE_SYSTEM );
		return true;
	case 1: //dex
		g_pAVATAR->Set_DEX( iVAL );
		g_itMGR.AppendChatMsg("dex",  IT_MGR::CHAT_TYPE_SYSTEM );
		return true;
	case 2: //int
		g_pAVATAR->Set_INT( iVAL );
		g_itMGR.AppendChatMsg("int",  IT_MGR::CHAT_TYPE_SYSTEM );
		return true;
	case 3: //con
		g_pAVATAR->Set_CON( iVAL );
		g_itMGR.AppendChatMsg("con",  IT_MGR::CHAT_TYPE_SYSTEM );
		return true;
	case 4: //cha
		g_pAVATAR->Set_CHARM( iVAL );
		g_itMGR.AppendChatMsg("charm", IT_MGR::CHAT_TYPE_SYSTEM );
		return true;
	case 5: //sen
		g_pAVATAR->Set_SENSE( iVAL );
		g_itMGR.AppendChatMsg("sen",  IT_MGR::CHAT_TYPE_SYSTEM );
		return true;
	case 6: //lev
		g_pAVATAR->Set_LEVEL( iVAL );
		g_itMGR.AppendChatMsg("lev",  IT_MGR::CHAT_TYPE_SYSTEM );
		return true;
	case 7: //job
		g_pAVATAR->Set_JOB( iVAL );
		g_itMGR.AppendChatMsg("job",   IT_MGR::CHAT_TYPE_SYSTEM );
		return true;
	}

	return false;
}

short classPARSESTR::GetSET_CodeIdx(char* pStr)
{
	for(short nI=0;nI<8;nI++) {
		if(!_strcmpi(pStr,s_SET[nI])) 
			return nI;
	}
	return -1;
}

bool classPARSESTR::DoADD(char* pStr)
{
	// 명령 조건 : /ADD EXP 숫자 

	char* pARG1 = ParseString(pStr,1);
	char* pARG2;
	int   iVAL;
	short nCodeIdx = GetADD_CodeIdx(pARG1);
	
	pARG2 = ParseString(pStr,2);
	iVAL  = StrToInt(pARG2);
	
	switch(nCodeIdx)
	{
	case 0: //exp		
		g_pAVATAR->Set_EXP( g_pAVATAR->GetCur_EXP() + iVAL );
		return true;
	case 1: //point
		g_pAVATAR->Set_BonusPOINT( g_pAVATAR->GetCur_BonusPOINT() + iVAL );
		return true;
	}
	return false;
}

short classPARSESTR::GetADD_CodeIdx(char* pStr)
{
	for(short nI=0;nI<5;nI++) {
		if(!_strcmpi(pStr,s_ADD[nI])) 
			return nI;
	}
	return -1;

}

bool classPARSESTR::DoFULL(char* pStr)
{
	// 명령 조건 : /FULL HP
	char* pARG1 = ParseString(pStr,1);
	short nCodeIdx = GetADD_CodeIdx(pARG1);
	
	switch(nCodeIdx)
	{
	case 2: //hp
		g_pAVATAR->Set_HP( g_pAVATAR->Get_MaxHP() );		
		return true;
	case 3: //MP
		g_pAVATAR->Set_HP( g_pAVATAR->Get_MaxMP() );
		return true;
	case 4: //item
		return true;
	case 5: //MONEY
		g_pAVATAR->Set_HP( g_pAVATAR->Get_MaxMP() );
	}
	return false;
}

short classPARSESTR::GetFULL_CodeIdx(char* pStr)
{
	for(short nI=0;nI<3;nI++) {
		if(!_strcmpi(pStr,s_FULL[nI]))
			return nI;
	}
	return -1;
}

bool classPARSESTR::DoSKILL(char* pStr)
{
	// SKILL 숫자  해당번호의 스킬을 가지게 된다 
	char* pARG2;
	int   iVAL;
	pARG2 = ParseString(pStr,2);
	iVAL  = StrToInt(pARG2);
	
	// ToDO......
	return true;
}


bool classPARSESTR::DoITEM(char* pStr)
{
	// ITEM 종류 숫자 개수   해당 종류의 아이템을 개수만큼 인벤토리에 생성시킨다 
	// 장비 아이템의 경우는 개수가 무조건 1개
	char* pARG;
	int   iType,iVal,iCNT;
	
	// 종류 
	pARG   = ParseString(pStr,2);
	iType  = StrToInt(pARG);
	// 숫자 
	pARG   = ParseString(pStr,3);
	iVal   = StrToInt(pARG);
	// 개수 
	pARG   = ParseString(pStr,4);
	iCNT   = StrToInt(pARG);
	
	// ToDO......
	
	return true;
}

bool classPARSESTR::DoMONEY(char* pStr)
{
	// MONEY 숫자	지정수치만큼 돈증가 

	char* pARG;
	int   iVal;
	
	pARG   = ParseString(pStr,2);
	iVal   = StrToInt(pARG);

	// ToDO
	return true;
}


short classPARSESTR::GetQV_CodeIdx(char* pStr)
{
	for(short nI=0;nI<2;nI++) {
		if(!_strcmpi(pStr,s_QuestUpDown[nI])) 
			return nI;
	}
	return -1;

}

bool classPARSESTR::DoSER(char* pStr)
{
	return true;
}

bool classPARSESTR::DoFAME(char* pStr)
{
	return true;
}

bool classPARSESTR::DoMEET(char* pStr)
{
	return true;
}

bool classPARSESTR::DoFLY_MODE(char* pStr)
{
	return true;
}

bool classPARSESTR::DoWARP(char* pStr)
{
	char* pARG;
		
	// Zone number
	pARG   = ParseString(pStr,1);
	int iZoneNO  = StrToInt(pARG);
	// X
	pARG   = ParseString(pStr,2);
	int iX   = StrToInt(pARG);
	// Y
	pARG   = ParseString(pStr,3);
	int iY   = StrToInt(pARG);


	/// Loading mode change	
//	g_pTerrain->SetLoadingMode( true );

	DrawLoadingImage();

	g_pTerrain->FreeZONE ();	
	g_pTerrain->LoadZONE( iZoneNO );
	

	tPOINTF PosWARP;

	PosWARP.x = iX * 1000;
	PosWARP.y = iY * 1000;

	g_pTerrain->InitZONE( PosWARP.x , PosWARP.y );

	g_pAVATAR->Warp_POSITION( PosWARP.x, PosWARP.y );

	g_pCamera->Detach ();
	g_pCamera->Attach ( g_pAVATAR->GetZMODEL() );

	g_pObjMGR->Set_ServerObjectIndex (g_pAVATAR->Get_INDEX(), g_pAVATAR->Get_INDEX() );
	g_pAVATAR->Set_HP( g_pAVATAR->GetCur_MaxHP() );
	g_pAVATAR->Set_MP( g_pAVATAR->GetCur_MaxMP() );	

	return true;
}

bool classPARSESTR::DoWARP_M(char* pStr)
{
	return true;
}

static CEffect *s_pEF=NULL;
bool classPARSESTR::DoEFFECT(char* pStr)
{
	char* pARG;
	// 효과 number
	pARG   = ParseString(pStr,1);
	int iEffectNO  = StrToInt(pARG);

	if( s_pEF )
	{
		g_pEffectLIST->Del_EFFECT( s_pEF );
		s_pEF = NULL;
	}

	if ( !s_pEF ) 
	{
		s_pEF = g_pEffectLIST->Add_EffectWithIDX ( iEffectNO );
		if ( s_pEF ) 
		{
			s_pEF->LinkNODE( g_pAVATAR->GetZMODEL() );
			s_pEF->InsertToScene ();
		}		
	}
	
	
	return true;
}

bool classPARSESTR::DoSOUND(char* pStr)
{
	return true;
}

//진행 수치
bool classPARSESTR::DoProcVal( char* pStr )
{
	char* pARG;
	// 소속번호 number
	pARG   = ParseString(pStr,1);
	int iUnionNO  = StrToInt(pARG);
	
	
	//sprintf( Buf, "소속별 명성[ %d: %d ]", iUnionNO, g_pAVATAR->m_Quests.m_nQuestFameOfUnion[ iUnionNO ] );
	g_itMGR.AppendChatMsg( g_MsgBuf, IT_MGR::CHAT_TYPE_SYSTEM );

	return true;
}

//소속 명성
bool classPARSESTR::DoUnionFame( char* pStr )
{
	char* pARG;
	// 소속번호 number
	pARG   = ParseString(pStr,1);
	int iUnionNO  = StrToInt(pARG);

	
	//sprintf( Buf, "진행수치[ %d : %d ]", iUnionNO, g_pAVATAR->m_Quests.m_nQuestProcOfUnion[ iUnionNO ] );
	g_itMGR.AppendChatMsg( g_MsgBuf, IT_MGR::CHAT_TYPE_SYSTEM );

	return true;
}

//퀘스트 변수
bool classPARSESTR::DoQuestVal( char* pStr )
{
	char* pARG;

	// 슬롯번호 number
	pARG   = ParseString(pStr,1);

	if( _stricmp( "VJ", pARG ) == 0 )
	{
		for( int i = 0; i < QUEST_JOB_VAR_CNT; ++i )
			g_itMGR.AppendChatMsg( CStr::Printf("[ JOB%d : %d ]", i+1, g_pAVATAR->m_Quests.m_nJobVAR[ i ]), IT_MGR::CHAT_TYPE_SYSTEM );
	}

	if( _stricmp( "VP", pARG ) == 0 )
	{
		for( int i = 0; i < QUEST_PLANET_VAR_CNT; ++i )
			g_itMGR.AppendChatMsg( CStr::Printf("[PLANET%d : %d ]", i+1, g_pAVATAR->m_Quests.m_nPlanetVAR[ i ]), IT_MGR::CHAT_TYPE_SYSTEM );
	}

	if( _stricmp( "VE", pARG ) == 0 )
	{
		for( int i = 0; i < QUEST_EPISODE_VAR_CNT; ++i )
			g_itMGR.AppendChatMsg( CStr::Printf("[EPISODE%d : %d ]", i+1, g_pAVATAR->m_Quests.m_nEpisodeVAR[ i ]), IT_MGR::CHAT_TYPE_SYSTEM );
	}

	if( _stricmp( "VU", pARG ) == 0 )
	{
		for( int i = 0; i < QUEST_UNION_VAR_CNT; ++i )
			g_itMGR.AppendChatMsg( CStr::Printf("[UNION%d : %d ]", i+1, g_pAVATAR->m_Quests.m_nUnionVAR[ i ]), IT_MGR::CHAT_TYPE_SYSTEM );
	}

#ifdef __CLAN_WAR_SET
	if( _stricmp( "VC", pARG ) == 0 )
	{
		for( int i = 0; i < QUEST_CLANWAR_VAR_CNT; ++i )
			g_itMGR.AppendChatMsg( CStr::Printf("[CLAN%d : %d ]", i+1, g_pAVATAR->m_Quests.m_nClanWarVAR[ i ]), IT_MGR::CHAT_TYPE_SYSTEM );
	}
#endif

	int iSlotNO  = StrToInt(pARG);

	pARG   = ParseString( pStr, 2 );


	
	if( strcmp( "VAL", pARG ) == 0 )
	{
		for( int i  = 0; i < QUEST_VAR_PER_QUEST; i++ )
		{			
			sprintf( g_MsgBuf, "[ %d, %d ]", i, g_pAVATAR->m_Quests.m_QUEST[ iSlotNO ].Get_VAR( i ) );
			g_itMGR.AppendChatMsg( g_MsgBuf, IT_MGR::CHAT_TYPE_SYSTEM );
		}
	}

	if( strcmp( "SW", pARG ) == 0 )
	{
		for( int i  = 0; i < QUEST_SWITCH_PER_QUEST; i++ )
		{			
			sprintf( g_MsgBuf, "[ %d, %d ]", i, g_pAVATAR->m_Quests.m_QUEST[ iSlotNO ].Get_SWITCH( i ) );
			g_itMGR.AppendChatMsg( g_MsgBuf, IT_MGR::CHAT_TYPE_SYSTEM );
		}
	}

	if( strcmp( "TIME", pARG ) == 0 )
	{
		sprintf( g_MsgBuf, "[ TIME : %d ]", g_pAVATAR->m_Quests.m_QUEST[ iSlotNO ].GetExpirationTIME() );
		g_itMGR.AppendChatMsg( g_MsgBuf, IT_MGR::CHAT_TYPE_SYSTEM );		
	}

	if( strcmp( "ID", pARG ) == 0 )
	{
		sprintf( g_MsgBuf, "[ ID : %d ]", g_pAVATAR->m_Quests.m_QUEST[ iSlotNO ].GetID() );
		g_itMGR.AppendChatMsg( g_MsgBuf, IT_MGR::CHAT_TYPE_SYSTEM );		
	}

	return true;
}

// 직업 전환
bool classPARSESTR::DoJob( char* pStr )
{
	char* pARG;

	// 직업번호 number
	pARG   = ParseString(pStr,1);
	int iJobNO  = StrToInt(pARG);
	

	//g_pNet->Recv_cli_SET_VAR_REQ( AT_CLASS, iJobNO );

	return true;
}
bool classPARSESTR::DoFree( char* pStr )
{
	char* pARG;
	
	// ON, OFF
	pARG   = ParseString(pStr,1);

	if( _strcmpi( "WEIGHT", pARG ) == 0 )
	{
		if( CGame::GetInstance().GetRight() )
		{
			g_GameDATA.m_bNoWeight = !g_GameDATA.m_bNoWeight;
			if( g_GameDATA.m_bNoWeight )
				g_pNet->Send_cli_SET_WEIGHT_RATE( 0 );					

			return true;
		}
	}

	if( _strcmpi( "CHAT", pARG ) == 0 )
	{
		CTDialog* pDlg = g_itMGR.FindDlg( DLG_TYPE_CHAT );
		if( CGame::GetInstance().GetRight() && pDlg )
		{
			CChatDLG* pChatDlg = (CChatDLG*)pDlg;
			pChatDlg->SetDisableShoutRestrict();
			return true;
		}
	}
	return false;
}
// 스페셜 치트
bool classPARSESTR::DoSpecial( char* pStr )
{
	char* pARG;
	
	/// PVP 가 허용되지 않음
	if( !g_pTerrain->IsPVPZone() )
		return false;


	// ON, OFF
	pARG   = ParseString(pStr,1);

	if( strcmp( "ON", pARG ) == 0 )
	{
		g_GameDATA.m_iPvPState = PVP_PERMITION_ALL;
		return true;
	}

	if( strcmp( "OFF", pARG ) == 0 )
	{
		g_GameDATA.m_iPvPState = PVP_CANT;
		return true;
	}

	return false;
}

bool classPARSESTR::DoCheckQuestCondition( char *pszStr )
{
	if( pszStr == NULL )
		return false;

	char* pszArg = ParseString( pszStr, 1 );
	if( pszArg == NULL )
		return false;

	char* pszBuf = NULL;
	if( QF_checkQuestCondition( pszArg ) )
		pszBuf = CStr::Printf("Quest Condition is True:%s", pszArg );
	else
		pszBuf = CStr::Printf("Quest Condition is False:%s", pszArg );

	g_itMGR.AppendChatMsg( pszBuf ,g_dwWHITE );
	return false;
}

bool classPARSESTR::DoClan( char* pStr )
{
	g_pNet->Send_cli_CLAN_CHAT( pStr );
	return true;
}

void classPARSESTR::DoFee()
{
	DWORD paytype = CGame::GetInstance().GetDetailPayType();
	switch( paytype  )
	{
	case BILLING_MSG_FREE_USER:
		// 귀하는 무료 사용자 입니다
		g_itMGR.AppendChatMsg( STR_BILLING_MSG_FREE_USER , IT_MGR::CHAT_TYPE_SYSTEM );
		break;
	case BILLING_MSG_PAY_FU:
		// 평생무료 사용자입니다.
		g_itMGR.AppendChatMsg( STR_FREE_ACCOUNT_USER , IT_MGR::CHAT_TYPE_SYSTEM );
		break;
	case BILLING_MSG_PAY_FA:
		// 개인 정액 사용자입니다.
		g_itMGR.AppendChatMsg( STR_PAID_DAYBASED_PLAN, IT_MGR::CHAT_TYPE_SYSTEM );
		break;
	case BILLING_MSG_PAY_FAP:
		// 개인정액(플래티넘) 사용자입니다.
		g_itMGR.AppendChatMsg( STR_PAID_DAYBASED_PLAN_PLATINUM , IT_MGR::CHAT_TYPE_SYSTEM );
		break;
	case BILLING_MSG_PAY_FQ:
		// 개인 정량 사용자 입니다.
		g_itMGR.AppendChatMsg( STR_PAID_TIMEBASED_PLAN ,IT_MGR::CHAT_TYPE_SYSTEM );
		break;
	case BILLING_MSG_PAY_GU:
		// "게임방 사용자로 접속되었습니다"
		g_itMGR.AppendChatMsg( STR_PAID_INTERNET_CAFE, IT_MGR::CHAT_TYPE_SYSTEM );
		break;
	case BILLING_MSG_PAY_GQ:
		// "게임방의 과금기간(시간)이 만료되어 개인정량으로 접속되었습니다"
		g_itMGR.AppendChatMsg( STR_PAID_USER_PLAN_IC_TIME_END, IT_MGR::CHAT_TYPE_SYSTEM );
		break;
	case BILLING_MSG_PAY_IQ:
		//"게임방의 접속 가능 IP숫자를 넘어 개인정량으로 접속되었습니다"
		g_itMGR.AppendChatMsg( STR_PAID_USER_PLAN_IC_IP_FULL ,IT_MGR::CHAT_TYPE_SYSTEM );
		break;
	default:
		break;
	}

	switch( CGame::GetInstance().GetExpireTimeType() )
	{
	case 1://날짜
		{
			const std::string& strTime = CGame::GetInstance().GetExpireTime();

			if( strTime.size() >= 12 )
			{
				int year  = atoi( strTime.substr( 0, 4 ).c_str() );
				int month = atoi( strTime.substr( 4, 2 ).c_str() );
				int day   = atoi( strTime.substr( 6, 2 ).c_str() );
				int hour  = atoi( strTime.substr( 8, 2 ).c_str() );
				int min   = atoi( strTime.substr( 10, 2 ).c_str() );

				g_itMGR.AppendChatMsg( CStr::Printf("%s:%d/%d/%d %d:%02d", STR_EXPIRED_TIME, year, month, day, hour, min ), IT_MGR::CHAT_TYPE_SYSTEM );
			}
		}
		break;
	case 2://분
		{
			const std::string& time = CGame::GetInstance().GetExpireTime();
			DWORD set_time = CGame::GetInstance().GetSetExpireTime();
			DWORD curr_time = g_GameDATA.GetGameTime();

			int iTime = atoi( time.c_str() );
			int remain_time = iTime - ( curr_time - set_time ) / ( 1000 * 60 );

			g_itMGR.AppendChatMsg( CStr::Printf( STR_GAME_TIME_LEFT_MINS, remain_time ), IT_MGR::CHAT_TYPE_SYSTEM );
		}
		break;
	default:
		break;
	}
}


bool classPARSESTR::DoQuestValUpDown(char* pStr)
{
	// 07. 01. 03 - 김주현 : 에피소드 변수 증가 / 감소.
	// 퀘스트 테스트의 편리를 위해서 추가. 현재는 1003번 npc의 0번 변수가 10일 경우에만 동작하도록 해뒀습니다.
    
	// 한번더 체크하자!! 악용의 소지가..ㅡㅜ
	if(CGame::GetInstance().GetRight() < 512) return CHEAT_RESULT_CLIENT;

	// 명령 조건 :/QVSET UP or DOWN 반복횟수, 예) /qvset up 10 - 3번 실행한다.
	char* pARG1 = ParseString(pStr,1);
	
	short nCodeIdx = GetQV_CodeIdx(pARG1);

	char* pARG2 = ParseString(pStr,2);

	int iRunPoint = 0;
	if(pARG2 != NULL)
		iRunPoint = atoi(pARG2);
	
	if(CheckEventNPCZeroVAR(1003, 10))
	{
		switch(nCodeIdx)
		{
		case 0: //UP
			{
				if(iRunPoint > 0)
				{
					for(int i = 0; i < iRunPoint; i++)
					{
						SC_QF_doQuestTrigger( "1111" );
					}
				}
				else
				{
					SC_QF_doQuestTrigger( "1111" );
				}
				
			}
			return true;
		case 1: //DOWN
			{
				if(iRunPoint > 0)
				{
					for(int i = 0; i < iRunPoint; i++)
					{
						SC_QF_doQuestTrigger( "2222" );
					}
				}
				else
				{
					SC_QF_doQuestTrigger( "2222" );
				}
				
			}
			return true;
		}
	}
	else
	{
		g_itMGR.AppendChatMsg( "GF_checkQuestCondition( TA_Event_VAR ) Failed" ,IT_MGR::CHAT_TYPE_SYSTEM);
	}

	return false;	
}

bool classPARSESTR::CheckEventNPCZeroVAR(int iNpcNum, int iNpcZeroVAR)
{
		int npc = 0;
		int npcVar0 = 0;
	
		CObjCHAR * pNPC = (CObjCHAR*)g_pObjMGR->Get_ClientCharOBJBySTBIndex(iNpcNum);

		if(pNPC)
		{
			npcVar0 = QF_getNpcQuestZeroVal(pNPC->Get_INDEX());
		}
		else
		{
			g_itMGR.AppendChatMsg( CStr::Printf("%d NPC로 이동한 다음에 치트를 사용해보세요..^^",  iNpcNum), IT_MGR::CHAT_TYPE_SYSTEM );
		}
		if( npcVar0 == iNpcZeroVAR )
			return 1;
		else
			return 0;
};
