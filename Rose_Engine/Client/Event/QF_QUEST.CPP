/*
	$Header: /Client/Event/QF_QUEST.CPP 3     06-07-20 5:21p Raning $
*/
#include "stdAFX.h"
#include "Quest_FUNC.h"
#include "OBJECT.h"
#include "Common/IO_Quest.h"
#include "Network/CNetwork.h"
#include "Util/LogWnd.h"

int LOG_QUESTS_DEBUG = 0;	//set to 1 to enable debug output. 0 to disable

//-------------------------------------------------------------------------------------------------
/// Quest check the trigger conditions... AddCODE: by icarus
int		QF_checkQuestCondition ( ZSTRING szQuestTriggerName )
{
	if (LOG_QUESTS_DEBUG == 1)
	{
		ClientLog (LOG_NORMAL, "GF_checkQuestCondition( %s ) \n", szQuestTriggerName );
		//--------------------------------------------------------------------------------
		ClientLog( LOG_NORMAL, " ");
		ClientLog( LOG_NORMAL, "========================CONDITION BEGIN=========================" );
		ClientLog( LOG_NORMAL, "GF_checkQuestCondition( %s )", szQuestTriggerName );
		//--------------------------------------------------------------------------------
	}

	t_HASHKEY HashKEY = ::StrToHashKey( szQuestTriggerName );

	eQST_RESULT bResult = g_QuestList.CheckQUEST( g_pAVATAR, HashKEY );


	//--------------------------------------------------------------------------------
	if( bResult )
	{
		//LOGWAR( "GF_checkQuestCondition( %s ) Success", szQuestTriggerName );
		//ClientLog( LOG_NORMAL, "GF_checkQuestCondition( %s ) Success", szQuestTriggerName );
	}
	else
	{
		//LOGWAR( "GF_checkQuestCondition( %s ) Failed", szQuestTriggerName );
		//ClientLog( LOG_NORMAL, "GF_checkQuestCondition( %s ) Failed", szQuestTriggerName );
	}
	//--------------------------------------------------------------------------------

	//--------------------------------------------------------------------------------
	//ClientLog( LOG_NORMAL, "=========================CONDITION END==========================" );
	//ClientLog( LOG_NORMAL, " " );
	//--------------------------------------------------------------------------------

	return bResult;
}

//-------------------------------------------------------------------------------------------------
/// If the trigger condition is satisfied Server quest a quest trigger the transmission... AddCODE: by icarus
int		QF_doQuestTrigger ( ZSTRING szQuestTriggerName )
{
	//--------------------------------------------------------------------------------
	//ClientLog( LOG_NORMAL, " " );
	//ClientLog( LOG_NORMAL, "========================TRIGGER BEGIN=========================" );
	//ClientLog( LOG_NORMAL, "QF_doQuestTrigger( %s ) ", szQuestTriggerName );
	//--------------------------------------------------------------------------------

	if ( QF_checkQuestCondition ( szQuestTriggerName ) <= 0 )
	{
		//--------------------------------------------------------------------------------
		//ClientLog( LOG_NORMAL, "QF_checkQuestCondition( %s ) Failed[ Conditions of check failure on the client ] ", szQuestTriggerName );
		//--------------------------------------------------------------------------------
		return 0;
	}

	//ClientLog( LOG_NORMAL, "GF_doQuestTrigger( %d ) ", szQuestTriggerName );

	//--------------------------------------------------------------------------------
	//ClientLog( LOG_NORMAL, "QF_checkQuestCondition( %s ) success[ Trigger request to the server ] ", szQuestTriggerName );
	//ClientLog( LOG_NORMAL, "=========================TRIGGER END==========================" );
	//ClientLog( LOG_NORMAL, " " );
	//--------------------------------------------------------------------------------

	g_pNet->Send_cli_QUEST_REQ(TYPE_QUEST_REQ_DO_TRIGGER, 0, 0, (char*)szQuestTriggerName );
	return 1;
}


