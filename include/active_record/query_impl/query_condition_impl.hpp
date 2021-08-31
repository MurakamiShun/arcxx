#pragma once
#include "query_condition.hpp"
#include "query_utils.hpp"

namespace active_record {
    /*
     * implements
     */
    template<Tuple BindAttrs>
    template<Tuple SrcBindAttrs>
    query_condition<active_record::tuple_cat_t<BindAttrs, SrcBindAttrs>> query_condition<BindAttrs>::concat_conditions(query_condition<SrcBindAttrs>&& cond, const conjunction conjunc) {
        query_condition<active_record::tuple_cat_t<BindAttrs, SrcBindAttrs>> ret;
        ret.condition.reserve(this->condition.size() + 3 + cond.condition.size());

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

        struct {
                decltype(ret.condition)& ret_cond;
                void operator()(active_record::string&& str) { ret_cond.push_back(std::move(str)); }
                void operator()(std::size_t idx) { ret_cond.push_back(idx + std::tuple_size_v<BindAttrs>); }
        } visitor { ret.condition };
        for(auto& cond : cond.condition){
            std::visit(visitor, std::move(cond));
        }
        ret.condition.push_back(")");

        ret.temporary_attrs.insert(
            ret.temporary_attrs.begin(),
            std::make_move_iterator(this->temporary_attrs.begin()),
            std::make_move_iterator(this->temporary_attrs.end())
        );
        ret.temporary_attrs.insert(
            ret.temporary_attrs.end(),
            std::make_move_iterator(cond.temporary_attrs.begin()),
            std::make_move_iterator(cond.temporary_attrs.end())
        );
        detail::set_bind_attrs_ptr<0>(ret.bind_attrs, ret.temporary_attrs);

        return ret;
    }
}