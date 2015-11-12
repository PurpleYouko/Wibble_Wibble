#ifndef _CTEventSkill_
#define _CTEventSkill_
#include "ctevent.h"

class CTEventSkill : public CTEvent
{
public:
	CTEventSkill(void);
	virtual ~CTEventSkill(void);
	enum
	{
		EID_NONE,
		EID_ADD_SKILL,
		EID_DEL_SKILL,
		EID_CHANGE_SKILL,
		EID_LEVELUP,
		EID_ADD_LEARNABLE_SKILL,
	};

	void	SetIndex( int iIndex );
	int		GetIndex();
protected:

	int			m_iIndex;
};
#endif