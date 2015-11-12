#include "stdafx.h"
#include "CSkillDLG.h"
#include "../it_Mgr.h"
#include "../CToolTipMgr.h"
#include "../Object.h"
#include "../IO_ImageRes.h"
#include "../icon/ciconskill.h"
#include "../../Network/CNetwork.h"
#include "../../GameCommon/Skill.h"
#include "../../gamecommon/item.h"
#include "subclass/CSkillListItem.h"

#include "TPane.h"
#include "JContainer.h"
#include "TabbedPane.h"
#include "TContainer.h"
#include "JListBox.h"
#include "TButton.h"
#include "TImage.h"
#include "TListBox.h"
#include "TCaption.h"


#define SKILL_SLOT_SX		24
#define SKILL_SLOT_SY		91

#define SKILL_SLOT_WIDTH	33
#define SKILL_SLOT_HEIGHT	33

#define SKILL_NEED_POINT_SX 85
#define SKILL_NEED_POINT_SY 251

#define SKILL_POINT_SX		151
#define SKILL_POINT_SY		301

#define SKILL_POINTTEXT_WIDTH	54
#define SKILL_POINTTEXT_HEIGHT	19

#include "ZListBox.h"

CSkillDLG::CSkillDLG( int iType )
{
	
}

CSkillDLG::~CSkillDLG()
{

}



POINT CSkillDLG::Get_SkillSlot_Pos(int iIndex)
{
	CWinCtrl * pCtrl00 = Find( "SLOT_00" );
	CWinCtrl * pCtrl01 = Find( "SLOT_01" );
	CWinCtrl * pCtrl10 = Find( "SLOT_10" );	

	assert( pCtrl00 && pCtrl01 && pCtrl10 );

	int iGapX = pCtrl01->GetOffset().x - pCtrl00->GetOffset().x;
	int iGapY = pCtrl10->GetOffset().y - pCtrl00->GetOffset().y;

	POINT pt;
	pt.x = iIndex % 7 * iGapX;
	pt.y = iIndex / 7 * iGapY;

	return pt;
		
}

void CSkillDLG::Init_Skill()
{	
	POINT pt = { GetPosition().x, GetPosition().y };
	Clear();
	Create("DlgSkill");		
	SetInterfacePos_After();
	MoveWindow(pt);
}


