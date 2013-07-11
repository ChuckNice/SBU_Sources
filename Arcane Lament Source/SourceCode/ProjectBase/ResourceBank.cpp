#include <unordered_map>
#include "stdafx.h"
#include "luabind\luabind.hpp"
#include "lua.h"
	
std::unordered_map<string,int> resourceBank;
	void ResourceBankaddResource(string s,int i){
		resourceBank[s]=i;
	}
	int ResourceBankgetResource(string &s){
		if(resourceBank.find(s)==resourceBank.end())return -1;
		return resourceBank[s];
	}
	void ResourceBankRegisterLua(lua_State* l){
		using namespace luabind;
		module(l)[
			def("addResource",&ResourceBankaddResource)
		];
	}