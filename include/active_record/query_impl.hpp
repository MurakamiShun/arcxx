#pragma once
#include "model.hpp"
#include "query.hpp"
#include "attribute.hpp"
#include "utils.hpp"

namespace active_record{
    struct query_operation_common {
        const query_operation operation;
        const active_record::string query_op_arg;
        const active_record::string query_table;
        const active_record::string query_condition;
        // order, limit
        const active_record::string query_options;
        [[nodiscard]] const active_record::string to_sql() const {
            if (operation == query_operation::select) {
                return active_record::string("SELECT ") + query_op_arg + " FROM " + query_table + (query_condition.empty() ? "": (" WHERE " + query_condition)) + query_options + ";";
            }
            else if (operation == query_operation::insert) {
                return active_record::string("INSERT INTO ") + query_table + " VALUES " + query_op_arg + ";";
            }
            else if (operation == query_operation::destroy) {
                return active_record::string("DELETE FROM ") + query_table + " WHERE " + query_condition + ";";
            }
            else if (operation == query_operation::update) {
                return active_record::string("UPDATE ") + query_table + " SET " + query_op_arg + (query_condition.empty() ? "": (" WHERE " + query_condition)) + ";";
            }
            else if (operation == query_operation::condition) {
                return active_record::string("SELECT ") + query_op_arg + " FROM " + query_table + " WHERE " + query_condition + query_options + ";";
            }
            else {
                return active_record::string("SELECT ") + query_op_arg + " FROM " + query_table + ";";
            }
        }
        query_operation_common(const query_operation op, active_record::string&& op_arg, active_record::string&& table, active_record::string&& condition, active_record::string&& options) :
            query_op_arg(std::move(op_arg)),
            query_table(std::move(table)),
            query_condition(std::move(condition)),
            query_options(std::move(options)),
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
        using query_operation_common::query_operation_common;

        template<typename Relation>
        query_relation<Result> join([[maybe_unused]] Relation);
        
        template<Attribute Col>
        query_relation<Result> order([[maybe_unused]] Col, active_record::order);
        query_relation<Result> limit(std::size_t);

        template<std::derived_from<query_operation_common>... Relations>
        query_relation<Result> where([[maybe_unused]] Relations... relations);
    };

    namespace {
        template<Model Last>
        constexpr active_record::string insert_values_to_string(const Last& src) {
            const auto value_strings = src.get_attribute_strings();
            active_record::string result = "(";
            active_record::string delimiter = "";
            for (const auto& value : value_strings) {
                result += delimiter + value;
                delimiter = ",";
            }
            result += ")";
            return result;
        }

        template<Model Head, Model... Tail>
        constexpr active_record::string insert_values_to_string(const Head& src, const Tail&... tail) {
            const auto first = insert_values_to_string(src);
            return first + "," + insert_values_to_string(tail...);
        }

        template<Model Mod>
        constexpr active_record::string insert_column_names_to_string() {
            active_record::string table = active_record::string{ "\"" } + active_record::string{ Mod::table_name } + "\"(";
            active_record::string delimiter = "";
            const auto column_names = Mod::column_names();
            for (const auto& col_name : column_names) {
                table += delimiter + "\"" + active_record::string{ col_name } + "\"";
                delimiter = ",";
            }
            table += ")";
            return table;
        }
    }

    template<typename Derived>
    template<Container C>
    inline constexpr query_relation<bool> model<Derived>::insert(const C& models) {
        active_record::string values = "";
        active_record::string value_delimiter = "";
        for (const auto& src : models) {
            values += value_delimiter + insert_values_to_string(src);
            value_delimiter = ",";
        }

        return query_relation<bool> {
            query_operation::insert,
                std::move(values),
                insert_column_names_to_string<Derived>(),
                active_record::string{ "" },
                active_record::string{ "" }
        };
    }

    template<typename Derived>
    template<std::same_as<Derived>... Models>
    inline constexpr query_relation<bool> model<Derived>::insert(const Models&... models) {
        return query_relation<bool> {
            query_operation::insert,
                insert_values_to_string(models...),
                insert_column_names_to_string<Derived>(),
                active_record::string{ "" },
                active_record::string{ "" }
        };
    }

    namespace {
        template<Attribute Last>
        constexpr active_record::string column_full_names_to_string([[maybe_unused]]const Last&){
            constexpr auto full_name = Last::column_full_name();
            return active_record::string{ "\"" } + active_record::string{ full_name.first } + "\".\"" + active_record::string{ full_name.second } + "\"";
        }
        template<Attribute Head, Attribute... Tail>
        constexpr active_record::string column_full_names_to_string([[maybe_unused]] const Head&, [[maybe_unused]] const Tail&...){
            return column_full_names_to_string(std::declval<Head>()) + "," + column_full_names_to_string(std::declval<Tail>()...);
        }

        template<Model Mod>
        constexpr active_record::string model_column_full_names_to_string(){
            auto column_names = Mod::column_names();
            active_record::string columns = "";
            active_record::string delimiter = "";
            for (auto& col_name : column_names) {
                columns += delimiter + "\"" + active_record::string{ Mod::table_name } + "\".\"" + active_record::string{ col_name } + "\"";
                delimiter = ",";
            }
            return columns;
        }
    }

    template<typename Derived>
    inline constexpr query_relation<std::vector<Derived>> model<Derived>::all() {
        return query_relation<std::vector<Derived>> {
            query_operation::unspecified,
                model_column_full_names_to_string<Derived>(),
                active_record::string{ "\"" } + active_record::string{ Derived::table_name } + "\"",
                active_record::string{ "" },
                active_record::string{ "" }
        };
    }

    template<typename Derived>
    template<Attribute... Attrs>
    inline constexpr query_relation<std::vector<std::tuple<Attrs...>>> model<Derived>::select(const Attrs... attrs) {
        return query_relation<std::vector<std::tuple<Attrs...>>> {
            query_operation::select,
                column_full_names_to_string(attrs...),
                active_record::string{ "\"" } + active_record::string{ Derived::table_name } + "\"",
                active_record::string{ "" },
                active_record::string{ "" }
        };
    }

    template<typename Derived>
    template<Attribute Attr>
    inline constexpr query_relation<std::vector<Attr>> model<Derived>::pluck(const Attr attr) {
        return query_relation<std::vector<Attr>> {
            query_operation::select,
                column_full_names_to_string(attr),
                active_record::string{ "\"" } + active_record::string{ Derived::table_name } + "\"",
                active_record::string{ "" },
                active_record::string{ "" }
        };
    }
    
    namespace {
        template<Attribute Attr>
        constexpr active_record::string attributes_to_condition_string(const Attr& attr){
            return column_full_names_to_string(attr) + " = " + attr.to_string();
        }
        
        template<std::same_as<query_condition> Condition>
        constexpr active_record::string attributes_to_condition_string(const Condition& cond){
            return cond.str;
        }
        template<WhereArgs Head, WhereArgs... Tail>
        constexpr active_record::string attributes_to_condition_string(const Head& head, const Tail&... tail){
            return attributes_to_condition_string(head) + " AND " + attributes_to_condition_string(tail...);
        }
    }

    template<typename Derived>
    template<WhereArgs... Attrs>
    inline constexpr query_relation<std::vector<Derived>> model<Derived>::where(const Attrs... attrs) {
        return query_relation<std::vector<Derived>> {
            query_operation::condition,
                model_column_full_names_to_string<Derived>(),
                active_record::string{ "\"" } + active_record::string{ Derived::table_name } + "\"",
                attributes_to_condition_string(attrs...),
                active_record::string{ "" }
        };
    }
        
}