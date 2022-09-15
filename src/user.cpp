
#include "user.hpp"
#include "common.hpp"


namespace {

int user_ref = LUA_NOREF;

HAuthTicket currentTicket;


class CallbackListener {
    private:
        STEAM_CALLBACK( CallbackListener, GetAuthSessionTicketResponse, GetAuthSessionTicketResponse_t );
        STEAM_CALLBACK( CallbackListener, ValidateAuthTicketResponse, ValidateAuthTicketResponse_t);
};


void CallbackListener::GetAuthSessionTicketResponse(GetAuthSessionTicketResponse_t *response) {
    /*
     * UNTESTED CODE!!! 
     
    function luasteam.user.getAuthSessionTicketResponse(success, response_code)
        ...
    end
     
     */
    lua_State *L = luasteam::global_lua_state;
    
    lua_rawgeti(L, LUA_REGISTRYINDEX, user_ref);
    lua_getfield(L, -1, "getAuthSessionTicketResponse");

    if (lua_isnil(L,-1)) {
        lua_pop(L, 2);
    } else {
        if (response->m_eResult == k_EResultOK) {
            lua_pushboolean(L, true);
        } else {
            lua_pushboolean(L, false);
        };
        lua_pushnumber(L, response->m_eResult);
        lua_call(L, 2, 0);
    }
}


void CallbackListener::ValidateAuthTicketResponse(ValidateAuthTicketResponse_t *response) {
    /*
    UNTESTED CODE!!!

        function luasteam.user.validateAuthTicketResponse(steamId, response_code)
            ...
        end
     */
    lua_State *L = luasteam::global_lua_state;
    lua_rawgeti(L, LUA_REGISTRYINDEX, user_ref);
    lua_getfield(L, -1, "validateAuthTicketResponse");
    if (lua_isnil(L,-1)) {
        lua_pop(L, 2);
    } else {
        auto resp = response->m_eAuthSessionResponse;
        if (resp == k_EAuthSessionResponseOK) {
            luasteam::pushuint64(L, response->m_SteamID.ConvertToUint64());
        } else {
            lua_pushboolean(L, false);
        }
        lua_pushnumber(L, resp);
        lua_call(L, 2, 0);
    }
}



// HAuthTicket GetAuthSessionTicket( void *pTicket, int cbMaxTicket, uint32 *pcbTicket );
EXTERN int luasteam_getAuthSessionTicket(lua_State *L) {
    /*
     * UNTESTED CODE!!! 
     */
    static const size_t BUFSIZE = 1024;
    static char pTicket[BUFSIZE];
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

    luasteam.user.beginAuthSession(ticket_data, steamID)

    */
    size_t len;
    const char* pAuthTicket = luaL_checklstring(L, 1, &len);
    uint64 steamid = luasteam::checkuint64(L, 2);
    EBeginAuthSessionResult result = SteamUser()->BeginAuthSession(pAuthTicket, len, steamid);
    if (result == k_EBeginAuthSessionResultOK) {
        lua_pushboolean(L,true);
    } else {
        lua_pushboolean(L,false);
    }
    lua_pushnumber(L,result);
    return 2;
}

// void CancelAuthTicket( HAuthTicket hAuthTicket );
EXTERN int luasteam_cancelAuthTicket(lua_State *L) {
    SteamUser()->CancelAuthTicket(currentTicket);
    return 0;
}

// void EndAuthSession( CSteamID steamID );
EXTERN int luasteam_endAuthSession(lua_State *L) {
    uint64 steamid = luasteam::checkuint64(L, 1);
    CSteamID id = CSteamID(steamid);
    SteamUser()->EndAuthSession(id);
    return 0;
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
    lua_createtable(L, 0, 6);
    add_func(L, "getPlayerSteamLevel", luasteam_getPlayerSteamLevel);
    add_func(L, "getSteamID", luasteam_getSteamID);
    add_func(L, "getAuthSessionTicket", luasteam_getAuthSessionTicket);
    add_func(L, "beginAuthSession", luasteam_beginAuthSession);
    add_func(L, "cancelAuthTicket", luasteam_cancelAuthTicket);
    add_func(L, "endAuthSession", luasteam_endAuthSession);
    lua_pushvalue(L, -1);
    user_ref = luaL_ref(L, LUA_REGISTRYINDEX);
    lua_setfield(L, -2, "user");
}


static CallbackListener *user_listener;

void init_user(lua_State *L) {

    user_listener = new CallbackListener();
}

void shutdown_user(lua_State *L) {
    luaL_unref(L, LUA_REGISTRYINDEX, user_ref);
    user_ref = LUA_NOREF;
    delete user_listener;
    user_listener = nullptr;
}

} // namespace luasteam
