#pragma once
#include <array>
#include <utility>
#include <vector>
#include <algorithm>
#include "query.hpp"
#include "attribute.hpp"

namespace active_record {
    template<typename T>
    concept WhereArgs = Attribute<T> || std::same_as<T, query_condition>;

    template<typename Derived>
    class model {
    private:
        struct has_table_name_impl {
        private:
            template<typename S>
            static decltype(S::table_name, std::true_type()) check(S);
            static std::false_type check(...);
        public:
            static constexpr bool value = decltype(check(std::declval<Derived>()))::value;
        };
        struct has_attributes_impl {
        private:
            template<typename S>
            static decltype(std::declval<S>().attributes, std::true_type{}) check(S);
            static std::false_type check(...);
        public:
            static constexpr bool value = decltype(check(std::declval<Derived>()))::value;
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
        constexpr std::array<active_record::string, 1> get_attribute_strings_aux(std::index_sequence<Last>) const {
            return { std::get<Last>(dynamic_cast<const Derived*>(this)->attributes).to_string() };
        }
        template<std::size_t Head, std::size_t... Tail>
        constexpr std::array<active_record::string, 1 + sizeof...(Tail)> get_attribute_strings_aux(std::index_sequence<Head, Tail...>) const {
            std::array<active_record::string, 1 + sizeof...(Tail)> head_name = { std::get<Head>(dynamic_cast<const Derived*>(this)->attributes).to_string() };
            const std::array<active_record::string, sizeof...(Tail)> tail_names = get_attribute_strings_aux(std::index_sequence<Tail...>{});
            std::copy(tail_names.begin(), tail_names.end(), head_name.begin() + 1);
            return head_name;
        }

        template<typename Adaptor, Attribute Attr>
        static active_record::string get_column_definitions([[maybe_unused]]std::tuple<Attr>) {
            return Adaptor::template column_definition<Attr>();
        }

        template<typename Adaptor, Attribute Head, Attribute... Tail>
        static active_record::string get_column_definitions([[maybe_unused]]std::tuple<Head, Tail...>) {
            return get_column_definitions<Adaptor>(std::tuple<Head>{}) + ","
                + get_column_definitions<Adaptor>(std::tuple<Tail...>{});
        }

        // remove reference
        template<typename Adaptor, Attribute... Attrs>
        static active_record::string get_column_definitions([[maybe_unused]]std::tuple<Attrs&...>) {
            return get_column_definitions<Adaptor>(std::tuple<Attrs...>{});
        }
    public:
        static constexpr bool has_table_name = has_table_name_impl::value;
        static constexpr bool has_attributes = has_attributes_impl::value;
        static constexpr auto column_names() noexcept {
            return column_names_aux(std::make_index_sequence<std::tuple_size_v<decltype(Derived{}.attributes)>>{});
        }

        constexpr auto get_attribute_strings() const {
            return get_attribute_strings_aux(std::make_index_sequence<std::tuple_size_v<decltype(Derived{}.attributes)>>{});
        }

        template<typename Adaptor>
        static active_record::string table_definition(){
            return active_record::string{ "CREATE TABLE IF NOT EXISTS " } + active_record::string{ Derived::table_name } + "("
                + get_column_definitions<Adaptor>(Derived{}.attributes)
                + ");";
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

        constexpr query_relation<bool> destroy();
        constexpr query_relation<bool> save();
    };

    template<typename T>
    concept Model = std::derived_from<T, model<T>>;
}