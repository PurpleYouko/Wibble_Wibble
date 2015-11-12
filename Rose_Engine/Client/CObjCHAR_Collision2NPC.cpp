#include "stdAFX.h"
#include "Game.h"
#include "OBJECT.h"
#include "IO_Terrain.h"
#include "Network\CNetwork.h"
#include "common\Calculation.h"
#include "Game_FUNC.h"
#include "ObjectActionProcessor.h"
#include "CObjCART.h"
#include "ObjCastleGear.h"
#ifdef __NEW_PAT_TYPES
#include ".\CObjNewRide.h"
#endif
#include "CObjCHAR_Collision2NPC.h"
#include "../Country.h"



CObjCHAR_Collision2NPC::CObjCHAR_Collision2NPC()
{


}

CObjCHAR_Collision2NPC::~CObjCHAR_Collision2NPC()
{



}


void CObjCHAR_Collision2NPC::Initialize(CObjCHAR * pObjAvater)
{
	D3DXVECTOR3 localCOM;
	D3DXVECTOR3 vPosition;

	m_hNodeModel = pObjAvater->GetZMODEL();
	m_pObjAvater = pObjAvater;
	
	::getModelDirectionVector( m_hNodeModel, m_vModelDirection );
	::getModelCOMPositionLocal(m_hNodeModel, localCOM);
	
	
	if(m_iObjTYPE != OBJ_CGEAR && (m_iObjTYPE != OBJ_NRIDE))
		m_vLocalCollisionPoint = localCOM + m_vModelDirection*0.0f + D3DXVECTOR3(0.0f, 0.0f, 70.0f);
	else
		m_vLocalCollisionPoint = m_vModelDirection*0.0f + D3DXVECTOR3(0.0f, 0.0f, 270.0f);
}

