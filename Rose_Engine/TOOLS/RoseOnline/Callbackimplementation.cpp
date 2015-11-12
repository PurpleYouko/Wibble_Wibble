#include "stdafx.h"
#include "Callbackimplementation.h"

CBSCallbackImpl::CBSCallbackImpl( CDownloader* owner )
{
	m_ulObjRefCount = 0;
	m_cdOwner = owner;
};
CBSCallbackImpl::~CBSCallbackImpl( )
{
};
HRESULT _stdcall CBSCallbackImpl::QueryInterface(REFIID, void** )
{
	return S_OK;
};
ULONG _stdcall CBSCallbackImpl::AddRef( )
{
	return ++m_ulObjRefCount;
};
ULONG _stdcall CBSCallbackImpl::Release( )
{
	return --m_ulObjRefCount;
};
HRESULT _stdcall CBSCallbackImpl::OnStartBinding( DWORD, IBinding* )
{
	return S_OK;
};
HRESULT _stdcall CBSCallbackImpl::GetPriority( LONG* )
{
	return E_NOTIMPL;
};
HRESULT _stdcall CBSCallbackImpl::OnLowResource( DWORD )
{
	return S_OK;
};
HRESULT _stdcall CBSCallbackImpl::OnProgress( ULONG ulProgress, ULONG ulProgressMax, ULONG, LPCWSTR tmp )
{
	m_cdOwner->OnProgress( ulProgress, ulProgressMax );
	return S_OK;
};
HRESULT _stdcall CBSCallbackImpl::OnStopBinding( HRESULT, LPCWSTR )
{
	return S_OK;
};
HRESULT _stdcall CBSCallbackImpl::GetBindInfo( DWORD*, BINDINFO* )
{
	return S_OK;
};
HRESULT _stdcall CBSCallbackImpl::OnDataAvailable( DWORD, DWORD, FORMATETC*, STGMEDIUM* )
{
	return S_OK;
};
HRESULT _stdcall CBSCallbackImpl::OnObjectAvailable( REFIID, IUnknown* )
{
	return S_OK;
};