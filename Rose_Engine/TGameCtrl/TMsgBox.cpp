#include "StdAfx.h"
#include ".\tmsgbox.h"
#include "TListBox.h"
#include "TButton.h"
#include "TImage.h"
#include "TCaption.h"
#include "TControlMgr.h"
#include "ITFont.h"
const short LINESPACE_BUTTONDROW = 2;///��ư�� �׸��� ���� ���� ���μ� 
CTMsgBox::CTMsgBox(void)
{
#ifdef _NEWUI
	m_iExtraHeight		= 20;
#else
	m_iExtraHeight		= 0;
#endif
	m_iExtraHeightStartPoint = 0;

	m_pListBox			= NULL;
	m_iButtonType		= BT_OK;
	m_pButtonOk			= NULL;
	m_pButtonCancel		= NULL;
	m_pCaption			= NULL;
	m_pImageTop			= NULL;
	m_pImageMiddle		= NULL;
	m_pImageBottom		= NULL;
//	m_iDialogID			= 0;
	ZeroMemory( &m_ptCaptionClicked, sizeof( POINT ));
	m_bUseDrawItem = false;
}

CTMsgBox::~CTMsgBox(void)
{
	if( m_pButtonOk )
	{
		delete m_pButtonOk;
		m_pButtonOk = NULL;
	}
	if( m_pButtonCancel )
	{
		delete m_pButtonCancel;
		m_pButtonCancel = NULL;
	}
	
	if( m_pImageTop )
	{
		delete m_pImageTop;
		m_pImageTop = NULL;
	}
	if( m_pImageMiddle )
	{
		delete m_pImageMiddle; 
		m_pImageMiddle = NULL;
	}
	if( m_pImageBottom )
	{
		delete m_pImageBottom;
		m_pImageBottom = NULL;
	}
	SAFE_DELETE(m_pListBox);
}

unsigned int CTMsgBox::Process( UINT uiMsg,WPARAM wParam,LPARAM lParam )
{
	if( !IsVision() )
		return 0;

	POINT ptMouse = { LOWORD(lParam), HIWORD(lParam ) };

	if( m_pCaption )
	{
		m_pCaption->Process( uiMsg, wParam, lParam );
		if( m_pCaption->IsClicked() && uiMsg == WM_LBUTTONDOWN )
		{
			m_ptCaptionClicked.x = ptMouse.x - m_sPosition.x;
			m_ptCaptionClicked.y = ptMouse.y - m_sPosition.y;
		}
	}

	//���ͷ� â�ݱ� �߰�
	//07. 01. 15 - ������ : MsgBox�� ��ư�� 2���ϰ�쿡 EnterŰ�� ������ Ok, ESCŰ�� ������ Cancel �����ϰ� ����.
	//��ư�� 1���� ��쿣 Enter / ESC �Ѵ� Ok ����.
	if( m_pButtonOk && m_iButtonType == (BT_OK | BT_CANCEL) && ( IsModal() || m_pFocusDlg==this ) )
	{
		switch( uiMsg )
		{
		case WM_KEYDOWN:
			{
				switch( wParam)
				{
				case VK_RETURN:
					{
						return m_pButtonOk->GetControlID();
					}
					break;
				case VK_ESCAPE:
					{
						return m_pButtonCancel->GetControlID();
					}
					break;
				}
			}
			break;
		}
	}
	else if( m_pButtonOk && ( IsModal() || m_pFocusDlg==this ) )
	{
		switch( uiMsg )
		{
		case WM_KEYDOWN:
			{
				switch( wParam)
				{
				case VK_RETURN:
					{
                        return m_pButtonOk->GetControlID();
					}
					break;
				case VK_ESCAPE:
					{
						return m_pButtonOk->GetControlID();
					}
				}
			}
			break;
		}
	}

	if( m_pButtonOk && m_pButtonOk->Process( uiMsg, wParam, lParam))
		return m_pButtonOk->GetControlID();

	if(	m_pButtonCancel  && m_pButtonCancel->Process( uiMsg, wParam, lParam))
		return m_pButtonCancel->GetControlID();


	if( IsInside( ptMouse.x, ptMouse.y ))
		return m_iControlID;
	

	return 0;
}

