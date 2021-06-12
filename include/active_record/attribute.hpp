#pragma once
#include <optional>
#include <utility>
#include "utils.hpp"

namespace active_record {
    constexpr static auto unspecified = std::nullopt;

    template<typename Model, typename Attribute, typename Type>
    struct attribute;
    
    template<typename Model, typename Attribute, typename Type>
    class attribute_common {
    private:
        struct has_column_name_impl {
            template<typename S>
            static decltype(S::column_name, std::true_type{}) check(S);
            static std::false_type check(...);
            static constexpr bool value = decltype(check(std::declval<Attribute>()))::value;
        };

        struct has_validators_impl {
            template<typename S>
            static decltype(S::validators, std::true_type{}) check(S);
            static std::false_type check(...);
            static constexpr bool value = decltype(check(std::declval<Attribute>()))::value;
        };

        static constexpr active_record::string attribute_values_to_string(const Attribute& attr){
            return attr.to_string();
        }
        template<std::same_as<Attribute>... Tail>
        static constexpr active_record::string attribute_values_to_string(const Attribute& head, const Tail&... tail){
            return attribute_values_to_string(head) + "," + attribute_values_to_string(tail...);
        }
    protected:
        std::optional<Type> data;
    public:
        using validator = bool(*)(const std::optional<Type>&);
        using model_type = Model;
        using attribute_type = Attribute;
        using value_type = Type;

        static constexpr bool has_column_name = has_column_name_impl::value;
        static constexpr bool has_validators = has_validators_impl::value;
        [[nodiscard]] static constexpr std::pair<active_record::string_view, active_record::string_view> column_full_name() noexcept {
            return { Model::table_name, Attribute::column_name };
        };

        static constexpr validator not_null = [](const std::optional<Type>& t) constexpr { return static_cast<bool>(t); };
        static constexpr validator unique = [](const std::optional<Type>& t) constexpr { return true; };
        static constexpr validator primary_key = [](const std::optional<Type>& t) constexpr { return not_null(t) && unique(t); };
        static constexpr validator auto_increment = [](const std::optional<Type>& t) constexpr { return not_null(t) && unique(t); };
        static constexpr validator default_value(){
            return [](const std::optional<Type>& t) constexpr { return true; };
        }

        inline static const bool is_primary_key =  has_validators && !(std::find(Attribute::validators.begin(), Attribute::validators.end(), primary_key) == Attribute::validators.end());

        constexpr attribute_common() {}
        constexpr attribute_common(const std::optional<Type>& default_value) : data(default_value) {}
        constexpr attribute_common(std::optional<Type>&& default_value) : data(std::move(default_value)) {}
        constexpr attribute_common(std::nullopt_t) : data(std::nullopt) {}
        constexpr attribute_common(const Type& default_value) : data(default_value) {}
        constexpr attribute_common(Type&& default_value) : data(std::move(default_value)) {}
        constexpr virtual ~attribute_common() {}

        virtual active_record::string to_string() const = 0;
        
        [[nodiscard]] constexpr bool is_valid() const {
            if constexpr (has_validators) {
                for (const auto& val : Attribute::validators) {
                    if (!val(data)) return false;
                }
            }
            return true;
        }
        constexpr operator bool() const noexcept { return static_cast<bool>(data); }
        [[nodiscard]] const Type& value() const& { return data.value(); }
        [[nodiscard]] Type& value()& { return data.value(); }
        [[nodiscard]] const Type&& value() const&& { return std::move(data.value()); }
        [[nodiscard]] Type&& value()&& { return std::move(data.value()); }
        [[nodiscard]] Type* operator->() { return data.operator->(); }
        [[nodiscard]] const Type* operator->() const { return data.operator->(); }

        template<std::convertible_to<Attribute>... Attrs>
        static constexpr query_condition in(const Attrs&... values) {
            return query_condition{
                active_record::string{ "\"" } + Model::table_name + "\".\"" + Attribute::column_name
                + "\" in (" + attribute_values_to_string(Attribute{ values }...) + ")"
            };
        }
    };

    template<typename T>
    concept Attribute = std::derived_from<T, attribute_common<typename T::model_type, typename T::attribute_type, typename T::value_type>>;
}