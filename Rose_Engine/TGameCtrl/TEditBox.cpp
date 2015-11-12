#include "StdAfx.h"
#include ".\teditbox.h"
#include "mbstring.h"
#include "ITFont.h"
#include "TControlMgr.h"
#include "crtdbg.h"
#include "TIme2.h"

#include <algorithm>
#include ".\tsplitstring.h"


CTEditBox*	CTEditBox::s_pFocusEdit = NULL;

CTEditBox::CTEditBox(void)
{
	m_iCharHeight			= 12;
	m_bShowCursor			= true;
	m_bMultiline			= false;
	m_dwType				= TES_NONE;
	m_pFontMgr				= NULL;
	m_iTextAlign			= 0;
	m_dwTextColor			= D3DCOLOR_RGBA(255,255,255,255);

	m_iPromptPos			= 0;
	m_iViewPos				= 0;
	m_iFont					= 0;

	m_bIsSetMuiltLine		= false;
	m_iLineWidth			= 0;
	m_iLineGap				= 0;
	m_iBeforeLineNum		= 0;
	m_iMaxLineCnt			= 100;
	m_iCurLineCnt			= 0;
	

	ZeroMemory( m_pszBuffer, sizeof ( m_pszBuffer ));
}

CTEditBox::~CTEditBox(void)
{
	if( s_pFocusEdit == this )
		s_pFocusEdit = NULL;
}

bool CTEditBox::Create(int iScrX, int iScrY, int iWidth, int iHeight ,int iCharWidth )
{
	m_iMaxCharCnt	= 126;
	m_iCharWidth	= iCharWidth;
	m_pFontMgr		= CTControlMgr::GetInstance()->GetFontMgr();
	Init(CTRL_EDITBOX, iScrX, iScrY, iWidth, iHeight );

	SetWidth( GetWidth() / m_iCharWidth * m_iCharWidth );

	return true;
}

void CTEditBox::SetMultiline( bool bMultiline )
{
	m_bMultiline  = bMultiline;
}

bool CTEditBox::OnKeyDown( WPARAM wParam, LPARAM lParam )
{
	///포커스를 가지고 있지 않았을때 Enter가 들어올수 있다.
	if(!IsFocus()) 
		return false;

	switch(wParam)
	{
	case VK_LEFT:
		{
			if( m_iPromptPos )
			{
				char* pCurrPos = &m_pszBuffer[ m_iPromptPos ];
				char* pPrevPos = CharPrevExA( CTControlMgr::GetInstance()->GetCodePage(), m_pszBuffer, pCurrPos, 0 );
				if( pCurrPos != pPrevPos )
				{
					int iMoveByte = ( pCurrPos - pPrevPos ) / sizeof(char);
					m_iPromptPos  -= iMoveByte;

					if( m_iPromptPos < m_iViewPos )
					{
						const char* pCurrViewPos = &m_pszBuffer[ m_iViewPos ];
						const char* pPrevViewPos = CharPrevExA( CTControlMgr::GetInstance()->GetCodePage(), m_pszBuffer, pCurrViewPos , 0);
						if( pCurrViewPos != pPrevViewPos )
							m_iViewPos -= ( pCurrViewPos - pPrevViewPos ) / sizeof( char ); 
					}
				}
			}
		}
		break;
	case VK_RIGHT:
		{
			char* pCurrPos = &m_pszBuffer[ m_iPromptPos ];
			char* pNextPos = CharNextExA( CTControlMgr::GetInstance()->GetCodePage(), pCurrPos, 0 );
			
			if( pCurrPos != pNextPos )
			{
				m_iPromptPos  += ( pNextPos - pCurrPos ) / sizeof(char);

				///화면에 표시되는 길이를 계산하고 만약 넘는다면 ViewPos를 뒤로 넘긴다.
				_ASSERT( m_iPromptPos > m_iViewPos );
				if( m_iPromptPos > m_iViewPos )
				{
					int iLen = m_iPromptPos - m_iViewPos;
					char* pszTemp = new char[ iLen + 1];

					memcpy( pszTemp, &m_pszBuffer[ m_iViewPos ], iLen );
					pszTemp[iLen] = 0;

					SIZE size = m_pFontMgr->GetFontTextExtent( m_iFont, pszTemp );
					if( size.cx > m_iWidth )
					{
						char* pCurrViewPos = &m_pszBuffer[ m_iViewPos ];
						char* pNextViewPos = CharNextExA( CTControlMgr::GetInstance()->GetCodePage(), pCurrViewPos, 0 );
						if( pCurrViewPos != pNextViewPos )
							m_iViewPos += ( pNextViewPos - pCurrViewPos ) / sizeof( char );
					}
					delete []pszTemp;
				}
			}
		}
		break;
	case VK_HOME:
		{
			m_iPromptPos = 0;
			m_iViewPos   = 0;
			return true;
		}
	case VK_END:
		{
			MoveEnd();
		}
		break;
	case VK_DELETE:
		{
			char* pCurrPos = &m_pszBuffer[ m_iPromptPos ];
			char* pNextPos = CharNextExA( CTControlMgr::GetInstance()->GetCodePage(), pCurrPos, 0 );
			if( pCurrPos != pNextPos )
			{
				int	  iDeletedByte = ( pNextPos - pCurrPos ) / sizeof(char);
				MoveMemory( pCurrPos, pNextPos, strlen( m_pszBuffer ) - m_iPromptPos + iDeletedByte );
			}
		}
		break;
	}
	return false;
}

