#pragma once
#include "sdk.hpp"
#include "drawmanager.hpp"
#include "esp.hpp"

typedef void(__thiscall *paint_traverse_fn)(void*, unsigned int, bool, bool);

namespace original
{
	paint_traverse_fn paint_traverse = nullptr;
}

void __fastcall hkPaintTraverse(void *thisptr, void *_EDX, unsigned int panel, bool forceRepaint, bool allowForce)
{
	original::paint_traverse(thisptr, panel, forceRepaint, allowForce);
	globals::engine->get_screen_size(globals::screenweight, globals::screenheight);

	static unsigned int OverlayPopupPanel;
	if (OverlayPopupPanel == NULL)
	{
		const char* szName = globals::panel->get_name(panel);
		printf("Panel name: %s\n", szName);
		if (szName[0] == 'O' && szName[7] == 'P')
		{
			drawmanager::setup_fonts();
			OverlayPopupPanel = panel;
		}
	}

	if (OverlayPopupPanel == panel)
	{
		globals::surface->push_make_current(panel, false);
		if (settings::esp::enabled) renderesp();
		globals::panel->set_mouse_input_enabled(panel, false);

		if (IsKeyPressed(VK_INSERT))
			globals::menuopened = !globals::menuopened;
		if (globals::menuopened) {
			globals::panel->set_mouse_input_enabled(panel, true);
			drawmanager::particles->draw(255);
		}
		drawmanager::draw_string(drawmanager::fonts::watermark, 4, 4, Color::Aqua(), " Nixware - Garry's Mod");

		globals::surface->pop_make_current(panel);
	}
}
