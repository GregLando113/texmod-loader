
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <d3d9.h>
#include <cstdio>

#include "utils/TexFile.h"
#include "texIDirect3D9.h"

HMODULE g_d3d9dll;

TexFile  g_files[0x10];
unsigned g_count = 0;

static HMODULE LoadD3D9(void)
{
	if (g_d3d9dll == 0)
	{

		TCHAR path[MAX_PATH];
		GetSystemDirectory(path, ARRAYSIZE(path));

		unsigned i = 0;
		while (path[i])
		{
			++i;
		}

		TCHAR* dll = TEXT("\\d3d9.dll");

		unsigned j = 0;
		while (dll[j])
		{
			path[i++] = dll[j++];
		}

		path[i] = 0;
		g_d3d9dll = LoadLibrary(path);
	}

	return g_d3d9dll;
}

IDirect3D9* WINAPI cpp_Direct3DCreate9(
	UINT SDKVersion
)
{
	typedef IDirect3D9* WINAPI Direct3DCreate9_t(UINT);
	static Direct3DCreate9_t* _imp_Direct3DCreate9 = NULL;
	HMODULE hD3D = LoadD3D9();



	if (hD3D && !_imp_Direct3DCreate9) {
		_imp_Direct3DCreate9 = (Direct3DCreate9_t*)GetProcAddress(hD3D, "Direct3DCreate9");
	}

	if (!_imp_Direct3DCreate9)
		return NULL;

	IDirect3D9* d3d9 = _imp_Direct3DCreate9(SDKVersion);
	return new texIDirect3D9(d3d9);
}

