#pragma once

#ifndef LUA_REGISTER_DLL_EXPORT
#define LUA_REGISTER_DLL_EXPORT
#endif

#include <lua.hpp>
#include <variant>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <type_traits>
#include <string_view>
#include <initializer_list>
using namespace std::string_view_literals;

#include <typeinfo>
#include <typeindex>

namespace sstd {

    LUA_REGISTER_DLL_EXPORT class VirtualBasic {
    public:
        virtual ~VirtualBasic();
    };

    LUA_REGISTER_DLL_EXPORT class LuaKeyString : public std::string_view {
        using ttt_super_type = std::string_view;
        std::shared_ptr<const void> mmm_data;
    public:
        LuaKeyString() = default;
        LuaKeyString(const std::string_view &);
        LuaKeyString(const LuaKeyString &) = default;
        LuaKeyString(LuaKeyString &&) = default;
        LuaKeyString&operator=(const LuaKeyString &) = default;
        LuaKeyString&operator=(LuaKeyString &&) = default;
        LuaKeyString(const std::string_view &, decltype(nullptr));
    };
    using LuaKeyUnsignedInteger = std::uintptr_t;
    using LuaCFunction = lua_CFunction;
    typedef void*(*LuaClassCastFunction)(void *);

    LUA_REGISTER_DLL_EXPORT class LuaTypeFunctionsMap :
        public std::shared_ptr<LuaTypeFunctionsMap>,
        public virtual VirtualBasic {
        std::unique_ptr<VirtualBasic> mmm_private;
    public:
        LuaTypeFunctionsMap();
        virtual ~LuaTypeFunctionsMap();
        using Function = std::variant<LuaCFunction, LuaClassCastFunction>;
        void appendFunction(const std::string_view &, Function);
        void appendFunction(LuaKeyUnsignedInteger, Function);
        std::optional<Function> getFunction(LuaKeyUnsignedInteger) const;
        std::optional<Function> getFunction(const std::string_view &) const;
    protected:
        void clear();
    };

    LUA_REGISTER_DLL_EXPORT class RuntimClass :
        public virtual VirtualBasic {
    public:
        RuntimClass();
        RuntimClass(const RuntimClass &) = delete;
        RuntimClass(RuntimClass &&) = delete;
        RuntimClass &operator=(const RuntimClass &) = delete;
        RuntimClass &operator=(RuntimClass &&) = delete;
        virtual ~RuntimClass();
    public:
        virtual void * data() const = 0;
        virtual std::string_view registerTypeName() const = 0;
        virtual LuaTypeFunctionsMap * registerTypeFunctions() const = 0;
        virtual LuaKeyUnsignedInteger registerTypeIndex() const = 0;
    };

    LUA_REGISTER_DLL_EXPORT std::optional<LuaKeyUnsignedInteger> getRegisterIndex(const std::string_view);
    LUA_REGISTER_DLL_EXPORT std::optional<std::string_view> getRegisterName(LuaKeyUnsignedInteger);
    LUA_REGISTER_DLL_EXPORT LuaKeyUnsignedInteger setRegisterTypeIndex(const std::string_view);
    LUA_REGISTER_DLL_EXPORT std::shared_ptr<LuaTypeFunctionsMap> getRegisterFunctionMap(LuaKeyUnsignedInteger);

    LUA_REGISTER_DLL_EXPORT void attachStdTypeIndex(const std::string_view, const std::type_index);
    LUA_REGISTER_DLL_EXPORT void removeStdTypeIndex(const std::type_index);
    LUA_REGISTER_DLL_EXPORT std::optional<LuaKeyUnsignedInteger> getRegisterIndex(const std::type_index);

    /**********************************************************************************/
    namespace private_sstd {

        template<typename ...TName>
        class class_wrap {
        public:
            constexpr static inline std::size_t size() { return sizeof...(TName); }
        };

        template<>
        class class_wrap<void> {
        public:
            constexpr static inline std::size_t size() { return 0; }
        };

        template<typename TName> class get_first;
        template<template<typename ...> class A, typename ATF, typename ATS, typename ... AT>
        class get_first< A< ATF, ATS, AT ...> > {
        public:
            using first_type = ATF;
            using second_type = ATS;
            using tail = class_wrap<ATS, AT...>;
            constexpr const static bool has_first = true;
            constexpr const static bool has_second = true;
        };

        template<template<typename > class A, typename ATF>
        class get_first< A<ATF> > {
        public:
            using first_type = ATF;
            using second_type = void;
            using tail = class_wrap<void>;
            constexpr const static bool has_first = true;
            constexpr const static bool has_second = false;
        };

