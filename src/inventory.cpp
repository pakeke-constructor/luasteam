#include "inventory.hpp"

// ==============================
// ======= SteamInventory =======
// ==============================

#include <vector>
using luasteam::CallResultListener;

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


namespace luasteam {


template <> void CallResultListener<SteamInventoryStartPurchaseResult_t>::Result(SteamInventoryStartPurchaseResult_t *data,bool io_fail) {
    /**
     * UNFINISHED!!!!
     *  need to use the actual result!!
     */
    lua_State *L = luasteam::global_lua_state;
    // getting stored callback function
    lua_rawgeti(L, LUA_REGISTRYINDEX, callback_ref);
    luaL_unref(L, LUA_REGISTRYINDEX, callback_ref);
    // calling function
    if (io_fail || data->m_result != k_EResultOK) {
        lua_pushnil(L);
        lua_pushint(L, data->m_result);
        lua_call(L, -3);
        lua_call();
        return 2;
    } else {
        lua_createtable(L, 0, 2);
        //luasteam::pushuint64(L, data->m_hSteamLeaderboard);
        lua_setfield(L, -2, "steamLeaderboard");
        //lua_pushboolean(L, data->m_bLeaderboardFound != 0);
        lua_setfield(L, -2, "leaderboardFound");
    }
    lua_pushboolean(L, io_fail);
    lua_call(L, 2, 0);
    delete this; // DELET THIS
}


}






// SteamAPICall_t StartPurchase( const SteamItemDef_t *pArrayItemDefs, const uint32 *punArrayQuantity, uint32 unArrayLength );
EXTERN int luasteam_startPurchase(lua_State *L) {
    luaL_argcheck(L, lua_istable(L,1), 1, "must be array");
    luaL_argcheck(L, lua_istable(L,2), 2, "must be array");
    size_t len1 = lua_objlen(L,1);
    size_t len2 = lua_objlen(L,2);
    luaL_argcheck(L, len1==len2, 2, "table lengths not equal");
    luaL_checktype(L, 3, LUA_TFUNCTION);

    std::vector<int32> pArrayItemDefs(len1);
    std::vector<uint32> punArrayQuantity(len1);

    for (int index = 0; index < len1; index++) {
        // Our actual index will be +1 because Lua 1 indexes tables.
        int actualIndex = index + 1; 

        lua_pushvalue(L,1); // pArrayItemDef table
        lua_pushinteger(L, actualIndex);
        // Get the table data at this index (and not get the table, which is what I thought this did.)
        lua_gettable(L, -2);
        int itemId = luaL_checkint(L, -1);
        lua_pop(L,3);
        pArrayItemDefs.push_back(itemId);

        lua_pushvalue(L,2); // punArrayQuantity table
        lua_pushinteger(L, actualIndex);
        lua_gettable(L, -2);
        int itemCount = luaL_checkint(L, -1);
        luaL_argcheck(L, itemCount > 0, 2, "buy counts must be positive");
        lua_pop(L,3);
        pArrayItemDefs.push_back(itemCount);
    }

    if (pArrayItemDefs.size() != len1 || punArrayQuantity.size() != len1) {
        lua_error(L);
    }

    auto *listener = new CallResultListener<SteamInventoryStartPurchaseResult_t>();
    SteamAPICall_t call = SteamInventory()->StartPurchase(
        pArrayItemDefs.data(), 
        punArrayQuantity.data(), 
        len1
    );

    return 0;
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

