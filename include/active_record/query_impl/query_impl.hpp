#pragma once
#include "../model.hpp"
#include "../query.hpp"
#include "../attribute.hpp"
#include "../adaptor.hpp"
#include "../utils.hpp"
#include <variant>
#include <any>

namespace active_record {
    template<Tuple BindAttrs>
    struct query_relation_common {
    public:
        using str_or_bind = std::variant<active_record::string, size_t>;
    private:
        template<std::derived_from<adaptor> Adaptor>
        struct str_or_bind_visitor {
            const BindAttrs bind_attrs;
            active_record::string operator()(const active_record::string& str) const {
                return str;
            }
            active_record::string operator()(const size_t idx) const {
                if constexpr (Adaptor::bindable) return Adaptor::bind_variable_str(idx);
                else {
                    if constexpr (std::is_same_v<std::tuple<void_attribute*>, BindAttrs>){
                        return "";
                    }
                    else{
                        return std::apply(
                            []<typename... Attrs>(const Attrs*... attrs){
                                return std::array<active_record::string, std::tuple_size_v<decltype(bind_attrs)>>{ to_string<Adaptor>(*attrs)... };
                            },
                            bind_attrs
                        )[idx];
                    }
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

        void* bind_attr_ptr(size_t index);
        constexpr size_t bind_attrs_count() const {
            if constexpr(std::is_same_v<void_attribute*, decltype(std::get<0>(bind_attrs))>){
                return 0;
            }
            else{
                return std::tuple_size_v<decltype(bind_attrs)>;
            }
        }

        query_relation_common(){}
        query_relation_common(const BindAttrs attrs) : bind_attrs(attrs){  }

        template<std::derived_from<adaptor> Adaptor = common_adaptor>
        [[nodiscard]] const active_record::string to_sql() const {
            if (operation == query_operation::create_table) {
                return active_record::string{"CREATE TABLE IF NOT EXISTS "} + sob_to_string<Adaptor>(query_table)
                    + "(" + sob_to_string<Adaptor>(query_op_arg) + ");";
            }
            else if (operation == query_operation::select) {
                return active_record::string{"SELECT "} + sob_to_string<Adaptor>(query_op_arg)
                    + " FROM " + sob_to_string<Adaptor>(query_table)
                    + (query_condition.empty() ? "": (active_record::string{" WHERE "} + sob_to_string<Adaptor>(query_condition)))
                    + sob_to_string<Adaptor>(query_options) + ";";
            }
            else if (operation == query_operation::insert) {
                return active_record::string{"INSERT INTO "} + sob_to_string<Adaptor>(query_table)
                    + " VALUES " + sob_to_string<Adaptor>(query_op_arg) + ";";
            }
            else if (operation == query_operation::destroy) {
                return active_record::string{"DELETE FROM "} + sob_to_string<Adaptor>(query_table)
                    + " WHERE " + sob_to_string<Adaptor>(query_condition) + ";";
            }
            else if (operation == query_operation::update) {
                return active_record::string{"UPDATE "} + sob_to_string<Adaptor>(query_table)
                    + " SET " + sob_to_string<Adaptor>(query_op_arg)
                    + (query_condition.empty() ? "": (active_record::string{" WHERE "} + sob_to_string<Adaptor>(query_condition))) + ";";
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

    template<typename T, Tuple BindAttrs>
    struct query_relation : public query_relation_common<BindAttrs> {
        using query_relation_common<BindAttrs>::query_relation_common;
    };

    template<Container Result, Tuple BindAttrs>
    requires std::derived_from<typename Result::value_type, model<typename Result::value_type>>
    struct query_relation<Result, BindAttrs> : public query_relation_common<BindAttrs> {
        using query_relation_common<BindAttrs>::query_relation_common;
        /*
        query_relation<bool> update();
        query_relation<bool> destroy();

        query_relation<bool> count() {
            return query_relation<bool>();
        }
        template<Attribute... Cols>
        query_relation<std::tuple<Cols...>> sum([[maybe_unused]] Cols... cols);

        template<Attribute... Col>
        static query_relation<std::vector<std::tuple<Col...>>> select([[maybe_unused]] Col... cols);
        
        template<Attribute Col>
        static query_relation<std::vector<Col>> pluck([[maybe_unused]] Col);

        template<Attribute Col>
        query_relation<Result> order([[maybe_unused]] Col, active_record::order);
        query_relation<Result> limit(std::size_t);

        template<Attribute... Cols>
        query_relation<Result> where([[maybe_unused]] Cols... cols);
        */
    };

    template<Container Result, Tuple BindAttrs>
    requires Tuple<typename Result::value_type>
    struct query_relation<Result, BindAttrs> : public query_relation_common<BindAttrs> {
        using query_relation_common<BindAttrs>::query_relation_common;
        /*
        template<typename Relation>
        query_relation<Result> join([[maybe_unused]] Relation);
        
        template<Attribute Col>
        query_relation<Result> order([[maybe_unused]] Col, active_record::order);
        query_relation<Result> limit(std::size_t);

        template<std::derived_from<query_relation_common>... Relations>
        query_relation<Result> where([[maybe_unused]] Relations... relations);
        */
    };

    template<typename T>
    query_relation<T, std::tuple<void_attribute*>> raw_query(const active_record::string_view query_str) {
        query_relation<T, std::tuple<void_attribute*>> ret;
        ret.operation     = query_operation::unspecified,
        ret.query_op_arg.push_back(active_record::string{ query_str });

        return ret;
    }
}