        template<template<typename > class A >
        class get_first< A<void> > {
        public:
            using first_type = void;
            using second_type = void;
            using tail = class_wrap<void>;
            constexpr const static bool has_first = false;
            constexpr const static bool has_second = false;
        };

        template<std::size_t, typename > class get_from_index;

        template<template<typename ...> class A, typename ... AT  >
        class get_from_index<0, A<AT...>> {
        public:
            using type = typename get_first<A<AT...>>::first_type;
        };

        template<std::size_t N, template<typename ...> class A, typename ... AT  >
        class get_from_index<N, A<AT...>> {
            using tail = typename get_first<A<AT...>>::tail;
        public:
            using type = std::conditional_t<
                (N < sizeof...(AT)),
                typename get_from_index<N - 1, tail>::type,
                private_sstd::class_wrap<void>>;
        };

        template<typename, typename > class has_class;

        template<template<typename > class A, typename AT, typename B>
        class has_class< A<AT>, B > {
        public:
            const constexpr static bool value = std::is_same_v<AT, B>;
        };

        template<template<typename ... > class A, typename AT0, typename AT1, typename ... AT, typename B>
        class has_class<A<AT0, AT1, AT...>, B> {
            using tail = typename get_first< A< AT0, AT1, AT... > >::tail;
        public:
            const constexpr static bool value = std::is_same_v<AT0, B> || has_class<tail, B>::value;
        };

        template<typename, typename> class unique_append;
        template<template <typename ...> class  A, typename ... AT, typename B>
        class unique_append<A<AT...>, B> {
        public:
            using type = std::conditional_t<has_class<A<AT...>, B>::value, class_wrap<AT...>, class_wrap<AT..., B> >;
        };

        template<template <typename ...> class  A, typename ... AT >
        class unique_append<A<AT...>, void> {
        public:
            using type = class_wrap<AT...>;
        };

        template<typename>
        class unique;

        namespace _private {
            template<typename, typename>
            class unique_detail;

            template<template<typename ...> class A, typename ... AT,
                template<typename > class B, typename BT >
            class unique_detail<A<AT...>, B<BT>> {
            public:
                using type = typename unique_append<A<AT...>, BT>::type;
            };

            template<template<typename ...> class A, typename ... AT,
                template<typename ... > class B, typename BT0, typename BT1, typename ... BT >
            class unique_detail<A<AT...>, B<BT0, BT1, BT...>> {
                using type0 = typename unique_append<A<AT...>, BT0>::type;
            public:
                using type = typename unique_detail<type0, class_wrap<BT1, BT...>>::type;
            };

        }/*_private*/

        template<template<typename> class T, typename T0>
        class unique<T<T0>> {
        public:
            using type = class_wrap<T0>;
        };

        template<template<typename...> class T, typename T0, typename T1, typename ... TT>
        class unique<T<T0, T1, TT...>> {
        public:
            using type = typename get_first<
                typename _private::unique_detail<
                class_wrap<void>, class_wrap<T0, T1, TT...> >::type >::tail;
        };

        namespace _private {
            template<typename T1, typename T2>
            class cat2;

            template<template<typename ...> class A, template<typename ...> class B, typename ... AT, typename ... BT>
            class cat2<A<AT ...>, B<BT ...>> {
            public:
                using type = class_wrap< AT ..., BT ... >;
            };

            template<template<typename ...> class A, template<typename> class B, typename ... AT >
            class cat2<A<AT ...>, B<void>> {
            public:
                using type = class_wrap< AT ... >;
            };

        }/*_private*/

        template<typename ... > class cat;

        template<template<typename ...> class A, typename ... TName>
        class cat<A<TName...>> {
        public:
            using type = class_wrap<TName...>;
        };

        template<template<typename ...> class A0, typename ...T0,
            template<typename ...> class A1, typename ...T1 >
        class cat<A0<T0...>, A1<T1...>> {
        public:
            using type = typename _private::cat2< class_wrap<T0...>, class_wrap<T1...> >::type;
        };

        template<typename T1, typename T2, typename T3, typename ... TName>
        class cat<T1, T2, T3, TName...> {
        public:
            using type = typename cat< typename cat<T1, T2>::type, typename cat<T3, TName...>::type >::type;
        };

        template<typename ... TName>
        class unique_cat {
            using cat_type = typename cat<TName...>::type;
        public:
            using type = typename unique<cat_type>::type;
        };

        namespace _private_helper {

            template<typename TName, typename = void>
            class has_supers_help {
            public:
                using supers = private_sstd::class_wrap< void >;
                using type = supers;
            };

