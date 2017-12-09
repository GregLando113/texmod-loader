#include "texIDirect3DDevice9.h"

HRESULT texIDirect3DDevice9::QueryInterface(REFIID riid, void** ppvObj)
{

	*ppvObj = NULL;
	HRESULT hRes = m_original->QueryInterface(riid, ppvObj);

	if (*ppvObj == m_original)
	{
		*ppvObj = this;
	}

	return (hRes);
}

ULONG texIDirect3DDevice9::AddRef(void)
{
	return (m_original->AddRef());
}

ULONG texIDirect3DDevice9::Release(void)
{
	if (--uMod_Reference == 0) //if our counter drops to zero, the real device will be deleted, so we clean up before
	{
		// we must not release the fake textures, cause they are released if the target textures are released
		// and the target textures are released by the game.

		if (SingleTexture != NULL) SingleTexture->Release(); //this is the only texture we must release by ourself
		if (SingleVolumeTexture != NULL) SingleVolumeTexture->Release(); //this is the only texture we must release by ourself
		if (SingleCubeTexture != NULL) SingleCubeTexture->Release(); //this is the only texture we must release by ourself
		if (OSD_Font != NULL) OSD_Font->Release();
		if (pSprite != NULL) pSprite->Release();

		if (uMod_Client != NULL) delete uMod_Client; //must be deleted at the end, because other releases might call a function of this object

		uMod_Client = NULL;
		SingleTexture = NULL;
		SingleVolumeTexture = NULL;
		SingleCubeTexture = NULL;
		OSD_Font = NULL;
		pSprite = NULL;
	}

	ULONG count = m_original->Release();
	Message("%lu = " PRE_MESSAGE "::Release(): %p\n", count, this);
	if (uMod_Reference != count) //bug
	{
		Message("Error in " PRE_MESSAGE "::Release(): %lu!=%lu\n", uMod_Reference, count);
	}

	if (count == 0u)  delete(this);
	return (count);
}

HRESULT texIDirect3DDevice9::TestCooperativeLevel(void)
{
	return(m_original->TestCooperativeLevel());
}

UINT texIDirect3DDevice9::GetAvailableTextureMem(void)
{
	return(m_original->GetAvailableTextureMem());
}

HRESULT texIDirect3DDevice9::EvictManagedResources(void)
{
	return(m_original->EvictManagedResources());
}

HRESULT texIDirect3DDevice9::GetDirect3D(IDirect3D9** ppD3D9)
{
	return(m_original->GetDirect3D(ppD3D9));
}

HRESULT texIDirect3DDevice9::GetDeviceCaps(D3DCAPS9* pCaps)
{
	return(m_original->GetDeviceCaps(pCaps));
}

HRESULT texIDirect3DDevice9::GetDisplayMode(UINT iSwapChain, D3DDISPLAYMODE* pMode)
{
	return(m_original->GetDisplayMode(iSwapChain, pMode));
}

HRESULT texIDirect3DDevice9::GetCreationParameters(D3DDEVICE_CREATION_PARAMETERS *pParameters)
{
	return(m_original->GetCreationParameters(pParameters));
}

HRESULT texIDirect3DDevice9::SetCursorProperties(UINT XHotSpot, UINT YHotSpot, IDirect3DSurface9* pCursorBitmap)
{
	return(m_original->SetCursorProperties(XHotSpot, YHotSpot, pCursorBitmap));
}

void texIDirect3DDevice9::SetCursorPosition(int X, int Y, DWORD Flags)
{
	m_original->SetCursorPosition(X, Y, Flags);
}

BOOL texIDirect3DDevice9::ShowCursor(BOOL bShow)
{
	return(m_original->ShowCursor(bShow));
}

HRESULT texIDirect3DDevice9::CreateAdditionalSwapChain(D3DPRESENT_PARAMETERS* pPresentationParameters, IDirect3DSwapChain9** pSwapChain)
{
	return(m_original->CreateAdditionalSwapChain(pPresentationParameters, pSwapChain));
}

HRESULT texIDirect3DDevice9::GetSwapChain(UINT iSwapChain, IDirect3DSwapChain9** pSwapChain)
{
	return(m_original->GetSwapChain(iSwapChain, pSwapChain));
}

UINT texIDirect3DDevice9::GetNumberOfSwapChains(void)
{
	return(m_original->GetNumberOfSwapChains());
}

HRESULT texIDirect3DDevice9::Reset(D3DPRESENT_PARAMETERS* pPresentationParameters)
{
	if (OSD_Font != NULL) { OSD_Font->Release(); OSD_Font = NULL; } //the game will crashes if the font is not released before the game is minimized!
	if (pSprite != NULL) { pSprite->Release(); pSprite = NULL; }
	return(m_original->Reset(pPresentationParameters));
}

HRESULT texIDirect3DDevice9::Present(CONST RECT* pSourceRect, CONST RECT* pDestRect, HWND hDestWindowOverride, CONST RGNDATA* pDirtyRegion)
{
	return (m_original->Present(pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion));
}

HRESULT texIDirect3DDevice9::GetBackBuffer(UINT iSwapChain, UINT iBackBuffer, D3DBACKBUFFER_TYPE Type, IDirect3DSurface9** ppBackBuffer)
{
	return(m_original->GetBackBuffer(iSwapChain, iBackBuffer, Type, ppBackBuffer));
}

HRESULT texIDirect3DDevice9::GetRasterStatus(UINT iSwapChain, D3DRASTER_STATUS* pRasterStatus)
{
	return(m_original->GetRasterStatus(iSwapChain, pRasterStatus));
}

HRESULT texIDirect3DDevice9::SetDialogBoxMode(BOOL bEnableDialogs)
{
	return(m_original->SetDialogBoxMode(bEnableDialogs));
}

void texIDirect3DDevice9::SetGammaRamp(UINT iSwapChain, DWORD Flags, CONST D3DGAMMARAMP* pRamp)
{
	(m_original->SetGammaRamp(iSwapChain, Flags, pRamp));
}

void texIDirect3DDevice9::GetGammaRamp(UINT iSwapChain, D3DGAMMARAMP* pRamp)
{
	(m_original->GetGammaRamp(iSwapChain, pRamp));
}

