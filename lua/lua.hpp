#pragma once

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

#include <exception>

class LuaCplusplusExceptioin : public std::exception {
public:
    LuaCplusplusExceptioin();
};