void CSkillDLG::SetInterfacePos_After()
{
	CWinCtrl * pCtrl = NULL;

	//문자열 추가.
	//스킬 23
	GetCaption()->SetString( LIST_STRING(23) );	

	//기본 845
	if(pCtrl = Find(IID_TAB_BASIC)){	pCtrl->SetText(LIST_STRING(845));	}
	if(pCtrl = Find(IID_TAB_BASIC_COMM))
	{
		//기본스킬 846
		pCtrl->SetText( LIST_STRING(846) );	
	}

	//직업 813
	if(pCtrl = Find(IID_TAB_000)){	pCtrl->SetText(LIST_STRING(813));	}
	
	//파츠 847
	if(pCtrl = Find(IID_TAB_PARTS)){	pCtrl->SetText(LIST_STRING(847));	}
	if(pCtrl = Find(IID_TAB_PARTS_CART))
	{
		//카트 733
		pCtrl->SetText(LIST_STRING(733));	
	}
	if(pCtrl = Find(IID_TAB_PARTS_CASTLEGEAR))
	{
		//캐슬기어 734
		pCtrl->SetText(LIST_STRING(734));	
	}
	
	//유니크 848
	if(pCtrl = Find(IID_TAB_UNIQUE)){	pCtrl->SetText( LIST_STRING(848) );	}
	if(pCtrl = Find(IID_TAB_UNIQUE_COMM))
	{		
		//유니크스킬 848 (번역이 완료되면 유니크 스킬   이라는 스트링으로 교체해야함!)
		pCtrl->SetText( LIST_STRING(848) );	
	}

		 
	//직업 813
	if(pCtrl = Find(IID_TAB_111)){	(pCtrl)->SetText( LIST_STRING(813) );	}	//솔져
		//공용계열 849
		if(pCtrl = Find(IID_TAB_111_COMM)){	(pCtrl)->SetText( LIST_STRING(849) );	}	
		//한손계열 850
		if(pCtrl = Find(IID_TAB_111_ONE)){	(pCtrl)->SetText( LIST_STRING(850) );	}	
		//양손계열 851
		if(pCtrl = Find(IID_TAB_111_TWO)){	(pCtrl)->SetText( LIST_STRING(851) );	}	
		//나이트
		if(pCtrl = Find(IID_TAB_121)){	(pCtrl)->SetText(CStringManager::GetSingleton().GetJobName( CLASS_SOLDIER_121 ) );	}	
		//챔프
		if(pCtrl = Find(IID_TAB_122)){	(pCtrl)->SetText(CStringManager::GetSingleton().GetJobName( CLASS_SOLDIER_122 ) );	}	


	
	//직업 813
	if(pCtrl = Find(IID_TAB_211)){	(pCtrl)->SetText( LIST_STRING(813) );	}	//뮤즈
		//공용계열 849
		if(pCtrl = Find(IID_TAB_211_COMM)){	(pCtrl)->SetText( LIST_STRING(849) );	}	
		//위자드계열 852
		if(pCtrl = Find(IID_TAB_211_ATTACK)){	(pCtrl)->SetText( LIST_STRING(909) );	}	
		//힐러계열 853
		if(pCtrl = Find(IID_TAB_211_SUB)){	(pCtrl)->SetText( LIST_STRING(910) );	}	
		//메지션
		if(pCtrl = Find(IID_TAB_221)){	(pCtrl)->SetText(CStringManager::GetSingleton().GetJobName( CLASS_MAGICIAN_221 ));	}	
		//클레릭
		if(pCtrl = Find(IID_TAB_222)){	(pCtrl)->SetText(CStringManager::GetSingleton().GetJobName( CLASS_MAGICIAN_222 ));	}	

	//직업 813
	if(pCtrl = Find(IID_TAB_311)){	(pCtrl)->SetText( LIST_STRING(813) );	}	//호커
		//공용계열 849
		if(pCtrl = Find(IID_TAB_311_COMM)){	(pCtrl)->SetText( LIST_STRING(849) );	}	
		//활계열 854
		if(pCtrl = Find(IID_TAB_311_BOW)){	(pCtrl)->SetText( LIST_STRING(854) );	}	
		//격투계열 855
		if(pCtrl = Find(IID_TAB_311_MEELE)){	(pCtrl)->SetText( LIST_STRING(855) );	}	
		//레이더
		if(pCtrl = Find(IID_TAB_321)){	(pCtrl)->SetText(CStringManager::GetSingleton().GetJobName( CLASS_MIXER_321 ));	}	
		//스카우트
		if(pCtrl = Find(IID_TAB_322)){	(pCtrl)->SetText(CStringManager::GetSingleton().GetJobName( CLASS_MIXER_322 ));	}	

	//직업 813
	if(pCtrl = Find(IID_TAB_411)){	(pCtrl)->SetText( LIST_STRING(813) );	}	//딜러
		//공용계열 849
		if(pCtrl = Find(IID_TAB_411_COMM)){	(pCtrl)->SetText( LIST_STRING(849) );	}	
		//공격계열 852
		if(pCtrl = Find(IID_TAB_411_ATTACK)){	(pCtrl)->SetText( LIST_STRING(852) );	}
		//제조계열 856
		if(pCtrl = Find(IID_TAB_411_MAKE)){	(pCtrl)->SetText( LIST_STRING(856) );	}	
		//브르주아
		if(pCtrl = Find(IID_TAB_421)){	(pCtrl)->SetText(CStringManager::GetSingleton().GetJobName( CLASS_MERCHANT_421 ));	}	
		//아티잔
		if(pCtrl = Find(IID_TAB_422)){	(pCtrl)->SetText(CStringManager::GetSingleton().GetJobName( CLASS_MERCHANT_422 ));	}	

	//마일리지 스킬
	if(pCtrl = Find(IID_BTN_MILEAGE)){	(pCtrl)->SetText( LIST_STRING(843) );	}	//마일리지
		if(pCtrl = Find(IID_BTN_MILEAGE_1)){	(pCtrl)->SetText( CStr::Printf( "%s%d", LIST_STRING(843), 1 ) );	}	//마일리지1
		if(pCtrl = Find(IID_BTN_MILEAGE_2)){	(pCtrl)->SetText( CStr::Printf( "%s%d", LIST_STRING(843), 2 ) );	}	//마일리지2
		if(pCtrl = Find(IID_BTN_MILEAGE_3)){	(pCtrl)->SetText( CStr::Printf( "%s%d", LIST_STRING(843), 3 ) );	}	//마일리지3
		if(pCtrl = Find(IID_BTN_MILEAGE_4)){	(pCtrl)->SetText( CStr::Printf( "%s%d", LIST_STRING(843), 4 ) );	}	//마일리지4
			

	if(pCtrl = Find("SKILL_POINT_TXT"))
	{
		((CTImage*)pCtrl)->SetText("Skill Point");
		((CTImage*)pCtrl)->SetAlign(DT_VCENTER | DT_RIGHT);
	}
	if(pCtrl = Find("SKILL_POINT_NUM_TXT"))
	{		
		((CTImage*)pCtrl)->SetAlign(DT_VCENTER | DT_CENTER);
	}


	if(pCtrl = Find("SKILL_NEW"))
	{		
		((CTImage*)pCtrl)->Hide();
	}	
}

