#pragma once
#include "lua.hpp"
#include "ModelInfo.hpp"
#include "Netvars.hpp"
#include <optional>

constexpr auto entity = hash::fnv1a("DT_BaseEntity");

class CBaseEntity
{
public:
	inline int get_flags()
	{
		return *reinterpret_cast<int*>((DWORD)this + 0x350);
	}

	inline const char* get_classname()
	{
		void* networkable = this + 8;
		using get_client_class_t = const char**(__thiscall*)(PVOID);
		return *(method<get_client_class_t>(2, networkable)(networkable) + 2);
	}

	inline model_t* get_model()
	{
		void *pRenderable = (void*)(this + 0x4);
		typedef model_t*(__thiscall* OriginalFn)(PVOID);
		return GetVFunc<OriginalFn>(pRenderable, 9)(pRenderable);
	}

	model_t* get_model2()
	{
		return *(model_t**)((DWORD)this + 0x6C);
	}

	inline Vector& get_abs_origin()
	{
		typedef Vector& (__thiscall* OriginalFn)(void*);
		return ((OriginalFn)GetFunction(this, 10))(this);
	}

	inline Vector& get_abs_angles()
	{
		typedef Vector& (__thiscall* OriginalFn)(void*);
		return ((OriginalFn)GetFunction(this, 11))(this);
	}

	inline bool is_dormant()
	{
		void* networkable = this + 8;
		using is_dormant_t = bool(__thiscall*)(PVOID);
		return method<is_dormant_t>(8, networkable)(networkable);
	}

	inline size_t get_index()
	{
		void* networkable = this + 8;
		using get_entity_index_t = size_t(__thiscall*)(PVOID);
		return method<get_entity_index_t>(9, networkable)(networkable);
	}

	inline bool is_npc()
	{
		return !!std::memcmp(this->get_classname(), "npc", 3);
	}

	inline bool is_player()
	{
		return !!std::strstr(this->get_classname(), "player");
	}

	inline const Angle& get_angles()
	{
		using namespace hash::literals;
		auto offset = globals::netvars["DT_GMOD_Player"_fnv1a]["m_angEyeAngles[0]"_fnv1a];
		return *reinterpret_cast<Angle*>(reinterpret_cast<uintptr_t>(this) + offset);
	}

	inline const int32_t get_health()
	{
		using namespace hash::literals;
		auto offset = globals::netvars[entity]["m_iHealth"_fnv1a];
		return *reinterpret_cast<int32_t*>(reinterpret_cast<uintptr_t>(this) + offset);
	}

	inline const int32_t get_max_health()
	{
		using namespace hash::literals;
		auto offset = globals::netvars[entity]["m_iMaxHealth"_fnv1a];
		return *reinterpret_cast<int32_t*>(reinterpret_cast<uintptr_t>(this) + offset);
	}

	inline const Vector& get_pos()
	{
		using namespace hash::literals;
		auto offset = globals::netvars[entity]["m_vecOrigin"_fnv1a];
		return *reinterpret_cast<Vector*>(reinterpret_cast<uintptr_t>(this) + offset);
	}

	inline const uint32_t get_team()
	{
		using namespace hash::literals;
		auto offset = globals::netvars[entity]["m_iTeamNum"_fnv1a];
		return *reinterpret_cast<int32_t*>(reinterpret_cast<uintptr_t>(this) + offset);
	}

	inline std::optional<uint32_t> get_team_color()
	{
		auto glua = globals::lua->create_interface(lua::type::client);

		if (!glua)
			return {};

		glua->PushSpecial(lua::special::glob);
		glua->GetField(-1, "team");
		glua->GetField(-1, "GetColor");
		glua->PushNumber(this->get_team());
		glua->Call(1, 1);

		glua->PushString("r");
		glua->GetTable(-2);
		auto r = glua->GetNumber(-1);
		glua->Pop();

		glua->PushString("g");
		glua->GetTable(-2);
		auto g = glua->GetNumber(-1);
		glua->Pop();

		glua->PushString("b");
		glua->GetTable(-2);
		auto b = glua->GetNumber(-1);
		glua->Pop(4);

		return 0xFF000000 + (static_cast<uint32_t>(r)) + (static_cast<uint32_t>(g) << 8) + (static_cast<uint32_t>(b) << 16);
	}

	inline std::optional<bool> is_admin()
	{
		auto glua = globals::lua->create_interface(lua::type::client);

		if (!glua)
			return {};

		glua->PushSpecial(lua::special::glob);
		glua->GetField(-1, "Entity");
		glua->PushNumber(this->get_index());
		glua->Call(1, 1);

		glua->GetField(-1, "IsAdmin");
		glua->Push(-2);
		glua->Call(1, 1);

		bool admin = glua->GetBool();
		glua->Pop(3);

		return admin;
	}

	inline const Vector& get_oob_min()
	{
		using namespace hash::literals;
		auto offset = globals::netvars[entity]["m_vecMins"_fnv1a];
		return *reinterpret_cast<Vector*>(reinterpret_cast<uintptr_t>(this) + offset);
	}

	inline const Vector& get_oob_max()
	{
		using namespace hash::literals;
		auto offset = globals::netvars[entity]["m_vecMaxs"_fnv1a];
		return *reinterpret_cast<Vector*>(reinterpret_cast<uintptr_t>(this) + offset);
	}

	inline std::string get_team_name()
	{
		auto glua = globals::lua->create_interface(lua::type::client);

		if (!glua)
			return {};

		glua->PushSpecial(lua::special::glob);
		glua->GetField(-1, "team");
		glua->GetField(-1, "GetName");
		glua->PushNumber(this->get_team());
		glua->Call(1, 1);

		std::string team = glua->GetString();
		glua->Pop(3);

		return team;
	}

	inline std::string get_name()
	{
		auto glua = globals::lua->create_interface(lua::type::client);

		if (!glua)
			return {};

		glua->PushSpecial(lua::special::glob);
		glua->GetField(-1, "Entity");
		glua->PushNumber(this->get_index());
		glua->Call(1, 1);

		glua->GetField(-1, "Name");
		glua->Push(-2);
		glua->Call(1, 1);

		std::string name = glua->GetString();
		glua->Pop(3);

		return name;
	} 

	inline Vector get_eye_pos()
	{
		Vector vecViewOffset = *reinterpret_cast<Vector*>((DWORD)this + 0xE8);
		return get_abs_origin() + vecViewOffset;
	} 

	inline bool is_alive()
	{
		BYTE lifestate = *(BYTE*)((DWORD)this + 0x8F);
		return (lifestate == 0);
	}

	inline bool setup_bones(matrix3x4_t *pBoneToWorldOut, int nMaxBones, int boneMask, float currentTime)
	{
		void *pRenderable = (void*)(this + 0x4);
		typedef bool(__thiscall* OriginalFn)(PVOID, matrix3x4_t*, int, int, float);
		return ((OriginalFn)GetFunction(pRenderable, 16))(pRenderable, pBoneToWorldOut, nMaxBones, boneMask, currentTime);
	}
};