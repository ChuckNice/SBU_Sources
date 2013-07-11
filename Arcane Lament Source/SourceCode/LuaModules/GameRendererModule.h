#pragma once
#include "stdafx.h"
#include <luabind\luabind.hpp>
#include "lua.h"
#include "SSSF_SourceCode\gsm\lualoader\LuaModule.h"

//RENDERERS
#include <SourceCode\InGameCode\Renderers\InGameRenderer.h>

//This class handles the custom renderers usually for the different GUI screens.
//Note that each GUI screen also calls a "update()" function that can be customized 
//in a lua file. This manages 
class GameRendererModule:public LuaModule{
public:
	static InGameRenderer *renderer;
	//static SpellSelectionRenderer *renderer_SpellSelection;
	static void setRenderer(ScreenGUI* gui){
		gui->setOverlayRenderer(renderer);
	}

	void registerLua(lua_State* state){
		using namespace luabind;
		module(state)[
			def("addInGameRenderer",&GameRendererModule::setRenderer),
			class_<InGameRenderer>("InGameRenderer")
			.def("setHPBarContainer",&InGameRenderer::setHBC)
			.def("setHPBarFill",&InGameRenderer::setHBF)
			.def("setMPBarContainer",&InGameRenderer::setMBC)
			.def("setMPBarFill",&InGameRenderer::setMBF)
		];
		luabind::globals(state)["IGRenderer"]=renderer;
	}
};