void CSkillDLG::Set_Job_Desc(int iJobIndex)
{
	CWinCtrl * pCtrl = NULL;

	if( pCtrl = Find("JOB_TYPE_NAME") )	
	{
		((CTImage*)pCtrl)->SetText( CStringManager::GetSingleton().GetJobName( iJobIndex ) );	
		((CTImage*)pCtrl)->SetAlign( DT_LEFT | DT_VCENTER );	
	}	

	if( pCtrl = Find("JOB_TYPE_DESC") )
	{

		
        //직업 설명.
		((CTListBox*)pCtrl)->ClearText();
		switch(iJobIndex)
		{
		case 000:
			((CTListBox*)pCtrl)->AppendText(LIST_STRING(799), g_dwWHITE);
			((CTListBox*)pCtrl)->AppendText(LIST_STRING(800), g_dwWHITE);
			break;

		case 111:
			((CTListBox*)pCtrl)->AppendText(LIST_STRING(801), g_dwWHITE);
			break;
		case 121:
			((CTListBox*)pCtrl)->AppendText(LIST_STRING(805), g_dwWHITE);
			break;
		case 122:
			((CTListBox*)pCtrl)->AppendText(LIST_STRING(806), g_dwWHITE);
			break;

		case 211:
			((CTListBox*)pCtrl)->AppendText(LIST_STRING(803), g_dwWHITE);
			break;
		case 221:
			((CTListBox*)pCtrl)->AppendText(LIST_STRING(810), g_dwWHITE);
			break;
		case 222:
			((CTListBox*)pCtrl)->AppendText(LIST_STRING(809), g_dwWHITE);
			break;

		case 311:
			((CTListBox*)pCtrl)->AppendText(LIST_STRING(802), g_dwWHITE);
			break;
		case 321:
			((CTListBox*)pCtrl)->AppendText(LIST_STRING(808), g_dwWHITE);
			break;
		case 322:
			((CTListBox*)pCtrl)->AppendText(LIST_STRING(807), g_dwWHITE);
			break;

		case 411:
			((CTListBox*)pCtrl)->AppendText(LIST_STRING(804), g_dwWHITE);
			break;
		case 421:
			((CTListBox*)pCtrl)->AppendText(LIST_STRING(811), g_dwWHITE);
			break;
		case 422:
			((CTListBox*)pCtrl)->AppendText(LIST_STRING(812), g_dwWHITE);
			break;
		}
	}
}

