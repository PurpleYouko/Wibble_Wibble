
#ifndef	__CITEM_H
#define	__CITEM_H
#pragma warning (disable:4201)
//-------------------------------------------------------------------------------------------------
/*
Money (MONEY) designation: 40 (0-999,999)
Item types(ITEM_CLASS)		(1 ~ 20)		: 5  bit  0~31		
Ah system number(ITEM_ID)		(0 ~ 999)		: 10 bit  0~1023	

Re Ming number 1(JAMMING1)		(0~120)			: 7  bit  0~127
Re Ming number 2(JAMMING2)		(0~120)         : 7  bit  0~127
Re Ming number 3(JAMMING3)		(0~120)         : 7  bit  0~127

Enhanced ratings (RESMELT) (0 ~ 9): 4 bit 0 to 15 equipment items only ...
Quality (QUALITY) (0 ~ 120): 7 bit 0 through 127 only if an item of equipment, ...
The number (QUANTITY) (1-999): 10 bit if you are consuming, miscellaneous items, 0 ~ 1023

  Equipment : 5 + 10 + 21 + 11 ==> 15+33 : 48   6 bytes
  Other : 5 + 10 + 10      ==> 15+10
  Don   : 5 + 10 + xx
*/


#include <time.h>

#define	MAX_ITEM_LIFE			1000
#define	MAX_DUP_ITEM_QUANTITY	999

#ifdef __ITEM_MAX

#pragma pack (push, 1)
struct tagPartITEM {
	unsigned int	m_nItemNo		: 18;	// 0~1023	Ah system number(ITEM_ID)		(0 ~ 999)
	unsigned int	m_nGEM_OP		: 9;	// 0~512	Jewelry number(m_bHasSocket==1) Or a number of options(m_bHasSocket==0)
	unsigned int	m_bHasSocket	: 1;	// 0~1		Jewelry, whether or not a socket
	unsigned int	m_cGrade	    : 4;	// 0~15		Rating						(0~9)
} ;

#else

#pragma pack (push, 1)
struct tagPartITEM {
	unsigned int	m_nItemNo		: 10;	// 0~1023	Ah system number(ITEM_ID)		(0 ~ 999)
	unsigned int	m_nGEM_OP		: 9;	// 0~512	Jewelry number(m_bHasSocket==1) Or a number of options(m_bHasSocket==0)
	unsigned int	m_bHasSocket	: 1;	// 0~1		Jewelry, whether or not a socket
	unsigned int	m_cGrade	    : 4;	// 0~15		Rating						(0~9)
} ;

#endif



#ifndef	__SERVER
#define	tagITEM		tagBaseITEM
#endif

int getItemNo(int iFullItemNo);
int getItemType(int iFullItemNo);

int setItemFullNo( int iItemType, int iItemNo );

// ÃÑ 48 bits, 6 bytes
struct tagBaseITEM 
{
	
#ifdef __ITEM_MAX

	union 
	{
		// Equipment item structure
		struct 
		{	
			// LSB ::
			// One of the two is a bit stale as.
			unsigned int	m_cType			: 5;	// 0~31		Item types(ITEM_CLASS)		(1 ~ 20)			
			unsigned int	m_nItemNo		: 26;	// 0~1023	Ah system number(ITEM_ID)		(0 ~ 999)
			unsigned int	m_bCreated		: 1;	// 0~1		The item is manufactured ?

			unsigned int	m_nGEM_OP		: 9;	// 0~512	Jewelry number(m_bHasSocket==1) Or a number of options(m_bHasSocket==0)
			unsigned int	m_cDurability	: 7;	// 0~127	My old

			unsigned int	m_nLife			: 10;	// 0~1023	Life
			unsigned int	m_bHasSocket	: 1;	// 0~1		Jewelry, whether or not a socket
			unsigned int	m_bIsAppraisal	: 1;	// 0~1		Options verification whether or not
			unsigned int	m_cGrade	    : 4;	// 0~15		Rating						(0~9)
			//unsigned int	m_Count			: 1;