HRESULT texIDirect3DDevice9::CreateTexture(UINT Width, UINT Height, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DTexture9** ppTexture, HANDLE* pSharedHandle)
{
	//create real texture
	//Message("texIDirect3DDevice9::CreateTexture()\n");
	HRESULT ret = (m_original->CreateTexture(Width, Height, Levels, Usage, Format, Pool, ppTexture, pSharedHandle));
	if (ret != D3D_OK) return (ret);

	//create fake texture
	uMod_IDirect3DTexture9 *texture = new uMod_IDirect3DTexture9(ppTexture, this);
	if (texture) *ppTexture = texture;

	if (LastCreatedTexture != NULL) //if a texture was loaded before, hopefully this texture contains now the data, so we can add it
	{
		if (uMod_Client != NULL) uMod_Client->AddTexture(LastCreatedTexture);
	}
	LastCreatedTexture = texture;
	return (ret);
}

HRESULT texIDirect3DDevice9::CreateVolumeTexture(UINT Width, UINT Height, UINT Depth, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DVolumeTexture9** ppVolumeTexture, HANDLE* pSharedHandle)
{
	//create real texture
	//Message("texIDirect3DDevice9::CreateVolumeTexture()\n");
	HRESULT ret = (m_original->CreateVolumeTexture(Width, Height, Depth, Levels, Usage, Format, Pool, ppVolumeTexture, pSharedHandle));
	if (ret != D3D_OK) return (ret);

	//create fake texture
	uMod_IDirect3DVolumeTexture9 *texture = new uMod_IDirect3DVolumeTexture9(ppVolumeTexture, this);
	if (texture) *ppVolumeTexture = texture;

	if (LastCreatedVolumeTexture != NULL) //if a texture was loaded before, hopefully this texture contains now the data, so we can add it
	{
		if (uMod_Client != NULL) uMod_Client->AddTexture(LastCreatedVolumeTexture);
	}
	LastCreatedVolumeTexture = texture;
	return (ret);
}

HRESULT texIDirect3DDevice9::CreateCubeTexture(UINT EdgeLength, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DCubeTexture9** ppCubeTexture, HANDLE* pSharedHandle)
{
	//create real texture
	//Message("texIDirect3DDevice9::CreateCubeTexture()\n");
	HRESULT ret = (m_original->CreateCubeTexture(EdgeLength, Levels, Usage, Format, Pool, ppCubeTexture, pSharedHandle));
	if (ret != D3D_OK) return (ret);

	//create fake texture
	uMod_IDirect3DCubeTexture9 *texture = new uMod_IDirect3DCubeTexture9(ppCubeTexture, this);
	if (texture) *ppCubeTexture = texture;

	if (LastCreatedCubeTexture != NULL) //if a texture was loaded before, hopefully this texture contains now the data, so we can add it
	{
		if (uMod_Client != NULL) uMod_Client->AddTexture(LastCreatedCubeTexture);
	}
	LastCreatedCubeTexture = texture;
	return (ret);
}

HRESULT texIDirect3DDevice9::CreateVertexBuffer(UINT Length, DWORD Usage, DWORD FVF, D3DPOOL Pool, IDirect3DVertexBuffer9** ppVertexBuffer, HANDLE* pSharedHandle)
{
	return(m_original->CreateVertexBuffer(Length, Usage, FVF, Pool, ppVertexBuffer, pSharedHandle));
}

HRESULT texIDirect3DDevice9::CreateIndexBuffer(UINT Length, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DIndexBuffer9** ppIndexBuffer, HANDLE* pSharedHandle)
{
	return(m_original->CreateIndexBuffer(Length, Usage, Format, Pool, ppIndexBuffer, pSharedHandle));
}

HRESULT texIDirect3DDevice9::CreateRenderTarget(UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Lockable, IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle)
{
	return(m_original->CreateRenderTarget(Width, Height, Format, MultiSample, MultisampleQuality, Lockable, ppSurface, pSharedHandle));
}

HRESULT texIDirect3DDevice9::CreateDepthStencilSurface(UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Discard, IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle)
{
	return(m_original->CreateDepthStencilSurface(Width, Height, Format, MultiSample, MultisampleQuality, Discard, ppSurface, pSharedHandle));
}

HRESULT texIDirect3DDevice9::UpdateSurface(IDirect3DSurface9* pSourceSurface, CONST RECT* pSourceRect, IDirect3DSurface9* pDestinationSurface, CONST POINT* pDestPoint)
{
	Message(PRE_MESSAGE "::UpdateSurface( %p, %p, %p, %p): %p\n", pSourceSurface, pSourceRect, pDestinationSurface, pDestPoint, this);

	uMod_IDirect3DSurface9* pSource = NULL;
	uMod_IDirect3DSurface9* pDest = NULL;
	IDirect3DSurface9* cpy;
	DWORD64 crc64 = 0ULL;
	DWORD32 crc32 = 0u;

	if (pSourceSurface != NULL)
	{
		long int ret = pSourceSurface->QueryInterface(IID_IDirect3D9, (void**)&cpy);
		if (ret == 0x01000000L)
		{
			pSource = (uMod_IDirect3DSurface9*)pSourceSurface;
			if (pSourceRect == NULL && pSource->m_D3DTex != NULL)
			{
				crc64 = pSource->m_D3DTex->CRC64;
				crc32 = pSource->m_D3DTex->CRC32;
			}
			pSourceSurface = pSource->m_D3Dsurf;
		}
	}
	if (pDestinationSurface != NULL)
	{
		long int ret = pDestinationSurface->QueryInterface(IID_IDirect3D9, (void**)&cpy);
		if (ret == 0x01000000L)
		{
			pDest = (uMod_IDirect3DSurface9*)pDestinationSurface;

			// copy the CRC makes only sense, if both rects point to NULL (the whole surface is copied)
			// and if the both textures are not cube textures, thus the hash from the texture is also the hash
			// from the surface
			if (pDestPoint == NULL && crc64>0 && pDest->m_D3DTex != NULL)
			{
				pDest->m_D3DTex->CRC64 = crc64;
				pDest->m_D3DTex->CRC32 = crc32;
				UnswitchTextures(pDest->m_D3DTex);
				uMod_Client->LookUpToMod(pDest->m_D3DTex);
			}
			else // compute the CRC again
			{
				if (pDest->m_D3DTex != NULL)
				{
					if (pDest->m_D3DTex->ComputetHash(uMod_Client->BoolComputeCRC) == RETURN_OK)
					{
						UnswitchTextures(pDest->m_D3DTex);
						uMod_Client->LookUpToMod(pDest->m_D3DTex);
					}
				}
				else if (pDest->m_D3DCubeTex != NULL)
				{
					if (pDest->m_D3DCubeTex->ComputetHash(uMod_Client->BoolComputeCRC) == RETURN_OK)
					{
						UnswitchTextures(pDest->m_D3DCubeTex);
						uMod_Client->LookUpToMod(pDest->m_D3DCubeTex);
					}
				}
			}
			pDestinationSurface = pDest->m_D3Dsurf;
		}
	}

	return(m_original->UpdateSurface(pSourceSurface, pSourceRect, pDestinationSurface, pDestPoint));
}