void CSkillDLG::Set_Skill4Job(int iJobIndex)
{
	CWinCtrl * pCtrl = NULL;	

	Set_Job_Desc( iJobIndex );
	
	if(pCtrl = Find(IID_TAB_000))	{	pCtrl->Hide();	}
	if(pCtrl = Find(IID_TAB_111))	{	pCtrl->Hide();	}
	if(pCtrl = Find(IID_TAB_211))	{	pCtrl->Hide();	}
	if(pCtrl = Find(IID_TAB_311))	{	pCtrl->Hide();	}
	if(pCtrl = Find(IID_TAB_411))	{	pCtrl->Hide();	}

	if(pCtrl = Find(IID_TAB_121))	{	pCtrl->Hide();	}
	if(pCtrl = Find(IID_TAB_122))	{	pCtrl->Hide();	}
	if(pCtrl = Find(IID_TAB_221))	{	pCtrl->Hide();	}
	if(pCtrl = Find(IID_TAB_222))	{	pCtrl->Hide();	}
	if(pCtrl = Find(IID_TAB_321))	{	pCtrl->Hide();	}
	if(pCtrl = Find(IID_TAB_322))	{	pCtrl->Hide();	}
	if(pCtrl = Find(IID_TAB_421))	{	pCtrl->Hide();	}
	if(pCtrl = Find(IID_TAB_422))	{	pCtrl->Hide();	}

	//job image icon index.
	if(pCtrl = Find(200000))	{	pCtrl->Hide();	}
	
	if(pCtrl = Find(200111))	{	pCtrl->Hide();	}
	if(pCtrl = Find(200121))	{	pCtrl->Hide();	}
	if(pCtrl = Find(200122))	{	pCtrl->Hide();	}
	
	if(pCtrl = Find(200211))	{	pCtrl->Hide();	}
	if(pCtrl = Find(200221))	{	pCtrl->Hide();	}
	if(pCtrl = Find(200222))	{	pCtrl->Hide();	}
	
	if(pCtrl = Find(200311))	{	pCtrl->Hide();	}
	if(pCtrl = Find(200321))	{	pCtrl->Hide();	}
	if(pCtrl = Find(200322))	{	pCtrl->Hide();	}
	
	if(pCtrl = Find(200411))	{	pCtrl->Hide();	}
	if(pCtrl = Find(200421))	{	pCtrl->Hide();	}
	if(pCtrl = Find(200422))	{	pCtrl->Hide();	}	

	if( pCtrl = Find(200000+iJobIndex) )	{	pCtrl->Show();	}

	switch(iJobIndex)
	{
	case 000:
		if(pCtrl = Find(IID_TAB_000))	{	pCtrl->Show();	}
		break;

	case 111:
		if(pCtrl = Find(IID_TAB_111))	{	pCtrl->Show();	}		
		break;
	case 121:
		if(pCtrl = Find(IID_TAB_111))	{	pCtrl->Show();	}
		if(pCtrl = Find(IID_TAB_121))	{	pCtrl->Show();	}		
		break;
	case 122:
		if(pCtrl = Find(IID_TAB_111))	{	pCtrl->Show();	}
		if(pCtrl = Find(IID_TAB_122))	{	pCtrl->Show();	}
		break;

	case 211:
		if(pCtrl = Find(IID_TAB_211))	{	pCtrl->Show();	}
		break;
	case 221:
		if(pCtrl = Find(IID_TAB_211))	{	pCtrl->Show();	}
		if(pCtrl = Find(IID_TAB_221))	{	pCtrl->Show();	}
		break;
	case 222:
		if(pCtrl = Find(IID_TAB_211))	{	pCtrl->Show();	}
		if(pCtrl = Find(IID_TAB_222))	{	pCtrl->Show();	}
		break;

	case 311:
		if(pCtrl = Find(IID_TAB_311))	{	pCtrl->Show();	}		
		break;
	case 321:
		if(pCtrl = Find(IID_TAB_311))	{	pCtrl->Show();	}
		if(pCtrl = Find(IID_TAB_321))	{	pCtrl->Show();	}
		break;
	case 322:
		if(pCtrl = Find(IID_TAB_311))	{	pCtrl->Show();	}
		if(pCtrl = Find(IID_TAB_322))	{	pCtrl->Show();	}
		break;

	case 411:
		if(pCtrl = Find(IID_TAB_411))	{	pCtrl->Show();	}
		break;
	case 421:
		if(pCtrl = Find(IID_TAB_411))	{	pCtrl->Show();	}
		if(pCtrl = Find(IID_TAB_421))	{	pCtrl->Show();	}
		break;
	case 422:
		if(pCtrl = Find(IID_TAB_411))	{	pCtrl->Show();	}
		if(pCtrl = Find(IID_TAB_422))	{	pCtrl->Show();	}
		break;
	}

	
/*
	if(pCtrl = Find(IID_TABBEDPANE) )
	{
		((CTabbedPane*)pCtrl)->SetSelectedIndex( 7 );
	}
*/
}

