#include "rlua.h"

#define setglobal(s, f)		    r_lua_pushcclosure((int)rL, f, 0); \
								r_lua_setfield((int)rL, -10002, s);

void set_encode_key(int rL, DWORD* key) { // string "local", decompile for decode key
	char* v9 = (char *)(rL + 12 - *(DWORD*)(rL + 12) + 36);

	*key = Inverse((DWORD)&v9[-*(DWORD*)v9], 4294967296);
}

void set_localscript(int rL) {
	r_lua_pushstring(rL, "LocalScript");
	r_instance_new(rL);
	r_tvalue localscript = *(*(r_tvalue**)(rL + RL_TOP) - 1);
	R_LUA_POP(rL, 2);
	**(r_tvalue**)(rL + RL_TOP) = localscript;
	R_API_INCR_TOP(rL);

	r_lua_setglobal(rL, "script");
}