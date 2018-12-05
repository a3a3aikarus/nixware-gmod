#include <Windows.h>
#include "fmath.hpp"
#include <thread>
#include <chrono>
#include <iostream>
#include "Esp.hpp"
#include "sdk.hpp"
#include <fstream>
#include "vmthook.hpp"
#include "drawmanager.hpp"
#include "pattern.hpp"
#include "PaintTraverse.hpp"
#include "EndScene.hpp"
#include "Reset.hpp"
#include "CreateMove.hpp"
#include "SceneEnd.hpp"

vfunc_hook panel_hook;
vfunc_hook renderview_hook;
vfunc_hook d3d_hook;
vfunc_hook client_hook;

void(__cdecl* datapaths)(void) = nullptr;

void unload(void* dll)
{
	SetWindowLongPtrW(FindWindowW(L"Valve001", nullptr), GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(original::wnd_proc));

	d3d_hook.unhook_all();
	panel_hook.unhook_all();
	client_hook.unhook_all();
	renderview_hook.unhook_all();

	FreeConsole();

	ImGui_ImplDX9_Shutdown();

	FreeLibraryAndExitThread(HMODULE(dll), 0);
}

unsigned long __stdcall init(void* dll)
{
	AllocConsole();
	freopen("conin$", "r", stdin);
	freopen("conout$", "w", stdout);
	freopen("conout$", "w", stderr);

	globals::engine = interface::get<Engine>(GetModuleHandleW(L"engine.dll"), "VEngineClient");
	globals::entitylist = interface::get<EntityList>(GetModuleHandleW(L"client.dll"), "VClientEntityList");
	globals::surface = interface::get<ISurface>(GetModuleHandleW(L"vguimatsurface.dll"), "VGUI_Surface030");
	globals::panel = interface::get<IPanel>(GetModuleHandleW(L"vgui2.dll"), "VGUI_Panel009");
	globals::lua = interface::get<lua::Shared>(GetModuleHandleW(L"lua_shared.dll"), "LUASHARED");
	globals::client = interface::get<IBaseClientDLL>(GetModuleHandleW(L"client.dll"), "VClient017");
	globals::trace = interface::get<IEngineTrace>(GetModuleHandleW(L"engine.dll"), "EngineTraceClient003");
	globals::modelinfo = interface::get<IVModelInfo>(GetModuleHandleW(L"engine.dll"), "VModelInfoClient006");
	globals::renderview = interface::get<RenderView>(GetModuleHandleW(L"engine.dll"), "VEngineRenderView014");
	globals::device = **(IDirect3DDevice9***)(signature::find_signature("shaderapidx9.dll", "A1 ? ? ? ? 50 8B 08 FF 51 0C") + 1);

	printf("globals::engine: 0x%X\n", (DWORD)globals::engine);
	printf("globals::entitylist: 0x%X\n", (DWORD)globals::entitylist);
	printf("globals::panel: 0x%X\n", (DWORD)globals::panel);
	printf("globals::lua: 0x%X\n", (DWORD)globals::lua);
	printf("globals::client: 0x%X\n", (DWORD)globals::client);
	printf("globals::trace: 0x%X\n", (DWORD)globals::trace);
	printf("globals::modelinfo: 0x%X\n", (DWORD)globals::modelinfo);
	printf("globals::renderview: 0x%X\n", (DWORD)globals::renderview);
	printf("globals::pDevice: 0x%X\n", (DWORD)globals::device);
	globals::engine->get_screen_size(globals::screenweight, globals::screenheight);

	using datapack_paths_t = void(__cdecl*)();
	datapaths = reinterpret_cast<datapack_paths_t>(signature::find_signature("client.dll", "55 8B EC 8B 0D ?? ?? ?? ?? 83 EC 7C"));

	// datapaths();

	auto chl = interface::get<void>(GetModuleHandleW(L"client.dll"), "VClient");
	auto classes = method<get_all_classes_t>(8, chl)(chl);

	for (; classes; classes = classes->next)
		netvars::store(classes->table->table, classes->table);

	// PaintTraverse Hook
	if (panel_hook.setup(globals::panel)) {
		original::paint_traverse = panel_hook.get_original<paint_traverse_fn>(41);
		printf("Original PaintTraverse founded \n");
		panel_hook.hook_index(41, hkPaintTraverse);
		printf("PaintTraverse founded \n");
	}

	// EndScene & Reset Hook
	if (d3d_hook.setup(globals::device)) {
		original::end_scene = d3d_hook.get_original<end_scene_fn>(42);
		printf("Original EndScene founded \n");
		d3d_hook.hook_index(42, hooked_end_scene);
		printf("EndScene hooked \n");

		original::reset = d3d_hook.get_original<reset_fn>(16);
		printf("Original Reset founded \n");
		d3d_hook.hook_index(16, hooked_reset);
		printf("Reset hooked \n");
	}

	// SceneEnd hook
	if (renderview_hook.setup(globals::renderview)) {
		original::scene_end = renderview_hook.get_original<scene_end_fn>(9);
		printf("Original SceneEnd founded \n");
		renderview_hook.hook_index(9, hooked_scene_end);
		printf("SceneEnd hooked \n");
	}

	// CreateMove Hook
	if (client_hook.setup(globals::client)) {
		original::create_move = client_hook.get_original<create_move_fn>(21);
		DWORD dw_input_pointer = signature::find_pattern((DWORD)original::create_move, 0x100, (byte*)"\x8B\x0D", "xx");
		if (dw_input_pointer != NULL)
		{
			dw_input_pointer += 0x2;
			globals::input = **(CInput***)dw_input_pointer;
			printf("globals::input: 0x%X\n", (DWORD)globals::input);

		}
		client_hook.hook_index(21, hooked_create_move);
	}

	drawmanager::particles = new particle_network(globals::screenweight, globals::screenheight, 60);

	for (; !(GetAsyncKeyState(VK_HOME) & 1); std::this_thread::sleep_for(std::chrono::milliseconds(25)));

	unload(dll);
}

int __stdcall dllmain(HINSTANCE dll, DWORD reason, LPVOID)
{
	DisableThreadLibraryCalls(dll);
	switch (reason)
	{
	case DLL_PROCESS_ATTACH:	
		CreateThread(nullptr, 0, init, dll, 0, nullptr);
		break;
	case DLL_PROCESS_DETACH:	
		unload(dll);
		break;	
	}

	return 1;
}