unsigned int CSkillDLG::Process(UINT uiMsg,WPARAM wParam,LPARAM lParam)
{
	unsigned iProcID = 0;
	if( iProcID = CTDialog::Process(uiMsg,wParam,lParam)) 
	{
		switch(uiMsg)
		{
		case WM_LBUTTONUP:
			On_LButtonUP( iProcID, wParam, lParam );
			break;
		case WM_LBUTTONDOWN:	
			{
				CWinCtrl * pCtrl = NULL;
				
				if( pCtrl = Find(iProcID) )
				{
					pCtrl->GetOffset();
				}
			}
			break;
		default:
			break;
		}
		return uiMsg;
	}
	return 0;
}


void CSkillDLG::Draw()
{

	if( !IsVision() ) return ;
	CTDialog::Draw();

	DrawTexts();
}

bool CSkillDLG::On_LButtonUP( unsigned iProcID , WPARAM wParam, LPARAM lParam )
{
	switch( iProcID )
	{
	case IID_BTN_ICONIZE:
		g_itMGR.AddDialogIcon( 	GetDialogType() );
		break;
	case IID_BTN_CLOSE:
		Hide();
		break;

	default:
		break;
	}

	return true;
}


void CSkillDLG::DrawTexts()
{
	CWinCtrl * pCtrl = NULL;
	if(pCtrl = Find("SKILL_POINT_NUM_TXT"))
	{		
		((CTImage*)pCtrl)->SetText( CStr::Printf("%d", g_pAVATAR->GetCur_SkillPOINT()) );
	}
}

bool CSkillDLG::IsValidLevelUp( int iSkillSlotIdx )
{
	if( iSkillSlotIdx  < 0 ) 
		return false;

	short nSkillIDX = g_pAVATAR->m_Skills.m_nSkillINDEX[iSkillSlotIdx];

	///스킬레벨이 10이상이거나 필요 포인트보다 현재 포인트가 적다면 false
	if ( SKILL_1LEV_INDEX( nSkillIDX) != SKILL_1LEV_INDEX( nSkillIDX + 1 ) ||
		SKILL_LEVEL( nSkillIDX ) + 1   != SKILL_LEVEL( nSkillIDX + 1) ) {
			return false;
		}

		if( g_pAVATAR->Get_NeedPoint2SkillUP( (short)iSkillSlotIdx ) <= 0 )
			return false;

		if( g_pAVATAR->Get_NeedPoint2SkillUP( (short)iSkillSlotIdx ) > g_pAVATAR->GetCur_SkillPOINT() )
			return false;


		int iNeedAbilityType = 0;
		for( int i = 0; i < SKILL_NEED_ABILITY_TYPE_CNT; ++i)
		{
			iNeedAbilityType  = SKILL_NEED_ABILITY_TYPE( nSkillIDX,i );
			if( iNeedAbilityType )
			{
				if( g_pAVATAR->Get_AbilityValue( iNeedAbilityType ) < SKILL_NEED_ABILITY_VALUE( nSkillIDX, i ))
					return false;
			}
		}

		return true;
}