void CTMsgBox::Update( POINT ptMouse )
{
	if( !IsVision() )
		return;
	if( m_pCaption )
	{
		if(m_pCaption->IsClicked())
		{
			POINT ptNew = {0,0 };
			ptNew.x = ptMouse.x - m_ptCaptionClicked.x;
			ptNew.y = ptMouse.y - m_ptCaptionClicked.y;
			MoveWindow( ptNew );
		}

		m_pCaption->Update( ptMouse );
	}

	if( m_pButtonOk )
		m_pButtonOk->Update( ptMouse );
	if(	m_pButtonCancel )
		m_pButtonCancel->Update( ptMouse );
}


void CTMsgBox::Draw()
{
	if( !IsVision() )
		return;

	DrawImages();
	int iStringCount = m_message_parser.GetStringCount();

	if( iStringCount && m_pImageMiddle )
	{
		CTControlMgr::GetInstance()->GetFontMgr()->SetTransformSprite( (float)m_sPosition.x, (float)m_sPosition.y );
		
		int y = 0;
#ifdef _NEWUI
		// �޼����ڽ��� �������� �׷��� �Ѵٸ�..
		
		if(m_bUseDrawItem)
		{
			y = 35;
		}	
		else
		{
			if(m_pImageMiddle)
			{
				y = (m_pImageMiddle->GetHeight() - iStringCount * 22) / 2 + m_pImageTop->GetHeight();
			}			
		}
				
#else
		y = 30;
#endif		

		CJString* pJString = NULL;

		for( int i = 0; i < iStringCount ; ++i )
		{
			if( pJString = m_message_parser.GetString( i ) )
				pJString->Draw( 15, y, false );

			y += 22; // �޽����� �� ����
		}
	}


	if( m_pButtonOk )
	{
		m_pButtonOk->Draw();
	}

	if(	m_pButtonCancel )
	{
		m_pButtonCancel->Draw();
	}
}


/// ��ư���� ���¿� x��ǥ����
void CTMsgBox::SetButtonType( short int iType )
{
	m_iButtonType = iType;
	POINT ptOffset = {0,0};
	switch( m_iButtonType )
	{
	case ( BT_OK | BT_CANCEL ):
		{
			if( m_pButtonOk )
			{
				ptOffset   = m_pButtonOk->GetOffset();
				ptOffset.x = m_iWidth / 4 - m_pButtonOk->GetWidth() / 2;
				m_pButtonOk->SetOffset( ptOffset );

				m_pButtonOk->Show();
			}
			if( m_pButtonCancel )
			{
				ptOffset   = m_pButtonCancel->GetOffset();
				ptOffset.x = (m_iWidth * 3) / 4 - m_pButtonCancel->GetWidth() / 2;
				m_pButtonCancel->SetOffset( ptOffset );
			
				m_pButtonCancel->Show();
			}
			break;
		}
	case BT_OK:
		{
			if( m_pButtonOk )
			{
				m_pButtonOk->Show();

				ptOffset   = m_pButtonOk->GetOffset();
				ptOffset.x = m_iWidth / 2 - m_pButtonOk->GetWidth() / 2;
				m_pButtonOk->SetOffset( ptOffset );
			}

			if( m_pButtonCancel )
				m_pButtonCancel->Hide();
			break;
		}
	case BT_CANCEL:
		{
			if( m_pButtonCancel )
			{
				m_pButtonCancel->Show();

				ptOffset   = m_pButtonCancel->GetOffset();
				ptOffset.x = m_iWidth / 2 - m_pButtonCancel->GetWidth() / 2;
				m_pButtonCancel->SetOffset( ptOffset );
			}

			if( m_pButtonOk )
				m_pButtonOk->Hide();
			break;
		}
	default:
		break;
	}
}

#ifdef _NEWUI

