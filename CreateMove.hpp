#pragma once
#include "sdk.hpp"
#include "drawmanager.hpp"

typedef void(__thiscall *create_move_fn)(void*, int, float, bool);

namespace original
{
	create_move_fn create_move = nullptr;
}

void __fastcall hooked_create_move(void* thiscall, void* _EDX, int sequence_number, float input_sample_frametime, bool active)
{
	original::create_move(thiscall, sequence_number, input_sample_frametime, active);

	if (!globals::input)
		return;

	CUserCmd* cmd = globals::input->cmds + (sequence_number % multiplayer_backup);
	if (!cmd)	
		return;

	CVerifiedUserCmd* verified_cmd = globals::input->verified_cmds + (sequence_number % multiplayer_backup);
	if (!verified_cmd)	
		return;

	CBaseEntity* local_player = (CBaseEntity*)(globals::entitylist->get_entity(globals::engine->get_local_player()));

	if (settings::misc::bhop && cmd->buttons & IN_JUMP && !(local_player->get_flags() & FL_ONGROUND))
	{
		cmd->buttons &= ~IN_JUMP;
	}

	verified_cmd->m_cmd = *cmd;
	verified_cmd->m_crc = cmd->get_checksum();
}
