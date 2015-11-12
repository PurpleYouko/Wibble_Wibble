#ifndef _CMSGBOX_
#define _CMSGBOX_
#include "TMsgBox.h"
#include "subclass/cslot.h"
class CTCommand;
class CIcon;

/**
* �޼��� �ڽ��� ���̾�α�
*
* @Author		������
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

	///�ߺ��Ǿ ���� MsgBox���� ���� Type���� ��� �ݰ� ������ Type�� �����ϰ� OpenMsgBox���� �� Type�� ���ڷ� �Ѱ��ش�.
	enum{
		MSGTYPE_NONE,
		MSGTYPE_RECV_TRADE_REQ,
		MSGTYPE_RECV_PARTY_REQ,
		MSGTYPE_RECV_CLANJOIN_REQ,
		MSGTYPE_RECV_CART_RIDE_REQ,	//2�ν� īƮ
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
	bool				m_bUserTimer;					/// Ÿ�̸� ��� ����.
	DWORD				m_dwLiveMsgBoxTImer;			/// �޼��� �ڽ��� ��� �ִ� �ð�.
	int					m_bCheckCommand;				/// Ok?, Cancel? �߿� � Ŀ�ǵ带 �������� üũ.->ok 1,  cancel 0, �ƹ��͵� ���� 2 �� ����.

protected:
	CTCommand*			m_pCmdOk;			/// Ok Btn�� �������� ó���� Command
	CTCommand*			m_pCmdCancel;		/// Cancel Btn�� �������� ó���� Command
	unsigned int		m_iInvokerDlgID;	/// Command �� ó���� TDialogID
	std::vector<CSlot>	m_Slots;			/// �޼��� �ڽ��� �������� �������� ���Ǵ� ���� �����̳�

	DWORD				m_dwMsgBoxCreateTime;			/// �޼��� �ڽ��� ������ �ð�.
	DWORD				m_dwMsgBoxTimer;				/// ���� �ð�.
};
#endif