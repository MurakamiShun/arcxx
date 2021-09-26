#pragma once
#include "../model.hpp"
#include "../query.hpp"
#include "../attribute.hpp"
#include "../adaptor.hpp"
#include "../utils.hpp"
#include "query_condition.hpp"

namespace active_record {
    template<Tuple BindAttrs>
    struct query_relation_common {
    public:
        using str_or_bind = std::variant<active_record::string, std::size_t>;
    private:
        template<std::derived_from<adaptor> Adaptor>
        struct sob_to_string_impl;
    public:
        const query_operation operation;
        std::vector<str_or_bind> op_args;
        std::vector<str_or_bind> tables;
        std::vector<str_or_bind> conditions;
        std::vector<str_or_bind> options; // order, limit

        BindAttrs bind_attrs;

        [[nodiscard]] static consteval std::size_t bind_attrs_count() noexcept {
            return std::tuple_size_v<BindAttrs>;
        }

        query_relation_common(const query_operation op) :
            operation(op) {
        }

        template<std::derived_from<adaptor> Adaptor = common_adaptor>
        [[nodiscard]] const active_record::string to_sql() const {
            sob_to_string_impl<Adaptor> convertor{ bind_attrs };
            if (operation == query_operation::select) {
                return concat_strings("SELECT ", convertor.to_string(op_args),
                    " FROM ", convertor.to_string(tables),
                    conditions.empty() ? "" : concat_strings(" WHERE ", convertor.to_string(conditions)),
                    " ", convertor.to_string(options), ";"
                );
            }
            else if (operation == query_operation::insert) {
                return concat_strings("INSERT INTO ", convertor.to_string(tables),
                    " VALUES ", convertor.to_string(op_args), ";"
                );
            }
            else if (operation == query_operation::destroy) {
                return concat_strings("DELETE FROM ", convertor.to_string(tables),
                    conditions.empty() ? "" : concat_strings(" WHERE ", convertor.to_string(conditions)), ";"
                );
            }
            else if (operation == query_operation::update) {
                return concat_strings("UPDATE ", convertor.to_string(tables),
                    " SET ", convertor.to_string(op_args),
                    conditions.empty() ? "": concat_strings(" WHERE ", convertor.to_string(conditions)), ";"
                );
            }
            else if (operation == query_operation::condition) {
                return concat_strings("SELECT ", convertor.to_string(op_args),
                    " FROM ", convertor.to_string(tables),
                    " WHERE ", convertor.to_string(conditions),
                    convertor.to_string(options), ";"
                );
            }
            else {
                return concat_strings(convertor.to_string(op_args), ";");
            }
        }
    };

    /*
     * Clang and MSVC has this bug.
     * https://bugs.llvm.org/show_bug.cgi?id=48020
     */

    template<Tuple BindAttrs>
    struct query_relation<bool, BindAttrs> : public query_relation_common<BindAttrs> {
        using query_relation_common<BindAttrs>::query_relation_common;
        template<std::derived_from<adaptor> Adaptor>
        auto exec(Adaptor& adapt) const {
            return adapt.exec(*this);
        }
    };

    template<typename Result, Tuple BindAttrs>
    struct query_relation : public query_relation_common<BindAttrs> {
        using query_relation_common<BindAttrs>::query_relation_common;
        template<std::derived_from<adaptor> Adaptor>
        [[nodiscard]] auto exec(Adaptor& adapt) const {
            return adapt.exec(*this);
        }

        template<Attribute Attr>
        [[nodiscard]] query_relation<Result, tuptup::tuple_cat_t<BindAttrs, std::tuple<Attr>>> where(const Attr&) &&;
        template<Attribute Attr>
        [[nodiscard]] query_relation<Result, tuptup::tuple_cat_t<BindAttrs, std::tuple<Attr>>> where(const Attr&) const &;

        template<Tuple SrcBindAttrs>
        [[nodiscard]] query_relation<Result, tuptup::tuple_cat_t<BindAttrs, SrcBindAttrs>> where(query_condition<SrcBindAttrs>&&) &&;
        template<Tuple SrcBindAttrs>
        [[nodiscard]] query_relation<Result, tuptup::tuple_cat_t<BindAttrs, SrcBindAttrs>> where(query_condition<SrcBindAttrs>&&) const&;
    };

    template<typename Result, Tuple BindAttrs>
    requires std::same_as<Result, std::vector<typename Result::value_type>>
    struct query_relation<Result, BindAttrs> : public query_relation_common<BindAttrs> {
        using query_relation_common<BindAttrs>::query_relation_common;
        template<std::derived_from<adaptor> Adaptor>
        [[nodiscard]] auto exec(Adaptor& adapt) const {
            return adapt.exec(*this);
        }