//-------------------------------------------------------------------------------------------------
int		QF_getQuestCount ( void )
{
	int iCnt=0;

	for (short nI=0; nI<QUEST_PER_PLAYER; nI++)
		if ( g_pAVATAR->m_Quests.m_QUEST[ nI ].GetID() )
			iCnt ++;
		
	LogString (LOG_DEBUG, "%d = QF_getQuestCount () \n", iCnt );

	//--------------------------------------------------------------------------------
	LOGOUT( "QF_getQuestCount() [ %d ] ", iCnt );
	//--------------------------------------------------------------------------------

	return iCnt;
}

//-------------------------------------------------------------------------------------------------
int		QF_findQuest ( int iQuestID )
{
	LogString (LOG_DEBUG, "QF_findQuest( %d ) \n", iQuestID );

	//--------------------------------------------------------------------------------
	LOGOUT( "QF_findQuest( %d ) ", iQuestID );
	//--------------------------------------------------------------------------------

	for (short nI=0; nI<QUEST_PER_PLAYER; nI++)
		if ( g_pAVATAR->m_Quests.m_QUEST[ nI ].GetID() == iQuestID )
		{
			//--------------------------------------------------------------------------------
			LOGOUT( "QF_findQuest( %d ) success[ %d Find a quest well ]", iQuestID, nI );
			//--------------------------------------------------------------------------------
			return nI;
		}

	//--------------------------------------------------------------------------------
	LOGERR( "QF_findQuest( %d ) Failed[ Unable to find the quest ]", iQuestID );
	//--------------------------------------------------------------------------------

	return -1;
}

//-------------------------------------------------------------------------------------------------
int		QF_getQuestID ( int hQUEST )
{
	LogString (LOG_DEBUG, "QF_getQuestID( %d ) \n", hQUEST );

	//--------------------------------------------------------------------------------
	LOGOUT( "QF_getQuestID( %d ) ", hQUEST );
	//--------------------------------------------------------------------------------

	if ( hQUEST<0 || hQUEST>QUEST_PER_PLAYER )
	{
		//--------------------------------------------------------------------------------
		LOGERR( "QF_getQuestID( %d ) FAILED", hQUEST );
		//--------------------------------------------------------------------------------
		return -1;
	}

	//--------------------------------------------------------------------------------
	LOGOUT( "QF_getQuestID( %d ) success [ Quest ID : %d ]", hQUEST, g_pAVATAR->m_Quests.m_QUEST[ hQUEST ].GetID() );
	//--------------------------------------------------------------------------------

	return g_pAVATAR->m_Quests.m_QUEST[ hQUEST ].GetID();
}

//-------------------------------------------------------------------------------------------------
int		QF_appendQuest ( int iQuestID )
{
	LogString (LOG_DEBUG, "QF_appendQuest( %d ) \n", iQuestID );

	//--------------------------------------------------------------------------------
	LOGOUT( "QF_appendQuest( %d ) ", iQuestID );
	//--------------------------------------------------------------------------------

	for (short nI=0; nI<QUEST_PER_PLAYER; nI++)
		if ( 0 == g_pAVATAR->m_Quests.m_QUEST[ nI ].GetID() ) 
		{
			g_pAVATAR->m_Quests.m_QUEST[ nI ].Init ();
			g_pAVATAR->m_Quests.m_QUEST[ nI ].SetID( iQuestID, true );

			//--------------------------------------------------------------------------------
			LOGOUT( "QF_appendQuest( %d ) [ The request to the server : %d ] ", iQuestID, nI );
			//--------------------------------------------------------------------------------

			g_pNet->Send_cli_QUEST_REQ(TYPE_QUEST_REQ_ADD, (BYTE)nI, iQuestID );
			return nI;
		}

	//--------------------------------------------------------------------------------
	LOGERR( "QF_appendQuest( %d ) FAILED ", iQuestID );
	//--------------------------------------------------------------------------------
	return -1;
}

