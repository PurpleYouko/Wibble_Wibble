#ifndef _SYSTEMDLG_
#define _SYSTEMDLG_

#include "TDialog.h"

/**
* ��������� ĳ���� ����â���� �̵������� ���� ���̾�α�
*
* @Author		������
* @Date			2005/9/14
*/
class CSystemDLG  : public CTDialog
{
public:
	CSystemDLG();
	virtual ~CSystemDLG();

	virtual void SetInterfacePos_After();

	unsigned int Process(UINT uiMsg,WPARAM wParam,LPARAM lParam);

protected:
	enum{
		IID_BTN_EXIT		= 10,
		IID_BTN_CONTINUE	= 11,
		IID_BTN_GOTO_SELECTAVT = 12,
		IID_BTN_CLOSE		= 13,
		IID_BTN_OPTION		= 14,
	};

	bool On_LButtonUP( unsigned iProcID);

};


#endif //_SYSTEMDLG_