#pragma once
#include "ISurface.hpp"
#include "particles.hpp"

namespace drawmanager
{
	particle_network* particles = nullptr;

	namespace fonts
	{
		DWORD watermark;
		DWORD esp;
	}

	void setup_fonts()
	{
		drawmanager::fonts::watermark = globals::surface->create_font();
		drawmanager::fonts::esp = globals::surface->create_font();

		globals::surface->set_font_glyph_set(drawmanager::fonts::esp, ("Calibri"), 14, 150, 0, 0, FONTFLAG_OUTLINE /*| FONTFLAG_ANTIALIAS*/);
		globals::surface->set_font_glyph_set(drawmanager::fonts::watermark, ("Arial"), 16, 800, 0, 0, FONTFLAG_OUTLINE /*| FONTFLAG_ANTIALIAS*/);
	}

	void DrawLine(int x0, int y0, int x1, int y1, Color color)
	{
		globals::surface->draw_set_color(color);
		globals::surface->draw_line(x0, y0, x1, y1);
	}

	void draw_rect_filled(int x0, int y0, int x1, int y1, Color color)
	{
		globals::surface->draw_set_color(color);
		globals::surface->draw_filled_rect(x0, y0, x1, y1);
	}

	void fill_rgba(int x, int y, int w, int h, Color c)
	{
		globals::surface->draw_set_color(c);
		globals::surface->draw_filled_rect(x, y, x + w, y + h);
	}

	void draw_rect(int x0, int y0, int x1, int y1, Color color)
	{
		globals::surface->draw_set_color(color);
		globals::surface->draw_outlined_rect(x0, y0, x1, y1);
	}

	void draw_outlined_rect(int x0, int y0, int x1, int y1, Color color)
	{
		globals::surface->draw_set_color(color);
		globals::surface->draw_outlined_rect(x0, y0, x1, y1);
		globals::surface->draw_set_color(Color(0, 0, 0, 255));
		globals::surface->draw_outlined_rect(x0 - 1, y0 - 1, x1 + 1, y1 + 1);
		globals::surface->draw_outlined_rect(x0 + 1, y0 + 1, x1 - 1, y1 - 1);
	}

	void draw_string(DWORD font, int x, int y, Color col, const char *pszText, ...)
	{
		if (pszText == NULL)
			return;

		va_list va_alist;
		char szBuffer[1024] = { '\0' };
		wchar_t szString[1024] = { '\0' };

		va_start(va_alist, pszText);
		vsprintf_s(szBuffer, pszText, va_alist);
		va_end(va_alist);

		wsprintfW(szString, L"%S", szBuffer);
		globals::surface->draw_set_text_pos(x, y);
		globals::surface->draw_set_text_font(font);
		globals::surface->draw_set_text_color(col.r(), col.g(), col.b(), col.a());
		globals::surface->draw_print_text(szString, wcslen(szString));
	}
}
