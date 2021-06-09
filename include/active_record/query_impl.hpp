#pragma once
#include "model.hpp"
#include "query.hpp"
#include "attribute.hpp"
#include "utils.hpp"

namespace active_record{
    enum class query_operation {
        unspecified,
        select,
        insert,
        destroy,
        update,
        condition
    };

    enum class order {
        asc,
        desc
    };

    template<typename T>
    concept Column = std::derived_from<T, attribute<typename T::model_type, typename T::attribute_type, typename T::value_type>>;

    struct query_operation_common {
        const query_operation operation;
        const active_record::string query_op_arg;
        const active_record::string query_table;
        const active_record::string query_condition;
        // order, limit
        const active_record::string query_options;
        [[nodiscard]] const active_record::string to_sql() const {
            if (operation == query_operation::select) {
                return active_record::string("SELECT ") + query_op_arg + " FROM " + query_table + " WHERE " + query_condition + query_options + ";";
            }
            else if (operation == query_operation::insert) {
                return active_record::string("INSERT INTO ") + query_table + " VALUES " + query_op_arg + ";";
            }
            else if (operation == query_operation::destroy) {
                return active_record::string("DELETE FROM ") + query_table + " WHERE " + query_condition + ";";
            }
            else if (operation == query_operation::update) {
                return active_record::string("UPDATE ") + query_table + " SET " + query_op_arg + " WHERE " + query_condition + ";";
            }
            else if (operation == query_operation::condition) {
                return active_record::string("SELECT ") + query_op_arg + " FROM " + query_table + " WHERE " + query_condition + query_options + ";";
            }
            else {
                return active_record::string("SELECT ") + query_op_arg + " FROM " + query_table + " WHERE " + query_condition + query_options + ";";
            }
        }
        query_operation_common(const query_operation op, active_record::string&& op_arg, active_record::string&& table, active_record::string&& condition, active_record::string&& options) :
            query_op_arg(std::move(op_arg)),
            query_table(std::move(table)),
            query_condition(std::move(condition)),
            query_options(std::move(options)),
            operation(op) {
        }
        query_operation_common(const query_operation op, const active_record::string& op_arg, const active_record::string& table, const active_record::string& condition, const active_record::string& options) :
            query_op_arg(op_arg),
            query_table(table),
            query_condition(condition),
            query_options(options),
            operation(op) {
        }
    };

    template<typename T>
    struct query_relation : public query_operation_common {
        using query_operation_common::query_operation_common;
    };

    template<Container Result>
    requires std::derived_from<typename Result::value_type, model<typename Result::value_type>>
    struct query_relation<Result> : public query_operation_common {
        using query_operation_common::query_operation_common;

        query_relation<bool> update();
        query_relation<bool> destroy();

        query_relation<bool> count() {
            return query_relation<bool>();
        }
        template<Column... Cols>
        query_relation<std::tuple<Cols...>> sum([[maybe_unused]] Cols... cols);

        template<Column... Col>
        static query_relation<std::vector<std::tuple<Col...>>> select([[maybe_unused]] Col... cols);
        
        template<Column Col>
        static query_relation<std::vector<Col>> pluck([[maybe_unused]] Col);

        template<Column Col>
        query_relation<Result> order([[maybe_unused]] Col, active_record::order);
        query_relation<Result> limit(std::size_t);

        template<Column... Cols>
        query_relation<Result> where([[maybe_unused]] Cols... cols);
    };

    template<Container Result>
    requires Tuple<typename Result::value_type>
    struct query_relation<Result> : public query_operation_common {
        using query_operation_common::query_operation_common;

        template<typename Relation>
        query_relation<Result> join([[maybe_unused]] Relation);
        
        template<Column Col>
        query_relation<Result> order([[maybe_unused]] Col, active_record::order);
        query_relation<Result> limit(std::size_t);

        template<std::derived_from<query_operation_common>... Relations>
        query_relation<Result> where([[maybe_unused]] Relations... relations);
    };

    namespace{
        template<typename Derived>
        constexpr active_record::string values_to_string(const Derived& src) {
            const auto value_strings = src.to_strings();
            active_record::string result = "(";
            active_record::string delimiter = "";
            for (const auto& value : value_strings) {
                result += delimiter +"\'" + value + "\'";
                delimiter = ",";
            }
            result += ")";
            return result;
        }

        template<typename Derived, typename... Tail>
        constexpr active_record::string values_to_string(const Derived& src, const Tail&... tail) {
            const auto first = values_to_string(src);
            return first + "," + values_to_string(tail...);
        }
    }

    template<typename Derived>
    template<Container C>
    inline query_relation<bool> model<Derived>::insert(const C& models) {
        auto column_names = Derived::column_names();
        active_record::string table = active_record::string("\"") + Derived::table_name + "\"" + "(";
        active_record::string delimiter = "";
        for (auto& col_name : column_names) {
            table += delimiter + "\"" + active_record::string(col_name) + "\"";
            delimiter = ",";
        }
        table += ")";

        active_record::string values;
        active_record::string value_delimiter = "";
        for (const auto& src : models) {
            values += value_delimiter + values_to_string(src);
            value_delimiter = ",";
        }
        return query_relation<bool> {
            query_operation::insert,
                std::move(values),
                std::move(table),
                active_record::string(""),
                active_record::string("")
        };
    }

    template<typename Derived>
    template<std::same_as<Derived>... Models>
    inline query_relation<bool> model<Derived>::insert(const Models&... models) {
        auto column_names = Derived::column_names();
        active_record::string table = active_record::string("\"") + Derived::table_name + "\"" + "(";
        active_record::string delimiter = "";
        for (auto& col_name : column_names) {
            table += delimiter + "\"" + active_record::string(col_name) + "\"";
            delimiter = ",";
        }
        table += ")";

        const auto values = values_to_string(models...);
        return query_relation<bool> {
            query_operation::insert,
                std::move(values),
                std::move(table),
                active_record::string(""),
                active_record::string("")
        };
    }
}