void CTEditBox::SetFocus(bool bFocus)
{
	CWinCtrl::SetFocus( bFocus );
	if( bFocus )
	{
		if( s_pFocusEdit && s_pFocusEdit != this )
			s_pFocusEdit->SetFocus( false );

		s_pFocusEdit = this;
		if( HIMC old_HIMC = CTIme::GetInstance().GetOldHIMC() )
		{
			ImmAssociateContext( CTControlMgr::GetInstance()->GetWindowHandle(), old_HIMC );
			CTIme::GetInstance().SetOldHIMC( NULL );
			_RPT0( _CRT_WARN,"ImmAssociateContext Old HIMC\n");
		}
		
	}
	else
	{
		if( s_pFocusEdit && s_pFocusEdit == this )
		{
			s_pFocusEdit = NULL;
			HIMC old_HIMC = ImmAssociateContext( CTControlMgr::GetInstance()->GetWindowHandle(), NULL );
			CTIme::GetInstance().SetOldHIMC( old_HIMC );
			_RPT0( _CRT_WARN,"ImmAssociateContext NULL\n");
		}
	}
}

//*-------------------------------------------------------------------------------------*
/// - 사용자의 키보드 입력을 EditBox에 반영한다.
/// @warning WM_KEYUP 처리에 주의할것, 현재 변환중 Key Up에 대한 메세지가 오고 있다.( 여기서 다른 처리시 일본어/중국어 변환시에 sideeffect발생가능성이 높다 )
///          WM_KEYDOWN에서 처리할것을 추천;;
//*-------------------------------------------------------------------------------------*
UINT CTEditBox::Process(UINT uiMsg,WPARAM wParam,LPARAM lParam)
{
	//홍근 : 커서가 없을때 입력도 못하게 수정.
	if( !IsEnable() || IsTypeHideCursor() ) {	return 0;	}
	
	POINT ptMouse = { LOWORD(lParam), HIWORD(lParam ) };

	unsigned uiRet = CWinCtrl::Process( uiMsg, wParam, lParam ) ;

	switch(uiMsg)
	{
	
	case WM_KEYDOWN:///Enter와 Esc처리하자.
		{
			if( IsFocus() )
			{
				//홍근 : 오전 10:09 2006-11-07 : 멀티라인 일때 return 쓸수 있게...
				if(m_bMultiline && wParam==VK_RETURN)
				{
					return m_iControlID;
				}
				else if( ( wParam == VK_ESCAPE || (wParam == VK_RETURN  ) ) 
					&& CTControlMgr::GetInstance()->GetKeyboardInputType() == CTControlMgr::INPUTTYPE_NORMAL  )
				{
					_RPT0( _CRT_WARN,"CTControlMgr::INPUTTYPE_NORMAL SetFocus(false)\n");
					SetFocus( false );
					return m_iControlID;
				}
				else if( OnKeyDown( wParam, lParam ) )
				{
					return m_iControlID;
				}
			}
			else
			{
				if( wParam == VK_RETURN )
				{
					switch( CTControlMgr::GetInstance()->GetKeyboardInputType() )
					{
					case CTControlMgr::INPUTTYPE_NORMAL:
						SetFocus( true );
						return m_iControlID;
					case CTControlMgr::INPUTTYPE_AUTOENTER:
						{
							if( s_pFocusEdit == NULL )
							{
								SetFocus( true );
								return m_iControlID;
							}
						}
						break;
					default:
						break;
					}
				}
			}
		}
		break;
	case WM_CHAR:
		{
			if( (GetAsyncKeyState( VK_CONTROL ) >= 0) && IsFocus() && OnChar( wParam ,lParam ) )
			{
				//_RPT0( _CRT_WARN,"CTEditBox::WM_CHAR\n");		
				return m_iControlID;
			}
		}		
		break;
	case WM_LBUTTONDOWN:
		if(IsInside( ptMouse.x, ptMouse.y )) 
		{
			SetFocus( true );
			m_bShowCursor          = true;
			return m_iControlID;
		}
		break;
		
	}
	return uiRet;
}