void CObjCHAR_Collision2NPC::CollisionDetectionAvatar2Mob(CObjCHAR * pObjAvater)
{
	m_iObjTYPE = pObjAvater->Get_TYPE();

	if((m_iObjTYPE != OBJ_AVATAR) && (m_iObjTYPE != OBJ_USER) && (m_iObjTYPE != OBJ_CART) && (m_iObjTYPE != OBJ_CGEAR) && (m_iObjTYPE != OBJ_NRIDE))
		return;

	if (pObjAvater->GetPetMode() >= 0) { // if is on the PET, then ignore UpdateHeight
		return;
	}

	CObjCHAR *pObjectChar;
	HNODE hNPCBoneNode;
    	
	float fAvaterPositionXYZ[3];
	float fAvaterFrontPositionXYZ[3];
	float fPreAvaterRotaitonWXYZ[4];
	float fPreAvaterPositionXYZ[3];
	float fPreAvaterFrontPositionXYZ[3];

	float fBoneRotationWXYZ[4];
	float fBonePositionXYZ[3];
	float fPreBoneRotaitonWXYZ[4];
	float fPreBonePositionXYZ[3];

    D3DXVECTOR4 vlocalPos, vWorldPos;
    D3DXMATRIX boneModelM, bonePreModelM, invBonePreModelM;
	D3DXMATRIX boneModelRotationM; 
	
	Initialize(pObjAvater);
	
	::getPosition(m_hNodeModel, fAvaterPositionXYZ);
	fAvaterFrontPositionXYZ[0] = fAvaterPositionXYZ[0] + m_vLocalCollisionPoint.x;
	fAvaterFrontPositionXYZ[1] = fAvaterPositionXYZ[1] + m_vLocalCollisionPoint.y;
	fAvaterFrontPositionXYZ[2] = fAvaterPositionXYZ[2] + m_vLocalCollisionPoint.z;

	classDLLNODE< int > *pNode;
	pNode = g_pObjMGR->m_CharLIST.GetHeadNode ();
	
	while( pNode ) 
	{
		if ( g_pObjMGR->m_pOBJECTS[ pNode->DATA ] ) 
		{			
			if (g_pObjMGR->m_pOBJECTS[ pNode->DATA ]) {

				if(g_pObjMGR->m_pOBJECTS[ pNode->DATA ]->IsNPC())
				{
					pObjectChar = (CObjCHAR*)g_pObjMGR->m_pOBJECTS[ pNode->DATA ];
					if(pObjectChar->m_bCollisionOnOff)
					{
						hNPCBoneNode = ::getBone(pObjectChar->GetZMODEL(), pObjectChar->m_iCollisionBoneIndex);
						getRotationQuad(hNPCBoneNode, fBoneRotationWXYZ);
						getPosition(hNPCBoneNode, fBonePositionXYZ);
						
						D3DXMatrixRotationQuaternion(&boneModelM, &D3DXQUATERNION(fBoneRotationWXYZ[1], fBoneRotationWXYZ[2], fBoneRotationWXYZ[3], fBoneRotationWXYZ[0]));
						boneModelM._41 = fBonePositionXYZ[0];	boneModelM._42 = fBonePositionXYZ[1];	boneModelM._43 = fBonePositionXYZ[2];

//						::InputSceneOBB2(fBonePositionXYZ, fBoneRotationWXYZ, pObjectChar->m_fCollisionBoxLength[0], pObjectChar->m_fCollisionBoxLength[1], pObjectChar->m_fCollisionBoxLength[2]);
//						::InputSceneSphere(fAvaterFrontPositionXYZ[0], fAvaterFrontPositionXYZ[1], fAvaterFrontPositionXYZ[2], 50.0f);
   										
						if(CollisionPosition2OBBOnOff(&boneModelM, pObjectChar->m_fCollisionBoxLength[0], pObjectChar->m_fCollisionBoxLength[1], pObjectChar->m_fCollisionBoxLength[2], &D3DXVECTOR3(fAvaterFrontPositionXYZ[0], fAvaterFrontPositionXYZ[1], fAvaterFrontPositionXYZ[2])))
						{
							::getPrevPosition (hNPCBoneNode, fPreBonePositionXYZ);
							::getPrevRotation (hNPCBoneNode, fPreBoneRotaitonWXYZ);

							::getPrevPosition (m_hNodeModel, fPreAvaterPositionXYZ);
							::getPrevRotation (m_hNodeModel, fPreAvaterRotaitonWXYZ);
							
							fAvaterFrontPositionXYZ[0] = fPreAvaterPositionXYZ[0] + m_vLocalCollisionPoint.x;
							fAvaterFrontPositionXYZ[1] = fPreAvaterPositionXYZ[1] + m_vLocalCollisionPoint.y;
							fAvaterFrontPositionXYZ[2] = fPreAvaterPositionXYZ[2] + m_vLocalCollisionPoint.z;
							
							
							if(CollisionPosition2OBBOnOff(&boneModelM, pObjectChar->m_fCollisionBoxLength[0], pObjectChar->m_fCollisionBoxLength[1], pObjectChar->m_fCollisionBoxLength[2], &D3DXVECTOR3(fAvaterFrontPositionXYZ[0], fAvaterFrontPositionXYZ[1], fAvaterFrontPositionXYZ[2])))
							{
								D3DXVECTOR3 vContactPositon;
								CollisionPosition2OBBResponse(&boneModelM, pObjectChar->m_fCollisionBoxLength[0], pObjectChar->m_fCollisionBoxLength[1], pObjectChar->m_fCollisionBoxLength[2], &D3DXVECTOR3(fAvaterFrontPositionXYZ[0], fAvaterFrontPositionXYZ[1], fAvaterFrontPositionXYZ[2]), &vContactPositon);

								fPreAvaterPositionXYZ[0] = vContactPositon.x - m_vLocalCollisionPoint.x;
								fPreAvaterPositionXYZ[1] = vContactPositon.y - m_vLocalCollisionPoint.y;
								fPreAvaterPositionXYZ[2] = vContactPositon.z - m_vLocalCollisionPoint.z;
										
							}
						
							
/*							fPreAvaterFrontPositionXYZ[0] = fPreAvaterPositionXYZ[0] + m_vLocalCollisionPoint.x;
                            fPreAvaterFrontPositionXYZ[1] = fPreAvaterPositionXYZ[1] + m_vLocalCollisionPoint.y;
							fPreAvaterFrontPositionXYZ[2] = fPreAvaterPositionXYZ[2] + m_vLocalCollisionPoint.z;
			
							
							D3DXMatrixRotationQuaternion(&bonePreModelM, &D3DXQUATERNION(fPreBoneRotaitonWXYZ[1], fPreBoneRotaitonWXYZ[2], fPreBoneRotaitonWXYZ[3], fPreBoneRotaitonWXYZ[0]));
						    bonePreModelM._41 = fPreBonePositionXYZ[0];	bonePreModelM._42 = fPreBonePositionXYZ[1];	bonePreModelM._43 = fPreBonePositionXYZ[2];
							
							::InputSceneSphere(m_vLocalCollisionPoint.x + fPreAvaterPositionXYZ[0], m_vLocalCollisionPoint.y + fPreAvaterPositionXYZ[1], m_vLocalCollisionPoint.z + fPreAvaterPositionXYZ[2], 50.0f);
   							
							D3DXVECTOR3 vContactPoint;							
							if(CollisionPosition2OBBOnOff(&bonePreModelM, pObjectChar->m_fCollisionBoxLength[0], pObjectChar->m_fCollisionBoxLength[1], pObjectChar->m_fCollisionBoxLength[2], &D3DXVECTOR3(fPreAvaterFrontPositionXYZ[0], fPreAvaterFrontPositionXYZ[1], fPreAvaterFrontPositionXYZ[2])))
							{
								if(ContactRay2OBBIn(&bonePreModelM, pObjectChar->m_fCollisionBoxLength[0], pObjectChar->m_fCollisionBoxLength[1], pObjectChar->m_fCollisionBoxLength[2], &vContactPoint))
								{
									fPreAvaterPositionXYZ[0] = vContactPoint.x;
									fPreAvaterPositionXYZ[1] = vContactPoint.y;
									fPreAvaterPositionXYZ[2] = vContactPoint.z;
								}
								else
								{
									assert(true);

								}
							}
												
							D3DXMatrixInverse(&invBonePreModelM, NULL, &bonePreModelM);
								
							boneModelRotationM = invBonePreModelM * boneModelM;
                                
							D3DXVec4Transform(&vWorldPos, &D3DXVECTOR4(fPreAvaterPositionXYZ[0], fPreAvaterPositionXYZ[1], fPreAvaterPositionXYZ[2], 1.0f), &boneModelRotationM);
						
							fPreAvaterPositionXYZ[0] = vWorldPos.x - m_vLocalCollisionPoint.x;
							fPreAvaterPositionXYZ[1] = vWorldPos.y - m_vLocalCollisionPoint.y;
							fPreAvaterPositionXYZ[2] = vWorldPos.z - m_vLocalCollisionPoint.z;

*/							
							if(true)
							{
								m_pObjAvater->Set_CurPOS(D3DXVECTOR3(fPreAvaterPositionXYZ[0], fPreAvaterPositionXYZ[1], fPreAvaterPositionXYZ[2]));
							
								if(m_iObjTYPE == OBJ_USER)
									g_pNet->Send_cli_CANTMOVE();
								else
								{
									if(m_iObjTYPE == OBJ_CGEAR||m_iObjTYPE ==OBJ_NRIDE)
									{
										CObjCastleGear* pCgear = (CObjCastleGear*)pObjAvater;
										CObjCHAR* pParent = pCgear->GetParent();
										
										if(pParent)
										{
											if( pParent->Get_TYPE() == OBJ_USER)
												g_pNet->Send_cli_CANTMOVE();
										}
									}
									else if(m_iObjTYPE == OBJ_CART)
									{
										CObjCART* pCart = (CObjCART*)pObjAvater;
										CObjCHAR* pParent = pCart->GetParent();
										if( pParent )
										{
											if( pParent->Get_TYPE() == OBJ_USER)
												g_pNet->Send_cli_CANTMOVE();
										}
									}

								}
								
								m_pObjAvater->SetCMD_STOP();
								setPositionVec3( m_hNodeModel, fPreAvaterPositionXYZ );
							}

							::setRotationQuat(m_hNodeModel, fPreAvaterRotaitonWXYZ); 
							::stopModelRotation(m_hNodeModel);

							if(m_iObjTYPE == OBJ_CGEAR||m_iObjTYPE == OBJ_NRIDE)
							{
								CObjCastleGear* pCgear = (CObjCastleGear*)pObjAvater;
								CObjCHAR* pParent = pCgear->GetParent();
								if( pParent )
								{
									pParent->Set_CurPOS( pObjAvater->Get_CurPOS() );
								}
							}
							else if(m_iObjTYPE == OBJ_CART)
							{
								CObjCART* pCart = (CObjCART*)pObjAvater;
								CObjCHAR* pParent = pCart->GetParent();
								if( pParent )
								{
									pParent->Set_CurPOS( pObjAvater->Get_CurPOS() );
								}

							}


						}
						
					}
                 
				}
			}			
		}

		pNode = g_pObjMGR->m_CharLIST.GetNextNode( pNode );
	} 
}