HRESULT texIDirect3DDevice9::UpdateTexture(IDirect3DBaseTexture9* pSourceTexture, IDirect3DBaseTexture9* pDestinationTexture)
{
	Message(PRE_MESSAGE "::UpdateTexture( %p, %p): %p\n", pSourceTexture, pDestinationTexture, this);
	// we must pass the real texture objects


	uMod_IDirect3DTexture9* pSource = NULL;
	uMod_IDirect3DVolumeTexture9* pSourceVolume = NULL;
	uMod_IDirect3DCubeTexture9* pSourceCube = NULL;
	IDirect3DBaseTexture9* cpy;
	if (pSourceTexture != NULL)
	{
		long int ret = pSourceTexture->QueryInterface(IID_IDirect3D9, (void**)&cpy);
		switch (ret)
		{
		case 0x01000000L:
		{
			pSource = (uMod_IDirect3DTexture9*)(pSourceTexture);
			DWORD64 crc64 = pSource->CRC64;
			DWORD32 crc32 = pSource->CRC32;
			if (pSource->ComputetHash(crc32>0) == RETURN_OK)
			{
				if (crc64 != pSource->CRC64 || (crc32>0 && crc32 != pSource->CRC32)) // this hash has changed !!
				{
					UnswitchTextures(pSource);
					uMod_Client->LookUpToMod(pSource);
				}
			}
			else UnswitchTextures(pSource); // we better unswitch

											// the source must be the original texture if not switched and the fake texture if it is switched
			if (pSource->CrossRef_D3Dtex != NULL) pSourceTexture = pSource->CrossRef_D3Dtex->m_D3Dtex;
			else pSourceTexture = pSource->m_D3Dtex;
			break;
		}
		case 0x01000001L:
		{
			pSourceVolume = (uMod_IDirect3DVolumeTexture9*)(pSourceTexture);
			DWORD64 crc64 = pSourceVolume->CRC64;
			DWORD32 crc32 = pSourceVolume->CRC32;
			if (pSourceVolume->ComputetHash(crc32>0) == RETURN_OK)
			{
				if (crc64 != pSourceVolume->CRC64 || (crc32>0 && crc32 != pSource->CRC32)) // this hash has changed !!
				{
					UnswitchTextures(pSourceVolume);
					uMod_Client->LookUpToMod(pSourceVolume);
				}
			}
			else UnswitchTextures(pSourceVolume); // we better unswitch

												  // the source must be the original texture if not switched and the fake texture if it is switched
			if (pSourceVolume->CrossRef_D3Dtex != NULL) pSourceTexture = pSourceVolume->CrossRef_D3Dtex->m_D3Dtex;
			else pSourceTexture = pSourceVolume->m_D3Dtex;
			break;
		}
		case 0x01000002L:
		{
			pSourceCube = (uMod_IDirect3DCubeTexture9*)(pSourceTexture);
			DWORD64 crc64 = pSourceCube->CRC64;
			DWORD32 crc32 = pSourceCube->CRC32;
			if (pSourceCube->ComputetHash(crc32>0) == RETURN_OK)
			{
				if (crc64 != pSourceCube->CRC64 || (crc32>0 && crc32 != pSource->CRC32)) // this hash has changed !!
				{
					UnswitchTextures(pSourceCube);
					uMod_Client->LookUpToMod(pSourceCube);
				}
			}
			else UnswitchTextures(pSourceCube); // we better unswitch

												// the source must be the original texture if not switched and the fake texture if it is switched
			if (pSourceCube->CrossRef_D3Dtex != NULL) pSourceTexture = pSourceCube->CrossRef_D3Dtex->m_D3Dtex;
			else pSourceTexture = pSourceCube->m_D3Dtex;
			break;
		}
		default:
			break; // this is no fake texture and QueryInterface failed, because IDirect3DBaseTexture9 object cannot be a IDirect3D9 object ;)
		}
	}


	if (pDestinationTexture != NULL)
	{
		long int ret = pDestinationTexture->QueryInterface(IID_IDirect3D9, (void**)&cpy);
		switch (ret)
		{
		case 0x01000000L:
		{
			uMod_IDirect3DTexture9* pDest = (uMod_IDirect3DTexture9*)(pDestinationTexture);

			if (pSource != NULL) // we have the crc from the source
			{
				pDest->CRC64 = pSource->CRC64; // take over the hash
				pDest->CRC32 = pSource->CRC32; // take over the hash

				UnswitchTextures(pDest); // we can always unswitch !
				pDestinationTexture = pDest->m_D3Dtex; // set the correct destination texture
				if (pSource->CrossRef_D3Dtex != NULL)
				{
					uMod_Client->LookUpToMod(pDest);
					//uMod_IDirect3DTexture9 *cpy = pSource->CrossRef_D3Dtex;
					//UnswitchTextures(pSource);
					//SwitchTextures( cpy, pDest);
				}
			}
			else if (pDest->ComputetHash(uMod_Client->BoolComputeCRC) == RETURN_OK) // we must compute the crc
			{
				uMod_Client->LookUpToMod(pDest);
			}
			break;
		}
		case 0x01000001L:
		{
			uMod_IDirect3DVolumeTexture9* pDest = (uMod_IDirect3DVolumeTexture9*)(pDestinationTexture);

			if (pSourceVolume != NULL) // we have the crc from the source
			{
				pDest->CRC64 = pSourceVolume->CRC64; // take over the hash
				pDest->CRC32 = pSource->CRC32; // take over the hash

				UnswitchTextures(pDest); // we can always unswitch !
				pDestinationTexture = pDest->m_D3Dtex; // set the correct destination texture
				if (pSourceVolume->CrossRef_D3Dtex != NULL)
				{
					uMod_Client->LookUpToMod(pDest);
					//uMod_IDirect3DVolumeTexture9 *cpy = pSourceVolume->CrossRef_D3Dtex;
					//UnswitchTextures(pSourceVolume);
					//SwitchTextures( cpy, pDest);
				}
			}
			else if (pDest->ComputetHash(uMod_Client->BoolComputeCRC) == RETURN_OK) // we must compute the crc
			{
				uMod_Client->LookUpToMod(pDest);
			}
			break;
		}
		case 0x01000002L:
		{
			uMod_IDirect3DCubeTexture9* pDest = (uMod_IDirect3DCubeTexture9*)(pDestinationTexture);

			if (pSourceCube != NULL) // we have the crc from the source
			{
				pDest->CRC64 = pSourceCube->CRC64; // take over the hash
				pDest->CRC32 = pSource->CRC32; // take over the hash

				UnswitchTextures(pDest); // we can always unswitch !
				pDestinationTexture = pDest->m_D3Dtex; // set the correct destination texture
				if (pSourceCube->CrossRef_D3Dtex != NULL)
				{
					uMod_Client->LookUpToMod(pDest);
					//uMod_IDirect3DCubeTexture9 *cpy = pSourceCube->CrossRef_D3Dtex;
					//UnswitchTextures(pSourceCube);
					//SwitchTextures( cpy, pDest);
				}
			}
			else if (pDest->ComputetHash(uMod_Client->BoolComputeCRC) == RETURN_OK) // we must compute the crc
			{
				uMod_Client->LookUpToMod(pDest);
			}
			break;
		}
		default:
			break; // this is no fake texture and QueryInterface failed, because IDirect3DBaseTexture9 object cannot be a IDirect3D9 object ;)
		}
	}
	return(m_original->UpdateTexture(pSourceTexture, pDestinationTexture));
}

