---
--- Generated by EmmyLua(https://github.com/EmmyLua)
--- Created by Administrator.
--- DateTime: 2020/8/28 0028 上午 10:03
---

local m = {};
local mem = require("hmem_lua");

--- create single type memory. type: can be 'd', 'w', 'b', 'f'.
--- 'd' is u-int32
--- 'w' is u-int16
--- 'b' is u-int8
--- 'f' is float
--- ...: means multi array.
function m.newMemory(type, ...)
    local tabs = {...};
    return mem.newMemory(type, table.unpack(tabs));
end

--- create fix type memory: 'float-float-float-uint32'
--- ...: means multi table of 'fffui'.
function m.newMemoryFFFUI(...)
    local tabs = {...};
    -- lua 'table.unpack()' as parameter must be the final parameter.or else the lua stack only contains first parameter.
    return mem.newMemoryFFFUI(table.unpack(tabs));
end
--- create single type memory array. type: can be 'd', 'w', 'b', 'f'.
--- 'd' is u-int32
--- 'w' is u-int16
--- 'b' is u-int8
--- 'f' is float
--- @param type: data type
--- @param len: the length of array
function m.newMemoryArray(type, len)
    return mem.newMemoryArray(type, len);
end

return m;