extern "C"
{

	__declspec(dllexport)
	IDirect3D9* WINAPI Direct3DCreate9(
		UINT SDKVersion
	)
	{
		return cpp_Direct3DCreate9(SDKVersion);
	}

	__declspec(dllexport)
	HRESULT WINAPI Direct3DCreate9Ex(
		_In_  UINT         SDKVersion,
		_Out_ struct IDirect3D9Ex** ppD3D
	)
	{
		typedef HRESULT WINAPI Direct3DCreate9Ex_t(UINT, struct IDirect3D9Ex**);
		static Direct3DCreate9Ex_t* _imp_Direct3DCreate9Ex = NULL;
		HMODULE hD3D = LoadD3D9();

		if (hD3D && !_imp_Direct3DCreate9Ex) {
			_imp_Direct3DCreate9Ex = (Direct3DCreate9Ex_t*)GetProcAddress(hD3D, "Direct3DCreate9Ex");
		}

		if (_imp_Direct3DCreate9Ex)
			return _imp_Direct3DCreate9Ex(SDKVersion, ppD3D);

		return D3DERR_NOTAVAILABLE;
	}

	__declspec(dllexport)
	int WINAPI D3DPERF_BeginEvent(D3DCOLOR col, LPCWSTR wszName)
	{
		typedef int (WINAPI* BeginEvent_t)(D3DCOLOR, LPCWSTR);
		static BeginEvent_t _imp_BeginEvent = NULL;
		HMODULE hD3D = LoadD3D9();



		if (hD3D && !_imp_BeginEvent)
			_imp_BeginEvent = (BeginEvent_t)GetProcAddress(hD3D, "D3DPERF_BeginEvent");

		if (_imp_BeginEvent)
			return _imp_BeginEvent(col, wszName);

		return D3DERR_NOTAVAILABLE;
	}

	__declspec(dllexport)
	int WINAPI D3DPERF_EndEvent(void)
	{
		typedef int (WINAPI* EndEvent_t)(void);
		static EndEvent_t _imp_EndEvent = NULL;
		HMODULE hD3D = LoadD3D9();



		if (hD3D && !_imp_EndEvent)
			_imp_EndEvent = (EndEvent_t)GetProcAddress(hD3D, "D3DPERF_EndEvent");

		if (_imp_EndEvent)
			return _imp_EndEvent();

		return D3DERR_NOTAVAILABLE;
	}

	__declspec(dllexport)
	void WINAPI D3DPERF_SetMarker(D3DCOLOR col, LPCWSTR wszName)
	{
		typedef VOID(WINAPI* Direct3DSet_t)(D3DCOLOR, LPCWSTR);
		static Direct3DSet_t _imp_SetMarker = NULL;
		HMODULE hD3D = LoadD3D9();



		if (hD3D && !_imp_SetMarker)
			_imp_SetMarker = (Direct3DSet_t)GetProcAddress(hD3D, "D3DPERF_SetMarker");

		if (_imp_SetMarker)
			_imp_SetMarker(col, wszName);
	}

	__declspec(dllexport)
	void WINAPI D3DPERF_SetRegion(D3DCOLOR col, LPCWSTR wszName)
	{
		typedef VOID(WINAPI* Direct3DSet_t)(D3DCOLOR, LPCWSTR);
		static Direct3DSet_t _imp_SetRegion = NULL;
		HMODULE hD3D = LoadD3D9();

		if (hD3D && !_imp_SetRegion)
			_imp_SetRegion = (Direct3DSet_t)GetProcAddress(hD3D, "D3DPERF_SetRegion");

		if (_imp_SetRegion)
			_imp_SetRegion(col, wszName);
	}

	__declspec(dllexport)
	BOOL WINAPI D3DPERF_QueryRepeatFrame(void)
	{
		typedef BOOL(WINAPI* QueryRepeatFrame_t)(void);
		static QueryRepeatFrame_t _imp_QueryRepeatFrame = NULL;
		HMODULE hD3D = LoadD3D9();



		if (hD3D && !_imp_QueryRepeatFrame)
			_imp_QueryRepeatFrame = (QueryRepeatFrame_t)GetProcAddress(hD3D, "D3DPERF_QueryRepeatFrame");

		if (_imp_QueryRepeatFrame)
			return _imp_QueryRepeatFrame();

		return FALSE;
	}

	__declspec(dllexport)
	void WINAPI D3DPERF_SetOptions(DWORD dwOptions)
	{
		typedef void (WINAPI* SetOptions_t)(DWORD);
		static SetOptions_t _imp_SetOptions = NULL;
		HMODULE hD3D = LoadD3D9();

		if (hD3D && !_imp_SetOptions)
			_imp_SetOptions = (SetOptions_t)GetProcAddress(hD3D, "D3DPERF_SetOptions");

		if (_imp_SetOptions)
			_imp_SetOptions(dwOptions);
	}

	__declspec(dllexport)
	DWORD WINAPI D3DPERF_GetStatus(void)
	{
		typedef DWORD(WINAPI* GetStatus_t)(void);
		static GetStatus_t _imp_GetStatus = NULL;
		HMODULE hD3D = LoadD3D9();

		if (hD3D && !_imp_GetStatus)
			_imp_GetStatus = (GetStatus_t)GetProcAddress(hD3D, "D3DPERF_GetStatus");

		if (_imp_GetStatus)
			return _imp_GetStatus();

		return 0;
	}
}


void LoadTextures(void)
{
	WIN32_FIND_DATAA fdata;
	char tempbuffer[MAX_PATH] = "texmods\\";
	char* s = tempbuffer + 8;
	HANDLE fh = FindFirstFileA("texmods\\*.tpf", &fdata);
	if (fh != INVALID_HANDLE_VALUE) do {
		strcpy(s, fdata.cFileName);
		g_files[g_count].LoadFromFile(tempbuffer);
		if (g_files[g_count].error()) {
			printf("ERROR: %s was unable to load. (err=%X)\n", tempbuffer, g_files[g_count].error());
			g_files[g_count].Release();
		}
		else {

			g_count++;
		}
	} while (FindNextFileA(fh, &fdata));
}

BOOL WINAPI DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved)
{
	switch (dwReason) {
	case DLL_PROCESS_ATTACH: {
		LoadTextures();
	} break;
	case DLL_PROCESS_DETACH: { // this should never be called but its here anyways i guess
		for(unsigned i = 0; i < g_count; ++i)
			g_files[i].Release();
	} break;
	}
	return TRUE;
}