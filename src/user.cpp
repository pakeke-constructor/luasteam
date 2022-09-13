
#include "user.hpp"
#include "common.hpp"


namespace {

int ticket_callback_ref = LUA_NOREF; // for GetAuthSessionTicketResponse_t
int validate_callback_ref = LUA_NOREF; // for ValidateAuthTicketResponse_t

HAuthTicket currentTicket;


class CallbackListener {
    private:
        STEAM_CALLBACK( CallbackListener, GetAuthSessionTicketResponse, GetAuthSessionTicketResponse_t );
        STEAM_CALLBACK( CallbackListener, ValidateAuthTicketResponse, ValidateAuthTicketResponse_t);
};


void CallbackListener::GetAuthSessionTicketResponse(GetAuthSessionTicketResponse_t *response) {
    /*
     * UNTESTED CODE!!! 
     */
    lua_State *L = luasteam::global_lua_state;
    lua_rawgeti(L, LUA_REGISTRYINDEX, ticket_callback_ref);
    if (response->m_eResult == k_EResultOK) {
        lua_pushboolean(L, true);
    } else {
        lua_pushboolean(L, false);
    };
    lua_pushnumber(L, response->m_eResult);
    lua_call(L, 2, 0);
}


void CallbackListener::ValidateAuthTicketResponse(ValidateAuthTicketResponse_t *response) {
    /*
     * UNTESTED CODE!!! 
     */
    lua_State *L = luasteam::global_lua_state;
    lua_rawgeti(L, LUA_REGISTRYINDEX, validate_callback_ref);
    auto resp = response->m_eAuthSessionResponse;
    if (resp == k_EAuthSessionResponseOK) {
        luasteam::pushuint64(L, response->m_SteamID.ConvertToUint64());
    } else {
        lua_pushboolean(L, false);
    }
    lua_pushnumber(L, resp);
    lua_call(L, 2, 0);
}



// HAuthTicket GetAuthSessionTicket( void *pTicket, int cbMaxTicket, uint32 *pcbTicket );
EXTERN int luasteam_getAuthSessionTicket(lua_State *L) {
    /*
     * UNTESTED CODE!!! 
     */
    static const size_t BUFSIZE = 1024;
    static char pTicket[BUFSIZE];
    luaL_checktype(L, 1, LUA_TFUNCTION);
    ticket_callback_ref = luaL_ref(L, LUA_REGISTRYINDEX);
    uint32 size;
    HAuthTicket ticket = SteamUser()->GetAuthSessionTicket(pTicket, BUFSIZE, &size);
    currentTicket = ticket;
    lua_pushlstring(L, pTicket, size);
    return 1;
}

// EBeginAuthSessionResult BeginAuthSession( const void *pAuthTicket, int cbAuthTicket, CSteamID steamID );
EXTERN int luasteam_beginAuthSession(lua_State *L) {
    /*
    * UNTESTED CODE!!! 
    */
    luaL_checkstring(L, 1);
    uint64 steamid = luasteam::checkuint64(L, 2);
    size_t len;
    const char* pAuthTicket = lua_tolstring(L, 1, &len);
    EBeginAuthSessionResult result = SteamUser()->BeginAuthSession(pAuthTicket, len, steamid);
    if (result == k_EBeginAuthSessionResultOK) {
        lua_pushboolean(L,true);
    } else {
        lua_pushboolean(L,false);
    }
    lua_pushnumber(L,result);
    return 1;
}

// void CancelAuthTicket( HAuthTicket hAuthTicket );
EXTERN int luasteam_cancelAuthTicket(lua_State *L) {
    SteamUser()->CancelAuthTicket(currentTicket);
    return 1;
}

// void EndAuthSession( CSteamID steamID );
EXTERN int luasteam_endAuthSession(lua_State *L) {
    uint64 steamid = luasteam::checkuint64(L, 1);
    CSteamID id = CSteamID(steamid);
    SteamUser()->EndAuthSession(id);
    return 1;
}

} // namespace



// int GetPlayerSteamLevel();
EXTERN int luasteam_getPlayerSteamLevel(lua_State *L) {
    lua_pushnumber(L, SteamUser()->GetPlayerSteamLevel());
    return 1;
}

// CSteamID GetSteamID();
EXTERN int luasteam_getSteamID(lua_State *L) {
    luasteam::pushuint64(L, SteamUser()->GetSteamID().ConvertToUint64());
    return 1;
}


namespace luasteam {

void add_user(lua_State *L) {
    lua_createtable(L, 0, 2);
    add_func(L, "getPlayerSteamLevel", luasteam_getPlayerSteamLevel);
    add_func(L, "getSteamID", luasteam_getSteamID);
    add_func(L, "getAuthSessionTicket", luasteam_getAuthSessionTicket);
    add_func(L, "beginAuthSession", luasteam_beginAuthSession);
    add_func(L, "cancelAuthTicket", luasteam_cancelAuthTicket);
    add_func(L, "endAuthSession", luasteam_endAuthSession);
    lua_setfield(L, -2, "user");
}


static CallbackListener *user_listener;

void init_user(lua_State *L) {
    user_listener = new CallbackListener();
}

void shutdown_user(lua_State *L) {
    delete user_listener;
    user_listener = nullptr;
}

} // namespace luasteam