bool CSkillDLG::IsVision()
{
	if( g_pAVATAR == NULL )
		return CTDialog::IsVision();
	else
		return( CTDialog::IsVision() && g_pAVATAR->Get_HP() > 0 );

	return false;
}
//-----------------------------------------------------------------------------------------------------
/// @brief 스킬에 대한 소모 능력치및 기타 변경사항에 대한 인터페이스 적용을 위한 Method
//-----------------------------------------------------------------------------------------------------
void CSkillDLG::UpdateSkillLists()
{
	

}
void CSkillDLG::UpdateSkillListBySkillLevelUp( CZListBox* pListBox )
{

	assert( pListBox );
	if( NULL == pListBox )
		return;

	CSkillListItem* pSkillListItem = NULL;
	int max_size = pListBox->GetSize();

	for( int i = 0 ; i < max_size; ++i )
	{
		pSkillListItem = (CSkillListItem*)pListBox->GetItem( i );
		pSkillListItem->Update();
	}
}

void CSkillDLG::Update( CObservable* pObservable, CTObject* pObj )
{
	assert( pObservable );
	assert( pObj );

	if( pObj == NULL || strcmp( pObj->toString(), "CTEventSkill" ) )
	{
		assert( 0 && "Invalid Event Type or Null" );
		return;
	}

	CTEventSkill* pEvent = (CTEventSkill*)pObj;

	switch( pEvent->GetID() )
	{
	case CTEventSkill::EID_CHANGE_SKILL:
		{
			Update_LearnSkill();
		}
		return;
	}

	// 스킬슬롯 인덱스 (기본스킬, 스킬 2가지로 나뉨 )
	int iSkillSlotIndex = pEvent->GetIndex(); 
	// 입력된 스킬슬롯의 스킬 인덱스.	
	CSkill * pSkill = g_pAVATAR->GetSkillSlot()->GetSkill( iSkillSlotIndex );
	int iSkillIndex = g_pAVATAR->GetSkillSlot()->GetSkill( iSkillSlotIndex )->GetSkillIndex();
	
	switch( pEvent->GetID() )
	{

	case CTEventSkill::EID_ADD_SKILL:
		{	
			CWinCtrl* pCtrl			= NULL;		
			CWinCtrl * pCtrl2		= NULL;

			//Tab 아래에 스킬아이콘을 에드 시킨다.
			int iSlotIndex = SKILL_SLOT_NUM( iSkillIndex );
			int iTab_Type = SKILL_TAB_TYPE(iSkillIndex);			
			int iClass_Set = SKILL_AVAILBLE_CLASS_SET(iSkillIndex);
			int iPaneID = 0;

			//유니크, 파츠는 직업 상관없이 같는 패널을 쓴다.
			if( iSlotIndex == -1)
			{
				iPaneID = iTab_Type*100;

                //마일리지 스킬 텝 4개 사용하기 위해서.				
				pCtrl = Find(iPaneID);
				while( pCtrl )
				{
					//스킬수가 42개가 넘을 경우.
					int iSkillNum = ((CTPane*)pCtrl)->GetChild().size();
					if( iSkillNum < 42)
					{
						break;
					}

					iPaneID += 100;			//다음 탭 판넬로 이동.
					pCtrl = Find(iPaneID);
				}
			}
			else
			{
				iPaneID = iTab_Type*100 + iClass_Set;
			}			

			if( pCtrl = Find(iPaneID) )
			{
				CIconSkill* pIcon		= new CIconSkill( iSkillSlotIndex );
				CSkillListItem* pItem	= new CSkillListItem;
				pItem->Show();
				pItem->SetIcon(pIcon );						

				pItem->SetControlID( SKILL_1LEV_INDEX(iSkillIndex) + IID_SKILLICON );


				POINT pt;				

				if( iSlotIndex == -1)
				{
					//서버에서 보내주는 순서대로 세팅.
					int iSkillNum = ((CTPane*)pCtrl)->GetChild().size();
					pt = Get_SkillSlot_Pos( iSkillNum );
					pItem->SetOffset( pt );
					((CTPane*)pCtrl)->AddChild( pItem );
				}
				else
				{
					pt = Get_SkillSlot_Pos( iSlotIndex );
					pItem->SetOffset( pt );
					((CTPane*)pCtrl)->AddChild( pItem );
				}				
				MoveWindow( GetPosition() );
			}			
		}
		break;

	case CTEventSkill::EID_DEL_SKILL :
		{	
			CWinCtrl	* pCtrl			= NULL;		
			CWinCtrl	* pCtrl2		= NULL;
			CTPane		* pPane			= NULL;

			if( pCtrl = Find( SKILL_1LEV_INDEX(iSkillIndex) + IID_SKILLICON) )
			{				
				if( pCtrl )
				{
					pPane = (CTPane*)Find( pCtrl->GetParent()->GetControlID() );
					if( pPane )
					{
						pPane->DelChild( pCtrl );						
					}					
				}
				MoveWindow( GetPosition() );
			}			
		}
		break;	
	}
}

