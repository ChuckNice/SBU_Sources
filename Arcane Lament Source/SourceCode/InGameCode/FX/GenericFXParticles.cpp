#include "luabind\luabind.hpp"
#include "lua.h"
#include <SSSF_SourceCode\game\Game.h>
#include <SSSF_SourceCode\gsm\state\GameStateManager.h>
#include <SSSF_SourceCode\gsm\fx\FXSystem.h>
#include <SSSF_SourceCode\gsm\fx\FXParticle.h>
#include <SourceCode\InGameCode\FX\GenericFXParticles.h>
#include <SourceCode\ProjectBase\GroupGameData.h>
#include <SSSF_SourceCode\graphics\GameGraphics.h>
#include <SSSF_SourceCode\graphics\TextureManager.h>
void loadExplosion(Game* game,lua_State* state);
void loadArrow(Game* game, lua_State* state);
void loadSlash(Game* game, lua_State* state);
void loadDirtExplosion(Game* game, lua_State* state);
void loadBlood(Game* game, lua_State* state);
void loadBloodPool(Game* game, lua_State* state);
void loadLightning(Game* game, lua_State* state);
void loadSparkle(Game* game, lua_State* state);

void loadParticles(Game* game,lua_State* state){
	loadExplosion(game,state);
	loadArrow(game,state);
	loadLightning(game,state);
	loadSlash(game,state);
	loadDirtExplosion(game,state);
	loadBlood(game,state);
	loadBloodPool(game,state);
	loadSparkle(game,state);
}
void loadDirtExplosion(Game* game, lua_State* state){
	FXParticleSprite* particle=game->getGSM()->getFXSys()->getSpriteParticle();
	particle->setSprite(game->getGSM()->getSpriteManager()->getSpriteType("DirtExplosionFXSprite"));
	particle->key=DirtExplosionParticleID;
	particle->a=200;
	particle->offX=particle->type->getOffsetX();
	particle->offY=particle->type->getOffsetY();
	particle->runTime=(int)(21);
	particle->currentState=L"IDLE_STATE";
	game->getGSM()->getFXSys()->addParticle(particle);
}
void loadSlash(Game* game, lua_State* state){
	FXParticleSprite* particle=game->getGSM()->getFXSys()->getSpriteParticle();
	particle->setSprite(game->getGSM()->getSpriteManager()->getSpriteType("SlashFXSprite"));
	particle->key=SlashParticleID;
	particle->a=255;
	particle->offX=particle->type->getOffsetX();
	particle->offY=particle->type->getOffsetY();
	particle->runTime=(int)(14);
	particle->currentState=L"IDLE_STATE";
	game->getGSM()->getFXSys()->addParticle(particle);
}
void loadBlood(Game* game, lua_State* state){
	FXParticleSprite* particle=game->getGSM()->getFXSys()->getSpriteParticle();
	particle->setSprite(game->getGSM()->getSpriteManager()->getSpriteType("BloodFXSprite"));
	particle->key=BloodParticleID;
	particle->a=255;
	particle->offX=particle->type->getOffsetX();
	particle->offY=particle->type->getOffsetY();
	
	//particle->sX=particle->sY=2.0f;
	particle->runTime=(int)(20);
	particle->currentState=L"IDLE_STATE";
	game->getGSM()->getFXSys()->addParticle(particle);
}
void loadSparkle(Game* game, lua_State* state){
	FXParticleSprite* particle=game->getGSM()->getFXSys()->getSpriteParticle();
	particle->setSprite(game->getGSM()->getSpriteManager()->getSpriteType("SparkleFXSprite"));
	particle->key=SparkleParticleID;
	particle->a=255;
	particle->offX=particle->type->getOffsetX();
	particle->offY=particle->type->getOffsetY();
	particle->va=-10;
	particle->vsX=particle->vsY=.05f;
	particle->vy=-1;
	particle->runTime=(int)(20);
	particle->currentState=L"IDLE_STATE";
	game->getGSM()->getFXSys()->addParticle(particle);
}

void loadBloodPool(Game* game, lua_State* state){
	FXParticleStatic* particle=game->getGSM()->getFXSys()->getStaticParticle();
	particle->imageID=luabind::object_cast<int>(luabind::globals(state)["ParticleFxImageIDs"]["bloodpool"]);
	particle->key=BloodPoolParticleID;
	particle->a=250;
	particle->va=-1.0f;
	particle->offX=-4;
	particle->offY=-2;
	particle->runTime=220;
	game->getGSM()->getFXSys()->addParticle(particle);
}
void loadLightning(Game* game, lua_State* state){
	FXParticleStatic* particle=game->getGSM()->getFXSys()->getStaticParticle();
	particle->imageID=luabind::object_cast<int>(luabind::globals(state)["ParticleFxImageIDs"]["lightning"]);
	particle->key=LightningParticleID;
	particle->a=255;
	particle->va=-5;
	particle->runTime=10;
	particle->drawFrom=1;
	particle->offY=-game->getGraphics()->getWorldTextureManager()->getTextureY(particle->imageID)*.5f;
	particle->sX=BOX2D_Scale_Factor_INV/game->getGraphics()->getWorldTextureManager()->getTextureX(particle->imageID);
	game->getGSM()->getFXSys()->addParticle(particle);
}
void loadArrow(Game* game, lua_State* state){
	FXParticleStatic* particle=game->getGSM()->getFXSys()->getStaticParticle();
	particle->imageID=luabind::object_cast<int>(luabind::globals(state)["ParticleFxImageIDs"]["arrow"]);
	particle->key=ArrowParticleID;
	particle->a=255;
	game->getGSM()->getFXSys()->addParticle(particle);
}
void loadExplosion(Game* game,lua_State* state){
	FXParticleStatic* particle=game->getGSM()->getFXSys()->getStaticParticle();
	particle->imageID=luabind::object_cast<int>(luabind::globals(state)["EXPLOSIONID"]);
	particle->key=EXPLOSION;
	particle->runTime=32;
	particle->a=255;
	particle->va=-8;
	particle->vsX=.3;
	particle->vsY=.3;
	game->getGSM()->getFXSys()->addParticle(particle);
}