void CTEditBox::Draw()
{
	if(!IsVision() || m_pFontMgr == NULL ) return ;

	std::string strView;
	int iInsertPos = 0;



	// 화면에 실제 표시할 SubString을 구한다.
	if( m_bMultiline )
	{
		iInsertPos  = m_iPromptPos;
		if( m_pszBuffer )
			strView = m_pszBuffer;
	}
	else
	{
		
		char* pViewPos = &m_pszBuffer[ m_iViewPos ];
		char* pCurrPos = pViewPos;


		char szBuffer[4096];
		ZeroMemory( szBuffer, sizeof( szBuffer ));

		SIZE sizeString;
		while( true )
		{
			char* pNextPos = CharNextExA( CTControlMgr::GetInstance()->GetCodePage(), pCurrPos, 0 );
			if( pNextPos == pCurrPos )
			{
				strView = szBuffer;
				break;
			}
			
			ZeroMemory( szBuffer, sizeof( szBuffer ));
			CopyMemory( szBuffer, pViewPos, (pNextPos - pViewPos) / sizeof( char ) );
			
			sizeString = m_pFontMgr->GetFontTextExtent( m_iFont, szBuffer );

			if( sizeString.cx > m_iWidth )
			{
				strView = szBuffer;		
				break;
			}
			pCurrPos = pNextPos;
		}		

		iInsertPos = m_iPromptPos - m_iViewPos;

		if( iInsertPos > strView.size() )
			iInsertPos = strView.size();


		if( iInsertPos < 0 )
			iInsertPos = 0;
	}

	int iLength =0;
	if( IsFocus() )
	{
		iLength = strlen(CTIme::GetInstance().GetCompsitionString().c_str());	
		if(iLength)
		{
			if( strView.length() + iLength <= GetMaxCharCnt() )		
			{
				strView.insert( iInsertPos, CTIme::GetInstance().GetCompsitionString() );
			}
			iLength = m_pFontMgr->GetFontTextExtent( m_iFont, CTIme::GetInstance().GetCompsitionString().c_str() ).cx;
		}		
	}
	/// 출력될 위치를 변경한다.
	m_pFontMgr->SetTransformSprite( (float)m_sPosition.x, (float)m_sPosition.y );

	// 조합중인 문자열 공간(iLength) 추가 표시.
	RECT rcDraw = { 0, 0, m_iWidth+iLength, m_iHeight };

	DWORD TextAlign = 0;

	
	switch( m_iTextAlign )
	{
	case TES_ALIGN_RIGHT:
		TextAlign =  DT_RIGHT ;
		break;
	case TES_ALIGN_CENTER:
		TextAlign =  DT_CENTER ;
		break;
	default://TES_ALIGN_LEFT
		TextAlign =  DT_LEFT ;
		break;
	}

	if( m_bMultiline )
	{
		TextAlign |= DT_WORDBREAK;		
	}

	if( IsTypePassword() )
		strView.replace( strView.begin(), strView.end(), strView.size(),'*');

	CTSplitString TempString;
	TempString.m_iMaxLine = m_iMaxLineCnt;

	std::string strLastString;
	BOOL bOverFlow = false;
	if( !strView.empty() )
	{
		if( m_bMultiline )
		{
#ifdef _NEWUI
		
			TempString.Split( m_iFont, strView.c_str(), GetWidth(), CTControlMgr::GetInstance()->GetCodePage() );
			int i;
			for( i = 0; i < TempString.GetLineCount(); ++i )
			{
				if( i >= m_iMaxLineCnt )
				{					
					bOverFlow = true;
					break;
				}

                m_pFontMgr->Draw( m_iFont, true, &rcDraw, m_dwTextColor,  TextAlign, TempString.GetString(i) );
				
				rcDraw.top		+= m_pFontMgr->GetFontHeight( m_iFont ) + 1;
				rcDraw.bottom	+= m_pFontMgr->GetFontHeight( m_iFont ) + 1;												
			}

			//현재 라인수 저장.
			m_iCurLineCnt = TempString.GetLineCount();
			strLastString = TempString.GetString(i-1);
#endif
		}
		else
		{
            m_pFontMgr->Draw( m_iFont, true, &rcDraw, m_dwTextColor,  TextAlign, strView.c_str() );		
		}
	}


	///프롬프트그리기	
	if( (IsFocus() && !IsTypeHideCursor() && TextAlign == DT_LEFT) || (m_bMultiline&&IsFocus() ))
	{
		CTIme& TIme = CTIme::GetInstance();


		std::string strTemp;
		SIZE size;
		SIZE sz;

		if(m_bMultiline)
		{
			int CurserPos = TIme.GetCursorPos();			
			sz = TempString.GetColmnRow(iInsertPos);
			strLastString = TempString.GetString(sz.cy);
			strTemp = strLastString.substr(0, sz.cx);
			size = m_pFontMgr->GetFontTextExtent( m_iFont, strTemp.c_str() );
			size.cy = m_pFontMgr->GetFontHeight( m_iFont );

			rcDraw.top		= (size.cy+1) * sz.cy    + 1;
			rcDraw.bottom   = (size.cy+1) *(sz.cy+1) + 1;

			
			if( strLastString.find("\n") == -1 
				|| strLastString.length() > sz.cx  )
			{
				rcDraw.left    = size.cx;
				rcDraw.right   = size.cx + 10;///고정
			}
			else
			{
				if( m_iCurLineCnt <= m_iMaxLineCnt )
				{
					//엔터 입력시 커서 위치 세팅.
					rcDraw.left    = 0;
					rcDraw.right   = 10;///고정

					rcDraw.top		+= (size.cy+1);
					rcDraw.bottom   += (size.cy+1);

				}
			
			}		
		}	
		else
		{
			strTemp = strView.substr( 0, iInsertPos + TIme.GetCursorPos() );

			size = m_pFontMgr->GetFontTextExtent( m_iFont, strTemp.c_str() );
			
			rcDraw.left    = size.cx ;
			rcDraw.right   = size.cx + 10;///고정				
			rcDraw.top		+= 1;
			rcDraw.bottom   += 1;
		}		
		if(!bOverFlow)
		{
			m_pFontMgr->Draw( m_iFont, true, &rcDraw, m_dwTextColor,  0, /*TIme.GetStrPrompt().c_str()*/"_" );			
		}		

		std::vector<DWORD>::iterator iterClauseInfo;
		std::vector<DWORD>& compClauseInfo = TIme.GetCompClauseInfo();
		std::vector<BYTE>& compAttr = TIme.GetCompAttr();

		std::string strunderline;
		DWORD dwClauseLen;
		if( !compClauseInfo.empty() )
		{
			SetRect( &rcDraw,0, 0, m_iWidth, m_iHeight );
			if( iInsertPos == 0 )
			{
				size		= m_pFontMgr->GetFontTextExtent( m_iFont , strView.c_str() );
				rcDraw.left	= 0;
			}
			else
			{
				strTemp		= strView.substr( 0, iInsertPos );
				size		= m_pFontMgr->GetFontTextExtent( m_iFont , strTemp.c_str() );
				rcDraw.left	= size.cx;
			}

			rcDraw.top		+= size.cy / 2 + 2;
			rcDraw.bottom	+= rcDraw.top + size.cy;
				
			for( iterClauseInfo = compClauseInfo.begin(); iterClauseInfo != compClauseInfo.end(); ++iterClauseInfo )
			{
				if( *iterClauseInfo == TIme.GetCompsitionString().length() )
					break;
				
				strunderline.clear();
				dwClauseLen	= *(iterClauseInfo + 1) - *iterClauseInfo;

				if(!strTemp.empty())
					strTemp = strView.substr( iInsertPos + (*iterClauseInfo), dwClauseLen );

				size = m_pFontMgr->GetFontTextExtent( 0 , strTemp.c_str() );

				rcDraw.right = rcDraw.left + size.cx - 1;

				for( int i = 0; i < dwClauseLen ; ++i )///속성에 따라서 Underline이 틀리게 해야한다.
				{
					switch( compAttr[ *iterClauseInfo ] )
					{
					case ATTR_TARGET_CONVERTED:
						strunderline.append( TIme.GetStr_ATTR_TARGET_CONVERTED() );
						break;
					case ATTR_CONVERTED:
						strunderline.append( TIme.GetStr_ATTR_CONVERTED() );
						break;
					case ATTR_INPUT:
						strunderline.append( TIme.GetStr_ATTR_INPUT() );///그냥 입력중
					default:
						break;
					}

					//strunderline.append( TIme.GetFullwidthUnderline() );
				}
				m_pFontMgr->Draw( m_iFont, true, &rcDraw, m_dwTextColor, TextAlign, strunderline.c_str() );
				rcDraw.left += size.cx;///다음절 대비
			}
		}
	
		POINT pt = m_sPosition;
		pt.x	+= rcDraw.left;
		CTIme::GetInstance().SetCandidateWindowPosition( pt );
	}
}


