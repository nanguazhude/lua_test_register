#include <string>
#include <map>
#include <vector>
#include <mutex>
#include <unordered_map>
#include <shared_mutex>
#include <memory_resource>
#include "lua_cpp_register.hpp"

namespace sstd {

    namespace { class invalid_type {}; }

    using string = std::string;
    template<typename TName, typename ... Args>
    std::shared_ptr<TName> make_shared(Args && ... args) {
        return std::make_shared<TName>(std::forward<Args>(args)...);
    }

    template<typename TName, typename ...Args>
    std::unique_ptr<TName> make_unique(Args && ... args) {
        return std::make_unique<TName>(std::forward<Args>(args)...);
    }

    template<typename K, typename V>
    using map = std::map<K, V, std::less<void>>;

    template<typename TName>
    using vector = std::vector<TName>;

    template<typename K, typename V>
    using unordered_map = std::unordered_map<K, V>;

    RuntimClass::RuntimClass() {

    }

    RuntimClass::~RuntimClass() {

    }

    LuaKeyString::LuaKeyString(const std::string_view & arg) {
        auto varThisData = std::make_shared<string>(arg);
        ttt_super_type::operator=(*varThisData);
        mmm_data = std::move(varThisData);
    }

    LuaKeyString::LuaKeyString(const std::string_view & a, decltype(nullptr)) :ttt_super_type(a) {

    }

    LuaTypeFunctionsMap::~LuaTypeFunctionsMap() {

    }

    namespace {

        class LuaTypeFunctionsMapPrivate : public VirtualBasic {
        public:
            using ItemKey = std::variant<LuaKeyUnsignedInteger, std::string_view>;
            class Item {
            public:
                LuaTypeFunctionsMap::Function function;
                LuaKeyString keystring;
                Item(LuaTypeFunctionsMap::Function arg) :function(arg) {}
                Item(LuaTypeFunctionsMap::Function arg, const std::string_view & arg1) :
                    function(arg),
                    keystring(arg1) {}
            };
            using map_type = map<ItemKey, Item>;
            map_type data;
            mutable std::shared_mutex mutex;

            template<typename KT>
            void appendFunction(const KT & xk, LuaTypeFunctionsMap::Function f) {
                std::unique_lock varWriteLock{ mutex };
                ItemKey k;
                if constexpr (std::is_unsigned_v< std::remove_reference_t<KT> >) {
                    k = xk;
                    if (data.count(k) > 0) { return; }
                    data.try_emplace(k, f);
                }
                else {
                    k = xk;
                    if (data.count(k) > 0) { return; }
                    Item v{ f,xk };
                    k = v.keystring;
                    data.try_emplace(k, std::move(v));
                }
            }

            template<typename KT>
            std::optional<LuaTypeFunctionsMap::Function> getFunction(const KT & k) const {
                std::shared_lock varReadLock{ mutex };
                map_type::const_iterator varPos;
                if constexpr (std::is_unsigned_v< std::remove_reference_t<KT> >) {
                    varPos = data.find(ItemKey(k));
                }
                else {
                    varPos = data.find(ItemKey(k));
                }
                if (varPos == data.end()) { return {}; }
                return varPos->second.function;
            }

            void clear() {
                std::unique_lock varWriteLock{ mutex };
                data.clear();
            }

        };

    }/**/

    LuaTypeFunctionsMap::LuaTypeFunctionsMap() :mmm_private(make_unique<LuaTypeFunctionsMapPrivate>()) {

    }

    void LuaTypeFunctionsMap::clear() {
        auto p = static_cast<LuaTypeFunctionsMapPrivate*>(mmm_private.get());
        p->clear();
    }

    void LuaTypeFunctionsMap::appendFunction(const std::string_view & k, Function v) {
        auto p = static_cast<LuaTypeFunctionsMapPrivate*>(mmm_private.get());
        p->appendFunction(k, v);
    }

    void LuaTypeFunctionsMap::appendFunction(LuaKeyUnsignedInteger k, Function v) {
        auto p = static_cast<LuaTypeFunctionsMapPrivate*>(mmm_private.get());
        p->appendFunction(k, v);
    }

    std::optional<LuaTypeFunctionsMap::Function> LuaTypeFunctionsMap::getFunction(LuaKeyUnsignedInteger k) const {
        auto p = static_cast<const LuaTypeFunctionsMapPrivate*>(mmm_private.get());
        return p->getFunction(k);
    }

    std::optional<LuaTypeFunctionsMap::Function> LuaTypeFunctionsMap::getFunction(const std::string_view & k) const {
        auto p = static_cast<const LuaTypeFunctionsMapPrivate*>(mmm_private.get());
        return p->getFunction(k);
    }

    VirtualBasic::~VirtualBasic() {}

    namespace {

        class LuaTypeFunctionsMapInner : public LuaTypeFunctionsMap {
        public:
            using LuaTypeFunctionsMap::clear;
        };

        class ClassItemInformation : public VirtualBasic {
        public:
            LuaKeyString className;
            LuaKeyUnsignedInteger classIndex;
            std::shared_ptr<LuaTypeFunctionsMapInner> classFunctions;
            std::type_index stdTypeIndex{ invalidStdTypeIndex() };
        };