bool CObjCHAR_Collision2NPC::CollisionPosition2OBBOnOff(D3DXMATRIX *m, float xLength, float yLength, float zLength, D3DXVECTOR3 *pPosition)
{
	D3DXMATRIX invM;
    D3DXVECTOR4 vWorldPos, vLocalPos;
    
	D3DXMatrixInverse(&invM, NULL, m);
	vWorldPos = (D3DXVECTOR4)*pPosition;
	vWorldPos.w = 1.0f;

	D3DXVec4Transform(&vLocalPos, &vWorldPos, &invM);

	if(fabsf(vLocalPos.x ) < xLength)
	{
		if(fabsf(vLocalPos.y ) < yLength)
		{
			if(fabsf(vLocalPos.z ) < zLength)
			{
				return true;
			}
		}
	}
	return false;
}

void CObjCHAR_Collision2NPC::CollisionPosition2OBBResponse(D3DXMATRIX *m, float xLength, float yLength, float zLength, D3DXVECTOR3 *pPosition,  D3DXVECTOR3 *pContactPosition)
{
	D3DXMATRIX invM;
    D3DXVECTOR4 vWorldPos, vLocalPos;
    
	D3DXMatrixInverse(&invM, NULL, m);
	vWorldPos = (D3DXVECTOR4)*pPosition;
	vWorldPos.w = 1.0f;

	D3DXVec4Transform(&vLocalPos, &vWorldPos, &invM);
    
	float fMinDistance;
    fMinDistance = 100000000.0f;

	D3DXVECTOR3 vNormal;
	D3DXVECTOR3 vPosition;
	D3DXVECTOR3 vContactLocalPosition;
	D3DXVECTOR4 vContactWorldPosition;
	float fD;
	float fDistance;
    bool bContactOnOff = false;

	vPosition = (D3DXVECTOR3)vLocalPos;
	for(int i = 0; i < 6; i += 1)
	{
		switch(i)
		{
		case 0:
			vNormal.x = -1.0f; vNormal.y = 0.0f; vNormal.z = 0.0f;
			fD =  -xLength;
			break;
		
		case 1:
			vNormal.x = 1.0f; vNormal.y = 0.0f; vNormal.z = 0.0f;
			fD =  -xLength;
			break;
		
		case 2:
			vNormal.x = 0.0f; vNormal.y = -1.0f; vNormal.z = 0.0f;
			fD =  -yLength;
			break;
		
		case 3:
			vNormal.x = 0.0f; vNormal.y = 1.0f; vNormal.z = 0.0f;
			fD =  -yLength;
			break;
		
		case 4:
			vNormal.x = 0.0f; vNormal.y = 0.0f; vNormal.z = -1.0f;
			fD =  -zLength;
			break;
		
		case 5:
			vNormal.x = 0.0f; vNormal.y = 0.0f; vNormal.z = 1.0f;
			fD =  -zLength;
			break;
		}

		fDistance = -(D3DXVec3Dot(&vPosition, &vNormal) + fD);

		if(fDistance < fMinDistance )
		{
			fMinDistance = fDistance;
			vContactLocalPosition = vNormal*(fDistance + 5.0f) + vPosition;
            bContactOnOff = true;
		}
	}

	pContactPosition->x = pPosition->x;
	pContactPosition->y = pPosition->y;
	pContactPosition->z = pPosition->z;
	
	if(bContactOnOff)
	{
		D3DXVec4Transform(&vContactWorldPosition, &D3DXVECTOR4(vContactLocalPosition.x, vContactLocalPosition.y, vContactLocalPosition.z, 1.0f), m);
        pContactPosition->x  =vContactWorldPosition.x;
		pContactPosition->y  =vContactWorldPosition.y;
		pContactPosition->z  =vContactWorldPosition.z;
	}
				

}