char* CTEditBox::get_text()
{
	return m_pszBuffer;
}

void CTEditBox::SetText( const char* szTxt )
{
	if( szTxt == NULL ) return;

	if( strlen(szTxt) < sizeof( m_pszBuffer ) )
	{
		ZeroMemory( m_pszBuffer, sizeof( m_pszBuffer ) );
		CopyMemory( m_pszBuffer, szTxt, strlen( szTxt ));
		MoveEnd();
	}
}


bool CTEditBox::chk_number( const char* szText )
{
	if( szText == NULL )
		return false;

	for( int i = 0; i < (int)strlen( szText ); ++i )
	{
		if( szText[i] < '0' || szText[i] > '9' )
			return false;
	}
	return true;
}

void CTEditBox::clear_text()
{
	ZeroMemory( m_pszBuffer, sizeof( m_pszBuffer ));
	m_iPromptPos	= 0;
	m_iViewPos		= 0;
}

void CTEditBox::delete_one_char(int iType)
{
	/// 0 : delete key
	/// 1 : backspace
	switch( iType )
	{
	case 0:
		{
			char* pCurrPos = &m_pszBuffer[ m_iPromptPos ];
			char* pNextPos = CharNextExA( CTControlMgr::GetInstance()->GetCodePage(), pCurrPos, 0 );
			if( pCurrPos != pNextPos )
			{
				int	  iDeletedByte = ( pNextPos - pCurrPos ) / sizeof(char);
				MoveMemory( pCurrPos, pNextPos, strlen( m_pszBuffer ) + 1 - m_iPromptPos );
			}
			_RPT2( _CRT_WARN,"VK_DEL: m_iViewPos %d, m_iPromptPos %d\n", m_iViewPos , m_iPromptPos );
		}
		break;
	case 1:
		{
			char* pCurrPos = &m_pszBuffer[ m_iPromptPos ];
			char* pPrevPos = CharPrevExA( CTControlMgr::GetInstance()->GetCodePage(), m_pszBuffer, pCurrPos, 0 );
			if( pCurrPos != pPrevPos )
			{
				if( m_iViewPos )
				{
					const char* pCurrViewPos = &m_pszBuffer[ m_iViewPos ];
					const char* pPrevViewPos = CharPrevExA(CTControlMgr::GetInstance()->GetCodePage(), m_pszBuffer, pCurrViewPos, 0 );
					if( pCurrViewPos != pPrevViewPos )
						m_iViewPos -= (pCurrViewPos - pPrevViewPos ) / sizeof( char);
				}
				//_ASSERT( g_iPromptPos - g_iViewPos <= g_iMaxView );
				//_ASSERT( g_iViewPos >= 0 && g_iViewPos <= g_iPromptPos );

				int	  iDeletedByte = ( pCurrPos - pPrevPos ) / sizeof(char);
				MoveMemory( pPrevPos, pCurrPos, strlen( m_pszBuffer ) + 1 - m_iPromptPos );
				m_iPromptPos -= iDeletedByte;


				//_ASSERT( g_iPromptPos <= strlen( g_pszBuffer ) );
				//_ASSERT( g_iPromptPos < sizeof(g_pszBuffer) );
				//_ASSERT( g_iPromptPos >= 0 );
			}
		}
		break;
	default:
		break;
	}
}

