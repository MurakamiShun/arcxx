#pragma once
/*
 * Active Record C++: https://github.com/akisute514/active_record_cpp
 * Copyright (c) 2021 akisute514
 * 
 * Released under the MIT Lisence.
 */
#include "../query.hpp"

namespace active_record {
    template<specialized_from<std::tuple> BindAttrs>
    struct query_condition {
    private:
        enum class conjunction{
            AND,
            OR
        };

        template<specialized_from<std::tuple> SrcBindAttrs>
        [[nodiscard]] query_condition<tuptup::tuple_cat_t<BindAttrs, SrcBindAttrs>> concat_conditions(query_condition<SrcBindAttrs>&&, const conjunction);
    public:
        using str_or_bind = std::variant<active_record::string, std::size_t>;
        std::vector<str_or_bind> condition;
        BindAttrs bind_attrs;

        [[nodiscard]] static consteval std::size_t bind_attrs_count() noexcept {
            return std::tuple_size_v<BindAttrs>;
        }

        template<specialized_from<std::tuple> DestBindAttrs>
        auto operator&&(query_condition<DestBindAttrs>&& cond) && {
            return concat_conditions(std::move(cond), conjunction::AND);
        }
        template<specialized_from<std::tuple> DestBindAttrs>
        auto operator||(query_condition<DestBindAttrs>&& cond) && {
            return concat_conditions(std::move(cond), conjunction::OR);
        }

        template<Attribute Attr>
        auto operator&&(const Attr& cond) && {
            return this->operator&&(Attr::cmp == cond);
        }
        template<Attribute Attr>
        auto operator||(const Attr& cond) && {
            return this->operator||(Attr::cmp == cond);
        }
    };
}

#include "query_condition_impl.ipp"