        class ClassItemInformations : public VirtualBasic {
        public:
            std::shared_mutex mutex;
            vector<std::unique_ptr<ClassItemInformation>> items;
            unordered_map<std::string_view, LuaKeyUnsignedInteger> dataMap;
            unordered_map<std::type_index, LuaKeyUnsignedInteger> stdDataMap;
            std::optional<LuaKeyUnsignedInteger> getRegisterIndex(const std::string_view arg) {
                std::shared_lock varReadLock{ mutex };
                auto varPos = dataMap.find(arg);
                if (varPos == dataMap.end()) { return {}; }
                return varPos->second;
            }

            ClassItemInformations() {
                /*set zero to invalid type index*/
                constexpr const static auto invalid_class_name = "::::::invalid_type_index"sv;
                this->appendRegisterTypeIndex(invalid_class_name);
                this->attachStdTypeIndex(invalid_class_name, invalidStdTypeIndex());
            }

            static ClassItemInformations * instance() {
                static auto varAns = std::make_shared<ClassItemInformations>();
                return varAns.get();
            }

            std::optional<std::string_view> getRegisterName(LuaKeyUnsignedInteger arg) {
                std::shared_lock varReadLock{ mutex };
                if (arg < items.size()) {
                    return items[arg]->className;
                }
                return{};
            }

            std::optional<std::type_index> getRegisterStdTypeIndex(LuaKeyUnsignedInteger arg) {
                std::shared_lock varReadLock{ mutex };
                if (arg < items.size()) {
                    return items[arg]->stdTypeIndex;
                }
                return{};
            }

            LuaKeyUnsignedInteger appendRegisterTypeIndex(const std::string_view arg) {
                {
                    std::shared_lock varReadLock{ mutex };
                    auto varPos = dataMap.find(arg);
                    if (varPos != dataMap.end()) {
                        return varPos->second;
                    }
                }
                {
                    std::unique_lock varWriteLock{ mutex };
                    auto varPos = dataMap.find(arg);
                    if (varPos != dataMap.end()) {
                        return varPos->second;
                    }
                    auto varData = make_unique<ClassItemInformation>();
                    auto varDataPointer = varData.get();
                    items.push_back(std::move(varData));
                    varDataPointer->className = arg;
                    varDataPointer->classIndex = items.size() - 1;
                    varDataPointer->classFunctions = make_shared<LuaTypeFunctionsMapInner>();
                    dataMap.emplace(varDataPointer->className, varDataPointer->classIndex);
                    return varDataPointer->classIndex;
                }
            }

            std::shared_ptr<LuaTypeFunctionsMap> getRegisterFunctionMap(LuaKeyUnsignedInteger arg) {
                std::shared_lock varReadLock{ mutex };
                if (arg < items.size()) {
                    return items[arg]->classFunctions;
                }
                return{};
            }

            void attachStdTypeIndex(const std::string_view n, const std::type_index i) {
                const auto varIndex = appendRegisterTypeIndex(n);
                std::unique_lock varWriteLock{ mutex };
                items[varIndex]->stdTypeIndex = i;
                stdDataMap.erase(i);
                stdDataMap.emplace(i, varIndex);
            }

            void removeStdTypeIndex(const std::type_index i) {
                std::unique_lock varWriteLock{ mutex };
                auto pos = stdDataMap.find(i);
                if (pos == stdDataMap.end()) { return; }
                items[pos->second]->stdTypeIndex = invalidStdTypeIndex();
                items[pos->second]->classFunctions->clear();
                stdDataMap.erase(pos);
            }

            std::optional<LuaKeyUnsignedInteger> getRegisterIndex(const std::type_index arg) {
                std::shared_lock varReadLock{ mutex };
                auto pos = stdDataMap.find(arg);
                if (pos == stdDataMap.end()) { return {}; }
                return pos->second;
            }

        };

    }

    std::optional<LuaKeyUnsignedInteger> getRegisterIndex(const std::string_view arg) {
        return ClassItemInformations::instance()->getRegisterIndex(arg);
    }

    LuaKeyUnsignedInteger setRegisterTypeIndex(const std::string_view arg) {
        return ClassItemInformations::instance()->appendRegisterTypeIndex(arg);
    }

    std::optional<std::string_view> getRegisterName(LuaKeyUnsignedInteger arg) {
        return ClassItemInformations::instance()->getRegisterName(arg);
    }

    std::optional<std::type_index> getRegisterStdTypeIndex(LuaKeyUnsignedInteger arg) {
        return ClassItemInformations::instance()->getRegisterStdTypeIndex(arg);
    }

    std::shared_ptr<LuaTypeFunctionsMap> getRegisterFunctionMap(LuaKeyUnsignedInteger arg) {
        return ClassItemInformations::instance()->getRegisterFunctionMap(arg);
    }

    void attachStdTypeIndex(const std::string_view x, const std::type_index y) {
        return ClassItemInformations::instance()->attachStdTypeIndex(x, y);
    }

    void removeStdTypeIndex(const std::type_index c) {
        return ClassItemInformations::instance()->removeStdTypeIndex(c);
    }

    std::optional<LuaKeyUnsignedInteger> getRegisterIndex(const std::type_index c) {
        return ClassItemInformations::instance()->getRegisterIndex(c);
    }

    std::type_index invalidStdTypeIndex() {
        const static std::type_index varAns{ typeid(invalid_type) };
        return varAns;
    }

}/*namespace sstd*/























