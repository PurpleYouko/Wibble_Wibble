#pragma once
#include "stdafx.h"

int lua_GetPatcherVersion( lua_State *L );

int lua_GetVfsVersion( lua_State *L );

int lua_DoPatchFile( lua_State *L );

int lua_GetFileChecksum( lua_State *L );

int lua_RunFile( lua_State *L );

int lua_SetRosePath( lua_State *L );

int lua_DownloadFile( lua_State *L );

int lua_DownloadFile2( lua_State *L );

int lua_SetNewsItem( lua_State *L );

int lua_DeleteFile( lua_State *L );

int lua_UpdatePatcher( lua_State *L );

int lua_SetVfsVersion( lua_State *L );