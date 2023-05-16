#include <iostream>
//extern "C" {
//#include "include/lua.h"
//#include "include/lauxlib.h"
//#include "include/lualib.h"
//}
#include "include/lua.hpp"
#pragma comment (lib, "lua54.lib")
using namespace std;

int add_from_lua(lua_State* L)
{
	int a = lua_tonumber(L, -2);
	int b = lua_tonumber(L, -1);
	lua_pop(L, 3);
	int result = a + b;
	lua_pushnumber(L, result);
	return 1;
}

int main()
{
	const char* buff = "print \"Hello from Lua.\"\n";

	lua_State* L = luaL_newstate();
	// 루아 가상 머신 만들기
	luaL_openlibs(L);
//	luaL_loadbuffer(L, buff, strlen(buff), "line");

	luaL_loadfile(L, "dragon_ai.lua");

	// 에러 디버깅
	int error = lua_pcall(L, 0, 0, 0);
	if (error) {
		cout << "Error:" << lua_tostring(L, -1);
		lua_pop(L, 1);
	}

	// 변수 받기
	lua_getglobal(L, "pos_x");
	lua_getglobal(L, "pos_y");
	// 변수를 읽어서 스택에 푸시 (후입 선출)

	// 스택에서 가져와서 사용
	int pos_x = lua_tonumber(L, -2);
							// 스택에 있는 숫자를 꺼내라, 뒷 입력값은 위치
	int pos_y = lua_tonumber(L, -1);
	lua_pop(L, 2);

	// 루아 자체가 스택 머신임

	cout << "pos = (" << pos_x << ", " << pos_y << ")\n";

	lua_getglobal(L, "plustwo");
	lua_pushnumber(L, 100);
	lua_pcall(L, 1, 1, 0);
	int result = lua_tonumber(L, -1);
	lua_pop(L, 1);

	cout << "result = " << result << endl;

	lua_register(L, "c_add", add_from_lua);

	lua_getglobal(L, "lua_add");
	lua_pushnumber(L, 100);
	lua_pushnumber(L, 200);
	lua_pcall(L, 2, 1, 0);
	int sum = lua_tonumber(L, -1);
	lua_pop(L, 1);

	cout << "Sum = " << sum << endl;

	lua_close(L);
}