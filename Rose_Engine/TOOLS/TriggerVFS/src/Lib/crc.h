#ifndef _BYTE_DEFINED
#define _BYTE_DEFINED
typedef unsigned char BYTE;
#endif

#ifndef _WORD_DEFINED
#define _WORD_DEFINED
typedef unsigned short WORD;
#endif

/// CRC value calculated from the data in the memory??
unsigned long getcrc_32 (const BYTE * buff, DWORD dwSize);
/// 32-bit CRC shall be calculated by reading the file
unsigned long getcrc_32_file (const char * FileName);