#pragma once
#include "stdafx.h"

LRESULT CALLBACK WindowProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );
ULONG CheckCollision( ULONG x, ULONG y );
void DrawString( char* text, LONG x, LONG y, LONG width, LONG height, LONG flags );
void ReadString( char* text, FILE* fh );
void DoUpdate( );
int RunManualPatch( );
void ExtractPatchFile( char* filename );
void LaunchRose( char* ip, int port );