#pragma once
#include "sdk.hpp"

typedef void(__fastcall *scene_end_fn)(void*, void*);

namespace original
{
	scene_end_fn scene_end = nullptr;
}

void __fastcall hooked_scene_end(void* thisptr, void* edx)
{
	original::scene_end(thisptr, edx);
}