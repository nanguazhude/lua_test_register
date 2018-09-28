#include <QtCore>
#include <QtWidgets>

#include "lua_cpp_register.hpp"



template<typename X>
class A {
public:
    using supers = sstd::class_wrap< float>;
    static constexpr std::string_view typeName() { return "int"sv; }
};

template<typename X>
class B {
public:
    using supers = sstd::class_wrap< double>;
    static constexpr std::string_view typeName() { return "int"sv; }
};

template<typename X>
class C {
public:
    using supers = sstd::class_wrap< int>;
    static constexpr std::string_view typeName() { return "int"sv; }
};

template<typename ZF>
class Z {
public:
    using supers = sstd::class_wrap<A<ZF>, B<ZF>, C<ZF>>;
};

template<typename T>
void fooo(int * arg) {}

template<typename ... Txxx>
void foxx(int * arg) {
    ( fooo<Txxx>( arg) , ...  );
}

int main(int argc, char ** argv) {

    //TreeToListHelper<1, sstd::unique_cat< HasSupers<Z<int>>::type, HasSupers <sstd::get_from_index<0, HasSupers<Z<int>>::type >::type>::type>::type>::type x;

    foxx<int,double,float>(&argc);

    sstd::private_sstd::tree_to_list< Z<int> >::type xfdsfa;

    //sstd::get_from_index<3, Z<int>::supers>::type sfasd;

    QApplication varApp{ argc,argv };

    QWidget widget;
    widget.show();


    return varApp.exec();

}





















