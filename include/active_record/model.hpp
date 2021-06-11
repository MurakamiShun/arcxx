#pragma once
#include <type_traits>
#include <array>
#include <utility>
#include <vector>
#include "query.hpp"
#include "attribute.hpp"

namespace active_record {
    template<typename T>
    concept WhereArgs = Attribute<T> || std::same_as<T, query_condition>;

    template<typename Derived>
    class model {
    protected:
        class relation;
    private:
        struct has_table_name_impl {
        private:
            template<typename S>
            static decltype(S::table_name, std::true_type()) check(S);
            static std::false_type check(...);
        public:
            static constexpr bool value = std::invoke_result_t<decltype(&has_table_name_impl::check), Derived>::value;
        };
        struct has_attributes_impl {
        private:
            template<typename S>
            static decltype(std::declval<S>().attributes, std::true_type{}) check(S);
            static std::false_type check(...);
        public:
            static constexpr bool value = std::invoke_result_t<decltype(&has_attributes_impl::check), Derived>::value;
        };
        // column_names
        template<std::size_t Last>
        static constexpr std::array<active_record::string_view, 1> column_names_aux(std::index_sequence<Last>) noexcept {
            return { std::get<Last>(Derived{}.attributes).column_name };
        }
        template<std::size_t Head, std::size_t... Tail>
        static constexpr std::array<active_record::string_view, 1 + sizeof...(Tail)> column_names_aux(std::index_sequence<Head, Tail...>) noexcept {
            std::array<active_record::string_view, 1 + sizeof...(Tail)> head_name = { std::get<Head>(Derived{}.attributes).column_name };
            const std::array<active_record::string_view, sizeof...(Tail)> tail_names = column_names_aux(std::index_sequence<Tail...>{});
            std::copy(tail_names.begin(), tail_names.end(), head_name.begin() + 1);
            return head_name;
        }

        // values_to_string
        template<std::size_t Last>
        constexpr std::array<active_record::string, 1> to_strings_aux(std::index_sequence<Last>) const {
            return { std::get<Last>(dynamic_cast<const Derived*>(this)->attributes).to_string() };
        }
        template<std::size_t Head, std::size_t... Tail>
        constexpr std::array<active_record::string, 1 + sizeof...(Tail)> to_strings_aux(std::index_sequence<Head, Tail...>) const {
            std::array<active_record::string, 1 + sizeof...(Tail)> head_name = { std::get<Head>(dynamic_cast<const Derived*>(this)->attributes).to_string() };
            const std::array<active_record::string, sizeof...(Tail)> tail_names = to_strings_aux(std::index_sequence<Tail...>{});
            std::copy(tail_names.begin(), tail_names.end(), head_name.begin() + 1);
            return head_name;
        }
    public:
        static constexpr bool has_table_name = has_table_name_impl::value;
        static constexpr bool has_attributes = has_attributes_impl::value;
        static constexpr auto column_names() noexcept {
            return column_names_aux(std::make_index_sequence<std::tuple_size_v<decltype(Derived{}.attributes)>>{});
        }

        constexpr auto to_strings() const {
            return to_strings_aux(std::make_index_sequence<std::tuple_size_v<decltype(Derived{}.attributes)>>{});
        }

        constexpr virtual ~model() {}

        template<Container C>
        static constexpr query_relation<bool> insert(const C&);        
        template<std::same_as<Derived>... Models>
        static constexpr query_relation<bool> insert(const Models&...);

        static constexpr query_relation<std::vector<Derived>> all();

        template<Attribute... Attrs>
        static constexpr query_relation<std::vector<std::tuple<Attrs...>>> select(const Attrs...);

        template<Attribute Attr>
        static constexpr query_relation<std::vector<Attr>> pluck(const Attr);
        
        template<WhereArgs... Attrs>
        static constexpr query_relation<std::vector<Derived>> where(const Attrs...);
    };

    template<typename T>
    concept Model = std::derived_from<T, model<T>>;
}