//-------------------------------------------------------------------------------------------------
void	QF_deleteQuest ( int iQuestID )
{
	LogString (LOG_DEBUG, "QF_deleteQuest( %d ) \n", iQuestID );

	//--------------------------------------------------------------------------------
	LOGOUT( "QF_deleteQuest( %d ) ", iQuestID );
	//--------------------------------------------------------------------------------
	
	for (short nI=0; nI<QUEST_PER_PLAYER; nI++)
		if ( iQuestID == g_pAVATAR->m_Quests.m_QUEST[ nI ].GetID() ) 
		{
			g_pAVATAR->m_Quests.m_QUEST[ nI ].Init ();

			//--------------------------------------------------------------------------------
			LOGOUT( "QF_deleteQuest( %d ) [ The request to the server : %d ] ", iQuestID, nI );
			//--------------------------------------------------------------------------------

			g_pNet->Send_cli_QUEST_REQ(TYPE_QUEST_REQ_DEL, (BYTE)nI, iQuestID );
			return;
		}
}

//-------------------------------------------------------------------------------------------------
int		QF_getQuestVar ( int hQUEST, int iVarNO )
{
	LogString (LOG_DEBUG, "QF_getQuestVar( %d, %d ) \n", hQUEST, iVarNO );

	//--------------------------------------------------------------------------------
	LOGOUT( "QF_getQuestVar( %d, %d ) ", hQUEST, iVarNO );
	//--------------------------------------------------------------------------------

	if ( hQUEST<0 || hQUEST>QUEST_PER_PLAYER )
	{
		//--------------------------------------------------------------------------------
		LOGERR( "QF_getQuestVar( %d, %d ) FAILED", hQUEST, iVarNO );
		//--------------------------------------------------------------------------------

		return -1;
	}

	//--------------------------------------------------------------------------------
	LOGOUT( "QF_getQuestVar( %d, %d ) SUCCESS[ %d ]", hQUEST, iVarNO, g_pAVATAR->m_Quests.m_QUEST[ hQUEST ].Get_VAR( iVarNO ) );
	//--------------------------------------------------------------------------------

    return g_pAVATAR->m_Quests.m_QUEST[ hQUEST ].Get_VAR( iVarNO );
}

/*
//-------------------------------------------------------------------------------------------------
void	QF_setQuestVar ( int hQUEST, int iVarNO, int iValue )
{
	LogString (LOG_DEBUG, "QF_setQuestVar( %d, %d, %d ) \n", hQUEST, iVarNO, iValue );

	if ( hQUEST<0 || hQUEST>QUEST_PER_PLAYER )
		return;

	g_pAVATAR->m_Quests.m_QUEST[ hQUEST ].Set_VAR (iVarNO, iValue);
	g_pNet->Send_cli_SET_QUEST_VAR( hQUEST, iVarNO, iValue );
}
*/

//-------------------------------------------------------------------------------------------------
int		QF_getQuestSwitch ( int hQUEST, int iSwitchNO )
{
	LogString (LOG_DEBUG, "QF_getQuestSwitch( %d, %d ) \n", hQUEST, iSwitchNO );

	//--------------------------------------------------------------------------------
	LOGOUT( "QF_getQuestSwitch( %d, %d ) ", hQUEST, iSwitchNO );
	//--------------------------------------------------------------------------------

	if ( hQUEST<0 || hQUEST>QUEST_PER_PLAYER )
	{
		//--------------------------------------------------------------------------------
		LOGERR( "QF_getQuestSwitch( %d, %d ) FAILED", hQUEST, iSwitchNO );
		//--------------------------------------------------------------------------------
		return -1;
	}

	//--------------------------------------------------------------------------------
	LOGOUT( "QF_getQuestSwitch( %d, %d ) SUCCESS[ %d ] ", hQUEST, iSwitchNO, g_pAVATAR->m_Quests.m_QUEST[ hQUEST ].Get_SWITCH( iSwitchNO ) );
	//--------------------------------------------------------------------------------

	return g_pAVATAR->m_Quests.m_QUEST[ hQUEST ].Get_SWITCH( iSwitchNO );
}

