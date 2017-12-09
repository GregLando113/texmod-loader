#include "texIDirect3D9.h"

HRESULT __stdcall texIDirect3D9::CreateDevice(UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow, DWORD BehaviorFlags, D3DPRESENT_PARAMETERS* pPresentationParameters, IDirect3DDevice9** ppReturnedDeviceInterface)
{
	// we intercept this call and provide our own "fake" Device Object
	HRESULT hres = m_original->CreateDevice(Adapter, DeviceType, hFocusWindow, BehaviorFlags, pPresentationParameters, ppReturnedDeviceInterface);

	int count = 1;
	if (pPresentationParameters != NULL) count = pPresentationParameters->BackBufferCount;
	texIDirect3DDevice9 *pIDirect3DDevice9 = new texIDirect3DDevice9(*ppReturnedDeviceInterface);
	// store our pointer (the fake one) for returning it to the calling program
	*ppReturnedDeviceInterface = pIDirect3DDevice9;

	return(hres);
}

HRESULT  __stdcall texIDirect3D9::QueryInterface(REFIID riid, void** ppvObj)
{
	*ppvObj = NULL;

	// call this to increase AddRef at original object
	// and to check if such an interface is there

	HRESULT hRes = m_original->QueryInterface(riid, ppvObj);

	if (hRes == NOERROR) // if OK, send our "fake" address
	{
		*ppvObj = this;
	}

	return (hRes);
}
HRESULT texIDirect3D9::QueryInterface(REFIID riid, void ** ppvObj)
{
	return E_NOTIMPL;
}

ULONG    __stdcall texIDirect3D9::AddRef(void)
{
	return(m_original->AddRef());
}

ULONG    __stdcall texIDirect3D9::Release(void)
{
	// call original routine
	ULONG count = m_original->Release();

	// in case no further Ref is there, the Original Object has deleted itself
	if (count == 0)
	{
		delete(this);
	}

	return(count);
}

HRESULT __stdcall texIDirect3D9::RegisterSoftwareDevice(void* pInitializeFunction)
{
	return(m_original->RegisterSoftwareDevice(pInitializeFunction));
}

UINT __stdcall texIDirect3D9::GetAdapterCount(void)
{
	return(m_original->GetAdapterCount());
}

HRESULT __stdcall texIDirect3D9::GetAdapterIdentifier(UINT Adapter, DWORD Flags, D3DADAPTER_IDENTIFIER9* pIdentifier)
{
	return(m_original->GetAdapterIdentifier(Adapter, Flags, pIdentifier));
}

UINT __stdcall texIDirect3D9::GetAdapterModeCount(UINT Adapter, D3DFORMAT Format)
{
	return(m_original->GetAdapterModeCount(Adapter, Format));
}

HRESULT __stdcall texIDirect3D9::EnumAdapterModes(UINT Adapter, D3DFORMAT Format, UINT Mode, D3DDISPLAYMODE* pMode)
{
	return(m_original->EnumAdapterModes(Adapter, Format, Mode, pMode));
}

HRESULT __stdcall texIDirect3D9::GetAdapterDisplayMode(UINT Adapter, D3DDISPLAYMODE* pMode)
{
	return(m_original->GetAdapterDisplayMode(Adapter, pMode));
}

HRESULT __stdcall texIDirect3D9::CheckDeviceType(UINT iAdapter, D3DDEVTYPE DevType, D3DFORMAT DisplayFormat, D3DFORMAT BackBufferFormat, BOOL bWindowed)
{
	return(m_original->CheckDeviceType(iAdapter, DevType, DisplayFormat, BackBufferFormat, bWindowed));
}

HRESULT __stdcall texIDirect3D9::CheckDeviceFormat(UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT AdapterFormat, DWORD Usage, D3DRESOURCETYPE RType, D3DFORMAT CheckFormat)
{
	return(m_original->CheckDeviceFormat(Adapter, DeviceType, AdapterFormat, Usage, RType, CheckFormat));
}

HRESULT __stdcall texIDirect3D9::CheckDeviceMultiSampleType(UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT SurfaceFormat, BOOL Windowed, D3DMULTISAMPLE_TYPE MultiSampleType, DWORD* pQualityLevels)
{
	return(m_original->CheckDeviceMultiSampleType(Adapter, DeviceType, SurfaceFormat, Windowed, MultiSampleType, pQualityLevels));
}

HRESULT __stdcall texIDirect3D9::CheckDepthStencilMatch(UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT AdapterFormat, D3DFORMAT RenderTargetFormat, D3DFORMAT DepthStencilFormat)
{
	return(m_original->CheckDepthStencilMatch(Adapter, DeviceType, AdapterFormat, RenderTargetFormat, DepthStencilFormat));
}

HRESULT __stdcall texIDirect3D9::CheckDeviceFormatConversion(UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT SourceFormat, D3DFORMAT TargetFormat)
{
	return(m_original->CheckDeviceFormatConversion(Adapter, DeviceType, SourceFormat, TargetFormat));
}

HRESULT __stdcall texIDirect3D9::GetDeviceCaps(UINT Adapter, D3DDEVTYPE DeviceType, D3DCAPS9* pCaps)
{
	return(m_original->GetDeviceCaps(Adapter, DeviceType, pCaps));
}

HMONITOR __stdcall texIDirect3D9::GetAdapterMonitor(UINT Adapter)
{
	return(m_original->GetAdapterMonitor(Adapter));
}