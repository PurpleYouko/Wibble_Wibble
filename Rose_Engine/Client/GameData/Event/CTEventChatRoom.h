#ifndef _CTEventChatRoom_
#define _CTEventChatRoom_
#include "ctevent.h"

class CTEventChatRoom :	public CTEvent
{
public:
	CTEventChatRoom(void);
	virtual ~CTEventChatRoom(void);
	enum{
		EID_STATE_ACTIVATED,
		EID_STATE_DEACTIVATED,///ä�÷뿡�� ������ - UI�� ����
		EID_JOIN_MEMBER,
		EID_LEAVE_MEMBER,
		EID_SET_MASTER,
	};

	void SetName( const char* pszName );
	void SetServerIdx( WORD wServerIdx );
	void SetUserTag(DWORD dwUserTag);

	const char* GetName();
	WORD  GetServerIdx();
	DWORD GetUserTag();
protected:
	WORD		m_wServerIdx;
	DWORD		m_dwUserTag;
	std::string	m_strName;
};
#endif