/*
//-------------------------------------------------------------------------------------------------
void	QF_setQuestSwitch ( int hQUEST, int iSwitchNO, int iValue )
{
	LogString (LOG_DEBUG, "QF_setQuestSwitch( %d, %d, %d ) \n", hQUEST, iSwitchNO, iValue );

	if ( hQUEST<0 || hQUEST>QUEST_PER_PLAYER )
		return;

	g_pAVATAR->m_Quests.m_QUEST[ hQUEST ].Set_SWITCH( iSwitchNO, iValue );
	g_pNet->Send_cli_SET_QUEST_SWITCH( hQUEST );
}
*/

//-------------------------------------------------------------------------------------------------
int		QF_getEpisodeVAR ( int iVarNO )
{
	//--------------------------------------------------------------------------------
	LOGOUT( "QF_getEpisodeVAR( %d ) ", iVarNO );
	//--------------------------------------------------------------------------------

	if ( iVarNO < 0 || iVarNO >= QUEST_EPISODE_VAR_CNT )
	{
		//--------------------------------------------------------------------------------
		LOGERR( "QF_getEpisodeVAR( %d ) FAILED", iVarNO );
		//--------------------------------------------------------------------------------
		return -1;
	}

	//--------------------------------------------------------------------------------
	LOGOUT( "QF_getEpisodeVAR( %d ) SUCCESS[ %d ]", iVarNO, g_pAVATAR->m_Quests.m_nEpisodeVAR[ iVarNO ] );
	//--------------------------------------------------------------------------------

	return g_pAVATAR->m_Quests.m_nEpisodeVAR[ iVarNO ];
}

//-------------------------------------------------------------------------------------------------
int		QF_getJobVAR ( int iVarNO )
{
	//--------------------------------------------------------------------------------
	LOGOUT( "QF_getJobVAR( %d ) ", iVarNO );
	//--------------------------------------------------------------------------------

	if ( iVarNO < 0 || iVarNO >= QUEST_JOB_VAR_CNT )
	{
		//--------------------------------------------------------------------------------
		LOGERR( "QF_getJobVAR( %d ) FAILED ", iVarNO );
		//--------------------------------------------------------------------------------

		return -1;
	}

	//--------------------------------------------------------------------------------
	LOGOUT( "QF_getJobVAR( %d ) SUCCESS[ %d ] ", iVarNO, g_pAVATAR->m_Quests.m_nJobVAR[ iVarNO ] );
	//--------------------------------------------------------------------------------

	return g_pAVATAR->m_Quests.m_nJobVAR[ iVarNO ];
}

//-------------------------------------------------------------------------------------------------
int		QF_getPlanetVAR ( int iVarNO )
{
	//--------------------------------------------------------------------------------
	LOGOUT( "QF_getPlanetVAR( %d ) ", iVarNO );
	//--------------------------------------------------------------------------------

	if ( iVarNO < 0 || iVarNO >= QUEST_PLANET_VAR_CNT )
	{
		//--------------------------------------------------------------------------------
		LOGERR( "QF_getPlanetVAR( %d ) FAILED ", iVarNO );
		//--------------------------------------------------------------------------------

		return -1;
	}

	//--------------------------------------------------------------------------------
	LOGOUT( "QF_getPlanetVAR( %d ) SUCCESS[ %d ] ", iVarNO, g_pAVATAR->m_Quests.m_nPlanetVAR[ iVarNO ] );
	//--------------------------------------------------------------------------------

	return g_pAVATAR->m_Quests.m_nPlanetVAR[ iVarNO ];
}

//-------------------------------------------------------------------------------------------------
int		QF_getUnionVAR ( int iVarNO )
{
	//--------------------------------------------------------------------------------
	LOGOUT( "QF_getUnionVAR( %d ) ", iVarNO );
	//--------------------------------------------------------------------------------

	if ( iVarNO < 0 || iVarNO >= QUEST_UNION_VAR_CNT )
	{
		//--------------------------------------------------------------------------------
		LOGERR( "QF_getUnionVAR( %d ) FAILED ", iVarNO );
		//--------------------------------------------------------------------------------

		return -1;
	}

	//--------------------------------------------------------------------------------
	LOGOUT( "QF_getUnionVAR( %d ) SUCCESS[ %d ] ", iVarNO, g_pAVATAR->m_Quests.m_nUnionVAR[ iVarNO ] );
	//--------------------------------------------------------------------------------

	return g_pAVATAR->m_Quests.m_nUnionVAR[ iVarNO ];
}