        template<Attribute... Attrs>
        [[nodiscard]] query_relation<std::vector<std::tuple<Attrs...>>, BindAttrs> select() &&;
        template<Attribute... Attrs>
        [[nodiscard]] query_relation<std::vector<std::tuple<Attrs...>>, BindAttrs> select() const &;

        template<AttributeAggregator... Attrs>
        [[nodiscard]] query_relation<std::tuple<typename Attrs::attribute_type...>, BindAttrs> select() &&;
        template<AttributeAggregator... Attrs>
        [[nodiscard]] query_relation<std::tuple<typename Attrs::attribute_type...>, BindAttrs> select() const &;

        template<Attribute Attr>
        [[nodiscard]] query_relation<std::vector<Attr>, BindAttrs> pluck() &&;
        template<Attribute Attr>
        [[nodiscard]] query_relation<std::vector<Attr>, BindAttrs> pluck() const &;

        template<Attribute... Attrs>
        requires Model<typename Result::value_type>
        [[nodiscard]] query_relation<bool, tuptup::tuple_cat_t<BindAttrs, std::tuple<Attrs...>>> update(const Attrs&...);

        template<Attribute Attr>
        [[nodiscard]] query_relation<Result, tuptup::tuple_cat_t<BindAttrs, std::tuple<Attr>>> where(const Attr&) &&;
        template<Attribute Attr>
        [[nodiscard]] query_relation<Result, tuptup::tuple_cat_t<BindAttrs, std::tuple<Attr>>> where(const Attr&) const &;

        template<Tuple SrcBindAttrs>
        [[nodiscard]] query_relation<Result, tuptup::tuple_cat_t<BindAttrs, SrcBindAttrs>> where(query_condition<SrcBindAttrs>&&) &&;
        template<Tuple SrcBindAttrs>
        [[nodiscard]] query_relation<Result, tuptup::tuple_cat_t<BindAttrs, SrcBindAttrs>> where(query_condition<SrcBindAttrs>&&) const&;

        [[nodiscard]] query_relation<Result, BindAttrs>& limit(const std::size_t) &&;
        [[nodiscard]] query_relation<Result, BindAttrs> limit(const std::size_t) const &;

        template<Attribute Attr>
        [[nodiscard]] query_relation<Result, BindAttrs>& order_by(const active_record::order = active_record::order::asc) &&;
        template<Attribute Attr>
        [[nodiscard]] query_relation<Result, BindAttrs> order_by(const active_record::order = active_record::order::asc) const &;

        [[nodiscard]] query_relation<std::size_t, BindAttrs> count() &&;
        [[nodiscard]] query_relation<std::size_t, BindAttrs> count() const&;

        template<Attribute Attr>
        requires requires{ typename Attr::sum; }
        [[nodiscard]] query_relation<typename Attr::sum::attribute_type, BindAttrs> sum() &&;
        template<Attribute Attr>
        requires requires{ typename Attr::sum; }
        [[nodiscard]] query_relation<typename Attr::sum::attribute_type, BindAttrs> sum() const&;

        template<Attribute Attr>
        requires requires{ typename Attr::avg; }
        [[nodiscard]] query_relation<typename Attr::avg::attribute_type, BindAttrs> avg() &&;
        template<Attribute Attr>
        requires requires{ typename Attr::avg; }
        [[nodiscard]] query_relation<typename Attr::avg::attribute_type, BindAttrs> avg() const&;

        template<Attribute Attr>
        requires requires{ typename Attr::max; }
        [[nodiscard]] query_relation<typename Attr::max::attribute_type, BindAttrs> max() &&;
        template<Attribute Attr>
        requires requires{ typename Attr::max; }
        [[nodiscard]] query_relation<typename Attr::max::attribute_type, BindAttrs> max() const&;

        template<Attribute Attr>
        requires requires{ typename Attr::min; }
        [[nodiscard]] query_relation<typename Attr::min::attribute_type, BindAttrs> min() &&;
        template<Attribute Attr>
        requires requires{ typename Attr::min; }
        [[nodiscard]] query_relation<typename Attr::min::attribute_type, BindAttrs> min() const&;
    };


    template<typename Result, Tuple BindAttrs>
    requires std::same_as<Result, std::unordered_map<typename Result::key_type, typename Result::mapped_type>>
    struct query_relation<Result, BindAttrs> : public query_relation_common<BindAttrs> {
        using mapped_type = typename Result::mapped_type;

