#ifndef _CChattingRoomPasswordDlg_
#define _CChattingRoomPasswordDlg_
#include "TDialog.h"

//ȫ�� ���� 12:02 2006-10-26 : chattingroom password �Է�â.
class CChattingRoomPasswordDlg : public CTDialog
{
public:
	CChattingRoomPasswordDlg(){}
	CChattingRoomPasswordDlg(int iDlgType);
	virtual ~CChattingRoomPasswordDlg();

	virtual unsigned Process( unsigned uiMsg, WPARAM wParam, LPARAM lParam );
	virtual void SetInterfacePos_After();
	virtual void Show();
	virtual void Hide();
	void			SetID( WORD wId );

protected:
	enum{
		IID_BTN_CLOSE	= 10,
		IID_BTN_CONFIRM = 11,
		IID_EDITBOX		= 20
	};

protected:
	bool SendReqJoinRoom();
	WORD		m_wID;
};

#endif