HRESULT texIDirect3DDevice9::GetRenderTargetData(IDirect3DSurface9* pRenderTarget, IDirect3DSurface9* pDestSurface)
{
	return(m_original->GetRenderTargetData(pRenderTarget, pDestSurface));
}

HRESULT texIDirect3DDevice9::GetFrontBufferData(UINT iSwapChain, IDirect3DSurface9* pDestSurface)
{
	return(m_original->GetFrontBufferData(iSwapChain, pDestSurface));
}

HRESULT texIDirect3DDevice9::StretchRect(IDirect3DSurface9* pSourceSurface, CONST RECT* pSourceRect, IDirect3DSurface9* pDestSurface, CONST RECT* pDestRect, D3DTEXTUREFILTERTYPE Filter)
{
	return(m_original->StretchRect(pSourceSurface, pSourceRect, pDestSurface, pDestRect, Filter));
}

HRESULT texIDirect3DDevice9::ColorFill(IDirect3DSurface9* pSurface, CONST RECT* pRect, D3DCOLOR color)
{
	return(m_original->ColorFill(pSurface, pRect, color));
}

HRESULT texIDirect3DDevice9::CreateOffscreenPlainSurface(UINT Width, UINT Height, D3DFORMAT Format, D3DPOOL Pool, IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle)
{
	return(m_original->CreateOffscreenPlainSurface(Width, Height, Format, Pool, ppSurface, pSharedHandle));
}

HRESULT texIDirect3DDevice9::SetRenderTarget(DWORD RenderTargetIndex, IDirect3DSurface9* pRenderTarget)
{
	{
		IDirect3DSurface9 *back_buffer;
		NormalRendering = false;
		for (int i = 0; !NormalRendering && i<BackBufferCount; i++)
		{
			m_original->GetBackBuffer(0, i, D3DBACKBUFFER_TYPE_MONO, &back_buffer);
			if (back_buffer == pRenderTarget) NormalRendering = true;
			back_buffer->Release();
		}
	}
	return (m_original->SetRenderTarget(RenderTargetIndex, pRenderTarget));
}

HRESULT texIDirect3DDevice9::GetRenderTarget(DWORD RenderTargetIndex, IDirect3DSurface9** ppRenderTarget)
{
	return (m_original->GetRenderTarget(RenderTargetIndex, ppRenderTarget));
}

HRESULT texIDirect3DDevice9::SetDepthStencilSurface(IDirect3DSurface9* pNewZStencil)
{
	return(m_original->SetDepthStencilSurface(pNewZStencil));
}

HRESULT texIDirect3DDevice9::GetDepthStencilSurface(IDirect3DSurface9** ppZStencilSurface)
{
	return(m_original->GetDepthStencilSurface(ppZStencilSurface));
}