			// 16 + 16 + 16 => 48
			// MSB ::
		} ;

		// Consumption, other items structure
		struct 
		{
			unsigned int	m_cType_1		: 5;	// 0~31		Item types(ITEM_CLASS)		(1 ~ 20)
			unsigned int	m_nItemNo_1		: 26;	// 0~1023	Ah system number(ITEM_ID)		(0 ~ 999)
			unsigned int    __dummy_0       : 1;

			unsigned int	m_uiQuantity	: 32;	// The number of (money)
		} ;

		// µ· ¾ÆÀÌÅÛ ±¸Á¶
		struct 
		{
			unsigned int	m_cType_2		: 5;	// 0~31
			unsigned int	m_nReserved1	: 26;
			unsigned int    __dummy_0       : 1;

			unsigned int	m_uiMoney		: 32;
		} ;

		struct 
		{
			unsigned int	m_wHeader		: 32;	//The server is the name of a heck of a place to fix fixes m_dwHeader.
			unsigned int	m_dwBody		: 32;
		} ;

		struct 
		{
			unsigned int	m_dwLSB;
			unsigned int	m_dwMSB;
		} ;		

		unsigned int	m_dwITEM;
	} ;


//#ifdef __ITEM_TIME_LIMMIT

	struct 
	{
		DWORD dwPickOutTime;
		WORD  wPeriod;
	};	

	// On January 1, 1970 00:00:00 January 01, 2007 0:00:00 time in seconds
	time_t tagBaseITEM::get_basicTime()
	{
		time_t basic_t;
		struct tm t;

		// 2007, Jan 01, 00:00:00 standard
		t.tm_year = 2007 - 1900; t.tm_mon = 1 - 1; t.tm_mday = 1; t.tm_hour = 0; t.tm_min = 0; t.tm_sec = 0;
		basic_t = mktime( &t);	

		return basic_t;
	}

	//Apply time
	tm tagBaseITEM::get_startTime()
	{		
		time_t long_time = (dwPickOutTime) + get_basicTime();
		struct tm l = *localtime( &long_time );
		l.tm_year += 1900;
		l.tm_mon += 1;
		return l;
	}
	
	//Time-lapse
	tm tagBaseITEM::get_endTime()
	{		
		time_t long_time = (dwPickOutTime) + get_basicTime() + (wPeriod) * 3600 ;
		
		struct tm l = *localtime( &long_time );
		//struct tm l = *gmtime( &long_time );
		
		l.tm_year += 1900;
		l.tm_mon += 1;
		if( l.tm_min )
		{
            l.tm_hour += 1;            
			l.tm_min = 0;
			l.tm_sec = 0;
		}		
		return l;
	}

//#endif


#else

	union {
		// Equipment item structure
		struct {	
			// LSB ::
			// One of the two is a bit stale.
			unsigned short	m_cType			: 5;	// 0~31		Item types(ITEM_CLASS)		(1 ~ 20)			
			unsigned short	m_nItemNo		: 10;	// 0~1023	Ah system number(ITEM_ID)		(0 ~ 999)
			unsigned short	m_bCreated		: 1;	// 0~1		The item is manufactured ?

			unsigned int	m_nGEM_OP		: 9;	// 0~512	Jewelry number(m_bHasSocket==1) Or a number of options(m_bHasSocket==0)
			unsigned int	m_cDurability	: 7;	// 0~127	My old

			unsigned int	m_nLife			: 10;	// 0~1023	Life
			unsigned int	m_bHasSocket	: 1;	// 0~1		Jewelry, whether or not a socket
			unsigned int	m_bIsAppraisal	: 1;	// 0~1		Options verification whether or not
			unsigned int	m_cGrade	    : 4;	// 0~15		Rating						(0~9)

			// 16 + 16 + 16 => 48
			// MSB ::
		} ;

		// Consumption, other items structure
		struct {
			unsigned short	m_cType_1		: 5;	// 0~31		Item types(ITEM_CLASS)		(1 ~ 20)
			unsigned short	m_nItemNo_1		: 10;	// 0~1023	Ah system number(ITEM_ID)		(0 ~ 999)

