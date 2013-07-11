#pragma once
#include "luabind\luabind.hpp"
#include "lua.h"
#include <SSSF_SourceCode\game\Game.h>
const wstring EXPLOSION = L"EXPLOSION";
const wstring ArrowParticleID =L"Arrow";
const wstring LightningParticleID =L"Lightning";
const wstring SlashParticleID =L"SlashParticle";
const wstring DirtExplosionParticleID =L"DirtExplosionParticle";
const wstring BloodParticleID =L"BloodParticle";
const wstring BloodPoolParticleID =L"BloodPoolParticle";
const wstring SparkleParticleID =L"SparkleParticle";
void loadParticles(Game* game,lua_State* state);