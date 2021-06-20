#pragma once
#include <optional>
#include <utility>
#include <typeinfo>
#include "utils.hpp"
#include "adaptor.hpp"

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

        struct has_constraints_impl {
            template<typename S>
            static decltype(S::constraints, std::true_type{}) check(S);
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
        using constraint = std::function<bool(const std::optional<Type>&)>;
        using model_type = Model;
        using attribute_type = Attribute;
        using value_type = Type;

        static constexpr bool has_column_name = has_column_name_impl::value;
        static constexpr bool has_constraints = has_constraints_impl::value;
        [[nodiscard]] static constexpr std::pair<active_record::string_view, active_record::string_view> column_full_name() noexcept {
            return { Model::table_name, Attribute::column_name };
        };

        // constexpr std::type_info::operator== is C++23
        inline static const constraint not_null = [](const std::optional<Type>& t) constexpr { return static_cast<bool>(t); };
        inline static const constraint unique = [](const std::optional<Type>& t) constexpr { return true; };
        inline static const constraint primary_key = [](const std::optional<Type>& t) { return not_null(t) && unique(t); };
        struct constraint_default_value_impl{
            const Type default_value;
            constexpr bool operator()(const std::optional<Type>&){ return true; }
        };
        static const constraint default_value(const Type& def_val) {
            return constraint_default_value_impl{ def_val };
        }

        static const bool has_constraint(const constraint& c){
            if (!has_constraints) return false;
            for(const auto& con : Attribute::constraints){
                if(con.target_type() == c.target_type()) return true;
            }
            return false;
        }
        static const constraint& get_constraint(const constraint& c){
            if (!has_constraints) return nullptr;
            for(const auto& con : Attribute::constraints){
                if(con.target_type() == c.target_type()) return con;
            }
            return nullptr;
        }

        inline static const bool is_primary_key =  [](){
            return has_constraint(primary_key);
        }();

        constexpr attribute_common() {}
        constexpr attribute_common(const std::optional<Type>& default_value) : data(default_value) {}
        constexpr attribute_common(std::optional<Type>&& default_value) : data(std::move(default_value)) {}
        constexpr attribute_common(std::nullopt_t) : data(std::nullopt) {}
        constexpr attribute_common(const Type& default_value) : data(default_value) {}
        constexpr attribute_common(Type&& default_value) : data(std::move(default_value)) {}
        constexpr virtual ~attribute_common() {}

        virtual active_record::string to_string() const = 0;
        static active_record::string column_statement_on_create_table(const adaptor&);
        
        [[nodiscard]] constexpr bool is_valid() const {
            if constexpr (has_constraints) {
                for (const auto& val : Attribute::constraints) {
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