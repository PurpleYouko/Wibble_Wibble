#pragma once
#include "downloader.h"

class CBSCallbackImpl : public IBindStatusCallback
{
public:
	CBSCallbackImpl( CDownloader* owner );
	~CBSCallbackImpl( );

	HRESULT _stdcall QueryInterface(REFIID, void** );
	ULONG _stdcall AddRef( );
	ULONG _stdcall Release( );
	HRESULT _stdcall OnStartBinding( DWORD, IBinding* );
	HRESULT _stdcall GetPriority( LONG* );
	HRESULT _stdcall OnLowResource( DWORD );
	HRESULT _stdcall OnProgress( ULONG ulProgress, ULONG ulProgressMax, ULONG, LPCWSTR tmp );
	HRESULT _stdcall OnStopBinding( HRESULT, LPCWSTR );
	HRESULT _stdcall GetBindInfo( DWORD*, BINDINFO* );
	HRESULT _stdcall OnDataAvailable( DWORD, DWORD, FORMATETC*, STGMEDIUM* );
	HRESULT _stdcall OnObjectAvailable( REFIID, IUnknown* );

protected:
	ULONG m_ulObjRefCount;
	class CDownloader* m_cdOwner;
};