#pragma once
/*
 * Active Record C++: https://github.com/akisute514/active_record_cpp
 * Copyright (c) 2021 akisute514
 * 
 * Released under the MIT Lisence.
 */
namespace active_record {
    /*
     * implements
     */
    template<specialized_from<std::tuple> BindAttrs>
    template<specialized_from<std::tuple> SrcBindAttrs>
    query_condition<tuptup::tuple_cat_t<BindAttrs, SrcBindAttrs>> query_condition<BindAttrs>::concat_conditions(query_condition<SrcBindAttrs>&& cond, const conjunction conjunc) {
        query_condition<tuptup::tuple_cat_t<BindAttrs, SrcBindAttrs>> ret;
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

        ret.bind_attrs = std::tuple_cat(std::move(this->bind_attrs), std::move(cond.bind_attrs));

        return ret;
    }
}