			unsigned int	m_uiQuantity	: 32;	// The number of (money)
		} ;

		// Money item structure
		struct {
			unsigned short	m_cType_2		: 5;	// 0~31
			unsigned short	m_nReserved1	: 11;

			unsigned int	m_uiMoney		: 32;
		} ;

		struct {
			unsigned short	m_wHeader		: 16;
			unsigned int	m_dwBody		: 32;
		} ;

		struct {
			DWORD	m_dwLSB;
			WORD	m_wMSB;
		} ;

		struct {
			WORD	m_wLSB;
			DWORD	m_dwMSB;
		} ;

		DWORD	m_dwITEM;
	} ;
#endif


	void		   Init(int iItem, unsigned int uiQuantity=1);

#ifdef __ITEM_MAX
	void			Clear ()				
	{
		m_dwLSB=m_dwMSB=0;
#ifdef __ITEM_TIME_LIMMIT
		dwPickOutTime = wPeriod = 0;
#endif
	}
	unsigned int	GetItemNO ()			{	return	m_nItemNo;				}
	void			SetItemNo (short ItmNo)	{	m_nItemNo = ItmNo;				}
	bool			IsEmpty ()				{	return (0==m_wHeader);			}
	unsigned short	GetHEADER ()			{	return (m_wHeader & 0x7fffffff);	}	// m_bCreated :: Compared to manufacturing without the header a bit. ...
#else
	void			Clear ()				{	m_dwLSB=m_wMSB=0;				}
	unsigned short	GetItemNO ()			{	return	m_nItemNo;				}
	bool			IsEmpty ()				{	return (0==m_wHeader);			}
	unsigned short	GetHEADER ()			{	return (m_wHeader & 0x7fff);	}	// m_bCreated :: Compared to manufacturing without a bit of the header...

#endif
	

	unsigned short GetTYPE ()			{	return	m_cType;				}
	unsigned short GetTYPE1()			{	return	m_cType_1;				}
	void		   SetType (short Type) {   m_cType = Type;					}
	void		   SetType1(short Type) {   m_cType_1 = Type;				}

	unsigned short GetGrade ()			{	return	m_cGrade;		}
	unsigned short GetOption ()			{	return  m_nGEM_OP;		}
	unsigned short GetGemNO ()			{	return  m_nGEM_OP;		}

	unsigned short GetLife()			{	return  m_nLife;		}
	unsigned short GetDurability()		{	return  m_cDurability;	}
	unsigned int   GetMoney ()			{	return	m_uiMoney;		}

	

	bool IsAppraisal()					{	return (0!=m_bIsAppraisal);		}
	bool HasSocket()					{	return (0!=m_bHasSocket);		}

	bool IsEnableDROP ();					// Discard the item is available ?
	bool IsEnableSELL ();					// Is possible to sell items ?
	bool IsEnableKEEPING ();				// Items kept in the Bank is ?

	bool IsValidITEM ();

	static bool IsEnableDupCNT( unsigned short cType )		
	{	
		// Applies the number of duplicate items, because???
		return (cType>=ITEM_TYPE_USE && cType<ITEM_TYPE_RIDE_PART);
	}
	bool IsEnableDupCNT()				{	return IsEnableDupCNT(m_cType);	}
	bool IsCreated()					{	return (1==m_bCreated);			}
	bool IsEquipITEM()					{	return ( m_cType && m_cType < ITEM_TYPE_USE );					}	// Mount items?
	bool IsEtcITEM()					{	return ( m_cType>ITEM_TYPE_USE && m_cType<ITEM_TYPE_QUEST);		}

	bool IsTwoHands ();

