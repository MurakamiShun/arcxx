#pragma once
#include "columns.hpp"
#include "utils.hpp"

namespace active_record{
    enum class query_operation{
        unspecified,
        select,
        insert,
        remove,
        update,
        where
    };

    template<typename Result>
    requires std::derived_from<Result, database::table<Result>>
    class database::query_relation {
    private:
    public:
        database::string query;
        database::string to_sql();
        query_relation(const string){}

        query_relation<bool> update();
        query_relation<bool> remove();

        query_relation<bool> exist();
        template<typename... Columns>
        static query_relation<std::tuple<Columns...>> select();
        template<typename... Columns>
        static query_relation<std::tuple<Columns...>> select([[maybe_unused]] Colums...);
        template<typename Column>
        static query_relation<Column> pluck();
        template<typename Column>
        static query_relation<Column> pluck([[maybe_unused]]Column);
    };

    template<typename Result>
    requires Container<Result> && std::derived_from<Result::value_type, database::table<Result::value_type>>
    class database::query_relation {
    private:
    public:
        database::string query;
        database::string to_sql();

        query_relation<bool> update_all();
        query_relation<bool> remove_all();

        query_relation<bool> exist();
        query_relation<std::size_t> count();
        template<typename Column>
        query_relation<Column> sum();
        template<typename... Columns>
        static query_relation<std::vector<std::tuple<Columns...>>> select();
        template<typename Column>
        static query_relation<std::vector<Column>> pluck();
        template<typename Relation>
        query_relation<Result> join();
        query_relation<Result> order();
        query_relation<Result> limit(std::size_t);
        query_relation<Result> or();
        query_relation<Result> where();
        query_relation<Result::value_type> find();
    };

    template<typename Result>
    class database::query_relation {
    private:
    public:
    };
}