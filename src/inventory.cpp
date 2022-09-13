#include "inventory.hpp"

/*
PLANNING:::

Okay, what functions do we actually need?
Lets try implement the bare minimum:::


SteamAPICall_t StartPurchase( const SteamItemDef_t *pArrayItemDefs, const uint32 *punArrayQuantity, uint32 unArrayLength );

bool SerializeResult( SteamInventoryResult_t resultHandle, void *pOutBuffer, uint32 *punOutBufferSize );
bool DeserializeResult( SteamInventoryResult_t *pOutResultHandle, const void *pBuffer, uint32 unBufferSize, bool bRESERVED_MUST_BE_FALSE = false );
bool CheckResultSteamID( SteamInventoryResult_t resultHandle, CSteamID steamIDExpected );

bool GetResultItems( SteamInventoryResult_t resultHandle, SteamItemDetails_t *pOutItemsArray, uint32 *punOutItemsArraySize );

bool GetResultItemProperty( SteamInventoryResult_t resultHandle, uint32 unItemIndex, const char *pchPropertyName, char *pchValueBuffer, uint32 *punValueBufferSizeOut );

bool ExchangeItems( 
    SteamInventoryResult_t *pResultHandle, const SteamItemDef_t *pArrayGenerate, 
    const uint32 *punArrayGenerateQuantity, uint32 unArrayGenerateLength, 
    const SteamItemInstanceID_t *pArrayDestroy, const uint32 *punArrayDestroyQuantity, 
    uint32 unArrayDestroyLength 
);

void DestroyResult( SteamInventoryResult_t resultHandle );

*/

namespace {

int inventoryResultMt_ref = LUA_NOREF;

void pushInventoryResult(lua_State *L, SteamInventoryResult_t resultHandle) {
    SteamInventoryResult_t *rh = reinterpret_cast<SteamInventoryResult_t *>(lua_newuserdata(L, sizeof(SteamInventoryResult_t)));
    *rh = resultHandle;
    lua_rawgeti(L, LUA_REGISTRYINDEX, inventoryResultMt_ref);
    lua_setmetatable(L, -2);
}

SteamInventoryResult_t checkInventoryResult(lua_State *L, int nParam) {
    luaL_argcheck(L, lua_isuserdata(L, nParam), nParam, "must be userdata");
    lua_rawgeti(L, LUA_REGISTRYINDEX, inventoryResultMt_ref);
    lua_getmetatable(L, nParam);
    luaL_argcheck(L, lua_rawequal(L, -2, -1), nParam, "must be inventoryResult");
    lua_pop(L, 2);
    SteamInventoryResult_t *ptr = reinterpret_cast<SteamInventoryResult_t *>(lua_touserdata(L, nParam));
    return *ptr;
}


} // namespace


// SteamAPICall_t StartPurchase( const SteamItemDef_t *pArrayItemDefs, const uint32 *punArrayQuantity, uint32 unArrayLength );
EXTERN int luasteam_startPurchase(lua_State *L) {
    
}

// bool SerializeResult( SteamInventoryResult_t resultHandle, void *pOutBuffer, uint32 *punOutBufferSize );
EXTERN int luasteam_serializeResult(lua_State *L) {

}

// bool DeserializeResult( SteamInventoryResult_t *pOutResultHandle, const void *pBuffer, uint32 unBufferSize, bool bRESERVED_MUST_BE_FALSE = false );
EXTERN int luasteam_deserializeResult(lua_State *L) {

}

// bool CheckResultSteamID( SteamInventoryResult_t resultHandle, CSteamID steamIDExpected );
EXTERN int luasteam_checkResultSteamID(lua_State *L) {

}

//bool GetResultItems( SteamInventoryResult_t resultHandle, SteamItemDetails_t *pOutItemsArray, uint32 *punOutItemsArraySize );
EXTERN int luasteam_getResultItems(lua_State *L) {

}

//bool GetResultItemProperty( SteamInventoryResult_t resultHandle, uint32 unItemIndex, const char *pchPropertyName, char *pchValueBuffer, uint32 *punValueBufferSizeOut );
EXTERN int luasteam_getResultItemProperty(lua_State *L) {

}

//bool ExchangeItems( SteamInventoryResult_t *pResultHandle, const SteamItemDef_t *pArrayGenerate, const uint32 *punArrayGenerateQuantity, uint32 unArrayGenerateLength, const SteamItemInstanceID_t *pArrayDestroy, const uint32 *punArrayDestroyQuantity, uint32 unArrayDestroyLength );
EXTERN int luasteam_exchangeItems(lua_State *L) {

}

//void DestroyResult( SteamInventoryResult_t resultHandle );
EXTERN int luasteam_destroyResult(lua_State *L) {

}



namespace luasteam {

void add_inventory(lua_State *L) {
    lua_createtable(L, 0, 1);
    // add_func(L, "parseUint64", luasteam_parseUint64);
    lua_setfield(L, -2, "inventory");
}

void init_inventory(lua_State *L) {
    lua_createtable(L, 0, 2);
    //add_func(L, "__eq", luasteam_equint64);
    //add_func(L, "__tostring", luasteam_uint64ToString);
    inventoryResultMt_ref = luaL_ref(L, LUA_REGISTRYINDEX);
}

void shutdown_inventory(lua_State *L) {}

} // namespace luasteam