void CTMsgBox::SetString( const char* szString )
{
	m_message_parser.Clear();
	m_message_parser.SetString( szString, m_iWidth - 30 );

	///��ư���� Y- Offset �� �ڽ��� ���� ����
	if( m_pImageTop && m_pImageMiddle && m_pImageBottom )
	{
		int iBoxHeight = 0;
		POINT ptOffset = {0,0};

		POINT ptDraw = {0,0};
		ptDraw.y += m_pImageTop->GetHeight();		

		for( int i = 0; i < m_message_parser.GetStringCount() /*+ LINESPACE_BUTTONDROW*/; ++i )
		{
			
			iBoxHeight += 22;
		}

		//�߰� �۾����� ĭ �ø�.
		iBoxHeight += m_iExtraHeight;

		//ó�� + �߰�
		ptDraw.y += iBoxHeight;

		if( m_iExtraHeight )
		{
			m_iExtraHeightStartPoint = iBoxHeight;
		}

		//�߰� �̹��� ��ġ ũ��.
		m_pImageMiddle->SetOffset(0, m_pImageTop->GetHeight());
		m_pImageMiddle->SetHeight( iBoxHeight );
		m_pImageMiddle->SetSizeFit(true);

		//�ٴ� �̹��� ��ġ
		m_pImageBottom->SetOffset(0, ptDraw.y);

		///��ư���� Y��ǥ ����
		if( m_pButtonOk && m_pImageBottom )
		{
			ptOffset   = m_pButtonOk->GetOffset();
			ptOffset.y = ptDraw.y + m_pImageBottom->GetHeight() / 2 - m_pButtonOk->GetHeight() / 2;
			m_pButtonOk->SetOffset( ptOffset );
		}

		if( m_pButtonCancel && m_pImageBottom )
		{
			ptOffset	= m_pButtonCancel->GetOffset();
			ptOffset.y	= ptDraw.y + m_pImageBottom->GetHeight() / 2 - m_pButtonOk->GetHeight() / 2;
			m_pButtonCancel->SetOffset( ptOffset );
		}		
		
		ptDraw.y += m_pImageBottom->GetHeight();

		///�޼��� �ڽ��� ��ü ���� ����
		SetHeight( ptDraw.y + m_iExtraHeight );
		///����� Offset�� �̿��Ͽ� ��ư���� ��ġ ������ 
		MoveWindow(GetPosition());
	}
	else
	{
		
		if( m_pListBox )
		{
			//06. 06. 16 - ������ : ExUI���� ������ ���ڸ���� ���ڰ� ��ư�� ��������. ����~
			if(strlen(szString) >= 150)
			{
				m_pListBox->SetLineSpace(3);
				m_pListBox->SetFont(2);	
			}
			else
			{
				m_pListBox->SetLineSpace(7);
				m_pListBox->SetFont(0);	
			}
			m_pListBox->ClearText();
			m_pListBox->AppendText(szString, D3DCOLOR_RGBA(255,255,255,255) );
			
		}
	}
}