            template<typename TName>
            class has_supers_help<TName, std::enable_if_t<sizeof(typename TName::supers)>> {
            public:
                using supers = typename TName::supers;
                using type = supers;
            };

            template<std::size_t N, typename TName, bool = (N < TName::size()) >
                class tree_to_list_helper {
                public:
                    using type = TName;
                };

                template<std::size_t N, typename TName >
                class tree_to_list_helper<N, TName, true > {
                    static_assert(N < 256, "this may be a error");
                    using TypeN = typename private_sstd::get_from_index<N, TName>::type/*get_from_index*/;
                    using TypeNSupers = typename has_supers_help</*HasSupers*/TypeN>::type/*HasSupers*/;
                    using TypeNext = typename private_sstd::unique_cat< TName, TypeNSupers/*unique_cat*/>::type/*unique_cat*/;
                public:
                    using type =
                        typename tree_to_list_helper< N + 1, TypeNext/*TreeToListHelper*/>::type/*TreeToListHelper*/;
                };

        }

        template<typename TName>
        class tree_to_list {
            using _supers = typename private_sstd::unique_cat<
                private_sstd::class_wrap<void>,
                typename _private_helper::has_supers_help<TName>::type
            >::type;
        public:
            using type = typename _private_helper::tree_to_list_helper< 0, _supers  >::type;
        };

    }/*namespace private sstd*/
    /**********************************************************************************/

    template<typename ...TName>
    using class_wrap = private_sstd::class_wrap<TName...>;

    using RegisterFunctionType = std::pair<std::string_view, lua_CFunction>;
    template<typename Child>
    class IntBasicRegister {
    public:
        inline constexpr static std::string_view typeName() { return "int"sv; }
        inline constexpr static std::initializer_list<RegisterFunctionType> getFunctions() { return{}; }
        using supers = class_wrap<void>;
        using this_type = int;
    private:
        static_assert((std::is_same_v<this_type, Child>) || (std::is_base_of_v<this_type, Child>), "Child must be child of this_type");
    };

    namespace private_register_type {

        template<typename TName, typename SuperType>
        void register_super(LuaTypeFunctionsMap * childMap) {
            using ThisType = SuperType;
            const auto varThisTypeIndex = setRegisterTypeIndex(ThisType::typeName());
            auto varThisFuncsMap = childMap;
            {
                std::initializer_list<RegisterFunctionType> varThisFuncs = ThisType::getFunctions();
                for (const auto & varI : varThisFuncs) {
                    varThisFuncsMap->appendFunction(varI.first, varI.second);
                }
            }
            {
                varThisFuncsMap->appendFunction(varThisTypeIndex, [](void * arg)->void * {
                    return static_cast<typename SuperType::this_type*>(reinterpret_cast<TName *>(arg));
                });
            }
        }

        template<typename, typename>class register_super_help;

        template<typename TName, template<typename ...> class U, typename ... Supers>
        class register_super_help<TName, U<Supers...>> {
        public:
            static void run(LuaTypeFunctionsMap * arg) {
                (register_super<TName, Supers>(arg), ...);
            }
        };

    }/*private_register_type*/

    template<typename This, class _ThisType/* SomeType<This> */ = This >
    inline LuaKeyUnsignedInteger registerTypeDirect() {
        using ThisType = _ThisType;
        const std::string_view argTypeName = ThisType::typeName();
        const auto varThisTypeIndex = setRegisterTypeIndex(argTypeName);
        std::shared_ptr<LuaTypeFunctionsMap> varThisFuncsMap = getRegisterFunctionMap(varThisTypeIndex);
        {/*register the functions*/
            std::initializer_list<RegisterFunctionType> varThisFuncs = ThisType::getFunctions();
            for (const auto & varI : varThisFuncs) {
                varThisFuncsMap->appendFunction(varI.first, varI.second);
            }
        }
        {/*add this type cast*/
            varThisFuncsMap->appendFunction(varThisTypeIndex, [](void * arg)->void * {
                return reinterpret_cast<This *>(arg);
            });
        }
        {/*add supers*/
            using super_types = typename private_sstd::tree_to_list<ThisType>::type;
            if constexpr (super_types::size()) {
                private_register_type::register_super_help<This, super_types>::run(varThisFuncsMap.get());
            }
        }
        {/*add std index support*/
            attachStdTypeIndex(argTypeName, typeid(This));
        }
        return varThisTypeIndex;
    }

    template<typename This, template<typename...> class _ThisType/* SomeType<This> */ >
    inline LuaKeyUnsignedInteger registerType() {
        return registerTypeDirect<This, _ThisType<This>>();
    }

}/*namespace sstd*/


















