#pragma once
#include "model.hpp"
#include "query.hpp"
#include "attribute.hpp"
#include "utils.hpp"

namespace active_record{
    template<Container Result>
    requires std::derived_from<Result::value_type, model<Result::value_type>>
    class query_relation<Result> {
    private:
        const enum class query_operation {
            unspecified,
            select,
            insert,
            remove,
            update,
            where
        } op;
    public:
        active_record::string to_sql();
        
        query_relation<bool> update();
        query_relation<bool> remove();

        query_relation<bool> exist();
        //template<typename... Columns>
        //static query_relation<std::tuple<Columns...>> select([[maybe_unused]]Colums...);
        template<typename Column>
        static query_relation<Column> pluck([[maybe_unused]]Column);
    };

    template<Tuple Result>
    requires std::derived_from<Result::value_type, model<Result::value_type>>
    class query_relation<Result> {
    private:
    public:
        active_record::string query;
        active_record::string to_sql();

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
        query_relation<Result> join([[maybe_unused]]Relation);
        //template<typename Columns>
        //query_relation<Result> order([[maybe_unused]] Column);
        query_relation<Result> limit(std::size_t);
        query_relation<Result> where();
    };

    template<>
    class query_relation<bool> {
    private:
    public:
    };
}