HRESULT texIDirect3DDevice9::BeginScene(void)
{
	if (uMod_Client != NULL)
	{
		if (LastCreatedTexture != NULL) // add the last created texture
		{
			uMod_Client->AddTexture(LastCreatedTexture);
		}
		if (LastCreatedVolumeTexture != NULL) // add the last created texture
		{
			uMod_Client->AddTexture(LastCreatedVolumeTexture);
		}
		if (LastCreatedCubeTexture != NULL) // add the last created texture
		{
			uMod_Client->AddTexture(LastCreatedCubeTexture);
		}
		uMod_Client->MergeUpdate(); // merge an update, if present

		if (uMod_Client->Bool_CheckAgainNonAdded) uMod_Client->CheckAgainNonAdded();

		if (uMod_Client->BoolSaveSingleTexture)
		{
			if (CreateSingleTexture() == 0)
			{
				if (uMod_Client->KeyBack>0 && (GetAsyncKeyState(uMod_Client->KeyBack) & 1)) //ask for the status of the back key
				{
					UnswitchTextures(SingleTexture); // can be called, even if texture is not switched
					UnswitchTextures(SingleVolumeTexture); // can be called, even if texture is not switched
					UnswitchTextures(SingleCubeTexture); // can be called, even if texture is not switched

					if (CounterSaveSingleTexture<-10) { CounterSaveSingleTexture = 0; SingleTextureMod = 0; } //first initialization of the counter
					else if (--CounterSaveSingleTexture<0)
					{
						if (--SingleTextureMod<0) SingleTextureMod = 2;
						switch (SingleTextureMod)
						{
						case 0:
							CounterSaveSingleTexture = uMod_Client->OriginalTextures.GetNumber() - 1;
							break;
						case 1:
							CounterSaveSingleTexture = uMod_Client->OriginalVolumeTextures.GetNumber() - 1;
							break;
						case 2:
							CounterSaveSingleTexture = uMod_Client->OriginalCubeTextures.GetNumber() - 1;
							break;
						}
					}

					if (CounterSaveSingleTexture >= 0)
					{
						switch (SingleTextureMod)
						{
						case 0:
							SwitchTextures(SingleTexture, uMod_Client->OriginalTextures[CounterSaveSingleTexture]);
							SingleTexture->CRC64 = uMod_Client->OriginalTextures[CounterSaveSingleTexture]->CRC64; //set the CRC64 for the display
							break;
						case 1:
							SwitchTextures(SingleVolumeTexture, uMod_Client->OriginalVolumeTextures[CounterSaveSingleTexture]);
							SingleVolumeTexture->CRC64 = uMod_Client->OriginalVolumeTextures[CounterSaveSingleTexture]->CRC64; //set the CRC64 for the display
							break;
						case 2:
							SwitchTextures(SingleCubeTexture, uMod_Client->OriginalCubeTextures[CounterSaveSingleTexture]);
							SingleCubeTexture->CRC64 = uMod_Client->OriginalCubeTextures[CounterSaveSingleTexture]->CRC64; //set the CRC64 for the display
							break;
						}
					}
				}

				if (uMod_Client->KeySave>0 && (GetAsyncKeyState(uMod_Client->KeySave) & 1)) //ask for the status of the save key
				{
					switch (SingleTextureMod)
					{
					case 0:
						uMod_Client->SaveTexture(SingleTexture); //after switching the SingleTexture holds the pointer to the original texture object
						break;
					case 1:
						uMod_Client->SaveTexture(SingleVolumeTexture); //after switching the SingleTexture holds the pointer to the original texture object
						break;
					case 2:
						uMod_Client->SaveTexture(SingleCubeTexture); //after switching the SingleTexture holds the pointer to the original texture object
						break;
					}
				}

				if (uMod_Client->KeyNext>0 && (GetAsyncKeyState(uMod_Client->KeyNext) & 1)) //ask for the status of the next key
				{
					UnswitchTextures(SingleTexture); // can be called, even if texture is not switched
					UnswitchTextures(SingleVolumeTexture); // can be called, even if texture is not switched
					UnswitchTextures(SingleCubeTexture); // can be called, even if texture is not switched

					if (CounterSaveSingleTexture<-10) { CounterSaveSingleTexture = 0; SingleTextureMod = 0; } //first initialization of the counter
					else
					{
						int num = 0;
						switch (SingleTextureMod)
						{
						case 0:
							num = uMod_Client->OriginalTextures.GetNumber();
							break;
						case 1:
							num = uMod_Client->OriginalVolumeTextures.GetNumber();
							break;
						case 2:
							num = uMod_Client->OriginalCubeTextures.GetNumber();
							break;
						}
						if (++CounterSaveSingleTexture >= num)
						{
							if (++SingleTextureMod>2) SingleTextureMod = 0;
							switch (SingleTextureMod)
							{
							case 0:
								CounterSaveSingleTexture = uMod_Client->OriginalTextures.GetNumber()>0 ? 0 : -1;
								break;
							case 1:
								CounterSaveSingleTexture = uMod_Client->OriginalVolumeTextures.GetNumber()>0 ? 0 : -1;
								break;
							case 2:
								CounterSaveSingleTexture = uMod_Client->OriginalCubeTextures.GetNumber()>0 ? 0 : -1;
								break;
							}
						}
					}

					if (CounterSaveSingleTexture >= 0)
					{
						switch (SingleTextureMod)
						{
						case 0:
							SwitchTextures(SingleTexture, uMod_Client->OriginalTextures[CounterSaveSingleTexture]);
							SingleTexture->CRC64 = uMod_Client->OriginalTextures[CounterSaveSingleTexture]->CRC64; //set the CRC64 for the display
							break;
						case 1:
							SwitchTextures(SingleVolumeTexture, uMod_Client->OriginalVolumeTextures[CounterSaveSingleTexture]);
							SingleVolumeTexture->CRC64 = uMod_Client->OriginalVolumeTextures[CounterSaveSingleTexture]->CRC64; //set the CRC64 for the display
							break;
						case 2:
							SwitchTextures(SingleCubeTexture, uMod_Client->OriginalCubeTextures[CounterSaveSingleTexture]);
							SingleCubeTexture->CRC64 = uMod_Client->OriginalCubeTextures[CounterSaveSingleTexture]->CRC64; //set the CRC64 for the display
							break;
						}
					}
				}
			}
		}
	}

	return (m_original->BeginScene());
}


HRESULT texIDirect3DDevice9::EndScene(void)
{
	if (uMod_Client != NULL && NormalRendering && uMod_Client->BoolSaveSingleTexture && SingleTexture != NULL && SingleVolumeTexture != NULL && SingleCubeTexture != NULL)
	{
		D3DVIEWPORT9 viewport;
		GetViewport(&viewport);

		if (SingleTexture->CrossRef_D3Dtex != NULL && uMod_Client->BoolShowSingleTexture)
		{
			if (pSprite == NULL)
			{
				if (D3D_OK != D3DXCreateSprite(m_original, &pSprite)) pSprite = NULL;
			}
			if (pSprite != NULL)
			{
				D3DSURFACE_DESC desc;
				SingleTexture->m_D3Dtex->GetLevelDesc(0, &desc);
				D3DXMATRIX scale;
				float factorx, factory;

				pSprite->Begin(D3DXSPRITE_ALPHABLEND);

				factorx = 0.5f * ((float)viewport.Width) / ((float)desc.Width);
				factory = 0.5f * ((float)viewport.Height) / ((float)desc.Height);

				if (factory<factorx) factorx = factory;
				if (factorx<1.0)
				{
					D3DXMATRIX scale;
					D3DXMatrixScaling(&scale, factorx, factorx, 1.0f);
					pSprite->SetTransform(&scale);
				}
				pSprite->Draw(SingleTexture->m_D3Dtex, NULL, NULL, NULL, 0xFFFFFFFF);
				pSprite->End();
			}

		}
		if (uMod_Client->BoolShowTextureString)
		{
			if (OSD_Font == NULL) // create the font
			{
				if (D3D_OK != D3DXCreateFontA(m_original, 18, 5, FW_BOLD, 0, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("Courier"), &OSD_Font))
					OSD_Font = NULL;
			}

			if (OSD_Font != NULL)
			{
				char buffer[256];
				buffer[0] = 0;
				switch (SingleTextureMod)
				{
				case 0:
				{
					if (SingleTexture->CrossRef_D3Dtex != NULL) sprintf_s(buffer, 256, "normal texture: %4d (1..%d): %#llX", CounterSaveSingleTexture + 1, uMod_Client->OriginalTextures.GetNumber(), SingleTexture->CRC64);
					else
					{
						if (uMod_Client->OriginalTextures.GetNumber()>0) sprintf_s(buffer, 256, "normal texture: %4d (1..%d): nothing selected ", CounterSaveSingleTexture + 1, uMod_Client->OriginalTextures.GetNumber());
						else sprintf_s(buffer, 100, "normal texture: nothing loaded");
					}
					break;
				}
				case 1:
				{
					if (SingleVolumeTexture->CrossRef_D3Dtex != NULL) sprintf_s(buffer, 256, "volume texture: %4d (1..%d): %#llX", CounterSaveSingleTexture + 1, uMod_Client->OriginalVolumeTextures.GetNumber(), SingleVolumeTexture->CRC64);
					else
					{
						if (uMod_Client->OriginalVolumeTextures.GetNumber()>0) sprintf_s(buffer, 256, "volume texture: %4d (1..%d): nothing selected", CounterSaveSingleTexture + 1, uMod_Client->OriginalVolumeTextures.GetNumber());
						else sprintf_s(buffer, 100, "volume texture: nothing loaded");
					}
					break;
				}
				case 2:
				{
					if (SingleCubeTexture->CrossRef_D3Dtex != NULL) sprintf_s(buffer, 256, "cube texture: %4d (1..%d): %#llX", CounterSaveSingleTexture + 1, uMod_Client->OriginalCubeTextures.GetNumber(), SingleCubeTexture->CRC64);
					else
					{
						if (uMod_Client->OriginalCubeTextures.GetNumber()>0) sprintf_s(buffer, 256, "cube texture: %4d (1..%d): nothing selected", CounterSaveSingleTexture + 1, uMod_Client->OriginalCubeTextures.GetNumber());
						else sprintf_s(buffer, 100, "cube   texture: nothing loaded");
					}
					break;
				}
				}

				RECT rct;
				rct.left = viewport.X + 10;
				rct.right = 0; //size of box is calculated automatically (DT_NOCLIP)
				rct.top = viewport.Y + 10;
				rct.bottom = 0; //size of box is calculated automatically (DT_NOCLIP)
				OSD_Font->DrawTextA(NULL, buffer, -1, &rct, DT_NOCLIP, uMod_Client->FontColour);
			}
		}
	}
	return(m_original->EndScene());
}

