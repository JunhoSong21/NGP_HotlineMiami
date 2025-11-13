#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

// 기본 헤더
#include <Windows.h>

// Gdiplus
#include <gdiplus.h>
#pragma comment(lib, "gdiplus.lib")
extern ULONG_PTR gdiplusToken;

// 디버그용 헤더
#include <wchar.h>
#include <string>

// 파이 정의
#define PI 3.141592f

// Debug 모드에서만 동작하는 디버그용 함수.
// DEBUG_MSG(L"출력 텍스트"); 와 같이 사용한다.
#ifdef _DEBUG
#define DEBUG_MSG(format, ...)							\
	do{													\
	wchar_t buffer[1024];								\
	swprintf_s(buffer, 1024, format, ##__VA_ARGS__);	\
	OutputDebugString(buffer);							\
    OutputDebugString(L"\n");							\
	} while(0)
#else
#define DEBUG_MSG(format, ...) ((void)0)
#endif