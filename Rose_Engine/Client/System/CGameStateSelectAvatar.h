#ifndef _CGAMESTATESELECTAVATAR_
#define _CGAMESTATESELECTAVATAR_
#include "cgamestate.h"
#include "../CJustModelAVT.h"

/**
* 캐릭터 선택 State Class
*	- Next State : CGameStateCreateAvatar, CGameStateMoveMain, CGameStateLoginVirtual
*
* @Author	최종진	
* @Date		2005/9/15
*/

struct SelectAvatarBox{

	float Position[3];
	float Rotation[4];  // x, y, z, w
	float xMinMax[2];
	float yMinMax[2];
	float zMinMax[2];
    float CenterPosition[3];
	float zMinMaxEx;
    float DirectionXY[2];
	int MotionIDX[6];
	std::string BufferAvatarName;

    HNODE hNode;      // Visible Node
};

struct MovingCameraState{

	float Position[3];
	float Rotation[4];
};

class CGameStateSelectAvatar : public CGameState
{
public:
	CGameStateSelectAvatar(int iID);
	~CGameStateSelectAvatar(void);
	
	virtual int Update( bool bLostFocus );
	virtual int Enter( int iPrevStateID );
	virtual int Leave( int iNextStateID );
	
	virtual void ServerDisconnected();

	virtual int ProcMouseInput( UINT uiMsg, WPARAM wParam, LPARAM lParam );
	virtual int ProcKeyboardInput( UINT uiMsg, WPARAM wParam, LPARAM lParam );
	
	int mouse_x, mouse_y;         // 조성현 10 - 19
	int pre_mouse_x, pre_mouse_y;

	//조성현 2005 12 - 14 Picking 추가   	
	SelectAvatarBox m_SelectAvatarBox[5];
	SelectAvatarBox m_MovingCameraZoomInBox;
    MovingCameraState m_MovingCameraState[6];
    
	
	void ReConstructSelectAvatar(int iPrevStateID);
    void InitStateSelectAvatarElement();
    const char* PickingSelectAvatar();

	tPOINT16	m_ScreenPOS;
	void Pick_POSITION ( );
	void RevisionCulledModel();
    void OnSelectAvatar(CJustModelAVT* pPrevAvt, CJustModelAVT* pNowAvt);
	void OnLeaveSelectAvatar();
	void PickUp_POSITION();
	void ClearSelectedAvatar();
	void SaveCurrentSelectAvatar();
	void LoadCurrentSelectAvatar();
    bool m_iPrevStateID;
    bool m_bMotionblendOption;     //임시로 모션 블랜딩 제거...
    int  m_iCurrentPickingMotionIndex;
	
	float TestElement[3];
	//  void 
};
#endif