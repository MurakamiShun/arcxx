#pragma once
#include "../model.hpp"
#include "../query.hpp"
#include "../attribute.hpp"
#include "../adaptor.hpp"
#include "../utils.hpp"

namespace active_record{
    struct query_operation_common {
        const query_operation operation;
        const active_record::string query_op_arg;
        const active_record::string query_table;
        const active_record::string query_condition;
        // order, limit
        const active_record::string query_options;
        template<std::derived_from<adaptor> Adaptor = common_adaptor>
        [[nodiscard]] const active_record::string to_sql() const {
            if (operation == query_operation::create_table) {
                return active_record::string{"CREATE TABLE IF NOT EXISTS "} + query_table + "(" + query_op_arg + ");";
            }
            else if (operation == query_operation::select) {
                return active_record::string{"SELECT "} + query_op_arg + " FROM " + query_table + (query_condition.empty() ? "": (" WHERE " + query_condition)) + query_options + ";";
            }
            else if (operation == query_operation::insert) {
                return active_record::string{"INSERT INTO "} + query_table + " VALUES " + query_op_arg + ";";
            }
            else if (operation == query_operation::destroy) {
                return active_record::string{"DELETE FROM "} + query_table + " WHERE " + query_condition + ";";
            }
            else if (operation == query_operation::update) {
                return active_record::string{"UPDATE "} + query_table + " SET " + query_op_arg + (query_condition.empty() ? "": (" WHERE " + query_condition)) + ";";
            }
            else if (operation == query_operation::condition) {
                return active_record::string{"SELECT "} + query_op_arg + " FROM " + query_table + " WHERE " + query_condition + query_options + ";";
            }
            else {
                return query_op_arg + ";";
            }
        }
    };

    template<typename T>
    struct query_relation : public query_operation_common {
    };

    template<Container Result>
    requires std::derived_from<typename Result::value_type, model<typename Result::value_type>>
    struct query_relation<Result> : public query_operation_common {
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
    };

    template<Container Result>
    requires Tuple<typename Result::value_type>
    struct query_relation<Result> : public query_operation_common {
        template<typename Relation>
        query_relation<Result> join([[maybe_unused]] Relation);
        
        template<Attribute Col>
        query_relation<Result> order([[maybe_unused]] Col, active_record::order);
        query_relation<Result> limit(std::size_t);

        template<std::derived_from<query_operation_common>... Relations>
        query_relation<Result> where([[maybe_unused]] Relations... relations);
    };

    template<typename T>
    query_relation<T> raw_query(const active_record::string_view query_str) {
        return query_relation<T>{{
            .operation       = query_operation::unspecified,
            .query_op_arg    = active_record::string{ query_str },
            .query_table     = "",
            .query_condition = "",
            .query_options   = ""
        }};
    }
}