#else
void CTMsgBox::SetString( const char* szString )
{
	m_message_parser.Clear();
	m_message_parser.SetString( szString, m_iWidth - 30 );

	///��ư���� Y- Offset �� �ڽ��� ���� ����
	if( m_pImageTop && m_pImageMiddle && m_pImageBottom )
	{
		int iBoxHeight = 0;
		POINT ptOffset = {0,0};

		POINT ptDraw = {0,0};
		ptDraw.y += m_pImageTop->GetHeight();

		iBoxHeight += m_pImageTop->GetHeight();

		for( int i = 0; i < m_message_parser.GetStringCount() /*+ LINESPACE_BUTTONDROW*/; ++i )
		{
			ptDraw.y += m_pImageMiddle->GetHeight();
			iBoxHeight += m_pImageMiddle->GetHeight();
		}

		if( m_iExtraHeight )

			m_iExtraHeightStartPoint = iBoxHeight;



		///��ư���� Y��ǥ ����
		if( m_pButtonOk && m_pImageBottom )
		{
			ptOffset   = m_pButtonOk->GetOffset();
			ptOffset.y = ptDraw.y + m_pImageBottom->GetHeight() / 2 - m_pButtonOk->GetHeight() / 2 + m_iExtraHeight;
			m_pButtonOk->SetOffset( ptOffset );
		}

		if( m_pButtonCancel && m_pImageBottom )
		{
			ptOffset	= m_pButtonCancel->GetOffset();
			ptOffset.y	= ptDraw.y + m_pImageBottom->GetHeight() / 2 - m_pButtonOk->GetHeight() / 2 + m_iExtraHeight;
			m_pButtonCancel->SetOffset( ptOffset );
		}

		iBoxHeight += m_pImageTop->GetHeight();

		///�޼��� �ڽ��� ��ü ���� ����
		SetHeight( iBoxHeight + m_iExtraHeight );
		///����� Offset�� �̿��Ͽ� ��ư���� ��ġ ������ 
		MoveWindow(GetPosition());
	}
}
#endif

void CTMsgBox::MoveWindow( POINT pt )
{
	CTDialog::MoveWindow( pt );
	
	if( m_pCaption )
		m_pCaption->MoveWindow( pt );

	if( m_pButtonOk )
		m_pButtonOk->MoveWindow( pt );

	if( m_pButtonCancel )
		m_pButtonCancel->MoveWindow( pt );

#ifdef _NEWUI
	if( m_pImageTop )
	{
		m_pImageTop->MoveWindow( pt );
	}

	if( m_pImageMiddle )
	{
		m_pImageMiddle->MoveWindow( pt );
	}

	if( m_pImageBottom )
	{
		m_pImageBottom->MoveWindow( pt );
	}
	if( m_pListBox )
	{
        m_pListBox->MoveWindow( pt );
	}
#else
#endif
	
}

void CTMsgBox::SetButtons( CTButton* pBtnOk, CTButton* pBtnCancel )
{
	m_pButtonOk		= pBtnOk;
	m_pButtonCancel = pBtnCancel;
	SetButtonType( m_iButtonType );
}

void CTMsgBox::SetImages( CTImage* pImageTop, CTImage* pImageMiddle, CTImage* pImageBottom )
{
	m_pImageTop		= pImageTop;
	m_pImageMiddle	= pImageMiddle;
	m_pImageBottom	= pImageBottom;
}


/// 2003 /12 / 11 / Navy
/// ���ܰ�� �������� �̹������� �׸���.
/// ��ư�� ��ġ�� �����Ұ�
/// �������Ӹ��� ó���Ǵµ� �ؾ��� �۾��� �ʹ� ����. ���� ��
/// y��ǥ����
#ifdef _NEWUI

void CTMsgBox::DrawImages()
{
	if( m_pImageTop )
	{
		POINT ptDraw = GetPosition();

		///����̹��� �׸��� 		
		m_pImageTop->Draw();
	}
	if( m_pImageMiddle )
	{
		/// �ߴ��̹��� �׸���
		m_pImageMiddle->Draw();
	}
	if( m_pImageBottom )
	{
		/// �ϴ��̹��� �׸���		
		m_pImageBottom->Draw();
	}	
	if( m_pListBox )
	{
		m_pListBox->Draw();
	}
}

#else

