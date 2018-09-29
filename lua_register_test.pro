TEMPLATE = app

QT += core
QT += gui
QT += widgets

SOURCES += $$PWD/lua_cpp_register.cpp
HEADERS += $$PWD/lua_cpp_register.hpp
include($$PWD/lua/lua.pri)

SOURCES += main.cpp

win32-msvc*{
    QMAKE_CXXFLAGS += /std:c++latest
    QMAKE_CXXFLAGS += /await
    CONFIG+=suppress_vcproj_warnings
}else{
    CONFIG+=c++17
}

CONFIG+=console













