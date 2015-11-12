#ifndef _CMSGBOX_
#define _CMSGBOX_
#include "TMsgBox.h"
#include "subclass/cslot.h"
class CTCommand;
class CIcon;

/**
* 메세지 박스용 다이얼로그
*
* @Author		최종진
* @Date			2005/9/12
*/
class CMsgBox :	public CTMsgBox
{
public:
	CMsgBox(void);
	virtual ~CMsgBox(void);

	virtual void Draw();
	virtual void Hide();
	virtual void Update( POINT ptMouse );
	virtual void MoveWindow( POINT pt );
	virtual void SetString( const char* szString );
	virtual unsigned int Process(UINT uiMsg,WPARAM wParam,LPARAM lParam);


	void SetCommand( CTCommand* pCmdOk, CTCommand* pCmdCancel );
	void SetInvokerDlgID( unsigned int iDlgID );
	void SetIcons( std::list<CIcon*>& Icons );

	///중복되어서 열린 MsgBox들을 같은 Type별로 모두 닫고 싶을때 Type을 지정하고 OpenMsgBox에서 그 Type을 인자로 넘겨준다.
	enum{
		MSGTYPE_NONE,
		MSGTYPE_RECV_TRADE_REQ,
		MSGTYPE_RECV_PARTY_REQ,
		MSGTYPE_RECV_CLANJOIN_REQ,
		MSGTYPE_RECV_CART_RIDE_REQ,	//2인승 카트
		MSGTYPE_RECV_CLANWAR_JOIN_REQ,
		MSGTYPE_RECV_CLANWAR_JOIN_REQ_RECONFIRM,
	};
	
	enum{
		IID_BTN_OK		= 255,
		IID_BTN_CANCEL	= 256
	};

	enum{
		CHECKCOMMAND_OK,
		CHECKCOMMAND_CANCEL,
		CHECKCOMMAND_NONE,
	};

	void	SetswTimer(bool UseTimer);
	bool	GetswTimer();

	void	SetMsgTimer(DWORD dwTimer);
	DWORD	GetMsgTImer();

	void 	SetCheckCommand(int m_bCheckCommand);
	int	GetCheckCommand();


protected:
	void SendCommand( unsigned int iDlgID, CTCommand* pCmd );
	void ClearCommand();

private:
	bool				m_bUserTimer;					/// 타이머 사용 여부.
	DWORD				m_dwLiveMsgBoxTImer;			/// 메세지 박스가 살아 있는 시간.
	int					m_bCheckCommand;				/// Ok?, Cancel? 중에 어떤 커맨드를 수행할지 체크.->ok 1,  cancel 0, 아무것도 안함 2 로 수정.

protected:
	CTCommand*			m_pCmdOk;			/// Ok Btn이 눌렸을때 처리할 Command
	CTCommand*			m_pCmdCancel;		/// Cancel Btn이 눌렸을때 처리할 Command
	unsigned int		m_iInvokerDlgID;	/// Command 를 처리할 TDialogID
	std::vector<CSlot>	m_Slots;			/// 메세지 박스에 아이콘이 보여질때 사용되는 슬롯 컨테이너

	DWORD				m_dwMsgBoxCreateTime;			/// 메세지 박스를 생성한 시간.
	DWORD				m_dwMsgBoxTimer;				/// 현재 시간.
};
#endif