        using query_relation_common<BindAttrs>::query_relation_common;

        template<std::derived_from<adaptor> Adaptor>
        [[nodiscard]] auto exec(Adaptor& adapt) const {
            return adapt.exec(*this);
        }

        template<AttributeAggregator... Attrs>
        [[nodiscard]] query_relation<std::unordered_map<typename Result::key_type, std::tuple<typename Attrs::attribute_type...>>, BindAttrs> select() &&;
        template<AttributeAggregator... Attrs>
        [[nodiscard]] query_relation<std::unordered_map<typename Result::key_type, std::tuple<typename Attrs::attribute_type...>>, BindAttrs> select() const &;

        template<AttributeAggregator Attr>
        [[nodiscard]] query_relation<std::unordered_map<typename Result::key_type, typename Attr::attribute_type>, BindAttrs> pluck() &&;
        template<AttributeAggregator Attr>
        [[nodiscard]] query_relation<std::unordered_map<typename Result::key_type, typename Attr::attribute_type>, BindAttrs> pluck() const &;

        template<Attribute Attr>
        [[nodiscard]] query_relation<Result, tuptup::tuple_cat_t<BindAttrs, std::tuple<Attr>>> where(const Attr&) &&;
        template<Attribute Attr>
        [[nodiscard]] query_relation<Result, tuptup::tuple_cat_t<BindAttrs, std::tuple<Attr>>> where(const Attr&) const &;

        template<Tuple SrcBindAttrs>
        [[nodiscard]] query_relation<Result, tuptup::tuple_cat_t<BindAttrs, SrcBindAttrs>> where(query_condition<SrcBindAttrs>&&) &&;
        template<Tuple SrcBindAttrs>
        [[nodiscard]] query_relation<Result, tuptup::tuple_cat_t<BindAttrs, SrcBindAttrs>> where(query_condition<SrcBindAttrs>&&) const&;

        [[nodiscard]] query_relation<Result, BindAttrs> limit(const std::size_t) &&;
        [[nodiscard]] query_relation<Result, BindAttrs> limit(const std::size_t) const &;

        template<Attribute Attr>
        [[nodiscard]] query_relation<Result, BindAttrs> order_by(const active_record::order = active_record::order::asc) &&;
        template<Attribute Attr>
        [[nodiscard]] query_relation<Result, BindAttrs> order_by(const active_record::order = active_record::order::asc) const &;

        [[nodiscard]] query_relation<std::unordered_map<typename Result::key_type, std::size_t>, BindAttrs> count() &&;
        [[nodiscard]] query_relation<std::unordered_map<typename Result::key_type, std::size_t>, BindAttrs> count() const&;

        template<Attribute Attr>
        requires requires{ typename Attr::sum; }
        [[nodiscard]] query_relation<std::unordered_map<typename Result::key_type, typename Attr::sum::attribute_type>, BindAttrs> sum() &&;
        template<Attribute Attr>
        requires requires{ typename Attr::sum; }
        [[nodiscard]] query_relation<std::unordered_map<typename Result::key_type, typename Attr::sum::attribute_type>, BindAttrs> sum() const&;

        template<Attribute Attr>
        requires requires{ typename Attr::avg; }
        [[nodiscard]] query_relation<std::unordered_map<typename Result::key_type, typename Attr::avg::attribute_type>, BindAttrs> avg() &&;
        template<Attribute Attr>
        requires requires{ typename Attr::avg; }
        [[nodiscard]] query_relation<std::unordered_map<typename Result::key_type, typename Attr::avg::attribute_type>, BindAttrs> avg() const&;

        template<Attribute Attr>
        requires requires{ typename Attr::max; }
        [[nodiscard]] query_relation<std::unordered_map<typename Result::key_type, typename Attr::max::attribute_type>, BindAttrs> max() &&;
        template<Attribute Attr>
        requires requires{ typename Attr::max; }
        [[nodiscard]] query_relation<std::unordered_map<typename Result::key_type, typename Attr::max::attribute_type>, BindAttrs> max() const&;

        template<Attribute Attr>
        requires requires{ typename Attr::min; }
        [[nodiscard]] query_relation<std::unordered_map<typename Result::key_type, typename Attr::min::attribute_type>, BindAttrs> min() &&;
        template<Attribute Attr>
        requires requires{ typename Attr::min; }
        [[nodiscard]] query_relation<std::unordered_map<typename Result::key_type, typename Attr::min::attribute_type>, BindAttrs> min() const&;
    };
}