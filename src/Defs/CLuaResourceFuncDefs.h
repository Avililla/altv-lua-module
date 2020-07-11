#pragma once

#include <Main.h>

class CLuaResourceFuncDefs
{
public:
	static const char* ClassName;
	static void Init(lua_State* L);

private:
	static int IsStarted(lua_State* L);

	static int GetType(lua_State* L);
	static int GetName(lua_State* L);
	static int GetPath(lua_State* L);
	static int GetMain(lua_State* L);
};