void CSkillDLG::Set_LearnSkill(int iSkillIndex, int bStatus )
{
	
	CWinCtrl* pCtrl			= NULL;
	CWinCtrl* pCtrl2		= NULL;

	//Tab 아래에 스킬아이콘을 에드 시킨다.
	int iTab_Type = SKILL_TAB_TYPE(iSkillIndex);
	int iClass_Set = SKILL_AVAILBLE_CLASS_SET(iSkillIndex);
	int iPaneID = (iTab_Type*100 + iClass_Set)*10; //XML 패널 아이디			

	//배울수 있는 스킬 판넬 22410 등등. 
	//스킬 판넬 2241 등등.
	//배울수 있는 스킬 아이디를 (스킬번호+100000)

	//clan으로 해줘야 슬롯인덱스 참조하지 않고 스킬 인덱스 사용.
	CSkill* pSkill = g_SkillManager.CreateNewSkill( SKILL_SLOT_CLAN, iSkillIndex );
	
	if( (pCtrl = Find(iPaneID) ) && pSkill )
	{
		int iSkill_Ctrl_Index = iSkillIndex+100000;

		CWinCtrl * pCtrl2 = NULL;
		if( pCtrl2 = Find(iSkill_Ctrl_Index) )
		{			
			switch( bStatus)
			{
			case STAT_SHOW:
				((CSkillListItem*)pCtrl2)->GetIcon()->Show(true);
				if( ((CSkillListItem*)pCtrl2)->GetButton() )
				{
					((CSkillListItem*)pCtrl2)->GetButton()->Show();	
				}
				((CSkillListItem*)pCtrl2)->GetImage()->Show();	
				break;
			case STAT_HIDE:
				((CSkillListItem*)pCtrl2)->GetIcon()->Show(false);			

				if( ((CSkillListItem*)pCtrl2)->GetButton() )
				{
					((CSkillListItem*)pCtrl2)->GetButton()->Hide();
				}
				
				((CSkillListItem*)pCtrl2)->GetImage()->Hide();
				break;
			case STAT_DEL:
				((CTPane*)pCtrl)->DelChild( pCtrl2 );
				break;
			}			
			return;
		}

		if(bStatus==STAT_DEL)	{	return;	}

		int iIconNo = SKILL_ICON_NO( iSkillIndex );

		CIconSkill* pIcon		= new CIconSkill( pSkill );
		CSkillListItem* pItem	= new CSkillListItem;
		//아이콘 색 바꾸기.
		//pIcon->SetDrawColor_Ori( D3DCOLOR_ARGB(255, 200, 200, 200) );
		pItem->SetNotLearnSkill();		
		pItem->SetIcon(pIcon );
		pItem->Show();
		pItem->SetControlID( iSkill_Ctrl_Index );	

		//처음 스킬 배울수 있는 상태 일때 new 이미지 추가				
		if( pCtrl2 = Find("SKILL_NEW") )
		{
			CTImage * pImage = ((CTImage*)pCtrl2)->Clone();			
			pItem->SetImage( pImage );		
		}

		switch( bStatus)
		{
		case STAT_SHOW:
			pItem->GetIcon()->Show(true);
			if( pItem->GetButton() )
			{
				pItem->GetButton()->Show();		
			}			
			pItem->GetImage()->Show();
			break;
		case STAT_HIDE:			
			pItem->GetIcon()->Show(false);
			if( pItem->GetButton() )
			{
				pItem->GetButton()->Hide();
			}			
			pItem->GetImage()->Hide();
			break;		
		}	
		
		POINT pt;
		int iSlotIndex = SKILL_SLOT_NUM( iSkillIndex );
		pt = Get_SkillSlot_Pos( iSlotIndex );
		pItem->SetOffset( pt );
		if( pItem )
		{
			pItem->GetImage()->SetOffset( pt );
		}

		((CTPane*)pCtrl)->AddChild( pItem );

		MoveWindow( GetPosition() );
	}		
	
}