HRESULT texIDirect3DDevice9::Clear(DWORD Count, CONST D3DRECT* pRects, DWORD Flags, D3DCOLOR Color, float Z, DWORD Stencil)
{
	return(m_original->Clear(Count, pRects, Flags, Color, Z, Stencil));
}

HRESULT texIDirect3DDevice9::SetTransform(D3DTRANSFORMSTATETYPE State, CONST D3DMATRIX* pMatrix)
{
	return(m_original->SetTransform(State, pMatrix));
}

HRESULT texIDirect3DDevice9::GetTransform(D3DTRANSFORMSTATETYPE State, D3DMATRIX* pMatrix)
{
	return(m_original->GetTransform(State, pMatrix));
}

HRESULT texIDirect3DDevice9::MultiplyTransform(D3DTRANSFORMSTATETYPE State, CONST D3DMATRIX* pMatrix)
{
	return(m_original->MultiplyTransform(State, pMatrix));
}

HRESULT texIDirect3DDevice9::SetViewport(CONST D3DVIEWPORT9* pViewport)
{
	return(m_original->SetViewport(pViewport));
}

HRESULT texIDirect3DDevice9::GetViewport(D3DVIEWPORT9* pViewport)
{
	return(m_original->GetViewport(pViewport));
}

HRESULT texIDirect3DDevice9::SetMaterial(CONST D3DMATERIAL9* pMaterial)
{
	return(m_original->SetMaterial(pMaterial));
}

HRESULT texIDirect3DDevice9::GetMaterial(D3DMATERIAL9* pMaterial)
{
	return(m_original->GetMaterial(pMaterial));
}

HRESULT texIDirect3DDevice9::SetLight(DWORD Index, CONST D3DLIGHT9* pLight)
{
	return(m_original->SetLight(Index, pLight));
}

HRESULT texIDirect3DDevice9::GetLight(DWORD Index, D3DLIGHT9* pLight)
{
	return(m_original->GetLight(Index, pLight));
}

HRESULT texIDirect3DDevice9::LightEnable(DWORD Index, BOOL Enable)
{
	return(m_original->LightEnable(Index, Enable));
}

HRESULT texIDirect3DDevice9::GetLightEnable(DWORD Index, BOOL* pEnable)
{
	return(m_original->GetLightEnable(Index, pEnable));
}

HRESULT texIDirect3DDevice9::SetClipPlane(DWORD Index, CONST float* pPlane)
{
	return(m_original->SetClipPlane(Index, pPlane));
}

HRESULT texIDirect3DDevice9::GetClipPlane(DWORD Index, float* pPlane)
{
	return(m_original->GetClipPlane(Index, pPlane));
}

HRESULT texIDirect3DDevice9::SetRenderState(D3DRENDERSTATETYPE State, DWORD Value)
{
	return(m_original->SetRenderState(State, Value));
}

HRESULT texIDirect3DDevice9::GetRenderState(D3DRENDERSTATETYPE State, DWORD* pValue)
{
	return(m_original->GetRenderState(State, pValue));
}

HRESULT texIDirect3DDevice9::CreateStateBlock(D3DSTATEBLOCKTYPE Type, IDirect3DStateBlock9** ppSB)
{
	return(m_original->CreateStateBlock(Type, ppSB));
}

HRESULT texIDirect3DDevice9::BeginStateBlock(void)
{
	return(m_original->BeginStateBlock());
}

HRESULT texIDirect3DDevice9::EndStateBlock(IDirect3DStateBlock9** ppSB)
{
	return(m_original->EndStateBlock(ppSB));
}

HRESULT texIDirect3DDevice9::SetClipStatus(CONST D3DCLIPSTATUS9* pClipStatus)
{
	return(m_original->SetClipStatus(pClipStatus));
}

HRESULT texIDirect3DDevice9::GetClipStatus(D3DCLIPSTATUS9* pClipStatus)
{
	return(m_original->GetClipStatus(pClipStatus));
}

HRESULT texIDirect3DDevice9::GetTexture(DWORD Stage, IDirect3DBaseTexture9** ppTexture)
{
	return(m_original->GetTexture(Stage, ppTexture));
}