void CTEditBox::Hide()
{
	CWinCtrl::Hide();
	SetFocus( false );
	clear_text();
}


void CTEditBox::AppendText( const char* szText )
{
	if( szText == NULL )
		return;

	int iLen	= (int)strlen(szText);
	int	iOrgLen = (int)strlen(m_pszBuffer);

	if( m_iMaxCharCnt < iOrgLen + iLen )	
		return;		

	if( GetType() & TES_NUMBER )
		if(!chk_number( szText ))
			return;



	strcat( m_pszBuffer, szText );

	MoveEnd();
}


void CTEditBox::SetTextColor( int iColor )
{
	switch( iColor )
	{
	case 1:
		m_dwTextColor	= D3DCOLOR_RGBA(0,0,0,255);
		break;
	case 0:
	default:
		m_dwTextColor	= D3DCOLOR_RGBA(255,255,255,255);
		break;
	}
}

void CTEditBox::SetCharHeight( int iHeight )
{
	m_iCharHeight = iHeight;
}

void CTEditBox::Show()
{
	CWinCtrl::Show();

	//홍근 : 불편해서 하이드 커서 모드가 아니면 항상 포커스 맞춰지는 것으로 수정.
	//if( CTControlMgr::GetInstance()->GetKeyboardInputType() == CTControlMgr::INPUTTYPE_AUTOENTER )
	if(!IsTypeHideCursor() )
	{
		SetFocus( true );
	}

	if(m_bMultiline)
	{
		m_iMaxLineCnt = GetHeight()/(m_pFontMgr->GetFontHeight( m_iFont )+1);
	}
}

