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
           



    QApplication varApp{ argc,argv };

    QWidget widget;
    widget.show();


    return varApp.exec();

}





















