#pragma once
#include <functional>
#include <optional>
#include "model.hpp"
#include "query.hpp"
#include "utils.hpp"

namespace active_record {
    constexpr static auto unspecified = std::nullopt;
    
    template<typename Model, typename Attribute, typename Type>
    class attribute {
    private:
        struct has_column_name_impl {
            template<typename S>
            static decltype(S::column_name, std::true_type{}) check(S);
            static std::false_type check(...);
            static constexpr bool value = std::invoke_result_t<decltype(&has_column_name_impl::check), Attribute>::value;
        };

        struct has_validators_impl {
            template<typename S>
            static decltype(S::validators, std::true_type{}) check(S);
            static std::false_type check(...);
            static constexpr bool value = std::invoke_result_t<decltype(&has_validators_impl::check), Attribute>::value;
        };
    protected:
        std::optional<Type> data;
    public:
        using validator = std::function<bool(const std::optional<Type>&)>;
        using model_type = Model;
        using attribute_type = Attribute;
        using value_type = Type;

        static constexpr bool has_column_name = has_column_name_impl::value;
        static constexpr bool has_validators = has_validators_impl::value;
        [[nodiscard]] static constexpr std::pair<active_record::string_view, active_record::string_view> column_full_name() noexcept {
            return { Model::table_name, Attribute::column_name };
        };

        inline static const validator not_null = [](const std::optional<Type>& t) constexpr { return static_cast<bool>(t); };

        constexpr attribute() {}
        constexpr attribute(const std::optional<Type>& default_value) : data(default_value) {}
        constexpr attribute(std::optional<Type>&& default_value) : data(std::move(default_value)) {}
        constexpr attribute(std::nullopt_t) : data(std::nullopt) {}
        constexpr attribute(const Type& default_value) : data(default_value) {}
        constexpr attribute(Type&& default_value) : data(std::move(default_value)) {}
        constexpr virtual ~attribute() {}

        //virtual void from_string(const database::string_view str) = 0;
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
    };
}