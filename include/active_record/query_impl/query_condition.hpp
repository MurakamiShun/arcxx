#pragma once
#include "../query.hpp"
#include "../attribute.hpp"

namespace active_record {
    template<Tuple BindAttrs>
    struct query_condition {
    private:
        enum class conjunction{
            AND,
            OR
        };
        
        template<Tuple DestBindAttrs>
        query_condition<std::invoke_result_t<decltype(std::tuple_cat<BindAttrs, DestBindAttrs>)>> concat_conditions(query_condition<DestBindAttrs>&& cond, const conjunction);
    public:
        using str_or_bind = std::variant<active_record::string, std::size_t>;
        std::vector<str_or_bind> condition;
        BindAttrs bind_attrs;
        std::vector<std::any> temporary_attrs;

        static constexpr std::size_t bind_attrs_count() {
            return std::tuple_size_v<BindAttrs>;
        }

        template<Tuple DestBindAttrs>
        query_condition<std::invoke_result_t<decltype(std::tuple_cat<BindAttrs, DestBindAttrs>)>> operator||(query_condition<DestBindAttrs>&& cond) && {
            return concat_conditions(std::forward(cond), conjunction::OR);
        }
        template<Tuple DestBindAttrs>
        query_condition<std::invoke_result_t<decltype(std::tuple_cat<BindAttrs, DestBindAttrs>)>> operator&&(query_condition<DestBindAttrs>&& cond) && {
            return concat_conditions(std::forward(cond), conjunction::AND);
        }

        template<Attribute Attr>
        auto operator&&(const Attr& cond){
            return this->operator&&(cond.to_equ_condition());
        }
        template<Attribute Attr>
        auto operator||(const Attr& cond){
            return this->operator||(cond.to_equ_condition());
        }
    };
}