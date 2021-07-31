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
        struct str_or_bind_visitor {
            const BindAttrs bind_attrs;
            active_record::string operator()(const active_record::string& str) const {
                return str;
            }
            active_record::string operator()(const std::size_t idx) const {
                if constexpr (Adaptor::bindable) return Adaptor::bind_variable_str(idx);
                else {
                    return std::apply(
                        []<typename... Attrs>(const Attrs*... attrs){
                            return std::array<active_record::string, std::tuple_size_v<BindAttrs>>{ active_record::to_string<Adaptor>(*attrs)... };
                        },
                        bind_attrs
                    )[idx];
                }
            }
        };

        template<std::derived_from<adaptor> Adaptor>
        active_record::string sob_to_string(const std::vector<str_or_bind>& sobs) const {
            active_record::string result;
            str_or_bind_visitor<Adaptor> visitor{ bind_attrs };
            for(const auto& sob : sobs) {
                result += std::visit(visitor, sob);
            }
            return result;
        }
    public:        
        query_operation operation;
        std::vector<str_or_bind> query_op_arg;
        std::vector<str_or_bind> query_table;
        std::vector<str_or_bind> query_condition;
        std::vector<str_or_bind> query_options; // order, limit
        
        BindAttrs bind_attrs;
        std::vector<std::any> temporary_attrs;

        static constexpr std::size_t bind_attrs_count() {
            return std::tuple_size_v<BindAttrs>;
        }

        query_relation_common(){}
        query_relation_common(const BindAttrs attrs) : bind_attrs(attrs){  }

        template<std::derived_from<adaptor> Adaptor = common_adaptor>
        [[nodiscard]] const active_record::string to_sql() const {
            if (operation == query_operation::select) {
                return active_record::string{"SELECT "} + sob_to_string<Adaptor>(query_op_arg)
                    + " FROM " + sob_to_string<Adaptor>(query_table)
                    + (query_condition.empty() ? "" : (active_record::string{" WHERE "} + sob_to_string<Adaptor>(query_condition)))
                    + " " + sob_to_string<Adaptor>(query_options) + ";";
            }
            else if (operation == query_operation::insert) {
                return active_record::string{"INSERT INTO "} + sob_to_string<Adaptor>(query_table)
                    + " VALUES " + sob_to_string<Adaptor>(query_op_arg) + ";";
            }
            else if (operation == query_operation::destroy) {
                return active_record::string{"DELETE FROM "} + sob_to_string<Adaptor>(query_table)
                    + (query_condition.empty() ? "" : (active_record::string{" WHERE "} + sob_to_string<Adaptor>(query_condition)))
                    + ";";
            }
            else if (operation == query_operation::update) {
                return active_record::string{"UPDATE "} + sob_to_string<Adaptor>(query_table)
                    + " SET " + sob_to_string<Adaptor>(query_op_arg)
                    + (query_condition.empty() ? "": (active_record::string{" WHERE "} + sob_to_string<Adaptor>(query_condition)))
                    + ";";
            }
            else if (operation == query_operation::condition) {
                return active_record::string{"SELECT "} + sob_to_string<Adaptor>(query_op_arg)
                    + " FROM " + sob_to_string<Adaptor>(query_table)
                    + " WHERE " + sob_to_string<Adaptor>(query_condition)
                    + sob_to_string<Adaptor>(query_options) + ";";
            }
            else {
                return sob_to_string<Adaptor>(query_op_arg) + ";";
            }
        }
    };

    template<Tuple BindAttrs>
    struct query_relation<bool, BindAttrs> : public query_relation_common<BindAttrs> {
        using query_relation_common<BindAttrs>::query_relation_common;
        template<std::derived_from<adaptor> Adaptor>
        decltype(auto) exec(Adaptor& adapt) const {
            return adapt.exec(*this);
        }
    };

    template<typename Result, Tuple BindAttrs>
    struct query_relation : public query_relation_common<BindAttrs> {
        using query_relation_common<BindAttrs>::query_relation_common;
        template<std::derived_from<adaptor> Adaptor>
        decltype(auto) exec(Adaptor& adapt) const {
            return adapt.exec(*this);
        }

        template<Attribute Attr>
        query_relation<Result, std::invoke_result_t<decltype(std::tuple_cat<BindAttrs, std::tuple<const Attr*>>), BindAttrs, std::tuple<const Attr*>>> where(const Attr&&) &&;
        template<Attribute Attr>
        query_relation<Result, std::invoke_result_t<decltype(std::tuple_cat<BindAttrs, std::tuple<const Attr*>>), BindAttrs, std::tuple<const Attr*>>> where(const Attr&&) const &;

        template<Tuple SrcBindAttrs>
        query_relation<Result, std::invoke_result_t<decltype(std::tuple_cat<BindAttrs, SrcBindAttrs>), BindAttrs, SrcBindAttrs>> where(query_condition<SrcBindAttrs>&&) &&;
        template<Tuple SrcBindAttrs>
        query_relation<Result, std::invoke_result_t<decltype(std::tuple_cat<BindAttrs, SrcBindAttrs>), BindAttrs, SrcBindAttrs>> where(query_condition<SrcBindAttrs>&&) const&;
    };

    template<Container Result, Tuple BindAttrs>
    struct query_relation<Result, BindAttrs> : public query_relation_common<BindAttrs> {
        using query_relation_common<BindAttrs>::query_relation_common;
        template<std::derived_from<adaptor> Adaptor>
        decltype(auto) exec(Adaptor& adapt) const {
            return adapt.exec(*this);
        }
        
        template<Attribute... Attrs>
        query_relation<std::vector<std::tuple<Attrs...>>, BindAttrs> select() &&;
        template<Attribute... Attrs>
        query_relation<std::vector<std::tuple<Attrs...>>, BindAttrs> select() const &;

        template<Attribute Attr>
        query_relation<std::vector<Attr>, BindAttrs> pluck() &&;
        template<Attribute Attr>
        query_relation<std::vector<Attr>, BindAttrs> pluck() const &;

        template<Attribute... Attrs>
        requires Model<typename Result::value_type>
        query_relation<bool, std::invoke_result_t<decltype(std::tuple_cat<BindAttrs, std::tuple<const Attrs*...>>), BindAttrs, std::tuple<const Attrs*...>>> update(const Attrs...);
        
        template<Attribute Attr>
        query_relation<Result, std::invoke_result_t<decltype(std::tuple_cat<BindAttrs, std::tuple<const Attr*>>), BindAttrs, std::tuple<const Attr*>>> where(const Attr&&) &&;
        template<Attribute Attr>
        query_relation<Result, std::invoke_result_t<decltype(std::tuple_cat<BindAttrs, std::tuple<const Attr*>>), BindAttrs, std::tuple<const Attr*>>> where(const Attr&&) const &;

        template<Tuple SrcBindAttrs>
        query_relation<Result, std::invoke_result_t<decltype(std::tuple_cat<BindAttrs, SrcBindAttrs>), BindAttrs, SrcBindAttrs>> where(query_condition<SrcBindAttrs>&&) &&;
        template<Tuple SrcBindAttrs>
        query_relation<Result, std::invoke_result_t<decltype(std::tuple_cat<BindAttrs, SrcBindAttrs>), BindAttrs, SrcBindAttrs>> where(query_condition<SrcBindAttrs>&&) const&;

        query_relation<Result, BindAttrs>& limit(const std::size_t) &&;
        query_relation<Result, BindAttrs> limit(const std::size_t) const &;

        template<Attribute Attr>
        query_relation<Result, BindAttrs>& order_by(const active_record::order = active_record::order::asc) &&;
        template<Attribute Attr>
        query_relation<Result, BindAttrs> order_by(const active_record::order = active_record::order::asc) const &;

        query_relation<std::size_t, BindAttrs> count() &&;
        query_relation<std::size_t, BindAttrs> count() const&;

        template<Attribute Attr>
        requires std::integral<typename Attr::value_type> || std::floating_point<typename Attr::value_type>
        query_relation<typename Attr::value_type, BindAttrs> sum() &&;
        template<Attribute Attr>
        requires std::integral<typename Attr::value_type> || std::floating_point<typename Attr::value_type>
        query_relation<typename Attr::value_type, BindAttrs> sum() const&;

        template<Attribute Attr>
        requires std::integral<typename Attr::value_type> || std::floating_point<typename Attr::value_type>
        query_relation<typename Attr::value_type, BindAttrs> avg() &&;
        template<Attribute Attr>
        requires std::integral<typename Attr::value_type> || std::floating_point<typename Attr::value_type>
        query_relation<typename Attr::value_type, BindAttrs> avg() const&;

        template<Attribute Attr>
        requires std::integral<typename Attr::value_type> || std::floating_point<typename Attr::value_type>
        query_relation<typename Attr::value_type, BindAttrs> max() &&;
        template<Attribute Attr>
        requires std::integral<typename Attr::value_type> || std::floating_point<typename Attr::value_type>
        query_relation<typename Attr::value_type, BindAttrs> max() const&;

        template<Attribute Attr>
        requires std::integral<typename Attr::value_type> || std::floating_point<typename Attr::value_type>
        query_relation<typename Attr::value_type, BindAttrs> min() &&;
        template<Attribute Attr>
        requires std::integral<typename Attr::value_type> || std::floating_point<typename Attr::value_type>
        query_relation<typename Attr::value_type, BindAttrs> min() const&;
    };
}