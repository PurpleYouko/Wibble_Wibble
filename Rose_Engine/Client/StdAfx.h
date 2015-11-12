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
#include <set>
#include <d3d9.h>
#include <D3DX9.h>


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
// Please leave a comment if you add the preprocessor.
// When you modify pre-processor settings, please just rebuilt.
// 06. 01. 12 - _NoRecover Should go according with the server. In each country, step-by-step demand apply.
//////////////////////////////////////////////////////////////////////////
// Preprocessor settings here.//////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

#ifdef _DEBUG	//Always put if debug. Add only if you need one if you release the.
#define __TEST
#endif

//#define __RELEASE_TEST	//Was added to a release version for testing.

//#define __TEST
//#define __WORK

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

// Korea version of the new open

#ifdef __TEST

#define _DE							// The test version allows overlapping execution.
//#define __USE_WITHIN_COMPANY_TEST	// use inner test << if not commented the packet will be 604

#define __MSG_USE_RETURN			// Use the message box returns.
#define __AUTOLOGIN					//Auto-Login.
#define __SET_PREDEFINED_LANGUAGE	//National pretreatment group language settings


// Unused portion.
//#define __CHGLISTBOX


#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///Common preprocessor s ///

#define _LOAD_NORMAL	1
#define _LOAD_VFS		0
#define __USEJEMBREAK
#define _GBC					// Battle Cart
#define _NoRecover				//  Juhyeon: The HP / MP will keep shoveling it out of the server every second cycle once sent. So climb resilience calculate X
#define _NEWUI					// Honggeun: A renewed interface.
#define __CLEAR_SKILL			// Skill Initialization
#define __CLAN_WAR_SET			// Keulraenjeon 
#define __ITEM_MAX				// Items extensions
#define __KRRE_NEW_AVATAR_DB	//
#define __MILEAGESKILL			// Mileage skills
#define _NEWBREAK				// Decomposition
#define _NEWINTERFACEKJH		// 
#define _NEWSTATUS				//
#define __NEWUPDATEPARTYBUFF
#define	__PRIVATECHAT2			// Additional community features
#define __BATTLECART2			// Information carbide cart
#define __CAL_BUF2				// Buff formula modified.
#define __DIGITEFFECT8			// Honggeun: Damage changed from 04 digits to 08 digits maximum expression.
#define __ENCRYPT2				// Juhyeon: use xor instead of the arc include encryption!
#define __ARCFOUR				// Honggeun: packet encryption
//#define __ARCFOURLOG
#define __EXPERIENCE_ITEM		// Item added experience.

#define __ITEM_TIME_LIMMIT		//Add Term Item. 20070111
#define __KEYBOARD_DIALOG		// Using the keyboard dialog. Pm 3:54 2007-02-26

#define __NEWUPGRADESYSTEM			// Refinement modified.
//Keulraenjeon Primary S
#define __CLAN_INFO_ADD			//
#define __NPC_COLLISION			//
#define __CLAN_WAR_BOSSDLG		// Add keulraenjeon boseumop window.
#define __EDIT_CLANDLG			// 
//클랜전 1차 E

#define __CLAN_STORE				//Clan Points Store.
#define __EDIT_CLANDLG_CLANWAR		//Clan reward points related to the modified window.
#define __NEW_PAT_TYPES			// New pat types (2010-03-21)


#define __BUILD_REGION_NAME   "ROSE Online Evolution"
#define _PACKETRESERVED		 0x0000
#define _WARNING
#define _DE
#define _USA
#define __NoNProtect
#define LAUNCHER_BYPASS_STRING "@TRIGGER_SOFT@"


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
#include "ClientLOG.h"
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