void CSkillDLG::Update_LearnSkill()
{
	CWinCtrl * pCtrl = NULL;
	CSkillListItem * pItem = NULL;
	int i=0, j=0;
	int iSkillIndex = 0;

	
	// 배울 수 있는 스킬을 찾는다. stb.job List 에서	
	int iJob = g_pAVATAR->Get_JOB() / 100 - 1;

	if( iJob >= 0)
	{
		//[root job][comm,1st,2nd,2nd job only]
		for( j=0 ; j<4 ; j++ )
		{
			for( i=0 ; i<(int)g_SkillList.Get_List_Job_Skill( iJob, j ).size() ; i++ )
			{
				iSkillIndex = g_SkillList.Get_List_Job_Skill( iJob, j )[i];			

				Set_LearnCondition( iSkillIndex );
			}
		}
		for( i=0; i<(int)g_SkillList.Get_List_Cart().size(); i++ )
		{
			iSkillIndex = g_SkillList.Get_List_Cart()[i];

			Set_LearnCondition( iSkillIndex );
		}
	}
}

void CSkillDLG::Set_LearnCondition(int iSkillIndex )
{
	BYTE ret = g_pAVATAR->Skill_LearnCondition( iSkillIndex );

	switch(ret)
	{
	case RESULT_SKILL_LEARN_SUCCESS:
		Set_LearnSkill( iSkillIndex, STAT_SHOW );
		break;
	case RESULT_SKILL_LEARN_FAILED:
		//배운스킬은 아이콘을 지운다.
		Set_LearnSkill( iSkillIndex, STAT_DEL );
		break;
	default:
		Set_LearnSkill( iSkillIndex, STAT_HIDE );
		break;
	}
}


void CSkillDLG::RemoveSkillListItem( CZListBox* pListBox, int iSkillSlotIndex )
{
	CSkillListItem* pItem;
	CWinCtrl* pCtrl = NULL;
	CIconSkill* pSkillIcon = NULL;
	for( int i = 0 ; i < pListBox->GetSize(); ++i )
	{
		pCtrl = pListBox->GetItem( i );
		assert( pCtrl );
		if( pCtrl )
		{
			pItem = (CSkillListItem*)pCtrl;
			pSkillIcon = pItem->GetIcon();
			assert( pSkillIcon );
			if( pSkillIcon )
			{
				if( pSkillIcon->GetSkillSlotFromIcon() == iSkillSlotIndex )
				{
					pListBox->DelItem( i );
					break;
				}
			}
		}
	}
}

bool CSkillDLG::Create( const char* IDD )
{
	if( CTDialog::Create( IDD ) )
	{
		return true;
	}
	return false;
}


void CSkillDLG::Show( )
{

#ifdef _DEBUG
	SetInterfacePos_After();
#endif	

	CTDialog::Show();		

	Set_Skill4Job(g_pAVATAR->Get_JOB());

	Update_LearnSkill();

	CWinCtrl * pCtrl = NULL;
	if(pCtrl = Find("SKILL_NEW"))
	{		
		pCtrl->Hide();
	}	
}

void CSkillDLG::RefreshDlg()
{
	CTDialog::RefreshDlg();
}