void CTMsgBox::DrawImages()
{
	if( m_pImageTop && m_pImageMiddle && m_pImageBottom )
	{
		POINT ptDraw = GetPosition();

		///����̹��� �׸��� 
		m_pImageTop->MoveWindow( ptDraw );
		m_pImageTop->Draw();


		ptDraw.y += m_pImageTop->GetHeight();

		/// �ߴ��̹��� �׸���
#ifdef _NEWUI
		m_pImageMiddle->MoveWindow( ptDraw );
		m_pImageMiddle->Draw();
		ptDraw.y += m_pImageMiddle->GetHeight();
#else
		for( int i = 0; i < m_message_parser.GetStringCount(); ++i )
		{
			m_pImageMiddle->MoveWindow( ptDraw );
			m_pImageMiddle->Draw();

			ptDraw.y += m_pImageMiddle->GetHeight();

		}
#endif
		///ExtraHeight�׸���
		if( m_iExtraHeight > 0)
		{
			for( int i = 0; i < m_iExtraHeight / m_pImageMiddle->GetHeight() + 1; ++i )
			{
				m_pImageMiddle->MoveWindow( ptDraw );
				m_pImageMiddle->Draw();

				ptDraw.y += m_pImageMiddle->GetHeight();
			}
		}
		/// �ϴ��̹��� �׸���
		m_pImageBottom->MoveWindow( ptDraw );
		m_pImageBottom->Draw();
	}
}

#endif


///��ư Ÿ�Կ� ���� ��ư�� Showó��
void CTMsgBox::Show()
{
	CTDialog::Show();

	switch( m_iButtonType )
	{
	case ( BT_OK | BT_CANCEL ):
		{
			if( m_pButtonOk )
				m_pButtonOk->Show();
			if( m_pButtonCancel )
				m_pButtonCancel->Show();

			break;
		}
	case BT_OK:
		{
			if( m_pButtonOk )
				m_pButtonOk->Show();

			if( m_pButtonCancel )
				m_pButtonCancel->Hide();
			break;
		}
	default:
		break;
	}

	if( m_pImageTop )
		m_pImageTop->Show();
	if( m_pImageMiddle )
		m_pImageMiddle->Show();
	if( m_pImageBottom )
		m_pImageBottom->Show();
	if( m_pCaption )
		m_pCaption->Show();
	if( m_pListBox )
	{
		m_pListBox->Show();
	}

}

void CTMsgBox::Hide()
{
	CTDialog::Hide();
	if( m_pButtonOk )
		m_pButtonOk->Hide();

	if( m_pButtonCancel )
		m_pButtonCancel->Hide();

	if( m_pImageTop )
		m_pImageTop->Hide();
	if( m_pImageMiddle )
		m_pImageMiddle->Hide();
	if( m_pImageBottom )
		m_pImageBottom->Hide();
	if( m_pCaption )
		m_pCaption->Hide();
	if( m_pListBox )
	{
		m_pListBox->Hide();
	}

}

bool CTMsgBox::Create( char* szIDD )
{
	return CTControlMgr::GetInstance()->MakeMsgBoxByXML( szIDD, this );
}

void CTMsgBox::AddExtraHeight( int iHeight )
{
	_ASSERT( m_iHeight == 0 );
#ifndef _NEWUI
	_ASSERT( m_iExtraHeight == 0 );
#endif
	m_iExtraHeight = iHeight;

	// ���� ���Դٴ°� ��ü�� �޼����ڽ����� �������� ��ο� �Ѵٴ� ���̴�.
	m_bUseDrawItem = true;

	SetHeight( GetHeight() + m_iExtraHeight );
	POINT ptOffset;

	if( m_pButtonOk )
	{
		ptOffset	=  m_pButtonOk->GetOffset();
		ptOffset.y	+= m_iExtraHeight;
		m_pButtonOk->SetOffset( ptOffset );
	}

	if( m_pButtonCancel )
	{
		ptOffset	=  m_pButtonCancel->GetOffset();
		ptOffset.y	+= m_iExtraHeight;
		m_pButtonCancel->SetOffset( ptOffset );
	}
}
			  
int CTMsgBox::GetExtraHeightStartPoint()
{
	return m_iExtraHeightStartPoint;
}

void CTMsgBox::SetListBox( CTListBox * pListBox )
{
	m_pListBox = pListBox;
}

void CTMsgBox::SetButtonText( const char* szBtnOK, const char* szBtnCancel )
{
	if( m_pButtonOk && szBtnOK )
	{
		m_pButtonOk->SetText( szBtnOK );
	}
	if( m_pButtonCancel && szBtnCancel )
	{
		m_pButtonCancel->SetText( szBtnCancel );
	}
}