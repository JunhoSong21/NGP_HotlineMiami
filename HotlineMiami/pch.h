#pragma once

// 기본 헤더
#include <Windows.h>

// Gdiplus
#include <gdiplus.h>
#pragma comment(lib, "gdiplus.lib")
extern ULONG_PTR gdiplusToken;

// 디버그용 헤더
#include <wchar.h>
#include <string>

// Debug 모드에서만 동작하는 디버그용 함수.
// DEBUG_MSG(L"출력 텍스트"); 와 같이 사용한다.
#ifdef _DEBUG
#define DEBUG_MSG(format, ...) {								\
	WCHAR buffer[512];											\
	swprintf_s(buffer, _countof(buffer), format, __VA__ARGS__);	\
	OutputDebugStringW(buffer);									\
	OutputDebugString(L"\n");									\
}
#else
#endif