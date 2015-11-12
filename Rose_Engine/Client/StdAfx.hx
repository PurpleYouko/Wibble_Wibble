// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#ifdef	__SERVER
#include <VCL.h>
#else
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#define	_WIN32_WINNT			0x0400

// Windows Header Files:
#include <windows.h>
#include <winUser.h>
#include <mmsystem.h> 
// C RunTime Header Files
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <crtdbg.h>
#include <string>
#include <d3d9.h>
#include <D3DX9.h>


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
// 전처리기 추가하면 주석 남겨주세요.
// 전처리기 세팅 수정하면 리빌드 꼭 해주세요.
// 06. 01. 12 - _NoRecover 서버랑 맞춰서 들어가야함. 각 국가별 단계별로 적용 요망.
//////////////////////////////////////////////////////////////////////////
// 전처리기 세팅 여기에.//////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

#ifdef _DEBUG	//디버그일 경우에는 항상 넣고. 릴리즈 일경우에는 필요한 경우만 넣는다.
#define __TEST
#endif

//#define __RELEASE_TEST	//릴리즈 버전으로 테스트용으로 사용하기 위해 추가했다.

//#define __TEST
//#define __WORK

//#define __KOREA_NEW
//#define __HEBAN
//#define __ARUA   //한국 테스트서버용(ARUA)

//#define __JAPAN_NEW
//#define __JAPAN

//#define _PHILIPPIN_EVO
//#define _TALA_EVO
#define _USA_EVO			// 미국 본섭용 에볼루션
//#define _USA_EVO_TEST		// 미국 사내테스트용 에볼루션
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

// 새로 오픈하는 한국버전 

#ifdef __TEST

#define _DE							// 테스트 버전은 중복 실행 가능하게 한다.
#define __USE_WITHIN_COMPANY_TEST	// 사내 테스트용 암호화 해제!

#define __MSG_USE_RETURN			// 메세지 박스 리턴 사용.
#define __AUTOLOGIN					//자동로그인.
#define __SET_PREDEFINED_LANGUAGE	//국가 전처리기로 언어 세팅


// 미사용 부분.
//#define __CHGLISTBOX
//#define __DROP_SOUND					//드랍 사운드.


#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///공통 전처리기s///

#define __USEJEMBREAK
#define _GBC					// 배틀카트
#define _NoRecover				//  주현 : HP / MP가 계속 삽질 일어나서 서버에서 1초마다 한번씩 보내주기로 했다. 그래서 클라회복력 계산 X
#define _NEWUI					// 홍근 : 리뉴얼 되는 인터페이스.
#define __CLEAR_SKILL			// 스킬 초기화
#define __CLAN_WAR_SET			// 클랜전 
#define __ITEM_MAX				// 아이템 확장
#define __KRRE_NEW_AVATAR_DB	//
#define __MILEAGESKILL			// 마일리지 스킬
#define _NEWBREAK				// 분해
#define _NEWINTERFACEKJH		// 
#define _NEWSTATUS				//
#define __NEWUPDATEPARTYBUFF
#define	__PRIVATECHAT2			// 커뮤니티 기능 추가
#define __BATTLECART2			// 카트 정탄화
#define __CAL_BUF2				// 버프 계산식 수정.
#define __DIGITEFFECT8			// 홍근 : 데미지 최대 표현 4자리에서 8자리로 변경.
#define __ENCRYPT2				// 주현 : 아크포 암호화 대신에 xor 사용!
#define __ARCFOUR				// 홍근 : 패킷 암호화 작업
#define __EXPERIENCE_ITEM		// 경험치 아이템 추가.

#define __ITEM_TIME_LIMMIT		//기간제 아이템 추가. 20070111
#define __KEYBOARD_DIALOG		// 대화창 키보드 사용. 오후 3:54 2007-02-26

#define __NEWUPGRADESYSTEM			// 제련 수정.
//클랜전 1차 S
#define __CLAN_INFO_ADD			//
#define __NPC_COLLISION			//
#define __CLAN_WAR_BOSSDLG		// 클랜전 보스몹 창 추가.
#define __EDIT_CLANDLG			// 
//클랜전 1차 E

#define __CLAN_STORE				//클랜 포인트 상점.
#define __EDIT_CLANDLG_CLANWAR		//클랜창 수정 중에 보상 포인트 관련.

///공통 전처리기e///
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef __KOREA_NEW
#define _PACKETRESERVED		 28283
#define _DE						// 중복 실행.
#define __NEWKOREA				//
#define _SCREENCAPTURE			// 홍근 : 스크린 캡쳐 방수정, 서버에서 시간 받아서 뿌려줌.
#endif

//////////////////////////////////////////////////////////////////////////

#ifdef __ARUA					// 한국 테섭용

#define _PACKETRESERVED		 28283
#define _SCREENCAPTURE			// 홍근 : 스크린 캡쳐 방수정, 서버에서 시간 받아서 뿌려줌.
#define _DE						// 중복 실행.

#endif

//////////////////////////////////////////////////////////////////////////

#ifdef __HEBAN					// 한국 테섭용

#define _PACKETRESERVED		 28283
#define _SCREENCAPTURE			// 홍근 : 스크린 캡쳐 방수정, 서버에서 시간 받아서 뿌려줌.
#define _DE						// 중복 실행.
#endif

