#ifndef	__COBJCOLLISION2NPC_H
#define	__COBJCOLLISION2NPC_H

#include <vector>
#include "IO_Terrain.h"


class CObjCHAR_Collision2NPC
{
public:

	CObjCHAR_Collision2NPC(void );
	~CObjCHAR_Collision2NPC(void);

	void Initialize(CObjCHAR * pObjAvater);
	void CollisionDetectionAvatar2Mob(CObjCHAR * pObjAvater);
    bool CollisionPosition2OBBOnOff(D3DXMATRIX *m, float xLength, float yLength, float zLength, D3DXVECTOR3 *pPosition);
    bool ContactRay2OBBIn(D3DXMATRIX *m, float xLength, float yLength, float zLength, D3DXVECTOR3 *pPosition);
    void CollisionPosition2OBBResponse(D3DXMATRIX *m, float xLength, float yLength, float zLength, D3DXVECTOR3 *pPosition,  D3DXVECTOR3 *pContactPosition);

	HNODE m_hNodeModel; 
	int m_iObjTYPE;
	CObjCHAR *m_pObjAvater;
    D3DXVECTOR3 m_vModelDirection;
	D3DXVECTOR3 m_vLocalCollisionPoint;
	
};


#endif