bool CTEditBox::OnChar( WPARAM wParam, LPARAM lParam )
{
	switch( wParam & 0xff )
	{
	case VK_BACK:///
		{
			char* pCurrPos = &m_pszBuffer[ m_iPromptPos ];
			char* pPrevPos = CharPrevExA( CTControlMgr::GetInstance()->GetCodePage(), m_pszBuffer, pCurrPos, 0 );
			if( pCurrPos != pPrevPos )
			{
				int	  iDeletedByte = ( pCurrPos - pPrevPos ) / sizeof(char);
				MoveMemory( pPrevPos, pCurrPos, strlen( m_pszBuffer ) - m_iPromptPos + iDeletedByte );
				m_iPromptPos -= iDeletedByte;

				if( m_iViewPos )
				{
					const char* pCurrViewPos = &m_pszBuffer[ m_iViewPos ];
					const char* pPrevViewPos = CharPrevExA(CTControlMgr::GetInstance()->GetCodePage(), m_pszBuffer, pCurrViewPos, 0 );
					if( pCurrViewPos != pPrevViewPos )
						m_iViewPos -= (pCurrViewPos - pPrevViewPos ) / sizeof( char);
				}
			}
			break;
		}
	default:
		{

			if( ((wParam & 0xff) != 27) 
				&& (( wParam & 0xff) != 9 ) 
				//&& ( (wParam != VK_RETURN) ) 
				)				
			{
				if( wParam == VK_RETURN && !m_bMultiline )
				{
					break;
				}
				if( m_iCurLineCnt == m_iMaxLineCnt && wParam==VK_RETURN )
				{
					break;
				}
				if( m_iCurLineCnt > m_iMaxLineCnt )
				{
					break;
				}
				///숫자만 입력받을때는 숫자인지를 체크한다.							
				if( IsTypeNumber() )
					if( wParam < '0' || wParam > '9' )
						break;

				if( IsDBCSLeadByteEx( CTControlMgr::GetInstance()->GetCodePage(), wParam & 0xff ) )
				{					

					//if( strlen( m_pszBuffer ) + 2 > m_iMaxCharCnt )									
					if( strlen( m_pszBuffer ) + 1 > m_iMaxCharCnt )
					{
						break;
					}
				}
				else				
				{
					if( strlen( m_pszBuffer ) >= m_iMaxCharCnt )
					{
						break;
					}						
				}
				//if( strlen( m_pszBuffer )
					
				MoveMemory( &m_pszBuffer[m_iPromptPos+1], &m_pszBuffer[m_iPromptPos], strlen( m_pszBuffer ) - m_iPromptPos );
				m_pszBuffer[m_iPromptPos] = wParam & 0xff;
				m_iPromptPos++;
		
#pragma message("2바이트 문자중에서 1번째 바이트만 들어오면 어떻게 될까나?")
				if( m_iPromptPos > m_iViewPos )
				{
					int iLen = m_iPromptPos - m_iViewPos;
					char* pszTemp = new char[iLen + 1];

					memcpy( pszTemp, &m_pszBuffer[ m_iViewPos ], iLen );
					pszTemp[iLen] = 0;

					SIZE size = m_pFontMgr->GetFontTextExtent( m_iFont, pszTemp );
					if( size.cx > m_iWidth )
					{
						char* pCurrViewPos = &m_pszBuffer[ m_iViewPos ];
						char* pNextViewPos = CharNextExA( CTControlMgr::GetInstance()->GetCodePage(), pCurrViewPos, 0 );
						if( pCurrViewPos != pNextViewPos )
							m_iViewPos += ( pNextViewPos - pCurrViewPos ) / sizeof( char );
					}
					delete []pszTemp;
				}


				//if( m_iPromptPos  > m_iViewPos + m_iMaxView )
				//{
				//	const char* pCurrViewPos = &m_pszBuffer[ m_iViewPos ];
				//	const char* pNextViewPos = CharNextExA( CTControlMgr::GetInstance()->GetCodePage(), pCurrViewPos, 0 );
				//	if( pNextViewPos != pCurrViewPos )
				//		m_iViewPos += ( pNextViewPos - pCurrViewPos ) / sizeof( char );
				//}




				//_RPT2( _CRT_WARN,"WM_CHAR: m_iViewPos %d, m_iPromptPos %d\n", m_iViewPos , m_iPromptPos );
			}
		}
		break;
	}
	return 1;
}

