#include "stdafx.h"
#include ".\delayedexp.h"
#include "../interface/it_mgr.h"
#include "../CObjCHAR.h"
#include "../Object.h"
#include "../GameCommon/StringManager.h"


/// for singleton
CDelayedExp _DealyedExp;


CDelayedExp::CDelayedExp(void)
{
}

CDelayedExp::~CDelayedExp(void)
{
}

void CDelayedExp::PushEXPData( int iOwnerIndex, long iAddExp, long iEXP, int iStamina )
{
	EXP_DATA data;
	data.iAddExp        = iAddExp;
	data.iExp			= iEXP;
	data.iGetTime		= g_GameDATA.GetGameTime();
	data.iOwnerIndex	= iOwnerIndex;

	data.iStamina		= iStamina;
	ClientLog( LOG_NORMAL, "PushEXPData saved Exp value of %i as %i for owner %i",iAddExp,data.iAddExp,data.iOwnerIndex);
	m_ExpData.push_back( data );
}

void CDelayedExp::GetExp( int iOwnerIndex )
{
	//PY: The code execution doesn't even seem to get here under normal running.
	//It does occasionally though so....
	//Disabling for now
	return;
	std::list< EXP_DATA >::iterator		begin = m_ExpData.begin();
	ClientLog( LOG_NORMAL, "Called GetExp for owner %i",iOwnerIndex);
	bool iOwnerTest = false;
	for( ; begin != m_ExpData.end() ; ++begin )
	{
		EXP_DATA& expData = *begin;

		/// It's the time to apply to avatar.
		if( expData.iOwnerIndex == iOwnerIndex )
		{
			/// Updated my experience is...
			/// In order to calculate the results can come out ...-by In the end, not the current experience is. .. Experience added value 
			/// Revisiting the process change..
			///long lDiff = expData.iExp - g_pAVATAR->GetCur_EXP();

			/// LDiff experience acquired to pay ... Message output..
			iOwnerTest = true;		//PY did we find the delayed Exp for this client?
			char szMsg[256];
			sprintf( szMsg, STR_GET_EXP, expData.iAddExp );
			g_itMGR.AppendChatMsg( szMsg, IT_MGR::CHAT_TYPE_SYSTEM );	
			ClientLog( LOG_NORMAL, "GetExp retrieved Exp value of %i",expData.iAddExp);
			g_pAVATAR->SetCur_EXP( expData.iExp );
			g_pAVATAR->SetCur_STAMINA( expData.iStamina );

			m_ExpData.erase( begin );
			break;
		}
	}
	if(!iOwnerTest)
		ClientLog( LOG_NORMAL, "GetExp Couldn't find a delayed Exp entry for owner",iOwnerIndex);
}

void CDelayedExp::Proc()
{
	//PY: This function is failing hardcore. Disabling it entirely
	return;
	std::list< EXP_DATA >::iterator		begin = m_ExpData.begin();
	ClientLog( LOG_NORMAL, "Called CDelayedExp::Proc");
	for( ; begin != m_ExpData.end() ; )
	{
		EXP_DATA& expData = *begin;

		bool bProcessExp = false;

		CObjCHAR* pObjCHAR = g_pObjMGR->Get_CharOBJ( expData.iOwnerIndex, true );	//returns a non-dead CObjCHAR object
		/// If the object was already dead or out of my sight, apply exp now.
		if( pObjCHAR == NULL )
		{			
			//bProcessExp = true;		//PY: This helps. It removes the ridiculous negative numbers but this entire function is still unreliable as hell
			begin = m_ExpData.erase( begin );		
		}
		else
		{
			DWORD dwCurrentTime = g_GameDATA.GetGameTime();
			if( dwCurrentTime - expData.iGetTime > 5000 )
			{
				bProcessExp = true;
				begin = m_ExpData.erase( begin );
			}else
				++begin;
		}

		/// erase exp
		if( bProcessExp )
		{			
			/// LDiff experience acquired to pay ... Message output..
			char szMsg[256];
			sprintf( szMsg, STR_GET_EXP, expData.iAddExp );
			g_itMGR.AppendChatMsg( szMsg, IT_MGR::CHAT_TYPE_SYSTEM );	
			ClientLog( LOG_NORMAL, "CDelayedExp::Proc retrieved Exp value of %i for owner %i",expData.iAddExp,expData.iOwnerIndex);
			g_pAVATAR->SetCur_EXP( expData.iExp );	
			g_pAVATAR->SetCur_STAMINA( expData.iStamina );

		}
	}
}