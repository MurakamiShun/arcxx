#pragma once
#include "query_condition.hpp"

namespace active_record {
    /*
     * implements
     */
    template<Tuple BindAttrs>
    template<Tuple DestBindAttrs>
    query_condition<std::invoke_result_t<decltype(std::tuple_cat<BindAttrs, DestBindAttrs>)>> query_condition<BindAttrs>::concat_conditions(query_condition<DestBindAttrs>&& cond, const conjunction conjunc) {
        query_condition<std::invoke_result_t<decltype(std::tuple_cat<BindAttrs, DestBindAttrs>)>> ret;
        
        this->condition.push_back("(");
        ret.condition = std::move(this->condition);
        switch (conjunc) {
        case conjunction::AND:
            ret.condition.push_back(" AND ");
            break;
        case conjunction::OR:
        default:
            ret.condition.push_back(" OR ");
            break;
        }
        ret.condition.insert(
            ret.condition.end(),
            std::make_move_iterator(cond.condition.begin()),
            std::make_move_iterator(cond.condition.end())
        );
        ret.condition.push_back(")");
        ret.temporary_attrs = std::move(this->temporary_attrs);
        ret.temporary_attrs.insert(
            ret.temporary_attrs,
            std::make_move_iterator(cond.temporary_attrs.begin()),
            std::make_move_iterator(cond.temporary_attrs.end())
        );
        detail::set_bind_attrs_ptr<0>(ret.bind_attrs, ret.temporary_attrs);

        return ret;
    }
}