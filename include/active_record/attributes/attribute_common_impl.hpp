#pragma once
#include "../attribute.hpp"
#include "../query.hpp"

namespace active_record {
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

        template<std::size_t I, typename Condition, std::convertible_to<Attribute> Last>
        static void copy_and_set_attrs_to_condition(Condition& ret, const Last& last){
            ret.temporary_attrs.push_back(Attribute{ last });
            std::get<I>(ret.bind_attrs) = std::any_cast<Attribute>(&(ret.temporary_attrs.back()));
            ret.condition.push_back(I);
        }

        template<std::size_t I, typename Condition, std::convertible_to<Attribute> Head, std::convertible_to<Attribute>... Tails>
        static void copy_and_set_attrs_to_condition(Condition& ret, const Head& head, const Tails&... tails){
            copy_and_set_attrs_to_condition<I+1>(ret, head);
            ret.condition.push_back(",");
            copy_and_set_attrs_to_condition<I+1>(ret, tails...);
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
        inline static const constraint unique = [](const std::optional<Type>&) constexpr { return true; };
        inline static const constraint primary_key = [](const std::optional<Type>& t) { return not_null(t) && unique(t); };
        struct constraint_default_value_impl{
            const Type default_value;
            constexpr bool operator()(const std::optional<Type>&){ return true; }
        };
        static const constraint default_value(const Type& def_val) {
            return constraint_default_value_impl{ def_val };
        }

        static bool has_constraint(const constraint& c){
            if constexpr (!has_constraints) return false;
            else{
                for(const auto& con : Attribute::constraints){
                    if(con.target_type() == c.target_type()) return true;
                }
                return false;
            }
        }
        static const std::optional<constraint> get_constraint(const constraint& c){
            if constexpr (!has_constraints) return std::nullopt;
            else{
                for(const auto& con : Attribute::constraints){
                    if(con.target_type() == c.target_type()) return con;
                }
                return std::nullopt;
            }
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

        template<std::convertible_to<Attribute>... Attrs>
        static auto in(const Attrs&... values) {
            query_condition<std::tuple<const decltype(values, std::declval<Attribute>())*...>> ret;
            ret.condition.push_back(
                active_record::string{ "\"" } + Model::table_name + "\".\""
                + Attribute::column_name + "\" in ("
            );
            copy_and_set_attrs_to_condition(ret, values...);
            ret.condition.push_back(")");
            return ret;
        }
    };
}