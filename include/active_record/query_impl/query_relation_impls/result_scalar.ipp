#pragma once
/*
 * Active Record C++: https://github.com/akisute514/active_record_cpp
 * Copyright (c) 2021 akisute514
 * 
 * Released under the MIT Lisence.
 */
namespace active_record {
    /*
     * return type == Scalar
     */
    template<typename Result, specialized_from<std::tuple> BindAttrs>
    template<is_attribute Attr>
    inline auto query_relation<Result, BindAttrs>::where(const Attr& attr) && {
        return std::move(*this).where(Attr::cmp == attr);
    }
    template<typename Result, specialized_from<std::tuple> BindAttrs>
    template<is_attribute Attr>
    inline auto query_relation<Result, BindAttrs>::where(const Attr& attr) const& {
        return this->where(Attr::cmp == attr);
    }

    template<typename Result, specialized_from<std::tuple> BindAttrs>
    template<specialized_from<std::tuple> SrcBindAttrs>
    inline auto query_relation<Result, BindAttrs>::where(query_condition<SrcBindAttrs>&& cond) &&{
        query_relation<Result, tuptup::tuple_cat_t<BindAttrs, SrcBindAttrs>> ret{ this->operation };

        ret.op_args = std::move(this->op_args);
        ret.tables = std::move(this->tables);

        if(!this->conditions.empty()){
            ret.conditions = std::move(this->conditions);
            ret.conditions.push_back(" AND ");
        }
        for(auto& cond : cond.condition){
            visit_by_lambda(std::move(cond),
                [&ret_cond = ret.conditions](active_record::string&& str) { ret_cond.push_back(std::move(str)); },
                [&ret_cond = ret.conditions](const std::size_t idx) { ret_cond.push_back(idx + std::tuple_size_v<BindAttrs>); }
            );
        }

        ret.options = std::move(this->options);
        ret.bind_attrs = std::tuple_cat(std::move(this->bind_attrs), std::move(cond.bind_attrs));
        return ret;
    }

    template<typename Result, specialized_from<std::tuple> BindAttrs>
    template<specialized_from<std::tuple> SrcBindAttrs>
    inline auto query_relation<Result, BindAttrs>::where(query_condition<SrcBindAttrs>&& cond) const&{
        query_relation<Result, tuptup::tuple_cat_t<BindAttrs, SrcBindAttrs>> ret{ this->operation };

        ret.op_args = this->op_args;
        ret.tables = this->tables;

        if(!this->conditions.empty()){
            ret.conditions = this->conditions;
            ret.conditions.push_back(" AND ");
        }

        for(auto& cond : cond.condition){
            visit_by_lambda(std::move(cond),
                [&ret_cond = ret.conditions](active_record::string&& str) { ret_cond.push_back(std::move(str)); },
                [&ret_cond = ret.conditions](const std::size_t idx) { ret_cond.push_back(idx + std::tuple_size_v<BindAttrs>); }
            );
        }

        ret.options = this->options;
        ret.bind_attrs = std::tuple_cat(this->bind_attrs, std::move(cond.bind_attrs));

        return ret;
    }
}