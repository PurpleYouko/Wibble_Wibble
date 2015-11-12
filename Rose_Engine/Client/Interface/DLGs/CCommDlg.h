#ifndef _CCommDlg_
#define _CCommDlg_
#include "TDialog.h"
class CZListBox;
class CFriendListItem;

/**
* Community �� ���� ���̾�α�
*
* @Author		������
* @Date			2005/9/12
*/
class CCommDlg : public CTDialog
{
public:
	CCommDlg( int iDlgType );
	virtual ~CCommDlg(void);
	
	virtual unsigned Process( unsigned uiMsg, WPARAM wParam, LPARAM lParam );
	
	virtual void Update( POINT ptMouse );

	virtual void Show();
	virtual void Hide();

	///ģ����� ����
	void AddFriend( DWORD dwUserTag, BYTE btStatus, const char* pszName );
	void ClearFriendList();
	void ChangeFriendStatus( DWORD dwUserTag, BYTE btStatus );
	void SetRemoveFriendBtn( CZListBox * pZList );

	void RemoveFriend( DWORD dwUserTag );

	CFriendListItem* FindFriend( DWORD dwUserTag );
	CFriendListItem* FindFriendByName( const char* pszName );

	///�����Ը�� ����
	void ClearMemoList();
	void AddMemo( int iRowid, const char* pszFromName, const char* pszMemo , DWORD dwRecvTime );
	void DeleteMemo( int iIndex );

	/// ��ȭ���� ����
	void ClearChatRoomList();
	void AddChatRoom( const char* pszTitle, BYTE btRoomType, short nRoomID, BYTE btUserCount );


	/// Ŭ���̾�Ʈ�� ����Ǿ� �ִ� ������ �������� ���Ͽ� �ʿ��� Method(  Sqlite ����� )
	static int callback(void *NotUsed, int argc, char **argv, char **azColName);

	void SetInterfacePos_After();

private:
	void		OnLButtonUp( unsigned uiProcID, WPARAM wParam, LPARAM lParam );
	CZListBox*	GetZListBox( int iTab, int iListID );

	/// Ŭ���̾�Ʈ�� ����Ǿ� �ִ� ���� ��������( Sqlite ����� )
	void		LoadMemoFromDB();

	
private:

	enum{
		IID_BTN_CLOSE			= 10,
		IID_BTN_ICONIZE			= 11,
		IID_TABBEDPANE			= 20,
		IID_TABBUTTON_FRIEND	= 25,
		IID_ZLIST_FRIEND		= 26,
		
		IID_BTN_ADDFRIEND		= 29,
		IID_TABBUTTON_CHATROOM	= 35,
		IID_ZLIST_CHATROOM		= 36,
		IID_BTN_CREATECHATROOM	= 39,
		IID_TABBUTTON_MAILBOXM	= 45,
		IID_ZLIST_MAILBOX		= 46,
		IID_BTN_CLEAR_MAIL		= 47,

		IID_BTN_REMOVEFRIEND	= 100,
		

		IID_BTN_REMOVEFRIEND01	= 101,
		IID_BTN_REMOVEFRIEND02	= 102,
		IID_BTN_REMOVEFRIEND03	= 103,
		IID_BTN_REMOVEFRIEND04	= 104,
		IID_BTN_REMOVEFRIEND05	= 105,
		IID_BTN_REMOVEFRIEND06	= 106,
		IID_BTN_REMOVEFRIEND07	= 107,
		IID_BTN_REMOVEFRIEND08	= 108,
		IID_BTN_REMOVEFRIEND09	= 109,



	};

	enum
	{
		TAB_FRIEND   = 0,		
		TAB_CHATROOM = 1,		
		TAB_MAILBOX  = 2
	};
	
};
#endif