//-------------------------------------------------------------------------------------------------
int		QF_getClanVAR ( int iVarNO )
{
#ifdef __CLAN_WAR_SET
	//--------------------------------------------------------------------------------
	LOGOUT( "QF_getClanVAR( %d ) ", iVarNO );
	//--------------------------------------------------------------------------------

	if ( iVarNO < 0 || iVarNO >= QUEST_CLANWAR_VAR_CNT )
	{
		//--------------------------------------------------------------------------------
		LOGERR( "QF_getClanVAR( %d ) FAILED ", iVarNO );
		//--------------------------------------------------------------------------------

		return -1;
	}

	//--------------------------------------------------------------------------------
	LOGOUT( "QF_getClanVAR( %d ) SUCCESS[ %d ] ", iVarNO, g_pAVATAR->m_Quests.m_nClanWarVAR[ iVarNO ] );
	//--------------------------------------------------------------------------------

	return g_pAVATAR->m_Quests.m_nClanWarVAR[ iVarNO ];
#else
	return 1;
#endif
}

//-------------------------------------------------------------------------------------------------
int		QF_getQuestItemQuantity ( int iQuestID, int iItemNo/*A five-digit:Type+No*/ )
{
	//--------------------------------------------------------------------------------
	LOGOUT( "QF_getQuestItemQuantity( %d, %d ) ", iQuestID, iItemNo );
	//--------------------------------------------------------------------------------

	tagITEM* pQuestItem = NULL;
	for (short nI=0; nI<QUEST_PER_PLAYER; nI++)
	{
		if ( g_pAVATAR->m_Quests.m_QUEST[ nI ].GetID() == iQuestID )
		{
			for( short nItemIdx = 0 ; nItemIdx < QUEST_ITEM_PER_QUEST; ++nItemIdx )
			{
				pQuestItem = g_pAVATAR->m_Quests.m_QUEST[ nI ].GetSlotITEM( nItemIdx );
				if( pQuestItem && pQuestItem->GetItemNO() == getItemNo( iItemNo ) && pQuestItem->GetTYPE() == getItemType( iItemNo ) )
				{
					if( pQuestItem->IsEnableDupCNT() )
					{
						//--------------------------------------------------------------------------------
						LOGOUT( "QF_getQuestItemQuantity( %d, %d ) [ The number of duplicate items : %d ] ", iQuestID, iItemNo, pQuestItem->m_uiQuantity );
						//--------------------------------------------------------------------------------

						return pQuestItem->m_uiQuantity;///중복가능한 아이템의 개수
					}
					else
					{
						//--------------------------------------------------------------------------------
						LOGOUT( "QF_getQuestItemQuantity( %d, %d ) [ Duplicates items : %d ] ", iQuestID, iItemNo, 1 );
						//--------------------------------------------------------------------------------
						return 1;///Duplicates items
					}
				}
			}
			//--------------------------------------------------------------------------------
			LOGERR( "QF_getQuestItemQuantity( %d, %d ) FAILED[ There is no corresponding items ] ", iQuestID, iItemNo );
			//--------------------------------------------------------------------------------

			return 0;///해당 아이템이 없다
		}
	}

	//--------------------------------------------------------------------------------
	LOGERR( "QF_getQuestItemQuantity( %d, %d ) FAILED[ No the quest ] ", iQuestID, iItemNo );
	//--------------------------------------------------------------------------------

	return -1;///There is no corresponding quests
}



int QF_getNpcQuestZeroVal ( int iNpcNO )
{
	CObjNPC* pNPC = (CObjNPC*)( g_pObjMGR->Get_CharOBJ( iNpcNO, false ) );

	if( pNPC != NULL )
	{
		return pNPC->GetEventValue();	
	}

	return 0;
}

int QF_getUserSwitch( int iSwitchNO )
{
	return g_pAVATAR->m_Quests.Get_SWITCH( iSwitchNO );
}