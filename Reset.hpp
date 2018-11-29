#pragma once

using reset_fn = long(__stdcall *)(IDirect3DDevice9*, D3DPRESENT_PARAMETERS*);

namespace original
{
	reset_fn reset = nullptr;
}

long __stdcall hooked_reset(IDirect3DDevice9* device, D3DPRESENT_PARAMETERS* pPresentationParameters)
{
	ImGui_ImplDX9_InvalidateDeviceObjects();
	auto hr = original::reset(device, pPresentationParameters);

	if (hr >= 0) {
		ImGui_ImplDX9_CreateDeviceObjects();
		drawmanager::setup_fonts();
		drawmanager::particles = new particle_network(globals::screenweight, globals::screenheight, 60);
	}

	return hr;
}

