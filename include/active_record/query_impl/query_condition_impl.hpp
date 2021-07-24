#pragma once
#include "query_condition.hpp"
#include "query_utils.hpp"

namespace active_record {
    /*
     * implements
     */
    template<Tuple BindAttrs>
    template<Tuple SrcBindAttrs>
    query_condition<std::invoke_result_t<decltype(std::tuple_cat<BindAttrs, SrcBindAttrs>), BindAttrs, SrcBindAttrs>> query_condition<BindAttrs>::concat_conditions(query_condition<SrcBindAttrs>&& cond, const conjunction conjunc) {
        query_condition<std::invoke_result_t<decltype(std::tuple_cat<BindAttrs, SrcBindAttrs>), BindAttrs, SrcBindAttrs>> ret;
        
        ret.condition.push_back("(");
        ret.condition.insert(
            ret.condition.end(),
            std::make_move_iterator(this->condition.begin()),
            std::make_move_iterator(this->condition.end())
        );
        switch (conjunc) {
        case conjunction::AND:
            ret.condition.push_back(" AND ");
            break;
        case conjunction::OR:
        default:
            ret.condition.push_back(" OR ");
            break;
        }
        for(auto& cond : cond.condition){
            struct {
                str_or_bind operator()(active_record::string&& str){ return std::move(str); }
                str_or_bind operator()(std::size_t idx){ return idx + std::tuple_size_v<BindAttrs>; }
            } visitor;
            
            ret.condition.push_back(
                std::visit(visitor, std::move(cond))
            );
        }
        ret.condition.push_back(")");
        ret.temporary_attrs = std::move(this->temporary_attrs);
        ret.temporary_attrs.insert(
            ret.temporary_attrs.end(),
            std::make_move_iterator(cond.temporary_attrs.begin()),
            std::make_move_iterator(cond.temporary_attrs.end())
        );
        detail::set_bind_attrs_ptr<0>(ret.bind_attrs, ret.temporary_attrs);

        return ret;
    }
}