void CTEditBox::MoveEnd()
{
	m_iPromptPos   = strlen( m_pszBuffer );
	m_iViewPos     = m_iPromptPos;
	char* pCurrPos = &m_pszBuffer[ m_iPromptPos ];
	char* pPrevPos = CharPrevExA( CTControlMgr::GetInstance()->GetCodePage(), m_pszBuffer, pCurrPos, 0 );
	SIZE sizeString;

	while( pPrevPos != pCurrPos )
	{
		sizeString = m_pFontMgr->GetFontTextExtent( m_iFont, pPrevPos );
		if( sizeString.cx > m_iWidth )
			break;

		m_iViewPos -= ( pCurrPos - pPrevPos ) / sizeof( char );

		pCurrPos = pPrevPos;
		pPrevPos = CharPrevExA( CTControlMgr::GetInstance()->GetCodePage(), m_pszBuffer, pCurrPos, 0 );
	}
}

void CTEditBox::Insert( const char* text )
{
	if( text == NULL )
		return;

	int iLen	= (int)strlen(text);
	int	iOrgLen = (int)strlen(m_pszBuffer);

	if( m_iMaxCharCnt < iOrgLen + iLen )	
		return;		

	if( GetType() & TES_NUMBER )
		if(!chk_number( text ))
			return;

	if( iOrgLen <= m_iPromptPos )
	{
		strcat( m_pszBuffer, text );
	}
	else
	{
		MoveMemory( &m_pszBuffer[m_iPromptPos+iLen],&m_pszBuffer[m_iPromptPos],iOrgLen - m_iPromptPos ); 
		CopyMemory( &m_pszBuffer[m_iPromptPos], text, iLen );
	}
	m_iPromptPos += iLen;
}

void CTEditBox::SetFont( int iFont )
{
	m_iFont = iFont;
}
int CTEditBox::GetMaxCharCnt()
{
	return m_iMaxCharCnt;
}

void CTEditBox::SetTextAlign( int iAlign )
{
	m_iTextAlign = iAlign ; 
}
