#ifndef __LIBCRC_H__
#define __LIBCRC_H__

/*******************************************************************
 *
 * CRC Library
 *
 */
class CLibCrc
{
public:
	// Investigate the CrC value.
	static DWORD GetIcarusCrc ( const unsigned char * pbtData, int iLength);
};

#endif
