//
// Created by Administrator on 2020/8/27 0027.
//
#include "SkMemory.h"
#include "common.h"
#include "lua_wrapper.h"

#define copy_data_f(dType, step) \
char * addr = static_cast<char *>(data); \
for (int i = 0; i < tableCount; ++i) { \
    idx = 1; \
    type = lua_rawgeti(L, i + 1, idx); \
    for (; type != LUA_TNIL ;) { \
        *addr = static_cast<dType>(lua_tonumber(L, -1)); \
        addr += step; \
        lua_pop(L, 1); \
        ++idx; \
        type = lua_rawgeti(L, i + 1, idx); \
    } \
}

#define copy_data_i(dType, step) \
char * addr = static_cast<char *>(data); \
for (int i = 0; i < tableCount; ++i) { \
    idx = 1; \
    type = lua_rawgeti(L, i + 1, idx); \
    for (; type != LUA_TNIL ;) { \
        *addr = static_cast<dType>(lua_tointeger(L, -1)); \
        addr += step; \
        lua_pop(L, 1); \
        ++idx; \
        type = lua_rawgeti(L, i + 1, idx); \
    } \
}

AbsSkMemory::~AbsSkMemory() {
    destroyData();
}
AbsSkMemory::AbsSkMemory():_ref(1) {

}
int AbsSkMemory::ref() {
    return _ref.fetch_add(1);
}
int AbsSkMemory::unRef() {
    return _ref.fetch_add(-1);
}

void AbsSkMemory::asConstant() {
    _constant = 1;
}
void AbsSkMemory::destroyData() {
    if(data){
        free(data);
        data = nullptr;
    }
}
//---------------------------------------------------------------------------------
SkMemory::SkMemory(const char *type, int len): AbsSkMemory(), _dType(type) {
    size = getUnitSize(type) * len;
    SkASSERT(size > 0);
    data = malloc(size);
}
SkMemory::SkMemory(lua_State *L, int tableCount, const char *t) : AbsSkMemory(){
    _dType = t;
    size = getTotalBytes(L, tableCount, t);
    SkASSERT(size > 0);
    data = malloc(size);
    int type;
    int idx;
    switch (t[0]) {
        case 'f': {
            copy_data_f(float, 4);
        }
            break;
        case 'd': {
            copy_data_i(uint32_t, 4);
        }
            break;
        case 'w': {
            copy_data_i(uint16_t, 2);
        }
            break;
        case 'b':{
            copy_data_i(uint8_t , 2);
        }
            break;
    }
    lua_pop(L, 1);
}

float SkMemory::readFloat(size_t index) {
    float* addr = (float*)data;
    addr += index;
    return *addr;
}
uint8_t SkMemory::readUInt8(size_t index) {
    uint8_t* addr = (uint8_t*)data;
    addr += index;
    return *addr;
}
uint16_t SkMemory::readUInt16(size_t index) {
    uint16_t* addr = (uint16_t*)data;
    addr += index;
    return *addr;
}
uint32_t SkMemory::readUInt32(size_t index) {
    uint32_t* addr = (uint32_t*)data;
    addr += index;
    return *addr;
}
void SkMemory::writeFloat(size_t index, float val) {
    float* addr = (float*)data;
    addr += index;
    *addr = val;
}
void SkMemory::writeUInt8(size_t index, uint8_t val){
    uint8_t * addr = (uint8_t*)data;
    addr += index;
    *addr = val;
}
void SkMemory::writeUInt16(size_t index, uint16_t val) {
    uint16_t * addr = (uint16_t*)data;
    addr += index;
    *addr = val;
}
void SkMemory::writeUInt32(size_t index, uint32_t val) {
    uint32_t * addr = (uint32_t*)data;
    addr += index;
    *addr = val;
}
bool SkMemory::isFloat() {
    return _dType[0] == 'f';
}
int SkMemory::write(SkMemory* mem, lua_State *L) {
    //table, index, value
    auto index = lua_tointeger(L, -2);
    switch (mem->_dType[0]) {
        case 'f':
            mem->writeFloat(index, TO_FLOAT(L, -1));
            return 0;
        case 'd':
            mem->writeUInt32(index, TO_NUMBER_32(L, -1));
            return 0;
        case 'w':
            mem->writeUInt16(index, TO_NUMBER_16(L, -1));
            return 0;
        case 'b':
            mem->writeUInt8(index, TO_NUMBER_8(L, -1));
            return 0;

        default:
            return luaL_error(L, "wrong data type = %s", mem->_dType);
    }
}
int SkMemory::read(SkMemory* mem, lua_State *L) {
    //table, index
    auto index = lua_tointeger(L, -1);
    switch (mem->_dType[0]) {
        case 'f':
            lua_pushnumber(L, mem->readFloat(index));
            return 1;
        case 'd':
            lua_pushnumber(L, mem->readUInt32(index));
            return 1;
        case 'w':
            lua_pushnumber(L, mem->readUInt16(index));
            return 1;
        case 'b':
            lua_pushnumber(L, mem->readUInt8(index));
            return 1;

        default:
            return luaL_error(L, "wrong data type = %s", mem->_dType);
    }
}

//-----------------------------------------------------------------------
inline int SkMemory::getUnitSize(const char *t) {
    switch (t[0]) {
        case 'f':
        case 'd':
            return 4;
        case 'w':
            return 2;
        case 'b':
            return 1;

        default:
            return 0;
    }
}
inline int SkMemory::getTotalBytes(lua_State *L, int tableCount, const char *t){
    int minSize = getUnitSize(t);
    if(minSize == 0){
        luaL_error(L, "wrong type of memory-unit: %s", t);
        return 0;
    }
    int totalSize = 0 ;
    for (int i = 0; i < tableCount; ++i) {
        size_t len = lua_rawlen(L, i + 1);
        totalSize += len * minSize;
    }
    return totalSize;
}

//------------------------ SkMemoryFFFI ----------------------
SkMemoryFFFUI::SkMemoryFFFUI(lua_State *L, int tableCount) : AbsSkMemory(){
    size = 4 * 4 * tableCount;
    SkASSERT(size > 0);
    data = malloc(size);

    //LOGD("----- new SkMemoryFFFUI -----");
    //luaB_dumpStack(L);
    char * addr = static_cast<char *>(data);
    int type;
    for (int i = 0; i < tableCount; ++i) {
       // LOGD("----- table %d -----", i);
        for (int idx = 0; idx < 3; ++idx) {
          //  LOGD("----- member index = %d -----", idx);
            type = lua_rawgeti(L, i + 1, idx + 1);
          //  luaB_dumpStack(L);
            if(type != LUA_TNUMBER){
                luaL_error(L, "create SkMemoryFFFUI failed. for error data type = %d", type);
                goto out;
            }
            *addr = static_cast<float >(lua_tonumber(L, -1));
            addr += 4;
            lua_pop(L, 1);
        }
       // LOGD("----- member index = 3 -----");
        type = lua_rawgeti(L, i + 1, 4);
       // luaB_dumpStack(L);
        if(type != LUA_TNUMBER){
            luaL_error(L, "create SkMemoryFFFUI failed. for error data type = %d", type);
            goto out;
        }
        *addr = static_cast<uint32_t>(lua_tointeger(L, -1));
        addr += 4;
        lua_pop(L, 1);
    }
    lua_pop(L, 1);
    return ;
    out:
        unRef();
}