#pragma once
#include "interface.hpp"

class IPanel
{
public:
	const char* get_name(int iIndex)
	{
		typedef const char*(__thiscall* OriginalFn)(PVOID, int);
		return GetVFunc< OriginalFn >(this, 36)(this, iIndex);
	}
	void set_mouse_input_enabled(VPANEL vguiPanel, bool state)
	{
		typedef void(__thiscall* OriginalFn)(PVOID, VPANEL, bool);
		return GetVFunc< OriginalFn >(this, 32)(this, vguiPanel, state);
	}
};

namespace globals
{
	inline IPanel* panel = nullptr;
}