	t_eSHOT			GetShotTYPE();
	t_EquipINDEX	GetEquipPOS();
#ifdef	__SERVER
	unsigned int	GetQuantity ()		{	return	m_uiQuantity;			}
#else
	unsigned int	GetQuantity ();
	unsigned int	GetQuantity1()		{   return	m_uiQuantity;			}
	void			SetQuantity1( short Count )	{ m_uiQuantity = Count;		}
	void			IncQuantity1()		{	m_uiQuantity ++;				}				
	//unsigned int	GetCount ()		{   return  m_Count;				}
	//void			SetQuantity (byte Count);		// Set the quantity of an item
	short			Subtract( tagITEM &sITEM );		// As much as the given item is contained in the missing results in relieving sITEM.
	void			SubtractOnly (tagITEM &sITEM);	// As long as a given item for relieving.

	bool			IsEnableAppraisal ();					///Emotional items?
	bool			IsEnableExchange ();					// Discard items as possible?
	bool			IsEnableSeparate ();
	bool			IsEnableUpgrade ();
	bool			HasLife();
	bool			HasDurability();
	unsigned short	GetModelINDEX ();
	char*			GettingMESSAGE (int iInventoryListNo);
	char*			GettingMESSAGE ();
	char*			GettingMESSAGE_Party (const char * partyName_);
	char*			GettingQuestMESSAGE();
	char*			SubtractQuestMESSAGE();
	///Consuming Tan items ShotTypeÀ» ¾ò±â
	static t_eSHOT GetNaturalBulletType( int iItemNo );
	t_eSHOT GetBulletType();
	///Famous gravity
	int				GetHitRate();
	int				GetAvoidRate();
	bool			IsEqual( int iType, int iItemNo );
	int				GetUpgradeCost();
	int				GetSeparateCost();
	int				GetAppraisalCost();
	const	char*	GetName();
#endif
} ;


//-------------------------------------------------------------------------------------------------
#ifdef	__SERVER
struct tagITEM : public tagBaseITEM {
	union {
		DWORD	m_dwSN[2];
		__int64	m_iSN;
	} ;

	// After subtracting the current item being given in item weighs as much as returns,
	short	Subtract( tagITEM &sITEM );		// As much as the given item is contained in the missing results in relieving sITEM.
	void	SubtractOnly (tagITEM &sITEM);	// As long as a given item for relieving.

	bool SubQuantity ()		{	
		if ( GetQuantity() > 0 ) {
			--m_uiQuantity;
			return true; 
		}
		return false;
	}
	bool SubQuantity(unsigned int uiCnt)
	{
		if ( GetQuantity() > uiCnt ) {
			m_uiQuantity -= uiCnt;
			return true;
		} else
		if ( m_uiQuantity == uiCnt ) {
			this->Clear ();
			return true;
		}
		return false;
	}

	void 	Init(int iItem, unsigned int uiQuantity=1)
	{
		tagBaseITEM::Init(iItem, uiQuantity);
		m_iSN = 0;
	}
	void	Clear ()
	{	
		tagBaseITEM::Clear();
		m_iSN = 0;
	}

#ifndef	__BORLANDC__

#ifdef __ITEM_MAX
	void operator=(tagBaseITEM &rBASE)
	{
		m_dwLSB  = rBASE.m_dwLSB;
		m_dwMSB = rBASE.m_dwMSB;
		m_iSN   = 0;
	}
#else
	void operator=(tagBaseITEM &rBASE)
	{
		m_wLSB  = rBASE.m_wLSB;
		m_dwMSB = rBASE.m_dwMSB;
		m_iSN   = 0;
	}
#endif
	
#endif
} ;
#endif

//The server uses the client rang to communicate except for structure configuration serial key ...
//On the client, you can use the CMileageInv as in the as. ..
struct Net_MileageItem:public tagBaseITEM
{
	DWORD m_dwDate;
};

#ifdef  __SERVER
struct MileageItem:public tagITEM
{
	DWORD m_dwDate;
	void 	Init(int iItem, unsigned int uiQuantity=1)
	{
		tagITEM::Init(iItem, uiQuantity);
		m_dwDate = 0;
	}

	void	Clear ()
	{	
		tagITEM::Clear();
		m_dwDate =0;
	}


};

#else

typedef  Net_MileageItem MileageItem;

#endif

#pragma pack (pop)

//-------------------------------------------------------------------------------------------------
#pragma warning (default:4201)
#endif