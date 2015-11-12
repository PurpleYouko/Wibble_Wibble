#ifndef _TARGETOBJECTDLG_
#define _TARGETOBJECTDLG_


#include "../../../TGameCtrl/TDialog.h"



/**
* 예전 클베때 타겟에 대한 정보를 보여주는 다이얼로그
*
* @Warning		현재( 2005/9/15)사용하지 않고 있다.
* @Author		최종진
* @Date			2005/9/15
*/
class CTGuage;
class CTargetObjectDLG : public CTDialog
{

public:
	CTargetObjectDLG();
	virtual ~CTargetObjectDLG();

	
	// *---------------------------------------------------* //
	// Overrided function
	// *---------------------------------------------------* //
	virtual void Show();	

	virtual void Draw();
	virtual unsigned int Process(UINT uiMsg,WPARAM wParam,LPARAM lParam);
	// *---------------------------------------------------* //

	void * m_TargetObj;
	CTGuage * m_pGuage;

	int m_iValue;
	int m_iMax;
	float m_iPer;

	bool	On_LButtonUP( unsigned iProcID, WPARAM wParam, LPARAM lParam );

	enum
	{
		IID_BTN_CLOSE = 10,
	};
};



#endif //_TARGETOBJECTDLG_