#include <QtCore>
#include <QtWidgets>

#include "lua_cpp_register.hpp"
 

class TypeA {};

template<typename Child>
class TypeAStaticInfo final {
public:
    constexpr static std::string_view typeName() { return "TypeA"sv; }
    constexpr static std::initializer_list<sstd::RegisterFunctionType> getFunctions() { return{}; }
    using supers = sstd::class_wrap<void>;
    using this_type = TypeA;
};

class TypeB : public TypeA {};

template<typename Child>
class TypeBStaticInfo final {
public:
    constexpr static std::string_view typeName() { return "TypeB"sv; }
    constexpr static std::initializer_list<sstd::RegisterFunctionType> getFunctions() { return{}; }
    using supers = sstd::class_wrap< TypeAStaticInfo<Child> >;
    using this_type = TypeB;
};

#include <lua.hpp>
#include <iostream>
int map_function(lua_State *L) try{

    std::cout << "the stack size : " <<lua_gettop(L) <<std::endl ;
    lua_pushvalue(L, lua_upvalueindex(1) );
    std::cout << "upvalue is table : " <<lua_istable(L,-1) <<std::endl ;
    lua_rawgeti(L,-1,1);
    std::cout << lua_tointeger(L,-1) <<std::endl ;
    lua_pop(L,2);

    lua_checkstack(L, 8);
       
    const auto varType = lua_type(L,-1);

    if (varType == LUA_TNUMBER) {
        const auto varIndex = lua_tointeger(L,-1);
        std::cout << varIndex << std::endl; 
    }
    else if (varType == LUA_TSTRING ) {

        const char * string_data;
        std::size_t string_length;

        string_data = lua_tolstring(L, -1, &string_length);
        std::cout << std::string_view(string_data, string_length) << std::endl;

    }

    lua_pushlstring(L,"can not find string",19);
    lua_error(L);

    lua_pushnil(L);
    return 1;

}
catch (const LuaCplusplusExceptioin & e) { 
    throw e;
}
catch (...) {
    lua_pushlstring(L, "there is a cplusplus exception !!!", 34);
    throw;
}

int destory_function(lua_State *L) {
    std::cout << "destory" << std::endl;

    std::destroy_at( reinterpret_cast<std::shared_ptr<void>*>( lua_touserdata(L,-1) ) );

    return 0;
}

int show_error(lua_State *L) {

    const char * string_data;
    std::size_t string_length;

    string_data = lua_tolstring(L, -1, &string_length);
    std::cout << std::string_view(string_data, string_length) << std::endl;

    return 0;
}

int main(int argc, char ** argv) {

    class TypeC final : public TypeB {
    public:
        using this_type = TypeC;
        constexpr static inline std::string_view typeName() { return "TypeC"sv; }
        constexpr static std::initializer_list<sstd::RegisterFunctionType> getFunctions() { return{}; }
        using supers = sstd::class_wrap< TypeBStaticInfo<this_type> >;
    };

    std::is_base_of<int, int>::value  ;

    sstd::registerType<int, sstd::IntBasicRegister >();
    sstd::registerType<TypeA, TypeAStaticInfo >();
    sstd::registerType<TypeB, TypeBStaticInfo >();
    sstd::registerTypeDirect<TypeC >();
    //sstd::registerType<double, sstd::direct_no_member_warp< DoubleName, sstd::IntBasicRegister<int> >  >();
           
    auto L = luaL_newstate();
    luaL_openlibs(L);
    
    {
        lua_newtable(L);
        lua_pushcclosure(L,&destory_function,1);
        //lua_newtable(L);
        //lua_setupvalue(L,-2,1);
        lua_getupvalue(L,-1,1);
        std::cout<<  lua_istable(L,-1) <<std::endl ;
    }

    lua_checkstack(L,8);
    const auto varTop = lua_gettop(L);
    int varMetateTable;

    auto varMemory = lua_newuserdata(L,sizeof(std::shared_ptr<void>));
   
    auto varData = ::new(varMemory)  std::shared_ptr<void>();
    
    if (luaL_newmetatable(L, "std::shared_ptr<void>")) {
        varMetateTable = lua_gettop(L);

        lua_pushlstring(L,"__index",7);

        lua_createtable(L, 1, 0);
        lua_pushinteger(L,999);
        lua_rawseti(L,-2,1);
        lua_pushcclosure(L,&map_function,1);
        //lua_pushcfunction(L, &map_function);
        //lua_createtable(L, 0, 0);
        //lua_setupvalue(L, -2, 0);

        lua_rawset(L, varMetateTable);

        lua_pushlstring(L, "__gc", 4);
        lua_pushcfunction(L, &destory_function);
        lua_rawset(L, varMetateTable);
        
    }
        
    lua_setmetatable(L,-2);

    lua_setglobal(L,"bbc");
    
    lua_settop(L,varTop);

    lua_pushcfunction(L,&show_error);
    auto varErrorFunction = lua_gettop(L);

    luaL_loadstring(L,u8R"( 
print("Hellow Word!") ;
  -- r = aa.bes  ;
  r = bbc.test    ;
 )");

    if (lua_type(L, -1) == LUA_TSTRING) {
        show_error(L);
    }
    else {
        lua_pcall(L, 0, 0, varErrorFunction);
    }

    lua_close(L);

    QApplication varApp{ argc,argv };

    QWidget widget;
    widget.show();


    return varApp.exec();

}





