bool CObjCHAR_Collision2NPC::ContactRay2OBBIn(D3DXMATRIX *m, float xLength, float yLength, float zLength, D3DXVECTOR3 *pPosition)
{
	D3DXMATRIX invM;
	float fPreAvaterPositionXYZ[3];
    D3DXVECTOR4 bufferV1, bufferV2;
	D3DXVECTOR3 localPosition, localDirection;
	D3DXVECTOR3 boxDim;

	D3DXMatrixInverse(&invM, NULL, m);
	::getPrevPosition (m_hNodeModel, fPreAvaterPositionXYZ);
		 
	bufferV1.x = fPreAvaterPositionXYZ[0] + m_vLocalCollisionPoint.x;
	bufferV1.y = fPreAvaterPositionXYZ[1] + m_vLocalCollisionPoint.y;
	bufferV1.z = fPreAvaterPositionXYZ[2] + m_vLocalCollisionPoint.z;
	bufferV1.w = 1.0f;
	
	D3DXVec4Transform(&bufferV2, &bufferV1, &invM);
    localPosition = (D3DXVECTOR3)bufferV2;

	bufferV1.x = -m_vModelDirection.x;
	bufferV1.y = -m_vModelDirection.y;
	bufferV1.z = -m_vModelDirection.z;
	bufferV1.w = 0.0f;
		
	D3DXVec4Transform(&bufferV2, &bufferV1, &invM);
    localDirection = (D3DXVECTOR3)bufferV2;
	
	boxDim.x = xLength;
	boxDim.y = yLength;
	boxDim.z = zLength;

    
	D3DXVECTOR3 Normal;
    float D;
    D3DXVECTOR3 Contact;
	
	for(int i = 0 ; i < 6; i += 1)
	{

		switch(i)
		{
		case 0:

			Normal.x = 1.0f; Normal.y = 0.0f; Normal.z = 0.0f;
            D = boxDim.x;
            break;

		case 1:

			Normal.x = -1.0f; Normal.y = 0.0f; Normal.z = 0.0f;
            D = boxDim.x;
			break;

		case 2:

			Normal.x = 0.0f; Normal.y = 1.0f; Normal.z = 0.0f;
            D = boxDim.y;
			break;

		case 3:

			Normal.x = 0.0f; Normal.y = -1.0f; Normal.z = 0.0f;
            D = boxDim.y;
			break;

		case 4:

			Normal.x = 0.0f; Normal.y = 0.0f; Normal.z = 1.0f;
            D = boxDim.z;
			break;

		case 5:
			
			Normal.x = 0.0f; Normal.y = 0.0f; Normal.z = -1.0f;
            D = boxDim.z;
			break;
		}

		if(D3DXVec3Dot(&localDirection, &Normal) < 0.0f)
		{
			Contact = localPosition + (-(D + D3DXVec3Dot(&localPosition, &Normal)))/(D3DXVec3Dot(&localDirection, &Normal))*localDirection;
            
			if(i < 2)
			{
				if(fabsf(Contact.y) < boxDim.y && fabsf(Contact.z) < boxDim.z)
				{
					
					bufferV1 = (D3DXVECTOR4)Contact;
					bufferV1.w = 1.0f;
					D3DXVec4Transform(&bufferV2, &bufferV1, m);
					*pPosition = (D3DXVECTOR3)bufferV2 - m_vLocalCollisionPoint;
					return true;
				}

			}
			else if(i < 4)
			{
				if(fabsf(Contact.x) < boxDim.x && fabsf(Contact.z) < boxDim.z)
				{
					bufferV1 = (D3DXVECTOR4)Contact;
					bufferV1.w = 1.0f;
					D3DXVec4Transform(&bufferV2, &bufferV1, m);
					*pPosition = (D3DXVECTOR3)bufferV2 - m_vLocalCollisionPoint;
				 	return true;
				}

			}
			else 
			{
				if(fabsf(Contact.x) < boxDim.x && fabsf(Contact.y) < boxDim.y)
				{
					bufferV1 = (D3DXVECTOR4)Contact;
					bufferV1.w = 1.0f;
					D3DXVec4Transform(&bufferV2, &bufferV1, m);
					*pPosition = (D3DXVECTOR3)bufferV2 - m_vLocalCollisionPoint;
				   	return true;
				}



			}
		}


	}

	return false;
}
