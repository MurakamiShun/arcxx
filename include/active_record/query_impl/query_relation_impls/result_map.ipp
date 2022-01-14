#pragma once
/*
 * Active Record C++: https://github.com/akisute514/active_record_cpp
 * Copyright (c) 2021 akisute514
 * 
 * Released under the MIT License.
 */
namespace active_record {
    /*
     * return type == std::unordered_map<is_attribute, specialized_from<std::tuple> or Attribute>
     */
    template<typename Result, specialized_from<std::tuple> BindAttrs>
    template<is_attribute_aggregator... Attrs>
    inline auto query_relation<Result, BindAttrs>::select() const& requires specialized_from<Result, std::unordered_map>{
        query_relation<std::unordered_map<typename Result::key_type, std::tuple<typename Attrs::attribute_type...>>, BindAttrs> ret{ query_operation::select };
        ret.op_args.push_back(detail::column_full_names_to_string<typename Result::key_type, Attrs...>());
        ret.tables = this->tables;

        ret.conditions = this->conditions;
        ret.options = this->options;
        ret.bind_attrs = this->bind_attrs;
        return ret;
    }
    template<typename Result, specialized_from<std::tuple> BindAttrs>
    template<is_attribute_aggregator... Attrs>
    inline auto query_relation<Result, BindAttrs>::select() && requires specialized_from<Result, std::unordered_map>{
        query_relation<std::unordered_map<typename Result::key_type, std::tuple<typename Attrs::attribute_type...>>, BindAttrs> ret{ query_operation::select };
        ret.op_args.push_back(detail::column_full_names_to_string<typename Result::key_type, Attrs...>());
        ret.tables = std::move(this->tables);

        ret.conditions = std::move(this->conditions);
        ret.options = std::move(this->options);
        ret.bind_attrs = std::move(this->bind_attrs);
        return ret;
    }

    template<typename Result, specialized_from<std::tuple> BindAttrs>
    template<is_attribute_aggregator Attr>
    inline auto query_relation<Result, BindAttrs>::pluck() const& requires specialized_from<Result, std::unordered_map>{
        query_relation<std::unordered_map<typename Result::key_type, typename Attr::attribute_type>, BindAttrs> ret{ query_operation::select };
        ret.op_args.push_back(detail::column_full_names_to_string<typename Result::key_type, Attr>());
        ret.tables = this->tables;

        ret.conditions = this->conditions;
        ret.options = this->options;
        ret.bind_attrs = this->bind_attrs;
        return ret;
    }
    template<typename Result, specialized_from<std::tuple> BindAttrs>
    template<is_attribute_aggregator Attr>
    inline auto query_relation<Result, BindAttrs>::pluck() && requires specialized_from<Result, std::unordered_map>{
        query_relation<std::unordered_map<typename Result::key_type, typename Attr::attribute_type>, BindAttrs> ret{ query_operation::select };
        ret.op_args.push_back(detail::column_full_names_to_string<typename Result::key_type, Attr>());
        ret.tables = std::move(this->tables);

        ret.conditions = std::move(this->conditions);
        ret.options = std::move(this->options);
        ret.bind_attrs = std::move(this->bind_attrs);
        return ret;
    }

    template<typename Result, specialized_from<std::tuple> BindAttrs>
    template<is_attribute Attr>
    inline auto query_relation<Result, BindAttrs>::where(const Attr& attr) && requires specialized_from<Result, std::unordered_map>{
        return std::move(*this).where(Attr::cmp == attr);
    }
    template<typename Result, specialized_from<std::tuple> BindAttrs>
    template<is_attribute Attr>
    inline auto query_relation<Result, BindAttrs>::where(const Attr& attr) const& requires specialized_from<Result, std::unordered_map>{
        return this->where(Attr::cmp == attr);
    }

