#pragma once
#include <unordered_map>
struct lua_State;
void ResourceBankaddResource(string s,int i);
int ResourceBankgetResource(string &s);
void ResourceBankRegisterLua(lua_State* l);