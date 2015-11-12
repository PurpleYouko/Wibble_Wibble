/** 
* @file zz_recycling_array.h
* @brief zz_recycling_array class.
* @author Jiho Choi (zho@korea.com)
* @version 1.0
* @date    30-apr-2003
*
* $Header: /engine/include/zz_recycling_array.h 2     04-06-25 3:03a Zho $
*/

#ifndef __ZZ_RECYCLING_ARRAY_H__
#define __ZZ_RECYCLING_ARRAY_H__

//#pragma warning (disable : 4311)
#include "zz_type.h"

template <class TArrayElement> class zz_recycling_array 
{
public:
	zz_recycling_array(int iNumElements)
	{
		m_iNumElements = iNumElements;
		m_iUsedElements = 0;
		m_aElements = new TArrayElement[m_iNumElements];
		m_bAlive = new bool[m_iNumElements];
		memset(m_bAlive, false, m_iNumElements);
	}
	virtual ~zz_recycling_array()
	{
		ZZ_SAFE_DELETE_ARRAY(m_aElements);
		ZZ_SAFE_DELETE_ARRAY(m_bAlive);
		m_iNumElements = 0;
		m_iUsedElements = 0;
	}
	TArrayElement *New()
	{
		if (GetNumFreeElements() < 1)
		{
			ZZ_LOG("zz_recycling_array::New: too many objects!");
			return NULL;
		}
		int i;
		for(i = 0; i < m_iNumElements; i++)
		{
			if(!m_bAlive[i])
			{
				//m_aElements[i] = m_NewTemplate;
				break;
			}
		}
		if (m_bAlive[i])
		{
			// huh? if we got here, there are no free elements in the list... yet 
			// GetNumFreeElements didn't tell us that in the beginning.  Logic error.
			throw(std::logic_error("TArrayElement::New(): internal logic error."));
		}
		m_bAlive[i] = true;
		m_iUsedElements++;
		return(&m_aElements[i]);
	}
	bool Delete(int index)
	{
		if(index < 0 || index > m_iNumElements || !m_bAlive[index])
		{
			return false;
		}
		m_bAlive[index] = false;
		TArrayElement *temp = new TArrayElement[m_iNumElements];
		for(int i = 0; i < m_iNumElements; i++)
		{
			if(i == index)
				continue;
			temp[i] = m_aElements[i];
		}
		delete[] m_aElements;
		m_aElements = new TArrayElement[m_iNumElements];
		for(int i = 0; i < m_iNumElements; i++)
		{
			m_aElements[i] = temp[i];
		}
		ZZ_SAFE_DELETE_ARRAY(temp);
		m_iUsedElements--;
		return true;
	}
	bool Delete(TArrayElement *elem)
	{
		if (m_iUsedElements == 0)
			return(false);
		int iElem = reinterpret_cast<int>(elem);
		iElem -= reinterpret_cast<int>(&m_aElements[0]);
		int index = iElem / sizeof(TArrayElement);
		if (index < 0 || index >= m_iNumElements || !m_bAlive[index])
			return(false);
		m_bAlive[index] = false;
		TArrayElement *temp = new TArrayElement[m_iNumElements];
		for(int i = 0; i < m_iNumElements; i++)
		{
			if(i == index)
				continue;
			temp[i] = m_aElements[i];
		}
		delete[] m_aElements;
		m_aElements = new TArrayElement[m_iNumElements];
		for(int i = 0; i < m_iNumElements; i++)
		{
			m_aElements[i] = temp[i];
		}
		ZZ_SAFE_DELETE_ARRAY(temp);
		m_iUsedElements--;
		return(true);
	}
	int GetNumFreeElements(void)
	{
		return (m_iNumElements - m_iUsedElements);
	}
	int GetNumUsedElements(void)
	{
		return (GetTotalElements() - GetNumFreeElements());
	}
	int GetTotalElements(void)
	{
		return m_iNumElements;
	}
	TArrayElement &GetAt(int index)
	{
		return(m_aElements[index]);
	}
	void DeleteAll(void)
	{
		memset(m_bAlive, false, m_iNumElements);
	}
	bool IsAlive(int index)
	{
		return m_bAlive[index];
	}
private:
	int m_iNumElements;
	int m_iUsedElements;
	TArrayElement* m_aElements;
	bool *m_bAlive;
	TArrayElement m_NewTemplate;
};

#endif // ZZ_RECYCLING_ARRAY
