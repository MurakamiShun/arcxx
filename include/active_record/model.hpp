#pragma once
#include <type_traits>
#include "query.hpp"

namespace active_record{
    template<typename Derived>
    class model {
    protected:
        class relation;
    private:
        struct has_table_name_impl {
        private:
            template<typename S>
            static decltype(S::table_name, std::true_type()) check(S) {}
            static std::false_type check(...) {}
        public:
            static constexpr bool value = decltype(check(std::declval<Derived>()))::value;
        };
        struct has_columns_impl {
        private:
            template<typename S>
            static decltype(std::declval<S::columns>(), std::true_type()) check(S) {}
            static std::false_type check(...) {}
        public:
            static constexpr bool value = decltype(check(std::declval<Derived>()))::value;
        };
        //static_assert(has_table_name<Derived>::value, "Model doesn't have table_name. Declare like \"constexpr static auto table_name = \"hoge_table\"\"");
        //static_assert(has_columns<Derived>::value, "Model doesn't have attributes. Declare like \"using attributes = std::tuple<Attributes...>");
    public:
        static constexpr bool has_table_name = has_table_name_impl::value;
        static constexpr bool has_columns = has_columns_impl::value;

        static query_relation<bool> insert(const Derived&);
        template<Container T>
        static query_relation<bool> bulk_insert(const T&);

        template<typename... Columns>
        static constexpr query_relation<std::vector<Derived>> where([[maybe_unused]] Columns...);
        template<typename... Columns>
        static constexpr query_relation<std::vector<std::tuple<Columns...>>> select([[maybe_unused]] Columns...);
        template<typename Column>
        static constexpr query_relation<std::vector<Column>> pluck([[maybe_unused]] Column);
        static constexpr query_relation<std::vector<Derived>> all();
    };
}