HRESULT texIDirect3DDevice9::SetTexture(DWORD Stage, IDirect3DBaseTexture9* pTexture)
{
	// we must pass the real texture objects
	IDirect3DBaseTexture9* cpy;
	if (pTexture != NULL)
	{
		long int ret = pTexture->QueryInterface(IID_IDirect3D9, (void**)&cpy);
		switch (ret)
		{
		case 0x01000000L:
			pTexture = ((uMod_IDirect3DTexture9*)(pTexture))->m_D3Dtex; break;
		case 0x01000001L:
			pTexture = ((uMod_IDirect3DVolumeTexture9*)(pTexture))->m_D3Dtex; break;
		case 0x01000002L:
			pTexture = ((uMod_IDirect3DCubeTexture9*)(pTexture))->m_D3Dtex; break;
		default:
			break; // this is no fake texture and QueryInterface failed, because IDirect3DBaseTexture9 object cannot be a IDirect3D9 object ;)
		}
	}
	return (m_original->SetTexture(Stage, pTexture));
}

HRESULT texIDirect3DDevice9::GetTextureStageState(DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD* pValue)
{
	return(m_original->GetTextureStageState(Stage, Type, pValue));
}

HRESULT texIDirect3DDevice9::SetTextureStageState(DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD Value)
{
	return(m_original->SetTextureStageState(Stage, Type, Value));
}

HRESULT texIDirect3DDevice9::GetSamplerState(DWORD Sampler, D3DSAMPLERSTATETYPE Type, DWORD* pValue)
{
	return(m_original->GetSamplerState(Sampler, Type, pValue));
}

HRESULT texIDirect3DDevice9::SetSamplerState(DWORD Sampler, D3DSAMPLERSTATETYPE Type, DWORD Value)
{
	return(m_original->SetSamplerState(Sampler, Type, Value));
}

HRESULT texIDirect3DDevice9::ValidateDevice(DWORD* pNumPasses)
{
	return(m_original->ValidateDevice(pNumPasses));
}

HRESULT texIDirect3DDevice9::SetPaletteEntries(UINT PaletteNumber, CONST PALETTEENTRY* pEntries)
{
	return(m_original->SetPaletteEntries(PaletteNumber, pEntries));
}

HRESULT texIDirect3DDevice9::GetPaletteEntries(UINT PaletteNumber, PALETTEENTRY* pEntries)
{
	return(m_original->GetPaletteEntries(PaletteNumber, pEntries));
}

HRESULT texIDirect3DDevice9::SetCurrentTexturePalette(UINT PaletteNumber)
{
	return(m_original->SetCurrentTexturePalette(PaletteNumber));
}

HRESULT texIDirect3DDevice9::GetCurrentTexturePalette(UINT *PaletteNumber)
{
	return(m_original->GetCurrentTexturePalette(PaletteNumber));
}

HRESULT texIDirect3DDevice9::SetScissorRect(CONST RECT* pRect)
{
	return(m_original->SetScissorRect(pRect));
}

HRESULT texIDirect3DDevice9::GetScissorRect(RECT* pRect)
{
	return(m_original->GetScissorRect(pRect));
}

HRESULT texIDirect3DDevice9::SetSoftwareVertexProcessing(BOOL bSoftware)
{
	return(m_original->SetSoftwareVertexProcessing(bSoftware));
}

BOOL texIDirect3DDevice9::GetSoftwareVertexProcessing(void)
{
	return(m_original->GetSoftwareVertexProcessing());
}

HRESULT texIDirect3DDevice9::SetNPatchMode(float nSegments)
{
	return(m_original->SetNPatchMode(nSegments));
}

float	texIDirect3DDevice9::GetNPatchMode(void)
{
	return(m_original->GetNPatchMode());
}

HRESULT texIDirect3DDevice9::DrawPrimitive(D3DPRIMITIVETYPE PrimitiveType, UINT StartVertex, UINT PrimitiveCount)
{
	return(m_original->DrawPrimitive(PrimitiveType, StartVertex, PrimitiveCount));
}

HRESULT texIDirect3DDevice9::DrawIndexedPrimitive(D3DPRIMITIVETYPE PrimitiveType, INT BaseVertexIndex, UINT MinVertexIndex, UINT NumVertices, UINT startIndex, UINT primCount)
{
	return(m_original->DrawIndexedPrimitive(PrimitiveType, BaseVertexIndex, MinVertexIndex, NumVertices, startIndex, primCount));
}

HRESULT texIDirect3DDevice9::DrawPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType, UINT PrimitiveCount, CONST void* pVertexStreamZeroData, UINT VertexStreamZeroStride)
{
	return(m_original->DrawPrimitiveUP(PrimitiveType, PrimitiveCount, pVertexStreamZeroData, VertexStreamZeroStride));
}

HRESULT texIDirect3DDevice9::DrawIndexedPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType, UINT MinVertexIndex, UINT NumVertices, UINT PrimitiveCount, CONST void* pIndexData, D3DFORMAT IndexDataFormat, CONST void* pVertexStreamZeroData, UINT VertexStreamZeroStride)
{
	return(m_original->DrawIndexedPrimitiveUP(PrimitiveType, MinVertexIndex, NumVertices, PrimitiveCount, pIndexData, IndexDataFormat, pVertexStreamZeroData, VertexStreamZeroStride));
}

HRESULT texIDirect3DDevice9::ProcessVertices(UINT SrcStartIndex, UINT DestIndex, UINT VertexCount, IDirect3DVertexBuffer9* pDestBuffer, IDirect3DVertexDeclaration9* pVertexDecl, DWORD Flags)
{
	return(m_original->ProcessVertices(SrcStartIndex, DestIndex, VertexCount, pDestBuffer, pVertexDecl, Flags));
}

HRESULT texIDirect3DDevice9::CreateVertexDeclaration(CONST D3DVERTEXELEMENT9* pVertexElements, IDirect3DVertexDeclaration9** ppDecl)
{
	return(m_original->CreateVertexDeclaration(pVertexElements, ppDecl));
}

HRESULT texIDirect3DDevice9::SetVertexDeclaration(IDirect3DVertexDeclaration9* pDecl)
{
	return(m_original->SetVertexDeclaration(pDecl));
}

HRESULT texIDirect3DDevice9::GetVertexDeclaration(IDirect3DVertexDeclaration9** ppDecl)
{
	return(m_original->GetVertexDeclaration(ppDecl));
}

HRESULT texIDirect3DDevice9::SetFVF(DWORD FVF)
{
	return(m_original->SetFVF(FVF));
}

HRESULT texIDirect3DDevice9::GetFVF(DWORD* pFVF)
{
	return(m_original->GetFVF(pFVF));
}

