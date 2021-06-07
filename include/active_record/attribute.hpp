#pragma once
#include <functional>
#include "model.hpp"
#include "query.hpp"
#include "utils.hpp"

namespace active_record {
    template<typename Model, typename Attribute, typename Type>
    struct attribute {
    protected:
        std::optional<Type> data;
        using validator = std::function<bool(const Type&)>;
        struct has_column_name_impl {
            template<typename S>
            static decltype(S::column_name, std::true_type()) check(S) {}
            static std::false_type check(...) {}
            static constexpr bool value = decltype(check(std::declval<Attribute>()))::value;
        };

        struct has_validators_impl {
            template<typename S>
            static decltype(S::validators, std::true_type()) check(S) {}
            static std::false_type check(...) {}
            static constexpr bool value = decltype(check(std::declval<Attribute>()))::value;
        };

    public:
        using model_type = Model;
        using attribute_type = Attribute;
        using value_type = Type;

        static constexpr bool has_column_name = has_column_name_impl::value;
        static constexpr bool has_validators = has_validators_impl::value;
        static constexpr std::pair<active_record::string_view, active_record::string_view> column_full_name() {
            return { Model::table_name, Attribute::column_name };
        };
        constexpr attribute() {}
        constexpr attribute(const std::optional<Type>& default_value) : data(default_value) {}
        constexpr attribute(std::optional<Type>&& default_value) : data(std::move(default_value)) {}
        constexpr attribute(std::nullopt_t) : data(std::nullopt) {}
        constexpr attribute(const Type& default_value) : data(default_value) {}
        constexpr attribute(Type&& default_value) : data(std::move(default_value)) {}


        //virtual void from_string(const database::string_view str) = 0;
        //virtual database::string to_string() const = 0;
        
        bool is_valid() const {
            if constexpr (has_validations<Attribute>::value) {
                for (const auto& val : Attribute::validators) {
                    if (!val(data)) return false;
                }
            }
            return true;
        }
        explicit operator bool() const noexcept { return static_cast<bool>(data); }
        const Type& value() const& { return data.value(); }
        Type& value()& { return data.value(); }
        const Type&& value() const&& { return std::move(data.value()); }
        Type&& value()&& { return std::move(data.value()); }
        Type* operator->() { return data.operator->(); }
        const Type* operator->() const { return data.operator->(); }
    };

    namespace attributes {
        constexpr static auto unspecified = std::nullopt;

        template<typename Model, typename Attribute>
        struct string : public attribute<Model, Attribute, active_record::string> {
            using attribute<Model, Attribute, active_record::string>::attribute;
        };

        template<typename Model, typename Attribute>
        struct integer : public attribute<Model, Attribute, int32_t> {
            using attribute<Model, Attribute, int32_t>::attribute;
        };

        template<typename Model, typename Attribute>
        struct decimal : public attribute<Model, Attribute, float> {
            using attribute<Model, Attribute, float>::attribute;
        };

        template<typename Model, typename Attribute>
        struct boolean : public attribute<Model, Attribute, bool> {
            using attribute<Model, Attribute, bool>::attribute;
        };

        template<typename Model, typename Attribute>
        struct datetime : public attribute<Model, Attribute, active_record::datetime> {
            using attribute<Model, Attribute, active_record::datetime>::attribute;
        };

        template<typename Model, typename Attribute>
        struct binary : public attribute<Model, Attribute, std::vector<std::byte>> {
            using attribute<Model, Attribute, std::vector<std::byte>>::attribute;
        };
    };
}