#include <string>
#include <map>
#include <vector>
#include <mutex>
#include <unordered_map>
#include <shared_mutex>
#include <memory_resource>
#include "lua_cpp_register.hpp"

namespace sstd {

    using string = std::string;
    template<typename T, typename ... Args>
    std::shared_ptr<T> make_shared(Args && ... args) {
        return std::make_shared<T>(std::forward<Args>(args)...);
    }

    template<typename T, typename ...Args>
    std::unique_ptr<T> make_unique(Args && ... args) {
        return std::make_unique<T>(std::forward<Args>(args)...);
    }

    template<typename K, typename V>
    using map = std::map<K, V, std::less<void>>;

    template<typename T>
    using vector = std::vector<T>;

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

        };

    }/**/

    LuaTypeFunctionsMap::LuaTypeFunctionsMap() :mmm_private(make_unique<LuaTypeFunctionsMapPrivate>()) {

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

        class ClassItemInformation : public VirtualBasic {
        public:
            LuaKeyString className;
            LuaKeyUnsignedInteger classIndex;
            std::shared_ptr<LuaTypeFunctionsMap> classFunctions;
        };

        class ClassItemInformations : public VirtualBasic {
        public:
            std::shared_mutex mutex;
            vector<std::unique_ptr<ClassItemInformation>> items;
            unordered_map<std::string_view, LuaKeyUnsignedInteger> dataMap;

            std::optional<LuaKeyUnsignedInteger> getRegisterIndex(const std::string_view arg) {
                std::shared_lock varReadLock{ mutex };
                auto varPos = dataMap.find(arg);
                if (varPos == dataMap.end()) { return {}; }
                return varPos->second;
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
                    varDataPointer->classFunctions = make_shared<LuaTypeFunctionsMap>();
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

    std::shared_ptr<LuaTypeFunctionsMap> getRegisterFunctionMap(LuaKeyUnsignedInteger arg) {
        return ClassItemInformations::instance()->getRegisterFunctionMap(arg);
    }

}/*namespace sstd*/