HRESULT texIDirect3DDevice9::CreateVertexShader(CONST DWORD* pFunction, IDirect3DVertexShader9** ppShader)
{
	return(m_original->CreateVertexShader(pFunction, ppShader));
}

HRESULT texIDirect3DDevice9::SetVertexShader(IDirect3DVertexShader9* pShader)
{
	return(m_original->SetVertexShader(pShader));
}

HRESULT texIDirect3DDevice9::GetVertexShader(IDirect3DVertexShader9** ppShader)
{
	return(m_original->GetVertexShader(ppShader));
}

HRESULT texIDirect3DDevice9::SetVertexShaderConstantF(UINT StartRegister, CONST float* pConstantData, UINT Vector4fCount)
{
	return(m_original->SetVertexShaderConstantF(StartRegister, pConstantData, Vector4fCount));
}

HRESULT texIDirect3DDevice9::GetVertexShaderConstantF(UINT StartRegister, float* pConstantData, UINT Vector4fCount)
{
	return(m_original->GetVertexShaderConstantF(StartRegister, pConstantData, Vector4fCount));
}

HRESULT texIDirect3DDevice9::SetVertexShaderConstantI(UINT StartRegister, CONST int* pConstantData, UINT Vector4iCount)
{
	return(m_original->SetVertexShaderConstantI(StartRegister, pConstantData, Vector4iCount));
}

HRESULT texIDirect3DDevice9::GetVertexShaderConstantI(UINT StartRegister, int* pConstantData, UINT Vector4iCount)
{
	return(m_original->GetVertexShaderConstantI(StartRegister, pConstantData, Vector4iCount));
}

HRESULT texIDirect3DDevice9::SetVertexShaderConstantB(UINT StartRegister, CONST BOOL* pConstantData, UINT  BoolCount)
{
	return(m_original->SetVertexShaderConstantB(StartRegister, pConstantData, BoolCount));
}

HRESULT texIDirect3DDevice9::GetVertexShaderConstantB(UINT StartRegister, BOOL* pConstantData, UINT BoolCount)
{
	return(m_original->GetVertexShaderConstantB(StartRegister, pConstantData, BoolCount));
}

HRESULT texIDirect3DDevice9::SetStreamSource(UINT StreamNumber, IDirect3DVertexBuffer9* pStreamData, UINT OffsetInBytes, UINT Stride)
{
	return(m_original->SetStreamSource(StreamNumber, pStreamData, OffsetInBytes, Stride));
}

HRESULT texIDirect3DDevice9::GetStreamSource(UINT StreamNumber, IDirect3DVertexBuffer9** ppStreamData, UINT* OffsetInBytes, UINT* pStride)
{
	return(m_original->GetStreamSource(StreamNumber, ppStreamData, OffsetInBytes, pStride));
}

HRESULT texIDirect3DDevice9::SetStreamSourceFreq(UINT StreamNumber, UINT Divider)
{
	return(m_original->SetStreamSourceFreq(StreamNumber, Divider));
}

HRESULT texIDirect3DDevice9::GetStreamSourceFreq(UINT StreamNumber, UINT* Divider)
{
	return(m_original->GetStreamSourceFreq(StreamNumber, Divider));
}

HRESULT texIDirect3DDevice9::SetIndices(IDirect3DIndexBuffer9* pIndexData)
{
	return(m_original->SetIndices(pIndexData));
}

HRESULT texIDirect3DDevice9::GetIndices(IDirect3DIndexBuffer9** ppIndexData)
{
	return(m_original->GetIndices(ppIndexData));
}

HRESULT texIDirect3DDevice9::CreatePixelShader(CONST DWORD* pFunction, IDirect3DPixelShader9** ppShader)
{
	return(m_original->CreatePixelShader(pFunction, ppShader));
}

HRESULT texIDirect3DDevice9::SetPixelShader(IDirect3DPixelShader9* pShader)
{
	return(m_original->SetPixelShader(pShader));
}

HRESULT texIDirect3DDevice9::GetPixelShader(IDirect3DPixelShader9** ppShader)
{
	return(m_original->GetPixelShader(ppShader));
}

HRESULT texIDirect3DDevice9::SetPixelShaderConstantF(UINT StartRegister, CONST float* pConstantData, UINT Vector4fCount)
{
	return(m_original->SetPixelShaderConstantF(StartRegister, pConstantData, Vector4fCount));
}

HRESULT texIDirect3DDevice9::GetPixelShaderConstantF(UINT StartRegister, float* pConstantData, UINT Vector4fCount)
{
	return(m_original->GetPixelShaderConstantF(StartRegister, pConstantData, Vector4fCount));
}

HRESULT texIDirect3DDevice9::SetPixelShaderConstantI(UINT StartRegister, CONST int* pConstantData, UINT Vector4iCount)
{
	return(m_original->SetPixelShaderConstantI(StartRegister, pConstantData, Vector4iCount));
}

HRESULT texIDirect3DDevice9::GetPixelShaderConstantI(UINT StartRegister, int* pConstantData, UINT Vector4iCount)
{
	return(m_original->GetPixelShaderConstantI(StartRegister, pConstantData, Vector4iCount));
}

HRESULT texIDirect3DDevice9::SetPixelShaderConstantB(UINT StartRegister, CONST BOOL* pConstantData, UINT  BoolCount)
{
	return(m_original->SetPixelShaderConstantB(StartRegister, pConstantData, BoolCount));
}

HRESULT texIDirect3DDevice9::GetPixelShaderConstantB(UINT StartRegister, BOOL* pConstantData, UINT BoolCount)
{
	return(m_original->GetPixelShaderConstantB(StartRegister, pConstantData, BoolCount));
}

HRESULT texIDirect3DDevice9::DrawRectPatch(UINT Handle, CONST float* pNumSegs, CONST D3DRECTPATCH_INFO* pRectPatchInfo)
{
	return(m_original->DrawRectPatch(Handle, pNumSegs, pRectPatchInfo));
}

HRESULT texIDirect3DDevice9::DrawTriPatch(UINT Handle, CONST float* pNumSegs, CONST D3DTRIPATCH_INFO* pTriPatchInfo)
{
	return(m_original->DrawTriPatch(Handle, pNumSegs, pTriPatchInfo));
}

HRESULT texIDirect3DDevice9::DeletePatch(UINT Handle)
{
	return(m_original->DeletePatch(Handle));
}

HRESULT texIDirect3DDevice9::CreateQuery(D3DQUERYTYPE Type, IDirect3DQuery9** ppQuery)
{
	return(m_original->CreateQuery(Type, ppQuery));
}