    template<typename Result, specialized_from<std::tuple> BindAttrs>
    template<specialized_from<std::tuple> SrcBindAttrs>
    inline auto query_relation<Result, BindAttrs>::where(query_condition<SrcBindAttrs>&& cond) && requires specialized_from<Result, std::unordered_map>{
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
    inline auto query_relation<Result, BindAttrs>::where(query_condition<SrcBindAttrs>&& cond) const& requires specialized_from<Result, std::unordered_map>{
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

    template<typename Result, specialized_from<std::tuple> BindAttrs>
    inline auto query_relation<Result, BindAttrs>::limit(const std::size_t lim) && requires specialized_from<Result, std::unordered_map>{
        this->options.push_back(concat_strings(" LIMIT ", std::to_string(lim)));
        return *this;
    }
    template<typename Result, specialized_from<std::tuple> BindAttrs>
    inline auto query_relation<Result, BindAttrs>::limit(const std::size_t lim) const& requires specialized_from<Result, std::unordered_map>{
        query_relation<Result, BindAttrs> ret{ this->operation };
        ret.op_args = this->op_args;
        ret.tables = this->tables;
        ret.conditions = this->conditions;
        ret.options = this->options;
        ret.options.push_back(concat_strings(" LIMIT ", std::to_string(lim)));
        ret.bind_attrs = this->bind_attrs;

        return ret;
    }

    template<typename Result, specialized_from<std::tuple> BindAttrs>
    template<is_attribute Attr>
    inline auto query_relation<Result, BindAttrs>::order_by(const active_record::order order) && requires specialized_from<Result, std::unordered_map>{
        this->options.push_back(concat_strings(
            " ORDER BY ", detail::column_full_names_to_string<Attr>(),
            order == active_record::order::asc ? " ASC" : " DESC"
        ));

        return *this;
    }
    template<typename Result, specialized_from<std::tuple> BindAttrs>
    template<is_attribute Attr>
    inline auto query_relation<Result, BindAttrs>::order_by(const active_record::order order) const& requires specialized_from<Result, std::unordered_map>{
        query_relation<Result, BindAttrs> ret{ this->operation };
        ret.op_args = this->op_args;
        ret.tables = this->tables;
        ret.conditions = this->conditions;
        ret.bind_attrs = this->bind_attrs;
        ret.options = this->options;

        ret.options.push_back(concat_strings(
            " ORDER BY ", detail::column_full_names_to_string<Attr>(),
            order == active_record::order::asc ? " ASC" : " DESC"
        ));

        return ret;
    }

    template<typename Result, specialized_from<std::tuple> BindAttrs>
    inline auto query_relation<Result, BindAttrs>::count() && requires specialized_from<Result, std::unordered_map>{
        query_relation<std::unordered_map<typename Result::key_type, std::size_t>, BindAttrs> ret{ query_operation::select };
        ret.op_args.push_back(detail::column_full_names_to_string<typename Result::key_type>() + ",count(*)");
        ret.tables = std::move(this->tables);

        ret.conditions = std::move(this->conditions);
        ret.options = std::move(this->options);
        ret.bind_attrs = std::move(this->bind_attrs);
        return ret;
    }
    template<typename Result, specialized_from<std::tuple> BindAttrs>
    inline auto query_relation<Result, BindAttrs>::count() const& requires specialized_from<Result, std::unordered_map>{
        query_relation<std::unordered_map<typename Result::key_type, std::size_t>, BindAttrs> ret{ query_operation::select };
        ret.op_args.push_back(detail::column_full_names_to_string<typename Result::key_type>() + ",count(*)");
        ret.tables = this->tables;

        ret.conditions = this->conditions;
        ret.options = this->options;
        ret.bind_attrs = this->bind_attrs;
        return ret;
    }

    template<typename Result, specialized_from<std::tuple> BindAttrs>
    template<is_attribute Attr>
    requires requires{ typename Attr::sum; }
    inline auto query_relation<Result, BindAttrs>::sum() && requires specialized_from<Result, std::unordered_map>{
        return std::move(*this).template pluck<typename Attr::sum>();
    }

    template<typename Result, specialized_from<std::tuple> BindAttrs>
    template<is_attribute Attr>
    requires requires{ typename Attr::sum; }
    inline auto query_relation<Result, BindAttrs>::sum() const& requires specialized_from<Result, std::unordered_map>{
        return this->pluck<typename Attr::sum>();
    }

    template<typename Result, specialized_from<std::tuple> BindAttrs>
    template<is_attribute Attr>
    requires requires{ typename Attr::avg; }
    inline auto query_relation<Result, BindAttrs>::avg() && requires specialized_from<Result, std::unordered_map>{
        return std::move(*this).template pluck<typename Attr::avg>();
    }

    template<typename Result, specialized_from<std::tuple> BindAttrs>
    template<is_attribute Attr>
    requires requires{ typename Attr::avg; }
    inline auto query_relation<Result, BindAttrs>::avg() const& requires specialized_from<Result, std::unordered_map>{
        return this->pluck<typename Attr::avg>();
    }

    template<typename Result, specialized_from<std::tuple> BindAttrs>
    template<is_attribute Attr>
    requires requires{ typename Attr::max; }
    inline auto query_relation<Result, BindAttrs>::max() && requires specialized_from<Result, std::unordered_map>{
        return std::move(*this).template pluck<typename Attr::max>();
    }

    template<typename Result, specialized_from<std::tuple> BindAttrs>
    template<is_attribute Attr>
    requires requires{ typename Attr::max; }
    inline auto query_relation<Result, BindAttrs>::max() const& requires specialized_from<Result, std::unordered_map>{
        return this->pluck<typename Attr::max>();
    }

    template<typename Result, specialized_from<std::tuple> BindAttrs>
    template<is_attribute Attr>
    requires requires{ typename Attr::min; }
    inline auto query_relation<Result, BindAttrs>::min() && requires specialized_from<Result, std::unordered_map>{
        return std::move(*this).template pluck<typename Attr::min>();
    }

    template<typename Result, specialized_from<std::tuple> BindAttrs>
    template<is_attribute Attr>
    requires requires{ typename Attr::min; }
    inline auto query_relation<Result, BindAttrs>::min() const& requires specialized_from<Result, std::unordered_map>{
        return this->pluck<typename Attr::min>();
    }

}