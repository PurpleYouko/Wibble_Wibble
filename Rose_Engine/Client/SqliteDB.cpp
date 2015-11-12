#include "stdafx.h"
#include ".\sqlitedb.h"


CSqliteDB::CSqliteDB(void)
{
	m_db = NULL;
}

CSqliteDB::~CSqliteDB(void)
{

}

CSqliteDB& CSqliteDB::GetInstance()
{
	static CSqliteDB s_Instance;
	return s_Instance;
}

int	CSqliteDB::Open( const char* pszName )
{
	assert( pszName );
	assert( m_db == NULL );
	if( pszName == NULL || m_db != NULL )
		return 1;

	int errcode = sqlite3_open( pszName, &m_db);
	if( m_db == 0 || errcode != SQLITE_OK )
	{
		assert( m_db );
 		return 1;
	}
	return 0;
}

int	CSqliteDB::Close()
{
	assert( m_db );
	if( m_db == NULL )
		return 1;

	sqlite3_close( m_db);
	return 0;
}

int CSqliteDB::Exec( const char* sql ,sqlite3_callback callback )
{
	assert( m_db );
	assert( sql );
	if( m_db == NULL )
		return 1;
	if( sql == NULL )
		return 1;
	
	char* zErrMsg;
	int rc = sqlite3_exec( m_db, sql, callback, 0, &zErrMsg);
	if( rc != SQLITE_OK )
	{
//		g_itMGR.OpenMsgBox( zErrMsg );
		sqlite3_free( zErrMsg );
		return 1;
	}
	return 0;
}
//
//int CSqliteDB::ExecPrintf( const char* sqlFormat, sqlite3_callback callback, ... )
//{
//	assert( m_db );
//	assert( sqlFormat );
//	if( m_db == NULL )
//		return 1;
//	if( sqlFormat == NULL )
//		return 1;
//
//	char* zErrMsg;
//	va_list ap;
//	va_start(ap, zErrMsg);
//	int rc = sqlite3_exec_printf( m_db, sqlFormat, callback, 0, &zErrMsg, ap );
//	va_end(ap);
//	if( rc != SQLITE_OK )
//	{
////		g_itMGR.OpenMsgBox( zErrMsg );
//		sqlite3_freemem( zErrMsg );
//		return 1;
//	}
//	return 0;
//}

int CSqliteDB::AddMemo( const char* pszName, const char* pszSender, const char* pszContent, DWORD time )
{
	assert( m_db );
	if( m_db == NULL )
		return 1;

	char* querystr = sqlite3_mprintf("insert into memo values ( '%q', '%q' ,'%q', %d )", pszName, pszSender, pszContent, time);
	int rc = sqlite3_exec( m_db, querystr, NULL, 0, 0 );
	sqlite3_free(querystr);
	if( rc != SQLITE_OK )
	{
		return 1;
	}
	return 0;
}

int CSqliteDB::LoadMemo( const char* pszName, sqlite3_callback callback )
{
	assert( m_db );
	if( m_db == NULL )
		return 1;

	char* querystr = sqlite3_mprintf("Select ROWID, sender, content, time from memo where name = '%q'", pszName );
	int rc = sqlite3_exec( m_db, querystr, NULL, 0, 0 );
	sqlite3_free(querystr);
	if( rc != SQLITE_OK )
	{
		return 1;
	}
	return 0;
}

int CSqliteDB::DeleteMemo( int iRowid )
{
	assert( m_db );
	if( m_db == NULL )
		return 1;

	char* querystr = sqlite3_mprintf("delete from memo where ROWID = '%d'", iRowid );
	int rc = sqlite3_exec( m_db, querystr, NULL, 0, 0 );
	sqlite3_free(querystr);
	if( rc != SQLITE_OK )
	{
		return 1;
	}
	return 0;
}