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

        template<Tuple SrcBindAttrs>
        [[nodiscard]] query_condition<active_record::tuple_cat_t<BindAttrs, SrcBindAttrs>> concat_conditions(query_condition<SrcBindAttrs>&&, const conjunction);
    public:
        using str_or_bind = std::variant<active_record::string, std::size_t>;
        std::vector<str_or_bind> condition;
        BindAttrs bind_attrs;

        [[nodiscard]] static consteval std::size_t bind_attrs_count() noexcept {
            return std::tuple_size_v<BindAttrs>;
        }

        template<Tuple DestBindAttrs>
        auto operator&&(query_condition<DestBindAttrs>&& cond) && {
            return concat_conditions(std::move(cond), conjunction::AND);
        }
        template<Tuple DestBindAttrs>
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