//////////////////////////////////////////////////////////////////////////
// 일본 리뉴얼 버젼
#ifdef __JAPAN_NEW
#define _PACKETRESERVED		34517 // 주현 : 사내는 604 / 유저용은 34517
#define __JAPAN_BILL			// 홍근 : 일본 빌링 관련 수정
#define __AUTOLOGIN				//자동로그인.
#endif

//////////////////////////////////////////////////////////////////////////
//필리핀 에보 서버
#ifdef _PHILIPPIN_EVO
#define _PACKETRESERVED		 12356 
#define _PHILIPPIN
#endif

//////////////////////////////////////////////////////////////////////////
//탈라 에볼루션
#ifdef _TALA_EVO
#define _PACKETRESERVED		 12356
#define _PHILIPPIN
#define _WARNING
#endif

//////////////////////////////////////////////////////////////////////////
// 미국 에볼루션 /////////////////////////////////////////////////////////////////////////
#ifdef _USA_EVO
#define _PACKETRESERVED		 9732
#define _WARNING
#define _DE						// 중복 실행.
#define _USA
#endif

//////////////////////////////////////////////////////////////////////////
// 미국 사내테스트용 에볼루션 /////////////////////////////////////////////////////////////////////////
#ifdef _USA_EVO_TEST
#define _PACKETRESERVED		 9732
#define _WARNING
#define _DE						// 중복 실행.
#define _USA
#define __NoNProtect			// nProtect 제거.
#endif
/*
//=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=
// 아래의 국가들은 서비스가 종료 되었습니다.
//=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=

// 한국 
#ifdef __KOREA
#define _PACKETRESERVED		8134
#define __ITEM_MAX				// 아이템 확장
#define _NoRecover
#define _SCREENCAPTURE
#define _DE						// 중복 실행.
#define __ARCFOUR				// 홍근 : 패킷 암호화 작업
#endif


// 필리핀 본서버
#ifdef __PHILIPPIN
#define _PACKETRESERVED		13487 // PH / TALA_EVO는 로그인 서버를 따로 사용합니다.
#define __ITEM_MAX				// 아이템 확장
#define __ARCFOUR				// 홍근 : 패킷 암호화 작업
#define _NoRecover
#define _GBC
#define _NEWINTERFACEKJH
#define _DE						// 중복 실행.
#define _PHILIPPIN
#endif


// 미국 본서버////////////////////////////////////////////////////////////////////////////
#ifdef __USA
#define _PACKETRESERVED		19831
#define __ITEM_MAX				// 아이템 확장
#define _NoRecover
#define _WARNING 
#define _GBC
#define _NEWINTERFACEKJH
#define _DE						// 중복 실행.
#define _USA
#define __ARCFOUR				// 홍근 : 패킷 암호화 작업
#endif

// 필리핀 탈라서버
#ifdef _TALA
//기존 서비스
#define _PACKETRESERVED		6915
#define __ARCFOUR				// 홍근 : 패킷 암호화 작업
#define _NoRecover
#define _GBC
#define _NEWINTERFACEKJH
#define _WARNING
#define _DE						// 중복 실행.
#define _PHILIPPIN

#endif


// 유럽 본서버////////////////////////////////////////////////////////////////////////////
#ifdef _EU

#define _NoRecover
#define _GBC
#define _NEWINTERFACEKJH
#define _DE						// 중복 실행.
#define _EU
#endif


// 일본 
#ifdef __JAPAN
//#define _PACKETRESERVED		5324
#define _DE						// 중복 실행.
#define _WARNING
#define __JAPAN_BILL			// 홍근 : 일본 빌링 관련 수정
//#define __ARCFOUR				// 홍근 : 패킷 암호화 작업

#endif

// 타이완 본서버
#ifdef _TAIWAN

#define _GBC
#define _NEWINTERFACEKJH
#define _DE						// 중복 실행.

#endif
*/

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////




// TODO: reference additional headers your program requires here
//-------------------------------------------------------------------------------------------------

typedef	LPDIRECT3DDEVICE9			LPD3DDEVICE;
typedef	IDirect3DTexture9			ID3DTexture;
typedef	LPDIRECT3DTEXTURE9			LPD3DTEXTURE;
typedef	D3DMATERIAL9				D3DMATERIAL;
typedef	LPDIRECT3DVERTEXBUFFER9		LPD3DVERTEXBUFFER;

#define	ZZ_MEM_OFF	// not use zz_mem

#include "zz_interface.h"

// #pragma warning(default : 4786)
#pragma warning( disable : 4651 ) 

#include "util\CVector.h"
#include "common\DataTYPE.h"

#include "util\CD3DUtil.h"
#include "util\classSTR.h"
#include "util\classLOG.h"
#include "util\classLUA.h"
#include "util\classHASH.h"
#include "util\classUTIL.h"
#include "util\SLLIST.h"
#include "util\DLLIST.h"
#include "util\CFileLIST.h"

#include "util\CGameSTR.h"
#include "util\CGameSTB.h"

#include "CApplication.h"

#endif
//-------------------------------------------------------------------------------------------------

#ifndef SAFE_DELETE
#define SAFE_DELETE(p)       { if(p) { delete (p);     (p)=NULL; } }
#endif

#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[] (p);   (p)=NULL; } }
#endif

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }
#endif


#ifndef	GET_WHEEL_DELTA_WPARAM
#define GET_WHEEL_DELTA_WPARAM(wParam)  ((short)HIWORD(wParam))
#endif

//-------------------------------------------------------------------------------------------------

