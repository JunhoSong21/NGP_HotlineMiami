#pragma once

// 기본 헤더
#include <Windows.h>

// CImage, Gdiplus 헤더
#include <atlimage.h>
#include <gdiplus.h>

// 디버그용 헤더
#include <wchar.h>
#include <string>

#ifdef _DEBUG
#define DEBUG_MSG(msg) {		\
	OutputDebugStringW(msg);	\
	OutputDebugStringW(L"\n");	\
}
#else
#endif