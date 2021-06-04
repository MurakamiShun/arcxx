#pragma once
#include "database,hpp"

namespace active_record{
    template<typename Derived>
    class database::table {
    protected:
        class column;
        class relation;
        friend column;
    private:
        template<typename T>
        struct has_table_name {
        private:
            template<typename S>
            static decltype(S::table_name, std::true_type()) check(S);
            static std::false_type check(...);
        public:
            static constexpr bool value = decltype(check(std::declval<T>()))::value;
        };
        template<typename T>
        struct has_columns {
        private:
            template<typename S>
            static decltype(std::declval<S::columns>(), std::true_type()) check(S);
            static std::false_type check(...);
        public:
            static constexpr bool value = decltype(check(std::declval<T>()))::value;
        };
        static_assert(has_table_name<Derived>::value, "table doesn't have table_name. Declare like \"constexpr static auto table_name = \"hoge_table\"\"");
        static_assert(has_columns<Derived>::value, "table doesn't have columns. Declare like \"using columns = std::tuple<Columns...>");
    public:
        static query_relation<bool> create(const Derived&);
        static query_relation<bool> insert(const Derived&);

        static query_relation<std::vector<Derived>> where() {

        }
        template<typename... Args>
        static query_relation<Derived> find(database::string_view, Args...);
        template<typename... Columns>
        static query_relation<Derived> find(Columns...);
        template<typename... Columns>
        static query_relation<std::vector<std::tuple<Columns...>>> select();
        template<typename Column>
        static query_relation<std::vector<Column>> pluck();
        static query_relation<std::vector<Derived>> all();
    };
}