#include "stdafx.h"
#include "libCrc.h"
#include "classCRC.h"


// Investigate the CrC value 
DWORD CLibCrc::GetIcarusCrc ( const unsigned char * pbtData, int lLength)
{
	return classCRC::DataCRC32 (pbtData, lLength, true );
};